
CFLAGS := -g -Wall -Werror -std=gnu99
SRCS   := $(wildcard *.c)

tsort: $(SRCS)
	gcc $(CFLAGS) -o tsort $(SRCS) -lm -lpthread

clean:
	rm -f tsort

.PHONY: clean tools
