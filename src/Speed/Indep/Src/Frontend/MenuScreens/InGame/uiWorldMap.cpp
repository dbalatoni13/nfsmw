#include "uiWorldMap.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOff.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
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
    static GameplayIconInfo& GetGameplayIconInfo(eWorldMapItemType itemType) {
        for (int i = 0; i < GIcon::kType_Count; i++) {
            if (kGameplayIconInfo[i].mItemType == itemType) {
                return kGameplayIconInfo[i];
            }
        }
        return kGameplayIconInfo[0];
    }
};

extern Timer RealTimer;
void FEngGetSize(FEObject* obj, float& x, float& y);
void FEngGetCenter(FEObject* obj, float& x, float& y);
void FEngGetTopLeft(FEObject* obj, float& x, float& y);
void FEngGetBottomRight(FEObject* obj, float& x, float& y);
float FEngGetScaleX(FEObject* obj);
float FEngGetScaleY(FEObject* obj);
void FEngSetColor(FEObject* obj, unsigned int color);
void FEngSetScript(FEObject* object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject* obj, unsigned int script_hash);
void FEngSetTopLeft(FEObject* object, float x, float y);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
bool FEngTestForIntersection(float xPos, float yPos, FEObject* obj);
void FEngSetLastButton(const char* pkg_name, unsigned char button);
void FEngSetRotationZ(FEObject* obj, float z);
FEColor FEngGetObjectColor(FEObject* obj);
unsigned int FEngGetTextureHash(FEImage* image);
bool GPS_IsEngaged();
void GPS_Disengage();
int GPS_Engage(const UMath::Vector3& pos, float radius);
void GetVehicleVectors(bVector2* pos, bVector2* dir, ISimable* simable);

inline float bDistBetween(const bVector2* v1, const bVector2* v2) {
    float x = v1->x - v2->x;
    float y = v1->y - v2->y;
    return bSqrt(x * x + y * y);
}
inline float bDistBetween(const bVector2& v1, const bVector2& v2) {
    return bDistBetween(&v1, &v2);
}

inline int tCubic1D::HasArrived() {
    return state == 0;
}

inline int tCubic2D::HasArrived() {
    return x.HasArrived() && y.HasArrived();
}

inline bool UITrackMapStreamer::IsZooming() {
    return !ZoomCubic.HasArrived();
}

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

inline MapItem::MapItem(eWorldMapItemType type, FEObject* iconObj, bVector2& map_pos, bVector2& world_pos,
                 float rot, GIcon* icon) {
    pIcon = iconObj;
    InitialPos = map_pos;
    WorldPos = world_pos;
    Rot = rot;
    TheType = type;
    TheIcon = icon;
    bHidden = false;
    if (FEDatabase->GetGameplaySettings()->IsMapItemEnabled(type)) {
        bHidden = false;
        Show();
    } else {
        bHidden = true;
        Hide();
    }
    FEngGetSize(pIcon, InitialSize.x, InitialSize.y);
    FEngSetCenter(pIcon, InitialPos.x, InitialPos.y);
    FEngSetRotationZ(pIcon, Rot);
}

inline CopItem::CopItem(FEObject* icon, bVector2& pos, bVector2& world_pos, float rot,
                 eWorldMapItemType type)
    : MapItem(type, icon, pos, world_pos, rot, nullptr) {
    FlashTimer = -1;
}

inline HeliItem::HeliItem(FEImage* view, FEObject* icon, bVector2& pos, bVector2& world_pos, float rot)
    : CopItem(icon, pos, world_pos, rot, WMIT_COP_HELI) {
    pViewCone = view;
    InitialSize.x = FEngGetScaleX(pIcon);
    InitialSize.y = FEngGetScaleY(pIcon);
    FEngSetCenter(static_cast< FEObject* >(pViewCone), pos.x, pos.y);
    FEngSetRotationZ(static_cast< FEObject* >(pViewCone), rot);
}

inline ItemTypeToggle::ItemTypeToggle(unsigned int name_hash, eWorldMapItemType type, bool vis)
    : FEButtonWidget(true) {
    ItemType = type;
    NameHash = name_hash;
    pIcon = nullptr;
    bVisibility = vis;
    bExiting = 0;
}

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

void WorldMap::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                   unsigned long param2) {
    if ((bInToggleMode || (msg != 0x72619778 && msg != 0x911c0a4b)) && msg != 0xc407210) {
        UIWidgetMenu::NotificationMessage(msg, obj, param1, param2);
    }
    if (msg == 0xa16ca7bd) {
        if (GPS_IsEngaged()) {
            GPS_Disengage();
            ClearGPSing();
        }
        if (SelectedItem != nullptr && SelectedItem->GetIcon() != nullptr) {
            UMath::Vector3 pos;
            eUnSwizzleWorldVector(SelectedItem->GetIcon()->GetPosition(),
                                  reinterpret_cast< bVector3& >(pos));
            if (GPS_Engage(pos, 0.0f) == 0) {
                DialogInterface::ShowOneButton(GetPackageName(), "", static_cast< eDialogTitle >(1),
                                               0x417b2601, 0x34dc1bec, 0x7afdf4cc);
            } else {
                SetGPSing(SelectedItem->GetIcon());
                FEngSetLastButton(GetPackageName(), 0);
                cFEng::Get()->QueuePackageMessage(0x911ab364, GetPackageName(), nullptr);
            }
        }
    } else if (msg < 0xa16ca7be) {
        if (msg != 0x72619778) {
            if (msg < 0x72619779) {
                if (msg == 0x35f8620b) {
                    FEWidget* w = pCurrentOption;
                    if (w != nullptr) {
                        w->UnsetFocus();
                    }
                    return;
                }
                if (msg > 0x35f8620b) {
                    if (msg == 0x5073ef13 && !bInToggleMode) {
                        ScrollZoom(eSD_PREV);
                    }
                    return;
                }
                if (msg != 0xc407210) {
                    return;
                }
                if (!bInToggleMode) {
                    IPlayer* iplayer = IPlayer::First(PLAYER_LOCAL);
                    if (iplayer != nullptr) {
                        ISimable* isimable = iplayer->GetSimable();
                        if (isimable != nullptr) {
                            if (SelectedItem == nullptr || SelectedItem->GetIcon() == nullptr) {
                                if (mGPSingIcon == nullptr) {
                                    return;
                                }
                                DialogInterface::ShowTwoButtons(
                                    GetPackageName(), "InGameDialog.fng",
                                    static_cast< eDialogTitle >(3), 0x417b2601, 0x1a294dad,
                                    0xa16ca7bd, 0xb4edeb6d, 0xb4edeb6d,
                                    static_cast< eDialogFirstButtons >(1), 0xa6be2ebb);
                            } else {
                                DialogInterface::ShowTwoButtons(
                                    GetPackageName(), "InGameDialog.fng",
                                    static_cast< eDialogTitle >(3), 0x70e01038, 0x417b25e4,
                                    0xa16ca7bd, 0xb4edeb6d, 0xb4edeb6d,
                                    static_cast< eDialogFirstButtons >(1), 0x96ac0a32);
                            }
                        }
                    }
                    return;
                }
                FEWidget* w = pCurrentOption;
                if (w == nullptr) {
                    return;
                }
                static_cast< ItemTypeToggle* >(w)->Act(GetPackageName(), 0xc407210);
                UpdateIconVisibility(static_cast< ItemTypeToggle* >(pCurrentOption)->GetType(),
                                     static_cast< ItemTypeToggle* >(pCurrentOption)->GetVisibility());
            } else if (msg != 0x911c0a4b) {
                if (msg < 0x911c0a4c) {
                    if (msg != 0x911ab364) {
                        return;
                    }
                    goto leave_screen;
                }
                if (msg != 0x9120409e || bInToggleMode || CurrentView == 3) {
                    return;
                }
                goto view_switch;
            }
        }
    } else {
        if (msg == 0xc519bfc4) {
            return;
        }
        if (msg > 0xc519bfc4) {
            if (msg == 0xd9feec59) {
                if (!bInToggleMode) {
                    ScrollZoom(eSD_NEXT);
                }
            } else if (msg < 0xd9feec5a) {
                if (msg == 0xc98356ba &&
                    cFEng::Get()->IsPackageInControl(GetPackageName())) {
                    UpdateCursor(false);
                    MapStreamer->UpdateAnimation();
                    UpdateCursor(true);
                    float zoom = MapStreamer->GetZoomFactor();
                    float max_zoom = GetZoomFactor(WMZ_LEVEL_4);
                    bVector2 pan(0.0f, 0.0f);
                    MapStreamer->GetPan(pan);
                    bVector2 map_center;
                    FEngGetCenter(static_cast< FEObject* >(TrackMap), map_center.x, map_center.y);
                    bVector2 map_br;
                    FEngGetBottomRight(static_cast< FEObject* >(TrackMap), map_br.x, map_br.y);
                    for (MapItem* item = TheMapItems.GetHead(); item != TheMapItems.EndOfList();
                         item = item->GetNext()) {
                        bVector2 pos(0.0f, 0.0f);
                        item->GetInitialPos(pos);
                        bVector2 delta = pos - map_center;
                        delta *= zoom;
                        pos = delta + map_center;
                        bVector2 dpan(pan.x * MapSize.x, pan.y * MapSize.y);
                        dpan = dpan * zoom;
                        pos -= dpan;
                        item->UpdatePos(pos);
                        float icon_scale =
                            ((zoom - 1.0f) / (max_zoom - 1.0f)) * 0.5f + 1.0f;
                        item->UpdateScale(icon_scale);
                        item->GetCurrentPos(pos);
                        if (!ClampToMapBounds(pos.x, pos.y)) {
                            if (!item->IsHidden()) {
                                item->Show();
                            }
                        } else {
                            item->Hide();
                        }
                        item->Draw();
                    }
                }
            } else if (msg == 0xe1fde1d1) {
                new EWorldMapOff();
            }
            return;
        }
        if (msg == 0xb5af2461) {
            FEngSetLastButton(GetPackageName(), 0);
            goto leave_screen;
        } else {
            if (msg < 0xb5af2462) {
                if (msg != 0xb5971bf1 || bInToggleMode || CurrentView == 3) {
                    return;
                }
                goto view_switch;
            }
            if (msg != 0xc519bfc3) {
                return;
            }
            if (!bInToggleMode) {
                bInToggleMode = true;
                cFEng::Get()->QueuePackageMessage(0x5c28136d, GetPackageName(), nullptr);
                FEWidget* w = pCurrentOption;
                if (w != nullptr) {
                    w->SetFocus(GetPackageName());
                }
                goto refresh_and_end;
            }
        }
        bInToggleMode = false;
        cFEng::Get()->QueuePackageMessage(0x947e6205, GetPackageName(), nullptr);
        goto finish_toggle;
    }
    return;

view_switch : {
    const unsigned int _UNSNAP = 0x7efe8ff4;
    FEngSetScript(Cursor, _UNSNAP, true);
    SelectedItem = nullptr;
    ClearItems();
    AddPlayerCar();
    if (CurrentView == 0) {
        CurrentView = 1;
        SetupEvent();
        SetInitialOption(0);
    } else if (CurrentView == 1) {
        CurrentView = 0;
        SetupNavigation();
        SetInitialOption(0);
    }
    FEDatabase->GetGameplaySettings()->LastMapView = static_cast< unsigned char >(CurrentView);
}

finish_toggle : {
    FEWidget* w = pCurrentOption;
    if (w != nullptr) {
        w->UnsetFocus();
    }
}

refresh_and_end:
    RefreshHeader();
    return;

leave_screen:
    if (!bInToggleMode) {
        cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        return;
    }
    bInToggleMode = false;
    cFEng::Get()->QueuePackageMessage(0x947e6205, GetPackageName(), nullptr);
    goto finish_toggle;
}

void WorldMap::ScrollZoom(eScrollDir dir) {
    int zoom = CurrentZoom;
    if (dir == eSD_PREV) {
        zoom--;
        if (zoom < 0) {
            zoom = WMZ_MAX_ZOOM;
        }
    } else if (dir == eSD_NEXT) {
        zoom++;
        if (zoom > WMZ_MAX_ZOOM) {
            zoom = 0;
        }
    }
    if (zoom != CurrentZoom) {
        CurrentZoom = zoom;
        RefreshHeader();
        float factor = GetZoomFactor(static_cast< eWorldMapZoomLevels >(zoom));
        float factorInv = 1.0f / factor;
        bVector2 scale(factorInv, factorInv);
        MapStreamer->ZoomTo(scale);
        PanToCursor(factor);
        if (CurrentView <= 1) {
            FEDatabase->GetGameplaySettings()->LastMapZoom = static_cast< unsigned char >(CurrentZoom);
        } else if (CurrentView == 3) {
            FEDatabase->GetGameplaySettings()->LastPursuitMapZoom = static_cast< unsigned char >(CurrentZoom);
        }
    }
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
    MapItem* item = TheMapItems.GetHead();
    while (item != TheMapItems.EndOfList()) {
        if (item->TheType == type) {
            if (visible) {
                item->bHidden = false;
                item->Show();
            } else {
                item->bHidden = true;
                item->Hide();
            }
        }
        item = item->GetNext();
    }
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

void WorldMap::UpdateCursor(bool zoom_thing) {
    UpdateAnalogInput();
    if (MapStreamer->IsZooming()) {
        float zoom = MapStreamer->GetZoomFactor();
        bVector2 pan(0.0f, 0.0f);
        MapStreamer->GetPan(pan);
        bVector2 map_center;
        FEngGetCenter(static_cast< FEObject* >(TrackMap), map_center.x, map_center.y);
        bVector2 map_br;
        FEngGetTopLeft(static_cast< FEObject* >(TrackMap), MapTopLeft.x, MapTopLeft.y);
        bVector2 pos(CursorMoveFrom.x, CursorMoveFrom.y);
        bVector2 delta = pos - map_center;
        delta *= zoom;
        pos = delta + map_center;
        bVector2 dpan(pan.x * MapSize.x, pan.y * MapSize.y);
        dpan = dpan * zoom;
        pos -= dpan;
        ClampToMapBounds(pos.x, pos.y);
        FEngSetCenter(Cursor, pos.x, pos.y);
    } else if (!zoom_thing) {
        if (CurrentVelocity.x == 0.0f && CurrentVelocity.y == 0.0f) {
            if (bCursorMoving) {
                cFEng::Get()->QueuePackageMessage(0x7e6687da, GetPackageName(), nullptr);
                bCursorMoving = false;
            }
            if (SnapCursor()) {
                RefreshHeader();
            }
        } else {
            if (!bCursorMoving) {
                cFEng::Get()->QueuePackageMessage(0x9f710838, GetPackageName(), nullptr);
                bCursorMoving = true;
            }
            MoveCursor(CurrentVelocity.x, CurrentVelocity.y);
            if (SelectedItem != nullptr) {
                bVector2 cursor;
                bVector2 pos;
                FEngGetCenter(Cursor, cursor.x, cursor.y);
                SelectedItem->GetCurrentPos(pos);
                float dist = bDistBetween(cursor, pos);
                if (dist >= fSnapDist) {
                    const unsigned int _UNSNAP = 0x7efe8ff4;
                    FEngSetScript(Cursor, _UNSNAP, true);
                    SelectedItem = nullptr;
                    RefreshHeader();
                }
            }
        }
    }
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

void WorldMap::AddMapItemOption(unsigned int name_hash, eWorldMapItemType type) {
    ItemTypeToggle* option = new ItemTypeToggle(name_hash, type, FEDatabase->GetGameplaySettings()->IsMapItemEnabled(type));
    Minimap::GameplayIconInfo& iconInfo = Minimap::GetGameplayIconInfo(type);
    unsigned int tex_hash = 0;
    unsigned int colour = 0xffffffff;
    FEObject* iconObj = FEngFindObject(GetPackageName(), FEngHashString(iconInfo.mElementString, 0));
    if (iconObj != nullptr) {
        FEColor c = FEngGetObjectColor(iconObj);
        colour = static_cast< unsigned long >(c);
        tex_hash = FEngGetTextureHash(static_cast< FEImage* >(iconObj));
    }
    option->SetIcon(GetCurrentFEImage("OPTION_ICON_"), tex_hash, colour);
    option->SetIconGroup(GetCurrentFEObject("ICON_VIS_GROUP_"));
    AddButtonOption(option);
}

void WorldMap::AddPlayerCar() {
    const unsigned int FEObj_PlayerCarIndicator = 0xdd9ef5ff;
    FEImage* icon = FEngFindImage(GetPackageName(), FEObj_PlayerCarIndicator);
    IPlayer* player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable* isimable = player->GetSimable();
    bVector2 target_pos;
    bVector2 target_dir;
    GetVehicleVectors(&target_pos, &target_dir, isimable);
    bVector2 world_pos;
    world_pos = target_pos;
    ConvertPos(target_pos);
    float rot = ConvertRot(target_dir);
    MapItem* item = new MapItem(WMIT_PLAYER_CAR, static_cast< FEObject* >(icon), target_pos, world_pos, rot, nullptr);
    TheMapItems.AddTail(item);
}

void WorldMap::AddCops() {
    int img_num = 0;
    const IVehicle::List& vehicles = IVehicle::GetList(VEHICLE_AICOPS);
    for (IVehicle* const* iter = vehicles.begin(); iter != vehicles.end(); iter++) {
        if (!(*iter)->IsActive()) {
            continue;
        }
        IPursuitAI* ipursuitai;
        (*iter)->QueryInterface(&ipursuitai);
        ISimable* isimable = (*iter)->GetSimable();
        bVector2 target_pos;
        bVector2 target_dir;
        GetVehicleVectors(&target_pos, &target_dir, isimable);
        bVector2 world_pos;
        world_pos = target_pos;
        ConvertPos(target_pos);
        float rot = ConvertRot(target_dir);
        if (ipursuitai != nullptr && ipursuitai->GetInPursuit()) {
            const UCrc32& vehicleClass = (*iter)->GetVehicleClass();
            if (vehicleClass == VehicleClass::CHOPPER) {
                AddMapItemOption(0xead9bd85, WMIT_COP_HELI);
                FEObject* icon = FEngFindObject(GetPackageName(), 0xe26be422);
                FEImage* view = FEngFindImage(GetPackageName(), 0x21390e47);
                HeliItem* item = new HeliItem(view, icon, target_pos, world_pos, rot);
                TheMapItems.AddTail(item);
            } else {
                FEImage* icon = FEngFindImage(GetPackageName(),
                                              FEngHashString("MMICON_COPCAR_%d", img_num));
                CopItem* item = new CopItem(static_cast< FEObject* >(icon), target_pos, world_pos,
                                            rot, WMIT_COP_CAR);
                TheMapItems.AddTail(item);
                img_num++;
            }
        }
    }
    if (img_num > 0) {
        AddMapItemOption(0xead6ef6c, WMIT_COP_CAR);
    }
}

void WorldMap::AddRoadBlocks() {
    int img_num = 0;
    const IRoadBlock::List& blocks = IRoadBlock::GetList();
    for (IRoadBlock* const* i = blocks.begin(); i != blocks.end(); i++) {
        IRoadBlock* rb = *i;
        UMath::Vector3 pos;
        UMath::Vector3 dir;
        const UMath::Vector3& centre = rb->GetRoadBlockCentre();
        const UMath::Vector3& direction = rb->GetRoadBlockDir();
        bVector2 target_pos;
        bVector2 target_dir;
        target_pos.x = centre.z;
        target_pos.y = -centre.x;
        target_dir.x = direction.z;
        target_dir.y = -direction.x;
        bVector2 world_pos;
        world_pos = target_pos;
        ConvertPos(target_pos);
        float rot = ConvertRot(target_dir);
        FEImage* icon = FEngFindImage(GetPackageName(), FEngHashString("MMICON_ROADBLOCK_%d", img_num));
        img_num++;
        MapItem* item = new MapItem(WMIT_ROADBLOCK, static_cast< FEObject* >(icon), target_pos, world_pos, rot, nullptr);
        TheMapItems.AddTail(item);
    }
}

void WorldMap::AddIcon(eWorldMapItemType type, unsigned int hash, GIcon* icon) {
    if (hash != 0 && icon != nullptr) {
        FEImage* image = FEngFindImage(GetPackageName(), hash);
        if (image != nullptr) {
            bVector2 pos2D;
            bVector2 dir2D;
            icon->GetPosition2D(pos2D);
            bVector2 world_pos;
            world_pos = pos2D;
            ConvertPos(pos2D);
            MapItem* item = new MapItem(type, static_cast< FEObject* >(image), pos2D, world_pos, 0.0f, icon);
            TheMapItems.AddTail(item);
        }
    }
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
    FEngSetVisible(Cursor);
    AddIcons(GIcon::kType_GateCustomShop);
    AddIcons(GIcon::kType_GateSafehouse);
    AddIcons(GIcon::kType_GateCarLot);
}

void WorldMap::SetupEvent() {
    FEngSetVisible(Cursor);
    AddIcons(GIcon::kType_RaceSprint);
    AddIcons(GIcon::kType_RaceCircuit);
    AddIcons(GIcon::kType_RaceDrag);
    AddIcons(GIcon::kType_RaceKnockout);
    AddIcons(GIcon::kType_RaceTollbooth);
    AddIcons(GIcon::kType_RaceSpeedtrap);
    AddIcons(GIcon::kType_RaceRival);
    AddIcons(GIcon::kType_SpeedTrap);
    AddIcons(GIcon::kType_SpeedTrapInRace);
}

void WorldMap::SetupPursuit() {
    FEngSetInvisible(GetPackageName(), 0xa808e057);
    FEngSetInvisible(GetPackageName(), 0x95fdfc4e);
    AddIcons(GIcon::kType_GateSafehouse);
    AddIcons(GIcon::kType_PursuitBreaker);
    AddIcons(GIcon::kType_HidingSpot);
    AddCops();
    AddRoadBlocks();
}

void WorldMap::ConvertPos(bVector2& pos) {
    float x = (pos.x - pCurrentTrack->TrackMapCalibrationUpperLeft.x) / pCurrentTrack->TrackMapCalibrationMapWidthMetres;
    pos.x = x;
    float y = (pCurrentTrack->TrackMapCalibrationUpperLeft.y - pos.y) / pCurrentTrack->TrackMapCalibrationMapWidthMetres + 1.0f;
    pos.y = y;
    pos.x = MapTopLeft.x + x * MapSize.x;
    pos.y = MapTopLeft.y + y * MapSize.y;
}

float WorldMap::ConvertRot(bVector2& rot) {
    return 0.0f;
}

void WorldMap::DrawItemType() {
    Minimap::GameplayIconInfo& desiredIconInfo = Minimap::GetGameplayIconInfo(SelectedItem->GetType());
    FEngSetLanguageHash(GetPackageName(), 0x9331fd4f, desiredIconInfo.mWorldMapTitle);
    if (desiredIconInfo.mWorldMapTitle != 0) {
        FEngSetVisible(GetPackageName(), 0x9331fd4f);
    } else {
        FEngSetInvisible(GetPackageName(), 0x9331fd4f);
    }
}

void WorldMap::DrawItemStats() {
}

void WorldMap::RefreshHeader() {
}
