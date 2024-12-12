#include <u.h>
#include "dat.h"
#include "fn.h"

#define BUFSIZE 1024

static void conputc(int);
static int congetc(void);

char*
doprint(char *p, char *ep, char *fmt, va_list ap)
{
	Op op = { .p = p, .ep = ep, .ap = ap };
	return dofmt(&op, fmt);
}

int
print(char *fmt, ...)
{
	char buf[BUFSIZE], *p;
	va_list args;

	va_start(args, fmt);
	p = doprint(buf, buf+sizeof(buf), fmt, args);
	va_end(args);
	putstr(buf, p-buf);
	return p-buf;
}

int
snprint(char *buf, int len, char *fmt, ...)
{
	char *p;
	va_list args;

	va_start(args, fmt);
	p = doprint(buf, buf+len, fmt, args);
	va_end(args);
	return p-buf;
}

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
putstr(char *s, int l)
{
	while(l--)
		putchar(*s++);
}

void
putchar(int c)
{
	switch(c){
	case '\177':
		conputc('\b');
		conputc(' ');
		break;
	case '\b':
		conputc('\b');
		break;
	case '\t':
		for(int i = 0; i < TABWIDTH; ++i)
			conputc(' ');
		break;
	case '\n':
	case '\r':
		conputc(c);
		break;
	default:
		conputc(c);
		break;
	}	
}

static void
conputc(int c)
{
	if(c){
		con->putc(con->dev, c);
		if(c == '\n')
			con->putc(con->dev, '\r');
	}	
}

static int 
congetc(void)
{
	return con->getc(con->dev);
}
