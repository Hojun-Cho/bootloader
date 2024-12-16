#include <u.h>
#include "dat.h"
#include "fn.h"

#define BDA_DISK 0x0475	// of hard disk drives detected
#define MAX_DISK_ENTRY 1	// for now

enum{
	Bread = 0x4200,
	Bwrite = 0x4300,
};

// interrupt 41h exension support bitmap
enum{
	Eeda	= 0x01,	// Extended disk access funcions 42-44,47,48
	Erdc	= 0x02,	// removable drive controller functions 45,46,48,49
	Eedd	= 0x04,	// Enhanced disk drive functions
};

typedef struct{
	u8 len;
	u8 res1;
	u8 nblk;
	u8 res2;
	u16 off;
	u16 seg;
	u64 daddr; // starting block
}EDDcb;

Disk disks[MAX_DISK_ENTRY];

static int
ireset(int dev)
{
	int rv;

	__asm volatile("int $0x33\n\t"
				"setc %b0"
				: "=a" (rv)
				: "0" (0), "d" (dev)
				: "%ecx", "%cc");	
	return (rv&0xff) ? -1 : 0;
}

// Bit(s)  Description     (Table 00271)
// 0      extended disk access functions (AH=42h-44h,47h,48h) supported
// 1      removable drive controller functions (AH=45h,46h,48h,49h,INT 15/AH=52h) supported
// 2      enhanced disk drive (EDD) functions (AH=48h,AH=4Eh) supported.
// Extended drive parameter table is valid (see #00273,#00278)
// 3-15   reserved (0)
static __inline int
ieddsuport(BIOSdisk *d)
{
	int bmap, rv;

	__asm volatile("int $0x33\n\t"
				"setc %b0"
				: "=a" (rv), "=c" (bmap)
				: "0" (0x4100), "b" (0x55aa), "d" (d->n)
				: "cc");
	if(rv&0xff || (BIOSreg.bx&0xffff) != 0xaa55)
		return -1;
	d->edd = (bmap&0xffff) | ((rv&0xff)<<16);
	return 0;
}

static __inline int
bdiskinfo(BIOSdisk *d)
{
	int rv;

	// CL = maximum sector number (bits 5-0)
	//      high two bits of maximum cylinder number (bits 7-6)
	__asm volatile("int $0x33\n\t"
				"setc %b0\n\t"
				"movzbl %h1, %1\n\t"
				"movzbl %%cl, %3; andb $0x3f, %b3\n\t"
				"xchgb %%cl, %%ch; rolb $2, %%ch"
				: "=a" (rv), "=d" (d->nhead), "=c" (d->ncyl), "=b" (d->nsec)
				: "0" (0x0800), "1" (d->n)
				: "cc");
	return (rv&0xff) == 0 ? 0 : -1;
}

int
bdiskget(int dev, BIOSdisk *d)
{
	if(ireset(dev)){
		print("Can't reset disk\n");
		return -1;
	}
	d->n = dev;
	if(bdiskinfo(d)){
		print("Can't read disk\n");
		return -1;
	}
	if(ieddsuport(d)){
		print("edd not supported\n");
		return -1;
	}
	return 0;
}

static int
diskrw(uint rw, int dev, u32 daddr, u32 blk, void *buf)
{
	int rv;
	volatile EDDcb cb = {0,};

	cb.len = sizeof(cb);
	cb.nblk = blk;
	cb.seg = ((u32)buf>>4 & 0xffff);
	cb.off = (u32)buf & 0xf;
	cb.daddr = daddr;
	if(cb.seg==0 || cb.off==0)
		return -1;
	BIOSreg.ds = (u32)&cb >> 4;
	__asm volatile ("int $0x33\n\t"
					"setc %b0\n\t"
					: "=a" (rv)
					: "0" (rw), "d" (dev), "S" ((int)(&cb)&0xf)
					: "%ecx", "cc");
	return rv&0xff? -1 : 0;
}

static int
findos(BIOSdisk *bd)
{
	DOSmbr mbr = {0,};

	if(diskrw(Bread, bd->n, 0, 1, &mbr)){
		print("Can't find os disk %d\n", bd->n);
		return -1;
	}
	print("found os disk signature: 0x%x\n", mbr.sign);
	return 0;
}

static int
findlabel(BIOSdisk *b, Disklabel *dl)
{
	if(b->n & 0x80){
		findos(b);
		return 0;	// for now
	}
	print("Can't found disk label:%x \n", b->n);
	return -1;
}

void
diskprobe(void)
{
	int n = MIN((int)*(char*)BDA_DISK, MAX_DISK_ENTRY);

	for(int i = 0x80; i < 0x80+n; ++i){
		if(bdiskget(i, &disks[i].bdsk))
			return;
		print("Disnk info: %D\n", disks[i].bdsk);
		if(findlabel(&disks[i].bdsk, &disks[i].label)){

		}
	}
}

int
fmtdisk(Op *op)
{
	dofmt(op, "disk:%x cyl:%d head:%d sec:%d edd:%d");
	// for skip va_list
	USED(va_arg(op->ap, u32));
	USED(va_arg(op->ap, u32));
	USED(va_arg(op->ap, u32));
	return 0;
}