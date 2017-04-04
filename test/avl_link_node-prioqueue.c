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
