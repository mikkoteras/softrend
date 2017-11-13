#ifndef SCENE_H
#define SCENE_H

#include "animation_clock.h"
#include "directional_light.h"
#include "matrix.h"
#include "material_library.h"
#include "vector.h"
#include <vector>

class framebuffer;

class scene {
public:
    scene();
    virtual ~scene();

    void start();
    virtual void render(framebuffer &fb) = 0;
    
public:
    math::matrix4x4f world_to_view();
    material_library &materials();
    const std::vector<directional_light> &lights() const;

protected:
    void set_eye_position(const math::vector3f &position);
    void set_eye_direction(const math::vector3f &direction);           // option 1
    void set_eye_reference_point(const math::vector3f &look_at_point); // option 2
    void set_eye_orientation(const math::vector3f &up_direction);
    void set_view_to_view_plane_distance(float distance);
    void add_light(const directional_light &light);
    
protected:
    animation_clock clock;
    material_library mat_lib;

private:
    math::vector3f eye_position;
    math::vector3f eye_direction;
    math::vector3f eye_up;
    float viewing_distance;
    math::matrix4x4f world_to_view_matrix;
    bool world_to_view_matrix_dirty;
    std::vector<directional_light> light_list;
};

#endif
