#include <common.h>
#include <s5pc100.h>

#include <movi.h>
#include <asm/io.h>
#include <regs.h>
#include "c100_lcdcon.h"
//#include "logo1.c"

//#include <i2c.h>
//#include <max8698c.h>

static inline int udelay_f(unsigned long usec)
{
	unsigned long loops = usec *300;
	 __asm__ volatile ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

#define mdelay(x)	udelay_f(1000*x)
static void s3cfb_init_ldi(void);

#define S3C_FB_HFP			4	/* front porch */
#define S3C_FB_HSW			3	/* hsync width */
#define S3C_FB_HBP		 	5	/* back porch */
                        	
#define S3C_FB_VFP			4	/* front porch */
#define S3C_FB_VSW			3	/* vsync width */
#define S3C_FB_VBP			3	/* back porch */
                        	
#define S3C_FB_HRES			320	/* horizon pixel  x resolition */
#define S3C_FB_VRES			480	/* line cnt       y resolution */

#define S3C_FB_HRES_VIRTUAL	320	/* horizon pixel  x resolition */
#define S3C_FB_VRES_VIRTUAL	960	/* line cnt       y resolution */

#define S3C_FB_HRES_OSD		320	/* horizon pixel  x resolition */
#define S3C_FB_VRES_OSD		480	/* line cnt       y resolution */

#define S3C_FB_VFRAME_FREQ     	60	/* frame rate freq */

#define S3C_FB_PIXEL_CLOCK	(S3C_FB_VFRAME_FREQ * (S3C_FB_HFP + S3C_FB_HSW + S3C_FB_HBP + S3C_FB_HRES) * (S3C_FB_VFP + S3C_FB_VSW + S3C_FB_VBP + S3C_FB_VRES))

#define VIDCON0			__REG(0xEE000000)
#define VIDCON1			__REG(0xEE000004)
#define VIDCON2			__REG(0xEE000008)
#define PRTCON			__REG(0xEE00000C)
#define VIDTCON0		__REG(0xEE000010)
#define VIDTCON1		__REG(0xEE000014)
#define VIDTCON2		__REG(0xEE000018)
#define WINCON0			__REG(0xEE000020)
#define WINCON1			__REG(0xEE000024)
#define WINCON2			__REG(0xEE000028)
#define WINCON3 		__REG(0xEE00002C)
#define WINCON4			__REG(0xEE000030)
#define VIDOSD0A		__REG(0xEE000040)
#define VIDOSD0B		__REG(0xEE000044)
#define VIDOSD0C		__REG(0xEE000048)
#define VIDOSD1A		__REG(0xEE000050)
#define VIDOSD1B		__REG(0xEE000054)
#define VIDOSD1C		__REG(0xEE000058)
#define VIDOSD1D		__REG(0xEE00005C)
#define VIDOSD2A		__REG(0xEE000060)
#define VIDOSD2B		__REG(0xEE000064)
#define VIDOSD2C		__REG(0xEE000068)
#define VIDOSD2D		__REG(0xEE00006C)
#define VIDOSD3A		__REG(0xEE000070)
#define VIDOSD3B		__REG(0xEE000074)
#define VIDOSD3C		__REG(0xEE000078)
#define VIDOSD4A		__REG(0xEE000080)
#define VIDOSD4B		__REG(0xEE000084)
#define VIDOSD4C		__REG(0xEE000088)
#define VIDW00ADD0B0	__REG(0xEE0000A0)
#define VIDW00ADD0B1	__REG(0xEE0000A4)
#define VIDW01ADD0B0	__REG(0xEE0000A8)
#define VIDW01ADD0B1	__REG(0xEE0000AC)
#define VIDW02ADD0		__REG(0xEE0000B0)
#define VIDW03ADD0		__REG(0xEE0000B8)
#define VIDW04ADD0		__REG(0xEE0000C0)
#define VIDW00ADD1B0	__REG(0xEE0000D0)
#define VIDW00ADD1B1	__REG(0xEE0000D4)
#define VIDW01ADD1B0	__REG(0xEE0000D8)
#define VIDW01ADD1B1	__REG(0xEE0000DC)
#define VIDW02ADD1		__REG(0xEE0000E0)
#define VIDW03ADD1		__REG(0xEE0000E8)
#define VIDW04ADD1		__REG(0xEE0000F0)
#define VIDW00ADD2		__REG(0xEE000100)
#define VIDW01ADD2		__REG(0xEE000104)
#define VIDW02ADD2		__REG(0xEE000108)
#define VIDW03ADD2		__REG(0xEE00010C)
#define VIDW04ADD2		__REG(0xEE000110)

#define FB_START_ADD	0x2E000000

static void s3cfb_set_fimd_info(void)
{
	unsigned int temp;

	VIDW00ADD0B0	= 	FB_START_ADD;
	VIDW00ADD0B1	=	FB_START_ADD;
	VIDW00ADD1B0	=	FB_START_ADD + (320*480*4*4);
	VIDW00ADD1B1	=	FB_START_ADD + (320*480*4*4);
	VIDW00ADD2		=	(320*480*4*4);

	temp = CLK_GATE_D1_1_REG;		// Clock gating HCLK for LCD controller
	CLK_GATE_D1_1_REG = temp | 0x1;
	
//	temp = CLK_GATE_SCLK_1_REG;		// Gating special clock for LCD controller
//	CLK_GATE_SCLK_1_REG = temp | 0x1;
	
	temp = CLK_SRC2_REG & 0xFFFFCFFF; // LCD Clock source = DOUT_MPLL
	CLK_SRC2_REG = temp | 0x00001000;
	
	VIDCON0			=	 S3C_VIDCON0_INTERLACE_F_PROGRESSIVE | S3C_VIDCON0_VIDOUT_RGB_IF | S3C_VIDCON0_L1_DATA16_SUB_16_MODE | \
			S3C_VIDCON0_L0_DATA16_MAIN_16_MODE | S3C_VIDCON0_PNRMODE_RGB_P | \
			S3C_VIDCON0_CLKVALUP_ALWAYS | S3C_VIDCON0_CLKDIR_DIVIDED | S3C_VIDCON0_CLKSEL_F_HCLK| \
			S3C_VIDCON0_ENVID_ENABLE| S3C_VIDCON0_ENVID_F_ENABLE| S3C_VIDCON0_CLKVAL_F(133000/9896 + 1);

	VIDCON1 		= 	S3C_VIDCON1_IHSYNC_INVERT | S3C_VIDCON1_IVSYNC_INVERT | S3C_VIDCON1_IVDEN_NORMAL;
	VIDTCON0		=	S3C_VIDTCON0_VBPD(S3C_FB_VBP - 1) | S3C_VIDTCON0_VFPD(S3C_FB_VFP - 1) | S3C_VIDTCON0_VSPW(S3C_FB_VSW - 1);
	VIDTCON1		= 	S3C_VIDTCON1_HBPD(S3C_FB_HBP - 1) | S3C_VIDTCON1_HFPD(S3C_FB_HFP - 1) | S3C_VIDTCON1_HSPW(S3C_FB_HSW - 1);
	VIDTCON2 		= 	S3C_VIDTCON2_LINEVAL(S3C_FB_VRES - 1) | S3C_VIDTCON2_HOZVAL(S3C_FB_HRES - 1);

	VIDOSD0A		= 	S3C_VIDOSDxA_OSD_LTX_F(0) | S3C_VIDOSDxA_OSD_LTY_F(0);
	VIDOSD0B	 	= 	S3C_VIDOSDxB_OSD_RBX_F(S3C_FB_HRES - 1) | S3C_VIDOSDxB_OSD_RBY_F(S3C_FB_VRES - 1);
	VIDOSD0C	 	= 	S3C_FB_HRES*S3C_FB_VRES;

	VIDOSD1A 		= 	S3C_VIDOSDxA_OSD_LTX_F(0) | S3C_VIDOSDxA_OSD_LTY_F(0);
	VIDOSD1B	 	= 	S3C_VIDOSDxB_OSD_RBX_F(S3C_FB_HRES_OSD - 1) | S3C_VIDOSDxB_OSD_RBY_F(S3C_FB_VRES_OSD - 1);

	WINCON0 =  S3C_WINCONx_ENLOCAL_DMA | S3C_WINCONx_BUFSEL_0 | S3C_WINCONx_BUFAUTOEN_DISABLE| \
			S3C_WINCONx_BITSWP_DISABLE | S3C_WINCONx_BYTSWP_DISABLE | S3C_WINCONx_HAWSWP_ENABLE | \
			S3C_WINCONx_BURSTLEN_8WORD| S3C_WINCONx_BPPMODE_F_16BPP_565| S3C_WINCONx_ENWIN_F_ENABLE;

//		s3cfb_fimd.wincon0 = S3C_WINCONx_HAWSWP_ENABLE | S3C_WINCONx_BURSTLEN_16WORD | S3C_WINCONx_BPPMODE_F_16BPP_565;	
/*
	WINCON1 =  S3C_WINCONx_ENLOCAL_DMA | S3C_WINCONx_BUFSEL_0 | S3C_WINCONx_BUFAUTOEN_DISABLE | \
			S3C_WINCONx_BITSWP_DISABLE | S3C_WINCONx_BYTSWP_DISABLE | S3C_WINCONx_HAWSWP_ENABLE | \
			S3C_WINCONx_BURSTLEN_16WORD | S3C_WINCONx_BLD_PIX_PLANE | S3C_WINCONx_BPPMODE_F_16BPP_565 | \
			S3C_WINCONx_ALPHA_SEL_1 | S3C_WINCONx_ENWIN_F_DISABLE;

	WINCON2 = S3C_WINCONx_ENLOCAL_DMA | S3C_WINCONx_BITSWP_DISABLE | S3C_WINCONx_BYTSWP_DISABLE | \
			S3C_WINCONx_HAWSWP_ENABLE | S3C_WINCONx_BURSTLEN_4WORD | S3C_WINCONx_BURSTLEN_16WORD | \
			S3C_WINCONx_BLD_PIX_PLANE | S3C_WINCONx_BPPMODE_F_16BPP_565 | S3C_WINCONx_ALPHA_SEL_1 | S3C_WINCONx_ENWIN_F_DISABLE,

	WINCON3 = S3C_WINCONx_BITSWP_DISABLE | S3C_WINCONx_BYTSWP_DISABLE | S3C_WINCONx_HAWSWP_ENABLE | \
			S3C_WINCONx_BURSTLEN_4WORD | S3C_WINCONx_BURSTLEN_16WORD | S3C_WINCONx_BLD_PIX_PLANE | \
			S3C_WINCONx_BPPMODE_F_16BPP_565 | S3C_WINCONx_ALPHA_SEL_1 | S3C_WINCONx_ENWIN_F_DISABLE;

	WINCON4 = S3C_WINCONx_BITSWP_DISABLE | S3C_WINCONx_BYTSWP_DISABLE | S3C_WINCONx_HAWSWP_ENABLE | \
			S3C_WINCONx_BURSTLEN_4WORD | S3C_WINCONx_BURSTLEN_16WORD | S3C_WINCONx_BLD_PIX_PLANE |
			S3C_WINCONx_BPPMODE_F_16BPP_565 | S3C_WINCONx_ALPHA_SEL_1 | S3C_WINCONx_ENWIN_F_DISABLE;
*/
}



void draw_background(unsigned short color)
{
	unsigned short *fb16;
	int j,k=0;

	fb16 = (ushort *)(FB_START_ADD);

	for(j=0;j<320*480+10000;j++) 
		fb16[k++] = color;
}


void lcdc_init_f (void)
{
	int i,j,k;//, err;
	unsigned int temp;
	unsigned char *bmap;	
	unsigned short *fb16;

//	return;
//	S5PC1XX_LCD *const s5pc_lcdc = S5PC1XX_GetBase_LCD();

//	puts ("*** LCDC init  step1 *** \n\n");


	/* LCD_HSYNC, LCD_VSYNC, LCD_VDEN, LCD_VCLK, VD[23:0] */
	GPF0CON_REG = 0x22222222;
	GPF1CON_REG = 0x22222222;
	GPF2CON_REG = 0x22222222;
	GPF3CON_REG = 0x00002222;	
//	puts ("*** LCDC init  step1.1 *** \n\n");

	/* backlight ON [GPD0] */
	temp = GPDCON_REG;	// Set output
	GPDCON_REG= (temp & 0xFFFFFFF0) | 0x1;
	temp = GPDDAT_REG;	// Set High
	GPDDAT_REG = (temp & 0xFFFFFFFE) | 0x1;
//	puts ("*** LCDC init  step1.2 *** \n\n");

	/* module reset [GPB4] */
	temp = GPBCON_REG;	// Set output (GPB 4,5,6,7)
	GPBCON_REG = (temp & 0x0000FFFF) | 0x11110000;
	temp = GPBDAT_REG;	// Set High (GPB 4,5,6,7)
	GPBDAT_REG= (temp & 0xFFFFFF0F) | 0xF0;
	temp = GPBPUD_REG;
	GPBPUD_REG = (temp & 0xFFFF00FF);
	mdelay(2);
	temp = GPBDAT_REG;	// Set Low
	GPBDAT_REG= (temp & 0xFFFFFFEF);
	mdelay(2);
	temp = GPBDAT_REG;	// Set High
	GPBDAT_REG= (temp & 0xFFFFFFEF) | 0x10;
//	puts ("*** LCDC init  step1.3 *** \n\n");

		fb16 = (ushort *)(FB_START_ADD);
		k=0;
	//	bmap = &logo1[0];	
/*	
		for (i=0; i<S3C_FB_VRES; i++) 
			{
				for(j=0;j<S3C_FB_HRES;j++)
					fb16[j] = logo1[k++];
	
				fb16 += S3C_FB_HRES;
			}
*/		
//		puts ("*** LCDC init  step2 *** \n\n");
/*
		for (i=0; i<(S3C_FB_VRES*S3C_FB_HRES)/0x1000; i++) 
			{
				for(j=0;j<0x1001;j++)
					fb16[j] = logo1[k++];
	
				fb16 += (0x1001+0x96);
			}
*/
/*
		for(j=0;j<160*320+200;j++) 
			fb16[k++] = 0xf800;
		for(j=0;j<160*320+200;j++)
			fb16[k++] = 0x07e0;
		for(j=0;j<(160*320+10000);j++)
			fb16[k++] = 0x001f;
*/

		draw_background(0xAE46);
/*
		for(j=0;j<160*320+200;j++) 
			fb16[k++] = 0xAE46;
		for(j=0;j<160*320+200;j++)
			fb16[k++] = 0xAE46;
		for(j=0;j<(160*320+10000);j++)
			fb16[k++] = 0xAE46;
*/

/*
		for (i=0; i<(S3C_FB_VRES*S3C_FB_HRES)/2; i++) 
			{
//				for(j=0;j<0x1001;j++)
					fb32[i] = logo1[k++];
	
//				fb32 += (0x1001+0x96);
			}
*/		
//		puts ("*** LCDC init  step3 *** \n\n");

	s3cfb_init_ldi();
//	puts ("*** LCDC init  step4 *** \n\n");
	
	s3cfb_set_fimd_info();
//	puts ("*** LCDC init  step5 *** \n\n");
	
}	

#define S5P_FB_SPI_CLK(x)       (S5PC1XX_GPB(1 + (x * 4)))
#define S5P_FB_SPI_MOSI(x)      (S5PC1XX_GPB(2 + (x * 4)))
#define S5P_FB_SPI_CS(x)        (S5PC1XX_GPB(3 + (x * 4)))

void s3cfb_spi_lcd_dclk(int value)
{
	unsigned int temp;

	if(value)
		{
			temp = GPBDAT_REG;	// Set High
			GPBDAT_REG= (temp & 0xFFFFFFDF) | 0x20;
		}
	else
		{
			temp = GPBDAT_REG;	// Set Low
			GPBDAT_REG= (temp & 0xFFFFFFDF);
		}
}

void s3cfb_spi_lcd_dseri(int value)
{
	unsigned int temp;

	if(value)
		{
			temp = GPBDAT_REG;	// Set High
			GPBDAT_REG= (temp & 0xFFFFFFBF) | 0x40;
		}
	else
		{
			temp = GPBDAT_REG;	// Set Low
			GPBDAT_REG= (temp & 0xFFFFFFBF);
		}
}

void s3cfb_spi_lcd_den(int value)
{
	unsigned int temp;

	if(value)
		{
			temp = GPBDAT_REG;	// Set High
			GPBDAT_REG= (temp & 0xFFFFFF7F) | 0x80;
		}
	else
		{
			temp = GPBDAT_REG;	// Set Low
			GPBDAT_REG= (temp & 0xFFFFFF7F);
		}
}

void s3cfb_spi_write(int address, int data)
{
	unsigned int delay = 25;
	unsigned char dev_id = 0x1d;
	int i;

	s3cfb_spi_lcd_den(1);
	s3cfb_spi_lcd_dclk(1);
	s3cfb_spi_lcd_dseri(1);
	udelay_f(delay);

	s3cfb_spi_lcd_den(0);
	udelay_f(delay);

	for (i = 5; i >= 0; i--) {
		s3cfb_spi_lcd_dclk(0);

		if ((dev_id >> i) & 0x1)
			s3cfb_spi_lcd_dseri(1);
		else
			s3cfb_spi_lcd_dseri(0);

		udelay_f(delay);

		s3cfb_spi_lcd_dclk(1);
		udelay_f(delay);
	}

	s3cfb_spi_lcd_dclk(0);
	s3cfb_spi_lcd_dseri(0);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(1);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(0);
	s3cfb_spi_lcd_dseri(0);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(1);
	udelay_f(delay);

	for (i = 15; i >= 0; i--) {
		s3cfb_spi_lcd_dclk(0);

		if ((address >> i) & 0x1)
			s3cfb_spi_lcd_dseri(1);
		else
			s3cfb_spi_lcd_dseri(0);

		udelay_f(delay);

		s3cfb_spi_lcd_dclk(1);
		udelay_f(delay);
	}

	s3cfb_spi_lcd_dseri(1);
	udelay_f(delay);

	s3cfb_spi_lcd_den(1);
	udelay_f(delay * 10);

	s3cfb_spi_lcd_den(0);
	udelay_f(delay);

	for (i = 5; i >= 0; i--) {
		s3cfb_spi_lcd_dclk(0);

		if ((dev_id >> i) & 0x1)
			s3cfb_spi_lcd_dseri(1);
		else
			s3cfb_spi_lcd_dseri(0);

		udelay_f(delay);

		s3cfb_spi_lcd_dclk(1);
		udelay_f(delay);

	}

	s3cfb_spi_lcd_dclk(0);
	s3cfb_spi_lcd_dseri(1);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(1);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(0);
	s3cfb_spi_lcd_dseri(0);
	udelay_f(delay);

	s3cfb_spi_lcd_dclk(1);
	udelay_f(delay);

	for (i = 15; i >= 0; i--) {
		s3cfb_spi_lcd_dclk(0);

		if ((data >> i) & 0x1)
			s3cfb_spi_lcd_dseri(1);
		else
			s3cfb_spi_lcd_dseri(0);

		udelay_f(delay);

		s3cfb_spi_lcd_dclk(1);
		udelay_f(delay);

	}

	s3cfb_spi_lcd_den(1);
	udelay_f(delay);
}

#define FRAME_CYCLE_TIME	25

static void s3cfb_init_ldi(void)
{
	s3cfb_spi_lcd_den(1);
	s3cfb_spi_lcd_dclk(1);
	s3cfb_spi_lcd_dseri(1);
	// power on sequence
	s3cfb_spi_write(0x07, 0x0000);
	mdelay(16);
	
	s3cfb_spi_write(0x11, 0x222f);
	s3cfb_spi_write(0x12, 0x0f00);
	s3cfb_spi_write(0x13, 0x7fe3);
	s3cfb_spi_write(0x76, 0x2213);
	s3cfb_spi_write(0x74, 0x0001);
	s3cfb_spi_write(0x76, 0x0000);
	s3cfb_spi_write(0x10, 0x560c);
	mdelay(FRAME_CYCLE_TIME*7);
	
	s3cfb_spi_write(0x12, 0x0c63);
	mdelay(FRAME_CYCLE_TIME*6);
	
	s3cfb_spi_write(0x01, 0x0B3B);
	s3cfb_spi_write(0x02, 0x0300);
	s3cfb_spi_write(0x03, 0xC040);		// SYNC without DE Mode
	s3cfb_spi_write(0x08, 0x0004);		// VBP
	s3cfb_spi_write(0x09, 0x0008);		// HBP
	s3cfb_spi_write(0x76, 0x2213);
	s3cfb_spi_write(0x0B, 0x3340);
	
	s3cfb_spi_write(0x0C, 0x0020);
	s3cfb_spi_write(0x1C, 0x7770);
	s3cfb_spi_write(0x76, 0x0000);
	s3cfb_spi_write(0x0D, 0x0000);
	s3cfb_spi_write(0x0E, 0x0500);
	s3cfb_spi_write(0x14, 0x0000);
	s3cfb_spi_write(0x15, 0x0803);
	s3cfb_spi_write(0x16, 0x0000);
	s3cfb_spi_write(0x30, 0x0005);
	s3cfb_spi_write(0x31, 0x070F);
	s3cfb_spi_write(0x32, 0x0300);
	s3cfb_spi_write(0x33, 0x0003);
	s3cfb_spi_write(0x34, 0x090C);
	s3cfb_spi_write(0x35, 0x0001);
	s3cfb_spi_write(0x36, 0x0001);
	s3cfb_spi_write(0x37, 0x0303);
	s3cfb_spi_write(0x38, 0x0F09);
	s3cfb_spi_write(0x39, 0x0105);
	
//	printk("power on sequence done...\n");	

	mdelay(4);

	// display on sequence
	s3cfb_spi_write(0x07, 0x0001);
	mdelay(FRAME_CYCLE_TIME*3);
	s3cfb_spi_write(0x07, 0x0101);
	mdelay(FRAME_CYCLE_TIME*3);
	s3cfb_spi_write(0x07, 0x0103);
//	printk("display on sequence done...\n");

}

