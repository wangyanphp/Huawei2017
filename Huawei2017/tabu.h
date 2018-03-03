#ifndef TABU_H_
#define TABU_H_
#include"networkflow.h"
#include"tabutable.h"
#include<string>
#include<set>
#include<random>
#include<limits.h>

class Tabu
{
private:
	TabuTable table_;//禁忌表

private:
	int ITER1;//迭代次数
	int ITER2;
	int iters_to_curbest = 0;//从上次最优解到现在的最优解总共迭代了多少次
	int iters_from_curbest = 0;//从现在最优解到如今总迭代次数
	int times_;//整个程序至多执行times毫秒
	unsigned long start_time;
	int iters = 0;
	int neighbors_num_1;
	int neighbors_num_2;

private:
	int s_, t_;
	int SERVECOST;
	int FLOWS;

private:
	Graph curG_;//当前图
	std::list<Edge> curEdges_;//因为需要copy整个图，所以需要保存
	int curNumServes_;//当前服务器个数
	std::string curX_;//当前解
	int curCost_;//当前解对应的费用
	std::string bestX_;//最优解
	int bestCost_;//最优解对应的费用
	std::list<Edge> bestEdges_;

public:
	Tabu(int V,const std::list<Edge> &edges, int s, int t, int maxflows, int servecost)
		:curG_(V), curEdges_(edges), bestEdges_(edges), s_(s), t_(t), FLOWS(maxflows), SERVECOST(servecost)
	{
		for (auto it = curEdges_.begin(); it != curEdges_.end(); ++it)
		{
			curG_.addEdge(&(*it));
		}
	}

	void setOutTime(int ms) { times_ = ms; }

	//设置超参数
	void setTableParamaters(int init_size);
	void setIteration(int n1, int n2=0)
	{
		ITER1 = n1; ITER2 = n2;
	}
	void setNeighborParamter(int n1, int n2=0) { neighbors_num_1 = n1; neighbors_num_2 = n2; }

	void setInitSolution(const std::string &x, int cost, int numserve);
	void localSearch();
	//返回结果
	std::list<Edge> localBestEdges() const { return bestEdges_; }
	std::string localBestLocation() const { return bestX_; }
	int localBestCost() const { return bestCost_; }
	

private:
	bool shouldStop();
	void bestNeighbor();
	std::list<Edge>  minCost(int i, bool add, int &cost);
};

#endif
