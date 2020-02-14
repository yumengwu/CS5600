
CFLAGS := -g -Wall -Werror
SRCS   := $(wildcard *.c)

psort: $(SRCS)
	gcc $(CFLAGS) -o psort $(SRCS) -lm -lpthread

clean:
	rm -f psort

.PHONY: clean
