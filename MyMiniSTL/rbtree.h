#ifndef __MYMINISTL_RBTREE_H__
#define __MYMINISTL_RBTREE_H__

#include <initializer_list>
#include <cassert>

#include "alloc.h"
#include "functional.h"
#include "iterator.h"
#include "type_traits.h"
#include "construct.h"
#include "util.h"

namespace ministl
{

typedef bool _rb_tree_color_type;
static _rb_tree_color_type _rb_tree_red = false; //��ɫΪ0
static _rb_tree_color_type _rb_tree_black = true;//��ɫΪ1

//������
struct _rb_tree_node_base
{
	typedef _rb_tree_color_type color_type;
	typedef _rb_tree_node_base* base_ptr;

	color_type color;//�ڵ���ɫ���Ǻڼ���
	base_ptr parent;//RB��������������֪�����ڵ�
	base_ptr left;//ָ����ڵ�
	base_ptr right;//ָ���ҽڵ�

	static base_ptr minimum(base_ptr x) {
		while (x->left != nullptr) x = x->left;
		return x;
	}

	static base_ptr maximum(base_ptr x) {
		while (x->right != 0) x = x->right;
		return x;
	}
};

//��ֵ����ͬ��
template <class T, bool>
struct rb_tree_value_traits_imp
{
	typedef T key_type;
	typedef T mapped_type;
	typedef T value_type;

	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value;
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value;
	}
};

template <class T>
struct rb_tree_value_traits
{
	static constexpr bool is_map = ministl::is_pair<T>::value;

	typedef rb_tree_value_traits_imp<T, is_map> value_traits_type;

	typedef typename value_traits_type::key_type key_type;
	typedef typename value_traits_type::mapped_type mapped_type;
	typedef typename value_traits_type::value_type value_type;

	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value_traits_type::get_key(value);
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value_traits_type::get_value(value);
	}
};

template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept
{
	return node == node->parent->left;
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
	return node->color == rb_tree_red;
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
	node->color = rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
	node->color = rb_tree_red;
}

template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept
{
	//�����Ľṹ�� ��һ���ڵ�����ҽڵ��������һֱ����
	if (node->right != nullptr)
		return rb_tree_min(node->right);
	//������Ǹýڵ㲻�����ӽڵ�Ļ� ��һֱ������ ֱ���������ӽڵ�󷵻����ĸ��ڵ�
	while (!rb_tree_is_lchild(node))
		node = node->parent;
	return node->parent;
}

//************************_rb_tree_rebalance()*****************************
inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base* &root) {
	/*
	* ������ĵ��������У�ÿһ���²���Ľڵ����ɫ���Ǻ�ɫ������Ǻ�ɫ��ô���е����ڵ㶼�Ǻ�ɫ�ˣ���û�������ˣ�
	* ���ڲ���ڵ������ڵ�ĸ��ڵ��Լ�����ڵ��Ǻ�ɫ����ô�ͽ����ڵ��Լ�����ڵ㶼��Ϊ��ɫ�����ҽ�үү�ڵ��Ϊ��ɫ
	* ֮������������ǲ�����Ļ����ͽ�����ת����
	* �����ж��ڲ�ͬ����ת���Ų�ͬ��Ҫ��
	* ������
	*	�������ڵ��Ǻ�ɫ������ڵ��Ǻ�ɫ ���ҵ�ǰ������ҽڵ㣬������������
	* ������
	*	�������ڵ��Ǻ�ɫ������ڵ��Ǻ�ɫ�����ҵ�ǰ�ڵ�����ڵ㣬�ͽ�����������
	*/
	x->color = _rb_tree_red;
	while (x != root && x->parent->color == _rb_tree_red) {
		if (x->parent == x->parent->parent->left) {
			_rb_tree_node_base* y = x->parent->parent->right;
			if (y && y->color == _rb_tree_red) {
				x->parent->color == _rb_tree_black;
				y->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				x = x->parent->parent;
			}
			else {
				if (x == x->parent->right) {
					x = x->parent;
					_rb_tree_rotate_left(x, root);//����
				}

				//��������ȣ�������΢�鷳һЩ������Ҫ���丸�ڵ��Ϊ��ɫ����үү�ڵ��Ϊ��ɫ
				x->parent->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				_rb_tree_rotate_right(x->parent->parent, root);//����
			}
		}
		else {
			_rb_tree_node_base* y = x->parent->parent->left;
			if (y && y->color == _rb_tree_red) {
				x->parent->color = _rb_tree_black;
				y->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				x = x->parent->parent;
			}
			else {
				if (x == x->parent->left) {
					x = x->parent;
					_rb_tree_rotate_right(x, root);
				}
				x->parent->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				_rb_tree_rotate_left(x->parent->parent, root);
			}
		}
	}

	root->color = _rb_tree_black;
}

//****************************_rb_tree_rotate_left()************************************
inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root) {

//��һ�����ı���ת����ҽڵ����ڵ�ָ����ת����ҽڵ㣨ͬʱ�ǵøı丸�ڵ㣩
//�ڶ������ı���ת���Լ����ҽڵ��λ�ã���ת���Ϊ��������ԭ������ת����ҽڵ��Ϊ֮ǰ��ת���λ�ã�ͬ���ǵø��¸��ڵ㣩
//������������֮ǰ�Ľڵ�

	_rb_tree_node_base* y = x->right;
	x->right = y->left;
	if (y->left != 0) {
		y->left->right = x;
	}
	y->parent = x->parent;

	if (x == root)
		root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;
}

//****************************_rb_tree_rotate_right()************************************
inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
	//��һ�����������ı仯����ͬ��

	_rb_tree_node_base* y = x->left;
	x->left = y->right;
	if (y->right != 0)
		y->right->parent = x;
	y->parent = x->parent;

	if (x == root) {
		root = y;
	}
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;

	y->right = x;
	x->parent = y;
}

// ɾ���ڵ��ʹ rb tree ����ƽ�⣬����һΪҪɾ���Ľڵ㣬������Ϊ���ڵ㣬������Ϊ��С�ڵ㣬������Ϊ���ڵ�
// 
// �ο�����: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
NodePtr _rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost)
{
	// y �ǿ��ܵ��滻�ڵ㣬ָ������Ҫɾ���Ľڵ�
	auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
	// x �� y ��һ�����ӽڵ�� NIL �ڵ�
	auto x = y->left != nullptr ? y->left : y->right;
	// xp Ϊ x �ĸ��ڵ�
	NodePtr xp = nullptr;

	// y != z ˵�� z �������ǿ��ӽڵ㣬��ʱ y ָ�� z ������������ڵ㣬x ָ�� y �����ӽڵ㡣
	// �� y ���� z ��λ�ã��� x ���� y ��λ�ã������ y ָ�� z
	if (y != z)
	{
		z->left->parent = y;
		y->left = z->left;

		// ��� y ���� z �����ӽڵ㣬��ô z �����ӽڵ�һ��������
		if (y != z->right)
		{ // x �滻 y ��λ��
			xp = y->parent;
			if (x != nullptr)
				x->parent = y->parent;

			y->parent->left = x;
			y->right = z->right;
			z->right->parent = y;
		}
		else
		{
			xp = y;
		}

		// ���� y �� z �ĸ��ڵ� 
		if (root == z)
			root = y;
		else if (rb_tree_is_lchild(z))
			z->parent->left = y;
		else
			z->parent->right = y;
		y->parent = z->parent;
		mystl::swap(y->color, z->color);
		y = z;
	}
	// y == z ˵�� z ����ֻ��һ������
	else
	{
		xp = y->parent;
		if (x)
			x->parent = y->parent;

		// ���� x �� z �ĸ��ڵ�
		if (root == z)
			root = x;
		else if (rb_tree_is_lchild(z))
			z->parent->left = x;
		else
			z->parent->right = x;

		// ��ʱ z �п���������ڵ�����ҽڵ㣬��������
		if (leftmost == z)
			leftmost = x == nullptr ? xp : rb_tree_min(x);
		if (rightmost == z)
			rightmost = x == nullptr ? xp : rb_tree_max(x);
	}
	// ��ʱ��y ָ��Ҫɾ���Ľڵ㣬x Ϊ����ڵ㣬�� x �ڵ㿪ʼ������
// ���ɾ���Ľڵ�Ϊ��ɫ����������û�б��ƻ����������������������x Ϊ���ӽڵ�Ϊ������
// case 1: �ֵܽڵ�Ϊ��ɫ����ڵ�Ϊ�죬�ֵܽڵ�Ϊ�ڣ��������ң�������������
// case 2: �ֵܽڵ�Ϊ��ɫ���������ӽڵ㶼Ϊ��ɫ�� NIL�����ֵܽڵ�Ϊ�죬���ڵ��Ϊ��ǰ�ڵ㣬��������
// case 3: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�� NIL�����ӽڵ�Ϊ��ɫ�� NIL��
//         ���ֵܽڵ�Ϊ�죬�ֵܽڵ�����ӽڵ�Ϊ�ڣ����ֵܽڵ�Ϊ֧���ң���������������
// case 4: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�����ֵܽڵ�Ϊ���ڵ����ɫ�����ڵ�Ϊ��ɫ���ֵܽڵ�����ӽڵ�
//         Ϊ��ɫ���Ը��ڵ�Ϊ֧�����ң������������ʵ�����ɣ��㷨����
	if (!rb_tree_is_red(y))
	{ // x Ϊ��ɫʱ������������ֱ�ӽ� x ��Ϊ��ɫ����
		while (x != root && (x == nullptr || !rb_tree_is_red(x)))
		{
			if (x == xp->left)
			{ // ��� x Ϊ���ӽڵ�
				auto brother = xp->right;
				if (rb_tree_is_red(brother))
				{ // case 1
					rb_tree_set_black(brother);
					rb_tree_set_red(xp);
					_rb_tree_rotate_left(xp, root);
					brother = xp->right;
				}
				// case 1 תΪΪ�� case 2��3��4 �е�һ��
				if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
					(brother->right == nullptr || !rb_tree_is_red(brother->right)))
				{ // case 2
					rb_tree_set_red(brother);
					x = xp;
					xp = xp->parent;
				}
				else
				{
					if (brother->right == nullptr || !rb_tree_is_red(brother->right))
					{ // case 3
						if (brother->left != nullptr)
							rb_tree_set_black(brother->left);
						rb_tree_set_red(brother);
						_rb_tree_rotate_right(brother, root);
						brother = xp->right;
					}
					// תΪ case 4
					brother->color = xp->color;
					rb_tree_set_black(xp);
					if (brother->right != nullptr)
						rb_tree_set_black(brother->right);
					_rb_tree_rotate_left(xp, root);
					break;
				}
			}
			else  // x Ϊ���ӽڵ㣬�Գƴ���
			{
				auto brother = xp->left;
				if (rb_tree_is_red(brother))
				{ // case 1
					rb_tree_set_black(brother);
					rb_tree_set_red(xp);
					_rb_tree_rotate_right(xp, root);
					brother = xp->left;
				}
				if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
					(brother->right == nullptr || !rb_tree_is_red(brother->right)))
				{ // case 2
					rb_tree_set_red(brother);
					x = xp;
					xp = xp->parent;
				}
				else
				{
					if (brother->left == nullptr || !rb_tree_is_red(brother->left))
					{ // case 3
						if (brother->right != nullptr)
							rb_tree_set_black(brother->right);
						rb_tree_set_red(brother);
						_rb_tree_rotate_left(brother, root);
						brother = xp->left;
					}
					// תΪ case 4
					brother->color = xp->color;
					rb_tree_set_black(xp);
					if (brother->left != nullptr)
						rb_tree_set_black(brother->left);
					_rb_tree_rotate_right(xp, root);
					break;
				}
			}
		}
		if (x != nullptr)
			rb_tree_set_black(x);
	}
	return y;
}

template <class Value>
struct _rb_tree_node :public _rb_tree_node_base {
	typedef _rb_tree_node<Value>* link_type;
	Value value_field;
};

//���������
struct _rb_tree_base_iterator
{
	typedef _rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;
	base_ptr node;//��������������һ�����ӹ�ϵ

	//++��������Ѱ�ұȵ�ǰ�������С�Ľ��
	void increment() {
		if (node->right != 0) {				//��������ӽڵ� ״��(1)
			node = node->right;				//��������
			while (node->left != nullptr) { //Ȼ��һֱ���������ߵ���
				node = node->left;			//���ǽ��
			}
		}
		else {								//û�����ӽڵ� ״��(2)
			base_ptr y = node->parent;		//�ҳ����ڵ�
			while (node == y->right) {		//������н�㱾�����һ�����ӽڵ�
				node = y;					//��һֱ���� ֱ����Ϊ���ӽڵ�Ϊֹ
				y = y->parent;
			}
			if (node->right != y) node = y;	//����ʱ���ӽڵ㲻���ڴ�ʱ�ĸ��ڵ� ״��(3)��ʱ�ĸ��ڵ㼴Ϊ�� �����ʱ��nodeΪ���״��(4)
		}
	}

	/*	ע�� �����жϡ�����ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ㡱����Ϊ��Ӧ��һ��
	*	���������������Ѱ�Ҹ�������һ�ڵ㣬��ǡ�ɸ��ڵ������ӽڵ㡣��Ȼ
	*	�������������������RB-tree �����������֮header֮��������ϵ
	*/

	//--��������Ѱ�ұȵ�ǰ���С��������
	void decrement() {
		if (node->color == _rb_tree_red && node->parent->parent == node)	//����Ǻ��� �Ҹ��ڵ�ĸ��ڵ�����Լ�
			node = node->right;												//״��(1)�ҽڵ㼴Ϊ���
		//�������������nodeΪheaderʱ������nodeΪend()ʱ��
		//ע�⣺header֮�ҽڵ㼴mpstright��ָ����������max���
		else if (node->left != 0) {		//��������ӽڵ� ״��(2)
			base_ptr y = node->left;	//��yָ�����ӽڵ�
			while (y->right != 0)		//��y�����ӽڵ�ʱ
				y = y->right;			//һֱ�����ӽڵ��ߵ���
			node = y;					//���Ϊ��
		}
		else {
			base_ptr y = node->parent;	//�ȷǸ��ڵ� �������ӽڵ�
			while (node == y->left) {	//״��3�ҳ����ڵ�
				node = y;				//�����н��Ϊ���ӽڵ�
				y = y->parent;			//һֱ���������ߣ�ֱ�����н�㲻Ϊ���ӽ��
			}
			node = y;					//��ʱ���ڵ㼴Ϊ��
		}
	}
};

template <class Value, class Ref, class Ptr>
struct _rb_tree_iterator :public _rb_tree_base_iterator
{
	typedef Value value_type;
	typedef Ref reference;
	typedef Ptr pointer;
	typedef _rb_tree_iterator<Value, Value&, Value*> iterator;
	typedef _rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
	typedef _rb_tree_iterator<Value, Ref, Ptr> self;
	typedef _rb_tree_node<Value>* link_type;

	_rb_tree_iterator() {}
	_rb_tree_iterator(link_type x) { node = x; }
	_rb_tree_iterator(const iterator& it) { node = it.node; }

	reference operator*() const {
		return link_type(node)->value_field;
	}

	pointer operator->()const {
		return &(operator*());
	}

	self& operator++() {
		increment();
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		increment();
		return tmp;
	}

	self& operator--() {
		decrement();
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		decrement();
		return tmp;
	}
};

//**********************rb_tree_min/max()*********************
template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept
{
	while (x->left != nullptr)
		x = x->left;
	return x;
}

template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept
{
	while (x->right != nullptr)
		x = x->right;
	return x;
}


template<class Key, class Value, class KeyValue, class Compare, class Alloc = alloc>
class rb_tree
{
protected:
	typedef rb_tree_value_traits<T> value_traits;
	typedef void* void_pointer;
	typedef _rb_tree_node_base* base_ptr;
	typedef _rb_tree_node<Value> rb_tree_node;
	typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
	typedef _rb_tree_color_type color_type;
public:
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type const_reference;
	typedef rb_tree_node* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
protected:

	//����һ�����
	link_type get_node() {
		return rb_tree_node_allocator::allocate();
	}
	//�ͷ�һ�����
	link_type put_node(link_type p) {
		rb_tree_node_allocator::deallocate(p);
	}
	//����һ����ֵ�Ľ��
	link_type create_node(const value_type& x) {
		link_type tmp = get_node();
		construct(&tmp->value_field, x);
		return tmp;
	}
	//����һ�����
	link_type clone_node(link_type x) {
		link_type tmp = create_node(x->value_field);
		tmp->color = x->color;
		tmp->left = 0;
		tmp->right = 0;
		return tmp;
	}
	//�ͷ�
	void destroy_node(link_type p) {
		destroy(&p->value_field);
		put_node(p);
	}
protected:
	size_type node_count;
	//һ�����⴦�� �����ӽڵ��ʾ��Сֵ ���ӽڵ��ʾ���ֵ
	link_type header;
	Compare key_compare;

	//��������������ȡ��header�ĳ�Ա
	link_type root() const {
		return (link_type)header->parent;
	}

	link_type leftmost() const {
		return (link_type)header->left;
	}

	link_type rightmost() const {
		return (link_type)header->right;
	}

	//����ȡ�ý��x
	static link_type& left(link_type x) {
		return (link_type&)(x->left);
	}

	static link_type& right(link_type x) {
		return (link_type&)(x->right);
	}

	static link_type& parent(link_type x) {
		return (link_type&)(x->parent);
	}

	static reference value(link_type x) {
		return x->value_field;
	}

	static Key& key(link_type x) {
		return KeyValue()(value(x));
	}

	static color_type& color(link_type x) {
		return (color_type&)(x->color);
	}

	//����Ҳ��Ϊ�˷���ȡ�ý��X�ĳ�Ա
	static link_type& left(base_ptr x) {
		return (link_type&)(x->left);
	}

	static link_type& right(base_ptr x) {
		return (link_type&)(x->right);
	}

	static link_type& parent(base_ptr x) {
		return (link_type&)(x->parent);
	}

	static reference value(base_ptr x) {
		return (link_type)x->value_field;
	}

	static Key& key(base_ptr x) {
		return KeyValue()(value(link_type(x)));
	}

	static color_type& color(base_ptr x) {
		return (color_type&)(link_type(x)->color);
	}

	//�󼫴�ֵ�ͼ�Сֵ
	static link_type minimum(link_type x) {
		return (link_type)_rb_tree_node_base::minimum(x);
	}
	static link_type maximum(link_type x) {
		return (link_type)_rb_tree_node_base::maximum(x);
	}

public:
	typedef _rb_tree_iterator<value_type, reference, pointer> iterator;
	typedef const _rb_tree_iterator<value_type, reference, pointer> const_iterator;
	typedef ministl::reverse_iterator<iterator>        reverse_iterator;
	typedef ministl::reverse_iterator<const_iterator>  const_reverse_iterator;

private:
	void erase_since(base_ptr x);//ɾ��x֮������н��
	iterator _insert(base_ptr x, base_ptr y, const value_type& v);
	base_ptr copy_from(base_ptr x, base_ptr p);//����һ����

	void init() {
		header = get_node();
		color(header) = _rb_tree_red;

		root() = 0;
		leftmost() = header;
		rightmost() = header;
	}

public:
	rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) {
		init();
	}
	~rb_tree() {
		clear();
		put_node(header);
	}
	rb_tree<Key, Value, KeyValue, Compare, Alloc>&
		operator=(const rb_tree<Key, Value, KeyValue, Compare, Alloc>& x);
public:

	Compare key_comp() const {
		return key_compare;
	}

	iterator begin() {
		return leftmost();
	}

	iterator end() {
		return header;
	}

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	bool empty() const {
		return node_count == 0;
	}

	size_type size() const {
		return node_count;
	}

	size_type max_size() const {
		return size_type(-1);
	}

	void erase(iterator first, iterator last);
	iterator erase(iterator it);

	void swap(rb_tree& rhs);
public:
	void clear();

	iterator find(const Key& k);

	pair<iterator, bool> insert_unique(const value_type& x);

	iterator insert_equal(const value_type& x);

	iterator       lower_bound(const key_type& key);
	const_iterator lower_bound(const key_type& key) const;

	iterator       upper_bound(const key_type& key);
	const_iterator upper_bound(const key_type& key) const;

	//ֱ��ѡ��һ������ ����������� ��Сֵ�պô���key ���ֵ�պ�С��key
	ministl::pair<iterator, iterator>
		equal_range_multi(const key_type& key)
	{
		return ministl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}
	ministl::pair<const_iterator, const_iterator>
		equal_range_multi(const key_type& key) const
	{
		return ministl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
	}

	ministl::pair<iterator, iterator>
		equal_range_unique(const key_type& key)
	{
		iterator it = find(key);
		auto next = it;
		return it == end() ? ministl::make_pair(it, it) : ministl::make_pair(it, ++next);
	}
	ministl::pair<const_iterator, const_iterator>
		equal_range_unique(const key_type& key) const
	{
		const_iterator it = find(key);
		auto next = it;
		return it == end() ? ministl::make_pair(it, it) : ministl::make_pair(it, ++next);
	}

};

//*******************************swap()*****************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
swap(rb_tree& rhs) noexcept
{
	if (this != &rhs)
	{
		ministl::swap(header, rhs.header);
		ministl::swap(node_count, rhs.node_count);
		ministl::swap(key_comp, rhs.key_comp);
	}
}

//***********************************clear()**************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
clear()
{
	if (node_count != 0)
	{
		erase_since(root());
		leftmost() = header;
		root() = nullptr;
		rightmost() = header;
		node_count = 0;
	}
}

//**********************************operator=()*********************
// ���Ƹ�ֵ������
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator=(const rb_tree& rhs)
{
	if (this != &rhs)
	{
		clear();

		if (rhs.node_count != 0)
		{
			root() = copy_from(rhs.root(), header);
			leftmost() = rb_tree_min(root());
			rightmost() = rb_tree_max(root());
		}

		node_count = rhs.node_count;
		key_comp = rhs.key_comp;
	}
	return *this;
}

// copy_from ����
// �ݹ鸴��һ�������ڵ�� x ��ʼ��p Ϊ x �ĸ��ڵ�
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::base_ptr
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::copy_from(base_ptr x, base_ptr p)
{
	auto top = clone_node(x);
	top->parent = p;
	if (x->right)
		top->right = copy_from(x->right, top);
	p = top;
	x = x->left;
	while (x != nullptr)
	{
		auto y = clone_node(x);
		p->left = y;
		y->parent = p;
		if (x->right)
			y->right = copy_from(x->right, y);
		p = y;
		x = x->left;
	}
	
	return top;
}

//*******************************erase()***************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
erase(iterator hint)
{
	auto node = hint.node->get_node_ptr();
	iterator next(node);
	++next;

	_rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
	destroy_node(node);
	--node_count;
	return next;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
erase(iterator first, iterator last)
{
	if (first == begin() && last == end())
	{
		clear();
	}
	else
	{
		while (first != last)
			erase(first++);
	}
}

// erase_since ����
// �� x �ڵ㿪ʼɾ���ýڵ㼰������
// �ݹ����
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
erase_since(base_ptr x)
{
	while (x != nullptr)
	{
		erase_since(x->right);
		auto y = x->left;
		destroy_node(x->get_node_ptr());
		x = y;
	}
}

//**************************find()*************************************
template<class Key,class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& v) {
	link_type y = header;
	link_type x = root();

	while (x != 0) {
		if (!key_compare(key(x), k)) {
			y = x;
			x = left(x);
		}
		else
			x = right(x);
	}

	iterator j = iterator(y);
	return (j == end() || key_compare(k, key(j.node))) ? end(), j);
}

//***************************insert_equal()*******************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
	link_type y = header;
	link_type x = root();
	
	while (x != 0) {
		y = x;
		x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		//���ϣ�����������ߣ���С���ߵ���������
		//ע������Ĵ�С�Ƚ���v��x���бȽϣ����v����x�Ļ�����ôv��������󣬷��������
	}
	//��xΪ��ֵ����㣬yΪ�����ĸ��ڵ㣬vΪ��ֵ
	return _insert(x, y, v);
}

//***************************insert_unique()*******************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
pair<typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
	link_type y = header;
	link_type x - root();
	bool comp = true;
	while (x != 0)
	{
		y = x;
		comp = key_compare(KeyOfValue()(v), key(x));
		x = comp ? left(x) : right(x);
	}
	//ͬ��������������� ����С������
	iterator j = iterator(y);//�������ָ������ĸ��ڵ�
	if (comp) {//����뿪ѭ��ʱcompΪtrue��ʾ�ڲ��������
		if (j == begin()) {
			//���������������
			//xΪ����㣬yΪ�����ĸ��ڵ㣬vΪ��ֵ
			return pair<iterator, bool>(_insert(x, y, v), true);
		}
		else {
			--j;
		}
	}
	//С����ֵ ��ʾ����С �������Ҳ�
	if (key_compare(key(j->node), KeyOfValue()(v)) {
		return pair<iterator, bool>(_insert(x, y, v), true);
	}

	return pair<iterator, bool>(j, false);
}

//********************_insert()***********************
template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
_insert(base_ptr x_, base_ptr y_, const value_type& v) {
	//x��ֵ����� y�ǲ����ĸ��ڵ� v����ֵ
	link_type x = (link_type)x_;
	link_type y = (link_type)y_;
	link_type z;

	//�Ǽ�ֵ�Ƚϴ�С׼�� 
	if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
		z = create_node(v);
		left(y) = z;//��ʹ�õ�y��Ϊheader()ʱ��leftmost() = z;
		if (y == header) {
			root() = z;
			rightmost() = z;
		}
		else if (y == leftmost()){ //���yΪ������
			leftmost() = z;//ά��leftmostʹ����Զָ������ڵ�
		}
	}
	else {
		z = create_node(v);
		right(y) = z;
		if (y == rightmost()) //ͬ����ά��rightmost�ڵ�
			rightmost() = z;
	}

	parent(z) = y;//�趨�µĸ��ڵ�
	left(z) = 0;//�趨�½������ӽڵ�
	right(z) = 0;//�趨�½������ӽڵ�

	//�����¼���Ľڵ���ɫ
	_rb_tree_rebalance(z, header->parent);
	//���ӽڵ���Ŀ
	++node_count;
	return iterator(z);
}

//***************************lower_bound()******************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
lower_bound(const key_type& key)
{
	auto y = header;
	auto x = root();
	while (x != nullptr)
	{
		if (!key_comp(value_traits::get_key(x->get_node_ptr()->value), key))
		{ // x��value��key�Ƚ� ���x���ڵ���key�Ļ��������ߣ���������ұ���
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
lower_bound(const key_type& key) const
{
	auto y = header;
	auto x = root();
	while (x != nullptr)
	{
		if (!key_comp(value_traits::get_key(x->get_node_ptr()->value), key))
		{ // key <= x ���x���ڵ���key�Ļ��������ߣ���֮���ұ���
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return const_iterator(y);
}

//**********************upper_bound()****************************
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
upper_bound(const key_type& key)
{
	auto y = header;
	auto x = root();
	while (x != nullptr)
	{
		if (key_comp(key, value_traits::get_key(x->get_node_ptr()->value)))
		{ // key < x �������պ��෴
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
upper_bound(const key_type& key) const
{
	auto y = header;
	auto x = root();
	while (x != nullptr)
	{
		if (key_comp(key, value_traits::get_key(x->get_node_ptr()->value)))
		{ // key < x
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return const_iterator(y);
}

// ���رȽϲ�����
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return lhs.size() == rhs.size() && ministl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return ministl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator>(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return rhs < lhs;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator<=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool operator>=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs, 
	const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
{
	return !(lhs < rhs);
}


}//!ministl


#endif
