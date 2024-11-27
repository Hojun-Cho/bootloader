#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <limits.h>
#include <assert.h>
#include "def.h"

#define elem(x) ((int)(sizeof(x)/sizeof((x)[0])))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned int uint;

typedef struct{
	u64 cyl;
	u64 head;
	u64 sec;
}CHS;

typedef struct{
	u8	flag;	/* bootstrap flags */
	u8	bhd;		/* begin head */
	u8	bsec;	/* begin sector */
	u8	bcyl;	/* begin cylinder */
	u8	type;		/* partition type (see below) */
	u8	ehd;		/* end head */
	u8	esec;	/* end sec2r */
	u8	ecyl;	/* end cylinder */
	u32	beg;	/* absolute starting sectoff number */
	u32	size;	/* partition size in sec2rs */
} __attribute__((packed)) DOSpart;

typedef struct{
	u8		boot[DOSPARTOFF];
	DOSpart parts[NDOSPART];
	u16		sign;
} __attribute__((packed)) DOSmbr;

typedef struct
{
	u64 bs, ns;
	u8 flag, id;
}Part;

typedef struct
{
	DOSpart parts[NDOSPART];
	u32 bps; // byte per sector 
	u32 spt; // sector per track
	u32 hpc; // header per cylinder
	u32 nsec; // number of sectors;
	u32 type; // major minor;
	int fd;
	char *name;
}Disk;

typedef struct
{
	u32 ext;
	u32 self;
	u8 code[DOSPARTOFF];
	Part parts[NDOSPART];
	u16 sign;
	u16 zeros;
}MBR;

static Disk
diskopen(char *name, int flag)
{
	char buf[512];
	u64 nsec;
	struct stat st;
	Disk d = {.bps=512, .spt=63, .hpc=255,};

	d.name = name;
	assert((d.fd = open(d.name, flag)) != -1);
	assert(fstat(d.fd, &st) != -1);
	if(S_ISBLK(st.st_mode))
		assert(ioctl(d.fd, BLKGETSIZE, &nsec) != -1);
	else if(S_ISREG(st.st_mode))
		nsec = st.st_size/d.bps;
	else
		assert(0);
		
	d.nsec = nsec < FAT16_MAX ? nsec : FAT16_MAX;
	assert(sizeof(buf) == read(d.fd, buf, sizeof(buf)));
	memcpy(d.parts, buf+DOSPARTOFF, sizeof(d.parts));
	assert(lseek(d.fd, 0, SEEK_SET) == 0);

	return d;
}
