#ifndef BUILD_REGION_HPP
#define BUILD_REGION_HPP

namespace BuildRegion {

bool IsAmerica();
bool IsEurope();
bool IsPal();
char *GetSlusCode();
unsigned char *GetDnasPassPhrase();

inline const char *GetCarBadgingSuffix() {
    if (IsEurope()) {
        return "_EU";
    } else {
        return nullptr;
    }
}

}; // namespace BuildRegion

#endif
