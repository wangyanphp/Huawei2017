#ifndef QUEUE_H_
#define QUEUE_H_
#include<forward_list>
#include<exception>
/**
 *  The {@code Queue} class represents a first-in-first-out (FIFO)
 *  queue of generic items.
 *  It supports the usual <em>enqueue</em> and <em>dequeue</em>
 *  operations, along with methods for peeking at the first item,
 *  testing if the queue is empty, and iterating through
 *  the items in FIFO order.
 *  <p>
 *  This implementation uses a singly-linked list with a static nested class for
 *  linked-list nodes. See {@link LinkedQueue} for the version from the
 *  textbook that uses a non-static nested class.
 *  The <em>enqueue</em>, <em>dequeue</em>, <em>peek</em>, <em>size</em>, and <em>is-empty</em>
 *  operations all take constant time in the worst case.
 *  <p>
 * API
 * Queue() �����ն���
 * void enqueue(Item item) ���һ��Ԫ��
 * Item dequeue() ɾ�������ӵ�Ԫ��
 * bool isEmpty() �����Ƿ�Ϊ��
 * int size() �����е�Ԫ������
 * ���ڵ���
 * Iter begin();
 * Iter end();
 *���ڵ���/����
 * string toString() ����queue���ַ�����ʾ
 *  @param <Item> the generic type of an item in this queue
 */

template<typename Item>
class Queue
{
public:
	typedef std::forward_list<Item> List;
	typedef typename List::iterator Iterator;
	typedef typename List::const_iterator CIterator;
private:
	List list;
	Iterator last;
	int N;//number of elements in bag
public:
	/**
	 *Initializes an empty queue
	 */
	Queue() :list(List()),last(list.before_begin()), N(0) {}

	/**
	 *return true if this queue is empty
	 */
	bool isEmpty() const
	{
		return list.empty();
	}

	/**
	 * return the number of items in this queue
	 */
	int size() const
	{
		return N;
	}

	/**
	 * add the item to this queue
	 */
	void enqueue(const Item &item)
	{
		last = list.insert_after(last, item);
		++N;
	}
	void enqueue(Item &&item)
	{
		last=list.insert_after(last, std::forward<Item>(item));
		++N;
	}

	/*
	 *Removes and returns the item on this queue that was least recently added.
	 */
	Item dequeue()
	{
		if (isEmpty())
			throw std::underflow_error("Queue underflow");
		Item item = list.front();
		list.pop_front();
		--N;
		if (isEmpty())
			last = list.before_begin();//��ʱ���һ��Ԫ�ر�ɾ��������last
		return item;
	}

	/*
	 * Returns the item least recently added to this queue.
	 */
	Item peek() const
	{
		if (isEmpty())
			throw std::underflow_error("Stack underflow");
		return list.front();
	}

	/*
	 *return the begin iterator
	 */
	Iterator begin() { return list.begin(); }
	CIterator begin() const { return list.begin(); }

	/*
	*return the end iterator
	*/
	Iterator end() { return list.end(); }
	CIterator end() const { return list.end(); }
};

#endif