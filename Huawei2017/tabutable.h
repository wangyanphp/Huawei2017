#ifndef TABUTABLE_H_
#define TABUTABLE_H_
#include<list>
#include<set>
#include<string>
class TabuTable
{//���ɱ����õ����ݽṹ���й̶���С
private:
	int N_;
	std::list<std::string> queue_;//�̶���С�Ķ���
	std::set<std::string> set_;//���ڲ���

public:
	TabuTable(int initN = 11) :N_(initN) {}
	int size() const { return N_; }
	bool contain(const std::string &x) const { return (set_.find(x) != set_.end()); }
	void add(const std::string& c);
	void erase(const std::string& c);
	void resize(int N);
};
#endif
