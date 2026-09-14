#include "Speed/Indep/Src/World/DamageZones.h"

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

static Attrib::StringKey DZSystemName[DamageZone::DZ_MAX] = {
    "DZ_FRONT", "DZ_REAR", "DZ_LEFT", "DZ_RIGHT", "DZ_LFRONT", "DZ_RFRONT", "DZ_LREAR", "DZ_RREAR", "DZ_TOP", "DZ_BOTTOM",
};
static UCrc32 DZDamageStimulus[7] = {0x5E6906CD, 0xD26DA825, 0xC2E881EF, 0x69F4A3BA, 0x72F7C774, 0x79C5AA6A, 0x95D2A082};
static UCrc32 DZImpactStimulus[7] = {0x4867478B, 0xC6C065C5, 0x5B5EEF00, 0xA0021482, 0x8F96F68B, 0x5BD74B1B, 0x89B3FB50};

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
