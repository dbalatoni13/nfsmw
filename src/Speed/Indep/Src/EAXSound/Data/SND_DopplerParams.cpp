#include "Speed/Indep/Src/EAXSound/Data/SND_DopplerParams.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixerDefines.hpp"

// size: 0x18, address: 0x8045E664, Decl: 10
stDOPPLER_PARAMS g_DOPPLER_PARAMS[3] = {
    stDOPPLER_PARAMS(0.35f, DIST_OBJ_TO_PLAYERCAR),
    stDOPPLER_PARAMS(0.3f, DIST_OBJ_TO_PLAYERCAR),
    stDOPPLER_PARAMS(0.3f, DIST_OBJ_TO_PLAYERCAR),
};

int SIZE_OF_g_DOPPLER_PARAMS = sizeof(g_DOPPLER_PARAMS); // size: 0x4, address: 0xFFFFFFFF, Decl: 30
