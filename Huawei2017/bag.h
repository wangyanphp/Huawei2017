#ifndef BAG_H
#define BAG_H
#include<forward_list>
#include<string>
template<typename Item>
class Bag
{
public:
	typedef std::forward_list<Item> List;
	typedef typename List::iterator Iterator;
	typedef typename List::const_iterator CIterator;
private:
	List list;
	int N;//number of elements in bag

public:

	Bag() :list(List()), N(0) {}

	bool isEmpty() const
	{
		return list.empty();
	}

	int size() const
	{
		return N;
	}

	void add(const Item &item)
	{
		list.push_front(item);
		++N;
	}
	void add(Item &&item)
	{
		list.push_front(std::forward<Item>(item));
		++N;
	}

	void erase(const Item &item)
	{
		auto prev = list.before_begin();
		auto cur = list.begin();
		while (cur != list.end())
		{
			if (*cur == item)
			{
				list.erase_after(prev);
				break;
			}
			prev = cur;
			++cur;
		}
	}

	Iterator begin() { return list.begin(); }
	CIterator begin() const { return list.begin(); }


	Iterator end() { return list.end(); }
	CIterator end() const { return list.end(); }
};
#endif


