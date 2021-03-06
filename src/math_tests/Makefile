BINDIR=../../bin
TARGET=$(BINDIR)/math_tests
SOURCES=linear_transform_tests.cpp \
        main.cpp \
        matrix_tests.cpp \
        test_harness.cpp \
        util_tests.cpp \
        vector_tests.cpp \
	vector_benchmarks.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I..
CC=g++
CFLAGS=-c -g -Wall -ansi -Wall -Werror -std=c++17
LD=g++
LDFLAGS=-o $(TARGET)
LIBDIRS=-L/usr/lib/x86_64-linux-gnu -L../../lib
LIBS=-lmath -lstdc++
DEBUGFLAGS=-g
OPTIMIZATIONFLAGS=-O3 -march=haswell

default: release
debug: CFLAGS += $(DEBUGFLAGS)
debug: LDFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)
release: CFLAGS += $(OPTIMIZATIONFLAGS)
release: LDFLAGS += $(OPTIMIZATIONFLAGS)
release: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	@echo linking
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS)

%.o: %.cpp %.d
	@echo compiling $<
	@$(CC) $(CFLAGS) $(INCLUDEDIRS) $<

%.d: %.cpp
	@echo computing dependencies for $<
	@$(DEP) $(DEPFLAGS) $(INCLUDEDIRS) $< > $@

$(BINDIR):
	mkdir $(BINDIR)

clean:
	@rm -f *.o *.d *~

-include $(DEPS)
