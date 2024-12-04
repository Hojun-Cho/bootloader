.file "srt0.S"

#include "def.h"

	.globl	end
	.globl	edata
	.globl	Gdtr
	.globl	boot
	.globl	pmode_init

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
// load GDT
	cli
	pushl	%cs
	popl	%ds
	addr32	data32 lgdt	(Gdtr - LINKADDR)
	movl	%cr0, %eax
// enable protected mode
	orl	$CR0_PE, %eax
	data32	movl %eax, %cr0
	data32	ljmp $S32TEXT, $1f

1:
	.code32
	movl	$0x10, %eax
	mov	%ax, %ds
	mov	%ax, %ss
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	movl	$BOOTSTACKOFF, %esp
	pushl	%edx

	// fill 0 .bss
	xorl	%eax, %eax
	movl	$end, %ecx
	subl	$edata, %ecx
	movl	$edata, %edi
	cld
	rep;	stosb

	call	pmode_init
	call	boot
