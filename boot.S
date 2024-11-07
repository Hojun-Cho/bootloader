#include "mbr.h"

#define BOOTSEG	0x7c0 // segment where we are loaded
#define BOOTRELOCSEG	0x7a0 // segment where we relocate to
#define BOOTSTACKOFF	0xfffc // stack starts here, grows down
#define PARTSZ	16

#define CHAR_R	'R'
#define CHAR_S	'S'
#define CHAR_L	'L'
#define CHAR_B	'B'
#define DBGMSG(c)	movb	$c, %al;	call	Lchr
#define puts(s)		movw	$s, %si;	call	Lmessage

// 0x07C00 - 0x07DFF	BIOS load us here
// 0x07E00 - 0x17BFC	using stack
// 0x07A00 - 0x07BFF	relocate to here
// 0x07C00 - 0x07DFF	load PBR here

.text
.code16

	// Adjust %cs to be right
	ljmp	$BOOTSEG, $1f
1:
	// set up stack
	movw	%cs, %ax
	// cli
	movw	%ax, %ss
	movw	$BOOTSTACKOFF, %sp

	// set up data segment
	movw	%ax, %ds
	DBGMSG(CHAR_S)

	// relocate for PBR
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
	call stay_stopped

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

efdmbr: .asciz	"MBR on floppy of old BIOS\r\n"
enoboot: .ascii	"No active partion"
clrf: .asciz "\r\n"

endofcode:
	nop

// (MBR) NT disk signature offset
	. = 0x1b8
	.space 4, 0

// partion table
	. = DOSPARTOFF // partion table start address
pt: .fill 0x40,1,0
	. = 0x1fe
signature:
	.short DOSMBR_SIGNATURE
	. = 0x200

