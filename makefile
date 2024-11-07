AS=as -march=i386 -32
CC=cc -m32
LD=ld -melf_i386
LDFLAGS=-nostdlib -Ttext 0 -x -N -s -Bstatic -e 0 -znorelro

all:
	$(CC) -O -nostdinc -nostdlib  -c boot.S -o boot.o 
	$(LD)  $(LDFLAGS) --oformat binary -o boot.bin boot.o
	$(CC) -O -c entry.S -o entry.o -nostdinc
	$(CC) -O -c kernel.c -o kernel.o  -nostdinc
	$(LD) -Ttext 0 -s -e main -znorelro -o kernel.bin entry.o kernel.o 
	objcopy -R .note -R .comment -S kernel.bin kernel.bin

clean:
	rm -f *.o boot.bin kernel.bin 
