LINKADDR = 0x40120
LOADADDR = 0x40000
BOOTMAGIC = 0xc001d00d
BOOTREL = 0x60000

CC=cc
CFLAGS=-m32 -Wall -Werror -march=i386 -O0  -fno-stack-protector -ffreestanding -fno-builtin
#CFLAGS=-m32  -march=i386 -O0  -fno-stack-protector -ffreestanding -fno-builtin
CPPFLAGS=-nostdinc
LD=ld 
AS=as
ASFLAGS=-Wa,-O0,--32
LDFLAGS=-melf_i386 -O0 -nostdlib -x -N -Bstatic -znorelro -no-pie 
