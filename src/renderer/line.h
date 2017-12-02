#ifndef LINE_H
#define LINE_H

#include "vector.h"
#include "color.h"

class framebuffer;
class mesh;
class scene;

class line {
public:
    line();
    line(int vi1, int v2, const color &c1, const color &c2);
    line(const line &rhs);
    line(line &&rhs);
    const line &operator=(const line &rhs);
    line &operator=(line &&rhs);
    ~line();

public:
    void render(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const;
    static void render(framebuffer &target,
                       float x1, float y1, float z1, const color &c1,
                       float x2, float y2, float z2, const color &c2);

private:
    int vertex_index[2];
    color endpoint_color[2];
};

#endif
