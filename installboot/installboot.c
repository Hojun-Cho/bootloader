#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include "btld_conf.h"
#include "btld_disk.h"
#include "elf32.h"

static char *devpath;
static char *stage1 =  "build/biosboot";
static char *shpath =  "build/filecopy.sh";
static Disk dsk;
static int btpart;
static char btcode[512];

// need patch
static struct{
	char *name;
	u32 size;
	u32 addr;
	union{
		u32 val;
		struct{
			u16 low;
			u16 high;
		};
		u8 byte;
	};
}syms[] = {
	{"start_cluster", 2},
	{"dst_seg_off", 2},
	{"spc_sqrt", 1},
	{"clu_off", 2},
	{"part_offset", 2},
	{"shift_magic", 1},
	{"and_magic", 2},
};

static void
usage(void)
{
	fprintf(stderr, "usgae: ./installboot device [bin]\n");
	exit(1);
}

static int
streq(char *a, char *b)
{
	int la = strlen(a);
	return la==strlen(b) && memcmp(a, b, la)==0;
}

static void
loadelf(char *fname)
{
	int fd;
	char *tbbeg, *tbstr;
	Ehdr h;
	Ephdr ph;
	Eshdr shsym, shstr;
	Esym *tbsym;

	assert((fd = open(fname, O_RDONLY))!=-1);
	h = readEhdr(fd);
	assert(h.e_phnum == 1); // only 1 program header
	assert(h.e_shnum >= 2);
	ph = readEphdr(fd, h, 0);
	assert(ph.p_filesz == sizeof(btcode));

	for(int i = 0; i < h.e_shnum; ++i)
		if((shsym = readEshdr(fd, h, i)).sh_type == SHT_SYMTAB)
			goto found;
	assert(0 && "Can't find shymbol table\n");

found:
	shstr = readEshdr(fd, h, shsym.sh_link);
	assert(shstr.sh_type == SHT_STRTAB);
	assert((tbbeg = calloc(1, shstr.sh_size + shsym.sh_size))!=NULL);
	tbstr = readsymtab(fd, shstr, tbbeg);
	tbsym = readsymtab(fd, shsym, tbbeg+shstr.sh_size);

	for(int i = 0; i < elem(syms); ++i){
		for(int j = 0; j < shsym.sh_size/sizeof(*tbsym); ++j)
			if(streq(syms[i].name, tbstr + tbsym[j].st_name)){
				syms[i].addr = tbsym[j].st_value;
				break;
			}
		assert(syms[i].addr != 0&& "Can't find symbol in symtab\n");
	}

	readEcode(fd, h, ph, btcode);
	free(tbbeg);
	close(fd);
}

static void
symset(char *name, u32 val)
{
	for(int i = 0; i < elem(syms); ++i){
		if(streq(name, syms[i].name)){
			syms[i].val = val;
			return;
		}
	}
	assert(0 && "symbol not exist");
}

static uint
tou32(u8 *str, int i)
{
	uint x = 0;

	do{
		x = x << 8;
		x |= str[--i];
	}while(i > 0);
	return x;
}

static u16
exponent(u16 x)
{
	switch(x){
	case 1: return 0;
	case 2: return 1;
	case 4: return 2;
	case 8: return 4;
	case 16: return 8;
	case 32: return 16;
	case 512: return 9;
	case 1024: return 10;
	case 2048: return 11;
	case 4096: return 12;
	default: assert(0);
	}
}

static void
emit(u64 off)
{
	for(int i = 0; i < elem(syms); ++i){
		switch(syms[i].size){
		case 1:
			btcode[syms[i].addr] = syms[i].byte;
			break;
		case 2:
			*(u16*)&btcode[syms[i].addr] = syms[i].low;
			break;
		case 4:
			*(u32*)&btcode[syms[i].addr] = syms[i].val;
			break;
		default:
			assert(0);
		}
	}

	assert(pwrite(dsk.fd, btcode+0x3c, sizeof(btcode)-0x3c, off+0x3c) ==
			sizeof(btcode)-0x3c);
}

static void
setbootparam(int ino, int off, int sec)
{
	u8 buf[512];
	u16	rdsecsz, dasec;

	assert(sizeof(buf) == pread(dsk.fd, buf, sizeof(buf), off));

	u32 bps = tou32(buf+0x0b, 2);
	u32 spc = tou32(buf+0x0d, 1);
	u32 res = tou32(buf+0x0e, 2);
	u32 nfat = tou32(buf+0x10, 1);
	u32 nrde = tou32(buf+0x11, 2);
//	u32 nsec = tou32(buf+0x13, 2);
	u32 nspf = tou32(buf+0x16, 2); // numberof sector per fat16
	rdsecsz = (nrde*32 + (bps - 1))/bps;
	dasec = res + (nspf*nfat) + rdsecsz;
	assert(0xaa55 == tou32(buf+0x1fe, 2));

	symset("dst_seg_off", bps>>4);
	symset("start_cluster", ino);
	symset("spc_sqrt", exponent(spc));
	symset("clu_off", dasec - 2 * spc);
	symset("part_offset", sec);
	symset("shift_magic", exponent(bps)-3);
	symset("and_magic", bps-1);
}

static void
install(void)
{
	char *args[] = {shpath, devpath, NULL};
	pid_t pid;
	u64 beg;

	switch(pid = fork()){
	default:
		assert(wait(NULL) == pid);
		sync();
		break;	
	case 0:
		assert(execvp(args[0], args) != -1);
	case -1:
		assert(0);
	}
	dsk = diskopen(devpath, O_RDWR);
	for(int i = 0; i < elem(dsk.parts); ++i)
		if(dsk.parts[i].type == DOSPTYP_FAT16 && dsk.parts[i].flag == DOSACTIVE){
			btpart = i;
			goto found;
		}
	assert(0);

found:
	beg = dsk.parts[btpart].beg * dsk.bps;
	setbootparam(4, beg, dsk.parts[btpart].beg);
	emit(beg);
	close(dsk.fd);
}

int
main(int argc, char *argv[])
{
	if(argc < 2 || argc > 3)
		usage();
	devpath = argv[1];
	loadelf(stage1);
	install();
	return 0;
}
