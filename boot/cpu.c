#include <u.h>
#include "dat.h"
#include "fn.h"

#define EFLAG_ID	0x00200000
#define	CPUID(code, a, b, c, d)				\
	__asm volatile("cpuid"				\
	    : "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
	    : "a" (code))

void*
memset(void *dst, int v, int l)
{
	char *p = dst;
	for(int i = 0; i < l; ++i)
		*p++ = 0;
	return dst;
}

void
cpuidprobe(void)
{
	int canuse;
	union{
		struct{u32 a,b,d,c;};
		char arr[16];
	}r = {0,};	

	__asm volatile("pushfl\n\t"
		"popl %2\n\t"
		"xorl %2, %0\n\t"	/* Invert ID sotred in EFLAGS */
		"pushl %0\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"xorl %2, %0\n\t"
		: "=r" (canuse)
		: "0" (EFLAG_ID), "r" (0) /* "EFLAGS_ID" same location "canuse" */
		: "cc");
	if(canuse != EFLAG_ID){
		print("cpuid not available\n");
		return;
	}

	// print vendor
	CPUID(0x00, r.a, r.b, r.c, r.d);
	print("CPU vender: %s\n", r.arr+4);
	// Is running on Hypervisor?	
	// But nothing to do...
	memset(&r, 0,sizeof(r));
	CPUID(0x01, r.a, r.b, r.c, r.d);
	if(r.c & (1<<31)){
		CPUID(1<<30, r.a, r.b, r.c, r.d);
		print("Running on Hypervisor: %s\n", r.arr+4);
	}
}