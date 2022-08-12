CFLAGS=-std=c11

ccompiler: ccompiler.c

test: ccompiler
	./test.sh

clean:
	rm -f ccompiler *.o *~ tmp*

.PHONY: test clean