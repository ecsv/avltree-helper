// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../avltree.h"

int main(void)
{
	struct avl_root testtree;

	INIT_AVL_ROOT(&testtree);
	assert(avl_empty(&testtree));

	return 0;
}
