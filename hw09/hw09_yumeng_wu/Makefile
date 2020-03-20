
BINS := test1 test2 test3 test4 test5 test6
HDRS := $(wildcard *.h)

CC := gcc
CFLAGS := -g -Wall -Werror -I./tests -I.

all: $(BINS)

test1: tests/test1.o gc.o
	$(CC) -o $@ $^

test2: tests/test2.o gc.o
	$(CC) -o $@ $^

test3: tests/test3.o gc.o tests/list.o
	$(CC) -o $@ $^

test4: tests/test4.o gc.o tests/list.o
	$(CC) -o $@ $^

test5: tests/test5.o gc.o tests/strint.o tests/str.o
	$(CC) -o $@ $^

test6: tests/test6.o gc.o
	$(CC) -o $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(BINS) tests/*.o
