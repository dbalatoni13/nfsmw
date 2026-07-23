#ifndef VEHICLEFX_H
#define VEHICLEFX_H

#include "Speed/Indep/Src/Misc/attribuserinclude.h"

namespace VehicleFX {

enum ID {
    LIGHT_NONE = 0,
    LIGHT_LHEAD = 1,
    LIGHT_RHEAD = 2,
    LIGHT_CHEAD = 4,
    LIGHT_HEADLIGHTS = 7,
    LIGHT_LBRAKE = 8,
    LIGHT_RBRAKE = 16,
    LIGHT_CBRAKE = 32,
    LIGHT_BRAKELIGHTS = 56,
    LIGHT_LREVERSE = 64,
    LIGHT_RREVERSE = 128,
    LIGHT_REVERSE = 192,
    LIGHT_LRSIGNAL = 256,
    LIGHT_RRSIGNAL = 512,
    LIGHT_LFSIGNAL = 1024,
    LIGHT_LSIGNAL = 1280,
    LIGHT_RFSIGNAL = 2048,
    LIGHT_RSIGNAL = 2560,
    LIGHT_COPRED = 4096,
    LIGHT_COPBLUE = 8192,
    LIGHT_COPWHITE = 16384,
    LIGHT_COPS = 28672,
};

const unsigned int MAX_FX = 32; // TODO why not 22?

struct Maps {
    ID id;
    Attrib::StringKey name;
    uint32 marker;
};

const Maps *GetMaps();

}; // namespace VehicleFX

#endif
