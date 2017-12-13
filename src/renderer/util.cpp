#include "util.h"

namespace util {
    std::string to_string(shading_model_t model) {
        switch (model) {
        case flat:
            return "flat";
        case gouraud:
            return "gouraud";
        case phong:
            return "phong";
        default:
            return "<not valid>";
        }
    }
}
