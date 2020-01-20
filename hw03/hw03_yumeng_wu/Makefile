
all:
	make sort || true
	make sort64 || true
	make sort32 || true

sort: sort.c
	gcc -g -o sort sort.c

sort64: sort64.S
	gcc -g -no-pie -o sort64 sort64.S

sort32: sort32.S
	gcc -g -m32 -o sort32 sort32.S

check_stat: check_stat.c
	gcc -g -o check_stat check_stat.c

check_stat32: check_stat.c
	gcc -m32 -g -o check_stat32 check_stat.c

clean:
	rm -f sort sort64 sort32 check_stat check_stat32
