// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../avltree.h"
#include "common.h"
#include "common-treeops.h"
#include "common-treevalidation.h"

static uint16_t values[256];
static uint16_t delete_items[ARRAY_SIZE(values)];
static uint8_t skiplist[ARRAY_SIZE(values)];

int main(void)
{
	struct avl_root root;
	size_t i, j;
	struct avlitem *item;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		memset(skiplist, 1, sizeof(skiplist));

		INIT_AVL_ROOT(&root);
		for (j = 0; j < ARRAY_SIZE(values); j++) {
			item = (struct avlitem *)malloc(sizeof(*item));
			assert(item);

			item->i = values[j];
			avlitem_insert_balanced(&root, item);
			skiplist[values[j]] = 0;
		}

		random_shuffle_array(delete_items, (uint16_t)ARRAY_SIZE(delete_items));
		for (j = 0; j < ARRAY_SIZE(delete_items); j++) {
			item = avlitem_find(&root, delete_items[j]);

			assert(item);
			assert(item->i == delete_items[j]);

			avl_erase(&item->avl, &root);
			skiplist[item->i] = 1;
			free(item);

			check_root_order(&root, skiplist,
					(uint16_t)ARRAY_SIZE(skiplist));
			check_depth(&root);
		}
		assert(avl_empty(&root));
	}

	return 0;
}
