#include"mincostmaxflow.h"
#include"sp.h"
#include<algorithm>

bool MinCostMaxFlow::findShortestPath()
{
	SPFA bf(g_, s_);
	if (bf.hasPathTo(t_))
	{
		path = bf.pathTo(t_);
		return true;
	}
	return false;
}
void MinCostMaxFlow::augment()
{
	//确定增广值
	int d = INT_MAX;
	int x = s_;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->capTo(w));
		x = w;
	}
	x = s_;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}