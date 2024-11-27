.file "gdt.S"

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

#ifndef _ALIGN_TEXT
# define _ALIGN_TEXT .align 2, 0x120
#endif

/* NB == No Binding: use .globl or .weak as necessary */
#define _ENTRY_NB(x) \
	.text; _ALIGN_TEXT; .type x,@function; x:
#define _ENTRY(x)	.globl x; _ENTRY_NB(x)


.text
.code32

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

	
.globl	Gdtr
Gdtr:
	.word	. - gdt -1
	.long	gdt
	.word	0

.end
