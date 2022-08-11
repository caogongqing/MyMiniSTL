#ifndef __MYMINISTL_MAP_H__
#define __MYMINISTL_MAP_H__

#include "rbtree.h"
namespace ministl
{
//*********************************  map  ******************************
template<class Key, class T, class Compare = less<Key>, class Alloc = alloc>
class map {
public:
	typedef Key key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef pair<const Key, T> value_type;
	typedef Compare key_compare;

	class value_compare : public binary_function<value_type, value_type, bool> {
		friend class map<Key, T, Compare, Alloc>;
	protected:
		Compare comp;
		value_compare(Compare c) : comp(c) { }
	public:
		bool operator()(const value_type& x, const value_type& y) const {
			return comp(x.first, y.first);
		}
	};

private:
	typedef rb_tree<key_type, value_type, key_type, key_compare, Alloc> rep_type;
	rep_type t;

public:
	typedef typename rep_type::pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::iterator iterator;

	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::difference_type difference_type;

	map() : t(Compare()) { }
	explicit map(const Compare& comp) : t(comp) { }

	template<class InputIterator>
	map(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_unique(first, last);
	}

	template<class InputIterator>
	map(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_unique(first, last);
	}

	map(const map<Key, T, Compare, Alloc>& x):t(x.t) { }
	map<Key, T, Compare, Alloc>& operator=(const map<Key, T, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	key_compare key_comp() const {
		return t.key_comp();
	}

	value_compare value_comp() const {
		return value_compare(t.key_comp());
	}

	iterator begin() {
		return t.begin();
	}

	const_iterator begin() const {
		return t.begin();
	}

	iterator end() {
		return t.end();
	}

	const_iterator end() const {
		return t.end();
	}

	reverse_iterator rbegin() {
		return t.rbegin();
	}

	const_reverse_iterator rbegin() const {
		return t.rbegin();
	}

	reverse_iterator rend() {
		return t.rend();
	}

	const_reverse_iterator rend() const {
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

	void swap(map<Key, T, Compare, Alloc>& x) {
		t.swap(x.t);
	}

	pair<iterator, bool> insert(const value_type& x) {
		return t.insert_unique(x);
	}

	iterator insert(iterator position, const value_type& x) {
		return t.insert_unique(first, last);
	}

	void erase(iterator position) {
		t.erase(position);
	}

	size_type erase(const key_type& x) {
		return t.erase(x);
	}

	void erase(iterator first, iterator last) {
		t.erase(first, last);
	}

	void clear() {
		t.clear();
	}

	iterator find(const key_type& x) {
		return t.find(x);
	}

	const_iterator find(const key_type& x) const {
		return t.find(x);
	}

	iterator lower_bound(const key_type& x) {
		return t.lower_bound(x);
	}

	const_iterator lower_bound(const key_type& x) const {
		return t.upper_bound(x);
	}

	iterator upper_bound(const key_type& x) {
		return t.upper_bound(x);
	}

	const_iterator upper_bound(const key_type& x) {
		return t.upper_bound(x);
	}

	pair<iterator, iterator> equal_range(const key_type& x) {
		return t.equal_range_unique(x);
	}

	pair<const_iterator, const_iterator> equal_range(const key_type& x) const{
		return t.equal_range_unique(x);
	}

	T& operator[](const key_type& k) {
		return (*((insert(value_type(k, T()))).first)).second;
	}

public:
	friend bool operator==(const map& lhs, const map& rhs) { return lhs.t == rhs.t; }
	friend bool operator< (const map& lhs, const map& rhs) { return lhs.t < rhs.t; }
};
// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
	return !(lhs < rhs);
}

//********************************  multimap  *******************************
template<class Key, class T, class Compare = less<Key>, class Alloc = alloc>
class multimap {
public:
	typedef Key key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef pair<const Key, T> value_type;
	typedef Compare key_compare;

	class value_compare : public binary_function<value_type, value_type, bool> {
		friend class multimap<Key, T, Compare, Alloc>;
	protected:
		Compare comp;
		value_compare(Compare c) : comp(c) { }
	public:
		bool operator()(const value_type& x, const value_type& y) const {
			return comp(x.first, y.first);
		}
	};

private:
	typedef rb_tree<key_type, value_type, key_type, key_compare, Alloc> rep_type;
	rep_type t;

public:
	typedef typename rep_type::pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::iterator iterator;

	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::difference_type difference_type;

	multimap() : t(Compare()) { }
	explicit multimap(const Compare& comp) : t(comp) { }

	template<class InputIterator>
	multimap(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_equal(first, last);
	}

	template<class InputIterator>
	multimap(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_equal(first, last);
	}

	multimap(const multimap<Key, T, Compare, Alloc>& x) :t(x.t) { }
	multimap<Key, T, Compare, Alloc>& operator=(const multimap<Key, T, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	key_compare key_comp() const {
		return t.key_comp();
	}

	value_compare value_comp() const {
		return value_compare(t.key_comp());
	}

	iterator begin() {
		return t.begin();
	}

	const_iterator begin() const {
		return t.begin();
	}

	iterator end() {
		return t.end();
	}

	const_iterator end() const {
		return t.end();
	}

	reverse_iterator rbegin() {
		return t.rbegin();
	}

	const_reverse_iterator rbegin() const {
		return t.rbegin();
	}

	reverse_iterator rend() {
		return t.rend();
	}

	const_reverse_iterator rend() const {
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

	void swap(multimap<Key, T, Compare, Alloc>& x) {
		t.swap(x.t);
	}

	pair<iterator, bool> insert(const value_type& x) {
		return t.insert_unique(x);
	}

	iterator insert(iterator position, const value_type& x) {
		return t.insert_unique(first, last);
	}

	void erase(iterator position) {
		t.erase(position);
	}

	size_type erase(const key_type& x) {
		return t.erase(x);
	}

	void erase(iterator first, iterator last) {
		t.erase(first, last);
	}

	void clear() {
		t.clear();
	}

	iterator find(const key_type& x) {
		return t.find(x);
	}

	const_iterator find(const key_type& x) const {
		return t.find(x);
	}

	iterator lower_bound(const key_type& x) {
		return t.lower_bound(x);
	}

	const_iterator lower_bound(const key_type& x) const {
		return t.upper_bound(x);
	}

	iterator upper_bound(const key_type& x) {
		return t.upper_bound(x);
	}

	const_iterator upper_bound(const key_type& x) {
		return t.upper_bound(x);
	}

	pair<iterator, iterator> equal_range(const key_type& x) {
		return t.equal_range_multi(x);
	}

	pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
		return t.equal_range_multi(x);
	}

	T& operator[](const key_type& k) {
		return (*((insert(value_type(k, T()))).first)).second;
	}

public:
	friend bool operator==(const multimap& lhs, const multimap& rhs) { return lhs.t == rhs.t; }
	friend bool operator< (const multimap& lhs, const multimap& rhs) { return lhs.t < rhs.t; }
};
// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
	return !(lhs < rhs);
}
}

#endif
