
all:
	(cd xv6 && make)

test: all
	perl test.pl

clean:
	(cd xv6 && make clean)

qemu: all
	(cd xv6 && make qemu-nox)

.PHONY: all test clean qemu
