#include <u.h>
#include "dat.h"
#include "fn.h"

#define I8042_DPORT 0x60
#define I8042_SPORT 0x64
#define I8042_CPORT 0x64
#define I8042_IN_FULL 0x02
#define I8042_DATA_FULL 0x01
#define I8042_OUT 0xd1
#define I8042_A20_UP 0xdf // 0xD0 | a20 | clock | buffer full IRQ1
// https://wiki.osdev.org/%228042%22_PS/2_Controller#Initialising_the_PS/2_Controller

void
a20up(void)
{
	while(inb(I8042_SPORT) & I8042_IN_FULL)
		;
	while(inb(I8042_SPORT) & I8042_DATA_FULL)
		inb(I8042_DPORT);

	outb(I8042_CPORT, I8042_OUT);
	// wait until input buffer is empty
	while(inb(I8042_SPORT) & I8042_IN_FULL)
		;

	outb(I8042_DPORT, I8042_A20_UP);
	while(inb(I8042_SPORT) & I8042_IN_FULL)
		;
	while(inb(I8042_SPORT) & I8042_DATA_FULL)
		inb(I8042_DPORT);
}