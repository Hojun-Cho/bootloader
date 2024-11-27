#include "pio.h"

typedef unsigned short u16;
typedef unsigned char u8;
typedef int dev_t;

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
setcursor(int x, int y)
{
	u16 pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

void
wr8(u8 c, u8 fg, u8 bg, int x, int y)
{
     u16 attrib = (bg << 4) | (fg & 0x0F);
     volatile u16 * where;
     where = (volatile u16 *)0xB8000 + (y * 80 + x);
     *where = c | (attrib << 8);
}


void
boot(dev_t bootdev)
{
	char *p;
	char s[VGA_WIDTH] = "booted on disk:0x";

	p = s+strlen(s);
	p = itostr((int)bootdev, p);
	setcursor(0,0);
	for (int i = 0; i < sizeof(s); ++i)
		wr8(s[i], 0xe8, 0x00, i, 0);
	for(int i = 1; i < VGA_LENGTH; ++i)
		for(int j = 0; j < VGA_WIDTH; ++j)
			wr8(0x00, 0x91, 0x00, j, i);
	// disable cursor
	outb(0x3D4, 0x0a);
	outb(0x3D5, 0x20);
	asm("hlt");
}
