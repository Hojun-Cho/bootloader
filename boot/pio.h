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
};
