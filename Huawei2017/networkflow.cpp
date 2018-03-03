#include"networkflow.h"
#include"directedcycle.h"
#include<algorithm>
#include<iostream>
//-----------------------------------------------------network simplex------------------------------------------------------
/**
 *1.获得生成树结构
 *2.获得相应的流（有时候这两步混合在一起）
 *3.获得对应的phi函数
 *while存在合格边：
 *   将合格边加入树结构，同时替出一条边，
 *   此时树结构改变，导致phi改变，导致合格边改变......
 *
 *networksimplex函数中，边的应用有三种情况：
 *1.有向边：当计算顶点势和合格边的时候，计算合格边的时候需要考虑是满边还是空边
 *2.无向边：关于树的操作，都是将边当做无向边，更多的是考虑树的父子结构
 *3.双向边：残存网络
 */

//1. 根据可行流构造一个生成树
/**
 *假定已经存在可行流
 *1. free arc是否构成环？如果构成环的话，就增广流，消环
 *2. 生成一棵树：包含所有的free arc
 */
void NetworkSimplex::createTree()
{
	Graph free_g(g_.V());
	for (int v = 0; v < g_.V(); ++v)
	{
		for (auto it = g_.begin(v); it != g_.end(v); ++it)
		{//为了避免重复边，只考虑一个方向上的边
			Edge * e = *it;
			int w = e->other(v);
			if (!e->from(v)) continue;
			if (e->capTo(w) == 0 || e->capTo(v) == 0) continue;
			free_g.addEdge(e);
		}
	}

	//如果freeGraph中存在环，则消环(无向环,所以不能考虑自环）
	DirectedCycle dc(free_g);
	while (dc.hasCycle())
	{
		int cycleBeg = dc.cycleBegin();
		std::vector<Edge*> cycle = dc.cycle();
		augment(free_g, cycle, cycleBeg);
		//更新free_g：将所有满边或空边从free_g中删除
		for (Edge* e : cycle)
		{
			int v = e->either();
			int w = e->other(v);
			if (e->capTo(v) == 0 || e->capTo(w) == 0) free_g.removeEdge(e);
		}
		dc = DirectedCycle(free_g);
	}

	//生成树//选择tree arc set
	std::list<Edge*> edges;
	UF uf(g_.V());
	for (int v = 0; v < g_.V(); ++v)
	{
		for (auto it = g_.begin(v); it != g_.end(v); ++it)
		{//为了避免重复边，只考虑一个方向上的边
			Edge * edge = *it;
			int w = edge->other(v);
			if (!edge->from(v)) continue;
			if (edge->capTo(w) == 0 || edge->capTo(v) == 0) edges.push_back(edge);
			else edges.push_front(edge);
		}
	}
	Graph g(g_.V());
	while (g.E() < g.V() - 1)
	{
		Edge* e = edges.front();edges.pop_front();
		int v = e->either();
		int w = e->other(v);
		if (uf.connected(v, w)) continue;
		uf.unions(v, w);
		g.addEdge(e);
	}

	//使用dfs确定树的结构
	dfs(g, s_);
}

//2.计算顶点势
int NetworkSimplex::phi(int v)
{//假设marked已经初始化
	if (marked_[v] == valid) return phi_[v];
	phi_[v] = phi(parent(v)) - st_[v]->costTo(v);
	marked_[v] = valid;
	return phi_[v];
}

//3.选择合格边
//将满边的reduced cost取反，以便空边和满边统一比较
int NetworkSimplex::reduceCost(Edge* e)
{
	int v = e->either();
	int w = e->other(v);
	int r = e->costTo(w) + phi(w) - phi(v);
	if (e->capTo(w) == 0) return -r;
	return r;
}

void NetworkSimplex::eligible()
{
	int old = 0;
	for (valid = 1; valid != old;)
	{
		old = valid;

		for (int v = 0; v < g_.V(); ++v)
		{
			for (auto it = g_.begin(v); it != g_.end(v); ++it)
			{
				//如果是合格边
				Edge* e = *it;
				int w = e->other(v);
				if (!e->from(v)) continue;
				int r = e->costTo(w) - phi(v) + phi(w);
				if ((e->capTo(w) == 0 && r>0) || (e->capTo(v) == 0 && r < 0))
				{
					Edge* out = augment(e);
					if(e!=out) 
						update(out, e);
					++valid;
					//更新顶点势
					marked_[s_] = valid; phi_[s_] = 0;
				}
			}
		}
	}
}

//4.将合格边加入树，然后找出增广环，增广，同时确定替出边
int NetworkSimplex::lca(int v, int w)
{
	marked_[v] = ++valid; marked_[w] = valid;
	while (v != w)
	{
		if (v != s_) v = st_[v]->other(v);
		if (v != s_ && marked_[v] == valid) return v;
		marked_[v] = valid;
		if (w != s_) w = st_[w]->other(w);
		if (w != s_&&marked_[w] == valid) return w;
		marked_[w] = valid;
	}
	return v;
}

bool NetworkSimplex::onPath(int a, int b, int c)
{
	for (int i = a; i != c; i = parent(i))
		if (i == b) return true;
	return false;
}

void NetworkSimplex::reverse(int u, int x)
{
	Edge* e = st_[u];
	int i = parent(u);
	while (i != x)
	{
		Edge* y = st_[i];
		int tmp_i = parent(i);
		st_[i] = e;
		e = y;
		i = tmp_i;
	}
}

Edge* NetworkSimplex::augment(Edge* x)
{//沿着x的方向进行增广:如果是空边，则沿着正方向，如果是满边，则沿着反方向

	int v = x->either();
	int w = x->other(v);
	if (x->capTo(v) > 0)//满边
		std::swap(v, w);
	int r = lca(v, w);

	//确定增广值
	int d = x->capTo(w);
	for (int u = w; u != r; u = parent(u))
		d = std::min(d, st_[u]->capTo(parent(u)));
	for (int u = v; u != r; u = parent(u))
		d = std::min(d, st_[u]->capTo(u));

	x->addFlowTo(w, d);
	Edge* e = x;
	for (int u = w; u != r; u = parent(u))
	{
		st_[u]->addFlowTo(parent(u), d);
		if (st_[u]->capTo(parent(u)) == 0)
			e = st_[u];
	}
	for (int u = v; u != r; u = parent(u))
	{
		st_[u]->addFlowTo(u, d);
		if (st_[u]->capTo(u) == 0)
			e = st_[u];
	}
	return e;
}

void NetworkSimplex::update(Edge* out, Edge* in)
{
	int v = in->either();
	int w = in->other(v);
	//x是out的父节点，也即:x是y的父节点（在树中）
	int x = out->either();
	int y = out->other(x);
	if (st_[x] == out) std::swap(x, y);

	int r = lca(v, w);
	//查看out在哪条路径上,因为x可能等于r，所以还是查看y更方便一下
	if (onPath(v, y, r))
	{
		reverse(v, x);
		st_[v] = in;
		return;
	}
	if (onPath(w, y, r))
	{
		reverse(w, x);
		st_[w] = in;
		return;
	}
}



//建立树结构需要使用的两个辅助函数：沿着环增广和dfs
void NetworkSimplex::augment(Graph &g, std::vector<Edge*>& cycle, int cycleBeg)
{
	//确定增广值
	int d = INT_MAX;
	int x = cycleBeg;
	for (auto it = cycle.begin(); it != cycle.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->capTo(w));
		x = w;
	}
	x = cycleBeg;
	for (auto it = cycle.begin(); it != cycle.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}

void NetworkSimplex::dfs(Graph& g, int v)
{
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{
		Edge* e = *it;
		int w = e->other(v);
		if (!marked_[w])
		{
			st_[w] = e;
			dfs(g, w);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------

void NetworkSimplex::eligible2()
{
	int NodesPerIter = 50;
	int arcsPerIter = 100;

	int old = 0;
	for (valid = 1; valid != old;)
	{
		old = valid;

		int M = std::ceil(((double)g_.V()) / NodesPerIter);
		int nextNode = 0;
		std::list<Edge*> arcs;
		for (int m = 0; m < M; ++m)
		{
			arcs.clear();
			for (int i = 0; i < NodesPerIter; ++i)
			{//只考虑有向边
				if (nextNode == g_.V()) nextNode = 0;
				int v = nextNode;
				for (auto it = g_.begin(v); it != g_.end(v); ++it)
				{
					Edge* e = *it;
					int w = e->other(v);
					if (!e->from(v)) continue;
					int r = e->costTo(w) - phi(v) + phi(w);
					if (e->capTo(w) == 0 && r>0) arcs.push_back(e);//满边
					else if (e->capTo(v) == 0 && r<0)arcs.push_back(e);//空边
				}
				++nextNode;
			}

			for (int i = 0; i < arcsPerIter; ++i)
			{
				Edge* x = nullptr;
				int min = INT_MAX;
				auto minIt = arcs.begin();

				for (auto it = arcs.begin(); it != arcs.end(); ++it)
				{
					Edge* e = *it;
					if (reduceCost(e) < min)
					{
						x = e;
						minIt = it;
						min = reduceCost(e);
					}
				}

				if (x == nullptr) break;
				if (reduceCost(x) == 0) break;

				Edge* y = augment(x);
				update(y, x);
				arcs.erase(minIt);
				int v = y->either();
				if ((v < nextNode && (nextNode - NodesPerIter) <= v) || (v < nextNode + g_.V() && (nextNode + g_.V() - NodesPerIter) <= v))
					arcs.push_back(y);
				++valid;
				//更新顶点势
				marked_[s_] = valid; phi_[s_] = 0;
			}
		}
	}
}

