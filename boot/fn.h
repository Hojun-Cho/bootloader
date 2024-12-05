
// console.c
int pcgetc(int dev);
void pcputc(int dev, int c);

// print.c
void putchar(int c);
int getchar(void);
void print(const char *fmt, ...);

// a20.c
void a20up(void);

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