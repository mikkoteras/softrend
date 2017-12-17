BINDIR=../../bin
TARGET=$(BINDIR)/softrend
SOURCES=animation_clock.cpp \
        auto_scene.cpp \
        benchmark.cpp \
        benchmark_frame.cpp \
        bounding_box.cpp \
        color.cpp \
        command_line.cpp \
        coordinate_system.cpp \
        demo.cpp \
        directional_light.cpp \
        framebuffer.cpp \
        freecam_scene.cpp \
        importer.cpp \
        light.cpp \
        light_list.cpp \
        line.cpp \
        lodepng.cpp \
        main.cpp \
        material.cpp \
        material_library.cpp \
        mesh.cpp \
        point_light.cpp \
        scene.cpp \
        scene_info.cpp \
        texture.cpp \
        triangle.cpp \
        triangle_render.cpp \
        util.cpp \
        window.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I.. -I../math -I../../math -I/usr/include/SDL2
CC=g++
CFLAGS=-c -ansi -Wall -Werror -fmax-errors=5 -D_REENTRANT -std=c++17
LD=g++
LDFLAGS=-o $(TARGET)
DEBUGFLAGS=-g
OPTIMIZATIONFLAGS=-O3
LIBDIRS=-L../../lib -L/usr/lib/x86_64-linux-gnu
LIBS=-lmath -lstdc++ -lstdc++fs -lpthread -lSDL2 -lSDL2_ttf

all: default
default: debug

debug: CFLAGS += $(DEBUGFLAGS)
debug: LDFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

release: CFLAGS += $(OPTIMIZATIONFLAGS)
release: LDFLAGS += $(OPTIMIZATIONFLAGS)
release: $(TARGET)

$(TARGET): $(BINDIR) $(OBJS)
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

-include $(DEPS)

loc:
	@wc -l $(ls ../*/*.{h,cpp} | grep -v lodepng)
