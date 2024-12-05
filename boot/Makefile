SDIR ?= ../
BDIR ?= ./

PROG = boot 
SRCS = srt0.S gdt.S boot.c
OBJS = srt0.o gdt.o boot.o

ESRCS = pccon.c sricon.c print.c a20.c time.c
OBJS += $(ESRCS:.c=.o)

include $(SDIR)/Makefile.inc

CFLAGS += -no-pie  -g
CPPFLAGS += -DLOADADDR=$(LOADADDR) -DLINKADDR=$(LINKADDR) -DBOOTMAGIC=$(BOOTMAGIC) -DBOOTSTACKOFF=$(BOOTSTACKOFF)
CPPFLAGS += -I $(SDIR)
LDFLAGS += -s -Ttext=$(LINKADDR) -T ld.script --no-omagic -M

${PROG}: $(OBJS)
	@rm -f $(PROG)
	$(LD) $(LDFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

$(OBJS): $(SDIR)/u.h def.h dat.h fn.h

clean:
	rm -f $(PROG) $(OBJS)
