#include"tabutable.h"
void TabuTable::add(const std::string& c)
{
	queue_.push_back(c);
	set_.insert(c);
	if (queue_.size() > N_)
	{
		auto x = queue_.front(); queue_.pop_front();
		set_.erase(x);
	}
}

void TabuTable::erase(const std::string& c)
{
	set_.erase(c);
	for (auto it = queue_.begin(); it != queue_.end(); ++it)
	{
		if (*it == c)
		{
			queue_.erase(it);
			break;
		}
	}
}

void TabuTable::resize(int N)
{
	if (N_ <= N)//扩大队列
	{
		N_ = N;
		return;
	}
	//缩小队列
	int n = queue_.size() - N;
	if (n <= 0) return;
	for (int i = 0; i < n; ++i)
	{
		auto x = queue_.front(); queue_.pop_front();
		set_.erase(x);
	}
	N_ = N;
}
