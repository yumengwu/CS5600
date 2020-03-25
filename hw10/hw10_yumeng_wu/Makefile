
all: tools sort

tools:
	(cd tools && make)

sort: ssort/src/main.rs
	(cd ssort && cargo build)
	cp ssort/target/debug/ssort sort

clean:
	(cd tools && make clean)
	(cd ssort && cargo clean)
	rm -f sort data.dat data-out.dat

demo: tools sort
	tools/gen-input 1000 /tmp/input.dat
	./sort 4 /tmp/input.dat /tmp/output.dat
	tools/check-sorted /tmp/output.dat || echo fail

.PHONY: clean all tools demo
