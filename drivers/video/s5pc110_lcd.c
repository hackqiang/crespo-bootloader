//------------------------------------------------------------------------------
#include <common.h>
#include <s5pc110.h>

#include <asm/io.h>
#include <regs.h>

//------------------------------------------------------------------------------
#include "regs-fb.h"

//------------------------------------------------------------------------------
// Bitmap struct define
//------------------------------------------------------------------------------
typedef struct tagBITMAPFILEHEADER__t
{  
	unsigned short	bfType;  
	unsigned int	bfSize;  
	unsigned short	bfReserved1;  
	unsigned short	bfReserved2;  
	unsigned int	bfOffBits;  
}	__attribute__((packed)) tagBITMAPFILEHEADER_t;

typedef struct tagBITMAPINFOHEADER__t
{  
	unsigned int	biSize;  
	unsigned long	biWidth;  
	unsigned long	biHeight;  
	unsigned short	biPlanes;  
	unsigned short	biBitCount;  
	unsigned int	biCompression;  
	unsigned int	biSizeImage;  
	unsigned long	biXPelsPerMeter;  
	unsigned long	biYPelsPerMeter;  
	unsigned int	biClrUsed;  
	unsigned int	biClrImportant;  
}	__attribute__((packed)) tagBITMAPINFOHEADER_t;

typedef struct tagRGBTRIPLE__t	{
	unsigned char	rgbtBlue;
	unsigned char	rgbtGreen;
	unsigned char	rgbtRed;
}	__attribute__((packed)) tagRGBTRIPLE_t;

typedef struct tagRGBQUAD_t	{
	unsigned char	rgbBlue;
	unsigned char	rgbGreen;
	unsigned char	rgbRed;
	unsigned char	rgbReserved;
}	__attribute__((packed)) tagRGBQUAD_t;

//------------------------------------------------------------------------------
static inline void udelay_f(unsigned long usec)
{
	unsigned long loops = usec *300;
	 __asm__ volatile ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

#define mdelay(x)	udelay_f(1000*x)

//------------------------------------------------------------------------------
void draw_background(unsigned int color)
{
	unsigned int *fb = (unsigned int *)(FB_START_ADDR);
	unsigned int i, j;
	
	for(i=0; i<LCD_SIZE_HIGHT; i++)	{
		for(j=0; j<LCD_SIZE_WIDTH; j++)	fb[(i*LCD_SIZE_WIDTH)+j] = color;
	}	
}

//------------------------------------------------------------------------------
void backlight_onoff(unsigned char status)
{
	if(status)	__REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET)  |= ( 0x00000001);
	else		__REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET)  &= (~0x00000001);
}

//------------------------------------------------------------------------------
void lcdc_init_f (void)
{
	unsigned int temp;

	#if defined(FB_TEST)
		puts ("==== LCD FB Initialized ==== \n");
	#endif

	/* backlight ON [GPD0.0], LCD Power ON [GPD0.1], LVDS Enable [GPD0.2] */
	temp = __REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET) ;	// Set High
	__REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET)  = (temp & 0xFFFFFFF0);

	temp = __REG(ELFIN_GPIO_BASE + GPD0CON_OFFSET) ;	// Set output
	__REG(ELFIN_GPIO_BASE + GPD0CON_OFFSET)= (temp & 0xFFFFF000) | 0x111;

	temp = __REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET) ;	// Set High(Power/LVDS on)
	__REG(ELFIN_GPIO_BASE + GPD0DAT_OFFSET)  = (temp & 0xFFFFFFF0) | 0x6;

	draw_background(0x00000000);	// FB Init(All Clear)
	
	#if defined(FB_TEST)
		puts ("==== LCD Backlight, Power, LVDS Initialized ==== \n");
	#endif

	/* LCD_HSYNC, LCD_VSYNC, LCD_VDEN, LCD_VCLK, VD[23:0] */
	__REG(ELFIN_GPIO_BASE + GPF0CON_OFFSET)	= 0x22222222;
	__REG(ELFIN_GPIO_BASE + GPF1CON_OFFSET) = 0x22222222;
	__REG(ELFIN_GPIO_BASE + GPF2CON_OFFSET) = 0x22222222;
	__REG(ELFIN_GPIO_BASE + GPF3CON_OFFSET) = 0x00002222;	

	#if defined(FB_TEST)
		puts ("==== LCD Port Initialized ==== \n");
	#endif

	temp = __REG(ELFIN_CLOCK_POWER_BASE + CLK_GATE_IP1_OFFSET);
	__REG(ELFIN_CLOCK_POWER_BASE + CLK_GATE_IP1_OFFSET) = temp | 0x1;

	temp = __REG(ELFIN_CLOCK_POWER_BASE + CLK_GATE_BLOCK_OFFSET);
	__REG(ELFIN_CLOCK_POWER_BASE + CLK_GATE_BLOCK_OFFSET) = temp | 0x8;

	temp = __REG(ELFIN_CLOCK_POWER_BASE + NORMAL_CFG_OFFSET);
	__REG(ELFIN_CLOCK_POWER_BASE + NORMAL_CFG_OFFSET) = temp | 0x8;

	temp = __REG(ELFIN_CLOCK_POWER_BASE + CLK_SRC1_OFFSET) & 0xFF0FFFFF; // LCD Clock source = DOUT_MPLL
	__REG(ELFIN_CLOCK_POWER_BASE + CLK_SRC1_OFFSET) = temp | 0x00200000;

	temp = __REG(ELFIN_CLOCK_POWER_BASE + CLK_SRC_MASK0_OFFSET) ;
	__REG(ELFIN_CLOCK_POWER_BASE + CLK_SRC_MASK0_OFFSET) = temp & 0xFFFFFFDF;

	__REG(ELFIN_CLOCK_POWER_BASE + 0x7008) = 0x03;
	udelay_f(1000);

	#if defined(FB_TEST)
		puts ("==== LCD Clock, Frame buffer, FB-DMA Initialized ==== \n");
	#endif

	__REG(ELFIN_VIDEO_BASE + 0x130) = 0x00009001;
	__REG(ELFIN_VIDEO_BASE + 0x4) 	= 0x00000060;
	__REG(ELFIN_VIDEO_BASE + 0x10) 	= 0x00180902;
	__REG(ELFIN_VIDEO_BASE + 0x14) 	= 0x00e5591d;
	__REG(ELFIN_VIDEO_BASE + 0x18) 	= 0x0012bbff;
	__REG(ELFIN_VIDEO_BASE + 0x20) 	= 0x00008034;
	__REG(ELFIN_VIDEO_BASE + 0x44) 	= 0x001ffa57;
	__REG(ELFIN_VIDEO_BASE + 0x48) 	= 0x00096000;
	__REG(ELFIN_VIDEO_BASE + 0x4C) 	= 0x00096000;
	__REG(ELFIN_VIDEO_BASE + 0xA0) 	= FB_START_ADDR;
	__REG(ELFIN_VIDEO_BASE + 0xD0) 	= FB_END_ADDR;
	__REG(ELFIN_VIDEO_BASE + 0x100) = LCD_SIZE_WIDTH * COLOR_ARGB8888;
	__REG(ELFIN_VIDEO_BASE + 0x0) 	= 0x00000093;
	__REG(ELFIN_VIDEO_BASE + 0x20)	= 0x00008035;
	__REG(ELFIN_VIDEO_BASE + 0x34) 	= 0x00000001;

	#if defined(FB_TEST)
		puts ("==== LCD Backlight On ==== \n");
		backlight_onoff(1);
	#endif
}	

//------------------------------------------------------------------------------
void logo_display (unsigned char mode)
{
#if defined(CONFIG_BOOT_SPLASH)


NoImage:
	switch(mode)	{
		default	:
		case	BOOTLOGO_NORMAL    	:	draw_background(0x0000FF00);	break;	// Green
		case	BOOTLOGO_RECOVERY	:	draw_background(0x00FF0000);	break;	// Red
		case	BOOTLOGO_UPGRADE	:	draw_background(0x000000FF);	break;	// Green
		case	BOOTLOGO_LOWBATTERY	:	draw_background(0x00FFFF00);	break;	// Yellow
		case	BOOTLOGO_BOOTLOADER	:	draw_background(0x00FFFFFF);	break;	// White
	}
	backlight_onoff(1);		mdelay(1000);
			
#endif	
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
