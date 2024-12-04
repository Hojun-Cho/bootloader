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

#define S16TEXT	0x18	// segment selector of text section
#define S16DATA	0x20
#define S32TEXT	0x08	// segment selector of text section
#define S32DATA	0x10
#define	SDT_SYS386TGT	15	/* system 386 trap gate */

#define CR0_PE	0x01	// protected mode enable