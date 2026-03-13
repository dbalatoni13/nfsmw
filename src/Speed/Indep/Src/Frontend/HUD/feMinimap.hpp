#ifndef FRONTEND_HUD_FEMINIMAP_H
#define FRONTEND_HUD_FEMINIMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"

struct TrackInfo;

struct COORD2 {
    float x;
    float y;
};

struct MiniMapItem : public bTNode<MiniMapItem> {
    FEImage *mpIcon;
    bVector2 mPos;
    unsigned int mItemType;
    bool mHidden;
};

class Minimap : public HudElement {
  public:
    Minimap(const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetupMinimap(IPlayer *player);
    void RefreshMapItems();
    void AdjustForWidescreen(bool widescreen);
    static void InitStaticMiniMapItems();

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
    FEImage *mGameplayIcons[15][8];
};

#endif
