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
static _rb_tree_color_type _rb_tree_red = false; //红色为0
static _rb_tree_color_type _rb_tree_black = true;//黑色为1

//结点设计
struct _rb_tree_node_base
{
	typedef _rb_tree_color_type color_type;
	typedef _rb_tree_node_base* base_ptr;

	color_type color;//节点颜色，非黑即红
	base_ptr parent;//RB树的许多操作必须知道父节点
	base_ptr left;//指向左节点
	base_ptr right;//指向右节点

	static base_ptr minimum(base_ptr x) {
		while (x->left != nullptr) x = x->left;
		return x;
	}

	static base_ptr maximum(base_ptr x) {
		while (x->right != 0) x = x->right;
		return x;
	}
};

//键值是相同的
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
	//在树的结构中 下一个节点就是右节点的左子树一直往左
	if (node->right != nullptr)
		return rb_tree_min(node->right);
	//如果不是该节点不是左子节点的话 就一直往上走 直到他是左子节点后返回他的父节点
	while (!rb_tree_is_lchild(node))
		node = node->parent;
	return node->parent;
}

//************************_rb_tree_rebalance()*****************************
inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base* &root) {
	/*
	* 在这里的调整过程中，每一个新插入的节点的颜色都是红色（如果是黑色那么所有的树节点都是黑色了，就没有意义了）
	* 对于插入节点后，如果节点的父节点以及叔叔节点是红色，那么就将父节点以及叔叔节点都变为黑色，并且将爷爷节点改为红色
	* 之后如果条件还是不满足的话，就进行旋转操作
	* 在其中对于不同的旋转有着不同的要求：
	* 左旋：
	*	倘若父节点是红色，叔叔节点是黑色 并且当前结点是右节点，进行左旋操作
	* 右旋：
	*	倘若父节点是红色，叔叔节点是黑色，并且当前节点是左节点，就进行右旋操作
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
					_rb_tree_rotate_left(x, root);//左旋
				}

				//与左旋相比，右旋稍微麻烦一些，就是要将其父节点变为黑色，将爷爷节点变为红色
				x->parent->color = _rb_tree_black;
				x->parent->parent->color = _rb_tree_red;
				_rb_tree_rotate_right(x->parent->parent, root);//右旋
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

//第一步：改变旋转点的右节点的左节点指向旋转点的右节点（同时记得改变父节点）
//第二步：改变旋转点以及其右节点的位置，旋转点变为左子树，原本的旋转点的右节点变为之前旋转点的位置（同样记得更新父节点）
//第三步：更新之前的节点

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
	//第一步：与左旋的变化是相同的

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

// 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
// 
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
NodePtr _rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost)
{
	// y 是可能的替换节点，指向最终要删除的节点
	auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
	// x 是 y 的一个独子节点或 NIL 节点
	auto x = y->left != nullptr ? y->left : y->right;
	// xp 为 x 的父节点
	NodePtr xp = nullptr;

	// y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
	// 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
	if (y != z)
	{
		z->left->parent = y;
		y->left = z->left;

		// 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
		if (y != z->right)
		{ // x 替换 y 的位置
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

		// 连接 y 与 z 的父节点 
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
	// y == z 说明 z 至多只有一个孩子
	else
	{
		xp = y->parent;
		if (x)
			x->parent = y->parent;

		// 连接 x 与 z 的父节点
		if (root == z)
			root = x;
		else if (rb_tree_is_lchild(z))
			z->parent->left = x;
		else
			z->parent->right = x;

		// 此时 z 有可能是最左节点或最右节点，更新数据
		if (leftmost == z)
			leftmost = x == nullptr ? xp : rb_tree_min(x);
		if (rightmost == z)
			rightmost = x == nullptr ? xp : rb_tree_max(x);
	}
	// 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
// 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
// case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
// case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
// case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
//         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
// case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
//         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
	if (!rb_tree_is_red(y))
	{ // x 为黑色时，调整，否则直接将 x 变为黑色即可
		while (x != root && (x == nullptr || !rb_tree_is_red(x)))
		{
			if (x == xp->left)
			{ // 如果 x 为左子节点
				auto brother = xp->right;
				if (rb_tree_is_red(brother))
				{ // case 1
					rb_tree_set_black(brother);
					rb_tree_set_red(xp);
					_rb_tree_rotate_left(xp, root);
					brother = xp->right;
				}
				// case 1 转为为了 case 2、3、4 中的一种
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
					// 转为 case 4
					brother->color = xp->color;
					rb_tree_set_black(xp);
					if (brother->right != nullptr)
						rb_tree_set_black(brother->right);
					_rb_tree_rotate_left(xp, root);
					break;
				}
			}
			else  // x 为右子节点，对称处理
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
					// 转为 case 4
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

//迭代器设计
struct _rb_tree_base_iterator
{
	typedef _rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;
	base_ptr node;//用来与容器产生一个连接关系

	//++操作就是寻找比当前结点大的最小的结点
	void increment() {
		if (node->right != 0) {				//如果有右子节点 状况(1)
			node = node->right;				//就向右走
			while (node->left != nullptr) { //然后一直往左子树走到底
				node = node->left;			//即是解答
			}
		}
		else {								//没有右子节点 状况(2)
			base_ptr y = node->parent;		//找出父节点
			while (node == y->right) {		//如果现行结点本身就是一个右子节点
				node = y;					//就一直上溯 直到不为右子节点为止
				y = y->parent;
			}
			if (node->right != y) node = y;	//若此时右子节点不等于此时的父节点 状况(3)此时的父节点即为答案 否则此时的node为解答，状况(4)
		}
	}

	/*	注意 以上判断“若此时的右子节点不等于此时的父节点”，是为了应付一种
	*	特殊情况：我们欲寻找根结点的下一节点，而恰巧根节点无右子节点。当然
	*	以上特殊做法必须配合RB-tree 根结点与特殊之header之间的特殊关系
	*/

	//--操作就是寻找比当前结点小的最大的数
	void decrement() {
		if (node->color == _rb_tree_red && node->parent->parent == node)	//如果是红结点 且父节点的父节点等于自己
			node = node->right;												//状况(1)右节点即为解答
		//以上情况发生于node为header时，即（node为end()时）
		//注意：header之右节点即mpstright，指向整棵树的max结点
		else if (node->left != 0) {		//如果有左子节点 状况(2)
			base_ptr y = node->left;	//令y指向左子节点
			while (y->right != 0)		//当y有右子节点时
				y = y->right;			//一直往右子节点走到底
			node = y;					//最后即为答案
		}
		else {
			base_ptr y = node->parent;	//既非根节点 亦无左子节点
			while (node == y->left) {	//状况3找出父节点
				node = y;				//当现行结点为左子节点
				y = y->parent;			//一直交替往上走，直到现行结点不为左子结点
			}
			node = y;					//此时父节点即为答案
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

	//开辟一个结点
	link_type get_node() {
		return rb_tree_node_allocator::allocate();
	}
	//释放一个结点
	link_type put_node(link_type p) {
		rb_tree_node_allocator::deallocate(p);
	}
	//创建一个带值的结点
	link_type create_node(const value_type& x) {
		link_type tmp = get_node();
		construct(&tmp->value_field, x);
		return tmp;
	}
	//复制一个结点
	link_type clone_node(link_type x) {
		link_type tmp = create_node(x->value_field);
		tmp->color = x->color;
		tmp->left = 0;
		tmp->right = 0;
		return tmp;
	}
	//释放
	void destroy_node(link_type p) {
		destroy(&p->value_field);
		put_node(p);
	}
protected:
	size_type node_count;
	//一种特殊处理 其左子节点表示最小值 右子节点表示最大值
	link_type header;
	Compare key_compare;

	//用以下三个函数取得header的成员
	link_type root() const {
		return (link_type)header->parent;
	}

	link_type leftmost() const {
		return (link_type)header->left;
	}

	link_type rightmost() const {
		return (link_type)header->right;
	}

	//以下取得结点x
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

	//以下也是为了方便取得结点X的成员
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

	//求极大值和极小值
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
	void erase_since(base_ptr x);//删除x之后的所有结点
	iterator _insert(base_ptr x, base_ptr y, const value_type& v);
	base_ptr copy_from(base_ptr x, base_ptr p);//复制一棵树

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

	//直接选择一个区间 在这个区间中 最小值刚好大于key 最大值刚好小于key
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
// 复制赋值操作符
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

// copy_from 函数
// 递归复制一颗树，节点从 x 开始，p 为 x 的父节点
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

// erase_since 函数
// 从 x 节点开始删除该节点及其子树
// 递归调用
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
		//以上，遇大则往左边，遇小或者等于则往右
		//注：这里的大小比较是v与x进行比较，如果v大于x的话，那么v遇大就往左，否则就往右
	}
	//以x为新值插入点，y为插入点的父节点，v为新值
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
	//同样的遇到大的往左 遇到小的往右
	iterator j = iterator(y);//令迭代器指向插入点的父节点
	if (comp) {//如果离开循环时comp为true表示在插入点的左侧
		if (j == begin()) {
			//如果插入点是最左侧
			//x为插入点，y为插入点的父节点，v为新值
			return pair<iterator, bool>(_insert(x, y, v), true);
		}
		else {
			--j;
		}
	}
	//小于新值 表示遇到小 将插于右侧
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
	//x新值插入点 y是插入点的父节点 v是新值
	link_type x = (link_type)x_;
	link_type y = (link_type)y_;
	link_type z;

	//是键值比较大小准则 
	if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
		z = create_node(v);
		left(y) = z;//这使得当y即为header()时，leftmost() = z;
		if (y == header) {
			root() = z;
			rightmost() = z;
		}
		else if (y == leftmost()){ //如果y为最左结点
			leftmost() = z;//维护leftmost使它永远指向最左节点
		}
	}
	else {
		z = create_node(v);
		right(y) = z;
		if (y == rightmost()) //同样的维护rightmost节点
			rightmost() = z;
	}

	parent(z) = y;//设定新的父节点
	left(z) = 0;//设定新结点的左子节点
	right(z) = 0;//设定新结点的右子节点

	//调整新加入的节点颜色
	_rb_tree_rebalance(z, header->parent);
	//增加节点数目
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
		{ // x的value与key比较 如果x大于等于key的话就往左走，否则就往右边走
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
		{ // key <= x 如果x大于等于key的话就往左走，反之往右边走
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
		{ // key < x 于上述刚好相反
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

// 重载比较操作符
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
