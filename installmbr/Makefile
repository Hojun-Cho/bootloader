SDIR ?= ../
BDIR ?= ./

PROG = installmbr
SRCS = installmbr.c
OBJS = installmbr.o

CFLAGS = -g -Wall -Werror
CPPFLAGS = -I $(SDIR)

${PROG}: $(OBJS)
	$(CC) $(CFLAGS) -o $(BDIR)/$(PROG) $(OBJS)

$(OBJS): $(SDIR)/u.h $(SDIR)/btld_conf.h $(SDIR)/btld_disk.h

clean:
	rm -f $(PROG) $(OBJS)
