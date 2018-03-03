#include"directedcycle.h"

DirectedCycle::DirectedCycle(Graph &g, bool icycle)
	:marked_(g.V(), false), edgeTo_(g.V(), nullptr), onStack_(g.V(), false)
{//当icycle=true时，考虑自环，否则不考虑自环
	if (icycle)
	{
		for (int s = 0; s < g.V(); ++s)
		{
			if (!marked_[s]) dfs(g, s, s);
		}
	}
	else {
		for (int s = 0; s < g.V(); ++s)
			if (!marked_[s]) dfs(g, s);
	}
}

/**
*由于此问题的特殊性：我们保存了v->w，但是可能方向是：v->w或w->v
*所以需要保存环的起始节点：也即环的方向
*/
std::vector<Edge*> DirectedCycle::cycle() const
{
	std::vector<Edge*> c;
	for (auto x : cycle_)
		c.push_back(x);
	return c;
}

//不考虑自环的情况
void DirectedCycle::dfs(Graph &g, int v, int u)
{
	onStack_[v] = true;
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{

		auto e = *it;
		int w = e->other(v);
		//访问v->w-----v->w是否存在
		if (e->capTo(w) <= 0) continue;

		if (hasCycle()) return;
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w, v);
		}
		else if (onStack_[w] && w != u)
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
	onStack_[v] = false;
}

//考虑自环的情况
void DirectedCycle::dfs(Graph &g, int v)
{
	onStack_[v] = true;
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{

		auto e = *it;
		int w = e->other(v);
		//访问v->w-----v->w是否存在
		if (e->capTo(w) <= 0) continue;

		if (hasCycle()) return;
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w);
		}
		else if (onStack_[w])
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
	onStack_[v] = false;
}