#include "VehicleFX.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace VehicleFX {

struct Maps {
    ID id;
    Attrib::StringKey name;
    uint32 marker;
};

// TODO somehow make the symbol of this final?
static const Maps vehicle_fx_maps[22] = {
    {LIGHT_LHEAD, Attrib::StringKey("LIGHT_LHEAD"), STRINGHASH_LEFT_HEADLIGHT},
    {LIGHT_RHEAD, Attrib::StringKey("LIGHT_RHEAD"), STRINGHASH_RIGHT_HEADLIGHT},
    {LIGHT_CHEAD, Attrib::StringKey("LIGHT_CHEAD"), STRINGHASH_CENTRE_HEADLIGHT},
    {LIGHT_LBRAKE, Attrib::StringKey("LIGHT_LBRAKE"), STRINGHASH_LEFT_BRAKELIGHT},
    {LIGHT_RBRAKE, Attrib::StringKey("LIGHT_RBRAKE"), STRINGHASH_RIGHT_BRAKELIGHT},
    {LIGHT_CBRAKE, Attrib::StringKey("LIGHT_CBRAKE"), STRINGHASH_CENTRE_BRAKELIGHT},
    {LIGHT_LREVERSE, Attrib::StringKey("LIGHT_LREVERSE"), STRINGHASH_LEFT_REVERSE},
    {LIGHT_RREVERSE, Attrib::StringKey("LIGHT_RREVERSE"), STRINGHASH_RIGHT_REVERSE},
    {LIGHT_LRSIGNAL, Attrib::StringKey("LIGHT_LRSIGNAL"), 0},
    {LIGHT_RRSIGNAL, Attrib::StringKey("LIGHT_RRSIGNAL"), 0},
    {LIGHT_LFSIGNAL, Attrib::StringKey("LIGHT_LFSIGNAL"), 0},
    {LIGHT_RFSIGNAL, Attrib::StringKey("LIGHT_RFSIGNAL"), 0},
    {LIGHT_COPRED, Attrib::StringKey("LIGHT_COPRED"), STRINGHASH_COPLIGHTRED},
    {LIGHT_COPBLUE, Attrib::StringKey("LIGHT_COPBLUE"), STRINGHASH_COPLIGHTBLUE},
    {LIGHT_COPWHITE, Attrib::StringKey("LIGHT_COPWHITE"), STRINGHASH_COPLIGHTWHITE},
    {LIGHT_COPS, Attrib::StringKey("LIGHT_COPS"), 0},
    {LIGHT_LSIGNAL, Attrib::StringKey("LIGHT_LSIGNAL"), 0},
    {LIGHT_RSIGNAL, Attrib::StringKey("LIGHT_RSIGNAL"), 0},
    {LIGHT_HEADLIGHTS, Attrib::StringKey("LIGHT_HEADLIGHTS"), 0},
    {LIGHT_REVERSE, Attrib::StringKey("LIGHT_REVERSE"), 0},
    {LIGHT_BRAKELIGHTS, Attrib::StringKey("LIGHT_BRAKELIGHTS"), 0},
    {LIGHT_NONE, Attrib::StringKey(""), 0},
};

const Maps *GetMaps() {
    return vehicle_fx_maps;
}

ID LookupID(UCrc32 name) {
    const Maps *fx = GetMaps();

    while (fx->id != LIGHT_NONE) {
        if (fx->name.GetHash32() == name.GetValue()) {
            return fx->id;
        }
        fx++;
    }

    return LIGHT_NONE;
}

} // namespace VehicleFX
