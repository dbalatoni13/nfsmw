#include "uiWorldMap.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct Minimap {
    struct GameplayIconInfo {
        GIcon::Type mIconType;
        eWorldMapItemType mItemType;
        const char* mElementString;
        unsigned int mWorldMapTitle;
        unsigned int mworldIconTexHash;
    };
    static GameplayIconInfo kGameplayIconInfo[];
    static GameplayIconInfo& GetGameplayIconInfo(GIcon::Type iconType) {
        return kGameplayIconInfo[iconType];
    }
};

extern Timer RealTimer;
void FEngGetSize(FEObject* obj, float& x, float& y);
void FEngSetColor(FEObject* obj, unsigned int color);
void FEngSetScript(FEObject* object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject* obj, unsigned int script_hash);
void FEngSetTopLeft(FEObject* object, float x, float y);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
bool FEngTestForIntersection(float xPos, float yPos, FEObject* obj);

inline float FEngGetSizeY(FEObject* obj) {
    float x;
    float y;
    FEngGetSize(obj, x, y);
    return y;
}

inline void FEngSetSizeX(FEObject* obj, float x) {
    float y = FEngGetSizeY(obj);
    FEngSetSize(obj, x, y);
}

MapItem::~MapItem() {}

void CopItem::Draw() {
    if (!bHidden) {
        unsigned int color;
        if (FlashTimer < 3) {
            color = 0xff0000ff;
        } else if (FlashTimer - 5U < 2) {
            color = 0xffa00000;
        } else {
            color = 0xffcccccc;
        }
        FEngSetColor(pIcon, color);
        FlashTimer = FlashTimer + 1;
        if (FlashTimer > 8) {
            FlashTimer = 1;
        }
    }
}

void HeliItem::Draw() {
    if (!bHidden) {
        float width = bSin(RealTimer.GetSeconds()) * 88.0f + 88.0f;
        FEngSetSizeX(static_cast< FEObject* >(pViewCone), width);
        FlashTimer++;
        if (FlashTimer > 32) {
            FlashTimer = 1;
        }
    }
}

void ItemTypeToggle::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0xc407210) {
        bVisibility ^= 1;
        FEDatabase->GetGameplaySettings()->SetMapItem(GetType(), bVisibility);
        g_pEAXSound->PlayUISoundFX(static_cast< eMenuSoundTriggers >(2));
        Position();
    }
}

void ItemTypeToggle::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {
    if (FEngTestForIntersection(mouse_x, mouse_y, static_cast< FEObject* >(GetTitleObject()))) {
        cFEng::Get()->QueueGameMessage(0xc407210, parent_pkg, 0xff);
    }
}

void ItemTypeToggle::Draw() {
    const unsigned long FEObj_Highlight = 0x249db7b7;
    FEngSetLanguageHash(GetTitleObject(), NameHash);
    if (!bVisibility) {
        const unsigned long FEObj_NORMAL = 0x163c76;
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
        if (!FEngIsScriptSet(static_cast< FEObject* >(GetTitleObject()), FEObj_Highlight)) {
            FEngSetScript(static_cast< FEObject* >(GetTitleObject()), FEObj_NORMAL, true);
        }
    } else {
        const unsigned long FEObj_GREY = 0x6ebbfb68;
        FEngSetScript(pIconGroup, FEObj_GREY, true);
        if (!FEngIsScriptSet(static_cast< FEObject* >(GetTitleObject()), FEObj_Highlight)) {
            FEngSetScript(static_cast< FEObject* >(GetTitleObject()), FEObj_GREY, true);
        }
    }
}

void ItemTypeToggle::Position() {
    FEButtonWidget::Position();
    FEngSetTopLeft(pIconGroup, GetTopLeftX() - 23.0f, GetTopLeftY() + 2.0f);
}

void ItemTypeToggle::UnsetFocus() {
    if (!GetVisibility() && !bExiting) {
        const unsigned long FEObj_NORMAL = 0x163c76;
        FEngSetScript(static_cast< FEObject* >(GetTitleObject()), FEObj_NORMAL, true);
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
        if (GetBacking() != nullptr) {
            FEngSetScript(GetBacking(), FEObj_NORMAL, true);
        }
    } else {
        const unsigned long FEObj_GREY = 0x6ebbfb68;
        FEButtonWidget::UnsetFocus();
        FEngSetScript(pIconGroup, FEObj_GREY, true);
    }
}

void ItemTypeToggle::SetIcon(FEImage* icon, unsigned int texHash, unsigned int texColour) {
    unsigned int color = texColour;
    unsigned int tex_hash = texHash;
    pIcon = icon;
    switch (ItemType) {
    case WMIT_PLAYER_CAR:
        color = 0xffabda4d;
        tex_hash = 0xada85247;
        break;
    case WMIT_COP_CAR:
        color = 0xffffffff;
        tex_hash = 0xdac364e9;
        break;
    case WMIT_ROADBLOCK:
        color = 0xffffed00;
        tex_hash = 0x123f07e2;
        break;
    default:
        break;
    }
    FEngSetColor(static_cast< FEObject* >(pIcon), color);
    FEngSetTextureHash(pIcon, tex_hash);
}

void ItemTypeToggle::Show() {
    FEButtonWidget::Show();
    FEngSetVisible(static_cast< FEObject* >(pIcon));
}

void ItemTypeToggle::Hide() {
    FEButtonWidget::Hide();
    FEngSetInvisible(static_cast< FEObject* >(pIcon));
}


struct FEObject;
struct FEMultiImage;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetTextureHash(FEImage* image, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetRotationZ(FEObject* obj, float rot);
void FEngSetPosition(FEObject* obj, float x, float y);
const char* GetLocalizedString(unsigned int hash);

extern unsigned int iCurrentViewBin;

GIcon* WorldMap::mGPSingIcon;

void WorldMap::SetGPSing(GIcon* icon) {
    if (icon != nullptr) {
        mGPSingIcon = icon;
        icon->SetFlag(0x80);
    }
}

void WorldMap::ClearGPSing() {
    if (mGPSingIcon != nullptr) {
        mGPSingIcon->ClearFlag(0x80);
    }
    mGPSingIcon = nullptr;
}

WorldMap::WorldMap(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) {
    Cursor = nullptr;
    mActionQ = nullptr;
    pCurrentTrack = nullptr;
    TrackMap = nullptr;
    SelectedItem = nullptr;
    MapStreamer = nullptr;
    CurrentView = 0;
    CurrentZoom = 0;
    CurrentRaceType = 0;
    bInToggleMode = false;
    bCursorOn = false;
    bCursorMoving = false;
    bLeftHeldOnMap = false;
    fSnapDist = 30.0f;
    MapStreamer = new UITrackMapStreamer();
    Setup();
}

WorldMap::~WorldMap() {
    ClearItems();
    delete MapStreamer;
}

void WorldMap::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    UIWidgetMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackagePop(0);
    }
}

void WorldMap::ScrollZoom(eScrollDir dir) {
}

float WorldMap::GetZoomFactor(eWorldMapZoomLevels level) {
    switch (level) {
    case WMZ_LEVEL_1: return 1.0f;
    case WMZ_LEVEL_2: return 2.0f;
    case WMZ_LEVEL_4: return 4.0f;
    default: return 1.0f;
    }
}

void WorldMap::UpdateIconVisibility(eWorldMapItemType type, bool visible) {
}

void WorldMap::ClearItems() {
    MapItem* item = TheMapItems.GetHead();
    while (item != nullptr) {
        MapItem* next = item->GetNext();
        delete item;
        item = next;
    }
}

bool WorldMap::ClampToMapBounds(float& x, float& y) {
    bool clamped = false;
    if (x < MapTopLeft.x) { x = MapTopLeft.x; clamped = true; }
    if (x > MapTopLeft.x + MapSize.x) { x = MapTopLeft.x + MapSize.x; clamped = true; }
    if (y < MapTopLeft.y) { y = MapTopLeft.y; clamped = true; }
    if (y > MapTopLeft.y + MapSize.y) { y = MapTopLeft.y + MapSize.y; clamped = true; }
    return clamped;
}

void WorldMap::UpdateAnalogInput() {
}

void WorldMap::UpdateCursor(bool snap) {
}

void WorldMap::MoveCursor(float dx, float dy) {
}

bool WorldMap::SnapCursor() {
    return false;
}

void WorldMap::PanToCursor(float speed) {
}

void WorldMap::PanToPlayer() {
}

void WorldMap::Setup() {
    RefreshHeader();
}

void WorldMap::AddMapItemOption(unsigned int hash, eWorldMapItemType type) {
}

void WorldMap::AddPlayerCar() {
}

void WorldMap::AddCops() {
}

void WorldMap::AddRoadBlocks() {
}

void WorldMap::AddIcon(eWorldMapItemType type, unsigned int hash, GIcon* icon) {
}

void WorldMap::AddIcons(GIcon::Type desiredIconType) {
    GIcon* sortedIcons[200];
    int numIcons;
    int numIconsPlaced;

    numIconsPlaced = 0;
    IPlayer* player = IPlayer::First(PLAYER_LOCAL);
    numIcons = GManager::Get().GatherVisibleIcons(sortedIcons, player);
    for (int onIcon = 0; onIcon < numIcons; onIcon++) {
        GIcon* icon = sortedIcons[onIcon];
        GIcon::Type iconType = icon->GetType();
        Minimap::GameplayIconInfo& iconInfo = Minimap::GetGameplayIconInfo(iconType);
        if (iconInfo.mItemType != 0 && iconType == desiredIconType) {
            unsigned int hash = FEngHashString(iconInfo.mElementString, numIconsPlaced);
            AddIcon(iconInfo.mItemType, hash, icon);
            numIconsPlaced++;
        }
    }
    if (numIconsPlaced > 0) {
        Minimap::GameplayIconInfo& desiredIconInfo = Minimap::GetGameplayIconInfo(desiredIconType);
        AddMapItemOption(desiredIconInfo.mWorldMapTitle, desiredIconInfo.mItemType);
    }
}

void WorldMap::SetupNavigation() {
}

void WorldMap::SetupEvent() {
}

void WorldMap::SetupPursuit() {
}

void WorldMap::ConvertPos(bVector2& pos) {
}

float WorldMap::ConvertRot(bVector2& rot) {
    return 0.0f;
}

void WorldMap::DrawItemType() {
}

void WorldMap::DrawItemStats() {
}

void WorldMap::RefreshHeader() {
}
