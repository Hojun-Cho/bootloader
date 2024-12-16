// IO Port	Access Type	Purpose
// 0x60	Read/Write	Data Port
// 0x64	Read	Status Register
// 0x64	Write	Command Register
#define TABWIDTH 8
#ifndef BOOT_DEBUG
#define BOOT_DEBUG 1
#endif

#define DOSPARTOFF 446
#define NDOSPART 4

enum{
	CON_PC,
	CON_SRI,
	CON_END,
};

struct BIOSreg{
	u32 ax; 
	u32 cx; 
	u32 dx; 
	u32 bx; 
	u32 bp; 
	u32 si; 
	u32 di; 
	u32 ds; 
	u32 es; 
}__attribute__((packed));

typedef struct{
	char *name;
	void (**probes)(void);
	uint cnt;
}BootProbe;

typedef struct ConDev ConDev;
struct ConDev{
	int (*probe)(ConDev*);
	void (*init)(ConDev *);
	int (*getc)(int);
	void (*putc)(int, int);
	uint dev;
	uchar pri; // the higher the better
};
typedef struct{
	u64 addr;		/* Beginning of block */
	u64 size;		/* Size of block */
	u32 type;		/* Type of block */
} __attribute__((packed)) BIOSmmap;

// fmt.c
typedef struct{
	int ucase;
	int padch;
	char *p;
	char *ep;
	int f1, f2, f3;
	va_list ap;
}Op;

// disk.c
typedef struct{
	u8 flag;	/* bootstrap flags */
	u8 bhd;	/* begin head */
	u8 bsec;	/* begin sector */
	u8 bcyl;	/* begin cylinder */
	u8 type;	/* partition type (see below) */
	u8 ehd;	/* end head */
	u8 esec;	/* end sec2r */
	u8 ecyl;	/* end cylinder */
	u32 beg;	/* absolute starting sectoff number */
	u32 size;	/* partition size in sec2rs */
} __attribute__((packed)) DOSpart;

typedef struct{
	u8 boot[DOSPARTOFF];
	DOSpart parts[NDOSPART];
	u16 sign;
} __attribute__((packed)) DOSmbr;

typedef struct{
	int n;
	u32 ncyl;
	u32 nhead;
	u32 nsec;
	i32 edd;
	u32 dev ;
	u32 checksum;
	u32 flag;
} __attribute__((packed)) BIOSdisk;

typedef struct{

}Disklabel;

typedef struct{
	BIOSdisk bdsk;	// bios disk info
	Disklabel label;

	int bootdev, osdev;
}Disk;

// gdt.S
extern volatile struct BIOSreg BIOSreg;

// boot.c
extern void (*probe1[])(void);
extern void (*probe2[])(void);
extern BootProbe probes[];
extern BIOSmmap biosmmap[64];
extern ConDev contab[CON_END];
extern ConDev *con;
extern uint cnvmem, extmem;
extern int debug;