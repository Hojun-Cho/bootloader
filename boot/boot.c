#include <u.h>
#include "dat.h"
#include "fn.h"

void (*probe1[])(void) = {
	a20up,
};
void (*probe2[])(void) = {
};

BootProbe probes[] = {
	{"probing", probe1, elem(probe1) },
	{"disk", probe2, elem(probe2) },
};

ConDev condev = {
	.getc = pcgetc,
	.putc = pcputc,
};

// https://wiki.osdev.org/BIOS
void
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
	machdep();	
	print("\n\n===> Hello world <===\n\tBooted on disk 0x%x\n", bootdev);

	for(;;){
		char buf[8192];

		print("\nboot >> ");
		rdline(buf, sizeof buf);
		print("%s", buf);
	}
}
