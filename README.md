# FAT16 bootloader
	x86 FAT16 bootloader
	
### How to run
	1. install image to device
		install to device
			./install.sh /dev/sda
		create image and install
			./install.sh
	2. run with qemu
		qemu-systme-i386 [device | ./build/img]

### Disk layout
	0x0000 ~ 0x0200: "mbr"
	0x8000 ~ *     : Using FAT16 filesystem
	0x8000 ~ 0x8200: "biosboot"
	    exist "boot" somewhere in FAT16 

### How to work
	mbr
		1. bios take us to 0x7C00
		2. relocate 0x7c00~0x7DFF to 0x7A00~0x7BFF
		3. load "biosboot" to 0x7C00~0x7DFF
		4. jump to 0x7c00 => biosboot
	biosboot
		1. find "boot" file in FAT16 file sysytem
		2. load "boot" to 0x40000
		2-1. 0x40000 ~ 0x401ff: ELF information
			 0x40120 ~ *      : text, data, bss section 
		3. jump to 0x40120 => boot
	boot
		1. set Global Descriptor Table
		2. enter protected mode
		2. set Interrupt Descriptor Table
		3. call boot function 