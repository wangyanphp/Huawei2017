#ifndef TABUTABLE_H_
#define TABUTABLE_H_
#include<list>
#include<set>
#include<string>
class TabuTable
{//禁忌表所用的数据结构，有固定大小
private:
	int N_;
	std::list<std::string> queue_;//固定大小的队列
	std::set<std::string> set_;//用于查找

public:
	TabuTable(int initN = 11) :N_(initN) {}
	int size() const { return N_; }
	bool contain(const std::string &x) const { return (set_.find(x) != set_.end()); }
	void add(const std::string& c);
	void erase(const std::string& c);
	void resize(int N);
};
#endif
