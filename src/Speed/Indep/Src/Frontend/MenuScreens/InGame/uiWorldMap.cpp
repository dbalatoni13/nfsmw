#include "uiWorldMap.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOff.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
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
extern RaceParameters TheRaceParameters;
void FEngGetSize(FEObject* obj, float& x, float& y);
void FEngGetCenter(FEObject* obj, float& x, float& y);
void FEngGetTopLeft(FEObject* obj, float& x, float& y);
void FEngGetBottomRight(FEObject* obj, float& x, float& y);
float FEngGetScaleX(FEObject* obj);
float FEngGetScaleY(FEObject* obj);
void FEngSetButtonTexture(FEImage* img, unsigned int tex_hash);
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

float FEngGetCenterX(FEObject* obj);
float FEngGetCenterY(FEObject* obj);

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

inline MapItem::MapItem(eWorldMapItemType type, FEObject* iconObj, bVector2& map_pos, bVector2& world_pos,
                  float rot, GIcon* icon) {
    unsigned int* initial_pos_words = reinterpret_cast< unsigned int* >(&InitialPos);
    unsigned int* world_pos_words = reinterpret_cast< unsigned int* >(&WorldPos);
    const unsigned int* map_pos_words = reinterpret_cast< const unsigned int* >(&map_pos);
    const unsigned int* source_world_pos_words = reinterpret_cast< const unsigned int* >(&world_pos);

    pIcon = iconObj;
    initial_pos_words[0] = map_pos_words[0];
    initial_pos_words[1] = map_pos_words[1];
    world_pos_words[0] = source_world_pos_words[0];
    world_pos_words[1] = source_world_pos_words[1];
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

void MapItem::Show() {
    FEngSetVisible(pIcon);
}

void MapItem::Hide() {
    FEngSetInvisible(pIcon);
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
        unsigned int color = 0xffcccccc;
        if (FlashTimer < 3) {
            color = 0xff0000ff;
        } else if (FlashTimer - 5U < 2) {
            color = 0xffa00000;
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
        Draw();
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
    if (bVisibility) {
        const unsigned long FEObj_GREY = 0x6ebbfb68;
        FEngSetScript(pIconGroup, FEObj_GREY, true);
        if (!FEngIsScriptSet(static_cast< FEObject* >(GetTitleObject()), FEObj_Highlight)) {
            FEngSetScript(static_cast< FEObject* >(GetTitleObject()), FEObj_GREY, true);
        }
    } else {
        const unsigned long FEObj_NORMAL = 0x163c76;
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
        if (!FEngIsScriptSet(static_cast< FEObject* >(GetTitleObject()), FEObj_Highlight)) {
            FEngSetScript(static_cast< FEObject* >(GetTitleObject()), FEObj_NORMAL, true);
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
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
int FEPrintf(const char* pkg_name, int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetRotationZ(FEObject* obj, float rot);
void FEngSetPosition(FEObject* obj, float x, float y);
const char* GetLocalizedString(unsigned int hash);
void FEngSetVisible(const char* pkg_name, unsigned int obj_hash);
void FEngSetInvisible(const char* pkg_name, unsigned int obj_hash);
void FEngSetTextureHash(const char* pkg_name, unsigned int obj_hash, unsigned int tex_hash);

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
        mGPSingIcon = nullptr;
    }
}

WorldMap::WorldMap(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) {
    Cursor = nullptr;
    mActionQ = nullptr;
    CurrentVelocity.x = 0.0f;
    CurrentVelocity.y = 0.0f;
    CursorMoveFrom.x = 0.0f;
    CursorMoveFrom.y = 0.0f;
    pCurrentTrack = nullptr;
    TrackMap = nullptr;
    MapTopLeft.x = 0.0f;
    MapTopLeft.y = 0.0f;
    MapSize.x = 0.0f;
    MapSize.y = 0.0f;
    SelectedItem = nullptr;
    MapStreamer = nullptr;
    CurrentView = 0;
    CurrentZoom = 0;
    CurrentRaceType = -1;
    bInToggleMode = false;
    bCursorOn = false;
    bCursorMoving = false;
    bLeftHeldOnMap = false;
    fSnapDist = 30.0f;
    mActionQ = new ActionQueue(FEDatabase->PlayerJoyports[0], 0x82d21520, "WorldMapMain", false);
    mActionQ->Enable(true);
    iMaxWidgetsOnScreen = 10;
    Setup();
    RefreshHeader();
}

WorldMap::~WorldMap() {
    delete mActionQ;
    delete MapStreamer;
    MapStreamer = nullptr;

    IPlayer* iplayer = IPlayer::First(PLAYER_LOCAL);
    if (iplayer != nullptr) {
        {
            IHud* hud = iplayer->GetHud();
            hud->RefreshMiniMapItems();
        }
    }
}

void WorldMap::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                   unsigned long param2) {
    if ((bInToggleMode || (msg != 0x72619778 && msg != 0x911c0a4b)) && msg != 0xc407210) {
        UIWidgetMenu::NotificationMessage(msg, obj, param1, param2);
    }
    if (msg != 0xa16ca7bd) {
        if (msg < 0xa16ca7be) {
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
                            unsigned int title_hash;
                            unsigned int message_hash;
                            unsigned int button_hash;
                            if (SelectedItem == nullptr || SelectedItem->GetIcon() == nullptr) {
                                if (mGPSingIcon == nullptr) {
                                    return;
                                }
                                title_hash = 0x417b2601;
                                message_hash = 0x1a294dad;
                                button_hash = 0xa6be2ebb;
                            } else {
                                title_hash = 0x70e01038;
                                message_hash = 0x417b25e4;
                                button_hash = 0x96ac0a32;
                            }
                            DialogInterface::ShowTwoButtons(
                                GetPackageName(), "InGameDialog.fng",
                                static_cast< eDialogTitle >(3), title_hash, message_hash,
                                0xa16ca7bd, 0xb4edeb6d, 0xb4edeb6d,
                                static_cast< eDialogFirstButtons >(1), button_hash);
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
    }

    if (GPS_IsEngaged()) {
        GPS_Disengage();
        ClearGPSing();
    }
    if (SelectedItem == nullptr) {
        return;
    }
    if (SelectedItem->GetIcon() == nullptr) {
        return;
    }

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
    while (item != TheMapItems.EndOfList()) {
        item->Hide();
        item->ResetSize();
        item = item->GetNext();
    }

    while (!TheMapItems.IsEmpty()) {
        delete TheMapItems.RemoveHead();
    }

    FEWidget* widget = Options.GetHead();
    while (widget != Options.EndOfList()) {
        static_cast< ItemTypeToggle* >(widget)->StartExit();
        widget = widget->GetNext();
    }

    ClearWidgets();
}

bool WorldMap::ClampToMapBounds(float& x, float& y) {
    float bottom_right_x;
    float bottom_right_y;
    FEngGetBottomRight(static_cast< FEObject* >(TrackMap), bottom_right_x, bottom_right_y);

    float min_x = MapTopLeft.x + 8.0f;
    if (min_x <= x) {
        if (x <= bottom_right_x - 8.0f) {
            float min_y = MapTopLeft.y + 26.0f;
            if (min_y <= y) {
                float max_y = bottom_right_y - 32.0f;
                if (y <= max_y) {
                    return false;
                }
                y = max_y;
            } else {
                y = min_y;
            }
        } else {
            x = bottom_right_x - 8.0f;
        }
    } else {
        x = min_x;
    }
    return true;
}

void WorldMap::UpdateAnalogInput() {
    if (mActionQ != nullptr) {
        while (!mActionQ->IsEmpty() && !bInToggleMode) {
            ActionRef aRef = mActionQ->GetAction();
            float speed = 14.0f;
            switch (aRef.ID()) {
            case FRONTENDACTION_RUP:
                CurrentVelocity.y = -aRef.Data() * speed;
                break;
            case FRONTENDACTION_RDOWN:
                CurrentVelocity.y = aRef.Data() * speed;
                break;
            case FRONTENDACTION_RLEFT:
                CurrentVelocity.x = -aRef.Data() * speed;
                break;
            case FRONTENDACTION_RRIGHT:
                CurrentVelocity.x = aRef.Data() * speed;
                break;
            }
            mActionQ->PopAction();
        }
    }
}

void WorldMap::UpdateCursor(bool zoom_thing) {
    UpdateAnalogInput();
    if (MapStreamer->IsZooming()) {
        float zoom = MapStreamer->GetZoomFactor();
        bVector2 pan(0.0f, 0.0f);
        MapStreamer->GetPan(pan);
        bVector2 map_center;
        FEngGetCenter(static_cast< FEObject* >(TrackMap), map_center.x, map_center.y);
        FEngGetTopLeft(static_cast< FEObject* >(TrackMap), MapTopLeft.x, MapTopLeft.y);
        bVector2 pos(CursorMoveFrom.x, CursorMoveFrom.y);
        bVector2 delta;
        delta.x = pos.x - map_center.x;
        delta.y = pos.y - map_center.y;
        delta.x *= zoom;
        delta.y *= zoom;
        pos.x = delta.x + map_center.x;
        pos.y = delta.y + map_center.y;
        bVector2 dpan;
        dpan.x = pan.x * MapSize.x;
        dpan.y = pan.y * MapSize.y;
        dpan.x *= zoom;
        dpan.y *= zoom;
        pos.x -= dpan.x;
        pos.y -= dpan.y;
        ClampToMapBounds(pos.x, pos.y);
        FEngSetCenter(Cursor, pos.x, pos.y);
    } else if (!zoom_thing) {
        if (CurrentVelocity.x != 0.0f || CurrentVelocity.y != 0.0f) {
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
        } else {
            if (bCursorMoving) {
                cFEng::Get()->QueuePackageMessage(0x7e6687da, GetPackageName(), nullptr);
                bCursorMoving = false;
            }
            if (SnapCursor()) {
                RefreshHeader();
            }
        }
    }
}

void WorldMap::MoveCursor(float x, float y) {
    bVector2 cursor_x;
    FEngGetCenter(Cursor, cursor_x.x, cursor_x.y);
    float dx = cursor_x.x + x;
    bVector2 cursor_y;
    FEngGetCenter(Cursor, cursor_y.x, cursor_y.y);
    float dy = cursor_y.y + y;
    bVector2 excess(0.0f, 0.0f);
    bVector2 bottom_right;
    FEngGetBottomRight(static_cast< FEObject* >(TrackMap), bottom_right.x, bottom_right.y);
    if (CurrentZoom != 0 && (x != 0.0f || y != 0.0f)) {
        if (dx < MapTopLeft.x + 8.0f) {
            excess.x = (MapTopLeft.x + 8.0f) - dx;
        } else if (dx > bottom_right.x + -8.0f) {
            excess.x = dx - (bottom_right.x + -8.0f);
        } else if (dy < MapTopLeft.y + 26.0f) {
            excess.y = (MapTopLeft.y + 26.0f) - dy;
        } else if (dy > bottom_right.y + -32.0f) {
            excess.y = dy - (bottom_right.y + -32.0f);
        }
        if (excess.x != 0.0f || excess.y != 0.0f) {
            bVector2 cur_pan;
            MapStreamer->GetPan(cur_pan);
            if (excess.x != 0.0f) {
                excess.x = x / MapSize.x;
            }
            if (excess.y != 0.0f) {
                excess.y = y / MapSize.y;
            }
            float factor = MapStreamer->GetZoomFactor();
            cur_pan += excess;
            float max_pan = 0.5f - 1.0f / factor * 0.5f;
            cur_pan.x = bClamp(cur_pan.x, -max_pan, max_pan);
            cur_pan.y = bClamp(cur_pan.y, -max_pan, max_pan);
            bVector2 prev_pan;
            MapStreamer->GetPan(prev_pan);
            bVector2 pan_to = cur_pan + prev_pan;
            cur_pan = pan_to * 0.5f + bVector2(0.5f, 0.5f);
            MapStreamer->SetPan(cur_pan);
        }
    }
    dx = bClamp(dx, MapTopLeft.x + 8.0f, bottom_right.x + -8.0f);
    dy = bClamp(dy, MapTopLeft.y + 26.0f, bottom_right.y + -32.0f);
    FEngSetCenter(Cursor, dx, dy);
}

bool WorldMap::SnapCursor() {
    bVector2 cursor;
    bVector2 item_pos;
    MapItem* snap_to = nullptr;
    float last_closest = 100000000.0f;
    FEngGetCenter(Cursor, cursor.x, cursor.y);
    for (MapItem* item = TheMapItems.GetHead(); item != TheMapItems.EndOfList(); item = item->GetNext()) {
        bVector2 pos;
        item->GetCurrentPos(pos);
        float cur_dist = bDistBetween(cursor, pos);
        if (!item->IsHidden() && cur_dist < fSnapDist && cur_dist < last_closest) {
            item_pos = pos;
            snap_to = item;
            last_closest = cur_dist;
        }
    }
    if (snap_to != nullptr) {
        const unsigned int _SNAP = 0x1cbf71;
        FEngSetCenter(Cursor, item_pos.x, item_pos.y);
        if (snap_to == SelectedItem) {
            return false;
        }
        SelectedItem = snap_to;
        FEngSetScript(Cursor, _SNAP, true);
    } else {
        if (SelectedItem == nullptr) {
            return false;
        }
        const unsigned int _UNSNAP = 0x7efe8ff4;
        FEngSetScript(Cursor, _UNSNAP, true);
        SelectedItem = nullptr;
    }
    return true;
}

void WorldMap::PanToCursor(float to_zoom) {
    bVector2 cursor;
    bVector2 pan;
    bVector2 map_c;
    FEngGetCenter(Cursor, cursor.x, cursor.y);
    MapStreamer->GetPan(pan);
    pan.x += 0.5f;
    pan.y += 0.5f;
    float zoom = MapStreamer->GetZoomFactor();
    FEngGetCenter(static_cast< FEObject* >(TrackMap), map_c.x, map_c.y);
    bVector2 offset;
    offset = cursor - map_c;
    offset.x = offset.x / MapSize.x;
    offset.y = offset.y / MapSize.y;
    float max_pan = 1.0f / to_zoom * 0.5f;
    offset = offset * (1.0f / zoom);
    bVector2 pan_to;
    pan_to = pan + offset;
    CursorMoveFrom.y = pan_to.y * MapSize.y + MapTopLeft.y;
    CursorMoveFrom.x = pan_to.x * MapSize.x + MapTopLeft.x;
    pan_to.x = bClamp(pan_to.x, max_pan, 1.0f - max_pan);
    pan_to.y = bClamp(pan_to.y, max_pan, 1.0f - max_pan);
    MapStreamer->PanTo(pan_to);
}

void WorldMap::PanToPlayer() {
    IPlayer* player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable* isimable = player->GetSimable();
    bVector2 target_pos;
    bVector2 target_dir;
    GetVehicleVectors(&target_pos, &target_dir, isimable);
    target_pos.x = (target_pos.x - pCurrentTrack->TrackMapCalibrationUpperLeft.x) / pCurrentTrack->TrackMapCalibrationMapWidthMetres;
    target_pos.y = (pCurrentTrack->TrackMapCalibrationUpperLeft.y - target_pos.y) / pCurrentTrack->TrackMapCalibrationMapWidthMetres + 1.0f;
    float max_pan = 1.0f / GetZoomFactor(static_cast< eWorldMapZoomLevels >(CurrentZoom)) * 0.5f;
    target_pos.x = bClamp(target_pos.x, max_pan, 1.0f - max_pan);
    target_pos.y = bClamp(target_pos.y, max_pan, 1.0f - max_pan);
    MapStreamer->SetPan(target_pos);
}

void WorldMap::Setup() {
    SetInitialPositions();

    FEImage* img;
    img = FEngFindImage(GetPackageName(), 0x5bc);
    FEngSetButtonTexture(img, 0x5bc);
    img = FEngFindImage(GetPackageName(), 0x682);
    FEngSetButtonTexture(img, 0x682);
    img = FEngFindImage(GetPackageName(), 0xfbb0b78e);
    FEngSetButtonTexture(img, 0xfbb0b78e);

    TrackMap = static_cast< FEMultiImage* >(FEngFindObject(GetPackageName(), 0x0f365871));
    FEngGetTopLeft(static_cast< FEObject* >(TrackMap), MapTopLeft.x, MapTopLeft.y);
    FEngGetSize(static_cast< FEObject* >(TrackMap), MapSize.x, MapSize.y);
    Cursor = FEngFindObject(GetPackageName(), 0xf156f6c5);

    int region_unlock = 0;
    unsigned char bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    if (bin >= 13) {
        region_unlock = 1;
    } else if (bin > 8) {
        region_unlock = 2;
    }

    MapStreamer = new (__FILE__, __LINE__) UITrackMapStreamer();
    GRaceParameters* params = GRaceStatus::Get().GetRaceParameters();
    MapStreamer->Init(params, TrackMap, 0, region_unlock);
    MapStreamer->SetZoomSpeed(0.5f);
    MapStreamer->SetPanSpeed(0.5f);
    MapStreamer->ResetZoom(false);
    MapStreamer->ResetPan(false);

    if (params != nullptr) {
        CurrentRaceType = params->GetRaceType();
    } else {
        CurrentRaceType = -1;
    }

    pCurrentTrack = TrackInfo::GetTrackInfo(TheRaceParameters.TrackNumber);
    AddPlayerCar();

    IPlayer* player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable* isimable = player->GetSimable();
    IVehicle* ivehicle;
    if (isimable->QueryInterface(&ivehicle)) {
        IVehicleAI* ivehicleai = ivehicle->GetAIVehiclePtr();
        if (ivehicleai->GetPursuit() != nullptr) {
            CurrentView = 3;
        }
    }

    if (CurrentView != 3) {
        CurrentView = FEDatabase->GetGameplaySettings()->LastMapView;
    }

    switch (CurrentView) {
    case 0:
        CurrentZoom = FEDatabase->GetGameplaySettings()->LastMapZoom;
        SetupNavigation();
        break;
    case 1:
        CurrentZoom = FEDatabase->GetGameplaySettings()->LastMapZoom;
        SetupEvent();
        break;
    case 3:
        CurrentZoom = FEDatabase->GetGameplaySettings()->LastPursuitMapZoom;
        SetupPursuit();
        break;
    }

    PanToPlayer();
    float zoomFactor = GetZoomFactor(static_cast< eWorldMapZoomLevels >(CurrentZoom));
    bVector2 zoom;
    zoom.x = 1.0f / zoomFactor;
    zoom.y = zoom.x;
    MapStreamer->SetZoom(zoom);
    SetInitialOption(0);
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
        IPursuitAI* ipursuitai = nullptr;
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
        pos = rb->GetRoadBlockCentre();
        dir = rb->GetRoadBlockDir();
        bVector2 target_pos;
        bVector2 target_dir;
        target_pos.x = pos.z;
        target_pos.y = -pos.x;
        target_dir.x = dir.z;
        target_dir.y = -dir.x;
        bVector2 world_pos;
        world_pos = target_pos;
        ConvertPos(target_pos);
        float rot = ConvertRot(target_dir);
        FEImage* icon = FEngFindImage(GetPackageName(), FEngHashString("MMICON_ROADBLOCK_%d", img_num));
        img_num++;
        MapItem* item = new MapItem(WMIT_ROADBLOCK, static_cast< FEObject* >(icon), target_pos, world_pos, rot, nullptr);
        TheMapItems.AddTail(item);
    }
    if (img_num > 0) {
        AddMapItemOption(0x411f1f86, WMIT_ROADBLOCK);
    }
}

void WorldMap::AddIcon(eWorldMapItemType type, unsigned int hash, GIcon* icon) {
    if (hash != 0 && icon != nullptr) {
        FEImage* image = FEngFindImage(GetPackageName(), hash);
        if (image != nullptr) {
            bVector2 pos2D;
            icon->GetPosition2D(pos2D);
            bVector2 world_pos;
            unsigned int* world_pos_words = reinterpret_cast< unsigned int* >(&world_pos);
            const unsigned int* pos2d_words = reinterpret_cast< const unsigned int* >(&pos2D);
            float rot = 0.0f;

            world_pos_words[0] = pos2d_words[0];
            world_pos_words[1] = pos2d_words[1];
            ConvertPos(pos2D);
            MapItem* item = new MapItem(type, static_cast< FEObject* >(image), pos2D, world_pos, rot, icon);
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

float WorldMap::ConvertRot(bVector2& dir) {
    return bAngToDeg(bATan(dir.y, dir.x));
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
    IPlayer* player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable* isimable = player->GetSimable();
    UMath::Vector3 player_pos = isimable->GetPosition();
    bVector2 real_player;
    bVector2 real_trigger;
    real_player.x = player_pos.z;
    real_player.y = -player_pos.x;
    SelectedItem->GetWorldPos(real_trigger);
    float distance = bDistBetween(real_trigger, real_player);
    bool kph = true;
    const char* distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        kph = false;
        distUnits = GetLocalizedString(0x867dcfd9);
    }
    if (SelectedItem->GetType() != WMIT_PLAYER_CAR) {
        float length;
        if (kph) {
            length = distance * 0.001f;
        } else {
            length = distance * 0.000625f;
        }
        FEPrintf(GetPackageName(), 0xfeeeb39b, "%$.1f %s", length, distUnits);
        FEngSetVisible(GetPackageName(), 0xfeeeb39b);
    } else {
        FEngSetInvisible(GetPackageName(), 0xfeeeb39b);
    }
    Minimap::GameplayIconInfo& desiredIconInfo = Minimap::GetGameplayIconInfo(SelectedItem->GetType());
    if (desiredIconInfo.mworldIconTexHash != 0) {
        FEngSetTextureHash(GetPackageName(), 0x9a5ab124, desiredIconInfo.mworldIconTexHash);
        FEngSetVisible(GetPackageName(), 0x9a5ab124);
    } else {
        FEngSetInvisible(GetPackageName(), 0x9a5ab124);
    }
}

void WorldMap::RefreshHeader() {
    switch (CurrentView) {
    case 0:
        FEngSetLanguageHash(GetPackageName(), 0xd259525f, 0xbf55e8b2);
        break;
    case 1:
        FEngSetLanguageHash(GetPackageName(), 0xd259525f, 0xdfd23484);
        break;
    case 2:
        FEngSetLanguageHash(GetPackageName(), 0xd259525f, 0xf74b357d);
        break;
    case 3:
        FEngSetLanguageHash(GetPackageName(), 0xd259525f, 0xfea872d4);
        break;
    }

    unsigned int zoom_hash = 0x213587bf;
    switch (CurrentZoom) {
    case 1:
        zoom_hash = 0x0a9be7d7;
        break;
    case 2:
        zoom_hash = 0x0a9be7d8;
        break;
    case 3:
        zoom_hash = 0x0a9be7da;
        break;
    }
    FEngSetLanguageHash(GetPackageName(), 0xcb76ce5b, zoom_hash);

    if (SelectedItem != nullptr) {
        DrawItemType();
        DrawItemStats();
    } else {
        FEPrintf(GetPackageName(), 0x9331fd4f, "");
        FEPrintf(GetPackageName(), 0xfeeeb39b, "");
    }

    if (pCurrentOption != nullptr && bInToggleMode) {
        ItemTypeToggle* tog = static_cast< ItemTypeToggle* >(pCurrentOption);
        if (tog->GetVisibility()) {
            FEngSetScript(GetPackageName(), 0x32490131, 0x6ebbfb68, true);
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x2c35ec64);
        } else {
            FEngSetScript(GetPackageName(), 0x32490131, 0x6ebbfb68, true);
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0xba0a6a2b);
        }
        FEngSetLanguageHash(GetPackageName(), 0x51f0064f, 0x58b828ed);
        return;
    }

    IPlayer* iplayer = IPlayer::First(PLAYER_LOCAL);
    if (iplayer == nullptr) {
        return;
    }

    ISimable* isimable = iplayer->GetSimable();
    if (isimable == nullptr) {
        return;
    }

    if (SelectedItem != nullptr && SelectedItem->TheIcon != 0) {
        FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x43512519);
        FEngSetScript(GetPackageName(), 0x32490131, 0x6ebbfb68, true);
    } else if (mGPSingIcon != nullptr) {
        FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0xf1d0d8a5);
        FEngSetScript(GetPackageName(), 0x32490131, 0x6ebbfb68, true);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x43512519);
        FEngSetScript(GetPackageName(), 0x32490131, 0x00163c76, true);
    }
    FEngSetLanguageHash(GetPackageName(), 0x51f0064f, 0x001335f0);
}
