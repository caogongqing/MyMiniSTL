#ifndef __MYMINISTL_TYPETRAITS_H__
#define __MYMINISTL_TYPETRAITS_H__

#include <type_traits>
namespace ministl
{
template <class T, T v>
struct m_integral_constant
{
	static constexpr T value = v;
};

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;

typedef m_bool_constant<true>  m_true_type;
typedef m_bool_constant<false> m_false_type;

template <class T>
struct is_pair : ministl::m_false_type {};

template <class T1, class T2>
struct is_pair<ministl::pair<T1, T2>> : ministl::m_true_type {};


}
#endif
