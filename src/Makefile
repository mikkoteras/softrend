BINDIR=../bin/

LIB=$(BINDIR)/libmath.a
LIBTEST=$(BINDIR)/math_tests
EXE=$(BINDIR)/softrend

TARGETS=$(LIB) $(LIBTEST) $(EXE)

MAKE=make
MAKEARGS=-j 4

debug: MAKEARGS += debug
debug: $(TARGETS)

release: MAKEARGS += release
release: $(TARGETS)

$(LIB): $(BINDIR)
	$(MAKE) $(MAKEARGS) -C math

$(LIBTEST): $(BINDIR)
	$(MAKE) $(MAKEARGS) -C math_tests

$(EXE): $(BINDIR)
	$(MAKE) $(MAKEARGS) -C renderer

$(BINDIR):
	mkdir $(BINDIR)

clean:
	for d in math math_tests renderer; do cd $$d; rm -f *.o *.d *~; cd ..; done

clobber: clean
	rm -rf ../bin ../lib

