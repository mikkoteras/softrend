BINDIR=../../bin
TARGET=$(BINDIR)/softrend
SOURCES=animation_clock.cpp \
        auto_scene.cpp \
        benchmark.cpp \
        benchmark_frame.cpp \
        bounding_box.cpp \
        color3.cpp \
        color4.cpp \
        command_line.cpp \
        constant_color_material.cpp \
        coordinate_system.cpp \
        demo.cpp \
        diffuse_material.cpp \
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
        specular_material.cpp \
        surface_position.cpp \
        texture.cpp \
        triangle.cpp \
        util.cpp \
        window.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I.. -I/usr/include/SDL2
CC=g++
CFLAGS=-c -ansi -Wall -Werror -fmax-errors=5 -D_REENTRANT -std=c++17
LD=g++
LDFLAGS=-o $(TARGET)
DEBUGFLAGS=-g
OPTIMIZATIONFLAGS=-O3 -march=haswell
LIBDIRS=-L../../lib -L/usr/lib/x86_64-linux-gnu
LIBS=-lmath -lstdc++ -lstdc++fs -lpthread -lSDL2 -lSDL2_ttf

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

-include $(DEPS)

clean:
	@rm -f *.o *.d *~

loc:
	@wc -l $(ls ../*/*.{h,cpp} | grep -v lodepng)
