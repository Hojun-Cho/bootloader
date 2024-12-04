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
puts(char *s)
{
	for(;*s;++s)	
		putchar(*s);
}

void
boot(int bootdev)
{
	char buf[8192];

	machdep();	
	putchar('\n');
	for(;;){
		int i=0, c=0;

		puts("\nboot >> ");
		do{
			c = getchar();
			buf[i++] = c;
		}while(c != '\n' && i < sizeof(buf) - 1);
		buf[i-1] = 0;
		puts(buf);
	}
}
