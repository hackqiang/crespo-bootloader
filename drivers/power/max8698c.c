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

#include <max8698c.h>

void max8698c_power_init(void)
{
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_BUCK3, 01); /*VCC_MEM = 1.7 */
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x0D); /* LDO4 = 2.9V */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO3_2, 0x66); /* LDO3,4 = 1.1V [PLL, ALIVE, UHOST, UOTG]*/
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x11); /* LDO4 = 3.3V [LCD]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x0F); /* LDO4 = 3.0V [LCD]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x02); /* LDO5 = 1.8V [MMC2/GPJ Port I/O]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x11); /* LDO5 = 3.3V */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x0F); /* LDO5 = 3.0V */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO7, 0x11); /* LDO7 = 3.3V [DAC, HDMI]*/
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_ONOFF1, 0xFE); /* Turn LDO4,LDO5 on */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_ONOFF2, 0xF0); 	/* Turn LDO7 on */
}
