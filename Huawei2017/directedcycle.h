#ifndef DIRECTEDCYCLE_H_
#define DIRECTEDCYCLE_H_
#include"head.h"

class  DirectedCycle
{/*����ʶ�𻷣��ڱ�����������������1.����ʶ���Ի���2.�������Ի�*/
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
