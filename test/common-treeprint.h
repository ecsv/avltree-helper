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

#ifndef __AVLTREE_COMMON_TREEPRINT_H__
#define __AVLTREE_COMMON_TREEPRINT_H__

#include <stdio.h>

#include "../avltree.h"
#include "common.h"

static __inline__ void printnode(const struct avl_node *node, size_t depth,
				 char prefix)
{
	const struct avlitem *item;
	size_t i;

	if (!node) {
		for (i = 0; i < depth; i++)
			printf("     ");

		printf("%c=\n", prefix);
		return;
	}

	item = avl_entry(node, struct avlitem, avl);

	printnode(node->right, depth+1, '/');

	for (i = 0; i < depth; i++)
		printf("     ");

	switch (avl_balance(node)) {
	case AVL_NEUTRAL:
		printf("%c=", prefix);
		break;
	case AVL_LEFT:
		printf("%c-", prefix);
		break;
	case AVL_RIGHT:
		printf("%c+", prefix);
		break;
	};

	printf("%03u\n", item->i);

	printnode(node->left, depth+1, '\\');

}

static __inline__ void printtree(const struct avl_root *root)
{
	printnode(root->node, 0, '*');
}

static __inline__ void printnode_dot(const struct avl_node *node,
				     size_t *nilcnt)
{
	const struct avlitem *item;
	const struct avlitem *citem;
	const char *color;

	if (!node)
		return;

	item = avl_entry(node, struct avlitem, avl);
	switch (avl_balance(node)) {
	case AVL_NEUTRAL:
		color = "black";
		break;
	case AVL_LEFT:
		color = "red";
		break;
	case AVL_RIGHT:
		color = "green";
		break;
	};
	printf("%03u [color=\"%s\"];\n", item->i, color);

	if (node->left) {
		citem = avl_entry(node->left, struct avlitem, avl);
		switch (avl_balance(node)) {
		case AVL_NEUTRAL:
			color = "black";
			break;
		case AVL_LEFT:
			color = "red";
			break;
		case AVL_RIGHT:
			color = "black";
			break;
		};

		printf("%03u:sw -> %03u [color=\"%s\"];\n", item->i, citem->i,
		       color);
	} else {
		printf("nil%zu [label=\"NIL\", shape=box, color=\"black\"];\n",
		       *nilcnt);
		printf("%03u:sw -> nil%zu [color=\"black\"];\n", item->i,
		       *nilcnt);

		(*nilcnt)++;
	}

	if (node->right) {
		citem = avl_entry(node->right, struct avlitem, avl);
		switch (avl_balance(node)) {
		case AVL_NEUTRAL:
			color = "black";
			break;
		case AVL_LEFT:
			color = "black";
			break;
		case AVL_RIGHT:
			color = "green";
			break;
		};

		printf("%03u:se -> %03u [color=\"%s\"];\n", item->i, citem->i,
			color);
	} else {
		printf("nil%zu [label=\"NIL\", shape=box, color=\"black\"];\n",
		       *nilcnt);
		printf("%03u:se -> nil%zu [color=\"black\"];\n", item->i,
		       *nilcnt);

		(*nilcnt)++;
	}

	printnode_dot(node->left, nilcnt);
	printnode_dot(node->right, nilcnt);
}

static __inline__ void printtree_dot(const struct avl_root *root)
{
	size_t nilcnt = 0;

	printf("digraph G {\n");
	printf("  graph [ordering=\"out\"];\n");

	printnode_dot(root->node, &nilcnt);

	printf("}\n");
}

#endif /* __AVLTREE_COMMON_TREEPRINT_H__ */
