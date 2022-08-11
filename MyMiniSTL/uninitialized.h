#ifndef __MYMINISTL_UNINT_H__
#define __MYMINISTL_UNINT_H__

#include "type_traits.h"
#include "construct.h"
#include "iterator.h"
#include "algobase.h"

//====================================================================
//29、56、84行还没实现 
//====================================================================

namespace ministl
{
//==========================uninitialized_copy函数================================
template <class InputInterator, class ForwardInterator>
inline ForwardInterator _uninitialized_copy(InputInterator first, InputInterator last, 
								ForwardInterator result,std::false_type) {
    auto cur = result;
    for (; first != last; ++first, ++cur)
    {
        ministl::construct(&*cur, *first);
    } 
    return cur;
}

template <class InputInterator, class ForwardInterator>
inline ForwardInterator _uninitialized_copy(InputInterator first, InputInterator last,
	ForwardInterator result, std::true_type) {
    return ministl::copy(first, last, result);
}

template <class InputInterator, class ForwardInterator>
inline ForwardInterator uninitialized_copy(InputInterator first, InputInterator last, ForwardInterator result) {
	return _uninitialized_copy(first, last, result, std::is_trivially_copy_assignable<
								typename iterator_traits<InputIter>::value_type>{}));
}

template<>
inline char* uninitialized_copy<const char*, char*>(const char* first, const char* last, char* result) {
    std::memmove(result, first, last - first);
    return result + (last - first);
}

template<>
inline wchar_t* uninitialized_copy<const wchar_t*, wchar_t*>(const wchar_t* first, 
                                                const wchar_t* last, wchar_t* result) {
    std::memmove(result, first, last - first);
    return result + (last - first);
}

// ================================uninitialized_copy_n==============================
template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator
_uninit_copy_n(InputIterator first, Size n, ForwardIterator result, std::true_type)
{
    return ministl::copy_n(first, n, result).second;
}

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator
_uninit_copy_n(InputIterator first, Size n, ForwardIterator result, std::false_type)
{
    auto cur = result;
    try
    {
        for (; n > 0; --n, ++cur, ++first)
        {
            ministl::construct(&*cur, *first);
        }
    }
    catch (...)
    {
        for (; result != cur; ++result)
            ministl::destroy(&*result);
    }
    return cur;
}

template <class InputIterator, class Size, class ForwardIterator>
ForwardIterator uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result)
{
    return ministl::_uninit_copy_n(first, n, result,
        std::is_trivially_copy_assignable<
        typename iterator_traits<InputIterator>::
        value_type>{});
}


//=============================uninitialized_fill=====================================
template <class ForwardIterator, class T>
void
_uninit_fill(ForwardIterator first, ForwardIterator last, const T& value, std::true_type)
{
    ministl::fill(first, last, value);
}

template <class ForwardIterator, class T>
void
_uninit_fill(ForwardIterator first, ForwardIterator last, const T& value, std::false_type)
{
    auto cur = first;
    for (; cur != last; ++cur)
    {
        ministl::construct(&*cur, value);
    }
}

template <class ForwardIterator, class T>
void  uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value)
{
    ministl::_uninit_fill(first, last, value,
        std::is_trivially_copy_assignable<
        typename iterator_traits<ForwardIterator>::
        value_type>{});
}

//=========================uninitialized_fill_n=============================
template <class ForwardIterator, class Size, class T>
ForwardIterator
_uninit_fill_n(ForwardIterator first, Size n, const T& value, std::true_type)
{
    return ministl::fill_n(first, n, value);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator
_uninit_fill_n(ForwardIterator first, Size n, const T& value, std::false_type)
{
    auto cur = first;
    for (; n > 0; --n, ++cur)
    {
        minitl::construct(&*cur, value);
    }
    return cur;
}

template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& value)
{
    return ministl::_uninit_fill_n(first, n, value,
        std::is_trivially_copy_assignable<
        typename iterator_traits<ForwardIterator>::
        value_type>{});
}

/*****************************************************************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
template <class InputIter, class ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
    return std::move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
    ForwardIter cur = result;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            ministl::construct(&*cur, mystl::move(*first));
        }
    }
    catch (...)
    {
        ministl::destroy(result, cur);
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
    return ministl::unchecked_uninit_move(first, last, result,
        std::is_trivially_move_assignable<
        typename iterator_traits<InputIter>::
        value_type>{});
}
}
#endif
