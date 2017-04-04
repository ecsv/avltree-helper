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

#ifndef __AVLTREE_COMMON_TREEVALIDATION_H__
#define __AVLTREE_COMMON_TREEVALIDATION_H__

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../avltree.h"
#include "common.h"

static __inline__ void check_node_order(struct avl_node *node,
					struct avl_node *parent,
					const uint8_t *skiplist, uint16_t *pos,
					uint16_t size)
{
	struct avlitem *item;

	if (!node)
		return;

	assert(avl_parent(node) == parent);

	check_node_order(node->left, node, skiplist, pos, size);

	while (*pos < size && skiplist[*pos])
		(*pos)++;
	assert(*pos < size);

	item = avl_entry(node, struct avlitem, avl);
	assert(item->i == *pos);
	(*pos)++;

	check_node_order(node->right, node, skiplist, pos, size);
}

static __inline__ void check_root_order(const struct avl_root *root,
					const uint8_t *skiplist, uint16_t size)
{
	uint16_t pos = 0;

	check_node_order(root->node, NULL, skiplist, &pos, size);

	while (pos < size && skiplist[pos])
		pos++;

	assert(size == pos);
}

static __inline__ size_t check_depth_node(const struct avl_node *node)
{
	size_t depth_left;
	size_t depth_right;
	size_t depth_min;
	size_t depth_max;

	if (!node)
		return 0;

	depth_left = check_depth_node(node->left);
	depth_right = check_depth_node(node->right);

	if (depth_right > depth_left) {
		depth_min = depth_left;
		depth_max = depth_right;
	} else {
		depth_min = depth_right;
		depth_max = depth_left;
	}

	assert(depth_max - depth_min <= 1);

	if (depth_left == depth_right) {
		assert(avl_balance(node) == AVL_NEUTRAL);
	} else {
		if (depth_left < depth_right)
			assert(avl_balance(node) == AVL_RIGHT);
		else
			assert(avl_balance(node) == AVL_LEFT);
	}

	return depth_max + 1;
}

static __inline__ void check_depth(const struct avl_root *root)
{
	check_depth_node(root->node);
}

#endif /* __AVLTREE_COMMON_TREEVALIDATION_H__ */
