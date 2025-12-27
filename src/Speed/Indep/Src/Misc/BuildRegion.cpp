#include "BuildRegion.hpp"

namespace BuildRegion {

#if defined(EA_REGION_FRANCE) || defined(EA_REGION_GERMANY)
#define EA_REGION_EUROPE
#endif

bool IsAmerica() {
#ifdef EA_REGION_AMERICA
    return true;
#else
    return false;
#endif
}

bool IsEurope() {
#ifdef EA_REGION_EUROPE
    return true;
#else
    return false;
#endif
}

bool IsEuropeFr() {
#ifdef EA_REGION_FRANCE
    return true;
#else
    return false;
#endif
}

bool IsEuropeGer() {
#ifdef EA_REGION_GERMANY
    return true;
#else
    return false;
#endif
}

bool IsJapan() {
#ifdef EA_REGION_JAPAN
    return true;
#else
    return false;
#endif
}

bool IsKorea() {
#ifdef EA_REGION_KOREA
    return true;
#else
    return false;
#endif
}

bool IsPal() {
    return IsEurope();
}

bool ShowLanguageSelect() {
    return false;
}

}; // namespace BuildRegion
