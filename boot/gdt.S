.file "gdt.S"

// Trap
#define	T_PRIVINFLT	 0	/* privileged instruction */
#define	T_BPTFLT	 1	/* breakpoint trap */
#define	T_ARITHTRAP	 2	/* arithmetic trap */
#define	T_RESERVED	 3	/* reserved fault base */
#define	T_PROTFLT	 4	/* protection fault */
#define	T_TRCTRAP	 5	/* trace trap */
#define	T_PAGEFLT	 6	/* page fault */
#define	T_ALIGNFLT	 7	/* alignment fault */
#define	T_DIVIDE	 8	/* integer divide fault */
#define	T_NMI		 9	/* non-maskable interrupt */
#define	T_OFLOW		10	/* overflow trap */
#define	T_BOUND		11	/* bounds check fault */
#define	T_DNA		12	/* device not available fault */
#define	T_DOUBLEFLT	13	/* double fault */
#define	T_FPOPFLT	14	/* fp coprocessor operand fetch fault (![P]Pro)*/
#define	T_TSSFLT	15	/* invalid tss fault */
#define	T_SEGNPFLT	16	/* segment not present fault */
#define	T_STKFLT	17	/* stack fault */
#define	T_MACHK		18	/* machine check ([P]Pro) */
#define	T_XFTRAP	19	/* SIMD FP exception */

/* memory segment types */
#define	SDT_MEMRO	16	/* memory read only */
#define	SDT_MEMROA	17	/* memory read only accessed */
#define	SDT_MEMRW	18	/* memory read write */
#define	SDT_MEMRWA	19	/* memory read write accessed */
#define	SDT_MEMROD	20	/* memory read only expand dwn limit */
#define	SDT_MEMRODA	21	/* memory read only expand dwn limit accessed */
#define	SDT_MEMRWD	22	/* memory read write expand dwn limit */
#define	SDT_MEMRWDA	23	/* memory read write expand dwn limit acessed */
#define	SDT_MEME	24	/* memory execute only */
#define	SDT_MEMEA	25	/* memory execute only accessed */
#define	SDT_MEMER	26	/* memory execute read */
#define	SDT_MEMERA	27	/* memory execute read accessed */
#define	SDT_MEMEC	28	/* memory execute only conforming */
#define	SDT_MEMEAC	29	/* memory execute only accessed conforming */
#define	SDT_MEMERC	30	/* memory execute read conforming */
#define	SDT_MEMERAC	31	/* memory execute read accessed conforming */

#define S32TEXT	0x08	// segment selector of text section
#define	SDT_SYS386TGT	15	/* system 386 trap gate */

#define IPROC(n)	X##n
#define idte(e) \
	movl $IPROC(e), %eax ; call entry_idt

#define IENTRY_ERR(name,err) \
IPROC(name): \
	pushl	$err ; \
	jmp 1f	

.text
.code32
.globl	pmm_init

entry_idt:
//	Table
//		IDTR offset + 0 : entry 0
//		IDTR offset + 8 : entry 1
//		
//	Gate Descriptor
//		63 ~ 48	: offset low
//		47 ~ 32	: info
//		31 ~ 16	: segment selector
//		15 ~ 0	: offset high
	movw	%ax, (%ebx)
	movw	$S32TEXT, 2(%ebx)
	movw	$((0x80|SDT_SYS386TGT) << 8), 4(%ebx)
	shr	$16, %eax
	movw	%ax, 6(%ebx)
	addl	$8, %ebx
	ret

	.align 8, 0x90
pmm_init:
	movl	$idt, %ebx
	movl	$Idtr, %eax
	movw	$(640 - 1), (%eax)
	movl	%ebx, 2(%eax)
	/* interal interrupts 0~31 */
	idte(de); idte(db); idte(nmi); idte(bp); idte(of); idte(br)
	idte(ud); idte(nm); idte(df);  idte(fo); idte(ts); idte(np)
	idte(ss); idte(gp); idte(pf);  idte(xx); idte(mf); idte(ac)
	idte(mc)
	idte(xx); idte(xx); idte(xx);  idte(xx); idte(xx); idte(xx)
	idte(xx); idte(xx); idte(xx);  idte(xx); idte(xx); idte(xx)
	idte(xx) 

	lidt	Idtr
	ret

	.bss
	.align	8, 0x90
idt:
	.space	640

	.globl	Idtr
Idtr:
	.word	0
	.long	0
	.word	0

	.text
	.align	8
gdt:
		/* 0x00 : null */
	.space	8
		/* 0x08 : flat code */
	.word	0xFFFF			# lolimit
	.word	0			# lobase
	.byte	0			# midbase
	.byte	SDT_MEMERAC | 0 | 0x80	# RXAC, dpl = 0, present
	.byte	0xf | 0 | 0x40 | 0x80	# hilimit, xx, 32bit, 4k granularity
	.byte	0			# hibase
		/* 0x10 : flat data */
	.word	0xFFFF			# lolimit
	.word	0			# lobase
	.byte	0			# midbase
	.byte	SDT_MEMRWA | 0 | 0x80	# RWA, dpl = 0, present
	.byte	0xf | 0 | 0x40 | 0x80	# hilimit, xx, 32bit, 4k granularity
	.byte	0			# hibase
		/* 0x18 : 16 bit code */
	.word	0xFFFF			# lolimit
	.word	(LINKADDR & 0xffff)	# lobase
	.byte	(LINKADDR >> 16) & 0xff	# midbase
	.byte	SDT_MEMERAC | 0 | 0x80	# RXAC, dpl = 0, present
	.byte	0x0 | 0 | 0 | 0		# hilimit, xx, 16bit, byte granularity
	.byte	(LINKADDR >> 20) & 0xff	# hibase
		/* 0x20 : 16 bit data */
	.word	0xFFFF			# lolimit
	.word	(LINKADDR & 0xffff)	# lobase
	.byte	(LINKADDR >> 16) & 0xff	# midbase
	.byte	SDT_MEMRWA | 0 | 0x80	# RWA, dpl = 0, present
	.byte	0x0 | 0 | 0 | 0		# hilimit, xx, 16bit, byte granularity
	.byte	(LINKADDR >> 20) & 0xff	# hibase

	
//	Register GDT 
.globl	Gdtr
Gdtr:
	.word	. - gdt -1
	.long	gdt
	.word	0

// ENTRY Reserved
IPROC(xx):
	pushl	$1
	pushl	$T_RESERVED
	jmp	1f

// trap entry points
IENTRY_ERR(de,T_DIVIDE)	/* #DE divide by zero */
IENTRY_ERR(db,T_TRCTRAP)	/* #DB debug */
IENTRY_ERR(nmi,T_NMI)		/* NMI */
IENTRY_ERR(bp,T_BPTFLT)	/* #BP breakpoint */
IENTRY_ERR(of,T_OFLOW)	/* #OF overflow */
IENTRY_ERR(br,T_BOUND)	/* #BR BOUND range exceeded */
IENTRY_ERR(ud,T_PRIVINFLT)	/* #UD invalid opcode */
IENTRY_ERR(nm,T_DNA)		/* #NM device not available */
IENTRY_ERR(df,T_DOUBLEFLT)		/* #DF double fault */
IENTRY_ERR(fo,T_FPOPFLT)	/* #FO coprocessor segment overrun */
IENTRY_ERR(ts,T_TSSFLT)		/* #TS invalid TSS */
IENTRY_ERR(np,T_SEGNPFLT)		/* #NP segment not present */
IENTRY_ERR(ss,T_STKFLT)		/* #SS stack fault */
IENTRY_ERR(gp,T_PROTFLT)		/* #GP general protection */
IENTRY_ERR(pf,T_PAGEFLT)		/* #PF page fault */
IENTRY_ERR(mf,T_ARITHTRAP)	/* #MF floating point error */
IENTRY_ERR(ac,T_ALIGNFLT)		/* #AC alignment check */
IENTRY_ERR(mc,T_MACHK)		/* #MC machine check */

1:
	jmp	alltraps

// in boot mode don't want handle trap
alltraps:
	hlt
	iret

// bios interrupt entry point
// 	switch to real to protected mode and emulate 16bit mode

	.globl	EMUh
	.align	8, 0x90
EMUh:
	pushl	%eax

.end