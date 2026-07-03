#ifndef BUILD_REGION_HPP
#define BUILD_REGION_HPP

namespace BuildRegion {

typedef enum {
    BUILD_REGION_AMERICA = 0,
    BUILD_REGION_EU = 1,
    BUILD_REGION_KOREA = 2,
    BUILD_REGION_JAPAN = 3,
    BUILD_REGION_TAIWAN = 4,
    BUILD_REGION_UK = 5,
    BUILD_REGION_FRANCE = 6,
    BUILD_REGION_GERMANY = 7,
    BUILD_REGION_ITALY = 8,
    BUILD_REGION_SPAIN = 9,
    BUILD_REGION_ROA = 10,
    BUILD_REGION_CHINA = 11,
    BUILD_REGION_THAILAND = 12,
    BUILD_REGION_SOUTHAMERICA = 13,
    BUILD_REGION_ASIA = 10,
    BUILD_REGION_EUROPE = 1
} eBuildRegion;

eBuildRegion GetBuildRegion();

const char *GetName();

const char *GetAbbreviation();

bool IsAmerica();
bool IsEurope();
bool IsEuropeEng();
bool IsEuropeFr();
bool IsEuropeGer();
bool IsEuropeSpan();
bool IsEuropeItal();
bool IsEuropeEngItalSpan();
bool IsEuropeEngFrGer();
bool IsAsia();
bool IsJapan();
bool IsKorea();
bool IsPal();
bool ShowLanguageSelect();

inline const char *GetCarBadgingSuffix() {
    if (IsEurope()) {
        return "_EU";
    } else {
        return nullptr;
    }
}

}; // namespace BuildRegion

#endif
