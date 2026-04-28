#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace VehicleFX {

struct Maps {
    ID id;
    Attrib::StringKey name;
    unsigned int marker;
};

Maps vehicle_fx_maps[22] asm("_9VehicleFX.vehicle_fx_maps");

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

}
