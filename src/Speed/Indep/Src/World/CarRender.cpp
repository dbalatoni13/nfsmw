#include "./CarRender.hpp"

static const CarFXPosInfo FXMarkerNameHashMappings[28] = {
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0xA5B28001, 0xBCF8A18B, 0xBD7CF15E, 0 }, 3 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
};

bool GetNumCarEffectMarkerHashes(CarEffectPosition fx_pos, int &count_out) {
    bool position_marker_based_fxpos = false;
    count_out = 0;
    if (FXMarkerNameHashMappings[fx_pos].marker_count != 0xFF) {
        count_out = FXMarkerNameHashMappings[fx_pos].marker_count;
        position_marker_based_fxpos = true;
    }

    return position_marker_based_fxpos;
}

const unsigned int *GetCarEffectMarkerHashes(CarEffectPosition fx_pos) {
    return reinterpret_cast<const unsigned int *>(&FXMarkerNameHashMappings[fx_pos].marker_count);
}
