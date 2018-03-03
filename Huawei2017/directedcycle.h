#ifndef DIRECTEDCYCLE_H_
#define DIRECTEDCYCLE_H_
#include"head.h"

class  DirectedCycle
{/*用于识别环，在本程序中有两种需求：1.可以识别自环；2.不考虑自环*/
private:
	std::vector<bool> marked_;
	std::vector<Edge*> edgeTo_;
	std::vector<bool> onStack_;
	Stack<Edge*> cycle_;
	int cycle_beg_;

public:
	DirectedCycle(Graph &g, bool icycle = true);
	bool hasCycle() const { return !cycle_.isEmpty(); }
	std::vector<Edge*> cycle() const;
	int cycleBegin() const { return cycle_beg_; }

private:
	void dfs(Graph &g, int v, int u);
	void dfs(Graph &g, int v);
};

#endif
