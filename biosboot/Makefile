include ../*.inc

PROG=biosboot
SRCS=$(wildcard *.S)
OBJS=$(SRCS:.S=.o)

CFLAGS+=-c -fno-pie
LDFLAGS+=-Ttext 0 -T ld.script
CPPFLAGS+=-nostdinc -DLOADADDR=$(LOADADDR) -DLINKADDR=$(LINKADDR) -DBOOTMAGIC=$(BOOTMAGIC)
CPPFLAGS+=-I $(IDIR)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

$(OBJS): ../def.h

clean:
	rm -f $(PROG) $(OBJS)
