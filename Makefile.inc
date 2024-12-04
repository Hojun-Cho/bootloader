
LINKADDR = 0x40120
LOADADDR = 0x40000
BOOTMAGIC = 0xc001d00d
BOOTSEG	= 0x7c0
BOOTRELOCSEG = 0x7a0
MBRSTACKOFF = 0xfffc
BOOTSTACKOFF = 0xfffc

CC=cc
CFLAGS= -m32 -march=i386 -Wall -Werror -march=i386 -O0  -fno-stack-protector -ffreestanding -fno-builtin -fno-builtin-function -fno-gnu-unique
CPPFLAGS=-nostdinc
ASFLAGS=-Wa,--32,-O0
LDFLAGS=-O0 -melf_i386 -nostdlib -x -N -Bstatic -znorelro -no-pie 
