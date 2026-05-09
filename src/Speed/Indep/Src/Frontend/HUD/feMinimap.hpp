#ifndef FRONTEND_HUD_FEMINIMAP_H
#define FRONTEND_HUD_FEMINIMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct TrackInfo;
struct IVehicle;
struct GIcon;

// TODO probably move to UMath
typedef float Angle;
typedef UMath::Vector2 COORD2;
typedef UMath::Vector3 COORD3;
typedef UMath::Vector4 COORD4;
typedef UMath::Matrix4 MATRIX4;
typedef UMath::Matrix3 MATRIX3;
typedef UMath::Quaternion RQUAT;
typedef unsigned char u_char;
typedef short unsigned int u_short;
typedef unsigned int u_int;
typedef long unsigned int u_long;

class MiniMapItem : public bTNode<MiniMapItem> {
  public:
    static inline void *operator new(size_t size, void *ptr) {}

    static inline void operator delete(void *mem, void *ptr) {}

    static inline void *operator new(size_t size) {}

    static void operator delete(void *mem, size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }
    static inline void *operator new(size_t size, const char *name) {}

    static inline void operator delete(void *mem, const char *name) {}

    static inline void operator delete(void *mem, size_t size, const char *name) {}

    inline void UpdatePos(float x, float y) {}

    FEImage *mpIcon;
    bVector2 mPos;
    uint32 mItemType;
    bool mHidden;
};

class Minimap : public HudElement {
  public:
    struct GameplayIconInfo {
        EAGL4Anim::AnimTypeId::Type mIconType;
        eWorldMapItemType mItemType;
        const char *mElementString;
        uint32 mWorldMapTitle;
        uint32 mworldIconTexHash;
    };

    Minimap(const char *pkg_name, int player_number);
    ~Minimap() override;
    void SetupMinimap(IPlayer *player);
    void RefreshMapItems();
    void Update(IPlayer *player) override;
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
    void AdjustForWidescreen(bool widescreen);
    static GameplayIconInfo &GetGameplayIconInfo(int iconType);
    static GameplayIconInfo &GetGameplayIconInfoByItemType(int itemType);

  private:
    void UpdateMiniMapItems();
    void InitStaticMiniMapItems();
    void UpdateGameplayIcons(IPlayer *player);
    void UpdateTrackMapArt();
    void UpdateElementArt(bVector2 *pos, bVector2 *dir, FEObject *element, bool visible);
    void UpdateCopElements(IVehicle *vehicle);
    void UpdateAiRacerElements();
    void UpdatePlayer2Element();
    void UpdateRaceElements();
    void UpdateIconElement(FEImage *icon, GIcon *gicon);

  private:
    bTList<MiniMapItem> StaticMiniMapItems;
    static const unsigned int mMapPieces; // size: 0x4, address: 0xFFFFFFFF
    FEObject *TrackmapLayout;
    FEMultiImage *TrackmapArt[4];
    FEVector2 TrackmapArtUVs[4][2];
    FEImage *TrackmapNorth;
    FEImage *mPlayerCarIndicator;
    FEImage *mPlayerCarIndicator2;
    TrackInfo *CurrentTrack;
    FEVector3 mMapDefaultPos;
    float mSpeedZoomScale;
    float mPolyRotation;
    bVector2 mTrackTargetNormalized;
    COORD2 mTrackMapCentre;
    int mCopFlashCounter;
    int MinimapRotateWithPlayer;
    static const unsigned int mMaxElements; // size: 0x4, address: 0xFFFFFFFF
    FEObject *mHeliElementArt;
    FEObject *mHeliLineOfSiteArt;
    FEImage *mCopElementArt[8];
    FEImage *mRacerElementArt[8];
    FEImage *mCheckpointElementArt;
    FEImage *mGPSSelectionElementArt;
    FEImage *mGameplayIcons[17][8];
    static GameplayIconInfo kGameplayIconInfo[]; // size: 0x0, address: 0x8041BCA0
};

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *isimable);

#endif
