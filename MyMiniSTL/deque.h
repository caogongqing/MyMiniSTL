#ifndef __MYMINISTL_DEQUE_H__
#define __MYMINISTL_DEQUE_H__

#include <initializer_list>

#include "iterator.h"
#include "alloc.h"
#include "allocator.h"
#include "util.h"

namespace ministl
{
enum {
		initial_map_size = 8
};

//deque的BufSize
inline size_t deque_buf_size(size_t n, size_t sz) {
	//缓冲区划分：如果传入的想要开辟的大小不为0的话，就使用传入的n作为缓冲区的大小
	//如果传入的想要开辟的大小为0的话，就用要存储的元素与512Bytes比较，如果小于512Bytes，使用512/n作为默认缓冲区
	//大小，否则返回1
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

//deque_iterator迭代器
template<class T, class Ref, class Ptr, size_t BufSize>
struct deque_iterator {
	friend template <class T> struct deque_buf_size;

	typedef deque_iterator<T, T&, T*, BufSize> iterator;
	typedef deque_iterator<T, const T&, const T*, BufSize> const_iterator;
	typedef deque_iterator self;
	static size_t buffer_size() { return deque_buf_size(Bufsize); }

	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T** map_pointer;//指向中控器

	T* cur;//缓冲区当前元素位置
	T* first;//指向缓冲区的头部位置
	T* last;//此迭代器所指之缓冲区的尾部（含备用空间）
	map_pointer node;//指向中控器

	// 构造函数
	deque_iterator():cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
	deque_iterator(const iterator& x):cur(x.cur), first(x.first), last(x.last), node(x.node){}
	deque_iterator(T* v, map_pointer m):cur(v) ,first(*m), last(*m + buffer_size()), node(m) {}
	deque_iterator(const const_iterator& rhs):cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){}

	//转到另外一个缓冲区
	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + difference_type(buffer_size());
	}
	//使用以下的运算符重载来完成缓冲区连续的假象
	self& operator=(const iterator& rhs)
	{
		if (this != &rhs)
		{
			cur = rhs.cur;
			first = rhs.first;
			last = rhs.last;
			node = rhs.node;
		}
		return *this;
	}

	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); }
	difference_type operator-(const self& x) const {
		//当前到被减后的那个缓冲区的距离 + 当前结点到尾部的距离 + 被减的结点到它尾部的距离
		return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
	}
	self& operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);//跳转到下一个缓冲区 并设定其first以及last、node
			cur = first;
		}
		return *this;
	}
	self& operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}
	self& operator--() {
		if (cur == first) {
			set_node(node - 1);//跳转到下一个缓冲区 并设定其first以及last、node
			cur = last;
		}
		--cur;
		return *this;
	}
	self& operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}

	self& operator+=(difference_type n) {
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
			cur += n;
		else {
			//如果大于一个缓冲区的话，也就意味着跳入其他缓冲区
			//决定前进还是后退，node_offset表示的是步数
			difference_type node_offset = offset > 0 ? 
				offset / difference_type(buffer_size()) : 
				-difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n) const {
		self tmp = *this;
		return tmp += n;
	}
	self& operator-=(difference_type n) { return *this += (-n); }
	self operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;
	}
	reference operator[](difference_type n) const { return *(*this + n); }
	bool operator==(const self& x) const { return cur == x.cur; }
	bool operator!=(const self& x) const { return !(*this == x); }
	bool operator<(const self& x) const {
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
	bool operator> (const self& rhs) const { return rhs < *this; }//调用operator<，rhs变为左，this变为右
	bool operator<=(const self& rhs) const { return !(rhs < *this); }//与上同理
	bool operator>=(const self& rhs) const { return !(*this < rhs); }//与上同理
};

//deque容器
template<class T,class Alloc = alloc, size_t BufSize = 0>
class deque {
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

protected:
	typedef simple_alloc<value_type, Alloc> data_allocator;
	typedef simple_alloc<pointer, Alloc> map_allocator;

public:
	typedef deque_iterator<T, T&, T*, BufSize> iterator;
protected:
	typedef pointer* map_pointer;

	iterator start;
	iterator finish;
	map_pointer map;
	size_type map_size;//map中的指针个数
protected:
	//创建中控器map，传入的参数代表的是map的尺寸
	map_pointer create_map(size_type size);
	//创建缓冲区，对每个map中的指针都开辟一片新空间
	void create_buffer(map_pointer nstart, map_pointer nfinish);
	//销毁缓冲区，对每个map中的指针都释放一片空间
	void destroy_buffer(map_pointer nstart, map_pointer nfinish);
	//负责产生并安排好deque的结构
	void create_map_and_nodes(size_type num_elements);
	//填充元素用于初始化
	void fill_init(size_type n, const value_type& value);
	//扩充后调整map的start以及last指针
	void reallocate_map(size_type nodes_to_add, bool add_at_front);
	//调整尾部插入元素，假如有多余的元素跳到下一个缓冲区
	void reserve_map_at_back(size_type nodes_to_add = 1);
	//调整头部插入元素，跳转到上一个缓冲区
	void reserve_map_at_front(size_type nodes_to_add = 1);

	void push_back_aux(const value_type& t);
	void push_front_aux(const value_type& t);
	void pop_back_aux();
	void pop_front_aux();

	iterator insert_aux(iterator pos, const value_type& x);

	map_pointer allocate_node() {
		map_pointer mp = nullptr;
		nstart = map_allocator::allocate(buffer_size());
		return nstart;

	}
	void deallocate_node(map_pointer nstart) {
		destroy_buffer(nstart, nstart + buffer_size());
		nstart = nullptr;
	}

	template <class Iterator>
	void copy_init(Iterator, Iterator, input_iterator_tag);
	template <class Iterator>
	void copy_init(Iterator, Iterator, forward_iterator_tag);


public:
	//构造，复制，析构函数
	deque()
	{
		fill_init(0, value_type());
	}

	explicit deque(size_type n)
	{
		fill_init(n, value_type());
	}

	deque(size_type n, const value_type& value)
	{
		fill_init(n, value);
	}
	template <class IIter>
	deque(IIter first, IIter last)
	{
		copy_init(first, last, iterator_category(first));
	}
	deque(std::initializer_list<value_type> ilist)
	{
		copy_init(ilist.begin(), ilist.end(), ministl::forward_iterator_tag());
	}
	deque(const deque& rhs)
	{
		copy_init(rhs.begin(), rhs.end(), ministl::forward_iterator_tag());
	}
	deque& operator=(const deque& rhs);
	deque& operator=(std::initializer_list<value_type> ilist)
	{
		deque tmp(ilist);
		swap(tmp);
		return *this;
	}

	~deque()
	{
		if (map != nullptr)
		{
			clear();
			//先释放每个缓冲区 然后再释放map上的元素
			data_allocator::deallocate(*start.node, deque_buf_size(BufSize, sizeof(T));
			*start.node = nullptr;
			map_allocator::deallocate(map, map_size);
			map = nullptr;
		}
	}

public:
	//容器操作
	iterator begin() { return start; }
	iterator end() { return finish; }
	reference operator[](size_type n) {
		return start[difference_type(n)];
	}
	reference operator[](size_t n) {
		return start[difference_type(n)];
	}
	reference front() { return *start; }
	reference back() {
		iterator tmp = finish;
		--tmp;
		return *tmp;
	}
	size_type size() const { return finish - start; }
	size_type max_size() const { return size_type(-1); }
	bool empty() const { return finish == start; }
	void swap(deque& rhs);
	void push_back(const value_type& t) {
		if (finish.cur != finish.last - 1) {
			construct(finish.cur, t);
			++finish.cur;
		}
		else {
			push_back_aux(t);
		}
	}
	void push_front() {
		if (start.cur != start.first) {
			ministl::construct(start.cur - 1, t);
			--start.cur;
		}
		else {
			push_front_aux(t);
		}
	}
	void pop_back() {
		if (finish.cur != finish.first) {
			--finish.cur;
			ministl::destroy(finish.cur);
		}
		else {
			pop_back_aux();
		}
	}

	void pop_front() {
		if (start.cur != start.last - 1) {
			destroy(start.cur);
			++start.cur;
		}
		else {
			pop_front_aux();
		}
	}
	void clear();
	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);
	iterator insert(iterator position, const value_type& x);
};

template <class T, class Alloc, size_t BufSize>
deque<T, Alloc, BufSize>& deque<T, Alloc, BufSize>::operator=(const deque& rhs)
{
	if (this != &rhs)
	{
		const auto len = size();
		if (len >= rhs.size())
		{
			erase(ministl::copy(rhs.start, rhs.finish, start), finish);
		}
		else
		{
			iterator mid = rhs.begin() + static_cast<difference_type>(len);
			ministl::copy(rhs.start, mid, finish);
			insert(start, mid, rhs.finish);
		}
	}
	return *this;
}

//******************************insert_aux()******************************
template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
	difference_type index = pos - start;//插入点之前的元素个数
	value_type x_copy = x;
	if (index < size() / 2) {//如果插入点之前的元素个数比较少
		push_front(front());//在插入点之前的元素个数比较少 在最前端加入于第一元素同值的元素
		iterator front1 = start;//以下标识记号，然后进行元素移动
		++front1;
		iterator front2 = front1;
		++front2;
		pos = start + index;
		iterator pos1 = pos;
		++pos1;
		copy(front2, pos1, front1);//元素移动
	}
	else {//插入点之后的元素个数比较少
		push_back(back());//在最尾端加入与最后元素同值的元素
		iterator back1 = finish;//以下标识记号，然后进行元素移动
		--back1;
		iterator back2 = back1;
		--back2;
		pos = start + index;
		std::copy_backward(pos, back2, back1);//元素移动
	}
	*pos = x_copy;//在插入点上设定新值
	return pos;
}

//*********************************insert()****************************
template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, const value_type& x) {
	if (position.cur == start.cur) {
		push_front(x);
		return start;
	}
	else if (position.cur == finish.cur) {
		push_back(x);
		iterator tmp = finish;
		--tmp;
		return tmp;
	}
	else {
		return insert_aux(position, x);
	}
}

//***********************************erase()****************************
template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator pos) {
	iterator next = pos;
	++next;
	difference_type index = pos - start;
	//在进行map数组的调整的时候，判断其距离两头哪边更近一些，这样我们对近的那头进行插入拷贝等操作的话效率会高一些
	if (index < (size() >> 1)) {
		std::copy_backward(start, pos, next);
		pop_front();
	}
	else {
		copy(next, finish, pos);
		pop_back();
	}
	return start + index;
}

template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
	if (first == start && last == finish) {
		//如果清楚区间就是整个deque
		clear();
		//直接调用clear
		return finish;
	}
	else {
		difference_type n = last - first;//清楚区间的长度
		difference_type elems_before = first - start;//清楚区间前方的元素个数
		if (elems_before < (size() - n) / 2) {//如果前方元素比较少
			std::copy_backward(start, first, last);//向后移动元素（覆盖清楚区间）
			iterator new_start = start + n;//标记deque的新起点
			destroy(start, new_start);//移动完毕，将冗余的元素析构
			for (map_pointer cur = start.node; cur < new_start.node; ++cur) {//以下将冗余的缓冲区释放
				date_allocator::deallocate(*cur, buffer_size());
			}
			start = new_start;
		}
		else {//如果清除区间后方的元素比较少
			copy(last, finish, first);//向前移动后方元素
			iterator new_finish = finish - n;//标记deque的新尾点
			destroy(new_finish, finish);//移动完毕，将冗余元素析构
			//以下将冗余的缓冲区释放
			for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			finish = new_finish;//设置deque的新尾点
		}

		return start + elems_before;
	}
}

//**********************************clear()********************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
	//先释放每个节点 再释放每个节点的缓冲区
	for (map_pointer node = start.node + 1; node < finish.node; ++node) {
		// 将缓冲区的所有元素都析构
		destroy(*node, *node + buffer_size());
		//释放缓冲区内存
		data_allocator::deallocate(*node, buffer_size());
	}

	if (start.node != finish.node) {
		//只有有头尾两个缓冲区
		destroy(start.cur, start.last);
		//将头缓冲区的目前所有元素析构
		destroy(finish.first, finish.cur);
		//将尾缓冲区的目前所有元素析构
		data_allocator::deallocate(finish.first, buffer_size());
	}
	else {
		//此时将唯一缓冲区的所有元素析构
		destroy(start.cur, finish.cur);
	}

	finish = start;
}

//***********************************create_map()*****************************
template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::map_pointer deque<T, Alloc, BufSize>::create_map(size_type size) {
	map_pointer mp = nullptr;
	mp = map_allocator::allocate(size);
	for (size_type i = 0; i < size; ++i)
		*(mp + i) = nullptr;
	return mp;
}

//*******************************create_buffer()*********************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_buffer(map_pointer nstart, map_pointer nfinish) {
	map_pointer cur;
	try
	{
		//++运算符使用到了之前重载的版本 可以跳过缓冲区
		for (cur = nstart; cur <= nfinish; ++cur)
		{
			*cur = data_allocator::allocate(buffer_size());
		}
	}
	catch (...)
	{
		while (cur != nstart)
		{
			--cur;
			data_allocator::deallocate(*cur, buffer_size());
			*cur = nullptr;
		}
		throw;
	}
}

//********************************destroy_buffer()***************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_buffer(map_pointer nstart, map_pointer nfinish)
{
	for (map_pointer n = nstart; n <= nfinish; ++n)
	{
		data_allocator::deallocate(*n, buffer_size);
		*n = nullptr;
	}
}

//********************************create_map_and_nodes()**********************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
	//map需要的节点数 = 元素个数 / 每个缓冲区可容纳的元素个数 + 1
	size_type num_nodes = num_elements / buffer_size() + 1;

	//一个map指针要管理几个结点，最少八个，最多是所需节点数加2 前后各预留一个扩充时使用
	map_size = ministl::max(initial_map_size(), num_nodes + 2);
	try
	{
		map = create_map(map_size);
	}
	catch (...)
	{
		map = nullptr;
		map_size = 0;
		throw;
	}

	//令nstart以及nfinish指向map所拥有全部的节点的最中心的区段。
	map_pointer nstart = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_nodes - 1;
	try
	{
		//为每个节点创建缓冲区
		create_buffer(nstart, nfinish);
	}
	catch (...)
	{
		map_allocator::deallocate(map, map_size);
		map = nullptr;
		map_size = 0;
		throw;
	}
	//这里start指针指向的是map数组的首个指针的首地址，finish指向的则是map数组的最后一个指针的末尾地址
	start.set_node(nstart);
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + (num_elements % buffer_size());
}

//*************************fill_init()**********************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_init(size_type n, const value_type& value) {
	create_map_and_nodes(n);
	map_pointer cur;
	if (n != 0) {
		for (cur = start.node; cur < finish.node; ++cur)
			ministl::uninitialized_fill(*cur, *cur + buffer_size(), value);
		ministl::uninitialized_fill(finish.node, finish.cur, value);
	}
}

//****************************reallocate_map()****************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
	size_type old_num_nodes = finish.node - start.node + 1;
	size_type new_num_nodes = old_num_nodes + nodes_to_add;

	//调整map数组的start指针，也就是开始的指针
	//如果满足条件还可以调整的话就继续调整，否则就重新开辟空间用来存放
	map_pointer new_nstart;
	if (map_size > 2 * new_num_nodes) {
		new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		if (new_nstart < start.node)
			ministl::copy(start.node, finish.node + 1, new_nstart);
		else
			std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
	}
	else {
		size_type new_map_size = map_size + ministl::max(map_size, nodes_to_add) + 2;
		map_pointer new_map = map_allocator::allocate(new_map_size);
		new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
		copy(start.node, finish.node + 1, new_nstart);
		map_allocator::deallocate(map, map_size);

		map = new_map;
		map_size = new_map_size;
	}
	//调整map数组中的开始和结束指针
	start.set_node(new_nstart);
	finish.set_node(new_nstart + old_num_ndoes - 1);
}

//**************************reserve_map_at_back()**************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add) {
	//判断你要添加的节点数目与map中剩余的节点数目比较，如果添加量大于剩余量的话，就调整map
	if (nodes_to_add + 1 > map_size - (finish.node - map))
		reallocate_map(nodes_to_add, false);
}

//**************************reserve_map_at_front()*************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add) {
	//判断尾端的节点是否是大于map中预留的节点个数，如果大于的话就重新开辟空间
	if (nodes_to_add + 1 > start.node - map)
		reallocate_map(nodes_to_add, true);
}

//***************************push_back_aux()*************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
	value_type t_copy = t;
	reserve_map_at_back();
	*(finish.node + 1) = allocate_node();
	if (*(finish.node + 1) != nullptr) {
		//能开辟处节点在尾部用于存放元素的话，就更新最后一个节点的值然后跳到下一个缓冲区
		construct(finish.cur, t_copy);
		finish.set_node(finish.node + 1);
		finish.cur = finish.first;
	}
	else {
		//否则销毁当前节点
		deallocate_node(*(finish.node + 1));
	}
}

//****************************push_front_aux()****************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
	//与上函数功能类似
	value_type t_copy = t;
	reserve_map_at_front();
	*(start.node - 1) = allocate_node();
	if (*(start.node - 1) != nullptr) {
		start.set_node(start.node - 1);
		start.cur = start.last - 1;
		construct(start.cur, t_copy);
	}
	else {
		start.set_node(start.node - 1);
		start.cur = start.first;
		deallocate_node(*(start.node - 1));
	}
}

//***********************************pop_back_aux()*************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux() {
	//销毁map指针所指向的最后一个节点
	deallocate_node(finish.first);
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	ministl::destroy(finish.cur);
}

//******************************pop_front_aux()*********************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
	//销毁map指针所指向的头一个节点
	ministl::destroy(start.cur);
	deallocate_node(start.first);
	start.set_node(start.node + 1);
	start.cur = start.first;
}

//*****************************swap()****************************
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::swap(deque& rhs) noexcept
{
	if (this != &rhs)
	{
		ministl::swap(start, rhs.start);
		ministl::swap(finish, rhs.finish);
		ministl::swap(map, rhs.map);
		ministl::swap(map_size, rhs.map_size);
	}
}

//*******************************copy_init()********************************
template<class T, class Alloc, size_t BufSize>
template <class Iterator>
void deque<T, Alloc, BufSize>::copy_init(Iterator first, Iterator last, input_iterator_tag) {
	const size_type n = ministl::distance(first, last);
	create_map_and_nodes(n);
	for (; first != last; ++first)
		push_back(*first);
}

template<class T, class Alloc, size_t BufSize>
template <class Iterator>
void deque<T, Alloc, BufSize>::copy_init(Iterator first, Iterator last, forward_iterator_tag) {
	const size_type n = ministl::distance(first, last);
	create_map_and_nodes(n);
	for (auto cur = start.node; cur < finish.node; ++cur)
	{
		auto next = first;
		ministl::advance(next, buffer_size);
		ministl::uninitialized_copy(first, next, *cur);
		first = next;
	}
	ministl::uninitialized_copy(first, last, finish.first);
}

// 重载比较操作符
template <class T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs)
{
	return lhs.size() == rhs.size() &&
		ministl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs)
{
	return ministl::lexicographical_compare(
		lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
{
	return !(lhs == rhs);
}

template <class T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs)
{
	return rhs < lhs;
}

template <class T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs)
{
	return !(rhs < lhs);
}

template <class T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs)
{
	return !(lhs < rhs);
}

}//!ministl
#endif

