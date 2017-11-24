TARGET=math_tests
SOURCES=linear_transform_tests.cpp \
        main.cpp \
        matrix_tests.cpp \
        test_harness.cpp \
        util_tests.cpp \
        vector_tests.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I.. -I../math -I../../math
CC=g++
CFLAGS=-c -g -Wall -ansi -Wall -Werror -std=c++17
LD=g++
LDFLAGS=-o $(TARGET)
LIBDIRS=-L/usr/lib/x86_64-linux-gnu
LIBS=-lstdc++

all: default
default: $(TARGET)

$(TARGET): $(OBJS)
	@echo linking
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS)

%.o: %.cpp %.d
	@echo compiling $<
	@$(CC) $(CFLAGS) $(INCLUDEDIRS) $<

%.d: %.cpp
	@echo computing dependencies for $<
	@$(DEP) $(DEPFLAGS) $(INCLUDEDIRS) $< > $@

-include $(DEPS)

clean:
	rm -f *.o *.d *~
