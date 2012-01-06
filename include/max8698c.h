/*
 * Copyright (c) 2010 Hardkernel, Inc.
 * Hakjoo Kim <ruppi.kim@hardkernel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */ 

#ifndef _MAX8698C_H
#define _MAX8698C_H

#include <common.h>
#include <i2c.h>

#define REG_ONOFF1 0x00
#define REG_ONOFF2 0x01
#define REG_ADISCHG_EN1 0x02
#define REG_ADISCHG_EN2 0x03
#define REG_DVSARM2_1 0x04
#define REG_DVSARM4_3 0x05
#define REG_DVSINT2_1 0x06
#define REG_BUCK3 0x07 /* VCC_MEM : Output 1.8V */
#define REG_LDO3_2 0x08
#define REG_LDO4 0x09
#define REG_LDO5 0x0A
#define REG_LDO6 0x0B
#define REG_LDO7 0x0C
#define REG_LDO8_BKCHR 0x0D /* LDO8 and Backup Battery Charger(VCC_COIN) output voltages */
#define REG_LDO9 0x0E
#define REG_LBCNFG 0x0F
/* For initializing power device */
void max8698c_power_init(void);

#endif /* _MAX8698C_H */
