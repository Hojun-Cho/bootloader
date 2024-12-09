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

static Op put(Op, int);
static Op noconv(Op);
static Op cconv(Op);
static Op dconv(Op);
static Op hconv(Op);
static Op lconv(Op);
static Op sconv(Op);
static Op uconv(Op);
static Op xconv(Op);
static Op Xconv(Op);
static Op percent(Op);

static Op (*fmtconv[MAXCON])(Op) = {
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
fmtinstall(int c, Op (*f)(Op))
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
doprint(char *p, char *ep, char *fmt, va_list ap)
{
	int sf1, c;
	Op o = { .p = p, .ep = ep, .ap = ap };

	while(1){
		c = *fmt++;
		if(c != '%'){
			if(c == 0){
				if(o.p < o.ep)
					*o.p = 0;
				return o.p;
			}
			o = put(o, c);
			continue;
		}
		o.f2 = -1;
		o.f1 = o.f3 = 0;
		o.padch = sf1 = 0;
		c = *fmt++;
		if(c == '-'){
			sf1 = 1;
			c = *fmt++;
		}else if(c == '0' || c == ' '){
			o.padch = c;
			c = *fmt++;
		}
		while(c >= '0' && c <= '9'){
			o.f1 = o.f1*10 + c-'0';
			c = *fmt++;
		}
		if(sf1)
			o.f1 = -o.f1;
		if(c != '.')
			goto conv;
		c = *fmt++;
		while(c >= '0' && c <= '9'){
			if(o.f2 < 0)
				o.f2 = 0;
			o.f2 = o.f2*10 + c-'0';
			c = *fmt++;
		}
conv:
		if(c == 0)
			fmt -= 1;
		o = (*fmtconv[fmtindex[c&0177]])(o);
		if(o.f3){
			c = *fmt++;
			goto conv;
		}
	}
}

static Op 
strconv(char *o, Op op, int f1, int f2)
{
	int n, c;
	char *p;

	n = strlen(o);
	if(f1 >= 0)
		while(n < f1){
			op = put(op, op.padch);
			n += 1;
		}
	for(p=o; (c = *p++);)
		if(f2){
			op = put(op, c);
			f2 -= 1;
		}
	if(f1 < 0){
		f1 = -f1;
		while(n < f1){
			op = put(op, ' ');
			n += 1;
		}	
	}
	return op;
}

static Op 
numconv(Op op, int base)
{
	char b[IDIGIT];
	int i,f,n;
	long v;
	short h;

	i = IDIGIT-1;
	f = 0;
	b[i] = 0;
	switch(op.f3 & (FLONG|FSHORT|FUNSIGN)){
	case FLONG:
		v = va_arg(op.ap, long);
		break;
	case FUNSIGN|FLONG:
		v = va_arg(op.ap, ulong);
		break;
	case FSHORT:
		v = h = va_arg(op.ap, short);
		break;
	case FUNSIGN|FSHORT:
		h = va_arg(op.ap, ushort);
		v = (ushort)h;
		break;
	case FUNSIGN:
		v = va_arg(op.ap, unsigned);
		break;
	default:
		v = va_arg(op.ap, long);
		break;
	}
	if((op.f3 & FUNSIGN) && v < 0){
		v = -v;
		f = 1;
	}
	while(--i){
		n = v % base;
		n += '0';
		if(n > '9'){
			n += 'a' - ('9'+1);
			if(op.ucase)
				n += 'A'-'a';
		}
		b[i] = n;
		v = (ulong)v / base;
		if(i < 2)
			break;
		if(op.f2 >= 0 && i >= IDIGIT - op.f2)
			continue;
		if(v <= 0)
			break;
	}
	if(f)
		b[--i] = '-';
	op.f3 = 0;
	return strconv(b+i, op, op.f1, -1);
}

static Op
noconv(Op op)
{
	return strconv("***ERROR: noconv***", op, 0, -1);
}

static Op
cconv(Op op)
{
	char b[2];

	b[0] = va_arg(op.ap, char);
	b[1] = 0;
	return strconv(b, op, op.f1, -1);
}

static Op
dconv(Op op)
{
	return numconv(op, 10);
}

static Op
hconv(Op op)
{
	op.f3 |= FSHORT;
	return op;
}

static Op
lconv(Op op)
{
	op.f3 |= FLONG;
	return op;
}

static Op
uconv(Op op)
{
	op.f3 |= FUNSIGN;
	return op;
}

static Op
sconv(Op op)
{
	char *p;

	p = va_arg(op.ap, char*);
	return strconv(p?p:"<nil>", op, op.f1, op.f2);
}

static Op
xconv(Op op)
{
	return numconv(op, 16);
}

static Op
Xconv(Op op)
{
	op.ucase = 1;
	op = numconv(op, 16);
	op.ucase = 0;
	return op;
}

static Op
percent(Op op)
{
	return put(op, '%');
}

static Op
put(Op o, int c)
{
	static int pos;
	int opos;

	if(c == 0)
		return o;
	if(c == '\t'){
		opos = pos;
		pos = (opos+TABWIDTH) & (~(TABWIDTH-1));
		while(opos++ < pos && o.p < o.ep)
			*o.p++ = ' ';
		return o;
	}	
	if(o.p < o.ep){
		*o.p++ = c;
		pos++;
	}
	if(c == '\n')
		pos = 0;
	return o;
}