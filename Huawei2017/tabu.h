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
	TabuTable table_;//���ɱ�

private:
	int ITER1;//��������
	int ITER2;
	int iters_to_curbest = 0;//���ϴ����Ž⵽���ڵ����Ž��ܹ������˶��ٴ�
	int iters_from_curbest = 0;//���������Ž⵽����ܵ�������
	int times_;//������������ִ��times����
	unsigned long start_time;
	int iters = 0;
	int neighbors_num_1;
	int neighbors_num_2;

private:
	int s_, t_;
	int SERVECOST;
	int FLOWS;

private:
	Graph curG_;//��ǰͼ
	std::list<Edge> curEdges_;//��Ϊ��Ҫcopy����ͼ��������Ҫ����
	int curNumServes_;//��ǰ����������
	std::string curX_;//��ǰ��
	int curCost_;//��ǰ���Ӧ�ķ���
	std::string bestX_;//���Ž�
	int bestCost_;//���Ž��Ӧ�ķ���
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

	//���ó�����
	void setTableParamaters(int init_size);
	void setIteration(int n1, int n2=0)
	{
		ITER1 = n1; ITER2 = n2;
	}
	void setNeighborParamter(int n1, int n2=0) { neighbors_num_1 = n1; neighbors_num_2 = n2; }

	void setInitSolution(const std::string &x, int cost, int numserve);
	void localSearch();
	//���ؽ��
	std::list<Edge> localBestEdges() const { return bestEdges_; }
	std::string localBestLocation() const { return bestX_; }
	int localBestCost() const { return bestCost_; }
	

private:
	bool shouldStop();
	void bestNeighbor();
	std::list<Edge>  minCost(int i, bool add, int &cost);
};

#endif
