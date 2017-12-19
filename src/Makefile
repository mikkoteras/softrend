BINDIR=../bin/

LIB=$(BINDIR)/libmath.a
LIBTEST=$(BINDIR)/math_tests
EXE=$(BINDIR)/softrend

TARGETS=$(LIB) $(LIBTEST) $(EXE)

MAKE=make
MAKEARGS=-j 4

debug: MAKEARGS += debug
debug: all

release: MAKEARGS += release
release: all

all:
	$(MAKE) $(MAKEARGS) -C math
	$(MAKE) $(MAKEARGS) -C math_tests
	$(MAKE) $(MAKEARGS) -C renderer

clean:
	for d in math math_tests renderer; do cd $$d; rm -f *.o *.d *~; cd ..; done

clobber: clean
	rm -rf ../bin ../lib

