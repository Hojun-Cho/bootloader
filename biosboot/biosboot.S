.file "biosboot.S"

#include "def.h"

// 0x00000 -> 0x07BFF	our stack		(to  31k)
// 0x07A00 -> 0x07BFF	typical MBR loc		(at  30k5)
// 0x07C00 -> 0x07DFF	our code biosboot	(at  31k)
// 0x07E00 -> ...    	/boot inode block	(at  31k5)
// 0x07E00 -> ...    	(indirect block if nec)
// 0x40000 -> ...	/boot			(at 256k)

#define PBR_READ_ERROR			'R'
#define PBR_CANT_BOOT			'X'
#define PBR_BAD_MAGIC			'M'
#define PBR_TOO_MANY_INDIRECTS		'I'
#define CHAR_BLOCK_READ		'.'
#define	putc(c)		movb	$c, %al;	call	Lchr
#define	puts(s)		movw	$s, %si;	call	Lmessage

	.globl	start_cluster, dst_seg_off
	.globl	spc_sqrt, clu_off
	.globl	part_offset, shift_magic, and_magic

	.type start_cluster, @function
	.type dst_seg_off, @function
	.type spc_sqrt, @function
	.type clu_off, @function
	.type part_offset, @function
	.type shift_magic, @function
	.type and_magic, @function

.text
.code16
.globl	_start

_start:
	jmp	begin
	nop

	. = _start + 0x1c
ebpb:	.long	16			/* hidden sectors */
	.long	0			/* large sectors */
	.word	0			/* physical disk */
	.byte	0x29			/* signature, needed by NT */
	.space	4, 0			/* volume serial number */
	.asciz	"YO   LABEL"
	.asciz	"FAT  16"


	. = _start + 0x3e
begin:
	ljmp $BOOTSEG, $main

cant_boot:
	movb	$PBR_CANT_BOOT, %al
	jmp	err_print_crlf

main:
	xorw	%ax, %ax
	movw	%ax, %ss
	movw	$BOOTSTACKOFF, %sp

// data segment == code segment
	pushw	%cs
	popw	%ds
	movw	$load_msg, %si
	call	Lmessage
	testb	$0x80, %dl
	jz	cant_boot	

load_boot:
	start_cluster = .+1
	movw	$0x9090, %ax
	movw	$LOADSEG, %bx	 // destination
	movw	%bx, %di
	movw	$FATSEG, %cx
	movw	%cx, %es
1:
	call	read_cluster

	read_fat:
		pushw	%bx
		pushw	%ax
		shift_magic = .+1
		movb	$0x90, %cl
		shrw	%cl, %ax	// get sector
		addw	%ds:0x0e, %ax	// add reserved sector
		movw	$FATSEG, %bx
		call	read_sector
		popw	%cx
		popw	%bx
		and_magic = .+2
		andw	$0x9090, %cx
		movw	%es:(,%ecx, 2), %ax
		cmpw	$0xFFF7, %ax
		jb	1b

	jmp	done_load

read_cluster:
	pushw	%ax

	spc_sqrt = .+2
	shlw	$0x90, %ax
	clu_off = .+1
	addw	$0x9090, %ax

	movzb	%ds:0x0d, %cx
1:
	call	read_sector
	incw	%ax
	dst_seg_off = .+2
	addw	$0x9090, %bx
	loop	1b

	popw	%ax
	ret

read_sector:
	pushw	%ax
	movw	%bx, lba_dst_seg
	part_offset = .+1
	addw	$0x9090, %ax
	movl	%eax, lba_src_low 
	movw	$lba_command, %si

do_int_13:
	movb	$0x42, %ah
	int	$0x13
	jc	read_error

	popw	%ax
	ret

read_error:
	movb	$PBR_READ_ERROR, %al
err_print_crlf:
	movw	$err_txt_crlf, %si
	jmp	err_print2
	call	stay_stopped
Lmessage:
	cld
1:
	lodsb
	orb	%al, %al
	jz	1f
	call Lchr
	jmp	1b

Lchr:
	pushw	%bx
	movb	$0x0e, %ah
	xorw	%bx, %bx
	incw	%bx
	int	$0x10
	popw	%bx
1:
	ret

done_load:
	movw	$LOADSEG, %ax
	movw	%ax, %es
	cmpl	$ELFMAGIC, %es:0(,1)
	je	exec_boot
	movb	$PBR_BAD_MAGIC, %al

err_print:
	movw	$err_txt, %si
err_print2:
	movb	%al, err_id
err_stop:
	call	Lmessage
stay_stopped:
	sti
	hlt
	jmp	stay_stopped

exec_boot:
	movzbl	%dl, %eax
	pushl	%eax
	pushl	$BOOTMAGIC
	ljmp	$(LINKADDR >> 4), $0

lba_command:
	.byte	0x10			/* size of command packet */
	.byte	0x00			/* reserved */
lba_nread_sec:			
	.word	0x01			// sectors to transfer max 127
lba_dst_offset:
	.word	0x00			// target buffer offset
lba_dst_seg:
	.word	0x00			// target buffer offset
lba_src_low:
	.long	0x00
lba_src_high:
	.long	0x00

load_msg: .asciz	"Loading"
err_txt_crlf: .ascii	"\r\n"
err_txt: .ascii	"ERR "
err_id: .ascii	"?"
crlf: .asciz	"\r\n"

. = 0x200 - 2
	.word DOSMBR_SIGNATURE
