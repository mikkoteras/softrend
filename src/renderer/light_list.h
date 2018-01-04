#ifndef LIGHT_LIST_H
#define LIGHT_LIST_H

#include <vector>
#include "color3.h"
#include "vector.h"

class light;
class directional_light;
class point_light;

class light_list {
public:
    light_list();
    ~light_list();

    void clear();
    void set_ambient_light(const color3 &col);
    void add_light(const directional_light &l);
    void add_light(const point_light &l);

    const std::vector<light*> &get() const; // TODO refactor
    color3 ambient_coeff() const;

private:
    color3 ambient_light;
    std::vector<light*> per_pixel_lights;
};

#endif
