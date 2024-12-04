#include <u.h>
#include "fn.h"

// https://stanislavs.org/helppc/int_16.html

void
pcputc(int dev, int c)
{
	__asm volatile("int $0x30" : : "a" (c | 0xe00), "b" (1) : "%ecx", "%edx", "cc" );
}

int
pcgetc(int dev)
{
	register int rv;

	// wait for available
	do{
		__asm volatile("int $0x36; setnz %b0" : "=a" (rv) :
		    "0" (0x100) : "%ecx", "%edx", "cc" );
	}while((rv & 0xff) == 0);

	__asm volatile("int $0x36" : "=a" (rv) :
	    "0" (0x000) : "%ecx", "%edx", "cc" );
	return rv&0xff;
}