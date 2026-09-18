#include "uiTrackMapStreamer.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

int eIsStreamingTexturePackLoaded(const char *filename);

// Inlined tCubic1D::tCubic1D(short, float) ctor (Spline.hpp is shared; replicated here).
static inline void TrackMapCubic1DInit(tCubic1D *cubic, short type, float dur) {
    cubic->Val = 0.0f;
    cubic->dVal = 0.0f;
    cubic->ValDesired = 0.0f;
    cubic->dValDesired = 0.0f;
    cubic->time = 0.0f;
    cubic->duration = dur;
    cubic->state = 0;
    cubic->flags = type;
    cubic->Coeff[0] = 0.0f;
    cubic->Coeff[1] = 0.0f;
    cubic->Coeff[2] = 0.0f;
    cubic->Coeff[3] = 0.0f;
}

// Inlined tCubic2D::tCubic2D(short, float) ctor.
static inline void TrackMapCubic2DInit(tCubic2D *cubic, short type, float dur) {
    TrackMapCubic1DInit(&cubic->x, type, dur);
    TrackMapCubic1DInit(&cubic->y, type, dur);
}

// Inlined tCubic1D::SetDuration / tCubic2D::SetDuration.
static inline void TrackMapCubicSetDuration(tCubic2D *cubic, float t) {
    cubic->x.duration = t;
    cubic->y.duration = t;
}

// Inlined tCubic1D::SetFlags / tCubic2D::SetFlags.
static inline void TrackMapCubicSetFlags(tCubic2D *cubic, short s) {
    cubic->x.flags = s;
    cubic->y.flags = s;
}

// Inlined tCubic1D::SetVal / tCubic2D::SetVal.
static inline void TrackMapCubicSetVal(tCubic2D *cubic, const float vx, const float vy) {
    cubic->x.SetVal(vx);
    cubic->y.SetVal(vy);
}

// Inlined tCubic1D::Snap / tCubic2D::Snap (this TU's Point.hpp does not write time).
static inline void TrackMapCubicSnap(tCubic1D *cubic) {
    cubic->Val = cubic->ValDesired;
    cubic->dVal = cubic->dValDesired;
    cubic->state = 0;
}

static inline void TrackMapCubicSnap(tCubic2D *cubic) {
    TrackMapCubicSnap(&cubic->x);
    TrackMapCubicSnap(&cubic->y);
}

#define TRACK_MAP_PACK "TRACKS\\L2RA\\TrackMaps.bin"

static UITrackMapStreamer *pInstance = nullptr;

UITrackMapStreamer::UITrackMapStreamer() {
    bMapPackLoaded = false;
    bLoadingMap = false;
    pCurrentTrack = nullptr;
    TrackMap = nullptr;
    MapHash = 0;
    TrackMapCubic2DInit(&ZoomCubic, 1, 1.0f);
    TrackMapCubic2DInit(&PanCubic, 1, 1.0f);

    bUsingTrackForAnim = true;
    pInstance = this;

    TrackMapCubicSetDuration(&ZoomCubic, 1.0f);
    TrackMapCubicSetDuration(&PanCubic, 1.0f);
    TrackMapCubicSetFlags(&ZoomCubic, 0);
    TrackMapCubicSetFlags(&PanCubic, 0);
    TrackMapCubicSetVal(&ZoomCubic, 1.0f, 1.0f);

    MemPoolNum = 0;

#ifndef EA_BUILD_A124
    bMakeSpaceInPoolComplete = false;
#endif
    bUseTrackStreamerMem = TheGameFlowManager.IsInGame();

    if (bUseTrackStreamerMem) {
        MemPoolNum = 7;
        TheTrackStreamer.DisableZoneSwitching();
        int mem_needed = 6 * 0x10000;

        TheTrackStreamer.MakeSpaceInPool(mem_needed, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int>(this));
    } else {
        eLoadStreamingTexturePack(TRACK_MAP_PACK, MapPackLoadCallback, reinterpret_cast<unsigned int>(this), 0);
    }
}

UITrackMapStreamer::~UITrackMapStreamer() {
    if (bUseTrackStreamerMem) {
#ifndef EA_BUILD_A124
        if (!bMakeSpaceInPoolComplete) {
            TheTrackStreamer.WaitForCurrentLoadingToComplete();
        }
#endif

        TheTrackStreamer.EnableZoneSwitching();
        TheTrackStreamer.RefreshLoading();
    }

    eWaitForStreamingTexturePackLoading(TRACK_MAP_PACK);
    eUnloadStreamingTexture(MapHash);

    eUnloadAllStreamingTextures(TRACK_MAP_PACK);

    if (bMapPackLoaded) {
        eUnloadStreamingTexturePack(TRACK_MAP_PACK);
    }

    pInstance = nullptr;
}

void UITrackMapStreamer::MakeSpaceInPoolCallback() {
#ifndef EA_BUILD_A124
    bMakeSpaceInPoolComplete = true;
#endif
    eLoadStreamingTexturePack(TRACK_MAP_PACK, MapPackLoadCallback, reinterpret_cast<unsigned int>(this), 0);
}

void UITrackMapStreamer::Init(GRaceParameters *track, FEMultiImage *map, int unused, int region_unlock) {
    RegionUnlock = region_unlock;

    pCurrentTrack = track;

    TrackMap = map;

    FEngSetInvisible(reinterpret_cast<FEObject *>(map));

    if (bMapPackLoaded) {
        if (!bLoadingMap) {
            eUnloadStreamingTexture(MapHash);

            eWaitForStreamingTexturePackLoading(TRACK_MAP_PACK);

            MapHash = CalcMapTextureHash();
            eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);

            bLoadingMap = true;
        }
    }
}

void UITrackMapStreamer::MapPackLoadCallback(unsigned int screenPtr) {
    reinterpret_cast<UITrackMapStreamer *>(screenPtr)->SetMapPackLoaded();
}

void UITrackMapStreamer::MapLoadCallback(unsigned int texture) {
    pInstance->SetMapLoaded(texture);
}

unsigned int UITrackMapStreamer::CalcMapTextureHash() {
    unsigned int hash;
    if (pCurrentTrack)
        hash = CalcLanguageHash("TRACK_MAP_", pCurrentTrack);
    else if (RegionUnlock)
        hash = FEngHashString("TRACK_MAP_UNLOCK_%d", RegionUnlock);
    else
        hash = FEHashUpper("TRACK_MAP");
    return hash;
}

void UITrackMapStreamer::SetMapPackLoaded() {
    if (eIsStreamingTexturePackLoaded(TRACK_MAP_PACK)) {
        bMapPackLoaded = true;

        MapHash = CalcMapTextureHash();
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);

        bLoadingMap = true;
    }
}

void UITrackMapStreamer::SetMapLoaded(unsigned int texture) {
    unsigned int hash = CalcMapTextureHash();

    if (hash != texture) {
        eUnloadStreamingTexture(texture);

        MapHash = hash;
        FEngSetInvisible(reinterpret_cast<FEObject *>(TrackMap));
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
    } else {
        bLoadingMap = false;

        FEngSetTextureHash(TrackMap, hash);
        FEngSetVisible(reinterpret_cast<FEObject *>(TrackMap));

        if (bUsingTrackForAnim) {
            ZoomToTrack();
            PanToTrack();
        }
    }
}

void UITrackMapStreamer::UpdateMap() {
    if (TrackMap) {
        bVector2 mapTL(0.0f, 0.0f);
        bVector2 mapBR(0.0f, 0.0f);
        bVector2 zoom;
        bVector2 pan;

        ZoomCubic.GetVal(&zoom);
        PanCubic.GetVal(&pan);

        mapTL.x = pan.x - zoom.x * 0.5f;
        mapTL.y = pan.y - zoom.y * 0.5f;
        mapBR.x = pan.x + zoom.x * 0.5f;
        mapBR.y = pan.y + zoom.y * 0.5f;

        float halfSizeX = (mapBR.x - mapTL.x) * 0.5f;
        float halfSizeY = (mapBR.y - mapTL.y) * 0.5f;
        float halfSize = bMax(halfSizeX, halfSizeY);

        FEVector2 mapCenter(mapTL.x + halfSizeX, mapTL.y + halfSizeY);
        FEVector2 TL(mapCenter.x - halfSize, mapCenter.y - halfSize);
        FEVector2 BR(mapCenter.x + halfSize, mapCenter.y + halfSize);
        TrackMap->SetTopLeft(TL, false);
        TrackMap->SetBottomRight(BR, false);
    }
}

void UITrackMapStreamer::CalcBoundsForRace(bVector2 &top_left, bVector2 &bottom_right) {
    if (pCurrentTrack) {
        UMath::Vector2 topLeftMap;
        UMath::Vector2 botRightMap;

        pCurrentTrack->GetBoundingBox(topLeftMap, botRightMap);

        top_left.x = topLeftMap.x - 0.125f;
        top_left.y = topLeftMap.y + 0.125f;
        bottom_right.x = botRightMap.x + 0.125f;
        bottom_right.y = botRightMap.y - 0.125f;
    }
}

void UITrackMapStreamer::UpdateAnimation() {
    cPoint::SplineSeek(&ZoomCubic, RealTimeElapsed);
    cPoint::SplineSeek(&PanCubic, RealTimeElapsed);

    UpdateMap();
}

float UITrackMapStreamer::GetZoomFactor() {
    bVector2 temp;
    ZoomCubic.GetVal(&temp);
    if (temp.x == 0.0f) {
        return 1.0f;
    } else {
        return 1.0f / temp.x;
    }
}

void UITrackMapStreamer::GetPan(bVector2 &pan) {
    bVector2 center(0.5f, 0.5f);
    PanCubic.GetVal(&pan);
    pan -= center;
}

void UITrackMapStreamer::ZoomTo(const bVector2 &factor) {
    ZoomCubic.SetValDesired(const_cast<bVector2 *>(&factor));
}

void UITrackMapStreamer::PanTo(const bVector2 &pos) {
    PanCubic.SetValDesired(const_cast<bVector2 *>(&pos));
}

void UITrackMapStreamer::ZoomToTrack() {
    bUsingTrackForAnim = true;

    bVector2 mapTL(0.0f, 0.0f);
    bVector2 mapBR(1.0f, 1.0f);

    CalcBoundsForRace(mapTL, mapBR);

    bVector2 zoom_to;
    zoom_to.x = mapBR.x - mapTL.x;
    zoom_to.y = mapTL.y - mapBR.y;
    ZoomTo(zoom_to);
}

void UITrackMapStreamer::PanToTrack() {
    bUsingTrackForAnim = true;

    bVector2 mapTL(0.0f, 0.0f);
    bVector2 mapBR(1.0f, 1.0f);

    CalcBoundsForRace(mapTL, mapBR);

    bVector2 pan_to;
    pan_to.x = (mapTL.x + mapBR.x) * 0.5f;
    pan_to.y = (mapTL.y + mapBR.y) * 0.5f;
    PanTo(pan_to);
}

void UITrackMapStreamer::SetZoom(const bVector2 &factor) {
    ZoomTo(factor);
    TrackMapCubicSnap(&ZoomCubic);
}

void UITrackMapStreamer::SetPan(const bVector2 &pos) {
    PanTo(pos);
    TrackMapCubicSnap(&PanCubic);
}

void UITrackMapStreamer::SetZoomSpeed(float sec) {
    TrackMapCubicSetDuration(&ZoomCubic, sec);
}

void UITrackMapStreamer::SetPanSpeed(float sec) {
    TrackMapCubicSetDuration(&PanCubic, sec);
}

void UITrackMapStreamer::ResetZoom(bool use_track) {
    bUsingTrackForAnim = use_track;
    if (use_track) {
        ZoomToTrack();
        TrackMapCubicSnap(&ZoomCubic);
    } else {
        SetZoom(bVector2(1.0f, 1.0f));
    }
}

void UITrackMapStreamer::ResetPan(bool use_track) {
    bUsingTrackForAnim = use_track;
    if (use_track) {
        PanToTrack();
        TrackMapCubicSnap(&PanCubic);
    } else {
        bVector2 pan(0.5f, 0.5f);
        SetPan(pan);
    }
}
