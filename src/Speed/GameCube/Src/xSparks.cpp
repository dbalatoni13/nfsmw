#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

struct XenonEffectDef {
    // total size: 0x58
    UMath::Vector4 vel;                 // offset 0x0, size 0x10
    UMath::Matrix4 mat;                 // offset 0x10, size 0x40
    Attrib::Collection *spec;           // offset 0x50, size 0x4
    EmitterGroup *piggyback_effect;     // offset 0x54, size 0x4
};

struct XenonEffectVec {
    XenonEffectDef *start;          // offset 0x0, size 0x4
    XenonEffectDef *finish;         // offset 0x4, size 0x4
    void *unused;                   // offset 0x8, size 0x4
    XenonEffectDef *end_of_storage; // offset 0xC, size 0x4
};

struct XenonEffectLists {
    XenonEffectVec active;  // offset 0x0, size 0x10
    XenonEffectVec staging; // offset 0x10, size 0x10
};

extern XenonEffectLists gNGEffectList;

void reserveXenonEffectVec(void *vec, unsigned int count)
    __asm__("reserve__Q24_STLt6vector2Z14XenonEffectDefZQ33UTL3Stdt9Allocator2Z14XenonEffectDefZ20_type_XenonEffectDefUi");

void DrawXenonEmitters(eView *view) {}

void ClearXenonEmitters() {
    gNGEffectList.active.finish = gNGEffectList.active.start;
    gNGEffectList.staging.finish = gNGEffectList.staging.start;
}

void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const UMath::Matrix4 *mat, const UMath::Vector4 *vel) {
    unsigned int size = gNGEffectList.active.finish - gNGEffectList.active.start;
    if (size < 20) {
        unsigned int active_capacity = gNGEffectList.active.end_of_storage - gNGEffectList.active.start;
        if (active_capacity < 20) {
            reserveXenonEffectVec(&gNGEffectList.active, 20);
        }

        unsigned int staging_capacity = gNGEffectList.staging.end_of_storage - gNGEffectList.staging.start;
        if (staging_capacity < 20) {
            reserveXenonEffectVec(&gNGEffectList.staging, 20);
        }

        XenonEffectDef effect;
        effect.mat = UMath::Matrix4::kIdentity;
        effect.mat.v3 = mat->v3;
        effect.spec = const_cast<Attrib::Collection *>(spec);
        effect.piggyback_effect = piggyback_fx;
        effect.vel = *vel;

        *gNGEffectList.active.finish = effect;
        ++gNGEffectList.active.finish;
    }
}
