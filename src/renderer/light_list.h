#ifndef LIGHT_LIST_H
#define LIGHT_LIST_H

#include <memory>
#include <vector>
#include "color.h"
#include "vector.h"

class light;

class light_list {
public:
    light_list();
    ~light_list();

    void add(std::unique_ptr<light> l);
    color sum(const math::vector3f &surface_normal) const;

private:
    std::vector<std::unique_ptr<light>> diffuse_lights;
    std::vector<std::unique_ptr<light>> specular_lights;
};

#endif
