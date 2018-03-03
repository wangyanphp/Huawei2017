#ifndef NETWORKSIMPLEX_H_
#define NETWORKSIMPLEX_H_

#include<vector>
#include"bag.h"
#include"uf.h"
#include"graph.h"
#include<list>
#include<limits.h>

/**
 *说明：使用有向边，同时使用有向边抽象出残存网络
 *所以：在Graph中/所有函数中，Edge都是以指针的形式出现
 *在Graph中同一条边保存两次，对于v的邻接表：
 * 1.当只考虑v->w方向的边时，Graph等价于一般的有向图
 * 2.当考虑v邻接表中所有的边：v->capTo(w)有值的边，那么Graph等价于残存网络
 */
class NetworkSimplex
{
private:
	Graph &g_;
	int s_, t_;
	std::vector<Edge*> st_;//生成树
	std::vector<int> phi_;//顶点势
	std::vector<int> marked_;
	int valid = 0;//全局变量，用于避免marked_初始化

public:
	NetworkSimplex(Graph &g, int s, int t)
		:g_(g), s_(s), t_(t), marked_(g.V(), 0), phi_(g.V(), 0), st_(g.V(), nullptr)
	{
		createTree();
		marked_ = std::vector<int>(g.V(), 0);
		marked_[s_] = ++valid; phi_[s_] = 0;
		eligible();
	}
private:
	int parent(int v) const { return st_[v]->other(v); }

	void createTree();
	int phi(int v);
	
	int reduceCost(Edge* e);
	void eligible();
	void eligible2();

	int lca(int v, int w);	
	bool onPath(int a, int b, int c);
	void reverse(int u, int x);
	Edge* augment(Edge* x);
	void update(Edge* out, Edge* in);


	void augment(Graph &g, std::vector<Edge*>& cycle, int cycleBeg);
	void dfs(Graph& g, int v);

};

#endif

