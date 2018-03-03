#ifndef INITIALSOLUTION_H_
#define INITIALSOLUTION_H_
#include"head.h"
#include"maxflow.h"
#include"networkflow.h"

class InitialSolution
{
private:
	Graph g_;
	std::list<Edge> edges_;
private:
	std::string x_;
	int numserve_;
	int cost_;

private:
	int V_;
	int maxflows_;
	int s_, t_;


public:
	InitialSolution(const std::list<Edge>& edges, int s, int t, int V, int maxflows) :edges_(edges), g_(0),V_(V),maxflows_(maxflows),s_(s),t_(t)
	{
		Graph gg(V_+2);
		for (auto it = edges_.begin(); it != edges_.end(); ++it)  gg.addEdge(&(*it));
		g_ = gg;
		initial();
	}

public:
	std::string initialX() const { return x_; }
	std::list<Edge> initialEdges() const { return edges_; }
	int initialServeNumber() const { return numserve_; }
	int initialCost(int servecost) const { return cost_+servecost*numserve_; }


private:
	void initial();
};
#endif
