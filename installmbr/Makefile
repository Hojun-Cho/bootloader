PROG=installmbr
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

CFLAGS = -g
CPPFLAGS = -I $(IDIR)

${PROG}: $(OBJS)
	@rm -f $(PROG)
	$(CC) $(CFLAGS) -o $(BDIR)/$(PROG) $^ 

$(OBJS): ../dat.h

clean:
	rm -rf $(PROG) $(OBJS)
