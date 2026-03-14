// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UITRACKMAPSTREAMER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UITRACKMAPSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct GRaceParameters;
struct FEMultiImage;

// total size: 0x2C
struct tCubic1D {
    float Val;           // offset 0x0, size 0x4
    float dVal;          // offset 0x4, size 0x4
    float ValDesired;    // offset 0x8, size 0x4
    float dValDesired;   // offset 0xC, size 0x4
    float Coeff[4];      // offset 0x10, size 0x10
    float time;          // offset 0x20, size 0x4
    float duration;      // offset 0x24, size 0x4
    short state;         // offset 0x28, size 0x2
    short flags;         // offset 0x2A, size 0x2

    tCubic1D(short type, float dur)
        : Val(0.0f) //
        , dVal(0.0f) //
        , ValDesired(0.0f) //
        , dValDesired(0.0f) //
        , time(0.0f) //
        , duration(dur) //
        , state(type) //
        , flags(1)
    {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void Snap() {
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
    void SetVal(const float v) { Val = v; state = 2; }
    void SetdVal(float v) { dVal = v; state = 2; }
    void SetValDesired(float v) { ValDesired = v; state = 2; }
    void SetdValDesired(float v) { dValDesired = v; }
    void SetDuration(const float t) { duration = t; }
    void SetState(short s) { state = s; }
    void SetFlags(short f) { flags = f; }
    float GetVal();
    float GetdVal();
    float GetddVal();
    int HasArrived();
    void PathdValDesired(float v);
    void MakeCoeffs();
    float GetVal(float t);
    float GetdVal(float t);
    float GetddVal(float t);
    float GetValDesired();
    float GetdValDesired();
    float GetDerivative(float t);
    float GetSecondDerivative(float t);
    void ClampDerivative(float fMag);
    void ClampSecondDerivative(float fMag);
    void Update(float fSeconds, float fDClamp, float fDDClamp);
};

// total size: 0x58
struct tCubic2D {
    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C

    tCubic2D(short type, float dur) : x(type, dur), y(type, dur) {}
    tCubic2D(short type, bVector2* pDuration);

    int HasArrived();
    void Snap() {
        x.Snap();
        y.Snap();
    }
    void SetVal(const float vx, const float vy) { x.SetVal(vx); y.SetVal(vy); }
    void SetdVal(float vx, float vy) { x.SetdVal(vx); y.SetdVal(vy); }
    void SetValDesired(float vx, float vy) { x.SetValDesired(vx); y.SetValDesired(vy); }
    void SetdValDesired(float vx, float vy) { x.SetdValDesired(vx); y.SetdValDesired(vy); }
    void SetDuration(const float t) {
        x.SetDuration(t);
        y.SetDuration(t);
    }
    void SetDuration(const float tx, const float ty);
    void SetState(short s) { x.SetState(s); y.SetState(s); }
    void SetFlags(short s) { x.SetFlags(s); y.SetFlags(s); }
    void PathdValDesired(float x2, float y2);
    void PathdValDesired(bVector2* v);
    void MakeCoeffs();

    void SetVal(const bVector2* pV);
    void SetdVal(bVector2* pV);
    void SetValDesired(bVector2* pV);
    void SetdValDesired(bVector2* pV);
    void SetDuration(const bVector2* pV);
    void GetVal(bVector2* pV);
    void GetdVal(bVector2* pV);
    void GetddVal(bVector2* pV);
    void GetVal(bVector2* pV, float t);
    void GetdVal(bVector2* pV, float t);
    void GetddVal(bVector2* pV, float t);
    void GetValDesired(bVector2* pV);
    void GetdValDesired(bVector2* pV);
    void Update(float fSeconds, float fDClamp, float fDDClamp);
};

// total size: 0xDC
struct UITrackMapStreamer {
    bool bMapPackLoaded;          // offset 0x0, size 0x1
    bool bMakeSpaceInPoolComplete; // offset 0x4, size 0x1
    bool bLoadingMap;             // offset 0x8, size 0x1
    bool bUseTrackStreamerMem;    // offset 0xC, size 0x1
    int MemPoolNum;               // offset 0x10, size 0x4
    GRaceParameters* pCurrentTrack; // offset 0x14, size 0x4
    FEMultiImage* TrackMap;       // offset 0x18, size 0x4
    unsigned int MapHash;         // offset 0x1C, size 0x4
    int RegionUnlock;             // offset 0x20, size 0x4
    tCubic2D ZoomCubic;           // offset 0x24, size 0x58
    tCubic2D PanCubic;            // offset 0x7C, size 0x58
    bool bUsingTrackForAnim;      // offset 0xD4, size 0x1
    // vtable at 0xD8

    static void MakeSpaceInPoolCallbackBridge(int param);

    UITrackMapStreamer();
    virtual ~UITrackMapStreamer();

    void MakeSpaceInPoolCallback();
    void Init(GRaceParameters* track, FEMultiImage* map, int unused, int region_unlock);
    void UpdateAnimation();
    bool IsZooming();
    float GetZoomFactor();
    void GetPan(bVector2& pan);
    void ZoomTo(const bVector2& factor);
    void PanTo(const bVector2& pos);
    void ZoomToTrack();
    void PanToTrack();
    void SetZoom(const bVector2& factor);
    void SetPan(const bVector2& pos);
    void SetZoomSpeed(float sec);
    void SetPanSpeed(float sec);
    void ResetZoom(bool use_track);
    void ResetPan(bool use_track);
    void SetMapPackLoaded();
    void SetMapLoaded(unsigned int texture);

    static void MapLoadCallback(unsigned int texture);
    static void MapPackLoadCallback(unsigned int screenPtr);
    void UpdateMap();
    void CalcBoundsForRace(bVector2& top_left, bVector2& bottom_right);
    unsigned int CalcMapTextureHash();
};

#endif
