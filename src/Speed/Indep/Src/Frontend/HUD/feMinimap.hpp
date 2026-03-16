#ifndef FRONTEND_HUD_FEMINIMAP_H
#define FRONTEND_HUD_FEMINIMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

struct TrackInfo;
struct IVehicle;
struct GIcon;

struct COORD2 {
    float x;
    float y;
};

struct MiniMapItem : public bTNode<MiniMapItem> {
    FEImage *mpIcon;
    bVector2 mPos;
    unsigned int mItemType;
    bool mHidden;

    static void operator delete(void *mem, unsigned int size) {
        gFastMem.Free(mem, size, nullptr);
    }
};

class Minimap : public HudElement {
  public:
    Minimap(const char *pkg_name, int player_number);
    ~Minimap();
    void Update(IPlayer *player) override;
    void SetupMinimap(IPlayer *player);
    void RefreshMapItems();
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
    void UpdateTrackMapArt();
    void UpdateElementArt(bVector2 *pos, bVector2 *dir, FEObject *element, bool visible);
    void UpdateCopElements(IVehicle *vehicle);
    void UpdateAiRacerElements();
    void UpdatePlayer2Element();
    void UpdateIconElement(FEImage *icon, GIcon *gicon);
    void UpdateRaceElements();
    void UpdateMiniMapItems();
    void UpdateGameplayIcons(IPlayer *player);
    void AdjustForWidescreen(bool widescreen);
    void InitStaticMiniMapItems();

    struct GameplayIconInfo {
        int mIconType;
        int mItemType;
        const char *mElementString;
        unsigned int mWorldMapTitle;
        unsigned int mworldIconTexHash;
    };
    static GameplayIconInfo kGameplayIconInfo[];
    static GameplayIconInfo &GetGameplayIconInfo(int iconType);
    static GameplayIconInfo &GetGameplayIconInfoByItemType(int itemType);

  private:
    bTList<MiniMapItem> StaticMiniMapItems;
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
    FEObject *mHeliElementArt;
    FEObject *mHeliLineOfSiteArt;
    FEImage *mCopElementArt[8];
    FEImage *mRacerElementArt[8];
    FEImage *mCheckpointElementArt;
    FEImage *mGPSSelectionElementArt;
    FEImage *mGameplayIcons[17][8];
};

#endif
