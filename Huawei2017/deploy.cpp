#include "deploy.h"
#include <stdio.h>
#include<sstream>
#include<algorithm>
#include"networkflow.h"
#include"maxflow.h"
#include"mincostmaxflow.h"
#include"tabu.h"
#include"stack.h"
#include<iostream>
#include<fstream>
#include<map>
#include<random>
#include<ctime>
#include"curtime.h"
#include<limits.h>
#include"head.h"
#include"initialsolution.h"

//全局变量
int V = 0;
int E = 0;
int customs = 0;
int s = 0;
int t = 0;
int servecost = 0;
int maxflows = 0;
std::map<int, int> map;//用于保存网络节点：相连的消费结点
std::list<int> custom_nodes;//用于保存于消费结点相连的网络结点
std::list<Edge> originEdges;
Graph originG(0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SearchPath2
{
private:
	std::vector<bool> marked_;
	std::vector<Edge*> edgeTo_;
	std::vector<Edge*> path_;

public:
	SearchPath2(Graph &g, int s, int t)
		:marked_(g.V(), false), edgeTo_(g.V(), nullptr)
	{
		dfs(g, s, t);
	}

	bool hasPath() const { return !path_.empty(); }
	std::vector<Edge*> path() { return path_; }

private:
	void dfs(Graph &g, int v, int t);
};
void SearchPath2::dfs(Graph &g, int v, int t)
{
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{
		Edge* e = *it;
		int w = e->other(v);
		if (e->from(w)) continue;
		if (e->flow() == 0) continue;

		if (hasPath()) return;
		else if (w == t)
		{//找到路径
			edgeTo_[t] = e;

			Stack<Edge*> stack;
			int x = t;
			auto f = edgeTo_[x];
			while (f != nullptr)
			{
				stack.push(f);
				x = f->other(x);
				f = edgeTo_[x];
			}

			for (auto x : stack)
				path_.push_back(x);
			return;
		}
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w, t);
		}
	}
}

class SolutionByTabu
{
private:
	std::list<Edge> bestEdges;
	std::string bestX;
	int bestCost;

	int ITER=3;
	int iters = 0;//迭代次数
	double aver_time;//迭代一次平均耗时

	std::vector<Edge*> path;

public:
	SolutionByTabu() {search();}
	std::string output();


private:
	void initial();
	void step();
	void search();


	
	void realPath(std::vector<Edge*>& path, std::string &ret);
	int augment();
};



void SolutionByTabu::initial()
{
	std::list<Edge> tmpEdges = originEdges;
	Graph tmpG(originG.V());
	for (auto it = tmpEdges.begin(); it != tmpEdges.end(); ++it)
		tmpG.addEdge(&(*it));

	//构建初始解
	std::string x(V, '0');
	int numserve = 0;
	for (auto it = custom_nodes.begin(); it != custom_nodes.end(); ++it)
	{
		int v = *it;
		tmpEdges.push_back(Edge(s, v, maxflows, 0));
		tmpG.addEdge(&tmpEdges.back());
		x[v] = '1';
		++numserve;
	}

	MinCostMaxFlow mm(tmpG, s, t);
	int cost = tmpG.cost() + numserve*servecost;



	bestCost = cost;
	bestX = x;
	bestEdges = std::move(tmpEdges);
}


void SolutionByTabu::step()
{
	InitialSolution init(originEdges, s, t, V, maxflows);
	auto edges = init.initialEdges();
	auto numserve = init.initialServeNumber();
	auto x = init.initialX();
	int cost = init.initialCost(servecost);

	int neighbor_num = 0;
	if (V < 200)
	{
		neighbor_num = V;
	}
	else {
		neighbor_num = 0.05*V;
	}

	Tabu tb(originG.V(), edges, s, t, maxflows, servecost);
	tb.setInitSolution(x, cost, numserve);
	tb.setOutTime(80000);
	tb.setTableParamaters(INT_MAX);
	tb.setIteration(100);
	tb.setNeighborParamter(neighbor_num);
	tb.localSearch();

	x = tb.localBestLocation();
	edges = tb.localBestEdges();
	cost = tb.localBestCost();

	if (cost < bestCost)
	{
		bestEdges = std::move(edges);
		bestX = x;
		bestCost = cost;
	}
	return;
}

void SolutionByTabu::search()
{
	auto start_time = getCurTime();
	initial();
	auto cur_time = getCurTime();
	int residual_time = 90000 - (cur_time - start_time);

	while (iters<1)
	{
		step();
		++iters;
		cur_time = getCurTime();
		residual_time = 90000 - (cur_time - start_time);
		aver_time = (int)((double)(cur_time - start_time) / iters);
		std::cout << iters << std::endl;
	}
	std::cout << bestCost << std::endl;
	for (int i = 0; i < bestX.size(); ++i)
	{
		if (bestX[i] == '1')std::cout << i << ' ';
	}
}

std::string SolutionByTabu::output()
{
	std::string ret;

	int numPaths = 0;
	Graph g(originG.V());
	for (auto it = bestEdges.begin(); it != bestEdges.end(); ++it)
	{
		if (it->flow() > 0)  g.addEdge(&(*it));
	}


	SearchPath2 sp(g, s, t);
	while (sp.hasPath())
	{
		path = sp.path();
		++numPaths;
		realPath(path, ret);
		int d =augment();
		ret += ' ';
		ret += std::to_string(d);
		ret += '\n';
		sp = SearchPath2(g, s, t);
	}

	ret = std::to_string(numPaths) + '\n' + '\n' + ret;
	return ret;
}

void SolutionByTabu::realPath(std::vector<Edge*>& path, std::string &ret)
{
	int w = 0;
	for (auto it = path.begin(); (it+1) != path.end(); ++it)
	{
		Edge* e = *it;
		int v = e->either();
		w = e->other(v);
		ret += std::to_string(w);ret += ' ';
	}
	ret += std::to_string(map[w]);
}

int SolutionByTabu::augment()
{
	//确定增广值
	int d = INT_MAX;
	int x = s;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->flow());
		x = w;
	}
	x = s;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(x, d);
		x = w;
	}

	return d;
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
	std::istringstream in;
	in.str(topo[0]);
	in >> V >> E >> customs;
	//增加虚拟s=V，t=V+1
	s = V, t = V + 1;
	Graph g(V + 2);


	in.str(topo[2]);
	in >> servecost;

	int v = 0, w = 0, cap = 0, cost = 0;
	for (int i = 4; i < E+4; ++i)
	{
		in.str(topo[i]);
		in >> v >> w >> cap >> cost;
		originEdges.push_back(Edge(v, w, cap, cost));
		g.addEdge(&originEdges.back());
		originEdges.push_back(Edge(w, v, cap, cost));
		g.addEdge(&originEdges.back());

	}

	for (int i = E+5; i < customs+E+5; ++i)
	{
		in.str(topo[i]);
		in >> v >> w >> cap;//v是消费结点，w是相连的网络结点
		map[w] = v;
		originEdges.push_back(Edge(w, t, cap, 0));
		g.addEdge(&originEdges.back());
		maxflows += cap;
		custom_nodes.push_back(w);
	}
	originG = g;
	


	SolutionByTabu sbt;

	std::string str = sbt.output();

	// 需要输出的内容
	char * topo_file = (char*)str.c_str();

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
}