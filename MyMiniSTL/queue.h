#ifndef __MYMINISTL_QUEUE_H__
#define __MYMINISTL_QUEUE_H__

#include "deque.h"
//下面两个头文件是用于定义优先队列的 
#include "vector.h"
#include "heapalgo.h"
#include "functional.h"

namespace ministl
{
template <class T, class Container = ministl::deque<T>>
class queue {
	friend bool operator==(const queue& lhs, const queue& rhs);
	friend bool operator< (const queue& lhs, const queue& rhs);

public:
	typedef Container container_type;
	typedef typename Container::value_type value_type;
	typedef typename Container::size_type size_type;
	typedef typename Container::reference reference;
	typedef typename Container::const_reference const_reference;

private:
	//使用一个deque来完成其功能
	container_type myctr;
public:
	queue() = default;
	explicit queue(size_type n):myctr(n){ 
	}
	queue(size_type n, const value_type& value) :myctr(n,value){
	}
	template <class Iterator>
	queue(Iterator first, Iterator last) : myctr(first, last) {
	}

	queue(std::initializer_list<T> llist) :myctr(llist.begin(), llist.end()) {
	}

	queue(const container_type& c) :myctr(c) {
	}
	queue(const queue& q) :myctr(q.myctr) {
	}

	queue& operator=(const queue& q) {
		myctr = q.myctr;
		return *this;
	}

	queue& operator=(std::initializer_list<T> llist) {
		myctr = llist;
		return *this;
	}

	~queue() = default;

	reference& front() {
		return myctr.front();
	}

	const_reference front() const {
		return myctr.front();
	}

	reference& back() {
		return myctr.back();
	}

	const_reference back() const {
		return myctr.back();
	}

	bool empty() const{
		return myctr.empty();
	}

	size_type size() const {
		return myctr.size();
	}

	void push(const value_type& value) {
		myctr.push_back(value);
	}

	void pop() {
		myctr.pop_front();
	}

	void clear()
	{
		while (!empty())
			pop();
	}

};

// 重载比较操作符
template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return lhs < rhs;
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
	return !(lhs < rhs);
}


template <class T, class Container = ministl::vector<T>,
	class Compare = ministl::less<T>>
class priority_queue {
	friend bool operator==(const priority_queue& lhs, const priority_queue& rhs);
	friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs);

public:
	typedef Container container_type;
	typedef Compare compare_type;

	typedef T value_type;
	typedef size_t size_type;
	typedef T& reference;
	typedef const reference const_reference;

private:
	container_type myctr;
	compare_type mycmp;

public:
	// 构造、复制、移动函数
	priority_queue() = default;
	priority_queue(const Compare& c):myctr(),mycmp(c) {
	}
	explicit priority_queue(size_type n):myctr(n) {
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	priority_queue(size_type n, const value_type& value):c_(n, value)
	{
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	template <class Iterator>
	priority_queue(Iterator first, Iterator last): myctr(first, last)
	{
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	priority_queue(std::initializer_list<T> llist):myctr(ilist)
	{
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	priority_queue(const Container& s)
		:myctr(s)
	{
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	priority_queue(const priority_queue& rhs)
		:myctr(rhs.myctr), mycmp(rhs.mycmp)
	{
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
	}
	priority_queue& operator=(const priority_queue& rhs)
	{
		myctr = rhs.myctr;
		mycmp = rhs.mycmp;
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
		return *this;
	}
	priority_queue& operator=(std::initializer_list<T> llist)
	{
		myctr = llist;
		mycmp = compare_type();
		ministl::make_heap(myctr.begin(), myctr.end(), mycmp);
		return *this;
	}
	~priority_queue() = default;
public:
	const_reference top() const { return myctr.front(); }
	bool empty() const { return myctr.empty(); }
	size_type size() const { return myctr.size(); }
	void push(const value_type& value)
	{
		c_.push_back(value);
		ministl::push_heap(myctr.begin(), myctr.end(), mycmp);
	}
	void pop()
	{
		ministl::pop_heap(myctr.begin(), myctr.end(), mycmp);
		myctr.pop_back();
	}
	void clear()
	{
		while (!empty())
			pop();
	}
	void swap(priority_queue& rhs) noexcept
	{
		ministl::swap(myctr, rhs.myctr);
		ministl::swap(mycmp, rhs.mycmp);
	}
	
};
// 重载比较操作符
template <class T, class Container, class Compare>
bool operator==(priority_queue<T, Container, Compare>& lhs,
	priority_queue<T, Container, Compare>& rhs)
{
	return lhs == rhs;
}

template <class T, class Container, class Compare>
bool operator!=(priority_queue<T, Container, Compare>& lhs,
	priority_queue<T, Container, Compare>& rhs)
{
	return lhs != rhs;
}


}//!ministl

#endif
