// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "avltree.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * avl_set_parent() - Set parent of node
 * @node: pointer to the avl node
 * @parent: pointer to the new parent node
 */
static void avl_set_parent(struct avl_node *node, struct avl_node *parent)
{
#ifndef AVL_PARENT_BALANCE_COMBINATION
	node->parent = parent;
#else
	node->parent_balance = (unsigned long)parent |
			       (node->parent_balance & 3lu);
#endif
}

/**
 * avl_set_balance() - Set balance of node
 * @node: pointer to the avl node
 * @balance: new balance of the node
 */
static void avl_set_balance(struct avl_node *node,
			    enum avl_node_balance balance)
{
#ifndef AVL_PARENT_BALANCE_COMBINATION
	node->balance = balance;
#else
	node->parent_balance = (node->parent_balance & ~3lu) | balance;
#endif
}

/**
 * avl_change_child() - Fix child entry of parent node
 * @old_node: avl node to replace
 * @new_node: avl node replacing @old_node
 * @parent: parent of @old_node
 * @root: pointer to avl root
 *
 * Detects if @old_node is left/right child of @parent or if it gets inserted
 * as as new root. These entries are then updated to point to @new_node.
 *
 * @old_node and @root must not be NULL.
 */
static void avl_change_child(struct avl_node *old_node,
			     struct avl_node *new_node,
			     struct avl_node *parent, struct avl_root *root)
{
	if (parent) {
		if (parent->left == old_node)
			parent->left = new_node;
		else
			parent->right = new_node;
	} else {
		root->node = new_node;
	}
}

/**
 * avl_rotate_switch_parents() - set parent for switched nodes after rotate
 * @node_top: avl node which became the new top node
 * @node_child: avl node which became the new child node
 * @node_child2: ex'child of @node_top which now is now 2. child of @node_child
 * @root: pointer to avl root
 * @balance_top: new balance for @node_top
 * @balance_child: new balance for @node_child
 *
 * @node_top must have been a valid child of @node_child. The child changes
 * for the rotation in @node_child and @node_top must already be finished.
 * The switch of parents for @node_top, @node_child and @node_child2
 * (when it exists) is peformend. The change of the child entry of the new
 * parent of @node_top is done afterwards.
 */
static void avl_rotate_switch_parents(struct avl_node *node_top,
				      struct avl_node *node_child,
				      struct avl_node *node_child2,
				      struct avl_root *root,
				      enum avl_node_balance balance_top,
				      enum avl_node_balance balance_child)
{
	/* switch parents and set new balance */
	avl_set_parent_balance(node_top, avl_parent(node_child), balance_top);
	avl_set_parent_balance(node_child, node_top, balance_child);

	/* switch parent of child2 from child to top */
	if (node_child2)
		avl_set_parent(node_child2, node_child);

	/* parent of node_top must get its child pointer get fixed */
	avl_change_child(node_child, node_top, avl_parent(node_top), root);
}

/**
 * avl_is_right_child() - Check if the node is a right child
 * @node: avl node to check
 *
 * Return: true when @node is a right child, false when it is a left child or
 *  when it has no parent
 */
static bool avl_is_right_child(struct avl_node *node)
{
	struct avl_node *parent = avl_parent(node);

	if (!parent)
		return false;

	if (parent->right == node)
		return true;

	return false;
}

/**
 * avl_rotate_rightleft() - Balance subtree using right left double rotate
 * @node: right node of @parent which moves balance to the right
 * @parent: root of the subtree to rotate to the left
 * @root: pointer to avl root
 *
 * The subtree under @node is rotated to the right and the subtree under @parent
 * is rotated to the left to avoid that the balance of @parent becomes double
 * right.
 *
 * WARNING this cannot be used when @node is not already left leaning. The
 * single rotate (avl_rotate_left) must be used instead in this situation.
 *
 * Return: new "root" of the rotated subtree
 */
static struct avl_node *avl_rotate_rightleft(struct avl_node *node,
					     struct avl_node *parent,
					     struct avl_root *root)
{
	enum avl_node_balance balance_parent, balance_node;
	struct avl_node *tmp;

	/* rotate right */
	tmp = node->left;
	node->left = tmp->right;
	tmp->right = node;

	switch (avl_balance(tmp)) {
	default:
	case AVL_RIGHT:
		balance_parent = AVL_LEFT;
		balance_node = AVL_NEUTRAL;
		break;
	case AVL_NEUTRAL:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_NEUTRAL;
		break;
	case AVL_LEFT:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_RIGHT;
		break;
	}

	avl_rotate_switch_parents(tmp, node, node->left, root, AVL_NEUTRAL,
				  balance_node);

	/* rotate left */
	tmp = parent->right;
	parent->right = tmp->left;
	tmp->left = parent;

	avl_rotate_switch_parents(tmp, parent, parent->right, root, AVL_NEUTRAL,
				  balance_parent);

	return tmp;
}

/**
 * avl_rotate_leftright() - Balance subtree using left right double rotate
 * @node: left node of @parent which moves balance to the left
 * @parent: root of the subtree to rotate to the right
 * @root: pointer to avl root
 *
 * The subtree under @node is rotated to the left and the subtree under @parent
 * is rotated to the right to avoid that the balance of @parent becomes double
 * left.
 *
 * WARNING this cannot be used when @node is not already right leaning. The
 * single rotate (avl_rotate_right) must be used instead in this situation.
 *
 * Return: new "root" of the rotated subtree
 */
static struct avl_node *avl_rotate_leftright(struct avl_node *node,
					     struct avl_node *parent,
					     struct avl_root *root)
{
	enum avl_node_balance balance_parent, balance_node;
	struct avl_node *tmp;

	/* rotate left */
	tmp = node->right;
	node->right = tmp->left;
	tmp->left = node;

	switch (avl_balance(tmp)) {
	default:
	case AVL_RIGHT:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_LEFT;
		break;
	case AVL_NEUTRAL:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_NEUTRAL;
		break;
	case AVL_LEFT:
		balance_parent = AVL_RIGHT;
		balance_node = AVL_NEUTRAL;
		break;
	}

	avl_rotate_switch_parents(tmp, node, node->right, root, AVL_NEUTRAL,
				  balance_node);

	/* rotate right */
	tmp = parent->left;
	parent->left = tmp->right;
	tmp->right = parent;

	avl_rotate_switch_parents(tmp, parent, parent->left, root, AVL_NEUTRAL,
				  balance_parent);

	return tmp;
}

/**
 * avl_rotate_left() - Rotate subtree at @parent to the left
 * @node: right node of @parent which moves balance to the right
 * @parent: root of the subtree to rotate to the left
 * @root: pointer to avl root
 *
 * The subtree under @parent is rotated to the right to avoid that the balance
 * of @parent becomes double right.
 *
 * WARNING this cannot be used when @node is already left leaning because this
 * would create again a double right leaning @parent balance. The double
 * rotate (avl_rotate_rightleft) must be used instead in this situation.
 *
 * Return: new "root" of the rotated subtree
 */
static struct avl_node *avl_rotate_left(struct avl_node *node,
					struct avl_node *parent,
					struct avl_root *root)
{
	enum avl_node_balance balance_parent, balance_node;
	struct avl_node *tmp;

	switch(avl_balance(node)) {
	case AVL_NEUTRAL:
		balance_parent = AVL_RIGHT;
		balance_node = AVL_LEFT;
		break;
	default:
	/* AVL_LEFT is not allowed */
	case AVL_RIGHT:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_NEUTRAL;
		break;
	}

	/* rotate left */
	tmp = parent->right;
	parent->right = tmp->left;
	tmp->left = parent;

	avl_rotate_switch_parents(tmp, parent, parent->right,
				  root, balance_node, balance_parent);

	return tmp;
}

/**
 * avl_rotate_right() - Rotate subtree at @parent to the right
 * @node: left node of @parent which moves balance to the left
 * @parent: root of the subtree to rotate to the right
 * @root: pointer to avl root
 *
 * The subtree under @parent is rotated to the left to avoid that the balance of
 * @parent becomes double left.
 *
 * WARNING this cannot be used when @node is already right leaning because this
 * would create again a double left leaning @parent balance. The double
 * rotate (avl_rotate_leftright) must be used instead in this situation.
 *
 * Return: new "root" of the rotated subtree
 */
static struct avl_node *avl_rotate_right(struct avl_node *node,
					 struct avl_node *parent,
					 struct avl_root *root)
{
	enum avl_node_balance balance_parent, balance_node;
	struct avl_node *tmp;

	switch(avl_balance(node)) {
	case AVL_NEUTRAL:
		balance_parent = AVL_LEFT;
		balance_node = AVL_RIGHT;
		break;
	default:
	/* AVL_RIGHT is not allowed */
	case AVL_LEFT:
		balance_parent = AVL_NEUTRAL;
		balance_node = AVL_NEUTRAL;
		break;
	}

	/* rotate right */
	tmp = parent->left;
	parent->left = tmp->right;
	tmp->right = parent;

	avl_rotate_switch_parents(tmp, parent, parent->left, root, balance_node,
				  balance_parent);

	return tmp;
}

/**
 * avl_insert_balance() - Go tree upwards and rebalance it after insert
 * @node: pointer to the new node
 * @root: pointer to avl root
 *
 * The tree is traversed from bottom to the top starting at @node. The relative
 * height of each node will be adjusted on the path upwards. Rotations are used
 * to fix nodes which would become double left or double right leaning.
 *
 * When the tree was an AVL tree before the link of the new node then the
 * resulting tree will again be an AVL tree
 */
void avl_insert_balance(struct avl_node *node, struct avl_root *root)
{
	struct avl_node *parent;

	/* go tree upwards and fix the nodes on the way */
	while ((parent = avl_parent(node))) {
		if (avl_is_right_child(node)) {
			switch (avl_balance(parent)) {
			default:
			case AVL_RIGHT:
				/* compensate double right balance by rotation
				 * and stop afterwards
				 */
				switch (avl_balance(node)) {
				default:
				case AVL_RIGHT:
				case AVL_NEUTRAL:
					avl_rotate_left(node, parent, root);
					break;
				case AVL_LEFT:
					avl_rotate_rightleft(node, parent,
							     root);
					break;
				}

				parent = NULL;
				break;
			case AVL_NEUTRAL:
				/* mark balance as right and continue upwards */
				avl_set_balance(parent, AVL_RIGHT);
				break;
			case AVL_LEFT:
				/* new right child + left leaning == balanced
				 * nothing to propagate upwards after that
				 */
				avl_set_balance(parent, AVL_NEUTRAL);
				parent = NULL;
				break;
			}
		} else {
			switch (avl_balance(parent)) {
			default:
			case AVL_RIGHT:
				/* new left child + right leaning == balanced
				 * nothing to propagate upwards after that
				 */
				avl_set_balance(parent, AVL_NEUTRAL);
				parent = NULL;
				break;
			case AVL_NEUTRAL:
				/* mark balance as left and continue upwards */
				avl_set_balance(parent, AVL_LEFT);
				break;
			case AVL_LEFT:
				/* compensate double left balance by rotation
				 * and stop afterwards
				 */
				switch (avl_balance(node)) {
				default:
				case AVL_LEFT:
				case AVL_NEUTRAL:
					avl_rotate_right(node, parent, root);
					break;
				case AVL_RIGHT:
					avl_rotate_leftright(node, parent,
							     root);
					break;
				}

				parent = NULL;
				break;
			}
		}

		if (!parent)
			break;

		node = parent;
	}
}

/**
 * avl_erase_node() - Remove avl node from tree
 * @node: pointer to the node
 * @root: pointer to avl root
 * @removed_right: returns whether returned node now has a decreased depth under
 *  the right child
 *
 * The node is only removed from the tree. Neither the memory of the removed
 * node nor the memory of the entry containing the node is free'd. The node
 * has to be handled like an uninitialized node. Accessing the parent or
 * right/left pointer of the node is not safe.
 *
 * WARNING The removed node may cause the tree to be become unbalanced or
 * violate any rules of the alv tree. A call to avl_erase_balance after
 * avl_erase_node is therefore always required to rebalance the tree correctly.
 * avl_erase can be used as helper to run both steps at the same time.
 *
 * Return: node whose balance value has to be modified and maybe has to be
 *  rebalanced, NULL if no rebalance is necessary
 */
struct avl_node *avl_erase_node(struct avl_node *node, struct avl_root *root,
				bool *removed_right)
{
	struct avl_node *smallest;
	struct avl_node *smallest_parent;
	struct avl_node *decreased_node;

	if (!node->left && !node->right) {
		/* no child
		 * just delete the current child
		 */
		*removed_right = avl_is_right_child(node);
		avl_change_child(node, NULL, avl_parent(node), root);

		return avl_parent(node);
	} else if (node->left && !node->right) {
		/* one child, left
		 * use left child as replacement for the deleted node
		 */
		*removed_right = avl_is_right_child(node);
		avl_set_parent(node->left, avl_parent(node));
		avl_change_child(node, node->left, avl_parent(node), root);

		return avl_parent(node);
	} else if (!node->left) {
		/* one child, right
		 * use right child as replacement for the deleted node
		 */
		*removed_right = avl_is_right_child(node);
		avl_set_parent(node->right, avl_parent(node));
		avl_change_child(node, node->right, avl_parent(node), root);

		return avl_parent(node);
	}

	/* two children, take smallest of right (grand)children */
	smallest = node->right;
	while (smallest->left)
		smallest = smallest->left;

	smallest_parent = avl_parent(smallest);
	if (smallest == node->right) {
		decreased_node = node->right;
		*removed_right = true;
	} else {
		decreased_node = smallest_parent;
		*removed_right = avl_is_right_child(smallest);
	}

	/* move right child of smallest one up */
	if (smallest->right)
		avl_set_parent(smallest->right, smallest_parent);
	avl_change_child(smallest, smallest->right, smallest_parent, root);

	/* exchange node with smallest */
	avl_set_parent_balance(smallest, avl_parent(node), avl_balance(node));

	smallest->left = node->left;
	avl_set_parent(smallest->left, smallest);

	smallest->right = node->right;
	if (smallest->right)
		avl_set_parent(smallest->right, smallest);

	avl_change_child(node, smallest, avl_parent(node), root);

	return decreased_node;
}

/**
 * avl_erase_balance() - Go tree upwards and rebalance it after erase_node
 * @parent: node whose child was removed
 * @removed_right: returns whether @parent now has a decreased depth under
 *  the right child
 * @root: pointer to avl root
 *
 * The tree is traversed from bottom to the top starting at @parent. The
 * relative height of each node will be adjusted on the path upwards. Rotations
 * are used to fix nodes which would become double left or double right leaning.
 *
 * When the tree was an AVL-tree before the erase of the node then the resulting
 * tree will again be an AVL-tree
 */
void avl_erase_balance(struct avl_node *parent, bool removed_right,
		       struct avl_root *root)
{
	struct avl_node *node;

	/* go tree upwards and fix the nodes on the way */
	while (parent) {
		if (!removed_right) {
			switch (avl_balance(parent)) {
			case AVL_RIGHT:
			default:
				/* compensate double right balance using
				 * rotations
				 */
				node = parent->right;
				switch (avl_balance(node)) {
				default:
				case AVL_RIGHT:
					parent = avl_rotate_left(node, parent,
								 root);
					break;
				case AVL_NEUTRAL:
					avl_rotate_left(node, parent, root);
					parent = NULL;
					break;
				case AVL_LEFT:
					parent = avl_rotate_rightleft(node,
								      parent,
								      root);
					break;
				}
				break;
			case AVL_NEUTRAL:
				/* there must have been a right child when
				 * the balance was neutral and the left child
				 * got removed. It is therefore enough to
				 * set balance to right and stop because the
				 * height of subtree didn't change
				 */
				avl_set_balance(parent, AVL_RIGHT);
				parent = NULL;
				break;
			case AVL_LEFT:
				/* mark balance as neutral and continue */
				avl_set_balance(parent, AVL_NEUTRAL);
				break;
			}
		} else {
			switch (avl_balance(parent)) {
			default:
			case AVL_RIGHT:
				/* mark balance as neutral and continue */
				avl_set_balance(parent, AVL_NEUTRAL);
				break;
			case AVL_NEUTRAL:
				/* there must have been a left child when
				 * the balance was neutral and the right child
				 * got removed. It is therefore enough to
				 * set balance to left and stop because the
				 * height of subtree didn't change
				 */
				avl_set_balance(parent, AVL_LEFT);
				parent = NULL;
				break;
			case AVL_LEFT:
				/* compensate double left balance using
				 * rotations
				 */
				node = parent->left;
				switch (avl_balance(node)) {
				case AVL_LEFT:
					parent = avl_rotate_right(node, parent,
								  root);
					break;
				case AVL_NEUTRAL:
					avl_rotate_right(node, parent, root);
					parent = NULL;
					break;
				default:
				case AVL_RIGHT:
					parent = avl_rotate_leftright(node,
								      parent,
								      root);
					break;
				}
				break;
			}
		}

		if (!parent)
			break;

		removed_right = avl_is_right_child(parent);
		parent = avl_parent(parent);
	}
}

/**
 * avl_first() - Find leftmost avl node in tree
 * @root: pointer to avl root
 *
 * Return: pointer to leftmost node. NULL when @root is empty.
 */
struct avl_node *avl_first(const struct avl_root *root)
{
	struct avl_node *node = root->node;

	if (!node)
		return node;

	/* descend down via smaller/preceding child */
	while (node->left)
		node = node->left;

	return node;
}

/**
 * avl_last() - Find rightmost avl node in tree
 * @root: pointer to avl root
 *
 * Return: pointer to rightmost node. NULL when @root is empty.
 */
struct avl_node *avl_last(const struct avl_root *root)
{
	struct avl_node *node = root->node;

	if (!node)
		return node;

	/* descend down via larger/succeeding child */
	while (node->right)
		node = node->right;

	return node;
}

/**
 * avl_next() - Find successor node in tree
 * @node: starting avl node for search
 *
 * Return: pointer to successor node. NULL when no successor of @node exist.
 */
struct avl_node *avl_next(struct avl_node *node)
{
	struct avl_node *parent;

	/* there is a right child - next node must be the leftmost under it */
	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;

		return node;
	}

	/* otherwise check if we have a parent (and thus maybe siblings) */
	parent = avl_parent(node);
	if (!parent)
		return parent;

	/* go up the tree until the path connecting both is the left child
	 * pointer and therefore the parent is the next node
	 */
	while (parent && parent->right == node) {
		node = parent;
		parent = avl_parent(node);
	}

	return parent;
}

/**
 * avl_prev() - Find predecessor node in tree
 * @node: starting avl node for search
 *
 * Return: pointer to predecessor node. NULL when no predecessor of @node exist.
 */
struct avl_node *avl_prev(struct avl_node *node)
{
	struct avl_node *parent;

	/* there is a left child - prev node must be the rightmost under it */
	if (node->left) {
		node = node->left;
		while (node->right)
			node = node->right;

		return node;
	}

	/* otherwise check if we have a parent (and thus maybe siblings) */
	parent = avl_parent(node);
	if (!parent)
		return parent;

	/* go up the tree until the path connecting both is the right child
	 * pointer and therefore the parent is the prev node
	 */
	while (parent && parent->left == node) {
		node = parent;
		parent = avl_parent(node);
	}

	return parent;
}
