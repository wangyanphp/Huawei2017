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
#include<windows.h>



int getRandInt(int N)
{
	static std::default_random_engine e(std::time(nullptr));
	static std::uniform_int_distribution<unsigned> u(0, N);
	return u(e);
} 

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
		if (e->capTo(w) == 0) continue;

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
	const Graph& originG;//原始图，不发生改变
	const std::list<Edge>& originEdges;

	std::list<Edge> bestEdges;
	Code bestX;
	int bestCost;

	int ITER;
	int iters = 0;//迭代次数
	double aver_time;//迭代一次平均耗时

	std::vector<Edge*> path;

public:
	SolutionByTabu(const Graph &g, const std::list<Edge> &edges, int iter=20)
		:originG(g), originEdges(edges),ITER(iter)
	{
		search();
		std::cout << "iters: " << iters << std::endl;
		Graph gg(originG.V());
		for (auto it = bestEdges.begin(); it != bestEdges.end(); ++it)
		{
			gg.addEdge(&(*it));
		}
		std::cout << bestCost << std::endl;
		gg.location();
		gg.valid();
		std::cout << gg.flow(s) << std::endl;
	}


private:
	void initial();
	void step();
	void search();


	void output();
	void realPath(std::vector<Edge*>& path);
	void augment();
};

void SolutionByTabu::initial()
{
	std::list<Edge> tmpEdges = originEdges;
	Graph tmpG(originG.V());
	for (auto it = tmpEdges.begin(); it != tmpEdges.end(); ++it)
		tmpG.addEdge(&(*it));

	//构建初始解
	Code x(V, '0');
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
	std::list<Edge> tmpEdges = originEdges;
	Graph tmpG(originG.V());
	for (auto it = tmpEdges.begin(); it != tmpEdges.end(); ++it)
		tmpG.addEdge(&(*it));

	//构建初始解
	Code x(V, '0');
	int numserve = 0;
	std::set<int> set;
	while (tmpG.flow(s) != maxflows)
	{
		int v = getRandInt(V - 1);
		if (set.find(v) != set.end()) continue;
		set.insert(v);
		tmpEdges.push_back(Edge(s, v, maxflows, 0));
		tmpG.addEdge(&tmpEdges.back());
		EdmondsKarp(tmpG, s, t);
		x[v] = '1';
		++numserve;
	}

	int cost = tmpG.cost() + numserve*servecost;

	//禁忌搜索
	Tabu tabu(tmpG.V());
	tabu.setHyperParamaters(10, 10);
	tabu.setTableParamaters(7);
	tabu.setInitGraph(tmpG, tmpEdges, s, t, maxflows);
	tabu.setInitSolution(x, cost, servecost);
	tabu.setIteration(INT_MAX);
	tabu.setOutTime(25000);
	tabu.localSearch();
	std::cout << tabu.count() << std::endl;

	x = tabu.localBestLocation();
	tmpEdges = tabu.localBestEdges();
	cost = tabu.localBestCost();

	if (cost < bestCost)
	{
		bestEdges = std::move(tmpEdges);
		bestX = x;
		bestCost = cost;
	}

	std::cout << "cost: "<<cost << std::endl;
}
void SolutionByTabu::search()
{
	DWORD start_time = GetTickCount();
	initial();
	DWORD cur_time = GetTickCount();
	int residual_time = 90000 - (cur_time - start_time);

	while ((residual_time>1.5*aver_time) &&(iters<ITER))
	{
		step();
		++iters;
		cur_time = GetTickCount();
		residual_time = 90000 - (cur_time - start_time);
		aver_time = (int)((double)(cur_time - start_time) / iters);
	}
}

void SolutionByTabu::output()
{
	int numPaths = 0;
	Graph g(originG.V());
	for (auto it = bestEdges.begin(); it != bestEdges.end(); ++it)
	{
		if (it->flow() > 0)  g.addEdge(&(*it));
	}

	SearchPath2 sp(g, s, t);
	while (sp.hasPath())
	{
		auto path = sp.path();
		++numPaths;

		augment();
		sp = SearchPath2(g, s, t);
	}
}
void SolutionByTabu::realPath(std::vector<Edge*>& path)
{
	std::vector<int> nodes;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int v = e->either();
		nodes.push_back(e->other(v));
	}
	nodes.pop_back();
	int w = nodes.back();
	nodes.push_back(map[w]);
}
void SolutionByTabu::augment()
{
	//确定增广值
	int d = INT_MAX;
	int x = s;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = min(d, e->capTo(w));
		x = w;
	}
	x = s;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	std::ifstream in("E:\\case_example\\case0.txt");
	in >> V >> E >> customs;
	//增加虚拟s=V，t=V+1
	s = V, t = V + 1;
	Graph g(V + 2);
	std::list<Edge> edges;
	in >> servecost;
	int v = 0, w = 0, cap = 0, cost = 0;
	for (int i = 0; i < E; ++i)
	{
		in >> v >> w >> cap >> cost;
		edges.push_back(Edge(v, w, cap, cost));
		g.addEdge(&edges.back());
		edges.push_back(Edge(w, v, cap, cost));
		g.addEdge(&edges.back());
	}

	for (int i = 0; i < customs; ++i)
	{
		in >> v >> w >> cap;//v是消费结点，w是相连的网络结点
		map[w] = v;
		edges.push_back(Edge(w, t, cap, 0));
		g.addEdge(&edges.back());
		maxflows += cap;
		custom_nodes.push_back(w);
	}


	/*
	edges.push_back(Edge(s, 7, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 13, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 15, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 22, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 37, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 38, maxflows, 0)); g.addEdge(&edges.back());
	edges.push_back(Edge(s, 43, maxflows, 0)); g.addEdge(&edges.back());

	MaxFlow mf(g, s, t);
	g.valid();
	std::cout << g.flow(s) << std::endl;
	g.location();
	MinCost ns(g, s, t);
	g.valid();
	g.cycleValid();
	g.location();
	std::cout << g.flow(s) << std::endl;
	std::cout << g.cost() + servecost * 7;
	*/
	DWORD start_time = GetTickCount();

	SolutionByTabu sbt(g, edges);



	DWORD end_time = GetTickCount();
	std::cout << "The run time is:" << (end_time - start_time) << "ms!";

	/*
	
	g.valid();
	g.location();
	std::cout << g.cost()<<std::endl;
	std::cout << g.flow(s) << std::endl;
	for (int v = 0; v < g.V(); ++v)
	{
		for (auto it = g.begin(v); it != g.end(v); ++it)
		{
			Edge* e = *it;
			if(e->either()==v)
				std::cout << e->either() << "->" << e->other(e->either()) << ": " << e->flow() << std::endl;
		}
	}*/
	
	system("pause");
	
}