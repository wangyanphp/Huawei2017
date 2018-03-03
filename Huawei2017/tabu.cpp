#include"tabu.h"
#include<random>
#include<ctime>
#include<algorithm>
#include<set>
#include"stack.h"
#include<iostream>
#include"curtime.h"
#include"canceledge.h"
#include<fstream>
void Tabu::setTableParamaters(int init_size)
{
	table_.resize(init_size);

}
void Tabu::setInitSolution(const std::string &x, int cost, int numserve)
{
	curX_ = x;
	curCost_ = cost;

	bestX_ = x;
	bestCost_ = cost;

	curNumServes_ = numserve;
}

void Tabu::localSearch()
{
	//std::string filename = "E:\\result" + std::to_string(ITER1) + ".txt";
	//std::ofstream out(filename);
	start_time = getCurTime();
	while(!shouldStop())
	{
		bestNeighbor();
		//此时一步迭代完成
		if (table_.contain(curX_))
			table_.erase(curX_);
		table_.add(curX_);
		curNumServes_ = 0;
		for (auto c : curX_)
		{
			if (c == '1')++curNumServes_;
		}
		++iters;
		curG_.location();
		std::cout << "cost: " << curCost_ << std::endl;
		if (iters_from_curbest > 20)
		{
			std::cout << "iters"<<iters<<": " <<bestCost_ << std::endl;
			iters_from_curbest = 0;
		}
	}
	
}



void Tabu::bestNeighbor()
{
	//两个中间值：next_x1：用于保存不在禁忌表中的最优邻居，next_x2用于保存在禁忌表中的最优邻居
	std::string nextX1; int costX1 = INT_MAX; std::list<Edge> edges1; int numserve1 = curNumServes_;
	std::string nextX2; int costX2 = INT_MAX; std::list<Edge> edges2; int numserve2 = curNumServes_;

	std::set<int> neigbors;
	int num = std::min(neighbors_num_1, int(curX_.size()));
	
	while (neigbors.size() < num)
	{
		int v = getRandInt(curX_.size() - 1);
		if (neigbors.find(v) == neigbors.end())
			neigbors.insert(v);
	}
	for (int i : neigbors)
	{
		int cost = 0;
		int numserve = curNumServes_;
		std::string x = curX_;
		std::list<Edge> edges;

		if (curX_[i] == '0')
		{
			edges = minCost(i, true, cost);
			x[i] = '1';
			++numserve;
		}else {
			//如果不成功，那么cost=INT_MAX, edges=empty,相当于没有
			edges = minCost(i, false, cost);
			x[i] = '0';
			--numserve;
		}

		//是否在禁忌表中
		if (table_.contain(x))
		{
			if (cost < costX2)
			{
				nextX2 = x;
				costX2 = cost;
				edges2 = std::move(edges);
				numserve2 = numserve;
			}
		}else {
			if (cost < costX1)
			{
				nextX1 = x;
				costX1 = cost;
				edges1 = std::move(edges);
				numserve1 = numserve;
			}
		}
	}

	std::string bestX;
	int bestCost;
	std::list<Edge> bestEdges;
	if (costX1 <= costX2)
	{
		bestX = nextX1;
		bestCost = costX1;
		bestEdges = edges1;
	}
	else {
		bestX = nextX2;
		bestCost = costX2;
		bestEdges = edges2;
	}


	if (bestCost < bestCost_)
	{
		bestCost_ = bestCost;
		bestX_ = bestX;
		curX_ = bestX;
		curCost_ = bestCost;

		Graph tmpG(curG_.V());
		curEdges_ = std::move(bestEdges);
		for (auto it = curEdges_.begin(); it != curEdges_.end(); ++it)
			tmpG.addEdge(&(*it));
		curG_ = tmpG;

		bestEdges_ = curEdges_;


		iters_to_curbest = (iters_from_curbest + 1);
		iters_from_curbest = 0;
		return;
	}

	//如果两个解都不优于历史解
	if (nextX1.empty())//如果所有的解都被禁忌了
	{
		curX_ = nextX2;
		curCost_ = costX2;
		curNumServes_ = numserve2;

		Graph tmpG(curG_.V());
		curEdges_ = std::move(edges2);
		for (auto it = curEdges_.begin(); it != curEdges_.end(); ++it)
			tmpG.addEdge(&(*it));
		curG_ = tmpG;

		++iters_from_curbest;
		return;
	}

	curX_ = nextX1;
	curCost_ = costX1;
	curNumServes_ = numserve1;
	Graph tmpG(curG_.V());
	curEdges_ = std::move(edges1);
	for (auto it = curEdges_.begin(); it != curEdges_.end(); ++it)
		tmpG.addEdge(&(*it));
	curG_ = tmpG;
	
	++iters_from_curbest;
}
std::list<Edge>  Tabu::minCost(int i, bool add, int &cost)
{
	//因为要改变流，所有首先需要复制整个图
	std::list<Edge> tmpEdges = curEdges_;
	Graph tmpG(curG_.V());
	auto removeIt = tmpEdges.begin();//找出s->i，用于删除
	for (auto it = tmpEdges.begin(); it != tmpEdges.end(); ++it)
	{
		tmpG.addEdge(&(*it));

		int v = it->either();
		if (v == s_ && it->other(v) == i)
			removeIt = it;
	}

	cost = curNumServes_*SERVECOST;
	if (add)
	{//如果增加一条边，那么只需要在原图中增加一条边，然后求最小费用最大流即可
		cost += SERVECOST;
		tmpEdges.push_back(Edge(s_, i, FLOWS, 0));
		tmpG.addEdge(&tmpEdges.back());
		NetworkSimplex mc(tmpG, s_, t_);
		//CancelingCycle mc(tmpG, s_, t_);
		cost += tmpG.cost();
	}
	else {
		cost -= SERVECOST;
		//要删除e，需要将e上的流推送至0
		Edge* e = &(*removeIt);
		
		CancelEdge ce(tmpG, e);
		bool valid = ce.cancelEdge();
		if (!valid)
		{
			cost = INT_MAX;
			return std::list<Edge>();
		}
		tmpG.removeEdge(e);
		tmpEdges.erase(removeIt);

		NetworkSimplex mc(tmpG, s_, t_);
		//CancelingCycle mc(tmpG, s_, t_);
		cost += tmpG.cost();
	}

	return tmpEdges;
}


bool Tabu::shouldStop()
{
	//迭代次数，时间
	auto cur_time= getCurTime();
	int c = cur_time - start_time;
	if ((cur_time - start_time) > times_)
		return true;
	if (iters > (ITER1+ITER2)) return true;
	

	return false;
}
//----------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------//