.file "gdt.S"

#include "def.h"

#define GEN_LABEL(n)	X##n

// idt entry
#define idte(e) 			\
	movl	$GEN_LABEL(e), %eax ;	\
	call	entry 

// idt bios entry
#define idtbe(b)	idte(emu##b)

#define GEN_TRAP(name, err)	\
	GEN_LABEL(name):	\
		pushl	$err;	\
		jmp 1f	

#define	GEN_EMU(n)		\
	GEN_LABEL(emu##n): 	\
		pushl $n; 	\
		jmp 1f


#define prot2real					\
	ljmp	$S16TEXT, $1f - LINKADDR;		\
1:							\
	.code16; 					\
	movw	$S16DATA, %ax;				\
	movw	%ax, %ds;				\
	movw	%ax, %es;				\
							\
	movl	%cr0, %eax;				\
	andl	$~CR0_PE, %eax;				\
	movl	%eax, %cr0;				\
							\
	data32 ljmp $(LINKADDR >> 4), $1f - LINKADDR;	\
1:							\
	movw	%cs, %ax;				\
	movw	%ax, %ds;				\
	movw	%ax, %es;				\
	xorw	%ax, %ax;				\
	movw	%ax, %ss;				\
	data32 addr32	lidt (Idtr_real - LINKADDR);

#define real2prot					\
	movw	$LINKADDR >> 4, %ax;			\
	movw	%ax, %ds;				\
	data32	addr32 lgdt (Gdtr - LINKADDR);		\
							\
	movl	%cr0, %eax;				\
	orl	$CR0_PE, %eax;				\
	movl	%eax, %cr0;				\
							\
	data32 ljmp	$S32TEXT, $1f;			\
1:							\
	.code32;					\
	mov	$S32DATA, %eax;				\
	mov	%ax, %ds;				\
	mov	%ax, %ss;				\
	mov	%ax, %es;				\
							\
	lidt	Idtr;

.text
.globl	BIOSreg	
BIOSreg:
	BIOS_AX: .long 0 
	BIOS_CX: .long 0 
	BIOS_DX: .long 0 
	BIOS_BX: .long 0 
	BIOS_BP: .long 0 
	BIOS_SI: .long 0 
	BIOS_DI: .long 0 
	BIOS_DS: .long 0 
	BIOS_ES: .long 0 

.text
.code32
.globl	pmode_init	

//	Table
//	IDTR offset + 0 : entry 0
//	IDTR offset + 8 : entry 1
//	
//	Gate Descriptor
//	63 ~ 48	: offset low
//	47 ~ 32	: info
//	31 ~ 16	: segment selector
//	15 ~ 0	: offset high
entry:
	movw	%ax, (%ebx)
	movw	$S32TEXT, 2(%ebx)
	movw	$((0x80|SDT_SYS386TGT) << 8), 4(%ebx)
	shr	$16, %eax
	movw	%ax, 6(%ebx)
	addl	$8, %ebx
	ret

// init IDT for protected mode
	.align 8, 0x90
pmode_init:
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
	/* BIOS interrupt call (32-63) */
	idtbe(0);  idtbe(1);  idtbe(2);  idtbe(3);  idtbe(4);  idtbe(5)
	idtbe(6);  idtbe(7);  idtbe(8);  idtbe(9);  idtbe(10); idtbe(11)
	idtbe(12); idtbe(13); idtbe(14); idtbe(15); idtbe(16); idtbe(17)
	idtbe(18); idtbe(19); idtbe(20); idtbe(21); idtbe(22); idtbe(23)
	idtbe(24); idtbe(25); idtbe(26); idtbe(27); idtbe(28); idtbe(29)
	idtbe(30); idtbe(31); idtbe(32); idtbe(33); idtbe(34); idtbe(35)
	idtbe(36); idtbe(37); idtbe(38); idtbe(39); idtbe(40); idtbe(41)
	idtbe(42); idtbe(43); idtbe(44); idtbe(45); idtbe(46); idtbe(47)

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
	.globl	Idtr_real
Idtr_real:
	.word	1023 // 256 entry, 1k
	.long	0
	.word	0

	.align	8
gdt:
	/* 0x00 : null */
	.space	8
	/* 0x08 : flat code */
	.word	0xFFFF		 // lolimit
	.word	0		 // lobase
	.byte	0		 // midbase
	.byte	SDT_MEMERAC | 0 | 0x80	 // RXAC, dpl = 0, present
	.byte	0xf | 0 | 0x40 | 0x80	 // hilimit, xx, 32bit, 4k granularity
	.byte	0		 // hibase
	/* 0x10 : flat data */
	.word	0xFFFF		 // lolimit
	.word	0		 // lobase
	.byte	0		 // midbase
	.byte	SDT_MEMRWA | 0 | 0x80	 // RWA, dpl = 0, present
	.byte	0xf | 0 | 0x40 | 0x80	 // hilimit, xx, 32bit, 4k granularity
	.byte	0		 // hibase
	/* 0x18 : 16 bit code */
	.word	0xFFFF		// lolimit
	.word	(LINKADDR & 0xffff)	 // lobase
	.byte	(LINKADDR >> 16) & 0xff	 // midbase
	.byte	SDT_MEMERAC | 0 | 0x80	 // RXAC, dpl = 0, present
	.byte	0x0 | 0 | 0 | 0	 // hilimit, xx, 16bit, byte granularity
	.byte	(LINKADDR >> 20) & 0xff	 // hibase
	/* 0x20 : 16 bit data */
	.word	0xFFFF		 // lolimit
	.word	(LINKADDR & 0xffff)	 // lobase
	.byte	(LINKADDR >> 16) & 0xff	 // midbase
	.byte	SDT_MEMRWA | 0 | 0x80	 // RWA, dpl = 0, present
	.byte	0x0 | 0 | 0 | 0	 // hilimit, xx, 16bit, byte granularity
	.byte	(LINKADDR >> 20) & 0xff	 // hibase

//	Register GDT 
.globl	Gdtr
Gdtr:
	.word	. - gdt - 1
	.long	gdt
	.word	0

// ENTRY Reserved
GEN_LABEL(xx):
	pushl	$1
	jmp	1f

// trap entry points
GEN_TRAP(de,T_DIVIDE)	/* DE divide by zero */
GEN_TRAP(db,T_TRCTRAP)	/* DB debug */
GEN_TRAP(nmi,T_NMI)	/* NMI */
GEN_TRAP(bp,T_BPTFLT)	/* BP breakpoint */
GEN_TRAP(of,T_OFLOW)	/* OF overflow */
GEN_TRAP(br,T_BOUND)	/* BR BOUND range exceeded */
GEN_TRAP(ud,T_PRIVINFLT)	/* UD invalid opcode */
GEN_TRAP(nm,T_DNA)	/* NM device not available */
GEN_TRAP(df,T_DOUBLEFLT)	/* DF double fault */
GEN_TRAP(fo,T_FPOPFLT)	/* FO coprocessor segment overrun */
GEN_TRAP(ts,T_TSSFLT)	/* TS invalid TSS */
GEN_TRAP(np,T_SEGNPFLT)	/* NP segment not present */
GEN_TRAP(ss,T_STKFLT)	/* SS stack fault */
GEN_TRAP(gp,T_PROTFLT)	/* GP general protection */
GEN_TRAP(pf,T_PAGEFLT)	/* PF page fault */
GEN_TRAP(mf,T_ARITHTRAP)	/* MF floating point error */
GEN_TRAP(ac,T_ALIGNFLT)	/* AC alignment check */
GEN_TRAP(mc,T_MACHK)	/* MC machine check */
1:
	popl	%ecx // what trap
	hlt
	jmp	1b

GEN_EMU(0);  GEN_EMU(1);  GEN_EMU(2);  GEN_EMU(3)
GEN_EMU(4);  GEN_EMU(5);  GEN_EMU(6);  GEN_EMU(7)
GEN_EMU(8);  GEN_EMU(9);  GEN_EMU(10); GEN_EMU(11)
GEN_EMU(12); GEN_EMU(13); GEN_EMU(14); GEN_EMU(15)
GEN_EMU(16); GEN_EMU(17); GEN_EMU(18); GEN_EMU(19)
GEN_EMU(20); GEN_EMU(21); GEN_EMU(22); GEN_EMU(23)
GEN_EMU(24); GEN_EMU(25); GEN_EMU(26); GEN_EMU(27)
GEN_EMU(28); GEN_EMU(29); GEN_EMU(30); GEN_EMU(31)
GEN_EMU(32); GEN_EMU(33); GEN_EMU(34); GEN_EMU(35)
GEN_EMU(36); GEN_EMU(37); GEN_EMU(38); GEN_EMU(39)
GEN_EMU(40); GEN_EMU(41); GEN_EMU(42); GEN_EMU(43)
GEN_EMU(44); GEN_EMU(45); GEN_EMU(46); GEN_EMU(47)
1:	jmp	EMUh


// bios interrupt entry point
// switch to real to protected mode and emulate 16bit mode
	.globl	EMUh
	.align	8, 0x90
EMUh: // build stack for real mode
	mov	%eax, 5f	// save number to code segment not in the data segment
	pop	%eax

	pusha
	push	%ds	
	push	%es
	push	%fs
	push	%gs

	movb	%al, intno	// save BIOS int vector

// BIOS_regs is  area for saving the contents of registers returned by the BIOS during a BIOS CALL
	movl	BIOS_ES, %eax
	movl	$0x00, %eax
	mov	%eax, 7f
	movl	BIOS_DS, %eax
	movl	$0x00, %eax
	mov	%eax, 6f	

	prot2real

	// save ds to stack and rewrite es, ds
	push	%ds

	// data32 movl $Leax, %eax
	.byte	0x66, 0xb8
7:	.long	0x90909090
	mov	%ax, %es

	// data32 movl $Leax, %eax
	.byte	0x66, 0xb8
6:	.long	0x90909090
	mov	%ax, %ds

	// data32 movl $Leax, %eax => now ax is holding interrupt nummber
	.byte	0x66, 0xb8
5:	.long	0x90909090

	;sti
	int	$0 // do interrupt
intno = . -1;
	cli;

	// restore register
	// preserve bx,es for protected mode
	pop	%ds
	addr32	movl %eax, (2f - LINKADDR)
	movl	%ebx, %eax
	addr32	movl %eax, (4f - LINKADDR)
	movl	%es, %eax
	addr32	movl %eax, (3f- LINKADDR)
	addr32	movl (2f - LINKADDR), %eax

	// save eflags to bh
	movb	%ah, %bh
	lahf
	xchgb	%ah, %bh

	// preserve ax for protected mode
	addr32	movl %eax, (2f - LINKADDR)

	real2prot

	// movl $Leax, %eax
	.byte	0xb8
4:	.long	0x90909090
	mov	%eax, BIOS_BX

	// movl $Leax, %eax
	.byte	0xb8
3:	.long	0x90909090
	mov	%eax, BIOS_ES

	// movl $Leax, %eax
	.byte	0xb8
2:	.long	0x90909090

	// pass BIOS return values back to caller
	movl	%eax, 0xb*4(%esp)
	movl	%ecx, 0xa*4(%esp)
	movl	%edx, 0x9*4(%esp)
	movb	%bh, 0xe*4(%esp)	// restore eflags

	// save register into BIOSREG
	.code32
	movl	%eax, BIOS_AX
	movl	%ecx, BIOS_CX
	movl	%edx, BIOS_DX
	movl	%ebp, BIOS_BP
	movl	%esi, BIOS_SI
	movl	%edi, BIOS_DI

	// clear NT(Nested Task Flag: 14) flag in eflag
	// if 1 : interrupting
	pushf
	pop	%eax
	and	$0xffffbfff, %eax
	push	%eax
	popf

	pop	%ds	
	pop	%es
	pop	%fs
	pop	%gs
	popa
	iret
