#ifndef CancelingCycle_H_
#define CancelingCycle_H_
#include"graph.h"
#include<limits.h>

class CancelingCycle
{
private:
	int s_, t_;//源节点和汇点
	Graph& g_;

	double mu_;

	std::vector<Edge*> cycle_;
	int cycle_beg_;

public:
	CancelingCycle(Graph &g, int s, int t);
private:
	bool findNegativeCycle();
	bool findNegativeCycle2();
	void augment();
	void calMu();
};

#endif
