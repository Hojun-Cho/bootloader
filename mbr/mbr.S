.file "mbr.S"

#include "def.h"

#define CHAR_LBA_READ '.'
#define CHAR_R	'R'
#define CHAR_S	'S'
#define CHAR_L	'L'
#define CHAR_B	'B'
#define CHAR_G	'G'
#define DBGMSG(c)	movb	$c, %al;	call	Lchr
#define puts(s)		movw	$s, %si;	call	Lmessage

// 0x07C00 - 0x07DFF	BIOS load us here
// 0x07E00 - 0x17BFC	using stack
// 0x07A00 - 0x07BFF	relocate to here
// 0x07C00 - 0x07DFF	load PBR here

.text
.code16
.globl start

start:
	// Adjust %cs to be right
	ljmp	$BOOTSEG, $1f
1:
	// set up stack
	movw	%cs, %ax
	// cli
	movw	%ax, %ss
	movw	$MBRSTACKOFF, %sp

	// set up data segment
	movw	%ax, %ds
	DBGMSG(CHAR_S)

	// relocate for PBR
	// copy to 0x07a0
	movw	$BOOTRELOCSEG, %ax
	movw	%ax, %es
	xorw	%si, %si
	xorw	%di, %di
	movw	$0x200, %cx // Move (E)CX words from DS:[(E)SI] to ES:[(E)DI]
	cld
	rep	movsb
	
	// jump to relocated self
	ljmp	$BOOTRELOCSEG, $reloc

reloc:
	DBGMSG(CHAR_R)

	pushw	%ds
	popw	%es
	pushw	%cs
	popw	%ds
	testb	$DOSACTIVE, %dl
	jnz	drive_ok
	puts(efdmbr)
	jmp	stay_stopped

drive_ok:
	movw 	$pt, %si
	movw	$NDOSPART, %cx

find_active:
	DBGMSG(CHAR_L)
	movb	(%si), %al
	cmpb	$DOSACTIVE, %al
	je	found
	addw	$PARTSZ, %si
	loop	find_active

no_part:
	movw	$enoboot, %si

err_stop:
	call Lmessage

stay_stopped:
	sti
	hlt
	jmp	stay_stopped

found:
	DBGMSG(CHAR_B)
	movb	%dl, %al
	andb	$0x0f, %al
	addb	$'0', %al
	movb	%al, drive_num

	movb	$'0'+4, %al
	subb	%cl, %al
	movb	%al, part_num
	pushw	%si
	movw	$info, %si
	call	Lmessage
	popw	%si
	movw	$0, %es:signature(,1)
	movb	%dl, (%si)
	movw	$0x55AA, %bx
	movb	$0x41, %ah
	int	$0x13
	
	movb	(%si), %dl
	jc	read_error
	cmpw	$0xAA55, %bx
	jne	read_error
	testb	$0x01, %cl
	jz	read_error

do_lba:
	movb	$CHAR_LBA_READ, %al
	call	Lchr
	movl	8(%si), %ecx
	movl	%ecx, lba_sector

	pushw	%si
	movb	$0x42, %ah
	movw	$lba_command, %si
	int	$0x13
	popw	%si
	jnc	booting_os

read_error:
	movw	$eread, %si
	jmp	err_stop

booting_os:
	puts(crlf)

	cmpw	$DOSMBR_SIGNATURE, %es:signature(,1)
	jne	missing_os
	ljmp	$0, $BOOTSEG << 4

missing_os:
	movw	$enoos, %si
	jmp	err_stop

Lmessage:
	pushw	%ax
	cld
1:
	lodsb
	testb	%al, %al
	jz	1f
	call	Lchr
	jmp	1b

Lchr:
	pushw	%ax
	pushw	%bx
	movb	$0x0e, %ah
	movw	$1, %bx
	int	$0x10
	popw	%bx
1:	popw	%ax
	ret

lba_command:
	.byte	0x10
	.byte	0x00
	.word	0x0001
	.word	0
	.word	BOOTSEG
lba_sector:
	.long	0, 0

info:	.ascii	"Using drive "
drive_num:
	.byte	'X'
	.ascii	", partition "
part_num:
	.asciz	"Y"

efdmbr: .asciz	"MBR on floppy of old BIOS\r\n"
eread:	.asciz	"\r\nRead error\r\n"
enoos:	.asciz	"No O/S\r\n"
enoboot: .ascii	"No active partion"
crlf: .asciz "\r\n"

endofcode:
	nop

// partion table
	. = DOSPARTOFF // partion table start address
pt: .fill 0x40,1,0

	. = 0x1fe
signature:
	.short DOSMBR_SIGNATURE
	. = 0x200

