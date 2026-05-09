#ifndef MISC_BUILDREGION_H
#define MISC_BUILDREGION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

// Range: 0xFFFFFFFF -> 0x801FB34C
eBuildRegion GetBuildRegion();

// Range: 0xFFFFFFFF -> 0x801FB34C
const char *GetName();

// Range: 0xFFFFFFFF -> 0x801FB34C
const char *GetAbbreviation();

// Range: 0x801FB34C -> 0x801FB354
bool IsAmerica();

// Range: 0x801FB354 -> 0x801FB35C
bool IsEurope();

// Range: 0xFFFFFFFF -> 0x801FB35C
bool IsEuropeEng();

// Range: 0x801FB35C -> 0x801FB364
bool IsEuropeFr();

// Range: 0x801FB364 -> 0x801FB36C
bool IsEuropeGer();

// Range: 0xFFFFFFFF -> 0x801FB36C
bool IsEuropeSpan();

// Range: 0xFFFFFFFF -> 0x801FB36C
bool IsEuropeItal();

// Range: 0xFFFFFFFF -> 0x801FB36C
bool IsEuropeEngItalSpan();

// Range: 0xFFFFFFFF -> 0x801FB36C
bool IsEuropeEngFrGer();

// Range: 0xFFFFFFFF -> 0x801FB36C
bool IsAsia();

// Range: 0x801FB36C -> 0x801FB374
bool IsJapan();

// Range: 0x801FB374 -> 0x801FB37C
bool IsKorea();

// Range: 0x801FB37C -> 0x801FB39C
bool IsPal();

// Range: 0x801FB39C -> 0x801FB3A4
bool ShowLanguageSelect();

}; // namespace BuildRegion

#endif
