#include "math_detail.h"
#include <cmath>

namespace math {
    namespace detail {
        template<> float sin<float>(float t)      { return ::sinf(t);   }
        template<> float cos<float>(float t)      { return ::cosf(t);   }
        template<> float tan<float>(float t)      { return ::tanf(t);   }
        template<> float asin<float>(float t)     { return ::asinf(t);  }
        template<> float acos<float>(float t)     { return ::acosf(t);  }
        template<> float abs<float>(float t)      { return ::fabs(t);   }
        template<> float floor<float>(float t)    { return ::floorf(t); }

        template<> float pi<float>()              { return 3.14159265358979323846f; }

        template<> double sin<double>(double t)   { return ::sin(t);    }
        template<> double cos<double>(double t)   { return ::cos(t);    }
        template<> double tan<double>(double t)   { return ::tan(t);    }
        template<> double asin<double>(double t)  { return ::asin(t);   }
        template<> double acos<double>(double t)  { return ::acos(t);   }
        template<> double abs<double>(double t)   { return ::abs(t);    }
        template<> double floor<double>(double t) { return ::floor(t);  }

        template<> double pi<double>()            { return 3.14159265358979323846;  }
    }
}
