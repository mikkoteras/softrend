BINDIR=../bin/

LIB=$(BINDIR)/libmath.a
LIBTEST=$(BINDIR)/math_tests
EXE=$(BINDIR)/softrend

TARGETS=$(LIB) $(LIBTEST) $(EXE)

MAKE=make
MAKEARGS=-j 4

default: release
debug: MAKEARGS += debug
debug: all
release: MAKEARGS += release
release: all

all:
	$(MAKE) $(MAKEARGS) -C math
	$(MAKE) $(MAKEARGS) -C math_tests
	$(MAKE) $(MAKEARGS) -C renderer

clean:
	$(MAKE) -C math clean
	$(MAKE) -C math_tests clean
	$(MAKE) -C renderer clean

clobber: clean
	rm -rf ../bin ../lib
