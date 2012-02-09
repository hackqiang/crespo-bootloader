#
# BL1 of U-Boot for OneNAND
#

sinclude ../../include/autoconf.mk

CROSS_COMPILE = /opt/arm-2010q1/bin/arm-none-eabi-

AS				= $(CROSS_COMPILE)as
LD				= $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

MKBL1			= mkbl1

TARGET          = BL1.bin
O_TARGET		= BL1.elf
OBJS			= Init.o
OBJS            += Main.o


FLAG4REV		= S5PC110_EVT1
TEXT_BASE       = 0xD0020010
BL1_LDS         = BL1_evt1.lds

AFLAGS			= --defsym $(FLAG4REV)=0x1
CFLAGS			= -I../include -Os -D$(FLAG4REV)
CFLAGS_HOST		= -D$(FLAG4REV)



all: $(TARGET) $(MKBL1) $(TARGET).padding $(TARGET).signed

$(TARGET):      $(OBJS)
	$(LD) -T $(BL1_LDS) -Ttext $(TEXT_BASE) -o $(O_TARGET) -Map BL1.map $(OBJS)
	$(OBJCOPY) -O binary $(O_TARGET) $(TARGET)

$(TARGET).padding:	$(TARGET) $(MKBL1)
	./$(MKBL1) $(TARGET) $(TARGET).padding

$(TARGET).signed: $(TARGET).padding c110.signedBL1_bin
	@cat c110.signedBL1_bin $(TARGET).padding > $(TARGET).signed

$(MKBL1): mkbl1.c
	gcc $(CFLAGS_HOST) -o $(MKBL1) mkbl1.c

Main.o: Main.c
	$(CC) $(CFLAGS) -c -o Main.o Main.c

Init.o: Init.s
	$(AS) $(AFLAGS) -o Init.o Init.s

dep:
	gccmakedep $(OBJS:.o=.c)(OBJS:.o=.s)

clean:
	rm -rf $(OBJS) $(TARGET) $(O_TARGET) BL1.map $(TARGET).padding $(TARGET).signed $(MKBL1)

# DO NOT DELETE

