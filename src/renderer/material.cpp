#include "material.h"
#include "texture.h"

material::material() :
    tex(nullptr) {
}

material::material(const texture *t) :
    tex(t) {
}

material::~material() {
}

const texture *material::get_texture() const {
    return tex;
}
