#ifndef STACK_H_
#define STACK_H_
#include<forward_list>
#include<exception>
/**
*  The {@code Stack} class represents a last-in-first-out (LIFO) stack of generic items.
*  It supports the usual <em>push</em> and <em>pop</em> operations, along with methods
*  for peeking at the top item, testing if the stack is empty, and iterating through
*  the items in LIFO order.
*  <p>
*  This implementation uses a singly-linked list with a static nested class for
*  linked-list nodes. See {@link LinkedStack} for the version from the
*  textbook that uses a non-static nested class.
*  The <em>push</em>, <em>pop</em>, <em>peek</em>, <em>size</em>, and <em>is-empty</em>
*  operations all take constant time in the worst case.
*  <p>
* API
* Stack() ����һ����ջ
* void push(Item item) ���һ��Ԫ��
* Item pop() ɾ�������ӵ�Ԫ��
* bool isEmpty() ջ�Ƿ�Ϊ��
* int size() ջ�е�Ԫ������
* ���ڵ���
* Iter begin();
* Iter end();
*���ڵ���/����
* string toString() ����stack���ַ�����ʾ
*  @param <Item> the generic type of an item in this stack
*/

template<typename Item>
class Stack
{
public:
	typedef std::forward_list<Item> List;
	typedef typename List::iterator Iterator;
	typedef typename List::const_iterator CIterator;
private:
	List list;
	int N;//number of elements in bag
public:
	/**
	 *Initializes an empty Stack
	 */
	Stack() :list(List()),N(0) {}

	/**
	 *return true if this stack is empty
	 */
	bool isEmpty() const
	{
		return list.empty();
	}

	/**
	 * return the number of items in this stack
	 */
	int size() const
	{
		return N;
	}

	/**
	 * add the item to this stack
	 */
	void push(const Item &item)
	{
		list.push_front(item);
		++N;
	}
	void push(Item &&item)
	{
		list.push_front(std::forward<Item>(item));
		++N;
	}

	/*
	 *return and remove item most recently added in this stack
	 */
	Item pop()
	{
		if (isEmpty())
			throw std::underflow_error("Stack underflow");
		Item item = list.front();
		list.pop_front();
		--N;
		return item;
	}

	/*
	 *return the item most recently added in this stack
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