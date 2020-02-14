
CFLAGS := -g -Wall -Werror
SRCS   := $(wildcard *.c)

all: tools psort tsort

tools:
	(cd tools && make)

psort:
	(cd psort && make)

tsort:
	(cd tsort && make)

clean:
	(cd tools && make clean)
	(cd psort && make clean)
	(cd tsort && make clean)
	rm -f data.dat *.plist valgrind.out

.PHONY: clean all tools psort tsort
