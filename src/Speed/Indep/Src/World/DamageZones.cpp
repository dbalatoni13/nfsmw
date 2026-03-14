#include "Speed/Indep/Src/World/DamageZones.h"

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

namespace DamageZone {

static Attrib::StringKey DZSystemName[DZ_MAX];
static UCrc32 DZDamageStimulus[7];
static UCrc32 DZImpactStimulus[7];

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
