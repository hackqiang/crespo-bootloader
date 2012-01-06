#include <config.h>
#include <s5pc100.h>

	.globl mem_ctrl_asm_init
mem_ctrl_asm_init:

	ldr	r0, =APB_DMC_BASE			@APB_DMC_BASE 0xE6000000

	ldr	r1, =PRO_ID_BASE
	ldr	r2, [r1, #PRO_ID_OFFSET]
	bic	r2, #0xfffffdff
	mov	r2, r2, lsr #9
	cmp	r2, #0x1
	beq	onenand_pop

single:

/************ delay loop *************/

#if 0
	ldr		r1, =0x10000000
	mov 		r2, #0
loop1:

	cmp		r2, r1
	addne	r2, r2, #0x1
	bne loop1
#endif

/************ DLL initialization *************/
      
        ldr     r1, =0x6A101000				@ Phycontrol0 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL0]

        ldr     r1, =0x000084F4                       	 @Phycontrol1 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL1]

        ldr     r1, =0x00000000                        	 @Phycontrol2 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL2]

        ldr     r1, =0x6A101002			    	  @DLL on
        str     r1, [r0, #DMC_PHYCONTROL0]

        ldr     r1, =0x6A101003			  	@Dll start
        str     r1, [r0, #DMC_PHYCONTROL0]



	ldr	r2, = 0xE6000040		@DMC_PHYSTATUS0

loop1:

	ldr	r1, [r2]				@Check DLL lock
	ands	r1, r1, #4
	beq	loop1

	ldr	r1, [r2]
	mov	r1, r1,  LSR #(0x6)
	and	r1, r1, #(0xff)
	mov	r1, r1, LSL  #(0x18)
    ldr 	r2,  = 0xE6000018			@DMC_PHYCONTROL0
	ldr	r3, [r2]
	bic	r3,  r3, #(0xff000000)
	orr	r1, r3, r2
	str	r1, [r2]



        ldr     r1, =0x6A101003			@Force Value locking
        str     r1, [r0, #DMC_PHYCONTROL0]

        ldr     r1, =0x6A101009			@Dll off
        str     r1, [r0, #DMC_PHYCONTROL0]



#if 0
        ldr     r1, =0x6A101000				@ Phycontrol0 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL0]

        ldr     r1, =0x00008484                             @Phycontrol1 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL1]

        ldr     r1, =0x00000000                             @Phycontrol2 DLL parameter setting
        str     r1, [r0, #DMC_PHYCONTROL2]

#endif
/************ DLL initialization - END *************/




        ldr     r1, =0x0FF01010                         @auto refresh off
        str     r1, [r0, #DMC_CONCONTROL]

        ldr     r1, =0x00212400                         @ BL=4 , 2 chip , DDR2
        str     r1, [r0, #DMC_MEMCONTROL]

#if 1	// add charles 512MB enable
        ldr     r1, =0x20E01323							@ 512MB config , 8banks , linear, Row=14bit,COL=10bit
        str     r1, [r0, #DMC_MEMCONFIG0]

        ldr     r1, =0x40E00323
        str     r1, [r0, #DMC_MEMCONFIG1]
#else	// 256MB enable
        ldr     r1, =0x20F80313							@ 128MB config , 8banks , linear, Row=13bit,COL=10bit
        str     r1, [r0, #DMC_MEMCONFIG0]

        ldr     r1, =0x40F80313
        str     r1, [r0, #DMC_MEMCONFIG1]
#endif
  
        ldr     r1, =0x20000000		
        str     r1, [r0, #DMC_PRECHCONFIG]


 		ldr     r1, =0x00100004			@ PwrdnConfig
        str     r1, [r0, #DMC_PWRDNCONFIG]

#ifdef	CONFIG_HCLKD0_222
	ldr 	r1, =0x000006c3 						@7.8us*222MHz=0x6c3, 7.8us*166MHz=1294(0x50E)
       str     r1, [r0, #DMC_TIMINGAREF]
	/* T-rfc   127.5nS/5ns  64 */
       ldr     r1, =0x202332C8                         		@TimingRow      @222MHz
       str     r1, [r0, #DMC_TIMINGROW]

       ldr     r1, =0x24450304							@CL=5
       str     r1, [r0, #DMC_TIMINGDATA]

 #else


      	ldr     r1, =0x0000050E		
        str     r1, [r0, #DMC_TIMINGAREF]

		ldr	 r1, =0x16233297						 @TimingRow 	 @166MHz
        str     r1, [r0, #DMC_TIMINGROW]

@;		ldr	 r1, =0x24250304						 @CL=5
		ldr	 r1, =0x23230000						 @CL=3
        str     r1, [r0, #DMC_TIMINGDATA]
 #endif

        ldr     r1, =0x07c80232    	                   @ Timing Power
        str     r1, [r0, #DMC_TIMINGPOWER]

/* Direct Command for DDR2 */
        ldr     r1, =0x07000000                         @chip0 Deselect
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x01000000                        @chip0 PALL
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x00020000                         @chip0 EMRS2
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x00030000                         @chip0 EMRS3
        str     r1, [r0, #DMC_DIRECTCMD]

		ldr     r1, =0x00010400                         @chip0 EMRS1 (MEM DLL on = DQS# disable)
        str     r1, [r0, #DMC_DIRECTCMD]

@;		ldr     r1, =0x00000552                         @chip0 MRS (MEM DLL reset) CL=5, Burst Length=4
        ldr     r1, =0x00000532                         @chip0 MRS (MEM DLL reset) CL=3, Burst Length=4
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x01000000                         @chip0 PALL
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x05000000                         @chip0 REFA
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x05000000                         @chip0 REFA
        str     r1, [r0, #DMC_DIRECTCMD]

@;		ldr     r1, =0x00000452                         @chip0 MRS (MEM DLL unreset) , BL=4 , CL=5
		ldr     r1, =0x00000432                         @chip0 MRS (MEM DLL unreset) , BL=4 , CL=3 
        str     r1, [r0, #DMC_DIRECTCMD]

        ldr     r1, =0x00010780                         @chip0 EMRS1 (OCD default)
        str     r1, [r0, #DMC_DIRECTCMD]


// 	    ldr     r1, =0x00010400                         @chip0 EMRS1 (OCD exit)
	    ldr     r1, =0x00010402                         @chip0 EMRS1 (OCD exit)  Reduced Strength
//      ldr     r1, =0x00010000                         @chip0 EMRS1 (OCD exit)  ODT Disabled
        str     r1, [r0, #DMC_DIRECTCMD]


/* Direct Command for LPDDR  - END */
 
	ldr     r1, =0x00FF20B0			@ConControl auto refresh on
        str     r1, [r0, #DMC_CONCONTROL]

#if 0
        ldr     r1, =0x001000FF			@ PwrdnConfig
        str     r1, [r0, #DMC_PWRDNCONFIG]
#endif

        ldr     r1, =0x00212413		        @ MemControl
        str     r1, [r0, #DMC_MEMCONTROL]

        b	exit_cpu_init


onenand_pop:
	ldr	r1, =0x50101000				@Phycontrol0 DLL parameter setting
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x000000F4				@Phycontrol1 DLL parameter setting
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000000				@Phycontrol2 DLL parameter setting
	str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x50101002				@Dll on
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x50101003				@dll start
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x50101003				@Force Value locking
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x50101001				@Dll off
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0xFF001010				@auto refresh off
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00212100				@Dll off
	str	r1, [r0, #DMC_MEMCONTROL]

@;	ldr	r1, =0x28F80222
	ldr	r1, =0x28F00222
	str	r1, [r0, #DMC_MEMCONFIG0]

	ldr	r1, =0x20F80222
	str	r1, [r0, #DMC_MEMCONFIG1]

	ldr	r1, =0x20000000
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x0000050E
	str	r1, [r0, #DMC_TIMINGAREF]

	ldr	r1, =0x0C233287				@TimingRow	@133MHz
	str	r1, [r0, #DMC_TIMINGROW]

	ldr	r1, =0x32330303
	str	r1, [r0, #DMC_TIMINGDATA]

	ldr	r1, =0x04141433				@Timing Power
	str	r1, [r0, #DMC_TIMINGPOWER]

	ldr	r1, =0x07000000				@chip0 Deselect
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01000000				@chip0 PALL
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05000000				@chip0 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05000000				@chip0 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00000032				@chip0 MRS
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x07100000				@chip1 Deselect
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01100000				@chip1 PALL
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05100000				@chip1 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05100000				@chip1 REFA
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00100032				@chip1 MRS
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0xFF002030				@ConControl auto refresh on
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00100002				@PwrdnConfig
	str	r1, [r0, #DMC_PWRDNCONFIG]

@;	ldr	r1, =0xFF212113				@MemControl
	ldr	r1, =0xFF212100				@MemControl
	str	r1, [r0, #DMC_MEMCONTROL]

	b	exit_cpu_init

exit_cpu_init:
	mov	pc, lr


/* Below code is for ARM926EJS and ARM1026EJS */
	.globl cleanDCache
cleanDCache:
	mrc	p15, 0, pc, c7, c10, 3	/* test/clean D-Cache */
	bne	cleanDCache
	mov	pc, lr

	.globl cleanFlushDCache
cleanFlushDCache:
	mrc	p15, 0, pc, c7, c14, 3	/* test/cleanflush D-Cache */
	bne	cleanFlushDCache
	mov	pc, lr

	.globl cleanFlushCache
cleanFlushCache:
	mrc	p15, 0, pc, c7, c14, 3	/* test/cleanflush D-Cache */
	bne	cleanFlushCache
	mcr	p15, 0, r0, c7, c5, 0	/* flush I-Cache */
	mov	pc, lr

	.ltorg

