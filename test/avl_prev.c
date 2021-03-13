// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../avltree.h"
#include "common.h"
#include "common-treeops.h"

static uint16_t values[256];

static struct avlitem items[ARRAY_SIZE(values)];

int main(void)
{
	struct avl_root root;
	struct avl_node *node;
	struct avlitem *item;
	size_t i, j;

	INIT_AVL_ROOT(&root);
	items[0].i = 0;
	avlitem_insert_unbalanced(&root, &items[0]);
	assert(avl_prev(&items[0].avl) == NULL);

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

		INIT_AVL_ROOT(&root);
		node = avl_last(&root);
		assert(!node);

		for (j = 0; j < ARRAY_SIZE(values); j++) {
			items[j].i = values[j];
			avlitem_insert_unbalanced(&root, &items[j]);
		}

		for (node = avl_last(&root), j = 0;
		     node;
		     j++, node = avl_prev(node)) {
			item = avl_entry(node, struct avlitem, avl);
			assert(item->i == ARRAY_SIZE(values) - j - 1);
		}
		assert(j == ARRAY_SIZE(values));
		assert(!node);
	}

	return 0;
}
