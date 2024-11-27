PROG=mbr
SRCS=$(wildcard *.S)
OBJS=$(SRCS:.S=.o)

LDFLAGS = -Ttext 0 -e 0
CPPFLAGS = -I $(IDIR)

${PROG}: $(OBJS)
	@rm -f $(PROG)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)
	objcopy -O binary mbr mbr.new
	dd if=mbr.new of=mbr bs=512 count=1
	rm -rf mbr.new
	mv $(PROG) $(BDIR)

$(OBJS): ../def.h

clean:
	rm -f *.o *.bin $(PROG)
