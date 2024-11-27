.file "srt0.S"

#define BOOTSTACK 0xfffc
#define	CR0_PE	0x00000001	/* Protected mode Enable */

	.globl	end
	.globl	edata
	.globl	Gdtr
	.globl	boot

	.text
	.code16
	.align 16
	.globl	_start
_start:
	popl	%eax
	cmpl	$BOOTMAGIC, %eax
	je	1f

1:
	popl	%edx
	cli
	pushl	%cs
	popl	%ds
	addr32	data32 lgdt	(Gdtr - LINKADDR)
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	data32	movl %eax, %cr0
	data32	ljmp $8, $1f

1:
	.code32
	movl	$0x10, %eax
	mov	%ax, %ds
	mov	%ax, %ss
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	movl	$BOOTSTACK, %esp
	pushl	%edx


	// fill 0 .bss
	xorl	%eax, %eax

	call	boot;

