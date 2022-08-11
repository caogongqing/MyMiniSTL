#ifndef __MYMINISTL_HEAPALGO_H__
#define __MYMINISTL_HEAPALGO_H__

#include "iterator.h"

namespace ministl
{
//push_heap算法
template<class RandomAccessIterator, class Distance, class T>
void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
	Distance parent = (holeIndex - 1) / 2; //找出父节点
	while (holeIndex > topIndex && *(first + parent) < value) {
		//如果当前节点的序列号小于顶部序列号（顶部序列号一般为heap底层容器的最开始位置）
		//并且当前位置的值比父节点的值要大
		//那么就将父节点的值给子节点 
		*(first + holeIndex) = *(first + parent);
		//然后更新插入的节点的新位置 并更新其父节点
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	//在符合条件的位置上给当前节点赋值
	*(first + holeIndex) = value;
}

template<class RandomAccessIterator, class Distance, class T>
inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
	//新添加的值一定是为于容器最底端
	_push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

//用两个迭代器来表示heap底部容器的头尾
template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
	push_heap_aux(first, last, ministl::distance_type(first), ministl::value_type(first));
}


//pop_heap算法
//这里的参数设定为：first设定为底部容器的最开始的位置 holeindex设定为要弹出的元素的位置
//len代表的是调整后的底部容器的长度，最后的value代表的是原来底部容器中最尾端的位置
template<class RandomAccessIterator, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
	Distance topIndex = holeIndex;
	//调整的节点的右节点
	Distance secondChild = 2 * holeIndex + 2;
	while (secondChild < len) {
		//比较左右两个节点的值，然后以secondChile表示较大的那一个
		if (*(first + secondChild) < *(first + (secondChild - 1)))
			--secondChild;
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * (secondChild + 1);
	}
	if (secondChild == len) {
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
		holeIndex = secondChild - 1;
	}
	_push_heap(first, holeIndex, topIndex, value);
}

template <class RandomAccessIterator, class T, class Distance>
inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
	RandomAccessIterator result, T value, Distance*) {
	*result = *first;
	_adjust_heap(first, Distance(0), Distance(last - first), value);
}

//pop操作的结果应为底部容器的第一个元素，因此，首先设定欲调整值为尾值，然后将首值调至
//尾结点（所以将迭代器result设为last-1），然后调整【first,last-1】，使之重新成一个合格的heap
template<class RandomAccessIterator, class T>
inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
	_pop_heap(first, last - 1, last - 1, T(*(last - 1)), ministl::distance_type(first));
}

template<class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
	_pop_heap_aux(first, last, ministl::value_type(first));
}

//sort_heap算法
template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
	while (last - first > 1)
		pop_heap(first, last--);
}

//make_heap算法
template<class RandomAccessIterator, class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
	if (last - first < 2)
		return;
	Distance len = last - first;
	//找出第一个需要重拍的子树头部，以parent标识出
	Distance parent = (len - 2) / 2;
	while (true) {
		//重新以parent为首的子树，len是为了用于判断操作范围
		_adjust_heap(first, parent, len, T(*(first + parent)));
		if (parent == 0) return;
		parent--;
	}
}

template<class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
	_make_heap(first, last, ministl::value_type(first), ministl::distance_type(first));
}
}//!ministl

#endif
