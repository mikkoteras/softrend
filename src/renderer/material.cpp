#include "material.h"
#include "texture.h"
#include "vector.h"

material::material(const model &data, const texture *t) :
    data(data),
    tex(t) {
}

material::~material() {
}

const texture *material::get_texture() const {
    return tex;
}
