SDIR ?= ../
BDIR ?= ./

PROG = installboot
SRCS = installboot.c
OBJS = installboot.o

CFLAGS = -g -Wall -Werror
CPPFLAGS = -I $(SDIR)

${PROG}: $(OBJS)
	$(CC) $(CFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

$(OBJS): $(SDIR)/u.h $(SDIR)/btld_conf.h $(SDIR)/btld_disk.h elf32.h

clean:
	rm -rf $(PROG) $(OBJS)
