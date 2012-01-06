/*
 *
 */

#include <common.h>
#include <asm/io.h>
#include <linux/mtd/onenand_regs.h>

#ifdef CONFIG_BOOT_ONENAND_IROM

#define ONENAND_BASE		0xB0000000

#if defined(CONFIG_MCP_AC) || defined(CONFIG_MCP_B) || defined(CONFIG_MCP_D)
#define ONENAND_PAGE_SIZE	4096
#elif defined(CONFIG_MCP_H)
#define ONENAND_PAGE_SIZE	2048
#else	// default page size (for V210 daughter board)
#define ONENAND_PAGE_SIZE	2048
#endif

#if defined(CONFIG_MCP_AC) || defined(CONFIG_MCP_B) || defined(CONFIG_MCP_D) || defined(CONFIG_MCP_H)
#define UBOOT_PHY_BASE		0x33E00000 /* For C110 */
#elif defined(CONFIG_MCP_SINGLE)
#define UBOOT_PHY_BASE		0x23E00000 /* For V210 */
#else
#error "No MCP type is not defined!"
#endif

#if defined(CONFIG_EVT1)
#if defined(CONFIG_FUSED)
#define FWBL1_SIZE		(4096)
#define UBOOT_START_BLOCK	(FWBL1_SIZE / ONENAND_PAGE_SIZE)
#else
#define UBOOT_START_BLOCK	(0)
#endif
#else
#error		"Unsupported H/W version!"
#error		"To support EVT0, a seperated BL1 is required!"
#endif


#define READ_INTERRUPT()	\
	onenand_readw(ONENAND_BASE + ONENAND_REG_INTERRUPT)

#define onenand_block_address(block)		(block)
#define onenand_sector_address(page)		(page << ONENAND_FPA_SHIFT)
#define onenand_buffer_address()		((1 << 3) << 8)
//#define onenand_buffer_address()		((0 << 3) << 8)
#define onenand_bufferram_address(block)	(0)

inline unsigned short onenand_readw (unsigned int addr)
{
	return *(unsigned short*)addr;
}

inline void onenand_writew (unsigned short value, unsigned int addr)
{
	*(unsigned short*)addr = value;
}

void onenand_readpage (
	void* base, unsigned int block, unsigned int page)
{
	int len;
	unsigned short* dest;
	unsigned short* src;

	// Block Number
	onenand_writew(onenand_block_address(block),
			ONENAND_BASE + ONENAND_REG_START_ADDRESS1);

	// BufferRAM
	onenand_writew(onenand_bufferram_address(block),
			ONENAND_BASE + ONENAND_REG_START_ADDRESS2);

	// Page (Sector) Number Set: FPA, FSA
	onenand_writew(onenand_sector_address(page),
			ONENAND_BASE + ONENAND_REG_START_ADDRESS8);

	// BSA, BSC
	onenand_writew(onenand_buffer_address(),
			ONENAND_BASE + ONENAND_REG_START_BUFFER);

	// Interrupt clear
	onenand_writew(ONENAND_INT_CLEAR, ONENAND_BASE + ONENAND_REG_INTERRUPT);

	onenand_writew(ONENAND_CMD_READ, ONENAND_BASE + ONENAND_REG_COMMAND);

#if 1
	while (!(READ_INTERRUPT() & ONENAND_INT_READ))
		continue;
#else
	while (!(READ_INTERRUPT() & ONENAND_INT_MASTER))
		continue;
#endif

	len = ONENAND_PAGE_SIZE >> 1;
	dest = (unsigned short *)(base);
	src = (unsigned short *)(ONENAND_BASE + ONENAND_DATARAM);
	while (len-- > 0)
	{
		*dest++ = *src++;
	}
}

/*
 * Copy data from pages [block:from  ~ block:to] to base address
 */
void onenand_readpages (
	void* base, unsigned int block, unsigned int from, unsigned int to)
{
	unsigned int page = from;
	for (; page < to; page++) {
		onenand_readpage((void *)base, block, page);
		base += ONENAND_PAGE_SIZE;
	}
}

/*
 * Copy U-Boot from OneNAND to DRAM (368KB)
 */
void onenand_bl2_copy(void)
{
	unsigned int base = UBOOT_PHY_BASE;

	onenand_readpages((void *)base, 0, UBOOT_START_BLOCK, 64);
	base += ONENAND_PAGE_SIZE*64;
#if (ONENAND_PAGE_SIZE == 4096)
	onenand_readpages((void *)base, 1, 0, 32);
#else
	onenand_readpages((void *)base, 1, 0, 64);
#endif
}

#endif /* CONFIG_BOOT_ONENAND_IROM */

