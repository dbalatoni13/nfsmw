#ifndef FRONTEND_MENUSCREENS_INGAME_UIWORLDMAP_H
#define FRONTEND_MENUSCREENS_INGAME_UIWORLDMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

void FEngGetCenter(FEObject* obj, float& x, float& y);
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct FEObject;
struct FEImage;
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetCenter(FEObject* obj, float x, float y);
void FEngSetSize(FEObject* obj, float x, float y);
void FEngSetScaleX(FEObject* obj, float x);
void FEngSetScaleY(FEObject* obj, float y);

struct FEMultiImage;
struct ActionQueue;
struct TrackInfo;
struct UITrackMapStreamer;

#ifndef FRONTEND_DATABASE_EWORLDMAPITEMTYPE_DEFINED
#define FRONTEND_DATABASE_EWORLDMAPITEMTYPE_DEFINED
enum eWorldMapItemType {
    WMIT_NONE = 0,
    WMIT_PLAYER_CAR = 1,
    WMIT_AI_RACE_CAR = 2,
    WMIT_COP_CAR = 4,
    WMIT_COP_HELI = 8,
    WMIT_TRAFFIC_CAR = 16,
    WMIT_ROADBLOCK = 32,
    WMIT_CHECKPOINT = 64,
    WMIT_CIRCUIT_RACE = 128,
    WMIT_SPRINT_RACE = 256,
    WMIT_LAP_KO_RACE = 512,
    WMIT_DRAG_RACE = 1024,
    WMIT_SPEED_TRAP_RACE = 2048,
    WMIT_TOLLBOOTH_RACE = 4096,
    WMIT_MULTIPOINT_RACE = 8192,
    WMIT_CELL_PHONE_RACE = 16384,
    WMIT_RIVAL_RACE = 32768,
    WMIT_CASH_GRAB_RACE = 65536,
    WMIT_CASH_GRAB_SMALL = 131072,
    WMIT_CASH_GRAB_MED = 262144,
    WMIT_CASH_GRAB_LARGE = 524288,
    WMIT_CASH_GRAB_ALL = 917504,
    WMIT_SPEED_TRAP = 1048576,
    WMIT_SAFEHOUSE = 2097152,
    WMIT_SHOP = 4194304,
    WMIT_CAR_LOT = 8388608,
    WMIT_TOKEN = 16777216,
    WMIT_HIDING_SPOT = 33554432,
    WMIT_PURSUIT_BREAKER = 67108864,
};
#endif

enum eWorldMapZoomLevels {
    WMZ_ALL = 0,
    WMZ_LEVEL_1 = 1,
    WMZ_LEVEL_2 = 2,
    WMZ_LEVEL_4 = 3,
    WMZ_MAX_ZOOM = 3,
    NUM_ZOOM_LEVELS = 4,
};

// total size: 0x38
struct MapItem : public bTNode<MapItem> {
    FEObject* pIcon;             // offset 0x8, size 0x4
    bVector2 InitialPos;         // offset 0xC, size 0x8
    bVector2 InitialSize;        // offset 0x14, size 0x8
    bVector2 WorldPos;           // offset 0x1C, size 0x8
    float Rot;                   // offset 0x24, size 0x4
    eWorldMapItemType TheType;   // offset 0x28, size 0x4
    GIcon* TheIcon;              // offset 0x2C, size 0x4
    bool bHidden;                // offset 0x30, size 0x1
    // vtable at 0x34

    MapItem(eWorldMapItemType type, FEObject* iconObj, bVector2& map_pos, bVector2& world_pos,
            float rot, GIcon* icon);
    virtual ~MapItem() {}

    void GetInitialPos(bVector2& pos) {
        pos = InitialPos;
    }
    void GetWorldPos(bVector2& pos) {
        pos = WorldPos;
    }
    void GetCurrentPos(bVector2& pos) {
        FEngGetCenter(pIcon, pos.x, pos.y);
    }
    virtual void UpdatePos(bVector2& pos) {
        FEngSetCenter(pIcon, pos.x, pos.y);
    }
    virtual void UpdateScale(float scale) {
        FEngSetSize(pIcon, InitialSize.x * scale, InitialSize.y * scale);
    }
    virtual void Draw() {}
    virtual void Show();
    virtual void Hide();
    virtual void ResetSize() {
        FEngSetSize(pIcon, InitialSize.x, InitialSize.y);
    }
    GIcon* GetIcon() { return TheIcon; }
    void SetHidden(bool b) {
        bHidden = b;
        if (!b) {
            Show();
        } else {
            Hide();
        }
    }
    bool IsHidden() { return bHidden; }
    eWorldMapItemType GetType() { return TheType; }
};

struct CopItem : public MapItem {
    int FlashTimer;              // offset 0x38, size 0x4

    CopItem(FEObject* icon, bVector2& pos, bVector2& world_pos, float rot, eWorldMapItemType type);
    ~CopItem() override {}
    void Draw() override;
};

struct HeliItem : public CopItem {
    FEImage* pViewCone; // offset 0x3C, size 0x4

    HeliItem(FEImage* view, FEObject* icon, bVector2& pos, bVector2& world_pos, float rot);
    ~HeliItem() override {}
    void UpdatePos(bVector2& pos) override {
        FEngSetCenter(pIcon, pos.x, pos.y);
        FEngSetCenter(static_cast< FEObject* >(pViewCone), pos.x, pos.y);
    }
    void UpdateScale(float scale) override {
        FEngSetScaleX(pIcon, InitialSize.x * scale);
        FEngSetScaleY(pIcon, InitialSize.y * scale);
    }
    void Draw() override;
    void Show() override {
        FEngSetVisible(pIcon);
        FEngSetVisible(static_cast< FEObject* >(pViewCone));
    }
    void Hide() override {
        FEngSetInvisible(pIcon);
        FEngSetInvisible(static_cast< FEObject* >(pViewCone));
    }
    void ResetSize() override {
        FEngSetScaleX(pIcon, InitialSize.x);
        FEngSetScaleY(pIcon, InitialSize.y);
    }
};

struct ItemTypeToggle : public FEButtonWidget {
    eWorldMapItemType ItemType;     // offset 0x40, size 0x4
    unsigned int NameHash;          // offset 0x44, size 0x4
    FEImage* pIcon;                 // offset 0x48, size 0x4
    FEObject* pIconGroup;           // offset 0x4C, size 0x4
    int bVisibility;                // offset 0x50, size 0x4
    int bExiting;                   // offset 0x54, size 0x4

    ItemTypeToggle(unsigned int name_hash, eWorldMapItemType type, bool vis);
    ~ItemTypeToggle() override {}
    void SetIconGroup(FEObject* obj) { pIconGroup = obj; }
    eWorldMapItemType GetType() { return ItemType; }
    int GetVisibility() { return bVisibility; }
    void StartExit() { bExiting = true; }

    void Act(const char* parent_pkg, unsigned int data) override;
    void CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Position() override;
    void UnsetFocus() override;
    void SetIcon(FEImage* icon, unsigned int texHash, unsigned int texColour);
    void Show() override;
    void Hide() override;
};

// total size: 0x19C
struct WorldMap : public UIWidgetMenu {
    FEObject* Cursor;              // offset 0x138, size 0x4
    ActionQueue* mActionQ;         // offset 0x13C, size 0x4
    bVector2 CurrentVelocity;      // offset 0x140, size 0x8
    Timer TimeSinceLastMove;       // offset 0x148, size 0x4
    bVector2 CursorMoveFrom;       // offset 0x14C, size 0x8
    TrackInfo* pCurrentTrack;      // offset 0x154, size 0x4
    FEMultiImage* TrackMap;        // offset 0x158, size 0x4
    bVector2 MapTopLeft;           // offset 0x15C, size 0x8
    bVector2 MapSize;              // offset 0x164, size 0x8
    bTList<MapItem> TheMapItems;   // offset 0x16C, size 0x8
    MapItem* SelectedItem;         // offset 0x174, size 0x4
    UITrackMapStreamer* MapStreamer; // offset 0x178, size 0x4
    unsigned int CurrentView;      // offset 0x17C, size 0x4
    int CurrentZoom;               // offset 0x180, size 0x4
    int CurrentRaceType;           // offset 0x184, size 0x4
    bool bInToggleMode;            // offset 0x188, size 0x1
    bool bCursorOn;                // offset 0x18C, size 0x1
    bool bCursorMoving;            // offset 0x190, size 0x1
    bool bLeftHeldOnMap;           // offset 0x194, size 0x1
    float fSnapDist;               // offset 0x198, size 0x4

    static GIcon* mGPSingIcon;

    static GIcon* GetGPSingIcon() { return mGPSingIcon; }
    static void SetGPSing(GIcon* icon);
    static void ClearGPSing();

    WorldMap(ScreenConstructorData* sd);
    ~WorldMap() override;
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;

    void ScrollZoom(eScrollDir dir);
    float GetZoomFactor(eWorldMapZoomLevels level);
    void UpdateIconVisibility(eWorldMapItemType type, bool vis);
    void ClearItems();
    bool ClampToMapBounds(float& x, float& y);
    void UpdateAnalogInput();
    void UpdateCursor(bool zoom_thing);
    void MoveCursor(float x, float y);
    bool SnapCursor();
    void PanToCursor(float to_zoom);
    void PanToPlayer();
    void Setup() override;
    void AddMapItemOption(unsigned int name_hash, eWorldMapItemType type);
    void AddPlayerCar();
    void AddCops();
    void AddRoadBlocks();
    void AddIcon(eWorldMapItemType type, unsigned int icon_hash, GIcon* icon);
    void AddIcons(GIcon::Type desiredIconType);
    void SetupNavigation();
    void SetupEvent();
    void SetupPursuit();
    void ConvertPos(bVector2& pos);
    float ConvertRot(bVector2& dir);
    void DrawItemType();
    void DrawItemStats();
    void RefreshHeader();
};

#endif
