#include"canceledge.h"
#include<limits.h>
void FindPath::dfs(Graph &g, int v, int t)
{
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{
		Edge* e = *it;
		int w = e->other(v);
		if (e->capTo(w) == 0) continue;

		if (hasPath()) return;
		else if (w == t)
		{//找到路径
			edgeTo_[t] = e;

			Stack<Edge*> stack;
			int x = t;
			auto f = edgeTo_[x];
			while (f != nullptr)
			{
				stack.push(f);
				x = f->other(x);
				f = edgeTo_[x];
			}

			for (auto x : stack)
				path_.push_back(x);
			return;
		}
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w, t);
		}
	}
}

void CancelEdge::augment(Graph &g, std::vector<Edge*>& cycle, int cycleBeg)
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

bool CancelEdge::decreaseFlow(Graph &g, Edge* e)
{//将e中的流变为0，如果不能则返回false
	int s = e->either();
	int v = e->other(s);

	FindPath sp(g, s, v);
	while (e->capTo(s) != 0 && sp.hasPath())
	{
		auto path = sp.path();
		path.push_back(e);
		augment(g, path, s);
		sp = FindPath(g, s, v);
	}

	if (e->capTo(s) != 0) return false;
	return true;
}