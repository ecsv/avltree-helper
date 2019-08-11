// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../avltree.h"
#include "common.h"
#include "common-treeops.h"
#include "common-treevalidation.h"

static uint16_t values[256];

static struct avlitem items[ARRAY_SIZE(values)];
static uint8_t skiplist[ARRAY_SIZE(values)];

int main(void)
{
	struct avl_root root;
	size_t i, j;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		memset(skiplist, 1, sizeof(skiplist));

		INIT_AVL_ROOT(&root);
		for (j = 0; j < ARRAY_SIZE(values); j++) {
			items[j].i = values[j];
			avlitem_insert_unbalanced(&root, &items[j]);
			skiplist[values[j]] = 0;

			check_root_order(&root, skiplist,
					 (uint16_t)ARRAY_SIZE(skiplist));
		}
	}

	return 0;
}
