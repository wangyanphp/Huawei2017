#ifndef MINCOSTMAXFLOW_H_
#define MINCOSTMAXFLOW_H_
#include"graph.h"
#include<limits.h>

class MinCostMaxFlow
{
private:
	int s_, t_;//源节点和汇点
	Graph& g_;

	std::vector<Edge*> path;

public:
	MinCostMaxFlow(Graph &g, int s, int t)
		:g_(g), s_(s), t_(t)
	{
		while (findShortestPath())
			augment();
	}
private:
	bool findShortestPath();
	void augment();
};

#endif

