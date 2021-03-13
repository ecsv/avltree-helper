/* SPDX-License-Identifier: MIT */
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#ifndef __AVLTREE_COMMON_PRIOQUEUE_H__
#define __AVLTREE_COMMON_PRIOQUEUE_H__

#include <stdbool.h>
#include <stddef.h>

#include "../avltree.h"
#include "common.h"

struct avl_prioqueue {
	struct avl_root root;
	struct avl_node *min_node;
};

static __inline__ void avl_prioqueue_init(struct avl_prioqueue *queue)
{
	INIT_AVL_ROOT(&queue->root);
	queue->min_node = NULL;
}

static __inline__ void
avl_prioqueue_insert_unbalanced(struct avl_prioqueue *queue,
				struct avlitem *new_entry)
{
	struct avl_node *parent = NULL;
	struct avl_node **cur_nodep = &queue->root.node;
	struct avlitem *cur_entry;
	int isminimal = 1;

	while (*cur_nodep) {
		cur_entry = avl_entry(*cur_nodep, struct avlitem, avl);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0) {
			cur_nodep = &((*cur_nodep)->left);
		} else {
			cur_nodep = &((*cur_nodep)->right);
			isminimal = 0;
		}
	}

	if (isminimal)
		queue->min_node = &new_entry->avl;

	avl_link_node(&new_entry->avl, parent, cur_nodep);
}

static __inline__ struct avlitem *
avl_prioqueue_pop_unbalanced(struct avl_prioqueue *queue)
{
	struct avlitem *item;
	bool removed_right;

	if (!queue->min_node)
		return NULL;

	item = avl_entry(queue->min_node, struct avlitem, avl);
	queue->min_node = avl_next(queue->min_node);

	avl_erase_node(&item->avl, &queue->root, &removed_right);

	return item;
}

static __inline__ void
avl_prioqueue_insert_balanced(struct avl_prioqueue *queue,
			      struct avlitem *new_entry)
{
	struct avl_node *parent = NULL;
	struct avl_node **cur_nodep = &queue->root.node;
	struct avlitem *cur_entry;
	int isminimal = 1;

	while (*cur_nodep) {
		cur_entry = avl_entry(*cur_nodep, struct avlitem, avl);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0) {
			cur_nodep = &((*cur_nodep)->left);
		} else {
			cur_nodep = &((*cur_nodep)->right);
			isminimal = 0;
		}
	}

	if (isminimal)
		queue->min_node = &new_entry->avl;

	avl_insert(&new_entry->avl, parent, cur_nodep, &queue->root);
}

static __inline__ struct avlitem *
avl_prioqueue_pop_balanced(struct avl_prioqueue *queue)
{
	struct avlitem *item;

	if (!queue->min_node)
		return NULL;

	item = avl_entry(queue->min_node, struct avlitem, avl);
	queue->min_node = avl_next(queue->min_node);

	avl_erase(&item->avl, &queue->root);

	return item;
}

#endif /* __AVLTREE_COMMON_PRIOQUEUE_H__ */
