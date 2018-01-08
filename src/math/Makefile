TARGETDIR=../../lib
TARGET=$(TARGETDIR)/libmath.a
SOURCES=math_detail.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I..
CC=g++
CFLAGS=-c -ansi -Wall -Werror -fmax-errors=5 -D_REENTRANT -std=c++17
AR=ar
ARFLAGS=rcs
DEBUGFLAGS=-g
OPTIMIZATIONFLAGS=-O3 -march=haswell

default: release
debug: CFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)
release: CFLAGS += $(OPTIMIZATIONFLAGS)
release: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGETDIR)
	@echo creating lib
	@$(AR) $(ARFLAGS) $(TARGET) $(OBJS)

%.o: %.cpp %.d
	@echo compiling $<
	@$(CC) $(CFLAGS) $(INCLUDEDIRS) $<

%.d: %.cpp
	@echo computing dependencies for $<
	@$(DEP) $(DEPFLAGS) $(INCLUDEDIRS) $< > $@

-include $(DEPS)

clean:
	@rm -f *.o *.d *~

loc:
	@wc -l $(ls ../*/*.{h,cpp} | grep -v lodepng)
