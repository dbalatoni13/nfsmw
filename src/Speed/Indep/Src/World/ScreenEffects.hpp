#ifndef WORLD_SCREEN_EFFECTS_H
#define WORLD_SCREEN_EFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

enum ScreenEffectType {
    SE_TINT = 0,
    SE_MOTION_BLUR = 1,
    SE_VISUAL_SIG = 2,
    SE_GLARE = 3,
    SE_FE_BLUR = 4,
    SE_NUM_TYPES = 5,
};

enum ScreenEffectControl {
    SEC_FUNCTION = 2,
    SEC_BOOLEAN = 1,
    SEC_FRAME = 0,
};

enum ScreenEffectPalette {
    EFX_CAMERA_FLASH = 0,
    EFX_TUNNEL = 1,
    EFX_UNIQUE = 2,
    EFX_NUMBER = 3,
};

// total size: 0xC
struct ScreenEffectInf {
    ScreenEffectControl Controller; // offset 0x0, size 0x4
    unsigned int frameNum;          // offset 0x4, size 0x4
    unsigned int active;            // offset 0x8, size 0x4
};

// total size: 0x50
struct ScreenEffectDef {
    float r;         // offset 0x0, size 0x4
    float g;         // offset 0x4, size 0x4
    float b;         // offset 0x8, size 0x4
    float a;         // offset 0xC, size 0x4
    float intensity; // offset 0x10, size 0x4
    float data[14];  // offset 0x14, size 0x38
    void (*UpdateFnc)(ScreenEffectType,
                      class ScreenEffectDB *); // offset 0x4C, size 0x4
};

// total size: 0x10C
struct ScreenEffectPaletteDef {
    int NumEffects;                       // offset 0x0, size 0x4
    ScreenEffectType SE_type[3];          // offset 0x4, size 0xC
    ScreenEffectDef SE_Def[3];            // offset 0x10, size 0xF0
    ScreenEffectControl SE_Controller[3]; // offset 0x100, size 0xC
};

// total size: 0x1E8
class ScreenEffectDB {
  public:
    ScreenEffectDB();
    void Update(float deltatime);
    void AddScreenEffect(ScreenEffectType type, float intensity, float r, float g, float b);
    void AddScreenEffect(ScreenEffectType type, ScreenEffectDef *info, unsigned int lock, ScreenEffectControl controller);
    void AddPaletteEffect(ScreenEffectPalette palette);
    void AddPaletteEffect(ScreenEffectPaletteDef *palette);

    unsigned int IsActive(ScreenEffectType type) {
        return SE_inf[type].active;
    }

    void RemoveScreenEffect(ScreenEffectType type) {
        SE_inf[type].active = 0;
        numType[type] = 0;
    }

    ScreenEffectControl GetController(ScreenEffectType type) {
        return SE_inf[type].Controller;
    }

    void SetController(ScreenEffectType type, ScreenEffectControl SEC) {
        SE_inf[type].Controller = SEC;
    }

    void SetMyView(eView *view) {
        MyView = view;
    }

    float GetIntensity(ScreenEffectType type) {
        return SE_data[type].intensity;
    }

    float GetDATA(ScreenEffectType type, int index) {
        return SE_data[type].data[index];
    }

    void SetDATA(ScreenEffectType type, float data, int index) {
        SE_data[type].data[index] = data;
    }

  private:
    eView *MyView;              // offset 0x0, size 0x4
    ScreenEffectInf SE_inf[5];  // offset 0x4, size 0x3C
    ScreenEffectDef SE_data[5]; // offset 0x40, size 0x190
    unsigned int numType[5];    // offset 0x1D0, size 0x14
    float SE_time;              // offset 0x1E4, size 0x4
};

void TickSFX();
void DoTunnelBloom(eView *view);
void DoTinting(eView *view);
void UpdateAllScreenEFX();
void FlushAccumulationBuffer();
void AccumulationBufferFlushed();
unsigned int QueryFlushAccumulationBuffer();

#endif
