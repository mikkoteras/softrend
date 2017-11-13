#ifndef MATERIAL_LIBRARY_H
#define MATERIAL_LIBRARY_H

#include "material.h"
#include "texture.h"
#include <map>
#include <string>

class material_library_exception {};

class material_library {
public:
    material_library();
    ~material_library();

    bool add_texture(const std::string &name, const std::string &filename);
    bool add_material(const std::string &name, const std::string &texture_name);
    const material *get_material(const std::string &name) const;
    const texture *get_texture(const std::string &name) const;
    
private:
    std::map<std::string, texture> textures;
    std::map<std::string, material> materials;
};

#endif
