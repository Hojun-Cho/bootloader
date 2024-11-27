#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <stdio.h>
#include <limits.h>
#include <endian.h>
#include <stdlib.h>
#include "dat.h"

static char *binpath = "build/mbr";
static char bootcode[DOSPARTOFF];
static Disk dsk;

void
usage(void)
{
	fprintf(stderr, "usage: ./mkmbr device\n");
	exit(1);
}

static int
secwrite(void *buf, u64 sec, u32 cnt, Disk *dsk)
{
	ssize_t wc;
	off_t off;
	ssize_t n = cnt * dsk->bps;
	off_t where = sec * dsk->bps;

	off = lseek(dsk->fd, where, SEEK_SET);
	assert(off != -1);
	wc = write(dsk->fd, buf, n);
	assert(wc != -1);
	assert(wc == n);	
	return wc;
}

char*
secread(u64 sec, u32 cnt, Disk *dsk)
{
	char *buf;
	ssize_t rc;
	off_t off;
	ssize_t n = cnt * dsk->bps;
	off_t where = sec * dsk->bps;

	off = lseek(dsk->fd, where, SEEK_SET);
	assert(off != -1);
	buf = calloc(1, n);
	assert(buf != NULL);
	rc = read(dsk->fd, buf, n);
	assert(rc != -1);
	assert(rc == n);	
	return buf;
}

int
diskwrite(void *buf, u64 sec, u64 sz, Disk *dsk)
{
	char *secbuf;
	u32 cnt;
	int wc;

	cnt = (sz +  dsk->bps - 1) / dsk->bps;
	secbuf = secread(sec, cnt, dsk);
	memcpy(secbuf, buf, sz);
	wc = secwrite(secbuf, sec, cnt, dsk);
	free(secbuf);
	return wc;
}

Part
dospt2pt(DOSpart dpt, u64 self, u64 ext)
{
	Part pt={0,};
	off_t off;
	u32 t;

	assert(dpt.type != DOSPTYP_FAT16);
	pt.flag = dpt.flag;
	pt.id = dpt.type;
	off = self;
	memcpy(&t, &dpt.beg, sizeof(u32));
	pt.bs = htole32(t) + off;
	memcpy(&t, &dpt.size, sizeof(u32));
	pt.ns = htole32(t);
	return pt;
}

// C = LBA / (HPC * SPT)
// H = (LBA / SPT) % HPC
// S = (LBA / SPT) + 1

CHS
lba2chs(u64 lba, u64 spt, u64 hpc)
{
	CHS c = {0,};

	c.cyl = lba / (spt * hpc);
	c.head = (lba / spt) % hpc;
	c.sec = (lba % spt) + 1;
	return c;
}

CHS
lba2chsbeg(Part pt)
{
	if(pt.ns == 0 || pt.id == DOSPTYP_UNUSED)
		return (CHS){0,};
	return lba2chs(pt.bs, dsk.spt, dsk.hpc);
}

CHS
lba2chsend(Part pt)
{
	if(pt.ns == 0 || pt.id == DOSPTYP_UNUSED)
		return (CHS){0,};
	return lba2chs(pt.bs+pt.ns-1, dsk.spt, dsk.hpc);
}

CHS
chsnorm(u8 id, CHS c)
{
	if(c.head > 254 || c.sec > 63 || c.cyl > 1023){
		c.head = 254;
		c.sec = 63;
		c.cyl = 1023;
	}
	c.head = c.head & 0xFF;
	c.sec = (c.sec & 0x3F) | ((c.cyl & 0x300) >> 2);
	c.cyl = c.cyl & 0xFF;
	return c;
}

DOSpart
pt2dospt(Part pt, u64 self, u64 ext)
{
	DOSpart d={0,};
	CHS c;
	u64 off, t;

	if(pt.ns == 0 || pt.id == DOSPTYP_UNUSED)
		return d;
	off = self;
	c = chsnorm(pt.id, lba2chsbeg(pt));
	d.bcyl = c.cyl;
	d.bhd = c.head;
	d.bsec = c.sec;
	c = chsnorm(pt.id, lba2chsend(pt));
	d.ecyl = c.cyl;
	d.ehd = c.head;
	d.esec = c.sec;
	d.flag = pt.flag & 0xff;
	d.type = pt.id & 0xff;
	t = pt.bs - off;
	memcpy(&d.beg, &t, sizeof(d.beg));
	t = htole64(pt.ns);
	memcpy(&d.size, &t, sizeof(d.size));
	return d;
}

DOSmbr
mbr2dosmbr(MBR m)
{
	DOSmbr d={0,};

	memcpy(d.boot, m.code, sizeof(m.code));
	d.sign = htole16(DOSMBR_SIGNATURE);
	for(int i = 0; i < elem(d.parts); ++i){
		DOSpart	dpt = {0,};
		if(i < elem(m.parts))
			dpt = pt2dospt(m.parts[i], m.self, m.ext);
		d.parts[i] = dpt;
	}
	return d;
}

MBR
mbrinit(void)
{
	MBR m = {0,};
	DOSpart dpt={0,};
	Part bpt={0,}, mpt={0,};
	u32 i;

	bpt = dospt2pt(dpt, 0, 0);
	i = 1;
	while(i < dsk.spt)
		i *= 2;
	mpt.bs = i;
	mpt.ns = dsk.nsec - mpt.bs;
	mpt.id = DOSPTYP_FAT16;
	mpt.flag = DOSACTIVE;

	memcpy(m.code, bootcode, sizeof(bootcode));
	m.parts[0] = bpt;
	m.parts[3] = mpt;
	m.sign = DOSMBR_SIGNATURE;
	return m;
}

int
mbrwrite(MBR m)
{
	DOSmbr d;
	int wc;

	d = mbr2dosmbr(m);
	wc = diskwrite(&d, m.self, sizeof(d), &dsk);
	return wc; 
}

void
read512(char *path)
{
	int fd;
	ssize_t rc;

	assert((fd = open(path, O_RDONLY)) != -1);
	rc = read(fd, bootcode, sizeof(bootcode));
	assert(rc == sizeof(bootcode));
	close(fd);
}

int
main(int argc, char *argv[])
{
	MBR m;
	char buf[1024];

	printf("%s\n", getcwd(buf, sizeof(buf)));
	if(argc < 2)
		usage();
	read512(binpath);
	dsk = diskopen(argv[1], O_RDWR);
	m = mbrinit();
	printf("write %dbyte to %s\n", mbrwrite(m), dsk.name);
	return 0;
}
