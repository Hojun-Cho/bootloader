#include <u.h>
#include "dat.h"
#include "fn.h"

#define TABWIDTH 8

static void conputc(int c);
static int congetc(void);

static int pos = 0;

int
getchar(void)
{
	int c;

	c = congetc();	
	if(c == '\r')
		c = '\n';
	if((c < ' ' && c != '\n') || c == '\177')
		return c;
	putchar(c);
	return c;
}

void
putchar(int c)
{
	switch(c){
	case '\177':
		conputc('\b');
		conputc(' ');
	case '\b':
		conputc('\b');
		if(pos > 0)
			--pos;
		break;
	case '\t':
		do{
			conputc(' ');
		}while(++pos % TABWIDTH);
		break;
	case '\n':
	case '\r':
		conputc(c);
		pos = 0;
		break;
	default:
		conputc(c);
		++pos;
		break;
	}	
}

static void
conputc(int c)
{
	if(c){
		condev.putc(condev.dev, c);
		if(c == '\n')
			condev.putc(condev.dev, '\r');
	}	
}

static int 
congetc(void)
{
	return condev.getc(condev.dev);
}
