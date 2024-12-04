SDIR ?= ../
BDIR ?= ./

PROG = biosboot
SRCS = biosboot.S
OBJS = biosboot.o

include $(SDIR)/Makefile.inc

CFLAGS += -c -fno-pie
LDFLAGS += -Ttext 0 -T ld.script
CPPFLAGS += -DLOADADDR=$(LOADADDR) -DLINKADDR=$(LINKADDR) -DBOOTMAGIC=$(BOOTMAGIC) -DBOOTSEG=$(BOOTSEG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

$(OBJS): $(SDIR)/Makefile.inc

clean:
	rm -f $(PROG) $(OBJS)
