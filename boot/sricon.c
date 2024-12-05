// Serial console
#include <u.h>
#include "dat.h"
#include "fn.h"

/*
 * 8250 UART and compatibles.
 */
enum {
	Uart0		= 0x3F8,	/* COM1 */
	Uart0IRQ	= 4,
	Uart1		= 0x2F8,	/* COM2 */
	Uart1IRQ 	= 3,
	UartFREQ 	= 1843200,	/* 1.8432 MHz */
};

/* I/O ports */
enum {
	Rbr = 0, /* Receiver Buffer (RO) */
	Thr = 0, /* Transmitter Holding (WO) */
	Ier = 1, /* Interrupt Enable */
	Iir = 2, /* Interrupt Identification (RO) */
	Fcr = 2, /* FIFO Control (WO) */
	Lcr = 3, /* Line Control */
	Mcr = 4, /* Modem Control */
	Lsr = 5, /* Line Status */
	Msr = 6, /* Modem Status */
	Scr = 7, /* Scratch Pad */
	Dll = 0, /* Divisor Latch Low */
	Dlh = 1, /* Divisor Latch High */
};

/* Interrupt Enable Registe */
enum {			/* Ier */
	Erda	= 0x01,	/* Enable Received Data Available */
	Ethre	= 0x02,	/* Enable Thr Empty */
	Erls	= 0x04,	/* Enable Receiver Line Status */
	Ems	= 0x08,	/* Enable Modem Status */
};

/* Interrupt Identification Register */
enum {			/* Iir */
	Ims	= 0x00,	/* Ms interrupt */
	Ip	= 0x01,	/* Interrupt Pending (not) */
	Ithre	= 0x02,	/* Thr Empty */
	Irda	= 0x04,	/* Received Data Available */
	Irls	= 0x06,	/* Receiver Line Status */
	Ictoi	= 0x0C,	/* Character Time-out Indication */
	IirMASK	= 0x3F,
	Ifena	= 0xC0,	/* FIFOs enabled */
};

/* FIFO Control Registe */
enum {			/* Fcr */
	FIFOena	= 0x01,	/* FIFO enable */
	FIFOrclr= 0x02,	/* clear Rx FIFO */
	FIFOtclr= 0x04,	/* clear Tx FIFO */
	FIFO1	= 0x00,	/* Rx FIFO trigger level 1 byte */
	FIFO4	= 0x40,	/* 4 bytes */
	FIFO8	= 0x80,	/* 8 bytes */
	FIFO14	= 0xC0,	/* 14 bytes */
};

/* Line Control Registe */
enum {			/* Lcr */
	Wls5	= 0x00,	/* Word Length Select 5 bits/byte */
	Wls6	= 0x01,	/* 6 bits/byte */
	Wls7	= 0x02,	/* 7 bits/byte */
	Wls8	= 0x03,	/* 8 bits/byte */
	WlsMASK	= 0x03,
	Stb	= 0x04,	/* 2 stop bits */
	Pen	= 0x08,	/* Parity Enable */
	Eps	= 0x10,	/* Even Parity Select */
	Stp	= 0x20,	/* Stick Parity */
	Brk	= 0x40,	/* Break */
	Dlab	= 0x80,	/* Divisor Latch Access Bit */
};

/* Modem Control Register */
enum {			/* Mcr */
	Dtr	= 0x01,	/* Data Terminal Ready */
	Rts	= 0x02,	/* Ready To Send */
	Out1	= 0x04,	/* no longer in use */
	Ie	= 0x08,	/* IRQ Enable */
	Dm	= 0x10,	/* Diagnostic Mode loopback */
};

/* Line Status Register */
enum {			/* Lsr */
	Dr	= 0x01,	/* Data Ready */
	Oe	= 0x02,	/* Overrun Error */
	Pe	= 0x04,	/* Parity Error */
	Fe	= 0x08,	/* Framing Error */
	Bi	= 0x10,	/* Break Interrupt */
	Thre	= 0x20,	/* Thr Empty */
	Temt	= 0x40,	/* Tramsmitter Empty */
	FIFOerr	= 0x80,	/* error in receiver FIFO */
};

/* Modem Status Register */
enum {			/* Msr */
	Dcts	= 0x01,	/* Delta Cts */
	Ddsr	= 0x02,	/* Delta Dsr */
	Teri	= 0x04,	/* Trailing Edge of Ri */
	Ddcd	= 0x08,	/* Delta Dcd */
	Cts	= 0x10,	/* Clear To Send */
	Dsr	= 0x20,	/* Data Set Ready */
	Ri	= 0x40,	/* Ring Indicator */
	Dcd	= 0x80,	/* Data Set Ready */
};

static int setbaud(int dev, int baud);

// probe serial console
// https://stanislavs.org/helppc/int_11.html
// https://wiki.osdev.org/Serial_Ports
int
comprobe(ConDev *d)
{
	register int n;

	__asm volatile("int $0x31" : "=a" (n):: "%ecx", "%edx", "cc");
	n >>= 9;
	n &= 0b111;
	if(n == 0)
		return 0;
	for(int i = 0; i < n; ++i)
		print("Found com %d\n", i);
	d->pri = 0;	
	d->dev = makedev(8, 0);
	return n;
}

// init serial consone
void
cominit(ConDev *d)
{
	long t = getsecs() + 1;

	outb(Uart0+Ier, 0);
	setbaud(d->dev, 9600);
	outb(Uart0+Mcr, Dtr|Rts);
	outb(Uart0+Fcr, FIFOena|FIFOrclr|FIFOtclr|FIFO1);
	inb(Uart0+Iir); // check fifo

	// wait
	for(ulong i = 1; ((i % 1000)!=0) && getsecs() < t; ++i)
		;
	// drain input buffer
	while(inb(Uart0+Lsr) & Dr)
		inb(Uart0+Rbr);
}

static int
setbaud(int dev, int baud)
{
	int port = Uart0;
	ulong div;

	if(baud <= 0)
		return -1;
	/* Divisor = 115200/baud */
	div = (UartFREQ+8*baud-1)/(16*baud);	
	outb(port+Lcr, Dlab);
	outb(port+Dlh, div>>8);
	outb(port+Dll, div);
	outb(port+Lcr, Wls8);
	return 0;
}

int
comgetc(int dev)
{
	while((inb(Uart0+Lsr) & Dr) == 0)
		;
	return inb(Uart0+Rbr) & 0xff;
}

void
computc(int dev, int c)
{
	while((inb(Uart0+Lsr) & Thre) == 0)
		;
	outb(Uart0+Thr, c);	
}