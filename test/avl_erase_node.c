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

#include <assert.h>
#include <stdbool.h>
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
	bool removed_right;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		memset(skiplist, 1, sizeof(skiplist));

		INIT_AVL_ROOT(&root);
		for (j = 0; j < ARRAY_SIZE(values); j++) {
			item = (struct avlitem *)malloc(sizeof(*item));
			assert(item);

			item->i = values[j];
			avlitem_insert_unbalanced(&root, item);
			skiplist[values[j]] = 0;
		}

		random_shuffle_array(delete_items, (uint16_t)ARRAY_SIZE(delete_items));
		for (j = 0; j < ARRAY_SIZE(delete_items); j++) {
			item = avlitem_find(&root, delete_items[j]);

			assert(item);
			assert(item->i == delete_items[j]);

			avl_erase_node(&item->avl, &root, &removed_right);
			skiplist[item->i] = 1;
			free(item);

			check_root_order(&root, skiplist,
					(uint16_t)ARRAY_SIZE(skiplist));
		}
		assert(avl_empty(&root));
	}

	return 0;
}
