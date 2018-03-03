#include"networkflow.h"
#include"directedcycle.h"
#include<algorithm>
#include<iostream>
//-----------------------------------------------------network simplex------------------------------------------------------
/**
 *1.����������ṹ
 *2.�����Ӧ��������ʱ�������������һ��
 *3.��ö�Ӧ��phi����
 *while���ںϸ�ߣ�
 *   ���ϸ�߼������ṹ��ͬʱ���һ���ߣ�
 *   ��ʱ���ṹ�ı䣬����phi�ı䣬���ºϸ�߸ı�......
 *
 *networksimplex�����У��ߵ�Ӧ�������������
 *1.����ߣ������㶥���ƺͺϸ�ߵ�ʱ�򣬼���ϸ�ߵ�ʱ����Ҫ���������߻��ǿձ�
 *2.����ߣ��������Ĳ��������ǽ��ߵ�������ߣ�������ǿ������ĸ��ӽṹ
 *3.˫��ߣ��д�����
 */

//1. ���ݿ���������һ��������
/**
 *�ٶ��Ѿ����ڿ�����
 *1. free arc�Ƿ񹹳ɻ���������ɻ��Ļ�����������������
 *2. ����һ�������������е�free arc
 */
void NetworkSimplex::createTree()
{
	Graph free_g(g_.V());
	for (int v = 0; v < g_.V(); ++v)
	{
		for (auto it = g_.begin(v); it != g_.end(v); ++it)
		{//Ϊ�˱����ظ��ߣ�ֻ����һ�������ϵı�
			Edge * e = *it;
			int w = e->other(v);
			if (!e->from(v)) continue;
			if (e->capTo(w) == 0 || e->capTo(v) == 0) continue;
			free_g.addEdge(e);
		}
	}

	//���freeGraph�д��ڻ���������(����,���Բ��ܿ����Ի���
	DirectedCycle dc(free_g);
	while (dc.hasCycle())
	{
		int cycleBeg = dc.cycleBegin();
		std::vector<Edge*> cycle = dc.cycle();
		augment(free_g, cycle, cycleBeg);
		//����free_g�����������߻�ձߴ�free_g��ɾ��
		for (Edge* e : cycle)
		{
			int v = e->either();
			int w = e->other(v);
			if (e->capTo(v) == 0 || e->capTo(w) == 0) free_g.removeEdge(e);
		}
		dc = DirectedCycle(free_g);
	}

	//������//ѡ��tree arc set
	std::list<Edge*> edges;
	UF uf(g_.V());
	for (int v = 0; v < g_.V(); ++v)
	{
		for (auto it = g_.begin(v); it != g_.end(v); ++it)
		{//Ϊ�˱����ظ��ߣ�ֻ����һ�������ϵı�
			Edge * edge = *it;
			int w = edge->other(v);
			if (!edge->from(v)) continue;
			if (edge->capTo(w) == 0 || edge->capTo(v) == 0) edges.push_back(edge);
			else edges.push_front(edge);
		}
	}
	Graph g(g_.V());
	while (g.E() < g.V() - 1)
	{
		Edge* e = edges.front();edges.pop_front();
		int v = e->either();
		int w = e->other(v);
		if (uf.connected(v, w)) continue;
		uf.unions(v, w);
		g.addEdge(e);
	}

	//ʹ��dfsȷ�����Ľṹ
	dfs(g, s_);
}

//2.���㶥����
int NetworkSimplex::phi(int v)
{//����marked�Ѿ���ʼ��
	if (marked_[v] == valid) return phi_[v];
	phi_[v] = phi(parent(v)) - st_[v]->costTo(v);
	marked_[v] = valid;
	return phi_[v];
}

//3.ѡ��ϸ��
//�����ߵ�reduced costȡ�����Ա�ձߺ�����ͳһ�Ƚ�
int NetworkSimplex::reduceCost(Edge* e)
{
	int v = e->either();
	int w = e->other(v);
	int r = e->costTo(w) + phi(w) - phi(v);
	if (e->capTo(w) == 0) return -r;
	return r;
}

void NetworkSimplex::eligible()
{
	int old = 0;
	for (valid = 1; valid != old;)
	{
		old = valid;

		for (int v = 0; v < g_.V(); ++v)
		{
			for (auto it = g_.begin(v); it != g_.end(v); ++it)
			{
				//����Ǻϸ��
				Edge* e = *it;
				int w = e->other(v);
				if (!e->from(v)) continue;
				int r = e->costTo(w) - phi(v) + phi(w);
				if ((e->capTo(w) == 0 && r>0) || (e->capTo(v) == 0 && r < 0))
				{
					Edge* out = augment(e);
					if(e!=out) 
						update(out, e);
					++valid;
					//���¶�����
					marked_[s_] = valid; phi_[s_] = 0;
				}
			}
		}
	}
}

//4.���ϸ�߼�������Ȼ���ҳ����㻷�����㣬ͬʱȷ�������
int NetworkSimplex::lca(int v, int w)
{
	marked_[v] = ++valid; marked_[w] = valid;
	while (v != w)
	{
		if (v != s_) v = st_[v]->other(v);
		if (v != s_ && marked_[v] == valid) return v;
		marked_[v] = valid;
		if (w != s_) w = st_[w]->other(w);
		if (w != s_&&marked_[w] == valid) return w;
		marked_[w] = valid;
	}
	return v;
}

bool NetworkSimplex::onPath(int a, int b, int c)
{
	for (int i = a; i != c; i = parent(i))
		if (i == b) return true;
	return false;
}

void NetworkSimplex::reverse(int u, int x)
{
	Edge* e = st_[u];
	int i = parent(u);
	while (i != x)
	{
		Edge* y = st_[i];
		int tmp_i = parent(i);
		st_[i] = e;
		e = y;
		i = tmp_i;
	}
}

Edge* NetworkSimplex::augment(Edge* x)
{//����x�ķ����������:����ǿձߣ���������������������ߣ������ŷ�����

	int v = x->either();
	int w = x->other(v);
	if (x->capTo(v) > 0)//����
		std::swap(v, w);
	int r = lca(v, w);

	//ȷ������ֵ
	int d = x->capTo(w);
	for (int u = w; u != r; u = parent(u))
		d = std::min(d, st_[u]->capTo(parent(u)));
	for (int u = v; u != r; u = parent(u))
		d = std::min(d, st_[u]->capTo(u));

	x->addFlowTo(w, d);
	Edge* e = x;
	for (int u = w; u != r; u = parent(u))
	{
		st_[u]->addFlowTo(parent(u), d);
		if (st_[u]->capTo(parent(u)) == 0)
			e = st_[u];
	}
	for (int u = v; u != r; u = parent(u))
	{
		st_[u]->addFlowTo(u, d);
		if (st_[u]->capTo(u) == 0)
			e = st_[u];
	}
	return e;
}

void NetworkSimplex::update(Edge* out, Edge* in)
{
	int v = in->either();
	int w = in->other(v);
	//x��out�ĸ��ڵ㣬Ҳ��:x��y�ĸ��ڵ㣨�����У�
	int x = out->either();
	int y = out->other(x);
	if (st_[x] == out) std::swap(x, y);

	int r = lca(v, w);
	//�鿴out������·����,��Ϊx���ܵ���r�����Ի��ǲ鿴y������һ��
	if (onPath(v, y, r))
	{
		reverse(v, x);
		st_[v] = in;
		return;
	}
	if (onPath(w, y, r))
	{
		reverse(w, x);
		st_[w] = in;
		return;
	}
}



//�������ṹ��Ҫʹ�õ������������������Ż������dfs
void NetworkSimplex::augment(Graph &g, std::vector<Edge*>& cycle, int cycleBeg)
{
	//ȷ������ֵ
	int d = INT_MAX;
	int x = cycleBeg;
	for (auto it = cycle.begin(); it != cycle.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->capTo(w));
		x = w;
	}
	x = cycleBeg;
	for (auto it = cycle.begin(); it != cycle.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}

void NetworkSimplex::dfs(Graph& g, int v)
{
	marked_[v] = true;

	for (auto it = g.begin(v); it != g.end(v); ++it)
	{
		Edge* e = *it;
		int w = e->other(v);
		if (!marked_[w])
		{
			st_[w] = e;
			dfs(g, w);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------

void NetworkSimplex::eligible2()
{
	int NodesPerIter = 50;
	int arcsPerIter = 100;

	int old = 0;
	for (valid = 1; valid != old;)
	{
		old = valid;

		int M = std::ceil(((double)g_.V()) / NodesPerIter);
		int nextNode = 0;
		std::list<Edge*> arcs;
		for (int m = 0; m < M; ++m)
		{
			arcs.clear();
			for (int i = 0; i < NodesPerIter; ++i)
			{//ֻ���������
				if (nextNode == g_.V()) nextNode = 0;
				int v = nextNode;
				for (auto it = g_.begin(v); it != g_.end(v); ++it)
				{
					Edge* e = *it;
					int w = e->other(v);
					if (!e->from(v)) continue;
					int r = e->costTo(w) - phi(v) + phi(w);
					if (e->capTo(w) == 0 && r>0) arcs.push_back(e);//����
					else if (e->capTo(v) == 0 && r<0)arcs.push_back(e);//�ձ�
				}
				++nextNode;
			}

			for (int i = 0; i < arcsPerIter; ++i)
			{
				Edge* x = nullptr;
				int min = INT_MAX;
				auto minIt = arcs.begin();

				for (auto it = arcs.begin(); it != arcs.end(); ++it)
				{
					Edge* e = *it;
					if (reduceCost(e) < min)
					{
						x = e;
						minIt = it;
						min = reduceCost(e);
					}
				}

				if (x == nullptr) break;
				if (reduceCost(x) == 0) break;

				Edge* y = augment(x);
				update(y, x);
				arcs.erase(minIt);
				int v = y->either();
				if ((v < nextNode && (nextNode - NodesPerIter) <= v) || (v < nextNode + g_.V() && (nextNode + g_.V() - NodesPerIter) <= v))
					arcs.push_back(y);
				++valid;
				//���¶�����
				marked_[s_] = valid; phi_[s_] = 0;
			}
		}
	}
}

