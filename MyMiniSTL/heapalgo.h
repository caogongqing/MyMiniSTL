#ifndef __MYMINISTL_HEAPALGO_H__
#define __MYMINISTL_HEAPALGO_H__

#include "iterator.h"

namespace ministl
{
//push_heap�㷨
template<class RandomAccessIterator, class Distance, class T>
void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
	Distance parent = (holeIndex - 1) / 2; //�ҳ����ڵ�
	while (holeIndex > topIndex && *(first + parent) < value) {
		//�����ǰ�ڵ�����к�С�ڶ������кţ��������к�һ��Ϊheap�ײ��������ʼλ�ã�
		//���ҵ�ǰλ�õ�ֵ�ȸ��ڵ��ֵҪ��
		//��ô�ͽ����ڵ��ֵ���ӽڵ� 
		*(first + holeIndex) = *(first + parent);
		//Ȼ����²���Ľڵ����λ�� �������丸�ڵ�
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	//�ڷ���������λ���ϸ���ǰ�ڵ㸳ֵ
	*(first + holeIndex) = value;
}

template<class RandomAccessIterator, class Distance, class T>
inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
	//����ӵ�ֵһ����Ϊ��������׶�
	_push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

//����������������ʾheap�ײ�������ͷβ
template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
	push_heap_aux(first, last, ministl::distance_type(first), ministl::value_type(first));
}


//pop_heap�㷨
//����Ĳ����趨Ϊ��first�趨Ϊ�ײ��������ʼ��λ�� holeindex�趨ΪҪ������Ԫ�ص�λ��
//len������ǵ�����ĵײ������ĳ��ȣ�����value�������ԭ���ײ���������β�˵�λ��
template<class RandomAccessIterator, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
	Distance topIndex = holeIndex;
	//�����Ľڵ���ҽڵ�
	Distance secondChild = 2 * holeIndex + 2;
	while (secondChild < len) {
		//�Ƚ����������ڵ��ֵ��Ȼ����secondChile��ʾ�ϴ����һ��
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

//pop�����Ľ��ӦΪ�ײ������ĵ�һ��Ԫ�أ���ˣ������趨������ֵΪβֵ��Ȼ����ֵ����
//β��㣨���Խ�������result��Ϊlast-1����Ȼ�������first,last-1����ʹ֮���³�һ���ϸ��heap
template<class RandomAccessIterator, class T>
inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
	_pop_heap(first, last - 1, last - 1, T(*(last - 1)), ministl::distance_type(first));
}

template<class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
	_pop_heap_aux(first, last, ministl::value_type(first));
}

//sort_heap�㷨
template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
	while (last - first > 1)
		pop_heap(first, last--);
}

//make_heap�㷨
template<class RandomAccessIterator, class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
	if (last - first < 2)
		return;
	Distance len = last - first;
	//�ҳ���һ����Ҫ���ĵ�����ͷ������parent��ʶ��
	Distance parent = (len - 2) / 2;
	while (true) {
		//������parentΪ�׵�������len��Ϊ�������жϲ�����Χ
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
