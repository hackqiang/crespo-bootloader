#include <common.h>
#include <s5pc100.h>

#include <movi.h>
#include <asm/io.h>
#include <regs.h>
#include <i2c.h>
#include <max8698c.h>

#ifndef CONFIG_GENERIC_MMC

uint movi_hc = 0;

void movi_set_capacity(void)
{
	if (MOVI_HIGH_CAPACITY & 0x1)
		movi_hc = 1;
}

int movi_set_ofs(uint last)
{
	int changed = 0;

	if (ofsinfo.last != last) {
		ofsinfo.last 	= last - (eFUSE_SIZE / MOVI_BLKSIZE);
		ofsinfo.bl1	= ofsinfo.last - MOVI_BL1_BLKCNT;
		ofsinfo.env	= ofsinfo.bl1 - MOVI_ENV_BLKCNT;
		ofsinfo.bl2	= ofsinfo.bl1 - (MOVI_BL2_BLKCNT + MOVI_ENV_BLKCNT);
		ofsinfo.kernel	= ofsinfo.bl2 - MOVI_ZIMAGE_BLKCNT;
		ofsinfo.rootfs	= ofsinfo.kernel - MOVI_ROOTFS_BLKCNT;
		changed = 1;
	}

	return changed;
}

int movi_init(void)
{
	int ret = 0;

        hsmmc_set_gpio();
        hsmmc_set_base();
        hsmmc_set_clock();
        hsmmc_reset();
        ret = hsmmc_init();
	if (ret) {
		printf("\nCard Initialization failed(%d)\n", ret);
		return -1;
	}

	return 1;
}

void movi_write_env(ulong addr)
{
	movi_write((uint)addr, ofsinfo.env, MOVI_ENV_BLKCNT);
}

void movi_read_env(ulong addr)
{
	movi_read(addr, ofsinfo.env, MOVI_ENV_BLKCNT);
}

typedef u32 (*MMC_ReadBlocks)(u32, u32, u32);
void movi_bl2_copy(void)
{
	MMC_ReadBlocks readmmc = (MMC_ReadBlocks)0x1F8;	
	readmmc(BL2_BASE, MOVI_BL2_POS, MOVI_BL2_BLKCNT);
}
#else	/* !CONFIG_GENERIC_MMC */

#include <mmc.h>

extern raw_area_t raw_area_control;

typedef u32 (*MMC_ReadBlocks)(u32, u32, u32);
void movi_bl2_copy(void)
{
	MMC_ReadBlocks readmmc = (MMC_ReadBlocks)0x1F8;

	readmmc(CFG_PHY_UBOOT_BASE, MOVI_BL2_POS, MOVI_BL2_BLKCNT);

	/* To clear card interrupt in SD/MMC controller */
	writew(0x100, ELFIN_HSMMC_BASE+HM_NORINTSTS);
}

void print_movi_bl2_info (void)
{
	printf("%d, %d, %d\n", MOVI_BL2_POS, MOVI_BL2_BLKCNT, MOVI_ENV_BLKCNT);
}

void movi_write_env(ulong addr)
{
	movi_write(raw_area_control.image[1].start_blk, 
		raw_area_control.image[1].used_blk, addr);
}

void movi_read_env(ulong addr)
{
	movi_read(raw_area_control.image[1].start_blk,
		raw_area_control.image[1].used_blk, addr);
}

void movi_write_bl1(ulong addr)
{
	movi_write(raw_area_control.image[0].start_blk, 
		raw_area_control.image[0].used_blk, addr);
}

#endif /* !CONFIG_GENERIC_MMC *

#define I2CSTAT_BSY 0x20        /* Busy bit */
#define I2C_MODE_MT	0xC0		/* Master Transmit Mode */
#define I2C_MODE_MR	0x80		/* Master Receive Mode */
#define I2C_START_STOP	0x20		/* START / STOP */
#define I2C_TXRX_ENA	0x10		/* I2C Tx/Rx enable */
#define I2C_TIMEOUT 1			/* 1 second */

static inline void i2c_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

static void i2c_init_f (int speed, int slaveadd)
{
	S5PC1XX_I2C *const i2c = S5PC1XX_GetBase_I2C ();
	ulong freq, pres = 1, div = 2;
	int i, status;

	/*Configure GPD[4:3]  to I2C0_SDA , I2C0_SCL */
	GPDCON_REG |= 0x22000;

	/* wait for some time to give previous transfer a chance to finish */
	i = I2C_TIMEOUT * 1000;
	status = i2c->IICSTAT;
	while ((i > 0) && (status & I2CSTAT_BSY)) {
		i2c_delay (1000);
		status = i2c->IICSTAT;
		i--;
	}

	/* set prescaler, divisor according to freq, also set
	 * ACKGEN, IRQ */
	i2c->IICCON = (pres<<6) | (1<<5) | (div&0xf);

	/* init to SLAVE REVEIVE and set slaveaddr */
	i2c->IICSTAT = 0;
	i2c->IICADD = slaveadd;
	/* program Master Transmit (and implicit STOP) */
	i2c->IICSTAT = I2C_MODE_MT | I2C_TXRX_ENA;

}


/* 10ms when 1000000 */
/* 68us when 10000 */
/* 7.8us when 1000 */
void set_max8698c(void)
{
	/* Configure output for PMIC_SET1~3, P3V3_EN, P5V_EN  */
	GPH1CON_REG =  0x11111111;

#if defined(CONFIG_CLK_666_166_66)
//	GPH1DAT_REG = 0x7E;
	GPH1DAT_REG = 0x7C;
#else
//	GPH1DAT_REG = 0x7C;
	GPH1DAT_REG = 0x7E;

#endif
	i2c_init_f(CFG_I2C_SPEED, MAX8698C_I2C_ADDR);

#if defined(CONFIG_CLK_666_166_66)
	/* DVS ARM Voltage tables setting 1.05 1.10 1.15 1.2  */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSARM2_1, 0x76); /* */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSARM4_3, 0x98); /* */

	/* DVS INT Voltage  1.15 1.2*/
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSINT2_1, 0x98); /* */
#else
	/* DVS Voltage tables setting 1.00 1.05 1.2 1.35  for VCCARM */ // FIXME 1.35? 1.30?
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSARM2_1, 0x65); /* */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSARM4_3, 0xc9); /* */

	/* DVS INT Voltage  1.0 1.2*/
	//i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSINT2_1, 0x97); /* */
	/* DVS INT Voltage  1.2 1.3 */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSINT2_1, 0xb9); /* */
	/* DVS INT Voltage  1.2 1.35 */
	//i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSINT2_1, 0xc9); /* */
	/* DVS INT Voltage  1.15 1.25 */
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_DVSINT2_1, 0xa8); /* */
#endif

	i2c_reg_write(MAX8698C_I2C_ADDR, REG_BUCK3, 01); /*VCC_MEM = 1.7 */
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x0D); /* LDO4 = 2.9V */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x11); /* LDO4 = 3.3V [LCD]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO4, 0x0F); /* LDO4 = 3.0V [LCD]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x02); /* LDO5 = 1.8V [MMC2/GPJ Port I/O]*/
//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x11); /* LDO5 = 3.3V */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO5, 0x0F); /* LDO5 = 3.0V */


//	i2c_reg_write(MAX8698C_I2C_ADDR, REG_LDO9, 0x11); /* LDO9 = 3.3V [System I/O] */

	i2c_reg_write(MAX8698C_I2C_ADDR, REG_ONOFF1, 0xFE); /* Turn LDO4,LDO5 on */
	i2c_reg_write(MAX8698C_I2C_ADDR, REG_ONOFF2, 0xF0); 	/* Turn LDO7 on */
}

