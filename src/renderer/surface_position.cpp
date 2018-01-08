#include "surface_position.h"
#include "util.h"
#include <sstream>

std::string surface_position::to_string() const {
    std::ostringstream o;
    o << "v = " << util::to_string(view_position)
      << " w = " << util::to_string(world_position)
      << " n = " << util::to_string(normal)
      << " uv = " << util::to_string(uv)
      << " s = " << shade.to_string();
    return o.str();
}
