#ifndef FRONTEND_HUD_FEMINIMAP_H
#define FRONTEND_HUD_FEMINIMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/uiWorldMap.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct GTrigger;
struct GIcon;
class IVehicle;
struct FEObject;
struct FEImage;
struct FEMultiImage;

// total size: 0x1C
struct MiniMapItem : public bTNode<MiniMapItem> {
    USE_FASTALLOC(MiniMapItem);

    MiniMapItem(FEImage *icon, bVector2 &pos, unsigned int item_type, bool hidden) {
        pIcon = icon;
        ItemPosition = pos;
        ItemType = item_type;
        bHidden = hidden;
    }

    void UpdatePos(float x, float y) {
        ItemPosition.x = x;
        ItemPosition.y = y;
    }

    FEImage *pIcon;         // offset 0x8, size 0x4
    bVector2 ItemPosition;  // offset 0xC, size 0x8
    unsigned int ItemType;  // offset 0x14, size 0x4
    bool bHidden;           // offset 0x18, size 0x1
};

// total size: 0x330
struct Minimap : public HudElement {
    enum Type {
        GII_NONE = 0,
        GII_SPRINT = 1,
        GII_CIRCUIT = 2,
        GII_DRAG = 3,
        GII_KNOCKOUT = 4,
        GII_TOLLBOOTH = 5,
        GII_SPEEDTRAP = 6,
        GII_RIVAL = 7,
        GII_SAFEHOUSE = 8,
        GII_CARLOT = 9,
        GII_SHOP = 10,
        GII_HIDINGSPOT = 11,
        GII_PURSUITSBREAKER = 12,
        GII_SPEEDTRAPCAM = 13,
        GII_SPEEDTRAPRACECAM = 14,
        GII_CHECKPOINT = 15,
        GII_MAX = 16,
    };

    // total size: 0x14
    struct GameplayIconInfo {
        Type mIconType;                // offset 0x0, size 0x4
        eWorldMapItemType mItemType;   // offset 0x4, size 0x4
        const char *mElementString;    // offset 0x8, size 0x4
        unsigned int mWorldMapTitle;   // offset 0xC, size 0x4
        unsigned int mworldIconTexHash; // offset 0x10, size 0x4
    };

    static GameplayIconInfo kGameplayIconInfo[];

    static GameplayIconInfo &GetGameplayIconInfo(Type iconType) {
        return kGameplayIconInfo[iconType];
    }

    static GameplayIconInfo &GetGameplayIconInfo(eWorldMapItemType itemType) {
        for (int i = 0; i <= GII_MAX; i++) {
            if (kGameplayIconInfo[i].mItemType == itemType) {
                return kGameplayIconInfo[i];
            }
        }
        return kGameplayIconInfo[0];
    }

    Minimap(const char *pkg_name, int player_number);
    ~Minimap() override;

    void SetupMinimap(IPlayer *player);
    void RefreshMapItems();
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
    void Update(IPlayer *player) override;
    void UpdateTrackMapArt();
    void UpdateElementArt(bVector2 *elementPos, bVector2 *elementDir, FEObject *elementArt, bool pulse);
    void UpdateCopElements(IVehicle *ivehicle);
    void UpdateAiRacerElements();
    void UpdatePlayer2Element();
    void UpdateTriggerElement(FEImage *image, GTrigger *trigger);
    void UpdateIconElement(FEImage *image, GIcon *icon);
    void UpdateRaceElements();
    void AdjustForWidescreen(bool moveOutwards);
    void UpdateMiniMapItems();
    void InitStaticMiniMapItems();
    void UpdateGameplayIcons(IPlayer *player);

    static const unsigned int mMapPieces;
    static const unsigned int mMaxElements;

    bTList<MiniMapItem> StaticMiniMapItems; // offset 0x28, size 0x8
    FEObject *TrackmapLayout;               // offset 0x30, size 0x4
    FEMultiImage *TrackmapArt[4];           // offset 0x34, size 0x10
    FEVector2 TrackmapArtUVs[4][2];         // offset 0x44, size 0x40
    FEImage *TrackmapNorth;                 // offset 0x84, size 0x4
    FEImage *mPlayerCarIndicator;           // offset 0x88, size 0x4
    FEImage *mPlayerCarIndicator2;          // offset 0x8C, size 0x4
    TrackInfo *CurrentTrack;                // offset 0x90, size 0x4
    FEVector3 mMapDefaultPos;               // offset 0x94, size 0xC
    float mSpeedZoomScale;                  // offset 0xA0, size 0x4
    float mPolyRotation;                    // offset 0xA4, size 0x4
    bVector2 mTrackTargetNormalized;        // offset 0xA8, size 0x8
    UMath::Vector2 mTrackMapCentre;         // offset 0xB0, size 0x8
    int mCopFlashCounter;                   // offset 0xB8, size 0x4
    int MinimapRotateWithPlayer;            // offset 0xBC, size 0x4
    FEObject *mHeliElementArt;              // offset 0xC0, size 0x4
    FEObject *mHeliLineOfSiteArt;           // offset 0xC4, size 0x4
    FEImage *mCopElementArt[8];             // offset 0xC8, size 0x20
    FEImage *mRacerElementArt[8];           // offset 0xE8, size 0x20
    FEImage *mCheckpointElementArt;         // offset 0x108, size 0x4
    FEImage *mGPSSelectionElementArt;       // offset 0x10C, size 0x4
    FEImage *mGameplayIcons[17][8];         // offset 0x110, size 0x220
};

#endif
