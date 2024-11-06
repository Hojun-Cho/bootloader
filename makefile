AS=as -march=i386 -32
CC=cc -m32
LD=ld -melf_i386

all:
	$(CC) -c boot.S -o boot.o -Wa,--gstabs -nostdinc  -I./
	$(LD) --oformat binary -N -Ttext 0 -x -N -s -Bstatic -e start -znorelro -o boot.bin boot.o

clean:
	rm -f *.o boot.bin
