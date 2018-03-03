#include"directedcycle.h"

DirectedCycle::DirectedCycle(Graph &g, bool icycle)
	:marked_(g.V(), false), edgeTo_(g.V(), nullptr), onStack_(g.V(), false)
{//��icycle=trueʱ�������Ի������򲻿����Ի�
	if (icycle)
	{
		for (int s = 0; s < g.V(); ++s)
		{
			if (!marked_[s]) dfs(g, s, s);
		}
	}
	else {
		for (int s = 0; s < g.V(); ++s)
			if (!marked_[s]) dfs(g, s);
	}
}

/**
*���ڴ�����������ԣ����Ǳ�����v->w�����ǿ��ܷ����ǣ�v->w��w->v
*������Ҫ���滷����ʼ�ڵ㣺Ҳ�����ķ���
*/
std::vector<Edge*> DirectedCycle::cycle() const
{
	std::vector<Edge*> c;
	for (auto x : cycle_)
		c.push_back(x);
	return c;
}

//�������Ի������
void DirectedCycle::dfs(Graph &g, int v, int u)
{
	onStack_[v] = true;
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{

		auto e = *it;
		int w = e->other(v);
		//����v->w-----v->w�Ƿ����
		if (e->capTo(w) <= 0) continue;

		if (hasCycle()) return;
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w, v);
		}
		else if (onStack_[w] && w != u)
		{
			//��w��ʼ
			int x = w;
			auto f = e;
			while (f->other(x) != w)
			{
				cycle_.push(f);
				x = f->other(x);
				f = edgeTo_[x];

			}
			cycle_.push(f);
			cycle_beg_ = w;
			return;
		}
	}
	onStack_[v] = false;
}

//�����Ի������
void DirectedCycle::dfs(Graph &g, int v)
{
	onStack_[v] = true;
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{

		auto e = *it;
		int w = e->other(v);
		//����v->w-----v->w�Ƿ����
		if (e->capTo(w) <= 0) continue;

		if (hasCycle()) return;
		else if (!marked_[w])
		{
			edgeTo_[w] = e;
			dfs(g, w);
		}
		else if (onStack_[w])
		{
			//��w��ʼ
			int x = w;
			auto f = e;
			while (f->other(x) != w)
			{
				cycle_.push(f);
				x = f->other(x);
				f = edgeTo_[x];

			}
			cycle_.push(f);
			cycle_beg_ = w;
			return;
		}
	}
	onStack_[v] = false;
}