#include <u.h>
#include "dat.h"
#include "fn.h"

// only for booting

typedef struct FreeList FreeList;
static struct FreeList{
	uint size;
	FreeList *next;
} *freed;

extern char end[];
static char *top = end;

static void
putfreed(FreeList *p)
{
	p->next = nil;
	if(freed){
		p->next = freed;
		freed = p;
	}else
		freed = p;
}

static FreeList*
getfreed(uint n)
{
	for(FreeList *p=freed,*l=freed; p; p=p->next){
		if(p->size >= n){
			l->next = p->next;
			if(p == freed)
				freed = nil;
			return p;
		}
		l = p;
	}
	return nil;
}

void*
alloc(uint n)
{
	FreeList *p;

	p = getfreed(n);
	if(p == nil){
		p = (FreeList*)top;
		p->size = n;
		p->next = nil;
		memset(p+sizeof(*p), 0, n);
		top += n + sizeof(*p);
	}
if(debug) print("alloc %x %ud\n", &p[1], n);
	return &p[1];
}

void
free(void *src)
{
	FreeList *p;

	p = (FreeList*)((char*)src-sizeof(FreeList));
if(debug) print("free %x %ud\n", src, p->size);
	putfreed(p);
}