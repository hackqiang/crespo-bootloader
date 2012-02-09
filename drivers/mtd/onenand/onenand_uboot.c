/*
 *  drivers/mtd/onenand/onenand_uboot.c
 *
 *  Copyright (C) 2005-2008 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * OneNAND initialization at U-Boot
 */

#include <common.h>

#ifdef CONFIG_CMD_ONENAND

#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>

#include <configs/herring.h>

struct mtd_info onenand_mtd;
struct onenand_chip onenand_chip;
static const __attribute__((unused)) char dev_name[] = "onenand0";

void onenand_init(void)
{
	memset(&onenand_mtd, 0, sizeof(struct mtd_info));
	memset(&onenand_chip, 0, sizeof(struct onenand_chip));

	onenand_mtd.priv = &onenand_chip;

#ifdef CONFIG_USE_ONENAND_BOARD_INIT
	/*
	 * It's used for some board init required
	 */
	onenand_board_init(&onenand_mtd);
#else
	onenand_chip.base = (void *) CFG_ONENAND_BASE;
#endif

	puts("\r");

	onenand_scan(&onenand_mtd, 1);

	puts("OneNAND: ");
	print_size(onenand_mtd.size, "\n");

	onenand_mtd.name = dev_name;
}

#endif	/* CONFIG_CMD_ONENAND */
