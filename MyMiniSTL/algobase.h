#ifndef __MYMINISTL_ALGOBASE_H__
#define __MYMINISTL_ALGOBASE_H__
#include <cstring>

#include "iterator.h"
#include "util.h"

namespace ministl
{
//***************************** max *****************************
template <class T>
inline const T& max(const T& lhs, const T& rhs)
{
	return lhs < rhs ? rhs : lhs;
}

template<class T, class Comp>
inline const T& max(const T& a, const T& b, Comp cp) {
	return cp(a, b) ? b : a;
}

//******************************min()**************************
template <class T>
const T& min(const T& lhs, const T& rhs)
{
	return lhs > rhs ? rhs : lhs;
}

template<class T, class Comp>
inline const T& min(const T& a, const T& b, Comp cp) {
	return cp(b, a) ? b : a;
}

//*************************** mismatch() ************************
template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first, InputIterator1 last, 
	InputIterator2 first1) {
	while (first != last && *first1 == *first) {
		++first;
		++first1;
	}
	return ministl::pair<InputIterator1, InputIterator2>(first, first1);
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first, InputIterator1 last,
	InputIterator2 first1, BinaryPredicate binary_pred) {
	while (first != last && binary_pred(*first, *first1) {
		++first;
		++first1;
	}
	return ministl::pair<InputIterator1, InputIterator2>(first, first1);
}

//****************************iter_swap()*************************
template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
	typename iterator_traits<ForwardIterator1>::value_type tmp = *a;
	*a = *b;
	*b = tmp;
}

//*****************************swap()****************************
template <class T>
void swap(T* a, T* b) {
	T* tmp = a;
	a = b;
	b = tmp;
}

//*********************************equal()*****************************
template <class InputIter1, class InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (*first1 != *first2)
			return false;
	}
	return true;
}

// ���ذ汾ʹ�ú������� comp ����Ƚϲ���
template <class InputIter1, class InputIter2, class Compared>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (!comp(*first1, *first2))
			return false;
	}
	return true;
}

/*********************************lexicographical_compare()***************************************/
// ���ֵ������ж��������н��бȽϣ�����ĳ��λ�÷��ֵ�һ�鲻���Ԫ��ʱ�������м��������
// (1)�����һ���е�Ԫ�ؽ�С������ true �����򷵻� false
// (2)������� last1 ����δ���� last2 ���� true
// (3)������� last2 ����δ���� last1 ���� false
// (4)���ͬʱ���� last1 �� last2 ���� false
template <class InputIter1, class InputIter2>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
	InputIter2 first2, InputIter2 last2)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (*first1 < *first2)//��һ����Ԫ��ֵС�ڵڶ�������ӦԪ��ֵ
			return true;
		if (*first2 < *first1)//�ڶ�����Ԫ��ֵС�ڵ�һ����Ԫ��ֵ
			return false;
	}
	return first1 == last1 && first2 != last2;
}

// ���ذ汾ʹ�ú������� comp ����Ƚϲ���
template <class InputIter1, class InputIter2, class Compred>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
	InputIter2 first2, InputIter2 last2, Compred comp)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (comp(*first1, *first2))
			return true;
		if (comp(*first2, *first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}

// ��� const unsigned char* ���ػ��汾
bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2)
{
	const auto len1 = last1 - first1;
	const auto len2 = last2 - first2;
	// �ȱȽ���ͬ���ȵĲ���
	const auto result = std::memcmp(first1, first2, ministl::min(len1, len2));
	// ����ȣ����Ƚϳ��ıȽϴ�
	return result != 0 ? result < 0 : len1 < len2;
}

// =============================copy============================
template <class InputIter, class OutputIter>
OutputIter
_copy_cat(InputIter first, InputIter last, OutputIter result,
	ministl::input_iterator_tag)
{
	for (; first != last; ++first, ++result)
	{
		*result = *first;
	}
	return result;
}

// ramdom_access_iterator_tag �汾
template <class RandomIter, class OutputIter>
OutputIter
_copy_cat(RandomIter first, RandomIter last, OutputIter result,
	ministl::random_access_iterator_tag)
{
	for (auto n = last - first; n > 0; --n, ++first, ++result)
	{
		*result = *first;
	}
	return result;
}

template <class InputIter, class OutputIter>
OutputIter
_copy(InputIter first, InputIter last, OutputIter result)
{
	return _copy_cat(first, last, result, iterator_category(first));
}


// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_copy_assignable<Up>::value,
	Up*>::type
	_copy(Tp* first, Tp* last, Up* result)
{
	const auto n = static_cast<size_t>(last - first);
	if (n != 0)
		std::memmove(result, first, n * sizeof(Up));
	return result + n;
}

template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	return _copy(first, last, result);
}

//���char�Լ�wchar_t���ػ��汾
inline char* copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

//==============================fill===============================
template <class ForwardIter, class T>
void fill_cat(ForwardIter first, ForwardIter last, const T& value,
	ministl::forward_iterator_tag)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}

template <class RandomIter, class T>
void fill_cat(RandomIter first, RandomIter last, const T& value,
	ministl::random_access_iterator_tag)
{
	fill_n(first, last - first, value);
}

template <class ForwardIter, class T>
void fill(ForwardIter first, ForwardIter last, const T& value)
{
	fill_cat(first, last, value, iterator_category(first));
}

//==============================fill_n=============================
template <class OutputIter, class Size, class T>
OutputIter fill_n(OutputIter first, Size n, const T& value)
{
	return _fill_n(first, n, value);
}

template <class OutputIter, class Size, class T>
OutputIter _fill_n(OutputIter first, Size n, const T& value)
{
	for (; n > 0; --n, ++first)
	{
		*first = value;
	}
	return first;
}

//********************************lexicographical_compare()*****************************
template <class InputIter1, class InputIter2>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
	InputIter2 first2, InputIter2 last2)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (*first1 < *first2)
			return true;
		if (*first2 < *first1)
			return false;
	}
	return first1 == last1 && first2 != last2;
}

// ���ذ汾ʹ�ú������� comp ����Ƚϲ���
template <class InputIter1, class InputIter2, class Compred>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
	InputIter2 first2, InputIter2 last2, Compred comp)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (comp(*first1, *first2))
			return true;
		if (comp(*first2, *first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}

// ��� const unsigned char* ���ػ��汾
bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2)
{
	const auto len1 = last1 - first1;
	const auto len2 = last2 - first2;
	// �ȱȽ���ͬ���ȵĲ���
	const auto result = std::memcmp(first1, first2, ministl::min(len1, len2));
	// ����ȣ����Ƚϳ��ıȽϴ�
	return result != 0 ? result < 0 : len1 < len2;
}

//**********************	set�����㷨		******************
//1.������set����һ��set_union
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}

	return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Comp>
OutputIterator set_union(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Comp cp) {
	while (first1 != last1 && first2 != last2) {
		if (cp(*first1, *first2)) {
			*result = *first1;
			++first1;
		}
		else if (cp(*first2,*first1)) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}

	return copy(first2, last2, copy(first1, last1, result));
}

//2.���������Ͻ���һ��set_intersection
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			++first1;
		}
		else if (*first2 < *first1) {
			++first2;
		}
		else {
			*result = *first1;
			++first1;
			++first2;
			++result;
		}
	}

	return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Comp>
OutputIterator set_intersection(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Comp cp) {
	while (first1 != last1 && first2 != last2) {
		if (cp(*first1,*first2)) {
			++first1;
		}
		else if (cp(*first2,*first1)) {
			++first2;
		}
		else {
			*result = *first1;
			++first1;
			++first2;
			++result;
		}
	}

	return result;
}

//3.����set֮��Ĳset_difference()
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1) {
			++first2;
		}
		else {
			++first1;
			++first2;
		}
	}

	return copy(first1, last1, result);
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Comp>
OutputIterator set_difference(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Comp cp) {
	while (first1 != last1 && first2 != last2) {
		if (cp(*first1,*first2)) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (cp(*first2,*first1)) {
			++first2;
		}
		else {
			++first1;
			++first2;
		}
	}

	return copy(first1, last1, result);
}

//4. �ԳƲ ������s1����������s2�Լ�������s2��������s1��Ԫ��set_sysmmetric_difference
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
			++result;
		}
		else {
			++first1;
			++first2;
		}
	}

	return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Comp>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator2 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Comp cp) {
	while (first1 != last1 && first2 != last2) {
		if (cp(*first1, *first2)) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (cp(*first2, *first1)) {
			*result = *first2;
			++first2;
			++result;
		}
		else {
			++first1;
			++first2;
		}
	}

	return copy(first2, last2, copy(first1, last1, result));
}

//*************************adjacent_find()**********************
//�ҳ���һ����������������Ԫ��
template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
	if (first == last) return last;
	ForwardIterator next = first;
	//�ҵ�����Ԫ�ؾͽ���
	while (++next != last) {
		if (*first == *next)
			return first;
		first = next;
	}
	return last;
}

template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred) {
	if (first == last) return last;
	ForwardIterator next = first;
	while (++next != last) {
		if (binary_pred(*first,*next))
			return first;
		first = next;
	}
	return last;
}

//************************* count() ********************
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type 
count(InputIterator first,InputIterator last, const T& value) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (*first == value) ++n;
	}
	return n;
}

//*************************count_if()***************************
template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type 
count_if(InputIterator first, InputIterator last, Predicate pre) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (pre(*first)) ++n;
	}
	return n;
}

//*****************************find()**************************
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
	while (first != last && *first != value) ++first;
	return first;
}

//*****************************find_if()**************************
template <class InputIterator, class Predicate>
InputIterator find(InputIterator first, InputIterator last, Predicate pre) {
	//����������ֱ������ѭ��
	while (first != last && !pre(*first)) ++first;
	return first;
}

// ***********************search()***************************
// ��[first1, last1)�в���[first2, last2)���״γ��ֵ�
template <class ForwardIter1, class ForwardIter2>
ForwardIter1
search(ForwardIter1 first1, ForwardIter1 last1,
	ForwardIter2 first2, ForwardIter2 last2)
{
	//�����Ĳ��� ���ҵ�Ԫ������ĸ��ͬ��ʱ�� ���бȶ� ������ֲ�ͬ�Ļ���������һ��λ�ü���Ѱ��
	auto d1 = ministl::distance(first1, last1);
	auto d2 = ministl::distance(first2, last2);
	if (d1 < d2)
		return last1;
	auto current1 = first1;
	auto current2 = first2;
	while (current2 != last2)
	{
		if (*current1 == *current2)
		{
			++current1;
			++current2;
		}
		else
		{
			if (d1 == d2)
			{
				return last1;
			}
			else
			{
				current1 = ++first1;
				current2 = first2;
				--d1;
			}
		}
	}
	return first1;
}

// ���ذ汾ʹ�ú������� comp ����Ƚϲ���
template <class ForwardIter1, class ForwardIter2, class Compared>
ForwardIter1
search(ForwardIter1 first1, ForwardIter1 last1,
	ForwardIter2 first2, ForwardIter2 last2, Compared comp)
{
	auto d1 = ministl::distance(first1, last1);
	auto d2 = ministl::distance(first2, last2);
	if (d1 < d2)
		return last1;
	auto current1 = first1;
	auto current2 = first2;
	while (current2 != last2)
	{
		if (comp(*current1, *current2))
		{
			++current1;
			++current2;
		}
		else
		{
			if (d1 == d2)
			{
				return last1;
			}
			else
			{
				current1 = ++first1;
				current2 = first2;
				--d1;
			}
		}
	}
	return first1;
}

//******************************find_end()*************************
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2) {
	typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
	typedef typename iterator_traits<ForwardIterator2>::iterator_category category2;
	return _find_end(first1, last1, first2, last2, category1(), category2());
}

template<class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 _find_end
(ForwardIterator1 first1, ForwardIterator1 last1, 
	ForwardIterator2 first2, ForwardIterator2 last2, forward_iterator_tag, forward_iterator_tag) {
	if (first1 == last1)
		return last1;
	else {
		ForwardIterator1 result = last1;
		while (1) {
			//����ĳ���������״γ��ֵ�
			ForwardIterator11 new_result = search(first1, last1, first2, last2);
			if (new_result == last1)
				return result;
			else {
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}
}

template<class BidrectionalIterator1, class BidrectionalIterator2>
BidrectionalIterator1 _find_end
(BidrectionalIterator1 first1, BidrectionalIterator1 last1,
	BidrectionalIterator2 first2, BidrectionalIterator2 last2, bidirectional_iterator_tag, bidirectional_iterator_tag) {
	typedef reverse_iterator<BidrectionalIterator1> reviter1;
	typedef reverse_iterator<BidrectionalIterator1> reviter2;

	reviter1 rlast1(first1);
	reviter2 rlast2(first2);

	//���Ƚ����з�ת
	reviter1 rresult = search(reviter1(last1), rlast1, reviter2(last2), rlast2);

	if (rresult1 == rlast1)
		return last1;
	else {
		//�ҵ���
		BidrectionalIterator1 result = rresult.base();
		advance(result. - distance(first2, last2));
		return result;
	}
}

//************************find_first_of()*************************
template <class InputIterator, class ForwardIterator>
InputIterator find_first_of
(InputIterator first1, InputIterator last1, ForwardIterator first2, ForwardIterator last2) {
	for (; first1 != last1; ++first1) {
		for (ForwardIterator iter = first2; iter != last2; ++iter) {
			if (*first == *iter)
				return first1;
		}
	}
	return last1;
}

template <class InputIterator, class ForwardIterator, class Comp>
InputIterator find_first_of
(InputIterator first1, InputIterator last1, ForwardIterator first2, ForwardIterator last2, Comp cp) {
	for (; first1 != last1; ++first1) {
		for (ForwardIterator iter = first2; iter != last2; ++iter) {
			if (cp(*first,*iter))
				return first1;
		}
	}
	return last1;
}

//************************for_each()***************************
template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
	for (; first != last; ++first) {
		f(*first);
	}
	return f;
}

//***************************generate()*************************
template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
	for (; first != last; ++first)
		*first = gen();
}

//***************************generate_n()*************************
template <class OutputIterator, class Size, class Generator>
void generate_n(OutputIterator first, Size n, Generator gen) {
	for (; n > 0; --n, ++first)
		*first = gen();
	return first;
}

//****************************includes()****************************
template<class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
	while (first1 != last1 && first2 != last2) {
		//���first2�е�Ԫ��С��first1�е�Ԫ�� ˵����first1�в��������Ԫ��
		if (*first2 < *first1)
			return false;
		else if (*first1 < *first2)
			++first1;
		else
			++first1, ++first2;
	}
	//�Ƿ��first2ȫ���������
	return first2 == last2;
}

template<class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, 
	InputIterator2 last2, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		//���first2�е�Ԫ��С��first1�е�Ԫ�� ˵����first1�в��������Ԫ��
		if (comp(first2,*first1))
			return false;
		else if (comp(*first1,*first2))
			++first1;
		else
			++first1, ++first2;
	}
	//�Ƿ��first2ȫ���������
	return first2 == last2;
}

//*************************max_element()**************************
template<class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
	if (first == last) return first;
	ForwardIterator result = first;
	while (++first != last) {
		if (*result < *first)
			result = first;
	}

	return result;
}

template<class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
	if (first == last) return first;
	ForwardIterator result = first;
	//�Զ����������ͱȴ�С
	while (++first != last) {
		if (comp(*result, *first))
			result = first;
	}

	return result;
}

//**************************merge()****************************
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}

	return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first2,*first1)) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}

	return copy(first2, last2, copy(first1, last1, result));
}

//***************************min_element()*************************
template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
	if (first == last) return first;
	ForwardIterator result = first;
	while (++first != last) {
		if (*first < *result)
			result = first;
	}
	return result;
}

//*****************************partition()****************************
template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pre) {
	while (true)
	{
		while (true) {
			if (first == last)
				return first;
			else if (pre(*first))
				++first;
			//�����������Ļ��ͽ�ָ��ͣ���ڵ�ǰλ�ü���
			else
				break;
		}
		--last;
		while (true) {
			if (first == last)
				return first;
			else if (!pre(*last))
				--last;
			//���������Ļ���ͣ���ڵ�ǰλ�� �ȴ�������ǰ��Ĳ�����������Ԫ�ؽ���
			else
				break;
		}
		iter_swap(first, last);
		++first;
	}
}

//***************************remove_copy()****************************
template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy
(InputIterator first, InputIterator last, OutputIterator result, const T& value) {
	for (; first != last; ++first) {
		//���������Ļ��ͽ���ֵ��ֵ���µĵ����� Ȼ���µĵ���������
		if (*first != value) {
			*result = *first;
			++result;
		}
	}

	return result;
}

//*****************************remove()**************************
template<class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
	first = find(first, last, value);
	ForwardIterator next = first;
	return first == last ? first : remove_copy(++next, last, first, value);
}

//************************** remove_copy_if() ***************************
template <class InputIterator, class OutputIteratpr, class Predicate>
OutputIteratpr remove_copy_if(InputIterator first, InputIterator last, OutputIteratpr result, Predicate pre) {
	for (; first != last; ++first) {
		if (pre(*first)) {
			*result = *first;
			++result;
		}
	}

	return result;
}

//*******************************remove_if()*************************
template<class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
	first = find(first, last, value);
	ForwardIterator next = first;
	return first == last ? first : remove_copy_if(++next, last, first, value);
}

//*********************************replace()****************************
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value) {
	for (; first != last; ++first) {
		if (*first == old_value)
			*first = new_value;
	}
}

//**************************replace_copy()***************************
template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy
(InputIterator first, InputIterator last, OutputIterator result, const T& old_value, const T& new_value) {
	for (; first != last; ++first, ++result) {
		*result = (*first == old_value ? new_value : *first);
	}
	return result;
}

//*************************replace_if()****************************
template<class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, Predicate pre, const T& new_value) {
	for (; first != last; ++first) {
		if (pre(*first))
			*first = new_value;
	}
}

//****************************replace_copy_if()************************
template <class Iterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if
(Iterator first, Iterator last, OutputIterator result, Predicate pre, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = pre(*first) ? new_value : *first;
	return result;
}

//**************************reverse()**************************
template <class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
	_reverse(first, last, iterator_category(first));
}

template <class BidirectionalIterator>
void _reverse(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag) {
	while (true) {
		if (first == last || first == --last) {
			return;
		}
		else {
			iter_swap(first++, last);
		}
	}
}

template <class RandomAccessIterator>
void _reverse(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
	while (first < last)
	{
		iter_swap(first++, --last);
	}
}

//************************reverse_copy()*************************
template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy
(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
	while (first != last) {
		--last;
		*result = *last;
		++result;
	}

	return result;
}

//**************************rotate()*****************************
template<class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
	if (first == middle || middle == last) return;
	_rotate(first, middle, last, distance_type(first), iterator_category(first));
}

template <class ForwardIterator , class Distance>
void _rotate
(ForwardIterator first, ForwardIterator middle, ForwardIterator last, Distance*, forward_iterator_tag) {
	for (ForwardIterator i = middle;;) {
		iter_swap(first, i);
		++first;
		++i;
		// һ���ж���ǰ��[first, middle]�Ƚ������Ǻ��[middle,last]�Ƚ���
		if (first == middle) {//ǰ�̽�����
			if (i == last) return;//������ͬʱҲ�����������ͽ�����
			middle = i;//������������µ�ǰ�����������
		}
		else if (i == last){
			i = middle;
		}
	}
}

template <class BidirectionalIterator, class Distance>
void _rotate
(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance*, bidirectional_iterator_tag) {
	reverse(first, middle);
	reverse(middle, last);
	reverse(first, last);
}

template <class EuclideanRingElement>
EuclideanRingElement _gcd(EuclideanRingElement m, EuclideanRingElement n) {
	while (n != 0) {
		EuclideanRingElement t = m % n;
		m = n;
		n = t;
	}
	return m;
}

template <class RandomAccessIterator, class Distance, class T>
void _rotate_cycle(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator initial, Distance shift, T*) {
	T value = *initial;
	RandomAccessIterator ptr1 = initial;
	RandomAccessIterator ptr2 = ptr1 + shift;
	while (ptr2 != initial) {
		*ptr1 = *ptr2;
		ptr1 = ptr2;
		if (last - ptr2 > shift)
			ptr2 += shift;
		else
			ptr2 = first + (shift - (last - ptr2));
	}
	*ptr1 = value;
}

template<class RandomAccessIterator, class Distance>
void _rotate
(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Distance*, random_access_iterator_tag) {
	Distance n = _gcd(last - first, middle - first);
	while (n--) {
		_rotate_cycle(first, last, first + n, middle - first, value_type(first));
	}
}

//*************************rotate_copy()************************
template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy
(ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result) {
	return copy(first, middle, copy(middle, last, result));
}

//*************************search_n()***************************
template <class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count, const T& value) {
	if (count <= 0)
		return first;
	else {
		first = find(first, last, value);//�����ҳ�value��һ�γ��ֵ�
		while (first != last) {//������������Ԫ��
			Integer n = count - 1;//value��Ӧ����n��
			ForwardIterator i = first;//���ϴγ��ֵ����ȥ����
			++i;
			while (i != last && n != 0 && *i == value) {//��һ��Ԫ����value
				++i;
				--n;
			}
			if (n == 0)//��������
				return first;
			else
				first = find(i, last, value);//��ʣ�������Ѱ����һ�����ֵ�
		}
		return last;
	}
}

template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n
(ForwardIterator first, ForwardIterator last, Integer count, const T& value, BinaryPredicate binary_pred) {
	if (count <= 0)
		return first;
	else {
		while (first != last) {
			if (binary_pred(*first, value))
				break;
		}
		while (first != last) {
			Integer n = count - 1;
			ForwardIterator i = first;
			++i;
			while (i != last && n != 0 && binary_pred(*i,value)) {
				++i;
				--n;
			}
			if (n == 0)
				return first;
			else {
				while (i != last) {
					if (binary_pred(*i, value)) break;
					++i;
				}
				first = i;
			}
		}

		return last;
	}
}

//*************************swap_ranges()**************************
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator2 last1, ForwardIterator2 first2) {
	for (; first1 != last1; ++first1, ++first2) {
		iter_swap(first1, first2);
	}
	return first2;
}

//****************************transform()****************************
template<class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op) {
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform
(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, OutputIterator result, BinaryOperation binary_op) {
	for (; first != last; ++first, ++result)
		*result = binary_op(*first);
	return result;
}

//*****************************unique()*************************
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
	first = adjacent_find(first, last);
	return unique_copy(first, last, first);
}

//****************************unique_copy()************************
template <class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result) {
	if (first == last)
		return result;
	return _unique_copy(first, last, result, iterator_category(result));
}

template<class InputIterator, class ForwardIterator>
ForwardIterator _unique_copy(InputIterator first, InputIterator last, ForwardIterator result, forward_iterator_tag) {
	*result = *first;
	while (++first != last) {
		//Ԫ�ز�ͬ�ͼ�¼ Ԫ����ͬ������
		if (*result != *first) *++result = *first;
	}
	return ++result;
}

//OutputIterator������ֻ��д �޷������ǰ�������һ����ȡ ��˲�����*result ��= *first�Ĳ���
template<class InputIterator, class OutputIterator>
inline OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, output_iterator_tag) {
	return _unique_copy(first, last, result, value_type(first));
}

template<class InputIterator, class OutputIterator, class T>
OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, T*) {
	T value = *first;
	*result = value;
	while (++first != last) {
		if (value != *first) {
			value = *first;
			*++result = value;
		}
	}
	return ++result;
}

//******************************mismatch()*************************
template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch
(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
	while (first1 != last1 && *first1 == *first2) {
		++first1;
		++first2;
	}
}

template <class InputIterator1, class InputIterator2, class Compare>
pair<InputIterator1, InputIterator2> mismatch
(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Compare comp) {
	while (first1 != last1 && comp(first1,*first2)) {
		++first1;
		++first2;
	}
}

//************************lower_bound()*************************
//���ַ�
template<class Forwarditerator, class T, class Distance>
Forwarditerator _lower_bound
(Forwarditerator first, Forwarditerator last, const T& value, Distance*, forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	Forwarditerator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else {
			len = half;
		}
	}
	return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator _lower_bound
(RandomAccessIterator first, RandomAccessIterator last, const T& value, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else {
			len = half;
		}
	}
	return first;
}

template<class Forwarditerator, class T, class Distance, class Compare>
Forwarditerator _lower_bound
(Forwarditerator first, Forwarditerator last, const T& value, Compare comp, Distance*, forward_iterator_tag) {
	Distance len = 0;
	ministl::distance(first, last, len);
	Distance half;
	Forwarditerator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (comp(*middle < value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else {
			len = half;
		}
	}
	return first;
}

template <class RandomAccessIterator, class T, class Distance,class Compare>
RandomAccessIterator _lower_bound
(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle,value)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else {
			len = half;
		}
	}
	return first;
}

template <class ForwardIter, class T>
ForwardIter
lower_bound(ForwardIter first, ForwardIter last, const T& value)
{
	return ministl::_lower_bound(first, last, value, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
	return ministl::_lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

//*****************************upper_bound()***************************
template<class ForwardIterator, class T,class Compare, class Distance>
ForwardIterator _upper_bound
(ForwardIterator first, ForwardIterator last, const T& value, Compare comp, Distance*, forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (comp(value ,*middle)) {
			len = half;
		}
		else {
			first = middle;
			++first;
			len = len - half - 1;
		}
	}
	return first;
}

template<class RandomAccessIterator, class T, class Distance, class Compare>
RandomAccessIterator _upper_bound
(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(value,*middle))
			len = half;
		else {
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}

template<class ForwardIterator, class T, class Distance>
ForwardIterator _upper_bound
(ForwardIterator first, ForwardIterator last, const T& value, Distance*, forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (value < *middle) {
			len = half;
		}
		else {
			first = middle;
			++first;
			len = len - half - 1;
		}
	}
	return first;
}

template<class RandomAccessIterator, class T, class Distance>
RandomAccessIterator _upper_bound
(RandomAccessIterator first, RandomAccessIterator last, const T& value, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (value < *middle)
			len = half;
		else {
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}

template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
	return _upper_bound(first, last, value, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
	return _upper_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

//**********************************binary_search()******************************
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
	ForwardIterator i = lower_bound(first, last, value);
	//�ҵ�λ�ò������λ���ϵ�ֵ������value
	return i != last && !(value < *i);
}

template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
	ForwardIterator i = lower_bound(first, last, value, comp);
	//�ҵ�λ�ò������λ���ϵ�ֵ������value
	return i != last && !comp(value,*i);
}

//****************************next_permutation()*****************************
template<class BidirectionIterator>
bool next_permutation(BidirectionIterator first, BidirectionIterator last) {
	if (first == last) return false;
	BidirectionIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;

	for (;;) {
		BidirectionIterator ii = i;
		--i;
		if (*i < *ii) {
			BidirectionIterator j = last;
			while (!(*i < *--j));
			iter_swap(i, j);
			reverse(ii, last);
			return false;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

template<class BidirectionIterator, class Compare>
bool next_permutation(BidirectionIterator first, BidirectionIterator last, Compare comp) {
	if (first == last) return false;
	BidirectionIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;

	for (;;) {
		BidirectionIterator ii = i;
		--i;
		if (comp(*i,*ii)) {
			BidirectionIterator j = last;
			while (!comp(*i,*--j));
			iter_swap(i, j);
			reverse(ii, last);
			return false;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

//***************************prev_permutation()*************************
template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
	if (first == last) {
		return false;
	}

	BidirectionalIterator i = first;
	++i;

	if (i == last) return false;
	i = last;
	--i;

	for (;;) {
		BidirectionalIterator ii = i;
		if (*ii < *i) {
			BidirectionalIterator j = last;
			while (!(*--j < *i));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
	if (first == last) {
		return false;
	}

	BidirectionalIterator i = first;
	++i;

	if (i == last) return false;
	i = last;
	--i;

	for (;;) {
		BidirectionalIterator ii = i;
		if (comp(*ii , *i) {
			BidirectionalIterator j = last;
			while (!comp(*--j,*i));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

//********************random_shuffle()*****************
template <class RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
	_random_shuffle(first, last, distance_type(first));
}

template <class RandomAccessIterator, class Distance>
void _random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance*) {
	if (first == last) return;
	for (RandomAccessIterator i = first + 1; i != last; ++i) {
		iter_swap(i, first + Distance(rand() % ((i - first) + 1)));
	}
}

template <class RandomAccessIterator, class RandomNumberGenerator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator& rand) {
	if (first == last) return;
	for (RandomAccessIterator i = first + 1; i != last; ++i)
		iter_swap(i, first + rand((i - first) + 1));
}

//*************************sort()****************************
template <class T>
inline const T& __median(const T& a, const T& b, const T& c)
{
	if (a < b)
		if (b < c)       // a < b < c  
			return b;
		else if (a < c)  // a < b, b >= c, a < c  -->     a < b <= c  
			return c;
		else            // a < b, b >= c, a >= c   -->   c <= a < b  
			return a;
	else if (a < c)      // c > a >= b  
		return a;
	else if (b < c)      // a >= b, a >= c, b < c   -->   b < c <= a  
		return c;
	else                // a >= b, a >= c, b >= c  -->    c<= b <= a  
		return b;
}

template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(
	RandomAccessIterator first,
	RandomAccessIterator last,
	T pivot)
{
	while (true) {
		while (*first < pivot) ++first;  // first �ҵ� >= pivot��Ԫ�ؾ�ͣ  
		--last;

		while (pivot < *last) --last;    // last �ҵ� <=pivot  

		if (!(first < last)) return first;   // ��������ѭ��    
	//  else  
		iter_swap(first, last);              // ��Сֵ����  
		++first;                            // ����  
	}
}

// paitial_sort���������ҳ�middle - first����СԪ�ء�  
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first,
	RandomAccessIterator middle,
	RandomAccessIterator last)
{
	__partial_sort(first, middle, last, value_type(first));
}
template <class RandomAccessIterator, class T>
inline void __partial_sort(RandomAccessIterator first,
	RandomAccessIterator middle,
	RandomAccessIterator last, T*)
{
	make_heap(first, middle); // Ĭ����max-heap����root������  
	for (RandomAccessIterator i = middle; i < last; ++i)
		if (*i < *first)
			__pop_heap(first, middle, i, T(*i), distance_type(first));
	sort_heap(first, middle);
}

template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first,
	RandomAccessIterator last)
{
	if (first != last) {
		__introsort_loop(first, last, value_type(first), __lg(last - first) * 2);
		__final_insertion_sort(first, last);
	}

}
// __lg()�������Ʒָ�񻯵����  
// �ҳ�2^k <= n �����ֵ,����n=7��k=2; n=20��k=4  
template<class Size>
inline Size __lg(Size n)
{
	Size k;
	for (k = 0; n > 1; n >>= 1)
		++k;
	return k;
}

// ��Ԫ�ظ���Ϊ40ʱ,__introsort_loop�����һ������  
// ��__lg(last-first)*2��5*2,��˼���������ָ�10�㡣  

const int  __stl_threshold = 16;

template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first,
	RandomAccessIterator last, T*,
	Size depth_limit)
{
	while (last - first > __stl_threshold) {      // > 16  
		if (depth_limit == 0) {                  // ���ˣ��ָ��  
			partial_sort(first, last, last);    // ���� heapsort  
			return;
		}

		--depth_limit;
		// ������ median-of-3 partition��ѡ��һ�����õ����Ტ�����ָ��  
		// �ָ�㽫���ڵ�����cut����  
		RandomAccessIterator cut = __unguarded_partition
		(first, last, T(__median(*first,
			*(first + (last - first) / 2),
			*(last - 1))));

		// ���Ұ�εݹ����sort  
		__introsort_loop(cut, last, value_type(first), depth_limit);

		last = cut;
		// ���ڻص�whileѭ���У�׼�������εݹ����sort  
		// ����д���ɶ��ԽϲЧ��Ҳ��û�бȽϺ�  
	}
}

template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first,
	RandomAccessIterator last)
{
	if (last - first > __stl_threshold) {
		// > 16  
		// һ��[first,first+16)���в�������  
		// ��������__unguarded_insertion_sort��ʵ����ֱ�ӽ������������ѭ����  
		//     *�μ�Insertion sort Դ��  
		sort(first, first + __stl_threshold);
		__unguarded_insertion_sort(first + __stl_threshold, last);
	}
	else
		__insertion_sort(first, last);
}

template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first,
	RandomAccessIterator last)
{
	__unguarded_insertion_sort_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>

void __unguarded_insertion_sort_aux(RandomAccessIterator first,
	RandomAccessIterator last,
	T*)
{
	for (RandomAccessIterator i = first; i != last; ++i)
		__unguarded_linear_insert(i, T(*i));
}

//*********************equal_range()*************************
template<class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>equal_range(ForwardIterator first, ForwardIterator last, const T& value) {
	return _equal_range(first, last, value, distance_type(first), iterator_category(first));
}

template<class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
_equal_range
(RandomAccessIterator first, RandomAccessIterator last, const T& value, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			right = upper_bound(++middle, first + len, value);
			return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
		}
	}

	return pair<RandomAccessIterator, RandomAccessIterator>(first, last);
}

template <class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator> _equal_range
(ForwardIterator first, ForwardIterator last, const T& value, Distance*, forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			advance(first, len);
			right = upper_bound(++middle, first, value);
			return pair<ForwardIterator, ForwardIterator>(left, right);
		}
	}
	return pair<ForwardIterator, ForwardIterator>(first, first);
}

//*************************nth_element()*************************
template<class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last) {
	_nth_element(first, nth, last, value_type(first));
}

template <class RandomAccessIterator, class T>
void _nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, T*) {
	while (last - first > 3) {
		RandomAccessIterator cut = _unguarded_partiton(first, last, T(_median(*first, *(first + (last - first) / 2, *(last - 1)))));
		if (cut <= nth)
			first = cut;
		else
			last = cut;
	}

	_insertion_sort(first, last);
}

//***************************merge_sort()**************************
//�鲢����
template <class BidirectionalIter>
void mergesort(BidirectionalIter first, BidirectionalIter last) {
	typename iterator_traits<BidirectionalIter>::difference_type n = distance(first, last);
	if (n == 0 || n == 1)
		return;
	else {
		BidirectionalIter mid = first + n / 2;
		mergesort(first, mid);
		mergesort(mid, last);
		inplace_merge(first, mid, last);
	}
}

//***********************partial_sort()***************************
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
	_partial_sort(first, middle, last, value_type(first));
}

template<class RandomAccessIterator, class T>
void _partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T*) {
	make_heap(first, last);
	for (RandomAccessIterator i = middle; i < last; ++i) {
		if (*i < *first) {
			_pop_heap(first, middle, i, T(*i), distance_type(first));
		}
	}
	sort_heap(first, last);
}


}//��ministl

#endif
