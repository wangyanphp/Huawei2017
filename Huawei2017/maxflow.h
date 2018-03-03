#ifndef MAXFLOW_H_
#define MAXFLOW_H_
#include"graph.h"
#include<list>
#include<limits.h>

class EdmondsKarp
{
private:
	int s_, t_;//源节点和汇点
	Graph& g_;
	std::vector<Edge*> path;

public:
	EdmondsKarp(Graph &g, int s, int t)
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
