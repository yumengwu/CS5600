# Author: Nat Tuck
# 3650 starter code
# don't change this file

SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=)

all: $(BINS)

$(BINS): $(SRCS)
	gcc -Wall -Werror -g -O2 -std=gnu11 -o $@ $@.c

clean:
	rm -f $(BINS)
