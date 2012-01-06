/*
 * (C) Copyright 2009
 * Kim Hak Joo, Hardkernel, ruppi.kim@hardkernel.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */


#include <common.h>
#include <config.h>
#include <command.h>
#include <i2c.h>
#include <regs.h>

#define armclk_NAME_MAX			12
#define KHZ          ((uint)1000)
#define MHZ          ((uint)(1000 * KHZ))
static const u32 s5p_apll_table[] [5]  = { 
	/* Target Fout  P  M S        VCOOUT  FOUT */
	{1666, 3, 417, 0, 1688, 1668},
	{1333, 4, 444, 0, 1335 , 1335},	
};

/* ARMCLK, HCLKD1, APllDIV,ArmRatio,HclkRatio,PclkRatio,hclkD1Ratio, pclkD1Ratio*/
static const u32 s5p_dout_clk_tab[][8] = {
//DVFS_Level_Table_Type s5p_dout_clk_tab[][8] = {
	{833* MHZ, 133* MHZ, 0, 0, 3, 1, 1, 1},//ARM:666.0, HCLKD0:166.5, PCLKD0:83.25, HCLKD1:133, PCLKD1:66.5
	{666* MHZ, 133* MHZ, 0, 0, 3, 1, 1, 1},//ARM:666.0, HCLKD0:166.5, PCLKD0:83.25, HCLKD1:133, PCLKD1:66.5
	{333* MHZ, 133* MHZ, 0, 1, 1, 1, 1, 1},//ARM:333.0, HCLKD0:166.5, PCLKD0:83.25, HCLKD1:133, PCLKD1:66.5
	{222* MHZ, 133* MHZ, 0, 2, 1, 1, 1, 1},//ARM:222.0, HCLKD0:111.0, PCLKD0:55.50, HCLKD1:133, PCLKD1:66.5
	{133* MHZ, 133* MHZ, 0, 4, 0, 1, 1, 1},//ARM:133.2, HCLKD0:133.2, PCLKD0:66.60, HCLKD1:133, PCLKD1:66.5
	{133* MHZ,  66* MHZ, 0, 4, 0, 1, 3, 0},//ARM:133.2, HCLKD0:133.2, PCLKD0:66.60, HCLKD1:66.5,PCLKD1:66.5
	{ 66* MHZ,  66* MHZ, 1, 4, 0, 1, 3, 0},//ARM:66.60, HCLKD0:66.60, PCLKD0:66.60, HCLKD1:66.5,PCLKD1:66.5 //KO bsp241 : 140030 -> 140130
};

#if 0
typedef enum {
	ARM_833,
	ARM_833,
	ARM_833,
	ARM_833,
	ARM_833,
	ARM_833,
	ARM_833,	/* For now, kilauea can't support */
	ARM_833,
	ARM_833,
	ARM_TOTAL
} armclk_freq_t;

static const char
armclk_name[][armclk_NAME_MAX] = {
	"833",
	"666",
	"533",
	"417",
	"333",
	"278",
	"266",
	"222",
	"177",
	"139",
	"133",
	"66",
	"60",
	""
};
#endif 

int do_armclk_alter(int argc, char *argv[])
{
	int armclk, ret=0;
	u32 mdiv, pdiv, sdiv;
		
	armclk = simple_strtoul(argv[1], NULL, 10);
	printf("armclk : %dMHz\n", armclk);

	if(armclk == 833) {
		CLK_DIV0_REG &= ~CLK_DIV0_MASK;
		CLK_DIV0_REG =  ((1<<APLL_RATIO)|(0<<ARM_RATIO)|(4<<D0_BUS_RATIO)|(1<<PCLKD0_RATIO)|(1<<SECSS_RATIO));
	//	APLL_CON_REG = set_pll( s5p_apll_table[0] [2],  s5p_apll_table[0] [1],  s5p_apll_table[0] [3]);
	} else  {
	//	CLK_DIV0_REG &= ~CLK_DIV0_MASK;
	//	CLK_DIV0_REG =  ((1<<APLL_RATIO)|(0<<ARM_RATIO)|(3<<D0_BUS_RATIO)|(1<<PCLKD0_RATIO)|(1<<SECSS_RATIO));
		APLL_CON_REG = set_pll( s5p_apll_table[1] [2],  s5p_apll_table[1] [1],  s5p_apll_table[1] [3]);
	}
	
	return ret;
}

int do_hclkd0_alter(int argc, char *argv[])
{
	int hclkd0, ret=0;

       hclkd0 = simple_strtoul(argv[1], NULL, 10);
	printf("hclkd0 : %d\n", hclkd0);

	return ret;
}

int
do_clk_alter (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int armclk, hclkd0, ret=0;
	
	switch (argc) {
	case 3:
		if (strncmp(argv[1], "armclk", 3) == 0) {
			printf ("armclk\n");	
			ret = do_armclk_alter(--argc, ++argv);
		}	
		else if (strncmp(argv[1], "hclkd0", 3) == 0) {
			printf ("hclkd0\n");
			ret = do_hclkd0_alter(--argc, ++argv);
		}			
		else { /* both armclk and hclkd0 */
			ret = do_armclk_alter(--argc, ++argv);
			ret = do_hclkd0_alter(--argc, ++argv);
		}	
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	return ret;		
}


U_BOOT_CMD(
	clkalter,	CFG_MAXARGS,	1,	do_clk_alter,		\
	"clkalter [arm freq] [hclkd0 freq] - change armclk or hclkd0 frequence\n",				\
	"833 166 -change armclk and hclkd0 frequence\n" \
	"\t armclk 833,  666, 533, 417, 333, 278, 266, 222, 177, 139, 133, 66, 60 [MHz] -change armclk frequence\n" \
	 "\t hclkd0 200, 166, 133 [MHz]  -change hclkd0 frequence\n" \
);

