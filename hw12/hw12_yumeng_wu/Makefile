
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)

CFLAGS := -g `pkg-config fuse --cflags`
LDLIBS := `pkg-config fuse --libs` -lbsd

all: cowmount cowtool

cowtool: $(filter-out cowmount.o, $(OBJS))
	gcc $(CLFAGS) -o $@ $^ $(LDLIBS)

cowmount: $(filter-out cowtool.o, $(OBJS))
	gcc $(CLFAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(HDRS)
	gcc $(CFLAGS) -c -o $@ $<

disk0.cow: cowtool
	mkdir -p mnt || true
	if [ ! -e disk0.cow ]; then ./cowtool new disk0.cow; fi

clean: unmount
	rm -f cowmount cowtool *.o test.log disk0.cow disk1.cow
	rmdir mnt || true

mount: all
	make disk0.cow
	./cowmount -s -f mnt disk0.cow

unmount:
	fusermount -u mnt || true

cleanmount:
	make clean
	make all
	make mount

test: all
	perl test.pl

gdb: all
	make disk0.cow
	gdb --args ./cowmount -s -f mnt disk0.cow

.PHONY: all clean mount unmount gdb
