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

//deque��BufSize
inline size_t deque_buf_size(size_t n, size_t sz) {
	//���������֣�����������Ҫ���ٵĴ�С��Ϊ0�Ļ�����ʹ�ô����n��Ϊ�������Ĵ�С
	//����������Ҫ���ٵĴ�СΪ0�Ļ�������Ҫ�洢��Ԫ����512Bytes�Ƚϣ����С��512Bytes��ʹ��512/n��ΪĬ�ϻ�����
	//��С�����򷵻�1
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

//deque_iterator������
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
	typedef T** map_pointer;//ָ���п���

	T* cur;//��������ǰԪ��λ��
	T* first;//ָ�򻺳�����ͷ��λ��
	T* last;//�˵�������ָ֮��������β���������ÿռ䣩
	map_pointer node;//ָ���п���

	// ���캯��
	deque_iterator():cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
	deque_iterator(const iterator& x):cur(x.cur), first(x.first), last(x.last), node(x.node){}
	deque_iterator(T* v, map_pointer m):cur(v) ,first(*m), last(*m + buffer_size()), node(m) {}
	deque_iterator(const const_iterator& rhs):cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){}

	//ת������һ��������
	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + difference_type(buffer_size());
	}
	//ʹ�����µ��������������ɻ����������ļ���
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
		//��ǰ����������Ǹ��������ľ��� + ��ǰ��㵽β���ľ��� + �����Ľ�㵽��β���ľ���
		return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
	}
	self& operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);//��ת����һ�������� ���趨��first�Լ�last��node
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
			set_node(node - 1);//��ת����һ�������� ���趨��first�Լ�last��node
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
			//�������һ���������Ļ���Ҳ����ζ����������������
			//����ǰ�����Ǻ��ˣ�node_offset��ʾ���ǲ���
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
	bool operator> (const self& rhs) const { return rhs < *this; }//����operator<��rhs��Ϊ��this��Ϊ��
	bool operator<=(const self& rhs) const { return !(rhs < *this); }//����ͬ��
	bool operator>=(const self& rhs) const { return !(*this < rhs); }//����ͬ��
};

//deque����
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
	size_type map_size;//map�е�ָ�����
protected:
	//�����п���map������Ĳ����������map�ĳߴ�
	map_pointer create_map(size_type size);
	//��������������ÿ��map�е�ָ�붼����һƬ�¿ռ�
	void create_buffer(map_pointer nstart, map_pointer nfinish);
	//���ٻ���������ÿ��map�е�ָ�붼�ͷ�һƬ�ռ�
	void destroy_buffer(map_pointer nstart, map_pointer nfinish);
	//������������ź�deque�Ľṹ
	void create_map_and_nodes(size_type num_elements);
	//���Ԫ�����ڳ�ʼ��
	void fill_init(size_type n, const value_type& value);
	//��������map��start�Լ�lastָ��
	void reallocate_map(size_type nodes_to_add, bool add_at_front);
	//����β������Ԫ�أ������ж����Ԫ��������һ��������
	void reserve_map_at_back(size_type nodes_to_add = 1);
	//����ͷ������Ԫ�أ���ת����һ��������
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
	//���죬���ƣ���������
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
			//���ͷ�ÿ�������� Ȼ�����ͷ�map�ϵ�Ԫ��
			data_allocator::deallocate(*start.node, deque_buf_size(BufSize, sizeof(T));
			*start.node = nullptr;
			map_allocator::deallocate(map, map_size);
			map = nullptr;
		}
	}

public:
	//��������
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
	difference_type index = pos - start;//�����֮ǰ��Ԫ�ظ���
	value_type x_copy = x;
	if (index < size() / 2) {//��������֮ǰ��Ԫ�ظ����Ƚ���
		push_front(front());//�ڲ����֮ǰ��Ԫ�ظ����Ƚ��� ����ǰ�˼����ڵ�һԪ��ֵͬ��Ԫ��
		iterator front1 = start;//���±�ʶ�Ǻţ�Ȼ�����Ԫ���ƶ�
		++front1;
		iterator front2 = front1;
		++front2;
		pos = start + index;
		iterator pos1 = pos;
		++pos1;
		copy(front2, pos1, front1);//Ԫ���ƶ�
	}
	else {//�����֮���Ԫ�ظ����Ƚ���
		push_back(back());//����β�˼��������Ԫ��ֵͬ��Ԫ��
		iterator back1 = finish;//���±�ʶ�Ǻţ�Ȼ�����Ԫ���ƶ�
		--back1;
		iterator back2 = back1;
		--back2;
		pos = start + index;
		std::copy_backward(pos, back2, back1);//Ԫ���ƶ�
	}
	*pos = x_copy;//�ڲ�������趨��ֵ
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
	//�ڽ���map����ĵ�����ʱ���ж��������ͷ�ı߸���һЩ���������ǶԽ�����ͷ���в��뿽���Ȳ����Ļ�Ч�ʻ��һЩ
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
		//�����������������deque
		clear();
		//ֱ�ӵ���clear
		return finish;
	}
	else {
		difference_type n = last - first;//�������ĳ���
		difference_type elems_before = first - start;//�������ǰ����Ԫ�ظ���
		if (elems_before < (size() - n) / 2) {//���ǰ��Ԫ�رȽ���
			std::copy_backward(start, first, last);//����ƶ�Ԫ�أ�����������䣩
			iterator new_start = start + n;//���deque�������
			destroy(start, new_start);//�ƶ���ϣ��������Ԫ������
			for (map_pointer cur = start.node; cur < new_start.node; ++cur) {//���½�����Ļ������ͷ�
				date_allocator::deallocate(*cur, buffer_size());
			}
			start = new_start;
		}
		else {//����������󷽵�Ԫ�رȽ���
			copy(last, finish, first);//��ǰ�ƶ���Ԫ��
			iterator new_finish = finish - n;//���deque����β��
			destroy(new_finish, finish);//�ƶ���ϣ�������Ԫ������
			//���½�����Ļ������ͷ�
			for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			finish = new_finish;//����deque����β��
		}

		return start + elems_before;
	}
}

//**********************************clear()********************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
	//���ͷ�ÿ���ڵ� ���ͷ�ÿ���ڵ�Ļ�����
	for (map_pointer node = start.node + 1; node < finish.node; ++node) {
		// ��������������Ԫ�ض�����
		destroy(*node, *node + buffer_size());
		//�ͷŻ������ڴ�
		data_allocator::deallocate(*node, buffer_size());
	}

	if (start.node != finish.node) {
		//ֻ����ͷβ����������
		destroy(start.cur, start.last);
		//��ͷ��������Ŀǰ����Ԫ������
		destroy(finish.first, finish.cur);
		//��β��������Ŀǰ����Ԫ������
		data_allocator::deallocate(finish.first, buffer_size());
	}
	else {
		//��ʱ��Ψһ������������Ԫ������
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
		//++�����ʹ�õ���֮ǰ���صİ汾 ��������������
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
	//map��Ҫ�Ľڵ��� = Ԫ�ظ��� / ÿ�������������ɵ�Ԫ�ظ��� + 1
	size_type num_nodes = num_elements / buffer_size() + 1;

	//һ��mapָ��Ҫ��������㣬���ٰ˸������������ڵ�����2 ǰ���Ԥ��һ������ʱʹ��
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

	//��nstart�Լ�nfinishָ��map��ӵ��ȫ���Ľڵ�������ĵ����Ρ�
	map_pointer nstart = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_nodes - 1;
	try
	{
		//Ϊÿ���ڵ㴴��������
		create_buffer(nstart, nfinish);
	}
	catch (...)
	{
		map_allocator::deallocate(map, map_size);
		map = nullptr;
		map_size = 0;
		throw;
	}
	//����startָ��ָ�����map������׸�ָ����׵�ַ��finishָ�������map��������һ��ָ���ĩβ��ַ
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

	//����map�����startָ�룬Ҳ���ǿ�ʼ��ָ��
	//����������������Ե����Ļ��ͼ�����������������¿��ٿռ��������
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
	//����map�����еĿ�ʼ�ͽ���ָ��
	start.set_node(new_nstart);
	finish.set_node(new_nstart + old_num_ndoes - 1);
}

//**************************reserve_map_at_back()**************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add) {
	//�ж���Ҫ��ӵĽڵ���Ŀ��map��ʣ��Ľڵ���Ŀ�Ƚϣ�������������ʣ�����Ļ����͵���map
	if (nodes_to_add + 1 > map_size - (finish.node - map))
		reallocate_map(nodes_to_add, false);
}

//**************************reserve_map_at_front()*************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add) {
	//�ж�β�˵Ľڵ��Ƿ��Ǵ���map��Ԥ���Ľڵ������������ڵĻ������¿��ٿռ�
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
		//�ܿ��ٴ��ڵ���β�����ڴ��Ԫ�صĻ����͸������һ���ڵ��ֵȻ��������һ��������
		construct(finish.cur, t_copy);
		finish.set_node(finish.node + 1);
		finish.cur = finish.first;
	}
	else {
		//�������ٵ�ǰ�ڵ�
		deallocate_node(*(finish.node + 1));
	}
}

//****************************push_front_aux()****************************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
	//���Ϻ�����������
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
	//����mapָ����ָ������һ���ڵ�
	deallocate_node(finish.first);
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	ministl::destroy(finish.cur);
}

//******************************pop_front_aux()*********************
template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
	//����mapָ����ָ���ͷһ���ڵ�
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

// ���رȽϲ�����
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

