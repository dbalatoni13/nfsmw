#ifndef WORLD_CAR_H
#define WORLD_CAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

typedef enum { NO_CONTROL = 0, REAL_CONTROL = 1, DRAG_CONTROL = 2, AI_CONTROL = 3, TRAFFIC_CONTROL = 4, COP_CONTROL = 5 } ControlMode;

typedef enum {
    NO_MOVEMENT = 0,
    PHYSICS_MOVEMENT = 1,
    TRAFFIC_MOVEMENT = 2,
    MELLOW_MOVEMENT = 3,
    ONLINE_MOVEMENT = 4,
    BELT_MOVEMENT = 5,
    NIS_MOVEMENT = 6,
    NUM_MOVEMENT_MODES = 7
} MovementMode;

typedef enum { NORMAL_DAMPENING = 0, WILDCOLLISION_DAMPENING = 1, COLLISION_DAMPENING = 2 } DampeningMode;

typedef enum { SIGNAL_LIGHT_ALL_OFF = 0, SIGNAL_LIGHT_LEFT_ON = 1, SIGNAL_LIGHT_RIGHT_ON = 2, SIGNAL_LIGHT_ALL_ON = 3 } SignalLightStateEnum;

typedef enum { COP_LIGHTS_OFF = 0, COP_LIGHTS_SLOW = 1, COP_LIGHTS_FAST = 2 } CopLightState;

typedef enum { IDLE_INTERVAL = 0, RPM2_INTERVAL = 1, RPM3_INTERVAL = 2, RPM4_INTERVAL = 3, RPM5_INTERVAL = 4, RPM6_INTERVAL = 5 } RPMInterval;

typedef enum { SIREN_MODE_NONE = 0, SIREN_MODE_01 = 1, SIREN_MODE_02 = 2, SIREN_MODE_03 = 3, SIREN_MODE_BROKEN = 4 } SirenMode;

typedef enum { HYDRAULICS_OFF = 0, HYDRAULICS_EXTENDED = 1, HYDRAULICS_EXTENDING = 2, HYDRAULICS_RELAXING = 3 } HydraulicState;

typedef enum {
    CARFX_NONE = 0,
    CARFX_DRIVE_OVER = 1,
    CARFX_DRIVE_OVER2 = 2,
    CARFX_SKID_SMOKE = 3,
    CARFX_TIRE_SPEW = 4,
    CARFX_BOTTOM_OUT = 5,
    CARFX_DAM_RADIATOR = 6,
    CARFX_DAM_ENGINE = 7,
    CARFX_BLOWN_TIRE = 8,
    CARFX_DRIVE_ON_FLAT_TIRE = 9,
    CARFX_NITRO = 10,
    CARFX_EXHAUST_SMOKE = 11,
    CARFX_EXHAUST_BLOWOFF = 12,
    CARFX_NOS_BLOWOFF = 13,
    CARFX_BREAK_SIDE_MIRROR_LEFT = 14,
    CARFX_BREAK_SIDE_MIRROR_RIGHT = 15,
    CARFX_BREAK_LICENSE_PLATE_FRONT = 16,
    CARFX_BREAK_LICENSE_PLATE_RIGHT = 17,
    CARFX_BREAK_HEADLIGHT_LEFT = 18,
    CARFX_BREAK_HEADLIGHT_RIGHT = 19,
    CARFX_BREAK_BRAKELIGHT_LEFT = 20,
    CARFX_BREAK_BRAKELIGHT_RIGHT = 21,
    CARFX_BREAK_BRAKELIGHT_CENTRE = 22,
    CARFX_BREAK_WINDSHIELD = 23,
    CARFX_BREAK_WINDOW_REAR = 24,
    CARFX_BREAK_WINDOW_LEFT_FRONT = 25,
    CARFX_BREAK_WINDOW_LEFT_REAR = 26,
    CARFX_BREAK_WINDOW_RIGHT_FRONT = 27,
    CARFX_BREAK_WINDOW_RIGHT_REAR = 28,
    NUM_CARFX = 29
} CarEffect;

// total size: 0x8
class Car : public bTNode<Car> {
  public:
    bVector3 *GetGeometryPosition() {
        return nullptr; // TODO
    }
};

struct VinylLayerInfo {
    // total size: 0x34
    unsigned int m_LayerHash;               // offset 0x0, size 0x4
    struct TextureInfo *m_LayerTexture;     // offset 0x4, size 0x4
    struct TextureInfo *m_LayerMaskTexture; // offset 0x8, size 0x4
    unsigned char *m_LayerImageData;        // offset 0xC, size 0x4
    unsigned int *m_LayerImagePaletteData;  // offset 0x10, size 0x4
    unsigned char *m_LayerMaskData;         // offset 0x14, size 0x4
    unsigned int *m_LayerMaskPaletteData;   // offset 0x18, size 0x4
    int m_NumColours;                       // offset 0x1C, size 0x4
    int m_RemapPalette;                     // offset 0x20, size 0x4
    unsigned int m_RemapColours[4];         // offset 0x24, size 0x10
};

struct SkinCompositeParams {
    // total size: 0x50
    struct TextureInfo *DestTexture;   // offset 0x0, size 0x4
    unsigned int BaseColour;           // offset 0x4, size 0x4
    unsigned int SwatchColours[4];     // offset 0x8, size 0x10
    VinylLayerInfo VinylLayerInfos[1]; // offset 0x18, size 0x34
    int NumLayers;                     // offset 0x4C, size 0x4
};

void UpdateSkinCompositeCache(SkinCompositeParams *skin_composite_params);
void FlushFromSkinCompositeCache(unsigned int texture_name_hash);

#endif
