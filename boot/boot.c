#include <u.h>
#include "dat.h"
#include "fn.h"

static void coninit(void);
static void machdep(void);

void (*probe1[])(void) = {
	a20up, coninit, memprobe,
	cpuidprobe,
};
void (*probe2[])(void) = {
};

BootProbe probes[] = {
	{"probing", probe1, elem(probe1) },
	{"disk", probe2, elem(probe2) },
};

ConDev contab[CON_END] = {
	{
		.probe = pcprobe,
		.init = pcinit,
		.getc = pcgetc,
		.putc = pcputc
	},
	{
		// serial console
		.probe = comprobe,
		.getc = comgetc,
		.putc = computc,
		.init = cominit,
	},
};

ConDev *con = &contab[0];
BIOSmmap biosmmap[64];
uint cnvmem, extmem;
int debug = BOOT_DEBUG;

static void
coninit(void)
{
	ConDev *p = con;

	for(int i = 0; i < elem(contab); ++i){
		ConDev *t = &contab[i];
		if(t->probe(t) != 0 && t->pri > p->pri)
			p = t;
	}
	if(p){
		p->init(p);
		con = p;
	}
}

// https://wiki.osdev.org/BIOS
static void
machdep(void)
{
	for(int i = 0; i < elem(probes); ++i){
		BootProbe bp = probes[i];
		for(int j = 0; j < bp.cnt; ++j)
			bp.probes[j]();
	}	
}

static void
rdline(char *buf, int n)
{
	char *p = buf;
	char *e = buf + n - 1;

	while(p < e){
		int c = getchar();
		switch(c){
		case '\177':
		case '\b':
			if(p > buf){
				putchar('\177');
				*--p = 0;
			}
			break;
		case '\n':
		case '\r':
			goto done;
		default:
			*p++ = c;
			break;
		}
	}
	putchar('\n');
done:
	*p = 0;
}

void
boot(int bootdev)
{
	BIOSdisk d = {0,};

	fmtinstall('D', fmtdisk);
	print("\n===> Hello world <===\n\t"
		"Booted on disk 0x%x debug:%d\n\t", bootdev, debug);
	machdep();	

	bdiskget(bootdev, &d);
	print("%D\n%D", d,d);

	for(;;){
		char buf[8192];

		print("\nboot >> ");
		rdline(buf, sizeof buf);
		print("%s", buf);
	}
}
