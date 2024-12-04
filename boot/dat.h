// IO Port	Access Type	Purpose
// 0x60	Read/Write	Data Port
// 0x64	Read	Status Register
// 0x64	Write	Command Register

struct BIOSreg{
	u32 ax; 
	u32 cx; 
	u32 dx; 
	u32 bx; 
	u32 bp; 
	u32 si; 
	u32 di; 
	u32 ds; 
	u32 es; 
}__attribute__((packed));


typedef struct{
	char *name;
	void (**probes)(void);
	int cnt;
}BootProbe;

typedef struct{
	int (*getc)(int);
	void (*putc)(int, int);
	int	dev;
}ConDev;

// gdt.S
extern volatile struct BIOSreg BIOSreg;

// boot.c
extern void (*probe1[])(void);
extern void (*probe2[])(void);
extern BootProbe probes[];
extern ConDev condev;