
CFLAGS := -no-pie -g

all:
	(make square || true)
	(make cube || true)
	(make add1 || true)

square: square-main.o square.o
	gcc $(CFLAGS) -o square square-main.o square.o

square-main.o: square-main.s
	gcc $(CFLAGS) -c -o square-main.o square-main.s

square.o: square.c
	gcc $(CFLAGS) -c -o square.o square.c

cube: cube-main.o cube.o
	gcc $(CFLAGS) -o cube cube-main.o cube.o

cube-main.o:
	gcc $(CFLAGS) -c -o cube-main.o cube-main.c

cube.o: cube.s
	gcc $(CFLAGS) -c -o cube.o cube.s

add1: add1-main.o add1.o
	gcc -m32 $(CFLAGS) -o add1 add1-main.o add1.o

add1-main.o: add1-main.c
	gcc -m32 $(CFLAGS) -c -o add1-main.o add1-main.c

add1.o: add1.s
	gcc -m32 $(CFLAGS) -c -o add1.o add1.s

clean:
	rm -f *.o square cube add1

.PHONY: all clean
