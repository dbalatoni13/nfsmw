#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static UITrackMapStreamer *pInstance = nullptr;

// UNSOLVED
UITrackMapStreamer::UITrackMapStreamer()
    : bMapPackLoaded(false),  //
      bLoadingMap(false),     //
      pCurrentTrack(nullptr), //
      TrackMap(nullptr),      //
      MapHash(0),             //
      ZoomCubic(0, 1.0f),     //
      PanCubic(0, 1.0f) {
    bUsingTrackForAnim = true;
    pInstance = this;

    ZoomCubic.SetDuration(1.0f);
    PanCubic.SetDuration(1.0f);
    ZoomCubic.SetFlags(0);
    PanCubic.SetFlags(0);
    ZoomCubic.SetVal(1.0f, 1.0f);

    MemPoolNum = 0;
#ifndef EA_BUILD_A124
    bMakeSpaceInPoolComplete = false;
#endif

    bUseTrackStreamerMem = TheGameFlowManager.IsInGame();

    if (bUseTrackStreamerMem) {
        int mem_needed = 0x60000;

        MemPoolNum = 7;
        TheTrackStreamer.DisableZoneSwitching();
        TheTrackStreamer.MakeSpaceInPool(mem_needed, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int32>(this));
    } else {
        eLoadStreamingTexturePack("TRACKS\\L2RA\\TrackMaps.bin", MapPackLoadCallback, reinterpret_cast<uint32>(this), BMEMORY_DEFAULT_POOL);
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
    eWaitForStreamingTexturePackLoading("TRACKS\\L2RA\\TrackMaps.bin");
    eUnloadStreamingTexture(MapHash);
    eUnloadAllStreamingTextures("TRACKS\\L2RA\\TrackMaps.bin");
    if (bMapPackLoaded) {
        eUnloadStreamingTexturePack("TRACKS\\L2RA\\TrackMaps.bin");
    }
    pInstance = nullptr;
}

void UITrackMapStreamer::MakeSpaceInPoolCallback() {
#ifndef EA_BUILD_A124
    bMakeSpaceInPoolComplete = true;
#endif
    eLoadStreamingTexturePack("TRACKS\\L2RA\\TrackMaps.bin", MapPackLoadCallback, reinterpret_cast<uint32>(this), BMEMORY_DEFAULT_POOL);
}

void UITrackMapStreamer::Init(GRaceParameters *track, FEMultiImage *map, int unused, int region_unlock) {
    RegionUnlock = region_unlock;
    pCurrentTrack = track;
    TrackMap = map;
    FEngSetInvisible(map);
    if (bMapPackLoaded && !bLoadingMap) {
        eUnloadStreamingTexture(MapHash);
        eWaitForStreamingTexturePackLoading("TRACKS\\L2RA\\TrackMaps.bin");
        MapHash = CalcMapTextureHash();
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
        bLoadingMap = true;
    }
}

void UITrackMapStreamer::MapPackLoadCallback(uint32 screenPtr) {
    reinterpret_cast<UITrackMapStreamer *>(screenPtr)->SetMapPackLoaded();
}

void UITrackMapStreamer::MapLoadCallback(uint32 texture) {
    pInstance->SetMapLoaded(texture);
}

uint32 UITrackMapStreamer::CalcMapTextureHash() {
    uint32 hash;

    if (pCurrentTrack != nullptr) {
        hash = CalcLanguageHash("TRACK_MAP_", pCurrentTrack);
    } else {
        if (RegionUnlock) {
            hash = FEngHashString("TRACK_MAP_UNLOCK_%d", RegionUnlock);
        } else {
            hash = FEHashUpper("TRACK_MAP");
        }
    }

    return hash;
}

void UITrackMapStreamer::SetMapPackLoaded() {
    if (eIsStreamingTexturePackLoaded("TRACKS\\L2RA\\TrackMaps.bin")) {
        bMapPackLoaded = true;
        MapHash = CalcMapTextureHash();
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
        bLoadingMap = true;
    }
}

void UITrackMapStreamer::SetMapLoaded(uint32 texture) {
    uint32 hash = CalcMapTextureHash();
    if (hash != texture) {
        eUnloadStreamingTexture(texture);
        MapHash = hash;
        FEngSetInvisible(TrackMap);
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
    } else {
        bLoadingMap = false;
        FEngSetTextureHash(TrackMap, hash);
        FEngSetVisible(TrackMap);
        if (bUsingTrackForAnim) {
            ZoomToTrack();
            PanToTrack();
        }
    }
}

void UITrackMapStreamer::UpdateMap() {
    if (TrackMap == nullptr) {
        return;
    }

    bVector2 mapTL(0.0f, 0.0f);
    bVector2 mapBR(0.0f, 0.0f);

    bVector2 zoom;
    ZoomCubic.GetVal(&zoom);
    bVector2 pan;
    PanCubic.GetVal(&pan);

    mapTL.x = pan.x - (zoom.x / 2);
    mapTL.y = pan.y - (zoom.y / 2);

    mapBR.x = pan.x + (zoom.x / 2);
    mapBR.y = pan.y + (zoom.y / 2);

    float halfSizeX = (mapBR.x - mapTL.x) / 2;
    float halfSizeY = (mapBR.y - mapTL.y) / 2;
    float halfSize = bMax(halfSizeX, halfSizeY);

    FEVector2 mapCenter(mapTL.x + halfSizeX, mapTL.y + halfSizeY);
    FEVector2 TL(mapCenter.x - halfSize, mapCenter.y - halfSize);
    FEVector2 BR(mapCenter.x + halfSize, mapCenter.y + halfSize);

    TrackMap->SetTopLeft(TL, false);
    TrackMap->SetBottomRight(BR, false);
}

void UITrackMapStreamer::CalcBoundsForRace(bVector2 &top_left, bVector2 &bottom_right) {
    if (pCurrentTrack == nullptr) {
        return;
    }
    UMath::Vector2 topLeftMap;
    UMath::Vector2 botRightMap;
    pCurrentTrack->GetBoundingBox(topLeftMap, botRightMap);
    top_left.x = topLeftMap.x - 0.125f;
    top_left.y = topLeftMap.y + 0.125f;
    bottom_right.x = botRightMap.x + 0.125f;
    bottom_right.y = botRightMap.y - 0.125f;
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
    }
    return 1.0f / temp.x;
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
    ZoomCubic.Snap();
}

void UITrackMapStreamer::SetPan(const bVector2 &pos) {
    PanTo(pos);
    PanCubic.Snap();
}

void UITrackMapStreamer::SetZoomSpeed(float sec) {
    ZoomCubic.SetDuration(sec);
}

void UITrackMapStreamer::SetPanSpeed(float sec) {
    PanCubic.SetDuration(sec);
}

void UITrackMapStreamer::ResetZoom(bool use_track) {
    bUsingTrackForAnim = use_track;
    if (use_track) {
        ZoomToTrack();
        ZoomCubic.Snap();
    } else {
        SetZoom(bVector2(1.0f, 1.0f));
    }
}

void UITrackMapStreamer::ResetPan(bool use_track) {
    bUsingTrackForAnim = use_track;
    if (use_track) {
        PanToTrack();
        PanCubic.Snap();
    } else {
        bVector2 pan(0.5f, 0.5f);
        SetPan(pan);
    }
}
