#pragma once

#include "Core/Definitions.h"
#include <xtree>

NAMESPACE_START(Gear)

enum class EColor : uint8_t
{
	Red,
	Black
};

template<class _tree, class _base = std::_Iterator_base0>
class tree_unchecked_const_iterator : public _base
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using nodePtr = typename _tree::nodePtr;
	using value_type = typename _tree::value_type;
	using difference_type = typename _tree::difference_type;
	using pointer = typename _tree::const_pointer;
	using reference = const value_type&;

	tree_unchecked_const_iterator()
		: m_Ptr()
	{
	}

	tree_unchecked_const_iterator(nodePtr node, const _tree* tree)
		: m_Ptr(node)
	{
		this->_Adopt(list);
	}

	reference operator*() const
	{
		return m_Ptr->Value;
	}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	tree_unchecked_const_iterator& operator++()
	{
		if (m_Ptr->Right->IsNil)
		{
			nodePtr node;
			while (!(node = m_Ptr->Parent)->IsNil && Ptr == node->Right)
			{
				m_Ptr = node;
			}

			m_Ptr = node;
		}
		else
		{
			m_Ptr = _tree::min(m_Ptr->Right);
		}

		return *this;
	}

	tree_unchecked_const_iterator operator++(int)
	{
		tree_unchecked_const_iterator tmp = *this;
		++*this;
		return tmp;
	}

	nodePtr m_Ptr;
};

template<class _tree>
class tree_unchecked_iterator : public tree_unchecked_const_iterator<_tree>
{
};

template<class _tree>
class tree_const_iterator : public tree_unchecked_const_iterator<_tree, std::_Iterator_base>
{
public:
	using base = tree_unchecked_const_iterator<_tree, std::_Iterator_base>;
	using iterator_category = std::bidirectional_iterator_tag;
	using nodePtr = typename _tree::nodePtr;
	using value_type = typename _tree::value_type;
	using difference_type = typename _tree::difference_type;
	using pointer = typename _tree::const_pointer;
	using reference = const value_type&;
};

template<class _tree>
class tree_iterator : public tree_const_iterator<_tree>
{
};

template<class _value_type,
	class _size_type,
	class _difference_type,
	class _pointer,
	class _const_pointer,
	class _reference,
	class _const_reference,
	class _nodeptr_type>
	struct tree_iter_types
{
	using value_type = _value_type;
	using size_type = _size_type;
	using difference_type = _difference_type;
	using pointer = _pointer;
	using const_pointer = _const_pointer;
	using nodeptr = _nodeptr_type;
};

template<class _value_type, class _voidptr>
struct tree_node
{
	using nodeptr = std::_Rebind_pointer_t<_voidptr, tree_node>;

	nodeptr Left;
	nodeptr Parent;
	nodeptr Right;
	EColor Color;
	bool8_t IsNil;
	_value_type Value;

	tree_node& operator=(const tree_node&) = delete;

	template<class _alloc>
	static void freeNode(_alloc& al, nodeptr ptr) noexcept
	{
		using alnode = std::_Rebind_alloc_t<_alloc, tree_node>;
		using alnode_traits = std::allocator_traits<alnode>;

		alnode node(al);
		alnode_traits::destroy(node, std::addressof(ptr->Left));
		alnode_traits::destroy(node, std::addressof(ptr->Parent));
		alnode_traits::destroy(node, std::addressof(ptr->Right));
		alnode_traits::deallocate(node, ptr, 1);
	}
};

template<class _t>
struct tree_simple_types : std::_Simple_types<_t>
{
	using node = tree_node<_T, void*>;
	using nodeptr = node*;
};

template<class _t, class _alloc>
struct tree_base_types
{
	using alty = std::_Rebind_alloc_t<_alloc, _t>;
	using alty_traits = std::allocator_traits<alty>;
	using node = tree_node<_t, typename std::allocator_traits<_alloc>::void_pointer>;
	using alnode = std::_Rebind_alloc_t<_alloc, node>;
	using alnode_traits = std::allocator_traits<alnode>;
	using nodeptr = typename alnode_traits::pointer;

	using val_types = std::conditional_t<std::_Is_simple_alloc_v<alnode>,
		tree_simple_types<_t>,
		tree_iter_types<_t,
			typename alnode_traits::size_type,
			typename alnode_traits::difference_type,
			typename alnode_traits::pointer,
			typename alnode_traits::const_pointer,
			T&,
			const T&,
			nodeptr>> ;
};

enum class tree_child
{
	Right,
	Left,
	Unused
};

template<class _nodeptr>
struct tree_id
{
	_nodeptr Parent;
	tree_child Child;
};

template<class _nodeptr>
struct tree_find_result
{
	tree_id<_nodeptr> Location;
	_nodeptr Bound;
};

template<class _nodeptr>
struct tree_find_hint_result
{
	tree_id<_nodeptr> Location;
	bool Duplicate;
};

template<class _val_types>
class tree_val : public std::_Container_base
{
public:
	using nodeptr = typename _val_types::nodeptr;

	using value_type = typename _val_types::value_type;
	using size_type = typename _val_types::size_type;
	using difference_type = typename _val_types::difference_type;
	using pointer = typename _val_types::pointer;
	using const_pointer = typename _val_types::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;

	using unchecked_const_iterator = tree_unchecked_const_iterator<tree_val>;
	using const_iterator = tree_const_iterator<tree_val>;

	tree_val() noexcept
		: m_Head()
		, m_Size(0)
	{
	}

	static nodeptr max(nodeptr node)
	{
		while (!node->Right->IsNil)
		{
			node = node->Right;
		}
		return node;
	}

	static nodeptr min(nodeptr node)
	{
		while (!node->Left->IsNil)
		{
			node = node->Left;
		}
		return node;
	}

	nodeptr& root() const
	{
		return m_Head->Parent;
	}

	nodeptr& lMost() const
	{
		return m_Head->Left;
	}

	nodeptr& rMost() const
	{
		return m_Head->Right;
	}

	void lRotate(nodeptr whereNode) noexcept
	{
		nodeptr node = whereNode->Right;
		whereNode->Right = node->Left;

		if (!node->Left->IsNil)
		{
			node->Left->Parent = whereNode;
		}

		node->Parent = whereNode->Parent;

		if (whereNode == m_Head->Parent)
		{
			m_Head->Parent = node;
		}
		else if (whereNode == whereNode->Parent->Left)
		{
			whereNode->Parent->Left = node;
		}
		else
		{
			whereNode->Parent->Right = node;
		}

		node->Left = whereNode;
		whereNode->Parent = node;
	}

	void rRotate(nodeptr whereNode) noexcept
	{
		nodeptr node = whereNode->Left;
		whereNode->Left = node->Right;

		if (!node->Right->IsNil)
		{
			node->Right->Parent = whereNode;
		}

		node->Parent = whereNode->Parent;

		if (whereNode == m_Head->Parent)
		{
			m_Head->Parent = node;
		}
		else if (whereNode == whereNode->Parent->Right)
		{
			whereNode->Parent->Right = node;
		}
		else
		{
			whereNode->Parent->Left = node;
		}

		node->Right = whereNode;
		whereNode->Parent = node;
	}

	nodeptr insert(const tree_id<nodeptr> loc, const nodeptr newNode) noexcept
	{
		++m_Size;
		const auto head = m_Head;
		newNode->Parent = loc.Parent;
		if (loc.Parent == head)
		{
			head->Left = newNode;
			head->Parent = newNode;
			head->Right = newNode;
			newNode->Color = EColor::Black;
			return newNode;
		}

		assert(loc.Child != tree_child::Unused);
		if (loc.Child == tree_child::Right)
		{
			assert(loc.Parent->Right->IsNil);
			loc.Parent->Right = newNode;
			if (loc.Parent == head->Right)
			{
				head->Right = newNode;
			}
		}
		else
		{
			assert(loc.Parent->Left->IsNil);
			loc.Parent->Left = newNode;
			if (loc.Parent == head->Left)
			{
				head->Left = newNode;
			}
		}

		for (nodeptr pnode = newNode; pnode->Parent->Color == EColor::Red;)
		{
			if (pnode->Parent == pnode->Parent->Parent->Left)
			{
				const auto parent_sibling = pnode->Parent->Parent->Right;
				if (parent_sibling->Color == EColor::Red)
				{
					pnode->Parent->Color = EColor::Black;
					parent_sibling->Color = EColor::Black;
					pnode->Parent->Parent->Color = EColor::Red;
					pnode = pnode->Parent->Parent;
				}
				else
				{
					if (pnode == pnode->Parent->Right)
					{
						pnode = pnode->Parent;
						lRotate(pnode);
					}

					pnode->Parent->Color = EColor::Black;
					pnode->Parent->Parent->Color = EColor::Red;
					rRotate(pnode->Parent->Parent);
				}
			}
			else
			{
				const auto parent_sibling = pnode->Parent->Parent->Left;
				if (parent_sibling->Color == EColor::Red)
				{
					pnode->Parent->Color = EColor::Black;
					parent_sibling->Color = EColor::Black;
					pnode->Parent->Parent->Color = EColor::Red;
					pnode = pnode->Parent->Parent;
				}
				else
				{
					if (pnode == pnode->Parent->Left)
					{
						pnode = pnode->Parent;
						rRotate(pnode);
					}

					pnode->Parent->Color = EColor::Black;
					pnode->Parent->Parent->Color = EColor::Red;
					lRotate(pnode->Parent->Parent);
				}
			}
		}

		head->Parent->Color = EColor::Black;
		return newNode;
	}

	nodeptr m_Head;
	size_type m_Size;
};

template<class _traits>
class tree_comp_alloc
{
public:
	using allocator_type = typename _traits::allocator_type;
	using key_compare = typename _traits::key_compare;

	using alloc_types = tree_base_types<typename _traits::value_type, allocator_type>;

	using alnode = typename alloc_types::alnode;
	using alnode_traits = typename alloc_types::alnode_traits;
	using alproxy = std::_Rebind_alloc_t<alnode, std::_Container_proxy>;
	using alproxy_traits = std::allocator_traits<alproxy>;
	using node = typename alloc_types::node;
	using nodeptr = typename alloc_types::nodeptr;
	using val_types = typename alloc_types::val_types;

	using value_type = typename val_types::value_type;
	using size_type = typename val_types::size_type;
	using difference_type = typename val_types::difference_type;
	using pointer = typename val_types::pointer;
	using const_pointer = typename val_types:const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;

	using iterator = tree_iterator<tree_val<val_types>>;
	using const_iterator = tree_const_iterator<tree_val<val_types>>;
	using unchecked_iterator = tree_unchecked_iterator<tree_val<val_types>>;
	using unchecked_const_iterator = tree_unchecked_const_iterator<tree_val<val_types>>;
};

template<class _traits>
class red_black_tree : public tree_comp_alloc<_traits>
{
public:
protected:
private:
};

NAMESPACE_END(Gear)