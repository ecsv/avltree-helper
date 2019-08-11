// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
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
	uint16_t maxval = 0;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

		INIT_AVL_ROOT(&root);
		node = avl_first(&root);
		assert(!node);

		for (j = 0; j < ARRAY_SIZE(values); j++) {
			if (j == 0)
				maxval = values[j];

			if (maxval < values[j])
				maxval = values[j];

			items[j].i = values[j];
			avlitem_insert_unbalanced(&root, &items[j]);

			node = avl_last(&root);
			assert(node);

			item = avl_entry(node, struct avlitem, avl);
			assert(item->i == maxval);
		}
	}

	return 0;
}
