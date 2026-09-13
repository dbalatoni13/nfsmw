#ifndef FEMINIMAPWRAPPER_H
#define FEMINIMAPWRAPPER_H

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

// total size: 0x1C
// Decl: 25
class MiniMapItem : public bTNode<MiniMapItem> {
  public:
    MiniMapItem(FEImage *icon, bVector2 &pos, uint32 item_type, bool hidden) {} // Decl: 28

    USE_FASTALLOC(MiniMapItem)

    void UpdatePos(float x, float y) {} // Decl: 33

    FEImage *pIcon;        // offset 0x8, size 0x4, Decl: 36
    bVector2 ItemPosition; // offset 0xC, size 0x8, Decl: 37
    uint32 ItemType;       // offset 0x14, size 0x4, Decl: 38
    bool bHidden;          // offset 0x18, size 0x1, Decl: 39
};

// total size: 0x330
// Decl: 47
class Minimap : public HudElement {
  public:
    Minimap(const char *pkg_name, int player_number);
    ~Minimap() override;

    void SetupMinimap(IPlayer *player);

    void RefreshMapItems();

    void Update(IPlayer *player) override;

    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);

    void AdjustForWidescreen(bool moveOutwards);

    struct GameplayIconInfo {        // Decl: 63
        GIcon::Type mIconType;       // offset 0x0, size 0x4, Decl: 63
        eWorldMapItemType mItemType; // offset 0x4, size 0x4, Decl: 63
        const char *mElementString;  // offset 0x8, size 0x4, Decl: 63
        uint32 mWorldMapTitle;       // offset 0xC, size 0x4, Decl: 63
        uint32 mworldIconTexHash;    // offset 0x10, size 0x4, Decl: 63
    }; // Decl: 63

    static GameplayIconInfo &GetGameplayIconInfo(GIcon::Type iconType) { // Decl: 70
        return kGameplayIconInfo[iconType];
    }
    static GameplayIconInfo &GetGameplayIconInfo(eWorldMapItemType itemType) { // Decl: 71
        for (int i = 0; i < GIcon::kType_Count; i++) {
            if (kGameplayIconInfo[i].mItemType == itemType) {
                return kGameplayIconInfo[i];
            }
        }
        return kGameplayIconInfo[0];
    }

  private:
    void UpdateMiniMapItems();

    void InitStaticMiniMapItems();

    void UpdateGameplayIcons(IPlayer *player);

    void UpdateTrackMapArt();
    void UpdateElementArt(bVector2 *elementPos, bVector2 *elementDir, FEObject *elementArt, bool pulse);

    void UpdateCopElements(IVehicle *ivehicle);
    void UpdateAiRacerElements();
    void UpdatePlayer2Element();
    void UpdateRaceElements();
    void UpdateTriggerElement(FEImage *image, GTrigger *trigger);
    void UpdateIconElement(FEImage *image, GIcon *icon);

  private:
    bTList<MiniMapItem> StaticMiniMapItems; // offset 0x28, size 0x8, Decl: 92

    static const uint32 mMapPieces = 4; // size: 0x4, address: 0xFFFFFFFF, Decl: 94

    FEObject *TrackmapLayout;                // offset 0x30, size 0x4, Decl: 96
    FEMultiImage *TrackmapArt[mMapPieces];   // offset 0x34, size 0x10, Decl: 97
    FEVector2 TrackmapArtUVs[mMapPieces][2]; // offset 0x44, size 0x40, Decl: 98
    FEImage *TrackmapNorth;                  // offset 0x84, size 0x4, Decl: 99
    FEImage *mPlayerCarIndicator;            // offset 0x88, size 0x4, Decl: 102
    FEImage *mPlayerCarIndicator2;           // offset 0x8C, size 0x4, Decl: 103
    TrackInfo *CurrentTrack;                 // offset 0x90, size 0x4
    FEVector3 mMapDefaultPos;                // offset 0x94, size 0xC
    float mSpeedZoomScale;                   // offset 0xA0, size 0x4, Decl: 122
    float mPolyRotation;                     // offset 0xA4, size 0x4, Decl: 124
    bVector2 mTrackTargetNormalized;         // offset 0xA8, size 0x8, Decl: 126
    COORD2 mTrackMapCentre;                  // offset 0xB0, size 0x8, Decl: 127
    int mCopFlashCounter;                    // offset 0xB8, size 0x4, Decl: 129
    int MinimapRotateWithPlayer;             // offset 0xBC, size 0x4, Decl: 130
    static const uint32 mMaxElements = 8;    // size: 0x4, address: 0xFFFFFFFF
    FEObject *mHeliElementArt;               // offset 0xC0, size 0x4
    FEObject *mHeliLineOfSiteArt;            // offset 0xC4, size 0x4
    FEImage *mCopElementArt[mMaxElements];   // offset 0xC8, size 0x20
    FEImage *mRacerElementArt[mMaxElements]; // offset 0xE8, size 0x20
    FEImage *mCheckpointElementArt;          // offset 0x108, size 0x4
    FEImage *mGPSSelectionElementArt;        // offset 0x10C, size 0x4, Decl: 137
    FEImage *mGameplayIcons[
#ifdef EA_BUILD_A124
        15
#else
        17
#endif
    ][mMaxElements];                             // offset 0x110, size 0x220
    static GameplayIconInfo kGameplayIconInfo[]; // size: 0x0, address: 0x8041BCA0, Decl: 69
};

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *isimable);

#endif
