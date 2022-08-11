#ifndef __MYMINISTL_NUMERIC_H__
#define __MYMINISTL_NUMERIC_H__

#include <math.h>
#include "iterator.h"

namespace ministl
{
//*************************** accumulate() ****************************
//将区间中的每一个元素都加在某一个值上
template<class Iterator, class T>
T accumulate(Iterator first, Iterator last, T value) {
	for (; first != last; ++first) {
		value += *first;
	}
	return value;
}

//对于不能直接使用+的元素类型需要提供其指定的accumulate版本
template <class Iterator, class T, class BinaryOperation>
T accumulate(Iterator first, Iterator last, T value, BinaryOperation binary_op) {
	for (; first != last; ++first) {
		value = binary_op(value, *first);
	}
	return value;
}

//***************************adjacent_difference()**********************
//计算相邻两元素的差额
template<class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
	if (first == last) return result;
	*result = *first;
	typename iterator_traits<InputIterator>::value_type value = *first;
	while (++first != last) {
		typename iterator_traits<InputIterator>::value_type tmp = *first;
		*++result = tmp - value;
		value = tmp;
	}
	return ++result;
}

template<class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
	OutputIterator result, BinaryOperation binary_op) {
	if (first == last) return result;
	*result = *first;
	typename iterator_traits<InputIterator>::value_type value = *first;
	while (++first != last) {
		typename iterator_traits<InputIterator>::value_type tmp = *first;
		*++result = binary_op(tmp,value);
		value = tmp;
	}
	return ++result;
}

//************************ inner_product() ***********************
//计算两个序列的内积
template<class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first, InputIterator1 last, InputIterator2 first1, T value) {
	for (; first < last; ++first, ++first1) {
		value += (*first * *first1);
	}
	return value;
}

template <class InputIterator1, class InputIterator2, class T, class BinaryOperation>
T inner_product(InputIterator1 first, InputIterator1 last, InputIterator2 first1, 
	T value, BinaryOperation binary_op) {
	for (; first != last; ++first, ++first1) {
		value += binary_op(*first, *first1);
	}
	return value;
}

//************************** partial_sum() ***************************
//相邻元素之和
template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
	if (first == last) return result;
	*result = *first;
	typename iterator_traits<InputIterator>::value_type value = *first;
	while (++first != last) {
		value += *first;
		*++result = value;
	}

	return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
	OutputIterator result, BinaryOperation binary_op) {
	if (first == last) return result;
	*result = *first;
	typename iterator_traits<InputIterator>::value_type value = *first;
	while (++first != last) {
		value = binary_op(value, *first);
		*++result = value;
	}

	return ++result;
}

//****************************** iota() ******************************
template <class ForwardIter, class T>
void iota(ForwardIter first, ForwardIter last, T value)
{
	while (first != last)
	{
		*first++ = value;
		++value;
	}
}

}//!ministl

#endif
