#ifndef __MYMINISTL_ALLOCATOR_H__
#define __MYMINISTL_ALLOCATOR_H__

#include "construct.h"

//		ͬ������Ϊ�ɱ�ģ������Լ���û��дһЩС������룬��˶����ƶ�������ʽ�Ĺ��캯���Լ��ռ�����ȿ��Ų�д


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
	//ע������֮���Զ���Ϊ��̬��Ա����������Ϊ��̬��Ա�����ǲ���ʵ�����Ϳ��Ե��õĺ���,���Ծ�̬��Ա����������Ϊ�ص��������̵߳���ں�����
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

// �Ȳ�ʵ�֣�����Ϊ��û�н�move���С���д��
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
