// SPDX-License-Identifier: MIT
/* Minimal AVL-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../avltree.h"
#include "common.h"

int main(void)
{
	struct avlitem item;

	assert(&item == avl_entry(&item.avl, struct avlitem, avl));

	return 0;
}
