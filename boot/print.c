#include <u.h>
#include "dat.h"
#include "fn.h"

#define TABWIDTH 4

static void conputc(int c);
static int congetc(void);
static void doprint(void (*put)(int), const char *fmt, va_list ap);
static void putint(void (*put)(int), int n, const char *sym, int base);
static int pos = 0;

void
print(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);	
	doprint(putchar, fmt, args);
	va_end(args);
}

static void
doprint(void (*put)(int), const char *s, va_list ap)
{
	while(*s){
		if(*s != '%'){
			put(*s++);
			continue;
		}
		++s;
		switch(*s){
		case 'd':
			putint(put, va_arg(ap,int), "0123456789", 10);		
			break;
		case 'x':
			putint(put, va_arg(ap,int), "0123456789abcdef", 16);
			break;
		case 'X':
			putint(put, va_arg(ap,int), "0123456789ABCDEF", 16);
			break;
		case 's':
			for(char *p=va_arg(ap,char*); *p; ++p)
				put(*p);
			break;
		default:
			return;
		}	
		++s;
	}
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

static void
putint(void (*put)(int), int n, const char *sym, int base)
{
	int i;
	char buf[16];

	i = 0;
	do{
		buf[i++] = n%base;
		n /= base;
	}while(n);
	do{
		put((int)sym[(int)buf[--i]]);
	}while(i);
}