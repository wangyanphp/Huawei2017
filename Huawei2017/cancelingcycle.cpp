#include"cancelingcycle.h"
#include"sp.h"
#include<algorithm>
#include<list>
#include"stack.h"
#include<iostream>

CancelingCycle::CancelingCycle(Graph &g, int s, int t)
	:g_(g), s_(s), t_(t)
{
	while (findNegativeCycle())
	{
		augment();
	}
		
}
bool CancelingCycle::findNegativeCycle()
{
	SPFA bf(g_, s_);
	if (bf.hasNegativeCycle())
	{
		cycle_ = bf.negativeCycle();
		cycle_beg_ = bf.cycleBegin();
		return true;
	}
	return false;
}
void CancelingCycle::augment()
{
	//确定增广值
	int d = INT_MAX;
	int x = cycle_beg_;
	for (auto it = cycle_.begin(); it != cycle_.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->capTo(w));
		x = w;
	}
	x = cycle_beg_;
	for (auto it = cycle_.begin(); it != cycle_.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//------------------------------最小平均值圈，需要定制SPFA-----------------------------------------------------------

class SPFA_CancelingCycle
{
private:
	std::vector<double> distTo_;
	std::vector<bool> onQ_;
	std::list<int> queue_;
	std::vector<int> num;//每个结点的入队次数

	Graph &g_;
	double mu_;

	//用于寻找zero-cycle
	std::vector<bool> marked_;
	std::vector<Edge*> edgeTo_;

	Stack<Edge*> cycle_;
	int cycle_beg_;

public:
	SPFA_CancelingCycle(Graph &g, int s, double mu);
	bool hasNegativeCycle() const { return !cycle_.isEmpty(); }
	std::vector<Edge*> cycle() const;
	int cycleBegin() const { return cycle_beg_; }
private:
	void findZeroCycle();
	void dfs(std::vector<std::list<Edge*>> &g, int v, int u);
	bool hasCycle() const { return !cycle_.isEmpty(); }
	void slf();
	void lll();
};
SPFA_CancelingCycle::SPFA_CancelingCycle(Graph &g, int s, double mu)
	:distTo_(g.V(), INT_MAX), edgeTo_(g.V(), nullptr), onQ_(g.V(), false), num(g.V(), 0),marked_(g.V(),false),g_(g),mu_(mu)
{
	distTo_[s] = 0;
	queue_.push_back(s);
	onQ_[s] = true;
	num[s] = 1;
	while (!queue_.empty())
	{
		int v = queue_.front(); queue_.pop_front();
		onQ_[v] = false;
		for (auto it = g.begin(v); it != g.end(v); ++it)
		{
			auto e = *it;
			int w = e->other(v);
			//v->w是否在残存网络中？
			if (e->capTo(w) == 0) continue;
			if (distTo_[w] > distTo_[v] + (e->costTo(w)-mu))
			{
				distTo_[w] = distTo_[v] + (e->costTo(w)-mu);
				if (!onQ_[w])
				{
					queue_.push_back(w);
					onQ_[w] = true;
					++num[w];
					if (num[w] > g.V())
					{
						findZeroCycle();
						return;
					}
					slf();
					lll();
				}
			}
		}
	}
	findZeroCycle();
}
void SPFA_CancelingCycle::findZeroCycle()
{
	//onQ_用作onStack_
	onQ_ = std::vector<bool>(onQ_.size(), false);

	int V = edgeTo_.size();
	std::vector<std::list<Edge*>> adj(g_.V());
	for (int v = 0; v < V; ++v)
	{
		for (auto it = g_.begin(v); it != g_.end(v); ++it)
		{
			auto e = *it;
			int w = e->other(v);
			//访问v->w-----v->w是否存在
			if (e->capTo(w) <= 0) continue;
			//只考虑0边
			if (e->costTo(w) - mu_ + distTo_[v] - distTo_[w] != 0) continue;
			adj[v].push_back(e);//只考虑v->w方向上
		}
	}
	for (int s = 0; s < adj.size(); ++s)
	{
		if (!hasCycle()&&!marked_[s]) dfs(adj, s, s);
	}
}
std::vector<Edge*> SPFA_CancelingCycle::cycle() const
{
	std::vector<Edge*> c;
	for (auto x : cycle_)
		c.push_back(x);
	return c;
}
void SPFA_CancelingCycle::dfs(std::vector<std::list<Edge*>> &adj, int v, int u)
{
	onQ_[v] = true;
	marked_[v] = true;

	for (auto it = adj[v].begin(); it != adj[v].end(); ++it)
	{

		auto e = *it;
		int w = e->other(v);

		if (hasCycle()) return;
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(adj, w, v);
		}
		else if (onQ_[w] && w != u)
		{
			//从w开始
			int x = w;
			auto f = e;
			while (f->other(x) != w)
			{
				cycle_.push(f);
				x = f->other(x);
				f = edgeTo_[x];

			}
			cycle_.push(f);
			cycle_beg_ = w;
			return;
		}
	}
	onQ_[v] = false;
}
void SPFA_CancelingCycle::slf()
{//smaller label first策略
	if (distTo_[queue_.back()] < distTo_[queue_.front()])
	{
		int v = queue_.back(); queue_.pop_back();
		queue_.push_front(v);
	}
}
void SPFA_CancelingCycle::lll()
{
	double aver = 0;
	for (auto v : queue_)
		aver += distTo_[v];
	aver /= queue_.size();

	while (distTo_[queue_.front()] > aver+0.001)
	{
		int v = queue_.front(); queue_.pop_front();
		queue_.push_back(v);
	}
}

//----------------------------------------------------------------------------------------------------------------
void CancelingCycle::calMu()
{
	std::vector<std::vector<int>> F(g_.V()+1, std::vector<int>(g_.V(), INT_MAX));
	//计算F[k,v]
	F[0][s_] = 0;
	for (int k = 1; k <= g_.V(); ++k)
	{
		for (int v = 0; v < g_.V(); ++v)
		{
			int f = INT_MAX;
			for (auto it = g_.begin(v); it != g_.end(v); ++it)
			{
				Edge* e = *it;
				int u = e->other(v);
				if (e->capTo(v) == 0)continue;
				if (F[k - 1][u] == INT_MAX) continue;
				if (F[k - 1][u] + e->costTo(v) < f)
					f = F[k - 1][u] + e->costTo(v);
			}
			F[k][v] = f;
		}
	}

	double mu = INT_MAX;

	for (int v = 0; v < g_.V(); ++v)
	{
		double x = INT_MIN;
		for (int k = 0; k < g_.V(); ++k)
		{
			if (F[k][v] == INT_MAX || F[g_.V()][v] == INT_MAX)
			{
				//x = INT_MAX;
				continue;
			}

			double y = (double)((F[g_.V()][v] - F[k][v])) / (g_.V() - k);
			if (y > x) x = y;
		}
		if (x == INT_MIN) continue;

		if (x < mu) mu = x;
	}
	mu_ = mu;
}
bool CancelingCycle::findNegativeCycle2()
{
	calMu();
	SPFA_CancelingCycle bf(g_, s_, mu_);
	if (bf.hasNegativeCycle())
	{
		cycle_ = bf.cycle();
		cycle_beg_ = bf.cycleBegin();
		return true;
	}
	return false;
}