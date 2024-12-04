SDIR ?= ../
BDIR ?= ./

PROG = mbr
SRCS = mbr.S
OBJS = mbr.o

include $(SDIR)/Makefile.inc

CFLAGS += -c -fno-pie
LDFLAGS += -Ttext 0 -e 0
CPPFLAGS += -DBOOTSEG=$(BOOTSEG) -DBOOTRELOCSEG=$(BOOTRELOCSEG) -DMBRSTACKOFF=$(MBRSTACKOFF)
CPPFLAGS += -I $(SDIR)/

${PROG}: $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)
	objcopy -S -O binary mbr mbr.new
	dd if=mbr.new of=$(BDIR)/mbr bs=512 count=1
	rm -f mbr.new

$(OBJS): $(SDIR)/Makefile.inc $(SDIR)/btld_conf.h

clean:
	rm -f $(OBJS) $(PROG)