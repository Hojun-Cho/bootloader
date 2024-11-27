include ../*.inc

PROG=boot
CSRCS= $(wildcard *.c)
SSRCS= $(wildcard *.S)
OBJS = $(SSRCS:.S=.o) $(CSRCS:.c=.o)

CFLAGS+=-c -fno-pie
CPPFLAGS+=-nostdinc -DLOADADDR=$(LOADADDR) -DLINKADDR=$(LINKADDR) -DBOOTMAGIC=$(BOOTMAGIC)
LDFLAGS+= -s -T ld.script -Ttext=$(LINKADDR) --no-omagic

${PROG}: $(OBJS)
	@rm -f $(PROG)
	$(LD) $(LDFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

clean:
	rm -f $(PROG) $(PROG).bin $(OBJS)
