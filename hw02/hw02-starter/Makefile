
all:
	make calc || true
	make calc32 || true
	make calc64 || true
	make fib || true
	make fib32 || true
	make fib64 || true

calc: calc.c
	gcc -g -o calc calc.c

calc32: calc32.S
	gcc -m32 -g -o calc32 calc32.S

calc64: calc64.S
	gcc -g -no-pie -o calc64 calc64.S

fib: fib.c
	gcc -g -o fib fib.c

fib32: fib32.S
	gcc -m32 -g -o fib32 fib32.S

fib64: fib64.S
	gcc -g -no-pie -o fib64 fib64.S

clean:
	rm -f calc calc32 calc64 fib fib32 fib64
