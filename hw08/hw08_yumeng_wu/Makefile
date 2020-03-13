
BINS := collatz-list-sys collatz-ivec-sys \
		collatz-list-xv6 collatz-ivec-xv6 \
		collatz-list-opt collatz-ivec-opt \
		frag-opt frag-sys frag-xv6

HDRS := $(wildcard *.h)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -g -Og -Wall -Werror
LDLIBS := -lpthread

all: $(BINS)

collatz-list-sys: list_main.o sys_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

collatz-ivec-sys: ivec_main.o sys_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

collatz-list-xv6: list_main.o xv6_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

collatz-ivec-xv6: ivec_main.o xv6_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

collatz-list-opt: list_main.o opt_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

collatz-ivec-opt: ivec_main.o opt_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

frag-opt: frag_main.o opt_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

frag-sys: frag_main.o sys_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

frag-xv6: frag_main.o xv6_malloc.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o : %.c $(HDRS) Makefile

clean:
	rm -f *.o $(BINS) time.tmp outp.tmp

test:
	perl test.pl

.PHONY: clean test
