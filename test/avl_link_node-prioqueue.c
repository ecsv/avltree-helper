// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../avltree.h"
#include "common.h"
#include "common-prioqueue.h"

static uint16_t values[256];

int main(void)
{
	struct avl_prioqueue queue;
	size_t i, j;
	struct avlitem *item;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

		avl_prioqueue_init(&queue);
		for (j = 0; j < ARRAY_SIZE(values); j++) {
			item = (struct avlitem *)malloc(sizeof(*item));
			assert(item);

			item->i = values[j];
			avl_prioqueue_insert_unbalanced(&queue, item);
		}

		for (j = 0; j < ARRAY_SIZE(values); j++) {
			item = avl_prioqueue_pop_unbalanced(&queue);
			assert(item);
			assert(item->i == j);

			free(item);
		}
		assert(avl_empty(&queue.root));
	}

	return 0;
}
