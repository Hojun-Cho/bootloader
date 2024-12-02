#define elem(x) (sizeof(x)/sizeof((x)[0]))

// IO Port	Access Type	Purpose
// 0x60	Read/Write	Data Port
// 0x64	Read	Status Register
// 0x64	Write	Command Register

typedef struct{
	char *name;
	void (**probes)(void);
	int cnt;
}BootProbe;

// for high memory
static void
a20up(void)
{
	struct{
		int dport, sport, cport;
		u8 a20;
		u8 dib, ib, wout;
	} i8042 = {
		.dport = 0x60, // data port
		.sport = 0x64, // status register port
		.cport = 0x64, // command register port
		.a20 = 0xdf, // enable a20 line value
		.dib = 0x01, // kbd data in buffer
		.ib = 0x02, // kbd input buffer
		.wout = 0xd1, // write output port value
	};

	while(inb(i8042.sport) & i8042.ib)
		;
	while(inb(i8042.sport) & i8042.dib)
		inb(i8042.dport);

	outb(i8042.cport, i8042.wout);
	while(inb(i8042.sport) & i8042.ib)
		;

	outb(i8042.dport, i8042.a20);
	while(inb(i8042.sport) & i8042.ib)
		;
	while(inb(i8042.sport) & i8042.dib)	
		inb(i8042.dport);
}

static void (*probe1[])(void) = {
	a20up,
};
static void (*probe2[])(void) = {
};

static BootProbe probes[] = {
	{"probing", probe1, elem(probe1) },
	{"disk", probe2, elem(probe2) },
};

static void
machdep(void)
{
	for(int i = 0; i < elem(probes); ++i){
		BootProbe bp = probes[i];
		for(int j = 0; j < bp.cnt; ++j)
			bp.probes[j]();
	}	
}
