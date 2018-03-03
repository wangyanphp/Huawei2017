#ifndef GRAPH_H_
#define GRAPH_H_
#include"bag.h"
#include<string>
#include<vector>
#include<limits.h>
class Edge
{
private:
	int v_;//起点
	int w_;//终点
	int cap_;//容量
	int flow_;//流量
	int cost_;//费用

public:
	Edge(int v = -1, int w = -1, int cap = 0, int cost = 0, int flow = 0) :v_(v), w_(w), cap_(cap), cost_(cost), flow_(flow) {}

	bool from(int v) const { return v_ == v; }
	int either() const { return v_; }
	int other(int v) const { return v == v_ ? w_ : v_; }
	int flow() const { return flow_; }
	int cap() const { return cap_; }
	void setFlow(int f) { flow_ = f; }

	//capTo和addFlowTo用于实现残存网络抽象
	int capTo(int v) const { return from(v) ? flow_ : cap_ - flow_; }
	int costTo(int v) const { return from(v) ? -cost_ : cost_; }
	void addFlowTo(int v, int d) { flow_ += from(v) ? -d : d; }

	std::string tostring();
};


class Graph
{
private:
	int V_;
	int E_ = 0;
	std::vector<Bag<Edge*>> adj_;

public:
	typedef Bag<Edge*>::Iterator Iterator;

	Graph(int V) :V_(V), adj_(V) {}
	int V() const { return V_; }
	int E() const { return E_; }
	int cost();
	

	void addEdge(Edge* e);//添加一条边
	void removeEdge(Edge* e);//删除一条边

	Iterator begin(int v) { return adj_[v].begin(); }
	Iterator end(int v) { return adj_[v].end(); }
	
	int flow(int s);
	std::string toString();
	void valid();
	void cycleValid();
	void location();
};

#endif
