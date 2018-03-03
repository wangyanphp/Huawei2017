#ifndef CANCELEDGE_H_
#define CANCELEDGE_H_
#include"graph.h"
#include<vector>
#include"stack.h"
#include<algorithm>
class FindPath
{
private:
	std::vector<bool> marked_;
	std::vector<Edge*> edgeTo_;
	std::vector<Edge*> path_;

public:
	FindPath(Graph &g, int s, int t)
		:marked_(g.V(), false), edgeTo_(g.V(), nullptr)
	{
		dfs(g, s, t);
	}

	bool hasPath() const { return !path_.empty(); }
	std::vector<Edge*> path() { return path_; }


private:
	void dfs(Graph &g, int v, int t);
};

class CancelEdge
{
private:
	Graph &g_;
	Edge* e_;
public:
	CancelEdge(Graph &g, Edge* e) :g_(g), e_(e) {}
	bool cancelEdge()
	{
		g_.removeEdge(e_);
		bool b = decreaseFlow(g_, e_);
		g_.addEdge(e_);
		return b;
	}
private:
	bool decreaseFlow(Graph &g, Edge* e);
	void augment(Graph &g, std::vector<Edge*>& cycle, int cycleBeg);
};
#endif


