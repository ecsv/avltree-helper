/* Minimal AVL-tree helper functions test
 *
 * Copyright (c) 2012-2017, Sven Eckelmann <sven@narfation.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
