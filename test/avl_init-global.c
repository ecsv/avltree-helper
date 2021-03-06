// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../avltree.h"

static DEFINE_AVLROOT(testtree);

int main(void)
{
	assert(avl_empty(&testtree));

	return 0;
}
