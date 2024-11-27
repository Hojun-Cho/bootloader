PROG=installboot
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

CFLAGS = -g -Wall -Werror
CPPFLAGS = -I $(IDIR)

${PROG}: $(OBJS)
	$(CC) $(CFLAGS) -o $(BDIR)/$(PROG) $^
	cp filecopy.sh $(BDIR)

$(OBJS): ../dat.h

clean:
	rm -rf $(PROG) *.o
