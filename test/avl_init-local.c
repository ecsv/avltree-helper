// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../avltree.h"

int main(void)
{
	DEFINE_AVLROOT(testtree);
	assert(avl_empty(&testtree));

	return 0;
}
