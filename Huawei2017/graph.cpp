#include"graph.h"
#include<algorithm>
#include<iostream>
#include"directedcycle.h"
//---------------------------------------------------------------------------------------------------------------------------
std::string Edge::tostring()
{
	std::string ret = std::to_string(v_) + '-' + std::to_string(w_) + ' ';
	std::string wei = std::to_string(cap_) + ' ' + std::to_string(flow_) + ' ' + std::to_string(cost_);
	return ret + wei;
}
//--------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------Graph---------------------------------------------------------------------
void Graph::addEdge(Edge* e)
{
	int v = e->either();
	int w = e->other(v);
	int u = std::max(v, w);
	if (V_ - 1 < u)
	{
		for (int i = V_; i <= u; ++i)
		{
			++V_;
			adj_.push_back(Bag<Edge*>());
		}
	}
	adj_[v].add(e);
	adj_[w].add(e);
	++E_;
}

void Graph::removeEdge(Edge* e)
{
	int v = e->either();
	int w = e->other(v);
	adj_[v].erase(e);
	adj_[w].erase(e);
	--E_;
}

int Graph::cost()
{
	int c = 0;
	for (int v = 0; v < V_; ++v)
	{
		for (auto it = begin(v); it != end(v); ++it)
		{
			Edge* e = *it;
			int w = e->other(v);
			if (e->from(v))
				c += e->costTo(w)*e->capTo(v);
		}
	}
	return c;
}

int Graph::flow(int s)
{
	int f = 0;
	for (auto it = begin(s); it != end(s); ++it)
	{
		Edge* e = *it;
		if (e->from(s))
			f += e->flow();
	}
	return f;
}

std::string Graph::toString()
{
	std::string ret;
	for (int v = 0; v < V_; ++v)
	{
		for (auto it = begin(v); it != end(v); ++it)
		{
			Edge* e = *it;
			if (e->from(v))
				ret += e->tostring() + '\n';
		}
	}
	return ret;
}

void Graph::valid()
{
	for (int v = 0; v < V_ - 2; ++v)
	{
		int f = 0;
		for (auto it = begin(v); it != end(v); ++it)
		{
			Edge* e = *it;
			if (e->from(v)) f -= e->flow();
			else f += e->flow();
		}
		if (f != 0)
		{
			std::cout << "not valid" << std::endl;
			return;	
		}
	}

	int fs = flow(V_-2);

	int ft = 0;
	int t = V_ - 1;
	for (auto it = begin(t); it != end(t); ++it)
	{
		Edge* e = *it;
		if (!e->from(t))
			ft += e->flow();
	}

	if (fs != ft)
		std::cout << "not valid" << std::endl;
	std::cout << "valid" << std::endl;
	
}


void Graph::cycleValid()
{
	for (int v = 0; v < V_ - 2; ++v)
	{
		for (auto it = begin(v); it != end(v); ++it)
		{
			Edge* e = *it;
			
			if (e->from(v)&&e->flow()>0)
			{
				int w = e->other(v);
				for (auto it = begin(v); it != end(v); ++it)
				{
					if (e->from(w) && e->flow() > 0)
					{
						std::cout << "have identify cycle" << std::endl;
						return;
					}
				}
			}
		}
	}
	std::cout << "no identify cycle" << std::endl;
	return;
}

void Graph::location()
{
	int v = V_ - 2;
	for (auto it = begin(v); it != end(v); ++it)
	{
		Edge* e = *it;
		if (e->from(v)) std::cout << e->other(v) << " ";
	}
	std::cout << std::endl;
}
//--------------------------------------------------------------------------------------------------------------------------//