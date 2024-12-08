#include <u.h>
#include "dat.h"
#include "fn.h"

#define IOMEM_BEGIN 0x0A0000
#define IOMEM_END   0x100000

enum{
	MAP_END  = 0x00,
	MAP_FREE = 0x01,
	MAP_RES = 0x02,
	MAP_ACPI_RECLAM = 0x03,
	MAP_ACPI_NVS = 0x04,
};

static BIOSmmap*
int15_E820(BIOSmmap *m)
{
	int rc, sig, off = 0;
	do{
		BIOSreg.es = ((uint)(m) >> 4);
		__asm volatile("int $0x35; setc %b1"
			: "=a" (sig), "=d" (rc), "=b" (off)
			: "0" (0xE820), "1" (0x534d4150), "b" (off),
			  "c" (sizeof(*m)), "D" (((uint)m) & 0xf)
			: "cc", "memory");

		off = BIOSreg.bx;
		if(rc &0xff || sig !=0x534d4150){
			break;
		if(m->type == 0)
			m->type = MAP_RES;
		}
		m++;
	}while(off);
	return m;
}

static void
dumpmem(BIOSmmap *m)
{
	ulong tot = 0;

	for(BIOSmmap *p=m; p->type != MAP_END; ++p){
		print("MEM %u type %u size %lldKB at 0x%llx\n",
			p-m, p->type, p->size/1024, p->addr);
		if(p->type == MAP_FREE)
			tot += p->size/1024;
	}
	print("RAM low:%dKB high:%dKB\n", cnvmem, extmem);	
	print("Total free memory: %dKB\n", tot);
}

static int
isa20done(void)
{
	register char *a = (char *)0x100000;
	register char *b = (char *)0x000000;

	return *a != *b;
}

void
memprobe(void)
{
	BIOSmmap *m;

	cnvmem = extmem = 0;
	m = int15_E820(biosmmap);
	m->type = MAP_END;
	for(m = biosmmap; m->type != MAP_END; ++m){
		if(m->type != MAP_FREE || m->size <= 0)
			continue;	
		if(m->addr < IOMEM_BEGIN)
			cnvmem = MAX(cnvmem, m->addr + m->size)/1024;
		if(m->addr >= IOMEM_END)
			extmem += m->size/1024;
	}
	dumpmem(biosmmap);
	print("A20:%d", isa20done());
}

