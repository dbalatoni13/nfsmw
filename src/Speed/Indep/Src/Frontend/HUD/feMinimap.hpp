#ifndef FRONTEND_HUD_FEMINIMAP_H
#define FRONTEND_HUD_FEMINIMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"

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
    static void InitStaticMiniMapItems();

  private:
    bTList<MiniMapItem> StaticMiniMapItems;
    FEObject *TrackmapLayout;
    FEMultiImage *TrackmapArt[4];
    FEVector2 TrackmapArtUVs[2][4];
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
    FEImage *mGameplayIcons[8][17];
};

#endif
