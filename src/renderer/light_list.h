#ifndef LIGHT_LIST_H
#define LIGHT_LIST_H

#include <vector>
#include "color.h"
#include "vector.h"

class light;
class directional_light;

class light_list {
public:
    light_list();
    ~light_list();
    
    void set_ambient_light(const color &col);
    void add_directional_light(const directional_light &l);

    const std::vector<light*> &get() const; // TODO refactor
    color ambient_coeff() const;

private:
    color ambient_light;
    std::vector<light*> per_pixel_lights;
};

#endif
