#ifndef __MYMINISTL_ITERATOR_H__
#define __MYMINISTL_ITERATOR_H__

#include <iostream>
#include <cstddef>

namespace ministl
{
//�������ֵ���������
	//���롢���������˫��������ʡ�
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag: public input_iterator_tag {};
struct bidirectional_iterator_tag: public input_iterator_tag {};
struct random_access_iterator_tag: public bidirectional_iterator_tag {};

//����������typedef���Դ�������֮���㷨������������
	//���ֵ�������Ԫ�ص����͡������������ľ��롢Ԫ�ص�ָ�����͡�Ԫ�ص���������(����templateģ��Ĳ���˳��) 
template <class Category, class T, class Distance = ptrdiff_t, 
		  class Pointer = T*, class Reference = T&>
	struct iterator {
	typedef Category iterator_category;
	typedef T value_type;
	typedef Pointer pointer;
	typedef Reference reference;
	typedef Distance difference_type;

};
//��ȡ��traits
template <class Iterator>
struct iterator_traits {
	//��û��������C++ ����������΢ע��һ��typename�����ã�
	//������Ϊģ���е�����֮�⣬��������Ҫ����ĳһ�����е����͵�ʱ��Ҳ��Ҫ�� Ŀ�ľ�����ȷ�ĸ��߱������������һ�����͡�
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

//���ԭ��ָ���ƫ�ػ��汾
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

// ������������ category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::iterator_category category;
	return category();
}

// ������������distance type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// ������������ value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// ��distance�ĺ�����
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
//��Ȼͬ�������������ķ������գ����Ƕ���������ʵ�������˵��ֱ��last-first���죬��Ϊ������Ծʽ���ʡ�
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

//advance������
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


//�����������
template <class Iterator>
class reverse_iterator
{
private:
	Iterator current;  // ��¼��Ӧ�����������

public:
	// �����������������Ӧ�ͱ�
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type        value_type;
	typedef typename iterator_traits<Iterator>::difference_type   difference_type;
	typedef typename iterator_traits<Iterator>::pointer           pointer;
	typedef typename iterator_traits<Iterator>::reference         reference;

	typedef Iterator                                              iterator_type;
	typedef reverse_iterator<Iterator>                            self;

public:
	// ���캯��
	reverse_iterator() {}
	explicit reverse_iterator(iterator_type i) :current(i) {}
	reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
	// ȡ����Ӧ�����������
	iterator_type base() const
	{
		return current;
	}

	// ���ز�����
	reference operator*() const
	{ // ʵ�ʶ�Ӧ�����������ǰһ��λ��
		auto tmp = current;
		return *--tmp;
	}
	pointer operator->() const
	{
		return &(operator*());
	}

	// ǰ��(++)��Ϊ����(--)
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
	// ����(--)��Ϊǰ��(++)
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
