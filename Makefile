CFLAGS = -std=gnu89 -Wno-int-conversion -Wno-implicit-int -Wno-old-style-definition -Wno-builtin-declaration-mismatch -Wno-implicit-function-declaration -Wno-pointer-to-int-cast

all: pflayer.o testpf testhash

buf.o: buf.c pftypes.h pf.h
	cc $(CFLAGS) -c buf.c

hash.o: hash.c pftypes.h pf.h
	cc $(CFLAGS) -c hash.c

pf.o: pf.c pftypes.h pf.h
	cc $(CFLAGS) -c pf.c

pflayer.o: buf.o hash.o pf.o
	ld -r -o pflayer.o buf.o hash.o pf.o

testpf: testpf.o pflayer.o
	cc $(CFLAGS) -o testpf testpf.o pflayer.o

testhash: testhash.o pflayer.o
	cc $(CFLAGS) -o testhash testhash.o pflayer.o

experiment: experiment.o pflayer.o
	cc $(CFLAGS) -o experiment experiment.o pflayer.o

clean:
	rm -f *.o pflayer.o testpf testhash experiment pf_results.csv
