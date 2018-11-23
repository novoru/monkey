CFLAGS=-Wall -std=c11 -O0 -v -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

monkey: $(OBJS)
	cc -o $@ $(OBJS) -ledit $(CFLAGS)

$(OBJS): $(wildcard *.h)

test: monkey
	./monkey -test

clean:
	rm -f monkey *.o *~ a.out *#*#*
