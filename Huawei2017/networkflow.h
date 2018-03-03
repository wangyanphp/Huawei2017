#ifndef NETWORKSIMPLEX_H_
#define NETWORKSIMPLEX_H_

#include<vector>
#include"bag.h"
#include"uf.h"
#include"graph.h"
#include<list>
#include<limits.h>

/**
 *˵����ʹ������ߣ�ͬʱʹ������߳�����д�����
 *���ԣ���Graph��/���к����У�Edge������ָ�����ʽ����
 *��Graph��ͬһ���߱������Σ�����v���ڽӱ�
 * 1.��ֻ����v->w����ı�ʱ��Graph�ȼ���һ�������ͼ
 * 2.������v�ڽӱ������еıߣ�v->capTo(w)��ֵ�ıߣ���ôGraph�ȼ��ڲд�����
 */
class NetworkSimplex
{
private:
	Graph &g_;
	int s_, t_;
	std::vector<Edge*> st_;//������
	std::vector<int> phi_;//������
	std::vector<int> marked_;
	int valid = 0;//ȫ�ֱ��������ڱ���marked_��ʼ��

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

