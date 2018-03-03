#ifndef SP_H_
#define SP_H_

#include<vector>
#include"graph.h"
#include<list>
#include"stack.h"

class SPFA
{
private:
	std::vector<int> distTo_;
	std::vector<Edge*> edgeTo_;
	std::vector<bool> onQ_;
	std::list<int> queue_;
	std::vector<int> num;//每个结点的入队次数

	Graph &g_;
	std::vector<bool> marked_;
	Stack<Edge*> cycle_;
	int cycle_beg_;

public:
	SPFA(Graph &g, int s);

	bool hasNegativeCycle() const { return !cycle_.isEmpty(); }
	std::vector<Edge*> negativeCycle();
	int cycleBegin() const { return cycle_beg_; }

	bool hasPathTo(int v) const { return distTo_[v] < INT_MAX; }
	std::vector<Edge*> pathTo(int v) const;


private:
	void findNegativeCycle();
	void dfs(std::vector<std::list<Edge*>> &g, int v, int u);
	bool hasCycle() const { return !cycle_.isEmpty(); }
	void slf();
	void lll();

};

#endif
