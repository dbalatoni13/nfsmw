#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UITRACKMAPSTREAMER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UITRACKMAPSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Misc/Point.hpp"

struct GRaceParameters;
struct FEMultiImage;

// total size: 0xDC
struct UITrackMapStreamer {
    bool bMapPackLoaded;            // offset 0x0, size 0x1
    bool bMakeSpaceInPoolComplete;  // offset 0x4, size 0x1
    bool bLoadingMap;               // offset 0x8, size 0x1
    bool bUseTrackStreamerMem;      // offset 0xC, size 0x1
    int MemPoolNum;                 // offset 0x10, size 0x4
    GRaceParameters *pCurrentTrack; // offset 0x14, size 0x4
    FEMultiImage *TrackMap;         // offset 0x18, size 0x4
    unsigned int MapHash;           // offset 0x1C, size 0x4
    int RegionUnlock;               // offset 0x20, size 0x4
    tCubic2D ZoomCubic;             // offset 0x24, size 0x58
    tCubic2D PanCubic;              // offset 0x7C, size 0x58
    bool bUsingTrackForAnim;        // offset 0xD4, size 0x1

    static void MakeSpaceInPoolCallbackBridge(int param) {
        reinterpret_cast<UITrackMapStreamer *>(param)->MakeSpaceInPoolCallback();
    };

    UITrackMapStreamer();
    virtual ~UITrackMapStreamer();

    void MakeSpaceInPoolCallback();
    void Init(GRaceParameters *track, FEMultiImage *map, int unused, int region_unlock);
    void UpdateAnimation();
    bool IsZooming();
    float GetZoomFactor();
    void GetPan(bVector2 &pan);
    void ZoomTo(const bVector2 &factor);
    void PanTo(const bVector2 &pos);
    void ZoomToTrack();
    void PanToTrack();
    void SetZoom(const bVector2 &factor);
    void SetPan(const bVector2 &pos);
    void SetZoomSpeed(float sec);
    void SetPanSpeed(float sec);
    void ResetZoom(bool use_track);
    void ResetPan(bool use_track);
    void SetMapPackLoaded();
    void SetMapLoaded(unsigned int texture);

    static void MapLoadCallback(unsigned int texture);
    static void MapPackLoadCallback(unsigned int screenPtr);
    void UpdateMap();
    void CalcBoundsForRace(bVector2 &top_left, bVector2 &bottom_right);
    unsigned int CalcMapTextureHash();
};

#endif
