#ifndef CAMERA_MOVERS_CUBICTWEAKS_H
#define CAMERA_MOVERS_CUBICTWEAKS_H

#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0xB0
struct CubicPovData {
    bVector3 *GetUpAccel() {
        return (bVector3 *)vUpAccel;
    }

    bVector3 *GetUpAccelMin() {
        return (bVector3 *)vUpAccelMin;
    }

    bVector3 *GetUpAccelMax() {
        return (bVector3 *)vUpAccelMax;
    }

    bVector3 *GetEyeAccel() {
        return (bVector3 *)vEyeAccel;
    }

    bVector3 *GetEyeAccelMin() {
        return (bVector3 *)vEyeAccelMin;
    }

    bVector3 *GetEyeAccelMax() {
        return (bVector3 *)vEyeAccelMax;
    }

    bVector3 *GetLookAccel() {
        return (bVector3 *)vLookAccel;
    }

    bVector3 *GetLookAccelMin() {
        return (bVector3 *)vLookAccelMin;
    }

    bVector3 *GetLookAccelMax() {
        return (bVector3 *)vLookAccelMax;
    }

    bVector3 *GetForwardDuration() {
        return (bVector3 *)vForwardDuration;
    }

    float fEyeDuration;         // offset 0x0, size 0x4
    float fLookDuration;        // offset 0x4, size 0x4
    float fFovDuration;         // offset 0x8, size 0x4
    float fUpDuration;          // offset 0xC, size 0x4
    float vUpAccel[4];          // offset 0x10, size 0x10
    float vUpAccelMin[4];       // offset 0x20, size 0x10
    float vUpAccelMax[4];       // offset 0x30, size 0x10
    float vEyeAccel[4];         // offset 0x40, size 0x10
    float vEyeAccelMin[4];      // offset 0x50, size 0x10
    float vEyeAccelMax[4];      // offset 0x60, size 0x10
    float vLookAccel[4];        // offset 0x70, size 0x10
    float vLookAccelMin[4];     // offset 0x80, size 0x10
    float vLookAccelMax[4];     // offset 0x90, size 0x10
    float vForwardDuration[4];  // offset 0xA0, size 0x10
};

template <> void tTable<CubicPovData>::Blend(CubicPovData *dest, CubicPovData *a, CubicPovData *b, float blend_a) {
    float blend_b = 1.0f - blend_a;

    float f1 = blend_a * a->fEyeDuration + blend_b * b->fEyeDuration;
    float f2 = blend_a * a->fLookDuration + blend_b * b->fLookDuration;
    float f3 = blend_a * a->fFovDuration + blend_b * b->fFovDuration;
    float f4 = blend_a * a->fUpDuration + blend_b * b->fUpDuration;

    dest->fEyeDuration = f1;
    dest->fLookDuration = f2;
    dest->fFovDuration = f3;
    dest->fUpDuration = f4;

    bVector3 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9;

    bScale(&v0, a->GetUpAccel(), blend_a);
    bScale(&v1, a->GetUpAccelMin(), blend_a);
    bScale(&v2, a->GetUpAccelMax(), blend_a);
    bScale(&v3, a->GetEyeAccel(), blend_a);
    bScale(&v4, a->GetEyeAccelMin(), blend_a);
    bScale(&v5, a->GetEyeAccelMax(), blend_a);
    bScale(&v6, a->GetLookAccel(), blend_a);
    bScale(&v7, a->GetLookAccelMin(), blend_a);
    bScale(&v8, a->GetLookAccelMax(), blend_a);
    bScale(&v9, a->GetForwardDuration(), blend_a);

    bScaleAdd(dest->GetUpAccel(), &v0, b->GetUpAccel(), blend_b);
    bScaleAdd(dest->GetUpAccelMin(), &v1, b->GetUpAccelMin(), blend_b);
    bScaleAdd(dest->GetUpAccelMax(), &v2, b->GetUpAccelMax(), blend_b);
    bScaleAdd(dest->GetEyeAccel(), &v3, b->GetEyeAccel(), blend_b);
    bScaleAdd(dest->GetEyeAccelMin(), &v4, b->GetEyeAccelMin(), blend_b);
    bScaleAdd(dest->GetEyeAccelMax(), &v5, b->GetEyeAccelMax(), blend_b);
    bScaleAdd(dest->GetLookAccel(), &v6, b->GetLookAccel(), blend_b);
    bScaleAdd(dest->GetLookAccelMin(), &v7, b->GetLookAccelMin(), blend_b);
    bScaleAdd(dest->GetLookAccelMax(), &v8, b->GetLookAccelMax(), blend_b);
    bScaleAdd(dest->GetForwardDuration(), &v9, b->GetForwardDuration(), blend_b);
}

#endif
