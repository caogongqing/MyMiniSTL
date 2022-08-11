#ifndef __MYMINISTL_HASHTABLE_H__
#define __MYMINISTL_HASHTABLE_H__

#include <initializer_list>

#include "algobase.h"
#include "functional.h"
#include "alloc.h"
#include "allocator.h"
#include "vector.h"
#include "util.h"

namespace ministl
{
// hashtable 的节点定义
template <class T>
struct hashtable_node
{
	hashtable_node* next;   // 指向下一个节点
	T value;  // 储存实值

	hashtable_node() = default;
	hashtable_node(const T& n) :next(nullptr), value(n) {}

	hashtable_node(const hashtable_node& node) :next(node.next), value(node.value) {}
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable;

static const int stl_num_primes = 28;
static const unsigned long stl_prime_list[stl_num_primes] =
{
	53,97,193,389,769,
	1543,3079,6151,12289,24593,
	49157,98317,196613,393241,786433,
	1572869,3145739,6291469,12582917,
	25165843,50331653,100663319,201326611,402653189,
	805306457,1610612741,3221225476ul,4294967291ul
};

inline unsigned long stl_next_prime(unsigned long n) {
	const unsigned long* first = stl_prime_list;
	const unsigned long* last = stl_prime_list + stl_num_primes;
	//作用与红黑树中的lower_bound()类似，即找大于first小于last的那个数
	const unsigned long* pos = lower_bound(first, last, n);

	return pos == last ? *(last - 1) : *pos;
}

// value traits
template <class T, bool>
struct ht_value_traits_imp
{
	typedef T key_type;
	typedef T mapped_type;
	typedef T value_type;

	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value;
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value;
	}
};

template <class T>
struct ht_value_traits_imp<T, true>
{
	typedef typename std::remove_cv<typename T::first_type>::type key_type;
	typedef typename T::second_type mapped_type;
	typedef T value_type;

	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value.first;
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value;
	}
};

template <class T>
struct ht_value_traits
{
	static constexpr bool is_map = ministl::is_pair<T>::value;

	typedef ht_value_traits_imp<T, is_map> value_traits_type;

	typedef typename value_traits_type::key_type    key_type;
	typedef typename value_traits_type::mapped_type mapped_type;
	typedef typename value_traits_type::value_type  value_type;

	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value_traits_type::get_key(value);
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value_traits_type::get_value(value);
	}
};

//hash table的迭代器的定义
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator {
	typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
	typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
	typedef const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
	typedef hashtable_node<Value> node;

	typedef ministl::forward_iterator_tag iterator_category;
	typedef Value value_type;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef Value& reference;
	typedef Value* pointer;

	node* cur;//迭代器所指向的结点
	hashtable* ht;//保持对容器的连结关系

	hashtable_iterator() = default;
	hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) { }
	reference operator*() const {
		return cur->value;
	}

	pointer operator->()const {
		return &(operator*());
	}

	iterator& operator++();
	iterator operator++(int);

	bool operator==(const iterator& it) const { return cur == it.cur; }
	bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template<class V, class K, class HF, class ExK, class EqK, class A>
hashtable_iterator<V, K, HF, ExK, EqK, A>& 
hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {
	const node* old = cur;
	cur = cur->next;
	//如果cur等于空就跳到下一个链表中去
	if (!cur) {
		size_type bucket = ht->bkt_num(old->value);
		while (!cur && ++bucket < ht->buckets.size())
			cur = ht->buckets[bucket];
	}

	return *this;
}

template<class V, class K, class HF, class ExK, class EqK, class A>
inline hashtable_iterator<V, K, HF, ExK, EqK, A>
hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
	iterator tmp = *this;
	++* this;
	return tmp;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {

private:
	typedef ht_value_traits<Value> value_traits;//元素萃取
	typedef Value value_type;
	typedef Key key_type;
	typedef typename value_traits::mapped_type mapped_type;

	typedef HashFcn hasher;//散列函数
	typedef EqualKey key_equal;//判断键值是否相等
	typedef size_t size_type;
	typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;//迭代器
	typedef const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;

	hasher hash;
	key_equal equals;
	ExtractKey get_key;
	typedef hashtable_node<Value> node;
	typedef node* node_ptr;
	typedef simple_alloc<node, Alloc> node_allocator;

	typedef typename node_allocator::pointer            pointer;
	typedef typename node_allocator::const_pointer      const_pointer;
	typedef typename node_allocator::reference          reference;
	typedef typename node_allocator::const_reference    const_reference;
	typedef typename node_allocator::size_type          size_type;
	typedef typename node_allocator::difference_type    difference_type;

	vector<node*, Alloc> buckets;
	size_type num_elements;

private:
	bool is_equal(const key_type& key1, const key_type& key2)
	{
		return EqualKey(key1, key2);
	}

	bool is_equal(const key_type& key1, const key_type& key2) const
	{
		return EqualKey(key1, key2);
	}

	const_iterator M_cit(node_ptr node) const noexcept
	{
		return const_iterator(node, const_cast<hashtable*>(this));
	}

	iterator M_begin() noexcept
	{
		for (size_type n = 0; n < num_elements; ++n)
		{
			if (buckets[n])  // 找到第一个有节点的位置就返回
				return iterator(buckets[n], this);
		}
		return iterator(nullptr, this);
	}


	const_iterator M_begin() const noexcept
	{
		for (size_type n = 0; n < bucket_size; ++n)
		{
			if (buckets[n])  // 找到第一个有节点的位置就返回
				return M_cit(buckets[n]);
		}
		return M_cit(nullptr);
	}

	size_type bkt_num_key(const key_type& key, size_t n) const {
		return hash(key) % n;
	}

	size_type bkt_num_key(const key_type& key) const {
		return bkt_num_key(key, buckets.size());
	}

	size_type bkt_num(const value_type& obj, size_t n) const {
		return bkt_num_key(get_key(obj), n);
	}

	size_type bkt_num(const value_type& obj) const {
		return bkt_num_key(get_key(obj));
	}


	void initialize_buckets(size_type n) {
		//next_size返回最接近n并大于n的质数
		const size_type n_buckets = next_size(n);
		//如果n_buckets大于当前容量的话就重建，小于的话就直接返回即可
		buckets.resize(n_buckets);
		buckets.insert(buckets.end(), n_buckets, (node*)0);
		num_elements = 0;
	}

	size_type next_size(size_type n) const {
		return stl_next_prime(n);
	}

	void copy_init(const hashtable& ht);

	pair<iterator, bool> insert_unique_noresize(const value_type& obj);
	iterator insert_equal_noresize(const value_type& obj);
	void erase_bucket(size_type n, node_ptr last);
	pair<iterator, iterator> equal_range_multi(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;
	pair<iterator, iterator> equal_range_unique(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

	template <class InputIter>
	void copy_insert_multi(InputIter first, InputIter last, ministl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_multi(ForwardIter first, ForwardIter last, ministl::forward_iterator_tag);
	template <class InputIter>
	void copy_insert_unique(InputIter first, InputIter last, ministl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_unique(ForwardIter first, ForwardIter last, ministl::forward_iterator_tag);


public:

	//构造函数
	hashtable(size_type n, const HashFcn& hf, const EqualKey& eql) :
		hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
		initialize_buckets(n);
	}

	hashtable(const hashtable& rhs)
		:hash(rhs.hash), equal(rhs.equal),get_key(rhs.get_key), num_elements(rhs.num_elements)
	{
		copy_init(rhs);
	}

	hashtable& operator=(const hashtable& rhs);

	~hashtable() { clear(); }

public:

	pair<iterator, bool> insert_unique(const value_type& obj) {
		resize(num_elements + 1);
		return insert_unique_noresize(obj);
	}

	iterator insert_equal(const value_type& obj) {
		resize(num_elements + 1);
		return insert_equal_noresize(obj);
	}
	template <class InputIter>
	void insert_multi(InputIter first, InputIter last)
	{
		copy_insert_multi(first, last, iterator_category(first));
	}

	template <class InputIter>
	void insert_unique(InputIter first, InputIter last)
	{
		copy_insert_unique(first, last, iterator_category(first));
	}

	size_type bucket_count() const {
		return buckets.size();
	}

	size_type max_bucket_count() const {
		return stl_prime_list[stl_num_primes - 1];
	}

	node* new_node(const value_type& obj) {
		node* n = node_allocator::allocate();
		n->next = nullptr;
		ministl::construct(&n->value, obj);
		return n;
	}

	void delete_node(node* n) {
		//先调用析构函数 再销毁空间
		ministl::destroy(&n->value);
		node_allocator::deallocate(n);
	}

	iterator begin() noexcept
	{
		return M_begin();
	}
	const_iterator begin() const noexcept
	{
		return M_begin();
	}
	iterator end() noexcept
	{
		return iterator(nullptr, this);
	}
	const_iterator end() const noexcept
	{
		return M_cit(nullptr);
	}

	const_iterator cbegin() const noexcept
	{
		return begin();
	}
	const_iterator cend() const noexcept
	{
		return end();
	}

	// 容量相关操作
	bool      empty()    const noexcept { return num_elements == 0; }
	size_type size()     const noexcept { return num_elements; }
	size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	void resize(size_type num_elements_hint);
	void erase(const_iterator position);
	void erase(const_iterator first, const_iterator last);
	size_type erase_multi(const key_type& key);
	size_type erase_unique(const key_type& key);
	void clear();
	void swap(hashtable& rhs) noexcept;

	void copy_from(const hashtable& ht);
	iterator find(const key_type& key) {
		//首先寻找落在哪个位置
		size_type n = bkt_num_key(key);
		node* first;
		//比对键值 如果相同就直接跳出返回即可
		for (first = buckets[n]; first && !equals(get_key(first->value)), key); first = first->next) {}
		return iterator(first, last);
	}

	size_type count(const key_type& key) const {
		//寻找落在哪个位置
		const size_type n = bkt_num_key(key);
		size_type result = 0;
		//键值相同的话则计数加1
		for (const node* cur = buckets[n]; cur; cur = cur->next) {
			if (equals(get_key(cur->value), key))
				++result;
		}
		return result;
	}
};

//************************copy_insert()**********************
template <class V, class K, class HF, class Ex, class Eq, class A>
template <class InputIter>
void hashtable<V, K, HF, Ex, Eq, A>::
copy_insert_multi(InputIter first, InputIter last, ministl::input_iterator_tag)
{
	resize(ministl::distance(first, last));
	for (; first != last; ++first)
		insert_equal_noresize(*first);
}

template <class V, class K, class HF, class Ex, class Eq, class A>
template <class ForwardIter>
void hashtable<V, K, HF, Ex, Eq, A>::
copy_insert_multi(ForwardIter first, ForwardIter last, ministl::forward_iterator_tag)
{
	size_type n = ministl::distance(first, last);
	resize(n);
	for (; n > 0; --n, ++first)
		insert_equal_noresize(*first);
}

template <class V, class K, class HF, class Ex, class Eq, class A>
template <class InputIter>
void hashtable<V, K, HF, Ex, Eq, A>::
copy_insert_unique(InputIter first, InputIter last, ministl::input_iterator_tag)
{
	resize(ministl::distance(first, last));
	for (; first != last; ++first)
		insert_unique_noresize(*first);
}

template <class V, class K, class HF, class Ex, class Eq, class A>
template <class ForwardIter>
void hashtable<V, K, HF, Ex, Eq, A>::
copy_insert_unique(ForwardIter first, ForwardIter last, ministl::forward_iterator_tag)
{
	size_type n = ministl::distance(first, last);
	resize(n);
	for (; n > 0; --n, ++first)
		insert_unique_noresize(*first);
}

//*************************operator()=****************
template <class V, class K, class HF, class Ex, class Eq, class A>
hashtable<V, K, HF, Ex, Eq, A>&
hashtable<V, K, HF, Ex, Eq, A>::
operator=(const hashtable& rhs)
{
	if (this != &rhs)
	{
		hashtable tmp(rhs);
		swap(tmp);
	}
	return *this;
}

//***************************swap()**********************************
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::
swap(hashtable& rhs) noexcept
{
	if (this != &rhs)
	{
		buckets.swap(rhs.buckets);
		ministl::swap(num_elements, rhs.num_elements);
	}
}

//********************************erase_buckets()***********************
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::
erase_bucket(size_type n, node_ptr last)
{
	auto cur = buckets[n];
	while (cur != last)
	{
		auto next = cur->next;
		destroy_node(cur);
		cur = next;
		--num_elements;
	}
	buckets[n] = last;
}

//*******************************erase()*********************************
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::
erase(const_iterator position)
{
	auto p = position.node;
	if (p)
	{
		const auto n = hash(value_traits::get_key(p->value));
		auto cur = buckets_[n];
		if (cur == p)
		{ // p 位于链表头部
			buckets[n] = cur->next;
			destroy_node(cur);
			--num_elements;
		}
		else
		{
			auto next = cur->next;
			while (next)
			{
				if (next == p)
				{
					cur->next = next->next;
					destroy_node(next);
					--num_elements;
					break;
				}
				else
				{
					cur = next;
					next = cur->next;
				}
			}
		}
	}
}

// 删除[first, last)内的节点
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::
erase(const_iterator first, const_iterator last)
{
	if (first.node == last.node)
		return;
	auto first_bucket = first.node
		? HF(value_traits::get_key(first.node->value))
		: num_elements;;
	auto last_bucket = last.node
		? HF(value_traits::get_key(last.node->value))
		: num_elements;
	if (first_bucket == last_bucket)
	{ // 如果在 bucket 在同一个位置
		erase_bucket(first_bucket, first.node, last.node);
	}
	else
	{
		erase_bucket(first_bucket, first.node, nullptr);
		for (auto n = first_bucket + 1; n < last_bucket; ++n)
		{
			if (buckets[n] != nullptr)
				erase_bucket(n, nullptr);
		}
		if (last_bucket != num_elements)
		{
			erase_bucket(last_bucket, last.node);
		}
	}
}

// 查找与键值 key 相等的区间，返回一个 pair，指向相等区间的首尾
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator,
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator>
	hashtable<V, K, HF, Ex, Eq, A>::
	equal_range_multi(const key_type& key)
{
	const auto n = HF(key);
	for (node_ptr first = buckets[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{ // 如果出现相等的键值
			for (node_ptr second = first->next; second; second = second->next)
			{
				if (!is_equal(value_traits::get_key(second->value), key))
					return ministl::make_pair(iterator(first, this), iterator(second, this));
			}
			for (auto m = n + 1; m < num_elements; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets[m])
					return ministl::make_pair(iterator(first, this), iterator(buckets[m], this));
			}
			return ministl::make_pair(iterator(first, this), end());
		}
	}
	return ministl::make_pair(end(), end());
}

template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator,
	typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator>
	hashtable<V, K, HF, Ex, Eq, A>::
	equal_range_multi(const key_type& key) const
{
	const auto n = HF(key);
	for (node_ptr first = buckets[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			for (node_ptr second = first->next; second; second = second->next)
			{
				if (!is_equal(value_traits::get_key(second->value), key))
					return ministl::make_pair(M_cit(first), M_cit(second));
			}
			for (auto m = n + 1; m < num_elements; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets[m])
					return ministl::make_pair(M_cit(first), M_cit(buckets_[m]));
			}
			return ministl::make_pair(M_cit(first), cend());
		}
	}
	return ministl::make_pair(cend(), cend());
}

template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator,
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator>
	hashtable<V, K, HF, Ex, Eq, A>::
	equal_range_unique(const key_type& key)
{
	const auto n = HF(key);
	for (node_ptr first = buckets[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			if (first->next)
				return ministl::make_pair(iterator(first, this), iterator(first->next, this));
			for (auto m = n + 1; m < num_elements; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets_[m])
					return ministl::make_pair(iterator(first, this), iterator(buckets[m], this));
			}
			return ministl::make_pair(iterator(first, this), end());
		}
	}
	return ministl::make_pair(end(), end());
}

template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator,
	typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator>
	hashtable<V, K, HF, Ex, Eq, A>::
	equal_range_unique(const key_type& key) const
{
	const auto n = HF(key);
	for (node_ptr first = buckets[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			if (first->next)
				return ministl::make_pair(M_cit(first), M_cit(first->next));
			for (auto m = n + 1; m < num_elements; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets_[m])
					return ministl::make_pair(M_cit(first), M_cit(buckets[m]));
			}
			return ministl::make_pair(M_cit(first), cend());
		}
	}
	return ministl::make_pair(cend(), cend());
}

// 删除键值为 key 的节点
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::size_type
hashtable<V, K, HF, Ex, Eq, A>::
erase_multi(const key_type& key)
{
	auto p = equal_range_multi(key);
	if (p.first.node != nullptr)
	{
		erase(p.first, p.second);
		return ministl::distance(p.first, p.second);
	}
	return 0;
}

template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::size_type
hashtable<V, K, HF, Ex, Eq, A>::
erase_unique(const key_type& key)
{
	const auto n = hash(key);
	auto first = buckets_[n];
	if (first)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			buckets_[n] = first->next;
			destroy_node(first);
			--size_;
			return 1;
		}
		else
		{
			auto next = first->next;
			while (next)
			{
				if (is_equal(value_traits::get_key(next->value), key))
				{
					first->next = next->next;
					destroy_node(next);
					--size_;
					return 1;
				}
				first = next;
				next = first->next;
			}
		}
	}
	return 0;
}

//*****************************copy_from()*****************************
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht) {
	//清空
	buckets.clear();
	//判断是否需要调整
	buckets.resize(ht.buckets.size());
	//从尾端开始插入n个元素，其值为nullptr
	buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
	for (size_type i = 0; i < ht.buckets.size(); ++i) {
		//复制vector中的每一个元素
		if (const node* cur = ht.buckets[i]) {
			node* copy = new_node(cur->value);
			buckets[i] = copy;
			//然后将vector中的每一个元素的每一个结点复制
			for (node* next = cur->next; next; cur = next, next = cur->next) {
				copy->next = new_node(next->value);
				copy = copy->next;
			}
		}
	}

	//改变调整后的大小
	num_elements = ht.num_elements;
}

//*****************************clear()**********************************
template<class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::clear() {
	for (size_type i = 0; i < buckets.size(); i++) {
		node* cur = buckets[i];
		//删除list中的每一个结点
		while (cur != 0) {
			node* next = cur->next;
			delete_node(cur);
			cur = next;
		}
		//令bucket内容为null指针
		buckets[i] = 0;
	}
	//令总结点个数为0
	num_elements = 0;
}

//*****************************insert_equal_noresize()****************************
template<class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::iterator hashtable<V, K ,HF, Ex, Eq, A>::
insert_equal_noresize(const value_type& obj) {
	//获取目标元素的位置
	const size_type n = bkt_num(obj);
	node* first = buckets[n];

	for (node* cur = first; cur; cur = cur->next) {
		//以下就是执行链表的插入操作
		if (equals(get_key(cur->value), get_key(obj))) {
			node* tmp = new_node(obj);
			tmp->next = cur->next;
			cur->next = tmp;
			++num_elements;
			return iterator(tmp, this);
		}
	}

	//如果我们没有找到与要插入的元素键值相等的元素 那么就直接将目标元素插入到链表头部即可
	node* tmp = new_node(obj);
	tmp->next = first;
	buckets[n] = tmp;
	++num_elements;
	return iterator(tmp, this);
}

//****************************insert_unique_noresie()***********************
template<class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj) {
	const size_type n = bkt_num(obj);
	node* first = buckets[n];
	//如果发现有键值相同的元素的话就直接返回
	for (node* cur = first; cur; cur = cur->next) {
		if (equals(get_key(cur->val), get_key(obj)))
			return pair<iterator, bool>(iterator(cur, this), false);
	}

	//进行插入操作在链表头部
	node* tmp = new_node(obj);
	tmp->next = first;
	buckets[n] = tmp;
	++num_elements;
	return pair<iterator, bool>(iterator(tmp, this), true);
}

//****************************resize()*************************
template<class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
	//通过元素个数和vector的大小进行比较，如果当前的元素个数大于vector的大小的话
	//我们就不进行操作直接返回即可，否则就需要重建vector
	const size_type old_n = buckets.size();
	if (n > old_n) {
		//创建新的vector
		vector<node*, A> tmp(n, (node*)0);
		for (size_type bucket = 0; bucket < old_n; ++bucket) {
			node* first = buckets[bucket];
			//将旧的vector上的每条链表上的内容拷贝到新vector上去
			while (first) {
				size_type new_bucket = bkt_num(first->value, n);
				buckets[bucket] = first->next;
				first->next = tmp[new_bucket];
				tmp[new_bucket] = first;
				first = buckets[bucket];
			}
			//将新旧vector交换
			buckets.swap(tmp);
		}
	}
}
//**************************copy_init（）************************
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn,ExtractKey, EqualKey, Alloc>::
copy_init(const hashtable& ht)
{
	bucket_size = 0;
	buckets.resize(ht.bucket_size);
	buckets.assign(ht.bucket_size, nullptr);
	try
	{
		for (size_type i = 0; i < ht.bucket_size; ++i)
		{
			node_ptr cur = ht.buckets[i];
			if (cur)
			{ // 如果某 bucket 存在链表
				auto copy = create_node(cur->value);
				buckets[i] = copy;
				for (auto next = cur->next; next; cur = next, next = cur->next)
				{  //复制链表
					copy->next = create_node(next->value);
					copy = copy->next;
				}
				copy->next = nullptr;
			}
		}
		bucket_size = ht.bucket_size;
		size = ht.size;
	}
	catch (...)
	{
		clear();
	}
}


}//!ministl

#endif
