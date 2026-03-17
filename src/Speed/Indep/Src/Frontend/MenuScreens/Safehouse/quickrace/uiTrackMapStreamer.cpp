// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

struct Vector2 {
    float x;
    float y;
};

struct FEObject;

void FEngSetInvisible(FEObject* obj);
void FEngSetVisible(FEObject* obj);
void FEngSetTextureHash(FEImage* image, unsigned int texture_hash);
unsigned long FEHashUpper(const char* string);
unsigned int FEngHashString(const char* format, ...);
unsigned int CalcLanguageHash(const char* prefix, GRaceParameters* pRaceParams);

void eLoadStreamingTexturePack(const char* filename, void (*callback)(void*), void* param,
                               int memory_pool_num);
void eLoadStreamingTexture(unsigned int* name_hash_table, int num_hashes,
                           void (*callback)(void*), void* param, int memory_pool_num);
void eUnloadStreamingTexture(unsigned int* name_hash_table, int num_hashes);

inline void eLoadStreamingTexture(unsigned int name_hash, void (*callback)(unsigned int),
                                   unsigned int param0, int memory_pool_num) {
    eLoadStreamingTexture(&name_hash, 1, reinterpret_cast< void (*)(void*) >(callback),
                          reinterpret_cast< void* >(param0), memory_pool_num);
}

inline void eUnloadStreamingTexture(unsigned int name_hash) {
    eUnloadStreamingTexture(&name_hash, 1);
}

void eWaitForStreamingTexturePackLoading(const char* filename);
void eUnloadAllStreamingTextures(const char* filename);
void eUnloadStreamingTexturePack(const char* filename);
int eIsStreamingTexturePackLoaded(const char* filename);

extern float RealTimeElapsed;

struct cPoint {
    static void SplineSeek(tCubic2D* p, float time);
};

static UITrackMapStreamer* pInstance;

UITrackMapStreamer::UITrackMapStreamer()
    : bMapPackLoaded(false) //
    , bLoadingMap(false) //
    , pCurrentTrack(nullptr) //
    , TrackMap(nullptr) //
    , MapHash(0) //
    , ZoomCubic(0, 1.0f) //
    , PanCubic(0, 1.0f)
{
    bUsingTrackForAnim = true;
    pInstance = this;

    ZoomCubic.SetDuration(1.0f);
    PanCubic.SetDuration(1.0f);
    ZoomCubic.SetFlags(0);
    PanCubic.SetFlags(0);
    ZoomCubic.SetVal(1.0f, 1.0f);

    bMakeSpaceInPoolComplete = false;
    MemPoolNum = 0;

    bUseTrackStreamerMem = TheGameFlowManager.IsInGame();

    if (bUseTrackStreamerMem) {
        MemPoolNum = 7;
        TheTrackStreamer.DisableZoneSwitching();
        TheTrackStreamer.MakeSpaceInPool(0x60000, MakeSpaceInPoolCallbackBridge,
                                        reinterpret_cast< int >(this));
    } else {
        eLoadStreamingTexturePack(
            "TRACKS\\L2RA\\TrackMaps.bin",
            reinterpret_cast< void (*)(void*) >(MapPackLoadCallback),
            reinterpret_cast< void* >(this), 0);
    }
}

UITrackMapStreamer::~UITrackMapStreamer() {
    if (bUseTrackStreamerMem) {
        if (!bMakeSpaceInPoolComplete) {
            TheTrackStreamer.WaitForCurrentLoadingToComplete();
        }
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
    bMakeSpaceInPoolComplete = true;
    eLoadStreamingTexturePack("TRACKS\\L2RA\\TrackMaps.bin",
                              reinterpret_cast< void (*)(void*) >(MapPackLoadCallback),
                              reinterpret_cast< void* >(this), 0);
}

void UITrackMapStreamer::Init(GRaceParameters* track, FEMultiImage* map, int unused,
                              int region_unlock) {
    RegionUnlock = region_unlock;
    pCurrentTrack = track;
    TrackMap = map;
    FEngSetInvisible(static_cast< FEObject* >(map));
    if (bMapPackLoaded && !bLoadingMap) {
        eUnloadStreamingTexture(MapHash);
        eWaitForStreamingTexturePackLoading("TRACKS\\L2RA\\TrackMaps.bin");
        MapHash = CalcMapTextureHash();
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
        bLoadingMap = true;
    }
}

void UITrackMapStreamer::MapLoadCallback(unsigned int texture) {
    pInstance->SetMapLoaded(texture);
}

unsigned int UITrackMapStreamer::CalcMapTextureHash() {
    if (pCurrentTrack) {
        return CalcLanguageHash("TRACK_MAP_", pCurrentTrack);
    } else if (!RegionUnlock) {
        return FEHashUpper("TRACK_MAP");
    } else {
        return FEngHashString("TRACK_MAP_UNLOCK_%d", RegionUnlock);
    }
}

void UITrackMapStreamer::SetMapPackLoaded() {
    if (eIsStreamingTexturePackLoaded("TRACKS\\L2RA\\TrackMaps.bin")) {
        bMapPackLoaded = true;
        MapHash = CalcMapTextureHash();
        eLoadStreamingTexture(MapHash, MapLoadCallback, MapHash, MemPoolNum);
        bLoadingMap = true;
    }
}

void UITrackMapStreamer::SetMapLoaded(unsigned int texture) {
    unsigned int hash = CalcMapTextureHash();
    if (hash != texture) {
        unsigned int old_texture = texture;
        eUnloadStreamingTexture(&old_texture, 1);
        MapHash = hash;
        FEngSetInvisible(static_cast< FEObject* >(TrackMap));
        unsigned int new_hash = MapHash;
        eLoadStreamingTexture(&new_hash, 1,
                              reinterpret_cast< void (*)(void*) >(MapLoadCallback),
                              reinterpret_cast< void* >(new_hash), MemPoolNum);
    } else {
        bLoadingMap = false;
        FEngSetTextureHash(static_cast< FEImage* >(TrackMap), hash);
        FEngSetVisible(static_cast< FEObject* >(TrackMap));
        if (bUsingTrackForAnim) {
            ZoomToTrack();
            PanToTrack();
        }
    }
}

void UITrackMapStreamer::UpdateMap() {
    if (TrackMap != nullptr) {
        bVector2 mapTL(0.0f, 0.0f);
        bVector2 mapBR(0.0f, 0.0f);
        bVector2 zoom;
        bVector2 pan;
        bVector2* pMapTL = &mapTL;
        bVector2* pMapBR = &mapBR;
        bVector2* pZoom = &zoom;
        bVector2* pPan = &pan;

        ZoomCubic.GetVal(pZoom);
        PanCubic.GetVal(pPan);

        pMapTL->x = pPan->x - pZoom->x * 0.5f;
        pMapTL->y = pPan->y - pZoom->y * 0.5f;
        pMapBR->x = pZoom->x * 0.5f + pPan->x;
        pMapBR->y = pZoom->y * 0.5f + pPan->y;

        float halfSizeX = (pMapBR->x - pMapTL->x) * 0.5f;
        float halfSizeY = (pMapBR->y - pMapTL->y) * 0.5f;
        float halfSize = bMax(halfSizeX, halfSizeY);

        FEVector2 mapCenter(pMapTL->x + halfSizeX, pMapTL->y + halfSizeY);
        FEVector2 TL(mapCenter.x - halfSize, mapCenter.y - halfSize);
        FEVector2 BR(mapCenter.x + halfSize, mapCenter.y + halfSize);

        TrackMap->SetTopLeft(TL, false);
        TrackMap->SetBottomRight(BR, false);
    }
}

void UITrackMapStreamer::CalcBoundsForRace(bVector2& top_left, bVector2& bottom_right) {
    if (pCurrentTrack != nullptr) {
        Vector2 topLeftMap;
        Vector2 botRightMap;
        pCurrentTrack->GetBoundingBox(topLeftMap, botRightMap);
        float margin = 0.125f;
        top_left.x = topLeftMap.x - margin;
        top_left.y = topLeftMap.y + margin;
        bottom_right.x = botRightMap.x + margin;
        bottom_right.y = botRightMap.y - margin;
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
    }
    return 1.0f / temp.x;
}

void UITrackMapStreamer::GetPan(bVector2& pan) {
    bVector2 center(0.5f, 0.5f);
    PanCubic.GetVal(&pan);
    pan -= center;
}

void UITrackMapStreamer::ZoomToTrack() {
    bVector2 mapTL(0.0f, 0.0f);
    bVector2 mapBR(1.0f, 1.0f);
    bUsingTrackForAnim = true;
    CalcBoundsForRace(mapTL, mapBR);
    bVector2 zoom_to(mapBR.x - mapTL.x, mapTL.y - mapBR.y);
    ZoomTo(zoom_to);
}

void UITrackMapStreamer::PanToTrack() {
    bVector2 mapTL;
    bVector2 mapBR;
    bVector2 pan_to;
    bVector2* pMapTL = &mapTL;
    bVector2* pMapBR = &mapBR;
    bVector2* pPanTo = &pan_to;

    bUsingTrackForAnim = true;
    pMapTL->x = 0.0f;
    pMapTL->y = 0.0f;
    pMapBR->x = 1.0f;
    pMapBR->y = 1.0f;
    CalcBoundsForRace(mapTL, mapBR);

    pPanTo->x = (pMapTL->x + pMapBR->x) * 0.5f;
    pPanTo->y = (pMapTL->y + pMapBR->y) * 0.5f;
    PanTo(pan_to);
}

void UITrackMapStreamer::SetZoom(const bVector2& factor) {
    ZoomTo(factor);
    ZoomCubic.Snap();
}

void UITrackMapStreamer::SetPan(const bVector2& pos) {
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

void UITrackMapStreamer::ZoomTo(const bVector2& factor) {
    ZoomCubic.SetValDesired(const_cast< bVector2* >(&factor));
}

void UITrackMapStreamer::PanTo(const bVector2& pos) {
    PanCubic.SetValDesired(const_cast< bVector2* >(&pos));
}

void UITrackMapStreamer::MapPackLoadCallback(unsigned int screenPtr) {
    reinterpret_cast< UITrackMapStreamer* >(screenPtr)->SetMapPackLoaded();
}

void UITrackMapStreamer::MakeSpaceInPoolCallbackBridge(int param) {
    reinterpret_cast< UITrackMapStreamer* >(param)->MakeSpaceInPoolCallback();
}
