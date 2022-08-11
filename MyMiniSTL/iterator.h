#ifndef __MYMINISTL_ITERATOR_H__
#define __MYMINISTL_ITERATOR_H__

#include <iostream>
#include <cstddef>

namespace ministl
{
//定义五种迭代器类型
	//输入、输出、单向、双向、随机访问。
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag: public input_iterator_tag {};
struct bidirectional_iterator_tag: public input_iterator_tag {};
struct random_access_iterator_tag: public bidirectional_iterator_tag {};

//划定了五种typedef，以此来用于之后算法向容器的提问
	//哪种迭代器、元素的类型、两个迭代器的距离、元素的指针类型、元素的引用类型(按照template模板的参数顺序) 
template <class Category, class T, class Distance = ptrdiff_t, 
		  class Pointer = T*, class Reference = T&>
	struct iterator {
	typedef Category iterator_category;
	typedef T value_type;
	typedef Pointer pointer;
	typedef Reference reference;
	typedef Distance difference_type;

};
//萃取器traits
template <class Iterator>
struct iterator_traits {
	//还没熟练掌握C++ 所以这里稍微注释一下typename的作用：
	//除了作为模板中的类型之外，当我们需要访问某一个类中的类型的时候也需要加 目的就是明确的告诉编译器我这个是一个类型。
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

//针对原生指针的偏特化版本
	//T*, const T*,
template <class T>
struct iterator_traits<T*> {
	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T& reference;
};
template <class T>
struct iterator_traits<const T*> {
	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef const T* pointer;
	typedef const T& reference;
};

// 决定迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();
}

// 决定迭代器的distance type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 决定迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 求distance的函数组
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type 
__distance(InputIterator first, InputIterator last,input_iterator_tag) {
	iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) {
		++first;
		++n;
	}
	return n;
}
//虽然同样可以用上述的方法接收，但是对于随机访问迭代器来说，直接last-first更快，因为可以跳跃式访问。
template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
	return last - first;
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
	typedef typename iterator_traits<InputIterator>::iterator_category category;
	return __distance(first,last,category);
}

//advance函数组
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
	while (n--) i++;
}

template <class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
	if (n >= 0) {
		while (n--) i++;
	}
	else {
		while (n++) i--;
	}
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
	i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
	__advance(i, n, iterator_category(i));
}


//代表反向迭代器
template <class Iterator>
class reverse_iterator
{
private:
	Iterator current;  // 记录对应的正向迭代器

public:
	// 反向迭代器的五种相应型别
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type        value_type;
	typedef typename iterator_traits<Iterator>::difference_type   difference_type;
	typedef typename iterator_traits<Iterator>::pointer           pointer;
	typedef typename iterator_traits<Iterator>::reference         reference;

	typedef Iterator                                              iterator_type;
	typedef reverse_iterator<Iterator>                            self;

public:
	// 构造函数
	reverse_iterator() {}
	explicit reverse_iterator(iterator_type i) :current(i) {}
	reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
	// 取出对应的正向迭代器
	iterator_type base() const
	{
		return current;
	}

	// 重载操作符
	reference operator*() const
	{ // 实际对应正向迭代器的前一个位置
		auto tmp = current;
		return *--tmp;
	}
	pointer operator->() const
	{
		return &(operator*());
	}

	// 前进(++)变为后退(--)
	self& operator++()
	{
		--current;
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		--current;
		return tmp;
	}
	// 后退(--)变为前进(++)
	self& operator--()
	{
		++current;
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		++current;
		return tmp;
	}

	self& operator+=(difference_type n)
	{
		current -= n;
		return *this;
	}
	self operator+(difference_type n) const
	{
		return self(current - n);
	}
	self& operator-=(difference_type n)
	{
		current += n;
		return *this;
	}
	self operator-(difference_type n) const
	{
		return self(current + n);
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}
};

//**************************back_iterator_tag**********************
template<class Container>
class back_insert_iterator {
protected:
	Container* container;
public:
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;

	explicit back_insert_iterator(Container& x):container(&x) { }
	back_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
		container->push_back(value);
		return *this;
	}

	back_insert_iterator<Container>& operator*() {
		return *this;
	}

	back_insert_iterator<Container>& operator++() {
		return *this;
	}
	back_insert_iterator<Container>& operator++(int) {
		return *this;
	}
};

//*************************front_insert_iterator************************
template<class Container>
class front_insert_iterator {
protected:
	Container* container;
public:
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;

	explicit front_insert_iterator(Container& x): Container(&x) { }
	front_insert_iterator<Container>& operator=(const typename Container::value_type & value){
		container->push_front(value);
		return *this;
	}

	front_insert_iterator<Container>& operator*() {
		return *this;
	}

	front_insert_iterator<Container>& operator++() {
		return *this;
	}

	front_insert_iterator<Container>& operator(int) {
		return *this;
	}
};

template <class Container>
inline front_insert_iterator<Container> front_inserter(Container& x) {
	return front_insert_iterator<Container>(x);
}

//************************insert_iterator**************************
template<class Container>
class insert_iterator {
	protected Container* container;
	typename Container::iterator iter;
public:
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;

	insert_iterator(Container& x, typename Container::iterator i):container(&x),iter(i) { }
	insert_iterator<Container>& operator=(const typename Container::value_type& value) {
		iter = container->insert(iter, value);
		++iter;
		return *this;
	}
	insert_iterator<Container>& operator*() {
		return *this;
	}
	insert_iterator<Container>& operator++() {
		return *this;
	}
	insert_iterator<Container>& operator++(int) {
		return *this;
	}
};

template <class Container, class Iterator>
inline insert_iterator<Container> inserter(Container& x, Iterator it) {
	typedef typename Container::iterator iter;
	return insert_iterator<Container>(x, iter(it));
}

//***********************istream_iterator**************************
template<class T, class Distance = ptrdiff_t>
class istream_iterator {
	friend bool operator==(const istream_iterator<T, Distance>& x, const istream_iterator<T, Distance>& y) {
		return x.stream == y.stream && x.value == y.value;
	}
protected:
	std::istream* stream;
	T value;
	bool end_marker;
	void read() {
		end_marker = (*stream) ? true : false;
		if (end_marker)
			*stream >> value;
		end_marker = (*stream) ? true : false;
	}
public:
	typedef input_iterator_tag iterator_category;
	typedef T value_type;
	typedef Distance difference_type;
	typedef const T* pointer;
	typedef const T& reference;

	istream_iterator() : stream(&cin), end_marker(false) { }
	istream_iterator(std::istream& s) : stream(&s) { read(); }
	reference operator*()const { return value; }
	pointer operator->()const { return &(operator*()); }

	istream_iterator<T, Distance>& operator++() {
		read();
		return *this;
	}

	istream_iterator<T, Distance> operator++(int) {
		istream_iterator<T, Distance> tmp = *this;
		read();
		return tmp;
	}

};

template<class T>
class ostream_iterator {
protected:
	ostream* stream;
	const char* string;
public:
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;

	ostream_iterator(std::ostream& s): stream(&s), string(0) { }
	ostream_iterator(std::ostream& s):stream(&s), string(c) { }
	ostream_iterator<T>& operator=(const T& value) {
		*stream << value;
		if (string) *stream << string;
		return *this;
	}
	ostream_iterator<T>& operator*() {
		return *this;
	}
	ostream_iterator<T>& operator++() {
		return *this;
	}
	ostream_iterator<T>& operator++(int) {
		return *this;
	}
};

}//!ministl

#endif
