#include"sp.h"
#include<list>
//--------------------------------------------------------------------------------------------------
SPFA::SPFA(Graph &g, int s)
	:distTo_(g.V(), INT_MAX), edgeTo_(g.V(), nullptr), onQ_(g.V(), false), num(g.V(), 0), marked_(g.V(), false), g_(g)
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
			if (distTo_[w] > distTo_[v] + e->costTo(w))
			{
				distTo_[w] = distTo_[v] + e->costTo(w);
				edgeTo_[w] = e;
				if (!onQ_[w])
				{
					queue_.push_back(w);
					onQ_[w] = true;
					++num[w];
					if (num[w] > g.V())
					{
						findNegativeCycle();
						return;
					}
					slf();
					lll();
				}
			}
		}
	}
}

std::vector<Edge*> SPFA::pathTo(int v) const
{
	std::vector<Edge*> path;
	Stack<Edge*> stack;
	if (!hasPathTo(v)) return path;
	//从v开始
	int x = v;
	auto f = edgeTo_[x];
	while (f != nullptr)
	{
		stack.push(f);
		x = f->other(x);
		f = edgeTo_[x];
	}

	for (auto x : stack)
		path.push_back(x);
	return path;
}
std::vector<Edge*> SPFA::negativeCycle()
{
	std::vector<Edge*> c;
	for (auto x : cycle_)
	{
		c.push_back(x);
	}
		
	return c;
}


void SPFA::findNegativeCycle()
{
	//onQ_用作onStack_
	onQ_ = std::vector<bool>(onQ_.size(), false);

	int V = edgeTo_.size();
	std::vector<std::list<Edge*>> adj(g_.V());
	for (int v = 0; v < edgeTo_.size(); ++v)
	{
		Edge* e = edgeTo_[v];
		if (e == nullptr) continue;
		int u = e->other(v);
		if (e->capTo(v) == 0) continue;
		adj[u].push_back(e);
	}

	for (int s = 0; s < adj.size(); ++s)
	{
		if (!hasCycle()&&!marked_[s]) dfs(adj, s, s);
	}
}

void SPFA::dfs(std::vector<std::list<Edge*>> &adj, int v, int u)
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

void SPFA::slf()
{//smaller label first策略
	if (distTo_[queue_.back()] < distTo_[queue_.front()])
	{
		int v = queue_.back(); queue_.pop_back();
		queue_.push_front(v);
	}
}

void SPFA::lll()
{
	double aver = 0;
	for (auto v : queue_)
		aver += distTo_[v];
	aver /= queue_.size();

	while (distTo_[queue_.front()] > aver)
	{
		int v = queue_.front(); queue_.pop_front();
		queue_.push_back(v);
	}
}