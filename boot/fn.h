#define MAX(x, y) ((x)>(y)?(x):(y))
#define MIN(x, y) ((x)<(y)?(x):(y))

// console.c
void cominit(ConDev *d);
void pcinit(ConDev *d);
int pcprobe(ConDev *d);
int pcgetc(int dev);
void pcputc(int dev, int c);
int comprobe(ConDev *d);
int comgetc(int dev);
void computc(int dev, int c);

// fmt.c
int fmtinstall(int c, Op (*f)(Op));
char *doprint(char *p, char *ep, char *fmt, va_list ap);
int strlen(char *s);

// print.c
void putchar(int c);
void putstr(char *s, int l);
int getchar(void);
int print(char *fmt, ...);
int snprint(char *buf, int len, char *fmt, ...);

// a20.c
void a20up(void);

// mem.c
void memprobe(void);

// time.c
long getsecs(void);

static __inline int
major(int x)
{
	return (x >> 8) & 0xff;
}

static __inline int
minor(int x)
{
	return (x & 0xff) | ((x & 0xffff0000) >> 8);
}

static __inline int
makedev(int x, int y)
{
	return (((x & 0xff) << 8) |
		(y & 0xff) |
		((y & 0xffff00)<<8));
}

static __inline void
outb(int port, u8 data)
{
	__asm volatile("outb %0,%w1" : : "a" (data), "d" (port));	
}

static __inline u8
inb(int port)
{
	u8	data;

	__asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}