#ifndef FEMINIMAPWRAPPER_H
#define FEMINIMAPWRAPPER_H

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

// File: speed/indep/src/frontend/hud/FeMinimap.hpp
// total size: 0x1C
// Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:25
class MiniMapItem : public bTNode<MiniMapItem> {
  public:
    MiniMapItem(FEImage *icon, bVector2 &pos, uint32 item_type, bool hidden) {} // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:28
    ~MiniMapItem() {}

    static void *operator new(size_t size, void *ptr) {}

    static void operator delete(void *mem, void *ptr) {}

    static void *operator new(size_t size) {}

    static void operator delete(void *mem, size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }

    static void *operator new(size_t size, const char *name) {}

    static void operator delete(void *mem, const char *name) {}

    static void operator delete(void *mem, size_t size, const char *name) {}

    void UpdatePos(float x, float y) {} // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:33

    FEImage *pIcon;        // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:36
    bVector2 ItemPosition; // offset 0xC, size 0x8, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:37
    uint32 ItemType;       // offset 0x14, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:38
    bool bHidden;          // offset 0x18, size 0x1, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:39
};

// total size: 0x330
// Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:47
class Minimap : public HudElement {
  public:
    Minimap(const char *pkg_name, int player_number);
    ~Minimap() override;

    void SetupMinimap(IPlayer *player);

    void RefreshMapItems();

    void Update(IPlayer *player) override;

    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);

    void AdjustForWidescreen(bool moveOutwards);

    struct GameplayIconInfo {        // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
        Attrib::Type mIconType;      // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
        eWorldMapItemType mItemType; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
        const char *mElementString;  // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
        uint32 mWorldMapTitle;       // offset 0xC, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
        uint32 mworldIconTexHash;    // offset 0x10, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63
    }; // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:63

    static GameplayIconInfo &GetGameplayIconInfo(Attrib::Type iconType) {}      // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:70
    static GameplayIconInfo &GetGameplayIconInfo(eWorldMapItemType itemType) {} // Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:71

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
    bTList<MiniMapItem> StaticMiniMapItems; // offset 0x28, size 0x8, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:92

    static const unsigned int mMapPieces; // size: 0x4, address: 0xFFFFFFFF, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:94

    FEObject *TrackmapLayout;                    // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:96
    FEMultiImage *TrackmapArt[4];                // offset 0x34, size 0x10, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:97
    FEVector2 TrackmapArtUVs[4][2];              // offset 0x44, size 0x40, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:98
    FEImage *TrackmapNorth;                      // offset 0x84, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:99
    FEImage *mPlayerCarIndicator;                // offset 0x88, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:102
    FEImage *mPlayerCarIndicator2;               // offset 0x8C, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:103
    TrackInfo *CurrentTrack;                     // offset 0x90, size 0x4
    FEVector3 mMapDefaultPos;                    // offset 0x94, size 0xC
    float mSpeedZoomScale;                       // offset 0xA0, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:122
    float mPolyRotation;                         // offset 0xA4, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:124
    bVector2 mTrackTargetNormalized;             // offset 0xA8, size 0x8, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:126
    COORD2 mTrackMapCentre;                      // offset 0xB0, size 0x8, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:127
    int mCopFlashCounter;                        // offset 0xB8, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:129
    int MinimapRotateWithPlayer;                 // offset 0xBC, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:130
    static const unsigned int mMaxElements;      // size: 0x4, address: 0xFFFFFFFF
    FEObject *mHeliElementArt;                   // offset 0xC0, size 0x4
    FEObject *mHeliLineOfSiteArt;                // offset 0xC4, size 0x4
    FEImage *mCopElementArt[8];                  // offset 0xC8, size 0x20
    FEImage *mRacerElementArt[8];                // offset 0xE8, size 0x20
    FEImage *mCheckpointElementArt;              // offset 0x108, size 0x4
    FEImage *mGPSSelectionElementArt;            // offset 0x10C, size 0x4, Decl: speed/indep/src/frontend/hud/FeMinimap.hpp:137
    FEImage *mGameplayIcons[17][8];              // offset 0x110, size 0x220
    static GameplayIconInfo kGameplayIconInfo[]; // size: 0x0, address: 0x8041BCA0, Decl: speed/indep/src/frontend/hud/FeMinimap.cpp:69
};

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *isimable);

#endif
