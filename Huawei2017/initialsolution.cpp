#include"initialsolution.h"
#include<iostream>

void InitialSolution::initial()
{
	std::list<Edge>::iterator begIt;
	x_ = std::string(V_, '0');
	numserve_ = 0;
	std::set<int> set;
	while (g_.flow(s_) != maxflows_)
	{
		int v = getRandInt(V_ - 1);
		if (set.find(v) != set.end()) continue;
		set.insert(v);
		edges_.push_back(Edge(s_, v, maxflows_, 0));

		if (set.size() == 1) begIt = --edges_.end();

		g_.addEdge(&edges_.back());
		EdmondsKarp(g_, s_, t_);
		x_[v] = '1';
		++numserve_;
	}
	while (begIt != edges_.end())
	{
		Edge* e = &(*begIt);
		int v = e->other(s_);
		if (begIt->flow() == 0)
		{
			g_.removeEdge(e);
			begIt = edges_.erase(begIt);
			--numserve_;
			x_[v] = '0';
		}
		else {
			++begIt;
		}
	}
	NetworkSimplex ns(g_, s_, t_);
	cost_ = g_.cost();
	

	////对于最后numserve条边，尝试消除
	//while (begIt != edges_.end())
	//{
	//	Edge* e = &(*begIt);
	//	int v = e->other(s);
	//	CancelEdge ce(g_, e);
	//	if (ce.cancelEdge())
	//	{
	//		g_.removeEdge(e);
	//		begIt = edges_.erase(begIt);
	//		--numserve;
	//		x[v] = '0';
	//	}
	//	else {
	//		++begIt;
	//	}
	//}
	//std::cout << "numserve: " << numserve << std::endl;
}