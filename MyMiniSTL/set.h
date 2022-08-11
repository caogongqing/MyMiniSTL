#ifndef __MYMINISTL_SET_H__
#define __MYMINISTL_SET_H__

#include "rbtree.h"

namespace ministl
{
//******************************  set  ****************************
template <class Key, class Compare = less<Key>, class Alloc = alloc>
class set
{
public:
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;
private:

	typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
	rep_type t; //采用红黑树来表现set

public:
	typedef typename rep_type::const_pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::const_reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::const_iterator iterator;

	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::const_reverse_iterator reverse_iterator; 
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::differnece_type difference_type;

	//构造函数
	//在set容器中 一定使用的是rb_tree的insert_unique()而不是insert_equal()
	//multiset才使用rb_tree的insert_equal()
	//因为set不允许重复的键值存在
	set():t(Compare()){ }
	explicit set(const Compare& comp) : t(comp) {}

	template<class InputIterator>
	set(InputIterator first, InputIterator last) :t(Compare()){
		t.insert_unique(first, last);
	}

	template<class InputIterator>
	set(InputIterator first, InputIterator last, const Compare& comp):t(comp){
		t.insert_unique(first, last);
	}

	set(const set<Key, Compare, Alloc>& x):t(x.t) { }
	set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	//元素操作
	key_compare key_comp() const {
		return t.key_comp();
	}

	value_compare value_comp() const {
		return t.key_comp();
	}

	iterator begin() const {
		return t.begin();
	}

	iterator end() const {
		return t.end();
	}

	reverse_iterator rbegin() const {
		return t.rbegin();
	}

	reverse_iterator rend() const {
		return t.rend();
	}

	bool empty() const {
		return t.empty();
	}

	size_type size() const {
		return t.size();
	}

	size_type max_size() const {
		return t.max_size();
	}

	void swap(set<Key, Compare, Alloc>& x) {
		t.swap(x.t);
	}

	//insert/以及erase操作
	typedef pair<iterator, bool> pair_iterator_bool;
	pair<iterator, bool> insert(const value_type& x) {
		pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
		return pair<iterator, bool>(p.first, p.second);
	}

	iterator insert(iterator position, const value_type& x) {
		typedef typename rep_type::iterator rep_iterator;
		return t.insert_unique((rep_iterator&)position, x);
	}

	template<class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		t.insert_unique(first, last);
	}

	void erase(iterator position) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)position);
	}

	size_type erase(const key_type& x) {
		return t.erase(x);
	}

	void erase(iterator first, iterator last) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)first, (rep_iterator&)last);
	}

	void clear() {
		t.clear();
	}

	//set operations
	iterator find(const key_type& x) const {
		return t.find(x);
	}

	size_type count(const key_type& x) const {
		return t.size(x);
	}

	iterator lower_bound(const key_type& x) const {
		return t.lower_bound(x);
	}

	iterator upper_bound(const key_type& x) const {
		return t.upper_bound(x);
	}
	
	pair<iterator, iterator> equal_range(const key_type& x) const {
		return t.equal_range_unique(x);
	}

	friend bool operator==(const set& lhs, const set& rhs) { return lhs.t == rhs.t; }
	friend bool operator< (const set& lhs, const set& rhs) { return lhs.t < rhs.t; }
};
// 重载比较操作符
template <class Key, class Compare, class Alloc>
bool operator==(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return lhs == rhs;
}

template <class Key, class Compare, class Alloc>
bool operator<(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return lhs < rhs;
}

template <class Key, class Compare, class Alloc>
bool operator!=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template <class Key, class Compare, class Alloc>
bool operator>(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return rhs < lhs;
}

template <class Key, class Compare, class Alloc>
bool operator<=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template <class Key, class Compare, class Alloc>
bool operator>=(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs)
{
	return !(lhs < rhs);
}

//*******************************  multiset  ****************************
template <class Key, class Compare = less<Key>, class Alloc = alloc>
class multiset
{
public:
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;
private:

	typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
	rep_type t; //采用红黑树来表现set

public:
	typedef typename rep_type::const_pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::const_reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::const_iterator iterator;

	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::const_reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::differnece_type difference_type;

	//构造函数
	//在set容器中 一定使用的是rb_tree的insert_unique()而不是insert_equal()
	//multiset才使用rb_tree的insert_equal()
	//因为set不允许重复的键值存在
	multiset() :t(Compare()) { }
	explicit multiset(const Compare& comp) : t(comp) {}

	template<class InputIterator>
	multiset(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_equal(first, last);
	}

	template<class InputIterator>
	multiset(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_equal(first, last);
	}

	multiset(const multiset<Key, Compare, Alloc>& x) :t(x.t) { }
	multiset<Key, Compare, Alloc>& operator=(const multiset<Key, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	//元素操作
	key_compare key_comp() const {
		return t.key_comp();
	}

	value_compare value_comp() const {
		return t.key_comp();
	}

	iterator begin() const {
		return t.begin();
	}

	iterator end() const {
		return t.end();
	}

	reverse_iterator rbegin() const {
		return t.rbegin();
	}

	reverse_iterator rend() const {
		return t.rend();
	}

	bool empty() const {
		return t.empty();
	}

	size_type size() const {
		return t.size();
	}

	size_type max_size() const {
		return t.max_size();
	}

	void swap(set<Key, Compare, Alloc>& x) {
		t.swap(x.t);
	}

	//insert/以及erase操作
	typedef pair<iterator, bool> pair_iterator_bool;
	pair<iterator, bool> insert(const value_type& x) {
		pair<typename rep_type::iterator, bool> p = t.insert_equal(x);
		return pair<iterator, bool>(p.first, p.second);
	}

	iterator insert(iterator position, const value_type& x) {
		typedef typename rep_type::iterator rep_iterator;
		return t.insert_equal((rep_iterator&)position, x);
	}

	template<class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		t.insert_equal(first, last);
	}

	void erase(iterator position) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)position);
	}

	size_type erase(const key_type& x) {
		return t.erase(x);
	}

	void erase(iterator first, iterator last) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)first, (rep_iterator&)last);
	}

	void clear() {
		t.clear();
	}

	//set operations
	iterator find(const key_type& x) const {
		return t.find(x);
	}

	size_type count(const key_type& x) const {
		return t.size(x);
	}

	iterator lower_bound(const key_type& x) const {
		return t.lower_bound(x);
	}

	iterator upper_bound(const key_type& x) const {
		return t.upper_bound(x);
	}

	pair<iterator, iterator> equal_range(const key_type& x) const {
		return t.equal_range_multi(x);
	}

	friend bool operator==(const multiset& lhs, const multiset& rhs) { return lhs.t == rhs.t; }
	friend bool operator< (const multiset& lhs, const multiset& rhs) { return lhs.t < rhs.t; }
};
// 重载比较操作符
template <class Key, class Compare, class Alloc>
bool operator==(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return lhs == rhs;
}

template <class Key, class Compare, class Alloc>
bool operator<(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return lhs < rhs;
}

template <class Key, class Compare, class Alloc>
bool operator!=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template <class Key, class Compare, class Alloc>
bool operator>(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return rhs < lhs;
}

template <class Key, class Compare, class Alloc>
bool operator<=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template <class Key, class Compare, class Alloc>
bool operator>=(const multiset<Key, Compare, Alloc>& lhs, const multiset<Key, Compare, Alloc>& rhs)
{
	return !(lhs < rhs);
}

}//!ministl

#endif
