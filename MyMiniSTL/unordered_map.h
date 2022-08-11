#ifndef __MYMINISTL_UNORDERED_MAP_H__
#define __MYMINISTL_UNORDERED_MAP_H__

#include "hashtable.h"
namespace ministl
{
template<class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>, class Alloc = alloc>
class unordered_map
{
private:
	typedef hashtable<pair<const Key, T>, Key, HashFcn, select1st<pair<const Key, T>>, EqualKey, Alloc> ht;
	ht rep;

public:
	typedef typename ht::key_type key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef typename ht::value_type value_type;
	typedef typename ht::hasher hasher;
	typedef typename ht::key_equal key_equal;

	typedef typename ht::size_type size_type;
	typedef typename ht::difference_type difference_type;
	typedef typename ht::const_pointer pointer;
	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::const_reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::const_iterator iterator;
	typedef typename ht::const_iterator const_iterator;
	typedef typename ht::node_allocator node_allocator;

public:
	unordered_map() : rep(100, hasher(), key_equal()) { }
	explicit unordered_map(size_type n) : rep(n, hasher(), key_equal()) { }
	unordered_map(size_type n, const hasher& hf):rep(n, hf, key_equal()) { }
	unordered_map(size_type n, const hasher& hf, const key_equal& eql):rep(n, hf, eql) { }

	template<class InputIterator>
	unordered_map(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
		rep.insert_unique(f, l);
	}

	template<class InputIterator>
	unordered_map(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
		rep.insert_unique(f, l);
	}

	template<class InputIterator>
	unordered_map(InputIterator f, InputIterator l, size_type n, const hasher& hf)
		: rep(n, hf, key_equal()) {
		rep.insert_unique(f, l);
	}
	template<class InputIterator>
	unordered_map(InputIterator f, InputIterator l, size_type n, const hasher& hf, const key_equal& eql)
		: rep(n, hf, eql) {
		rep.insert_unique(f, l);
	}

public:
	size_type size() const {
		return rep.size();
	}

	size_type max_size() const {
		return rep.max_size();
	}

	bool empty() const {
		return rep.empty();
	}

	void swap(unordered_set& us) {
		rep.swap(us.rep);
	}

	friend bool operator==(const unordered_map& um1, const unordered_map& um2) {
		return um1 == um2;
	}
	mapped_type& at(const key_type& key)
	{
		iterator it = rep.find(key);
		if (it.node == nullptr) {
			cout << "unordered_map<Key, T> no such element exists" << endl;
			exit(0);
		}
		return it->second;
	}
	const mapped_type& at(const key_type& key) const
	{
		iterator it = rep.find(key);
		if (it.node == nullptr) {
			cout << "unordered_map<Key, T> no such element exists" << endl;
			exit(0);
		}
		return it->second;
	}

	mapped_type& operator[](const key_type& key)
	{
		iterator it = rep.find(key);
		if (it.node == nullptr)
			it = rep.insert_unique(key).first;
		return it->second;
	}

	iterator begin() const {
		return rep.begin();
	}
	iterator end() const {
		return rep.end();
	}

	const_iterator begin() const {
		return rep.begin();
	}

	const_iterator end() const {
		return rep.end();
	}

public:
	pair<iterator, bool> insert(const value_type& obj) {
		return rep.insert_unique(obj);
	}
	template <class InputIterator>
	void insert(InputIterator f, InputIterator l) {
		rep.insert_unique(f, l);
	}

	pair<iterator, bool> insert_noresize(const value_type& obj) {
		pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
		return pair<iterator, bool>(p.first, p.second);
	}

	iterator find(const key_type& key) const {
		return rep.find(key);
	}

	size_type count(const key_type& key) const {
		return rep.count(key);
	}

	pair<iterator, iterator> equal_range(const key_type& key) {
		return rep.equal_range(key);
	}
	
	pair<iterator, iterator> equal_range(const key_type& key) const {
		return rep.equal_range(key);
	}

	size_type erase(const key_type& key) {
		return rep.erase(key);
	}

	void erase(iterator it) {
		rep.erase(it);
	}

	void erase(iterator f, iterator l) {
		rep.erase(f, l);
	}

	void clear() { rep.clear(); }
public:
	void resize(size_type hint) {
		rep.resize(hint);
	}
	size_type bucket_count() const {
		return rep.bucket_count();
	}
	size_type max_bucket_count() const {
		return rep.max_bucket_count();
	}
	size_type elems_in_bucket(size_type n) const {
		return rep.elems_in_bucket(n);
	}
};

template<class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const unordered_map<Value, HashFcn, EqualKey, Alloc>& us1,
	const unordered_map<Value, HashFcn, EqualKey, Alloc>& us2) {
	return us1.rep == us2.rep;
}

template<class Value, class HashFcn, class EqualKey, class Alloc>
bool operator!=(const unordered_map<Value, HashFcn, EqualKey, Alloc>& us1,
	const unordered_map<Value, HashFcn, EqualKey, Alloc>& us2)
{
	return us1.rep != us2.rep;
}

// 重载 ministl 的 swap
template <class Value, class HashFcn, class EqualKey, class Alloc>
void swap(unordered_map<Value, HashFcn, EqualKey, Alloc>& us1,
	unordered_map<Value, HashFcn, EqualKey, Alloc>& us2)
{
	us1.swap(us2);
}


//****************************** unordered_mutlimap() **************************
template<class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>, class Alloc = alloc>
class unordered_multimap
{
private:
	typedef hashtable<pair<const Key, T>, Key, HashFcn, select1st<pair<const Key, T>>, EqualKey, Alloc> ht;
	ht rep;

public:
	typedef typename ht::key_type key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef typename ht::value_type value_type;
	typedef typename ht::hasher hasher;
	typedef typename ht::key_equal key_equal;

	typedef typename ht::size_type size_type;
	typedef typename ht::difference_type difference_type;
	typedef typename ht::const_pointer pointer;
	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::const_reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::const_iterator iterator;
	typedef typename ht::const_iterator const_iterator;
	typedef typename ht::node_allocator node_allocator;

public:
	unordered_multimap() : rep(100, hasher(), key_equal()) { }
	explicit unordered_multimap(size_type n) : rep(n, hasher(), key_equal()) { }
	unordered_multimap(size_type n, const hasher& hf) :rep(n, hf, key_equal()) { }
	unordered_multimap(size_type n, const hasher& hf, const key_equal& eql) :rep(n, hf, eql) { }

	template<class InputIterator>
	unordered_multimap(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
		rep.insert_equal(f, l);
	}

	template<class InputIterator>
	unordered_multimap(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
		rep.insert_equal(f, l);
	}

	template<class InputIterator>
	unordered_multimap(InputIterator f, InputIterator l, size_type n, const hasher& hf)
		: rep(n, hf, key_equal()) {
		rep.insert_equal(f, l);
	}
	template<class InputIterator>
	unordered_multimap(InputIterator f, InputIterator l, size_type n, const hasher& hf, const key_equal& eql)
		: rep(n, hf, eql) {
		rep.insert_equal(f, l);
	}

public:
	size_type size() const {
		return rep.size();
	}

	size_type max_size() const {
		return rep.max_size();
	}

	bool empty() const {
		return rep.empty();
	}

	void swap(unordered_set& us) {
		rep.swap(us.rep);
	}

	mapped_type& at(const key_type& key)
	{
		iterator it = rep.find(key);
		if (it.node == nullptr) {
			cout << "unordered_map<Key, T> no such element exists" << endl;
			exit(0);
		}
		return it->second;
	}
	const mapped_type& at(const key_type& key) const
	{
		iterator it = rep.find(key);
		if (it.node == nullptr) {
			cout << "unordered_map<Key, T> no such element exists" << endl;
			exit(0);
		}
		return it->second;
	}

	mapped_type& operator[](const key_type& key)
	{
		iterator it = rep.find(key);
		if (it.node == nullptr)
			it = rep.insert_equal(key).first;
		return it->second;
	}

	friend bool operator==(const unordered_multimap& um1,const unordered_multimap& um2) {
		return um1.rep.equal_range_unique(um2);
	}

	iterator begin() const {
		return rep.begin();
	}
	iterator end() const {
		return rep.end();
	}

	const_iterator begin() const {
		return rep.begin();
	}

	const_iterator end() const {
		return rep.end();
	}

public:
	pair<iterator, bool> insert(const value_type& obj) {
		return rep.insert_equal(obj);
	}
	template <class InputIterator>
	void insert(InputIterator f, InputIterator l) {
		rep.insert_equal(f, l);
	}

	pair<iterator, bool> insert_noresize(const value_type& obj) {
		pair<typename ht::iterator, bool> p = rep.insert_equal_noresize(obj);
		return pair<iterator, bool>(p.first, p.second);
	}

	iterator find(const key_type& key) const {
		return rep.find(key);
	}

	size_type count(const key_type& key) const {
		return rep.count(key);
	}

	pair<iterator, iterator> equal_range(const key_type& key) {
		return rep.equal_range(key);
	}

	pair<iterator, iterator> equal_range(const key_type& key) const {
		return rep.equal_range(key);
	}

	size_type erase(const key_type& key) {
		return rep.erase(key);
	}

	void erase(iterator it) {
		rep.erase(it);
	}

	void erase(iterator f, iterator l) {
		rep.erase(f, l);
	}

	void clear() { rep.clear(); }
public:
	void resize(size_type hint) {
		rep.resize(hint);
	}
	size_type bucket_count() const {
		return rep.bucket_count();
	}
	size_type max_bucket_count() const {
		return rep.max_bucket_count();
	}
	size_type elems_in_bucket(size_type n) const {
		return rep.elems_in_bucket(n);
	}
};

template<class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us1,
	const unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us2) {
	return us1.rep == us2.rep;
}

template<class Value, class HashFcn, class EqualKey, class Alloc>
bool operator!=(const unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us1,
	const unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us2)
{
	return us1.rep != us2.rep;
}

// 重载 ministl 的 swap
template <class Value, class HashFcn, class EqualKey, class Alloc>
void swap(unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us1,
	unordered_multimap<Value, HashFcn, EqualKey, Alloc>& us2)
{
	us1.swap(us2);
}

}

#endif
