#include <u.h>
#include "dat.h"
#include "fn.h"

#define MAXCON 30
#define IDIGIT 30

enum{
	FLONG = (1<<0),
	FSHORT = (1<<2),
	FUNSIGN = (1<<3),
};

static void put(Op*, int);
static int noconv(Op*);
static int cconv(Op*);
static int dconv(Op*);
static int hconv(Op*);
static int lconv(Op*);
static int sconv(Op*);
static int uconv(Op*);
static int xconv(Op*);
static int Xconv(Op*);
static int percent(Op*);

static int (*fmtconv[MAXCON])(Op*) = {
	noconv,
	cconv, dconv, hconv, lconv,
	sconv, uconv, xconv, Xconv,
	percent,
};

int fmtindex[128] = {
	['c'] = 1,
	['d'] = 2,
	['h'] = 3,
	['l'] = 4,
	['s'] = 5,
	['u'] = 6,
	['x'] = 7,
	['X'] = 8,
	['%'] = 9,
};
int convcnt = 11;

int
strlen(char *s)
{
	char *p = s;

	while(*p)
		++p;
	return p-s;
}

int
fmtinstall(int c, int (*f)(Op*))
{
	c &= 0177;
	if(fmtindex[c] == 0){
		if(convcnt + 1 >= MAXCON)
			return -1;
		fmtindex[c] = convcnt++;
	}
	fmtconv[fmtindex[c]] = f;
	return 0;
}

char*
dofmt(Op *op, char *fmt)
{
	int sf1, c;

	while(1){
		c = *fmt++;
		if(c != '%'){
			if(c == 0){
				if(op->p < op->ep)
					*op->p = 0;
				return op->p;
			}
			put(op, c);
			continue;
		}
		op->f2 = -1;
		op->f1 = op->f3 = 0;
		op->padch = sf1 = 0;
		c = *fmt++;
		if(c == '-'){
			sf1 = 1;
			c = *fmt++;
		}else if(c == '0' || c == ' '){
			op->padch = c;
			c = *fmt++;
		}
		while(c >= '0' && c <= '9'){
			op->f1 = op->f1*10 + c-'0';
			c = *fmt++;
		}
		if(sf1)
			op->f1 = -op->f1;
		if(c != '.')
			goto conv;
		c = *fmt++;
		while(c >= '0' && c <= '9'){
			if(op->f2 < 0)
				op->f2 = 0;
			op->f2 = op->f2*10 + c-'0';
			c = *fmt++;
		}
conv:
		if(c == 0)
			fmt -= 1;
		c = (*fmtconv[fmtindex[c&0177]])(op);
		if(c < 0){
			op->f3 |= -c;
			c = *fmt++;
			goto conv;
		}
	}
}

static void 
strconv(char *s, Op *op, int f1, int f2)
{
	int n, c;
	char *p;

	n = strlen(s);
	if(f1 >= 0)
		while(n < f1){
			put(op, op->padch);
			n += 1;
		}
	for(p=s; (c = *p++);)
		if(f2){
			put(op, c);
			f2 -= 1;
		}
	if(f1 < 0){
		f1 = -f1;
		while(n < f1){
			put(op, ' ');
			n += 1;
		}	
	}
}

static void 
numconv(Op *op, int base)
{
	char b[IDIGIT];
	int i,f,n;
	long v;
	short h;

	i = IDIGIT-1;
	f = 0;
	b[i] = 0;
	switch(op->f3 & (FLONG|FSHORT|FUNSIGN)){
	case FLONG:
		v = va_arg(op->ap, long);
		break;
	case FUNSIGN|FLONG:
		v = va_arg(op->ap, ulong);
		break;
	case FSHORT:
		v = h = va_arg(op->ap, short);
		break;
	case FUNSIGN|FSHORT:
		h = va_arg(op->ap, ushort);
		v = (ushort)h;
		break;
	case FUNSIGN:
		v = va_arg(op->ap, unsigned);
		break;
	default:
		v = va_arg(op->ap, int);
		break;
	}
	if((op->f3 & FUNSIGN) && v < 0){
		v = -v;
		f = 1;
	}
	while(--i){
		n = v % base;
		n += '0';
		if(n > '9'){
			n += 'a' - ('9'+1);
			if(op->ucase)
				n += 'A'-'a';
		}
		b[i] = n;
		v = (ulong)v / base;
		if(i < 2)
			break;
		if(op->f2 >= 0 && i >= IDIGIT - op->f2)
			continue;
		if(v <= 0)
			break;
	}
	if(f)
		b[--i] = '-';
	strconv(b+i, op, op->f1, -1);
}

static int
noconv(Op *op)
{
	strconv("***ERROR: noconv***", op, 0, -1);
	return 0;
}

static int
cconv(Op *op)
{
	char b[2];

	b[0] = va_arg(op->ap, char);
	b[1] = 0;
	strconv(b, op, op->f1, -1);
	return 0;
}

static int
dconv(Op *op)
{
	numconv(op, 10);
	return 0;
}

static int
hconv(Op *op)
{
	return -FSHORT;
}

static int
lconv(Op *op)
{
	return -FLONG;
}

static int
uconv(Op *op)
{
	return -FUNSIGN;
}

static int
sconv(Op *op)
{
	char *p;

	p = va_arg(op->ap, char*);
	strconv(p?p:"<nil>", op, op->f1, op->f2);
	return 0; 
}

static int
xconv(Op *op)
{
	numconv(op, 16);
	return 0;
}

static int
Xconv(Op *op)
{
	op->ucase = 1;
	numconv(op, 16);
	op->ucase = 0;
	return 0;
}

static int
percent(Op *op)
{
	put(op, '%');
	return 0;
}

static void
put(Op *op, int c)
{
	static int pos;
	int opos;

	if(c == 0)
		return;
	if(c == '\t'){
		opos = pos;
		pos = (opos+TABWIDTH) & (~(TABWIDTH-1));
		while(opos++ < pos && op->p < op->ep)
			*op->p++ = ' ';
		return;
	}	
	if(op->p < op->ep){
		*op->p++ = c;
		pos++;
	}
	if(c == '\n')
		pos = 0;
}