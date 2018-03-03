#include"maxflow.h"
#include<algorithm>
#include"queue.h"
#include"stack.h"

class BFS
{
private:
	std::vector<Edge*> edgeTo_;
	std::vector<bool> marked_;
	int s_;

public:
	BFS(Graph &g, int s) :marked_(g.V(), false), edgeTo_(g.V(), nullptr),s_(s)
	{
		bfs(g, s);
	}

	bool hasPathTo(int v) const { return marked_[v]; }
	std::vector<Edge*> pathTo(int v) const;

private:
	void bfs(Graph &g, int s);
};

void BFS::bfs(Graph &g, int s)
{
	Queue<int> queue;
	marked_[s] = true;
	queue.enqueue(s);
	while (!queue.isEmpty())
	{
		int v = queue.dequeue();
		for (auto it = g.begin(v); it != g.end(v); ++it)
		{

			auto e = *it;
			int w = e->other(v);
			//访问v->w-----v->w是否存在
			if (e->capTo(w) <= 0) continue;
			if (!marked_[w])
			{
				edgeTo_[w] = e;
				marked_[w] = true;
				queue.enqueue(w);
			}
		}
	}
}

std::vector<Edge*> BFS::pathTo(int v) const
{
	std::vector<Edge*> path;
	Stack<Edge*> stack;
	if (!hasPathTo(v)) return path;
	//从v开始
	int x = v;
	auto f = edgeTo_[x];
	while (f != nullptr)
	{
		stack.push(f);
		x = f->other(x);
		f = edgeTo_[x];
	}

	for (auto x : stack)
		path.push_back(x);
	return path;
}


bool EdmondsKarp::findShortestPath()
{
	BFS bfs(g_, s_);
	if (bfs.hasPathTo(t_))
	{
		path = bfs.pathTo(t_);
		return true;
	}
	return false;
}
void EdmondsKarp::augment()
{
	//确定增广值
	int d = INT_MAX;
	int x = s_;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		d = std::min(d, e->capTo(w));
		x = w;
	}
	x = s_;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		Edge* e = *it;
		int w = e->other(x);
		e->addFlowTo(w, d);
		x = w;
	}
}