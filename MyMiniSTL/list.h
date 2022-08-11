#ifndef __MYMINISTL_LIST_H__
#define __MYMINISTL_LIST_H__

#include <initializer_list>
#include "iterator.h"
#include "allocator.h"
#include "alloc.h"
#include "util.h"
#include "uninitialized.h"

namespace ministl
{
//结点设计
template <class T>
struct _list_node {
	typedef void* void_pointer;
	void_pointer prev;
	void_pointer next;
	T data;
};

//迭代器设计 
template <class T>
struct _list_iterator: public ministl::iterator<ministl::bidirectional_iterator_tag, T> {
	typedef _list_iterator<T> self;

	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef _list_node<T>* link_type;
	typedef size_t size_type;

	//typedef _list_iterator<T> self;
	link_type node;

	//构造函数
	_list_iterator() = default;
	_list_iterator(link_type x) :node(x) {}
	_list_iterator(const _list_iterator& x): node(x.node) {}

	//运算符重载
	bool operator==(const self& x) const { return node == x.ndoe; }
	bool operator!=(const self& x) const { return node != x.node; }
	
	reference operator*() const { return this->node->data; }
	pointer operator->()const { return &(operator()); }

	self& operator++() {
		node = node->next;
		return *this;
	}

	self& operator++(int) {
		self tmp = this;
		++*this;//先调用*运算符重载，再调用前置++运算符重载
		return tmp;
	}

	self& operator--() {
		node = node->next;
		return *this;
	}
	self& operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}

};

template <class T, class Alloc = alloc>
class list {
protected:
	typedef _list_node<T> list_node;
	typedef simple_alloc<list_node, Alloc> list_node_allocator;
	typedef _list_iterator<T> iterator;
	typedef const _list_iterator<T> const_iterator;

	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
public:
	typedef list_node* link_type;
protected:
	link_type node;

protected:
	//配置一个结点
	link_type get_node() { return list_node_allocator::allocate(); }

	//释放一个结点
	void put_node(link_type p) { list_node_allocator::deallocate(p); }

	//产生(配置并构造)一个结点，带有元素值
	link_type create_node(const T& x) {
		link_type p = get_node();
		ministl::construct(&p->data, x);
		return p;
	}
	//销毁（析构并释放一个结点）
	void destroy_node(link_type p) {
		ministl::destroy(&p->data);
		put_node(p);
	}

	//配置一个空链表
	void empty_initialize() {
		node = get_node();
		node->next = node;
		node->prev = node;
	}


	//配置n个node
	void fill_init(size_type n, const value_type& x);
	//用范围来初始化容器
	template <class Iterator>
	void copy_init(Iterator first, Iterator last);
	//用n个元素赋值
	void fill_assign(size_type n, const value_type& value);
	//拷贝的形式来赋值
	template <class Iterator>
	void copy_assign(Iterator first, Iterator last);
	//插入元素
	iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
	//拷贝插入元素
	template <class Iterator>
	iterator copy_insert(const_iterator pos, size_type n, Iterator first);

public:
	//构造、复制、析构
	list() {
		empty_initialize();
	}
	explicit list(size_type n) {
		fill_init(n, value_type());
	}
	list(size_type n, const T& value)
	{
		fill_init(n, value);
	}

	template<class Iterator>
	list(Iterator first, Iterator last) {
		copy_init(first, last);
	}
	list(std::initializer_list<T> ilist)
	{
		copy_init(ilist.begin(), ilist.end());
	}
	list(const list& rhs)
	{
		copy_init(rhs.cbegin(), rhs.cend());
	}
	list& operator=(const list& rhs) {
		if (this != &rhs)
		{
			assign(rhs.begin(), rhs.end());
		}
		return *this;
	}
	list& operator=(std::initializer_list<T> ilist)
	{
		list tmp(ilist.begin(), ilist.end());
		swap(tmp);
		return *this;
	}
	~list()
	{
		if (node)
		{
			clear();
			list_node_allocator::deallocate(node);
			node_ = nullptr;
		}
	}

	//迭代器操作
	iterator begin() { return (link_type)((*node).next); }
	const_iterator cbegin() { return begin(); }
	iterator end() { return node; }
	const_iterator cend() { return end(); }
	bool empty() const { return node->next == node; }
	size_type size() const {
		size_type result = 0;
		distance(begin(), end(), result);
		return result;
	}
	size_type max_size() const noexcept
	{
		return static_cast<size_type>(-1);
	}
	reference front() { return *begin(); }
	reference back() { return *(--end()); }
	void swap(list& rhs) noexcept { ministl::swap(node, rhs.node); }
	void clear();
	void push_back(const T& x) { insert(end(), x); }
	void push_front(const T& x) { insert(begin(), x); }
	void pop_front() { erase(begin()); }
	void pop_back() { iterator tmp = end(); erase(--tmp); }
	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);
	iterator insert(iterator pos, const value_type& value);
	iterator insert(iterator pos, size_type n, const value_type& value);
	template<class Iterator>
	iterator insert(iterator pos, Iterator first, Iterator last);
	void remove(const T& value);
	//移除数值相同的连续元素，只有连续而相同的元素，才会被移除剩一个
	void unique();
	iterator find(const T& value);
	void transfor(iterator position, iterator first, iterator last);
	void splice(iterator pos, list& s);
	void splice(iterator pos, list&, iterator it);
	void splice(iterator pos, list&, iterator first, iterator last);
	void merge(list<T, Alloc>& x);
	void reverse();
	void sort();
	void resize(size_type, const value_type&);
	template<class U>
	void remove_if(U pred);
	//赋值操作assign()
	template <class Iterator>
	void assign(Iterator first, Iterator last) { copy_assign(first, last); };
	void assign(size_type n, const value_type& value) { fill_assign(n, value); }
	void assign(std::initializer_list<T> ilist) { copy_assign(ilist.begin(), ilist.end()); }
};

//***************************remove_if()**************************
template <class T,class Alloc>
template <class U>
void list<T,Alloc>::remove_if(U pred)
{
	link_type f = begin();
	link_type l = end();
	for (auto next = f; f != l; f = next)
	{
		++next;
		if (pred(*f))
		{
			erase(f);
		}
	}
}

//*****************************resize()**************************
template <class T,class Alloc>
void list<T,Alloc>::resize(size_type new_size, const value_type& value)
{
	link_type i = begin();
	size_type len = 0;
	while (i != end() && len < new_size)
	{
		++i;
		++len;
	}
	if (len == new_size)
	{
		erase(i, node);
	}
	else
	{
		insert(node, new_size - len, value);
	}
}

//*****************************sort()*******************************
template <class T,class Alloc>
void list<T, Alloc>::sort() {
	if (node->next == node || link_type(node->next)->next == node)
		return;

	list<T, Alloc> carr;
	list<T, Alloc> counter[64];
	int fill = 0;
	while (!empty()) {
		carr.splice(carr.begin, *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty()) {
			counter[i].merge(carr);
			carr.swap(counter[i++]);
		}
		carr.swap(counter[i]);
		if (i == fill) ++fill;
	}

	for (int i = 1; i < fill; ++i) {
		counter[i].merge(counter[i - 1]);
	}
	swap(counter[fill - 1]);
}

//****************************reverse()*******************************
template<class T,class Alloc>
void list<T, Alloc>::reverse(){
	if (node->next == node || link_type(node->next)->next == node)
		return;
	iterator first = begin();
	++first;
	while (first != end()) {
		iterator old = first;
		++first;
		transfor(begin(), old, first);
	}
}

//*****************************merge()********************************
template<class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();
	while (first1 != last1 && first2 != last2) {
		if (*first2 < *first1) {
			iterator next = first2;
			transfor(first, first, ++next);
			first2 = next;
		}
		else {
			++first1;
		}
		if (first2 != last2) transfor(last1, first2, last2);
	}
}

//******************************splice()*******************************
template<class T,class Alloc>
void list<T, Alloc>::splice(iterator pos, list& s) {
	if (!s.empty())
		transfor(pos, s.begin(), s.end());
}
template<class T,class Alloc>
void list<T,Alloc>::splice(iterator pos, list&, iterator it) {
	iterator i = it;
	++i;
	if (pos == i || pos == it) return;
	transfor(pos, it, i);
}
template <class T,class Alloc>
void list<T, Alloc>::splice(iterator pos, list&, iterator first, iterator last) {
	if (first != last) {
		transfor(pos, first, last);
	}
}

//******************************transfor()***************************
template <class T,class Alloc>
void list<T, Alloc>::transfor(iterator position, iterator first, iterator last) {
	if (position != last) {
		//修正每个结点的next域
		last.node->prev->next = position.node;
		first.node->prev->next = last.node;
		position.node->prev->next = first.node;
		//修正每个结点的prev域
		iterator tmp = position;
		position.node->prev = last.node->prev;
		last.node->prev = first.node->prev;
		first.node->prev= tmp.node->prev;
	}
}

//******************************find()*******************************
template<class T,class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::find(const T& value) {
	iterator first = begin();
	iterator last = end();
	while (first != last) {
		if (*first == value) {
			return first;
		}
		++first;
	}
	return last;
}

//******************************unique()*****************************
template<class T,class Alloc>
void list<T, Alloc>::unique() {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;
	iterator tmp = first;
	while (++tmp != end())
	{
		if (*tmp == *first) {
			erase(tmp);
		}
		else {
			first = tmp;
		}
		tmp = first;
	}
}

//******************************remove()****************************
template <class T,class Alloc>
void list<T, Alloc>::remove(const T& value) {
	iterator first = begin();
	iterator last = end();
	while (first != last) {
		if (*first == value) {
			iterator tmp = first;
			++tmp;
			if (*first == value) {
				erase(first);
			}
			first = tmp;
		}
	}
}

//******************************insert()***************************
template <class T,class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator pos, const value_type& value) {
	link_type tmp = create_node(value);
	tmp->next = pos.node;
	tmp->prev = pos.node->prev;
	(link_type(pos.node->prev))->next = tmp;
	pos.node->prev = tmp;
	return tmp;
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator pos, size_type n, const value_type& value) {
	return fill_insert(pos, n, value);
}

template<class T,class Alloc>
template<class Iterator>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator pos, Iterator first, Iterator last) {
	size_type n = ministl::distance(first, last);
	return copy_insert(pos, n, first);
}

//******************************earse()**************************
template <class T, class Alloc>
typename list<T,Alloc>::iterator list<T, Alloc>::erase(iterator pos) {
	link_type tmp = pos->node;
	link_type tnext = tmp->next;
	tmp->prev->next = tnext;
	tnext->prev = tmp->prev;
	destroy_node(pos->node);

	return iterator(tnext);
}
template <class T, class Alloc>
typename list<T,Alloc>::iterator list<T,Alloc>::erase(iterator first, iterator last) {
	if (first != last) {
		first->prev->next = last->next;
		last->next->prev = first->prev;
		while (first != last) {
			link_type tmp = first.node;
			++first;
			destroy_node(tmp);
		}
	}
	else {
		erase(first);
	}
	return iterator(last);
}

//******************************fill_insert()***************************
template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::fill_insert(const_iterator pos, size_type n, const value_type& value) {

	iterator tmp(pos.node);

	for (; n > 0; --n)
	{
		iterator tmp1 = tmp;
		link_type tnext = create_node(value);
		tnext->next = tmp1.node;
		tnext->prev = tmp1.node->prev;
		tmp1.node->prev->next = tnext;
	}
	return tmp;
}

//*********************************fill_init()********************
template <class T, class Alloc>
void list<T, Alloc>::fill_init(size_type n, const value_type& x) {
	link_type mnode = node;
	for (; n > 0; --n) {
		link_type tmp = create_node(x);
		tmp->prev = node;
		tmp->next = node->next;
		node->next = tmp;
		node = node->next;
		node->prev = mnode;
	}
	node = mnode;
}

//*******************************fill_assign()***********************
template<class T,class Alloc>
void list<T, Alloc>::fill_assign(size_type n, const value_type& value) {
	auto i = begin();
	auto e = end();
	for (; n > 0 && i != e; --n, ++i)
	{
		*i = value;
	}
	if (n > 0)
	{
		insert(e, n, value);
	}
	else
	{
		erase(i, e);
	}
}

//******************************copy_insert()***************************
template<class T,class Alloc>
template<class Iterator>
list<T, Alloc>::iterator list<T, Alloc>::copy_insert(const_iterator pos, size_type n, Iterator first) {
	iterator tmp(pos.node);

	for (; n > 0; --n, ++first)
	{
		iterator tmp1 = tmp;
		link_type tnext = create_node(*first);
		tnext->next = tmp1.node;
		tnext->prev = tmp1.node->prev;
		tmp1.node->prev->next = tnext;
	}
	return tmp;
}

//******************************copy_assign()*************************
template<class T, class Alloc>
template <class Iterator>
void list<T, Alloc>::copy_assign(Iterator first, Iterator last) {
	auto f1 = begin();
	auto l1 = end();
	auto f2 = first;
	auto l2 = last;
	for (; f1 != l1 && f2 != l2; ++f1, ++f2)
	{
		*f1 = *f2;
	}
	if (f2 == l2)
	{
		erase(f1, l1);
	}
	else
	{
		insert(l1, f2, l2);
	}
}

//*******************************copy_init()************************
template<class T,class Alloc>
template <class Iterator>
void list<T,Alloc>::copy_init(Iterator first, Iterator last) {
	size_type n = ministl::distance(first, last);
	link_type mnode = node;
	for (; n > 0; --n) {
		link_type tmp = create_node(*first);
		++first;
		node->next = tmp;
		node->prev = mnode;
		node = node->next;
	}
	node = mnode;
}

//*******************************clear()********************************
template <class T,class Alloc>
void list<T,Alloc>::clear()
{
	if (size() != 0)
	{
		link_type cur = node->next;
		for (link_type next = cur->next; cur != node; cur = next, next = cur->next)
		{
			destroy_node(cur);
		}
		node->next = node;
		node->prev = node;
	}
}

//*********************************运算符重载***********************************
template <class T>
bool operator==(const list<T>& lhs, const list<T>& rhs)
{
	auto f1 = lhs.cbegin();
	auto f2 = rhs.cbegin();
	auto l1 = lhs.cend();
	auto l2 = rhs.cend();
	for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2)
		;
	return f1 == l1 && f2 == l2;
}

template <class T>
bool operator<(const list<T>& lhs, const list<T>& rhs)
{
	return ministl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <class T>
bool operator!=(const list<T>& lhs, const list<T>& rhs)
{
	return !(lhs == rhs);
}

template <class T>
bool operator>(const list<T>& lhs, const list<T>& rhs)
{
	return rhs < lhs;
}

template <class T>
bool operator<=(const list<T>& lhs, const list<T>& rhs)
{
	return !(rhs < lhs);
}

template <class T>
bool operator>=(const list<T>& lhs, const list<T>& rhs)
{
	return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T>
void swap(list<T>& lhs, list<T>& rhs) noexcept
{
	lhs.swap(rhs);
}

}//!ministl

#endif
