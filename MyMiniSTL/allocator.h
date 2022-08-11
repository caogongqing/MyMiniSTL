#ifndef __MYMINISTL_ALLOCATOR_H__
#define __MYMINISTL_ALLOCATOR_H__

#include "construct.h"

//		同样是因为可变模板参数以及还没有写一些小零件代码，因此对于移动构造形式的构造函数以及空间分配先空着不写


namespace ministl
{
template<class T>
class allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

public:
	//注：这里之所以定义为静态成员函数，是因为静态成员函数是不用实例化就可以调用的函数,所以静态成员函数可以作为回调函数和线程的入口函数。
	static T* allocate();
	static T* allocate(size_type n);

	static void deallocate(T* ptr);
	static void deallocate(T* ptr, size_type n);

	static void construct(T* ptr);
	static void construct(T* ptr, const T& value);
	static void construct(T* ptr, T&& value);

	//template <class... Args>
	//static void construct(T* ptr, Args&& ...args);

	static void destroy(T* ptr);
	static void destroy(T* first, T* last);
};

template <class T>
T* allocator<T>::allocate() {
	return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n) {
	if (n == 0) return nullptr;
	return static_cast<T*>(::operator new(sizeof(T) * n));
}

template <class T>
void allocator<T>::deallocate(T* ptr) {
	if (ptr == nullptr) return;
	::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type) {
	if (ptr == nullptr) return;
	::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr) {
	ministl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value) {
	ministl::construct(ptr, value);
}

// 先不实现，是因为还没有将move这个小组件写好
template <class T>
void allocator<T>::construct(T* ptr, T&& value) {

}

template <class T>
void allocator<T>::destroy(T* ptr)
{
	ministl::destroy(ptr);
}

template <class T>
void allocator<T>::destroy(T* first, T* last)
{
	ministl::destroy(first, last);
}
}
#endif
