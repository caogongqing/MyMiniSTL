#ifndef __MYMINISTL_CONSTRUCT_H__
#define __MYMINISTL_CONSTRUCT_H__
#include <new>

#include "type_traits.h"
#include "iterator.h"

//		！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//		注：在本文件中，应当有可变参数的移动构造函数 但是由于目前掌握不够因此先略过，在之后做补充
//		！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

namespace ministl
{
// construct构造对象

template <class Ty>
void construct(Ty* ptr)
{
	::new ((void*)ptr) Ty();
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value)
{
	::new ((void*)ptr) Ty1(value);
}

// destroy析构对象 这里的手法真的太太太巧妙了 -！ 最后那个参数是用来判断是否调用析构函数的
// 询问你的析构函数是不是不重要，如果是true_type，那么就是不重要的，如果是false_type那就是重要的。
	//传入的是对象类型指针用于析构的泛化版本

	//对于直接传入的是对象指针的话，直接调用析构函数就不解释了；
	//对于传入的是迭代器的时候，因为我们将迭代器传入进去后，会一直循环向尾后迭代器移动，每遇到一个迭代器，就会调用的是
	//接收一个参数的destroy函数，那么就会调用这个迭代器所指对象的析构函数，然后结束后返回，循环调用
template <class Ty>
void destroy_one(Ty* ptr, std::true_type) { }
	//特化版本
template <class Ty>
void destroy_one(Ty* ptr, std::false_type) {
	if (ptr != nullptr) {
		ptr->~Ty();
	}
}
	//传入的是两个迭代器来用于析构的泛化版本
template <class ForwardIter>
void destroy_two(ForwardIter first, ForwardIter last, std::true_type) { }
	//特化版本
template <class ForwardIter>
void destroy_two(ForwardIter first, ForwardIter last, std::false_type) {
	for (; first != last; ++first)
		destroy(&*first);
}

template <class Ty>
void destroy(Ty* ptr) {
	destroy_one(ptr, std::is_trivially_destructible<Ty>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last, std::false_type) {
	destroy_cat(first, last, std::is_trivially_destructible<
		typename iterator_traits<ForwardIter>::value_type>{});
}

}

#endif
