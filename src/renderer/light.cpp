#include "light.h"

light::light() {
}

light::~light() {
}

light::light(const light&) {
}
        
light::light(light&&) {
}

const light &light::operator=(const light&) {
    return *this;
}

light &light::operator=(light&&) {
    return *this;
}
