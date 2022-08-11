#ifndef __MYMINISTL_STACK_H__
#define __MYMINISTL_STACK_H__

#include "deque.h"

namespace ministl
{
template <class T, class Container = ministl::deque<T>>
class stack {
	friend bool operator==(const stack& lhs, const stack& rhs);
	friend bool operator< (const stack& lhs, const stack& rhs);

public:
	typedef Container container_type;
	// 使用底层容器的型别
	typedef typename Container::value_type value_type;
	typedef typename Container::size_type size_type;
	typedef typename Container::reference reference;
	typedef typename Container::const_reference const_reference;

	//使用一个deque来完成其功能
private:
	container_type myctr;
public:
	//构造 复制 析构
	stack() = default;
	explicit stack(size_type n):myctr(n){
	}
	stack(size_type n, const value_type& value):myctr(n, value){
	}
	template <class IIter>
	stack(IIter first, IIter last)
		: c_(first, last)
	{
	}
	stack(std::initializer_list<T> llist):myctr(llist){
	}
	stack(const stack& s) :myctr(s.myctr) {
	}
	stack(const Container& c) :myctr(c) {
	}

	stack& operator=(const stack& s) {
		myctr = s.myctr;
		return *this;
	}

	stack& operator=(std::initializer_list<T> llist) {
		myctr = llist;
		return *this;
	}

	~stack() = default;

	reference top() {
		return myctr.back();
	}
	const_reference top() const {
		return myctr.back();
	}

	bool empty() const noexcept { 
		return myctr.empty(); 
	}
	size_type size() const noexcept { 
		return myctr.size(); 
	}
	void push(const value_type& value)
	{
		myctr.push_back(value);
	}
	void pop() {
		myctr.pop_back();
	}
	void clear()
	{
		while (!empty())
			pop();
	}
	void swap(stack& rhs) noexcept(noexcept(ministl::swap(myctr, rhs.myctr)))
	{
		ministl::swap(myctr, rhs.myctr);
	}
};

//运算符重载
template <class T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return lhs == rhs;
}

template <class T, class Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return lhs < rhs; 
}

template <class T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return !(lhs == rhs);
}

template <class T, class Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
	return !(lhs < rhs);
}
}//!ministl

#endif
