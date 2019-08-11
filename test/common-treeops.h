/* SPDX-License-Identifier: MIT */
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __AVLTREE_COMMON_TREEOPS_H__
#define __AVLTREE_COMMON_TREEOPS_H__

#include <stddef.h>
#include <stdint.h>

#include "../avltree.h"
#include "common.h"

static __inline__ void avlitem_insert_unbalanced(struct avl_root *root,
						 struct avlitem *new_entry)
{
	struct avl_node *parent = NULL;
	struct avl_node **cur_nodep = &root->node;
	struct avlitem *cur_entry;

	while (*cur_nodep) {
		cur_entry = avl_entry(*cur_nodep, struct avlitem, avl);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0)
			cur_nodep = &((*cur_nodep)->left);
		else
			cur_nodep = &((*cur_nodep)->right);
	}

	avl_link_node(&new_entry->avl, parent, cur_nodep);
}

static __inline__ void avlitem_insert_balanced(struct avl_root *root,
					       struct avlitem *new_entry)
{
	avlitem_insert_unbalanced(root, new_entry);
	avl_insert_balance(&new_entry->avl, root);
}

static __inline__ struct avlitem *avlitem_find(struct avl_root *root,
					       uint16_t x)
{
	struct avl_node **cur_nodep = &root->node;
	struct avlitem *cur_entry;
	int res;

	while (*cur_nodep) {
		cur_entry = avl_entry(*cur_nodep, struct avlitem, avl);

		res = cmpint(&x, &cur_entry->i);
		if (res == 0)
			return cur_entry;

		if (res < 0)
			cur_nodep = &((*cur_nodep)->left);
		else
			cur_nodep = &((*cur_nodep)->right);
	}

	return NULL;
}

#endif /* __AVLTREE_COMMON_TREEOPS_H__ */
