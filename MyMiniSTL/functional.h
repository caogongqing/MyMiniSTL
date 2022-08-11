#ifndef __MYMINISTL_FUNCTIONAL_H__
#define __MYMINISTL_FUNCTIONAL_H__

#include <cstddef>

namespace ministl
{
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

template<class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

//相加
template<class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x + y;
    }
};

//相减
template<class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x - y;
    }
};

//相乘
template<class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x * y;
    }
};

//相除
template<class T>
struct divides: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x / y;
    }
};

//取模
template<class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x % y;
    }
};

//否定
template<class T>
struct negate : public unary_function<T, T> {
    T operator()(const T& x, const T& y) const {
        return -x;
    }
};

// 函数对象：小于
template <class T>
struct less :public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return x < y; }
};

/*****************************************************************************************/
// 哈希函数对象

// 对于大部分类型，hash function 什么都不做
template <class Key>
struct hash {};

// 针对指针的偏特化版本
template <class T>
struct hash<T*>
{
    size_t operator()(T* p) const noexcept
    {
        return reinterpret_cast<size_t>(p);
    }
};

// 对于整型类型，只是返回原值
#define MYSTL_TRIVIAL_HASH_FCN(Type)         \
template <> struct hash<Type>                \
{                                            \
  size_t operator()(Type val) const noexcept \
  { return static_cast<size_t>(val); }       \
};

MYSTL_TRIVIAL_HASH_FCN(bool)

MYSTL_TRIVIAL_HASH_FCN(char)

MYSTL_TRIVIAL_HASH_FCN(signed char)

MYSTL_TRIVIAL_HASH_FCN(unsigned char)

MYSTL_TRIVIAL_HASH_FCN(wchar_t)

MYSTL_TRIVIAL_HASH_FCN(char16_t)

MYSTL_TRIVIAL_HASH_FCN(char32_t)

MYSTL_TRIVIAL_HASH_FCN(short)

MYSTL_TRIVIAL_HASH_FCN(unsigned short)

MYSTL_TRIVIAL_HASH_FCN(int)

MYSTL_TRIVIAL_HASH_FCN(unsigned int)

MYSTL_TRIVIAL_HASH_FCN(long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long)

MYSTL_TRIVIAL_HASH_FCN(long long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef MYSTL_TRIVIAL_HASH_FCN

// 对于浮点数，逐位哈希
inline size_t bitwise_hash(const unsigned char* first, size_t count)
{
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull;
    const size_t fnv_prime = 1099511628211ull;
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif
    size_t result = fnv_offset;
    for (size_t i = 0; i < count; ++i)
    {
        result ^= (size_t)first[i];
        result *= fnv_prime;
    }
    return result;
}

template <>
struct hash<float>
{
    size_t operator()(const float& val)
    {
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
    }
};

template <>
struct hash<double>
{
    size_t operator()(const double& val)
    {
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
    }
};

template <>
struct hash<long double>
{
    size_t operator()(const long double& val)
    {
        return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
    }
};

// 函数对象：等于
template <class T>
struct equal_to :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x == y; }
};

// 函数对象：不等于
template <class T>
struct not_equal_to :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <class T>
struct greater :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x > y; }
};

// 函数对象：小于
template <class T>
struct less :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x < y; }
};

// 函数对象：大于等于
template <class T>
struct greater_equal :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

// 函数对象：小于等于
template <class T>
struct less_equal :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x <= y; }
};

// 函数对象：逻辑与
template <class T>
struct logical_and :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x && y; }
};

// 函数对象：逻辑或
template <class T>
struct logical_or :public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x || y; }
};

// 函数对象：逻辑非
template <class T>
struct logical_not :public unary_function<T, bool>
{
    bool operator()(const T& x) const { return !x; }
};

// 证同函数：不会改变元素，返回本身
template <class T>
struct identity :public unary_function<T, bool>
{
    const T& operator()(const T& x) const { return x; }
};

// 选择函数：接受一个 pair，返回第一个元素
template <class Pair>
struct select1st :public unary_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type& operator()(const Pair& x) const
    {
        return x.first;
    }
};

// 选择函数：接受一个 pair，返回第二个元素
template <class Pair>
struct select2nd :public unary_function<Pair, typename Pair::second_type>
{
    const typename Pair::second_type& operator()(const Pair& x) const
    {
        return x.second;
    }
};

// 投射函数：返回第一参数
template <class Arg1, class Arg2>
struct project1st :public binary_function<Arg1, Arg2, Arg1>
{
    Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

// 投射函数：返回第二参数
template <class Arg1, class Arg2>
struct project2nd :public binary_function<Arg1, Arg2, Arg1>
{
    Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};

//****************************配接器********************************
template<class Predicate>
class unary_negate :public unary_function<typename Predicate::argument_type, bool> {
protected:
    Predicate pred;
public:
    explicit unary_negate(const Predicate& x):pred(x) { }
    bool operator()(const typename Predicate::argument_type& x)const {
        return !pred(x);
    }
};

template<class Predicate>
inline unary_negate<Predicate> not1(const Predicate& pred) {
    return unary_negate<Predicate>(pred);
}

template<class Predicate>
class binary_negate :public binary_function<typename Predicate::first_argument_type,
    typename Predicate::second_argument_type, bool> {
protected:
    Predicate pred;
public:
    explicit binary_negate(const Predicate& x):pred(x) { }
    bool operator()
        (const typename Predicate::first_argument_type& x, const typename Predicate::second_argument_type& y)const {
        return !pred(x, y);
    }
};

template<class Predicate>
inline binary_negate<Predicate> not2(const Predicate& pred) {
    return binary_negate<Predicate>(pred);
}

//***********************binder1st***************************
template<class Operator>
class binder1st :public unary_function
    <typename Operation::second_argument_type, typename Operation::result_type> {
protected:
    Operator op;
    typename Operator::first_argument_type value;
public:
    binder1st(const Operator& x, const typename Operator::first_argument_type& y) :op(x), value(y) { }
    typename Operator::result_type
    operator()(const typename Operator::second_argument_type& x) const {
        return op(value, x);
    }
};

template <class Operator, class T>
inline binder1st<Operator> bind1st(const Operator& op, const T& x) {
    typename typename Operator::first_argument_type arg1_type;
    return binder1st<Operator>(op, arg1_type(x));
}

//*****************************binder2nd*************************************
template<class Operation>
class binder2nd:unary_function<typename Operation::first_argument_type, typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::second_argument_type value;
public:
    binder2nd(const Operation& x, const typename Operation::second_argument_type& y):op(x),value(y) { }
    typename Operation::result_type
        operator()(const typename Operation::first_argument_type& x) const {
        return op(x, value);
    }
};

template<class Operation, class T>
inline binder2nd<Operation> bind2nd(const Operation& op, const T& x) {
    typedef typename Operation::second_argument_type arg2_type;
    return binder2nd<Operation>(op, arg2_type(x));
}

//**********************用于函数合成*************************
template <class Operation1, class Operation2>
class unary_compose :unary_function<typename Operation2::argument_type, typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation& y):op1(x),op2(y) { }
    typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const {
        return op1(op2(x));
    }
};

template<class Operation1, class Operation2>
inline unary_compose<Operation1, Operation2> compose1(const Operation1& op1, const Operation2& op2) {
    return unary_compose<Operation1, Operation2>(op1, op2);
}

template<class Operation1, class Operation2, class Operation3>
class binary_compose :
    public unary_function<typename Operation2::argument_type, typename Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, const Operation3& z):op1(x),op2(y),op3(z) { }
    typename Operation1::result_type;
    operator()(const typename Operation2::argument_type& x) const {
        return op1(op2(x), op3(x));
    }
};

template <class Operation1, class Operation2, class Operation3>
inline binary_compose<Operation1, Operation2, Operation3>
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
    return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}

}

#endif
