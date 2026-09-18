#include "Speed/Indep/Src/World/DamageZones.h"

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

static Attrib::StringKey DZSystemName[DamageZone::DZ_MAX] = {
    "DZ_FRONT",  //
    "DZ_REAR",   //
    "DZ_LEFT",   //
    "DZ_RIGHT",  //
    "DZ_LFRONT", //
    "DZ_RFRONT", //
    "DZ_LREAR",  //
    "DZ_RREAR",  //
    "DZ_TOP",    //
    "DZ_BOTTOM", //
};

static UCrc32 DZDamageStimulus[7] = {
    UCRC32_DAMAGE_1, //
    UCRC32_DAMAGE_2, //
    UCRC32_DAMAGE_3, //
    UCRC32_DAMAGE_4, //
    UCRC32_DAMAGE_5, //
    UCRC32_DAMAGE_6, //
    UCRC32_DAMAGE_7, //
};

static UCrc32 DZImpactStimulus[7] = {
    UCRC32_IMPACT_1, //
    UCRC32_IMPACT_2, //
    UCRC32_IMPACT_3, //
    UCRC32_IMPACT_4, //
    UCRC32_IMPACT_5, //
    UCRC32_IMPACT_6, //
    UCRC32_IMPACT_7, //
};

namespace DamageZone {
Attrib::StringKey GetSystemName(ID id) {
    return DZSystemName[id];
}

UCrc32 GetDamageStimulus(unsigned int level) {
    return DZDamageStimulus[level];
}

UCrc32 GetImpactStimulus(unsigned int level) {
    return DZImpactStimulus[level];
}

} // namespace DamageZone

