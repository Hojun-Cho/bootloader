
typedef unsigned short u16;
typedef unsigned char u8;
typedef int dev_t;

#include "pio.h"
#include "conf.h"

#define VGA_WIDTH 80
#define VGA_LENGTH 25

char*
memcpy(char *a, char *b, int l)
{
	for(int i = 0; i < l; ++i)
		a[i] = b[i];
	return a;
}

int
strlen(char *s)
{
	int i = 0;
	for(; s[i]; ++i)
		;
	return i;
}

char*
itostr(int n, char *s)
{
	char buf[16];	
	char *p = buf;

	do {
		*p++ = n%16;
	}while((n/=16) != 0);

	while(p > buf){
		*s++ = "0123456789abcdef"[(int)*--p];
	}
	return s;
}

void
boot(dev_t bootdev)
{
	machdep();
	for(;;);
}
