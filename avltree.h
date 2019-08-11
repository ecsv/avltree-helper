/* SPDX-License-Identifier: MIT */
/* Minimal AVL-tree helper functions
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

/* inject the balance info in the lowest two bits of the parent pointer */
#define AVL_PARENT_BALANCE_COMBINATION

#if defined(__GNUC__)
#define AVLTREE_TYPEOF_USE 1
#define AVL_NODE_ALIGNED __attribute__ ((aligned(sizeof(unsigned long))))
#endif

#if defined(_MSC_VER)
#define __inline__ __inline
#define AVL_NODE_ALIGNED __declspec(align(sizeof(unsigned long)))
#endif

/**
 * container_of() - Calculate address of object that contains address ptr
 * @ptr: pointer to member variable
 * @type: type of the structure containing ptr
 * @member: name of the member variable in struct @type
 *
 * Return: @type pointer of object containing ptr
 */
#ifndef container_of
#ifdef AVLTREE_TYPEOF_USE
#define container_of(ptr, type, member) __extension__ ({ \
	const __typeof__(((type *)0)->member) *__pmember = (ptr); \
	(type *)((char *)__pmember - offsetof(type, member)); })
#else
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#endif
#endif

/**
 * enum avl_node_balance - balance type of node in avl_tree
 * @AVL_NEUTRAL: depth of left and right subtree are the same
 * @AVL_LEFT: depth of left subtree is one higher than right subtree
 * @AVL_RIGHT: depth of right subtree is one higher than left subtree
 */
enum avl_node_balance {
	AVL_NEUTRAL = 0,
	AVL_LEFT = 1,
	AVL_RIGHT = 2
};

/**
 * struct avl_node - node of an avl tree
 * @parent: pointer to the parent node in the tree
 * @balance: balance of the node
 * @parent_balance: combination of @parent and @balance (lowest two bits)
 * @left: pointer to the left child in the tree
 * @right: pointer to the right child in the tree
 *
 * The avl tree consists of a root and nodes attached to this root. The
 * avl_* functions and macros can be used to access and modify this data
 * structure.
 *
 * The @parent pointer of the avl node points to the parent avl node in the
 * tree, the @left to the left "smaller key" child and @right to the right
 * "larger key" node of the tree.
 *
 * The avl nodes are usually embedded in a container structure which holds the
 * actual data. Such an container object is called entry. The helper avl_entry
 * can be used to calculate the object address from the address of the node.
 */
struct avl_node {
#ifndef AVL_PARENT_BALANCE_COMBINATION
	struct avl_node *parent;
	enum avl_node_balance balance;
#else
	unsigned long parent_balance;
#endif
	struct avl_node *left;
	struct avl_node *right;
} AVL_NODE_ALIGNED;

/**
 * struct avl_root - root of an avl-tree
 * @node: pointer to the root node in the tree
 *
 * For an empty tree, node points to NULL.
 */
struct avl_root {
	struct avl_node *node;
};

/**
 * DEFINE_AVLROOT - define tree root and initialize it
 * @root: name of the new object
 */
#define DEFINE_AVLROOT(root) \
	struct avl_root root = { NULL }

/**
 * INIT_AVL_ROOT() - Initialize empty tree
 * @root: pointer to avl root
 */
static __inline__ void INIT_AVL_ROOT(struct avl_root *root)
{
	root->node = NULL;
}

/**
 * avl_empty() - Check if tree has no nodes attached
 * @root: pointer to the root of the tree
 *
 * Return: 0 - tree is not empty !0 - tree is empty
 */
static __inline__ int avl_empty(const struct avl_root *root)
{
	return !root->node;
}

/**
 * avl_parent() - Get parent of node
 * @node: pointer to the avl node
 *
 * Return: avl parent node of @node
 */
static __inline__ struct avl_node *avl_parent(struct avl_node *node)
{
#ifndef AVL_PARENT_BALANCE_COMBINATION
	return node->parent;
#else
	return (struct avl_node *)(node->parent_balance & ~3lu);
#endif
}

/**
 * avl_balance() - Get balance of node
 * @node: pointer to the avl node
 *
 * Return: balance of @node
 */
static __inline__ enum avl_node_balance avl_balance(const struct avl_node *node)
{
#ifndef AVL_PARENT_BALANCE_COMBINATION
	return node->balance;
#else
	return (enum avl_node_balance)(node->parent_balance & 3lu);
#endif
}

/**
 * avl_set_parent_balance() - Set parent and balance of node
 * @node: pointer to the avl node
 * @parent: pointer to the new parent node
 * @balance: new balance of the node
 */
static __inline__ void avl_set_parent_balance(struct avl_node *node,
					      struct avl_node *parent,
					      enum avl_node_balance balance)
{
#ifndef AVL_PARENT_BALANCE_COMBINATION
	node->parent = parent;
	node->balance = balance;
#else
	node->parent_balance = (unsigned long)parent | balance;
#endif
}

/**
 * avl_link_node() - Add new node as new leaf
 * @node: pointer to the new node
 * @parent: pointer to the parent node
 * @avl_link: pointer to the left/right pointer of @parent
 *
 * @node will be initialized as leaf node of @parent. It will be linked to the
 * tree via the @avl_link pointer. @parent must be NULL and @avl_link has to
 * point to "node" of avl_root when the tree is empty.
 *
 * WARNING The new node may cause the tree to be become unbalanced or violate
 * any rules of the avl tree. A call to avl_insert_balance after avl_link_node
 * is therefore always required to rebalance the tree correctly. avl_insert can
 * be used as helper to run both steps at the same time.
 */
static __inline__ void avl_link_node(struct avl_node *node,
				     struct avl_node *parent,
				     struct avl_node **avl_link)
{
	avl_set_parent_balance(node, parent, AVL_NEUTRAL);
	node->left = NULL;
	node->right = NULL;

	*avl_link = node;
}

void avl_insert_balance(struct avl_node *node, struct avl_root *root);

/**
 * avl_insert() - Add new node as new leaf and rebalance tree
 * @node: pointer to the new node
 * @parent: pointer to the parent node
 * @avl_link: pointer to the left/right pointer of @parent
 * @root: pointer to avl root
 */
static __inline__ void avl_insert(struct avl_node *node,
				  struct avl_node *parent,
				  struct avl_node **avl_link,
				  struct avl_root *root)
{
	avl_link_node(node, parent, avl_link);
	avl_insert_balance(node, root);
}

struct avl_node *avl_erase_node(struct avl_node *node, struct avl_root *root,
				bool *removed_right);
void avl_erase_balance(struct avl_node *parent, bool removed_right,
		       struct avl_root *root);

/**
 * avl_erase() - Remove avl node from tree and rebalance tree
 * @node: pointer to the node
 * @root: pointer to avl root
 */
static __inline__ void avl_erase(struct avl_node *node, struct avl_root *root)
{
	struct avl_node *decreased_node;
	bool removed_right;

	decreased_node = avl_erase_node(node, root, &removed_right);
	if (decreased_node)
		avl_erase_balance(decreased_node, removed_right, root);
}

struct avl_node *avl_first(const struct avl_root *root);
struct avl_node *avl_last(const struct avl_root *root);
struct avl_node *avl_next(struct avl_node *node);
struct avl_node *avl_prev(struct avl_node *node);

/**
 * avl_entry() - Calculate address of entry that contains tree node
 * @node: pointer to tree node
 * @type: type of the entry containing the tree node
 * @member: name of the avl_node member variable in struct @type
 *
 * Return: @type pointer of entry containing node
 */
#define avl_entry(node, type, member) container_of(node, type, member)

#ifdef __cplusplus
}
#endif

#endif /* __AVLTREE_H__ */
