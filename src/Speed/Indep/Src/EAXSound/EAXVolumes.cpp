#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"

// Decl: 215
int RoadNoiseVolumes[9] = {
    15000, 24000, 32000, 24000, 24000, 12000, 19000, 32500, 32500,
};

int GetRoadNoiseTransitionVol(FXROADNOISE_TRANSITION ID) {
    switch (ID) {
        case FXROADNOISE_TRANSITION_CURBOFF:
            return 15500;

        case FXROADNOISE_TRANSITION_CURBON:
            return 15500;

        case FXROADNOISE_TRANSITION_SPIKESTRIP:
            return 26000;

        case FXROADNOISE_TRANSITION_BLOWN:
            return 0x7FFF;

        default:
            return 0x7FFF;
    }
}
