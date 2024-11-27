#define outb(port, data) \
	(__outb(port, data)) 

static __inline void
__outb(int port, unsigned char data)
{
	__asm volatile("outb %0,%w1" : : "a" (data), "d" (port));	
}
