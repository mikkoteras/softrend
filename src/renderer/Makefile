TARGET=softrend
SOURCES=ambient_light.cpp \
        animation_clock.cpp \
        auto_scene.cpp \
        benchmark.cpp \
        bounding_box.cpp \
        color.cpp \
        coordinate_system.cpp \
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
        texture.cpp \
        triangle.cpp \
        util.cpp \
        vertex.cpp \
        window.cpp

DEPS=$(patsubst %.cpp,%.d,$(SOURCES))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))

DEP=g++
DEPFLAGS=-E -MM
INCLUDEDIRS=-I. -I.. -I../math -I../../math -I/usr/include/SDL2
CC=g++
CFLAGS=-c -g -Wall -ansi -Wall -Werror -D_REENTRANT -std=c++14
LD=g++
LDFLAGS=-o $(TARGET)
LIBDIRS=-L/usr/lib/x86_64-linux-gnu
LIBS=-lstdc++ -lstdc++fs -lSDL2

all: debug

debug: $(OBJS)
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
