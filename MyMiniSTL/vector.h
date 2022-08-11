#ifndef __MYMINISTL_VECTOR_H__
#define __MYMINISTL_VECTOR_H__

#include <initializer_list>
#include "iterator.h"
#include "allocator.h"
#include "alloc.h"
#include "util.h"
#include "uninitialized.h"

namespace ministl
{
template<class T,class Alloc = allocator<T>>
class vector {
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
protected:
	typedef simple_alloc<value_type, Alloc> data_allocator;
	iterator start;
	iterator finish;
	iterator end_of_storage;


	iterator allocate_fill(size_type size, const T& value);//��ʼ���ռ�
	void insert_aux(iterator position, const T& x);

	void deallocate();//���ٿռ�
	//���ٿռ䲢�����ֵ
	void fill_initialize(size_type n, const T& value); 
	//���ٿռ䲢�õ�������Χ�ڵ�Ԫ�ظ����ֵ
	template <class Iterator>
	void fill_initialize_range(Iterator fisrt, Iterator last);
	//���ÿռ���Ԫ�ؽ�����䣬����������������´���һ���µķ����������������������ԭ�����Ͻ��в���
	void fill_assign(size_type n, const T& value);
	//���·��䲢����Ԫ��
	void reallocate_insert(iterator pos, const value_type& value);
	//���������
	template<class IIter>
	void copy_insert(iterator pos, IIter first, IIter last);
	//������ֵ
	template <class Iterator>
	void copy_assign(Iterator first, Iterator last, input_iterator_tag);
	template <class Iterator>
	void copy_assign(Iterator first, Iterator last, forward_iterator_tag);

public:
	//���� ��ֵ �ƶ� ����
	explicit vector(size_type n) { fill_initialize(n, value_type());}
	vector() :start(0),finish(0),end_of_storage(0){} 
	vector(size_type n, const T& value) { fill_initialize(n, value); }
	vector(const vector& v) { fill_initialize_range(v.start, v.finish); }
	vector(std::initializer_list<value_type> _list) { fill_initialize_range(_list.begin(), _list.end()); }
	//***********************************************
	vector& operator=(const vector& res);
	vector& operator=(std::initializer_list<value_type> _list);
	//***********************************************
	~vector(){
		destroy_remove(first, last, finish - first);
		first = nullptr;
		last = nullptr;
		finish = nullptr;
	}
public:
	//����Ԫ��
	void insert(iterator pos, const value_type& value);
	void insert(iterator position, size_type n, const T& x);
	template<class Iterator>
	void insert(iterator position, Iterator first, Iterator last);


	//������ʼλ��
	iterator begin() { return start; }
	const_iterator cbegin() { return start; }

	//��������λ��
	iterator end() { return finish; }
	const_iterator cend() { return finish; }

	//max_size()
	size_type max_size() const noexcept
	{
		return static_cast<size_type>(-1) / sizeof(T);
	}

	//������ǰ��С
	size_type size() const { return size_type(end() - begin()); }

	//��������
	size_type capacity() const { return size_type(end_of_storage - begin()); }

	//�����Ƿ�Ϊ��
	bool empty() const { return begin() == end(); }

	//[]���������
	reference operator[](size_type n) { return *(begin() + n); }
	const_reference operator[](size_type n) const { return *(start + n); }

	//at������������ĳ��λ�õ�����
	reference at(size_type n);
	const_reference at(size_type n) const;

	//���������׸�Ԫ��
	reference front() const { return *begin(); }

	//���������е�β��Ԫ��
	reference back() const { return *(end() - 1); }

	//��ֵ����
	template<class Iterator>
	void assign(Iterator first, Iterator last);
	void assign(size_type n, const T& x = T());
	void assign(std::initializer_list<value_type> il)
	{
		copy_assign(il.begin(), il.end(), ministl::forward_iterator_tag{});
	}

	//��ȡ������
	size_type get_new_cap(size_type add_size);

	//β�巨
	void push_back(const T& x);

	//βɾ��
	void pop_back();

	//��������������Ԫ��
	void swap(vector& rhs) noexcept;

	//ɾ���������ϵ�Ԫ��
	iterator erase(iterator position);

	//ɾ����������Χ�ڵ�Ԫ��
	iterator erase(iterator first, iterator last);

	//���µ���������С
	void resize(size_type new_size, const T& x);
	void resize(size_type new_size) { resize(new_size, T()); }

	//�������
	void clear(size_type new_size, const T& x);

private:
	void destroy_remove(iterator first, iterator last, size_type n);
};
//****************************pop_back()***********************
template <class T, class Alloc>
void vector<T, Alloc>::pop_back() {
	--finish;
	destroy(finish);
}

//****************************get_new_cap()************************
template <class T,class Alloc>
typename vector<T,Alloc>::size_type
vector<T,Alloc>::
get_new_cap(size_type add_size)
{
	const auto old_size = capacity();
	if (old_size > max_size() - old_size / 2)
	{
		return old_size + add_size > max_size() - 16
			? old_size + add_size : old_size + add_size + 16;
	}
	const size_type new_size = old_size == 0
		? ministl::max(add_size, static_cast<size_type>(16))
		: ministl::max(old_size + old_size / 2, old_size + add_size);
	return new_size;
}

//****************************fill_assign()**********************
template <class T, class Alloc>
void vector<T, Alloc>::fill_assign(size_type n, const T& value) {
	if (n > capacity())
	{
		vector tmp(n, value);
		swap(tmp);
	}
	else if (n > size())
	{
		ministl::fill(begin(), end(), value);
		last = ministl::uninitialized_fill_n(last, n - size(), value);
	}
	else
	{
		erase(ministl::fill_n(start, n, value), last);
	}
}
//****************************swap()****************************
template<class T,class Alloc>
void vector<T, Alloc>::swap(vector& rhs) {
	if (this != &rhs)
	{
		ministl::swap(start, rhs.start);
		ministl::swap(last, rhs.last);
		ministl::swap(finish, rhs.finish);
	}
}

//****************************copy_assign()**********************
template<class T,class Alloc>
template<class Iterator>
void vector<T, Alloc>::copy_assign(Iterator first, Iterator last, input_iterator_tag) {
	auto cur = start;
	for (; first != last && cur != last; ++first, ++cur)
	{
		*cur = *first;
	}
	if (first == last)
	{
		erase(cur, last);
	}
	else
	{
		insert(last, first, last);
	}
}

template<class T, class Alloc>
template<class Iterator>
void vector<T, Alloc>::copy_assign(Iterator first, Iterator last, forward_iterator_tag) {
	const size_type len = ministl::distance(first, last);
	if (len > capacity())
	{
		vector tmp(first, last);
		swap(tmp);
	}
	else if (size() >= len)
	{
		auto new_end = ministl::copy(first, last, start);
		data_allocator::destroy(new_end, last);
		last = new_end;
	}
	else
	{
		auto mid = first;
		ministl::advance(mid, size());
		ministl::copy(first, mid, start);
		auto new_end = ministl::uninitialized_copy(mid, last, last);
		last = new_end;
	}
}

//**************************** at() ****************************
template <class T,class Alloc>
vector<T, Alloc>::reference vector<T, Alloc>::at(size_type n) {
	try {
		if (n < size()) {
			return *(this + n)
		}
	}
	catch (...)
	{
		std::cout << "out of range\n" << endl;
		throw;
	}
}

template <class T, class Alloc>
vector<T, Alloc>::const_reference vector<T, Alloc>::at(size_type n) const{
	try {
		if (n < size()) {
			return *(this + n)
		}
	}
	catch (...)
	{
		std::cout << "out of range\n" << endl;
		throw;
	}
}

//***************************copy_insert()************************
template <class T,class Alloc>
template <class IIter>
void vector<T,Alloc>::
copy_insert(iterator pos, IIter first, IIter last)
{
	if (first == last)
		return;
	const auto n = ministl::distance(first, last);
	if ((end_of_storage - last) >= n)
	{ // ������ÿռ��С�㹻
		const auto after_elems = last - pos;
		auto old_end = last;
		if (after_elems > n)
		{
			last = ministl::uninitialized_copy(end_ - n, end_, end_);
			ministl::move_backward(pos, old_end - n, old_end);
			ministl::uninitialized_copy(first, last, pos);
		}
		else
		{
			auto mid = first;
			ministl::advance(mid, after_elems);
			last = ministl::uninitialized_copy(mid, last, end_);
			ministl::uninitialized_copy(first, mid, pos);
		}
	}
	else
	{ // ���ÿռ䲻��
		const auto new_size = get_new_cap(n);
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try
		{
			new_end = ministl::uninitialized_move(begin_, pos, new_begin);
			new_end = ministl::uninitialized_copy(first, last, new_end);
			new_end = ministl::uninitialized_move(pos, end_, new_end);
		}
		catch (...)
		{
			destroy_and_recover(new_begin, new_end, new_size);
			throw;
		}
		data_allocator::deallocate(start, last - start);
		start = new_begin;
		last = new_end;
		end_of_storage= start + new_size;
	}
}

//***************************insert()****************************
template <class T,class Alloc>
void vector<T,Alloc>::insert(iterator pos, const value_type& value)
{
	iterator xpos = pos;
	const size_type n = pos - start;
	//��������ֱ�Ӹ�lastָ��ָ��++Ȼ��ֵ
	if (last != end_of_storage && xpos == last)
	{
		ministl::construct(last, value);
		++last;
	}
	else if (last != end_of_storage)
	{
		auto new_end = last;
		ministl::construct(last, *(last - 1));
		++new_end;
		auto value_copy = value;  // ����Ԫ�������¸��Ʋ��������ı�
		std::copy_backward(xpos, last - 1, last);
		*xpos = std::move(value_copy);
		last = new_end;
	}
	else
	{
		reallocate_insert(xpos, value);
	}
	return start + n;
}

template<class T,class Alloc>
template<class Iterator>
void vector<T,Alloc>::insert(iterator position, Iterator first, Iterator last) {
	ministl::copy_insert(position, first, last);
}

template<class T,class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
	if (n != 0) {
		// ���ڱ��ÿռ���װn��Ԫ��
		if (size_type(end_of_storage - finish) >= n) {
			T x_copy = x;
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			//Ҫ�����Ԫ�����Ѿ��е�Ԫ���м�
			if (elems_after > n) {
				ministl::uninitialized_copy(finish - n, finish, finish);
				finish += n;
				std::copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);
			}
			//Ҫ�����Ԫ����ĩβ
			else {
				ministl::uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				ministl::uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			new_finish = ministl::uninitialized_copy(start, position, new_start);
			new_finish = ministl::uninitialized_fill_n(new_finish, n, x);
			new_finish = ministl::uninitialized_copy(position, finish, new_finish);
		}

		ministl::destroy(start, finish);
		deallocate();

		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}
//***********************************reallocate_insert()****************
template <class T,class Alloc>
void vector<T,Alloc>::reallocate_insert(iterator pos, const value_type& value)
{
	const auto new_size = get_new_cap(1);
	auto new_begin = data_allocator::allocate(new_size);
	auto new_end = new_begin;
	const value_type& value_copy = value;
	try
	{
		new_end = ministl::uninitialized_move(start, pos, new_begin);
		ministl::construct(&new_end), value_copy);
		++new_end;
		new_end = ministl::uninitialized_move(pos, last, new_end);
	}
	catch (...)
	{
		data_allocator::deallocate(new_begin, new_size);
		throw;
	}
	destroy_remove(start, finish, end_of_storage - start);
	start = new_begin;
	finish = new_end;
	end_of_storage = new_begin + new_size;
}

//***************************assign()****************************
template<class T, class Alloc>
template<class Iterator>
void vector<T, Alloc>::assign(Iterator first, Iterator last) {
	ministl::copy_assign(first, last, ministl::iterator_category(first));
}
template<class T, class Alloc>
void vector<T, Alloc>::assign(size_type n, const T& x) {
	fill_assign(n, x);
}

//***************************resize()****************************
template<class T,class Alloc>
void vector<T, Alloc>::resize(size_type new_size, const T& x) {
	if (new_size < size())
		erase(begin() + new_size, end());
	else
		insert(end(), new_size - size(), x);
}

template<class T,class Alloc>
void vector<T, Alloc>::clear(size_type new_size, const T& x) {
	erase(begin(), end());
}

//*****************************erase()******************************
template<class T,class Alloc>
typename vector<T, Alloc>::iterator vector<T,Alloc>::erase(iterator position) {
	if (position + 1 != end()) {
		copy(position + 1, finish, position);
	}
	--finish;
	ministl::destroy(finish);
	return position;
}

template<class T,class Alloc>
typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first,iterator last) {
	iterator i = copy(last, finish, first);
	destroy(i, finish);
	finish = finish - (last - first);
	return first;
}

//******************************insert_aux()************************
//			��������ʱ�Ѿ����ˣ��������в���Ԫ�ص�ʱ����øú���
template <class T,class Alloc>
void vector<T,Alloc>::insert_aux(iterator position, const T& x) {
	const size_type old_size = size();
	const size_type len = old_size != 0 ? 2 * old_size : 1;

	iterator new_start = data_allocator::allocate(len); //ʵ������
	iterator new_finish = new_start;
	try {
		//�ȿ�����һ���¿ռ䣬Ȼ�����¿ռ��п�������λ��֮ǰ��Ԫ�� Ȼ�����Ҫ�����Ԫ�أ�Ȼ���ƶ�֮���Ԫ��
		new_finish = ministl::uninitialized_copy(start, position, new_start);
		ministl::construct(new_finish, x);
		++new_finish;
		new_finish = uninitialized_copy(position, finish, new_finish);
	}
	catch (...) {
		ministl::destroy(new_start, new_finish);
		data_allocator::deallocate(new_start, len);
		throw;
	}
	
	destroy_remove(begin(),end(),size());

	start = new_start;
	finish = new_finish;
	end_of_storage = new_start + len;
}

//****************************allocate_fill()*******************************
template<class T, class Alloc>
typename vector<T,Alloc>::iterator vector<T,Alloc>::allocate_fill(size_type size, const T& value) {
	iterator res = data_allocator::allocate(size);
	ministl::uninitialized_fill_n(res, n, value);
	return res;
}

//*******************************fill_initialize()*******************************
template<class T, class Alloc>
void vector<T,Alloc>::fill_initialize(size_type n, const T& value) {
	//��n��Ԫ�������ٿռ� ÿ��Ԫ�ض���value���͵ı���
	start = allocate_fill(n,value);
	finish = start + n;
	end_of_storage = finish;
}

//********************************deallocate()***********************************
template<class T, class Alloc>
void vector<T, Alloc>::deallocate() {
	if (start) data_allocator::deallocate(start, end_of_storage - start);
}

//********************************fill_initialize_range()***********************
template<class T, class Alloc>
template <class Iterator>
void vector<T,Alloc>::fill_initialize_range(Iterator first, Iterator last) {

	//init_space(static_cast<size_type>(last - first), init_size);
	iterator tmp= (iterator)data_allocator::allocate(last - first);
	start = tmp;
	finish = start + (last - first);
	this->last = finish;

	ministl::uninitialized_copy(first, last, start);
}

//**************************destroy_remove()*************************
template<class T,class Alloc>
void vector<T, Alloc>::destroy_remove(iterator first, iterator last, size_type n) {
	ministl::destroy(first,last);
	data_allocator::deallocate(first,n);
}

//*************************push_back()*******************************
template<class T,class Alloc>
void vector<T, Alloc>::push_back(const T& x) {
	if (last != finish) {
		ministl::construct(finish,T);
		++finish;
	}
	else {
		insert_aux(end(), x);
	}
}

//******************************���������********************************
template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs)
{
	return lhs.size() == rhs.size() &&
		ministl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs)
{
	return ministl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
}

template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
{
	return !(lhs == rhs);
}

template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs)
{
	return rhs < lhs;
}

template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
{
	return !(rhs < lhs);
}

template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
{
	return !(lhs < rhs);
}

// ���� mystl �� swap
template <class T>
void swap(vector<T>& lhs, vector<T>& rhs)
{
	lhs.swap(rhs);
}

}//!ministl

#endif