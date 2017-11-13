#ifndef MATERIAL_H
#define MATERIAL_H

class texture;

class material {
public:
    material();
    material(const texture *t);
    ~material();

    const texture *get_texture() const;

private:
    const texture *tex;
};

#endif
