#include <u.h>
#include "dat.h"
#include "fn.h"

// https://stanislavs.org/helppc/int_1a.html
enum{
	GET_TIME = 2<<8,
 	GET_DATE = 4<<8,
};

static int
bcd2int(u8 c)
{
	return ((c & 0xf0)/8*5+(c & 0x0f));	
}

static int
isleap(int y)
{
	return ((y % 4 == 0 && y % 100) || (y % 400 == 0));
}

static long 
date2day(int y, u8 m, u8 d)
{
	long t;	
	static const u16 m2d[] = {(u16)-1,
		0,31,59,90,120,151,181,212,243,273,304,334,365};

	t = (y - 1970) * 365 + m2d[m] + d - 1;
	y -= (m <= 2);
	for(; y >= 1970; --y){
		if(isleap(y))
			t += 1;
	}
	return t;
}

// bios long data
static void 
int1a(int f, u8 *b)
{
	__asm volatile("int $0x3a \n\t"
		"setc %b0\n\t"
		"movb %%ch, 0(%2)\n\t"
		"movb %%cl, 1(%2)\n\t"
		"movb %%dh, 2(%2)\n\t"
		"movb %%dl, 3(%2)\n\t"
		: "=a" (f)
		: "0" (f), "r" (b)
		: "%ecx", "%edx", "cc");
	b[0] = bcd2int(b[0]);
	b[1] = bcd2int(b[1]);
	b[2] = bcd2int(b[2]);
	b[3] = bcd2int(b[3]);
}

long
getsecs(void)
{
	u8 t[4], d[4];
	long s;

	int1a(GET_TIME, t);
	int1a(GET_DATE, d);
	s = (date2day(d[0]*100+d[1], d[2], d[3]) * 24 + t[0]) * 60
		+ (t[1] * 60) + t[2];
	return s;	
}
