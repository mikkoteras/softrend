#include "util.h"

namespace util {
    std::string to_string(reflection_model_t model) {
        switch (model) {
        case lambert:
            return "lambert";
        case gouraud:
            return "gouraud";
        case phong:
            return "phong";
        case blinn_phong:
            return "blinn-phong";
        default:
            return "<not valid>";
        }
    }
}
