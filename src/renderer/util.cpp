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

    std::string to_string(const bounding_box &box) {
        std::ostringstream result;
        result << "[ " << to_string(box.min()) << " " << to_string(box.max()) << " ]";
        return result.str();
    }
}
