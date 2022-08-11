#ifndef __MYMINISTL_CONSTRUCT_H__
#define __MYMINISTL_CONSTRUCT_H__
#include <new>

#include "type_traits.h"
#include "iterator.h"

//		������������������������������������������������������������������������������������������
//		ע���ڱ��ļ��У�Ӧ���пɱ�������ƶ����캯�� ��������Ŀǰ���ղ���������Թ�����֮��������
//		������������������������������������������������������������������������������������������

namespace ministl
{
// construct�������

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

// destroy�������� ������ַ����̫̫̫������ -�� ����Ǹ������������ж��Ƿ��������������
// ѯ��������������ǲ��ǲ���Ҫ�������true_type����ô���ǲ���Ҫ�ģ������false_type�Ǿ�����Ҫ�ġ�
	//������Ƕ�������ָ�����������ķ����汾

	//����ֱ�Ӵ�����Ƕ���ָ��Ļ���ֱ�ӵ������������Ͳ������ˣ�
	//���ڴ�����ǵ�������ʱ����Ϊ���ǽ������������ȥ�󣬻�һֱѭ����β��������ƶ���ÿ����һ�����������ͻ���õ���
	//����һ��������destroy��������ô�ͻ���������������ָ���������������Ȼ������󷵻أ�ѭ������
template <class Ty>
void destroy_one(Ty* ptr, std::true_type) { }
	//�ػ��汾
template <class Ty>
void destroy_one(Ty* ptr, std::false_type) {
	if (ptr != nullptr) {
		ptr->~Ty();
	}
}
	//����������������������������ķ����汾
template <class ForwardIter>
void destroy_two(ForwardIter first, ForwardIter last, std::true_type) { }
	//�ػ��汾
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
