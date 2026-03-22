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
    if (!FEDatabase->GetGameplaySettings()->IsMapItemEnabled(type)) {
        bHidden = true;
        Hide();
    } else {
        bHidden = false;
        Show();
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
    if (GetVisibility() || bExiting) {
        const unsigned long FEObj_GREY = 0x6ebbfb68;
        FEButtonWidget::UnsetFocus();
        FEngSetScript(pIconGroup, FEObj_GREY, true);
    } else {
        const unsigned long FEObj_NORMAL = 0x163c76;
        FEngSetScript(static_cast<FEObject *>(GetTitleObject()), FEObj_NORMAL, true);
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
        if (GetBacking()) {
            FEngSetScript(GetBacking(), FEObj_NORMAL, true);
        }
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

void GIcon::SetFlag(unsigned int mask) {
    mFlags |= mask;
}

void GIcon::ClearFlag(unsigned int mask) {
    mFlags &= ~mask;
}

WorldMap::WorldMap(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) {
    MapSize.y = 0.0f;
    TheMapItems.HeadNode.Prev = &TheMapItems.HeadNode;
    CurrentRaceType = -1;
    fSnapDist = 30.0f;
    CurrentVelocity.x = 0.0f;
    CurrentVelocity.y = 0.0f;
    CursorMoveFrom.x = 0.0f;
    CursorMoveFrom.y = 0.0f;
    MapTopLeft.x = 0.0f;
    MapTopLeft.y = 0.0f;
    MapSize.x = 0.0f;
    TheMapItems.HeadNode.Next = &TheMapItems.HeadNode;
    bLeftHeldOnMap = false;
    Cursor = nullptr;
    mActionQ = nullptr;
    TimeSinceLastMove.ResetLow();
    pCurrentTrack = nullptr;
    TrackMap = nullptr;
    SelectedItem = nullptr;
    MapStreamer = nullptr;
    CurrentView = 0;
    CurrentZoom = 0;
    bInToggleMode = false;
    bCursorMoving = false;
    bCursorOn = false;

    signed char joyport = FEDatabase->PlayerJoyports[0];
    mActionQ = new ActionQueue(joyport, 0x82d21520, "WorldMapMain", false);
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
#ifdef EA_PLATFORM_PLAYSTATION2
    unsigned long message = msg;
#else
    register unsigned long message asm("r30") = msg;
#endif
    UMath::Vector3 pos;

    if (!bInToggleMode) {
        if (message == 0x72619778) {
            goto after_base_message;
        }
        if (message == 0x911c0a4b) {
            goto after_base_message;
        }
    }
    if (message != 0xc407210) {
        UIWidgetMenu::NotificationMessage(message, obj, param1, param2);
    }
after_base_message:
    if (message == 0xa16ca7bd) {
        goto handle_gps;
    }
    if (message > 0xa16ca7bd) {
        goto msg_gt_a16ca7bd;
    }
    if (message == 0x72619778) {
        goto refresh_and_end;
    }
    if (message > 0x72619778) {
        goto msg_gt_72619778;
    }
    if (message == 0x35f8620b) {
        goto clear_focus;
    }
    if (message > 0x35f8620b) {
        goto msg_gt_35f8620b;
    }
    if (message == 0xc407210) {
        goto handle_toggle_or_dialog;
    }
    return;

msg_gt_35f8620b:
    if (message == 0x5073ef13) {
        goto zoom_prev;
    }
    return;

msg_gt_72619778:
    if (message == 0x911c0a4b) {
        goto refresh_and_end;
    }
    if (message > 0x911c0a4b) {
        goto msg_gt_911c0a4b;
    }
    if (message == 0x911ab364) {
        goto leave_screen;
    }
    return;

msg_gt_911c0a4b:
    if (message == 0x9120409e) {
        goto maybe_view_switch;
    }
    return;

msg_gt_a16ca7bd:
    if (message == 0xc519bfc4) {
        return;
    }
    if (message > 0xc519bfc4) {
        goto msg_gt_c519bfc4;
    }
    if (message == 0xb5af2461) {
        goto set_last_button_and_leave;
    }
    if (message > 0xb5af2461) {
        goto msg_gt_b5af2461;
    }
    if (message == 0xb5971bf1) {
        goto maybe_view_switch;
    }
    return;

msg_gt_b5af2461:
    if (message == 0xc519bfc3) {
        goto handle_toggle;
    }
    return;

msg_gt_c519bfc4:
    if (message == 0xd9feec59) {
        goto zoom_next;
    }
    if (message > 0xd9feec59) {
        goto msg_gt_d9feec59;
    }
    if (message == 0xc98356ba) {
        goto update_map;
    }
    return;

msg_gt_d9feec59:
    if (message == 0xe1fde1d1) {
        goto world_map_off;
    }
    return;

clear_focus : {
    FEWidget* w = pCurrentOption;
    if (w != nullptr) {
        w->UnsetFocus();
    }
}
    return;

update_map:
    if (!cFEng::Get()->IsPackageInControl(GetPackageName())) {
        return;
    } else {
        float zoom;
        float max_zoom;
        bVector2 pan;
        bVector2* pPan = &pan;

        UpdateCursor(false);
        MapStreamer->UpdateAnimation();
        UpdateCursor(true);
        zoom = MapStreamer->GetZoomFactor();
        max_zoom = GetZoomFactor(WMZ_LEVEL_4);
        pPan->x = 0.0f;
        pPan->y = 0.0f;
        MapStreamer->GetPan(*pPan);

        bVector2 map_center;
        bVector2* pMapCenter = &map_center;
        bVector2* pSavedMapCenter = pMapCenter;
        FEngGetCenter(static_cast< FEObject* >(TrackMap), pMapCenter->x, pMapCenter->y);

        bVector2 map_br;
        FEngGetBottomRight(static_cast< FEObject* >(TrackMap), map_br.x, map_br.y);

        bVector2 pos;
        bVector2* pPos = &pos;
        bVector2 delta;
        bVector2* pDelta = &delta;
        bVector2 map_pos;
        bVector2* pMapPos = &map_pos;
        bVector2 pan_offset;
        bVector2* pPanOffset = &pan_offset;
        bVector2 zoomed_pan;
        bVector2* pZoomedPan = &zoomed_pan;
        bVector2 final_pos;
        bVector2* pFinalPos = &final_pos;

        for (MapItem* item = TheMapItems.GetHead(); item != TheMapItems.EndOfList();
             item = item->GetNext()) {
            pPos->x = 0.0f;
            pPos->y = 0.0f;
            item->GetInitialPos(*pPos);
            pDelta->x = pPos->x - pSavedMapCenter->x;
            pDelta->y = pPos->y - pSavedMapCenter->y;
            pDelta->x *= zoom;
            pDelta->y *= zoom;
            pMapPos->x = pDelta->x + pSavedMapCenter->x;
            pMapPos->y = pDelta->y + pSavedMapCenter->y;
            pPos->x = pMapPos->x;
            pPos->y = pMapPos->y;

            reinterpret_cast< unsigned int* >(pPanOffset)[0] =
                reinterpret_cast< const unsigned int* >(pPan)[0];
            reinterpret_cast< unsigned int* >(pPanOffset)[1] =
                reinterpret_cast< const unsigned int* >(pPan)[1];
            float pan_offset_x = pPanOffset->x * MapSize.x;
            float pan_offset_y = pPanOffset->y * MapSize.y;
            pPanOffset->x = pan_offset_x;
            pPanOffset->y = pan_offset_y;
            pZoomedPan->x = pan_offset_x * zoom;
            pZoomedPan->y = pan_offset_y * zoom;
            pFinalPos->x = pPos->x - pZoomedPan->x;
            pFinalPos->y = pPos->y - pZoomedPan->y;
            reinterpret_cast< unsigned int* >(pPos)[0] =
                reinterpret_cast< const unsigned int* >(pFinalPos)[0];
            reinterpret_cast< unsigned int* >(pPos)[1] =
                reinterpret_cast< const unsigned int* >(pFinalPos)[1];

            item->UpdatePos(*pPos);

            float icon_scale = ((zoom - 1.0f) / (max_zoom - 1.0f)) * 0.5f + 1.0f;
            item->UpdateScale(icon_scale);

            item->GetCurrentPos(*pPos);
            if (ClampToMapBounds(pPos->x, pPos->y)) {
                item->Hide();
            } else if (!item->IsHidden()) {
                item->Show();
            }
            item->Draw();
        }
    }
    return;

handle_toggle_or_dialog:
    if (bInToggleMode) {
        FEWidget* w = pCurrentOption;
        if (w == nullptr) {
            return;
        }
        ItemTypeToggle* tog = static_cast< ItemTypeToggle* >(w);
        tog->Act(GetPackageName(), message);
        UpdateIconVisibility(tog->GetType(), tog->GetVisibility());
        goto refresh_and_end;
    } else {
        IPlayer* iplayer = IPlayer::First(PLAYER_LOCAL);
        if (iplayer == nullptr) {
            return;
        }
        ISimable* isimable = iplayer->GetSimable();
        if (isimable == nullptr) {
            return;
        }

        unsigned int title_hash;
        unsigned int message_hash;
        unsigned int button_hash;
        if (SelectedItem != nullptr && SelectedItem->GetIcon() != nullptr) {
            title_hash = 0x70e01038;
            message_hash = 0x417b25e4;
            button_hash = 0x96ac0a32;
        } else {
            if (mGPSingIcon == nullptr) {
                return;
            }
            title_hash = 0x417b2601;
            message_hash = 0x1a294dad;
            button_hash = 0xa6be2ebb;
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng",
                                        static_cast< eDialogTitle >(3), title_hash, message_hash,
                                        0xa16ca7bd, 0xb4edeb6d, 0xb4edeb6d,
                                        static_cast< eDialogFirstButtons >(1), button_hash);
    }
    return;

handle_gps:
    if (GPS_IsEngaged()) {
        GPS_Disengage();
        ClearGPSing();
    }
    if (SelectedItem == nullptr) {
        goto refresh_and_end;
    }
    if (SelectedItem->GetIcon() == nullptr) {
        goto refresh_and_end;
    }

    eUnSwizzleWorldVector(SelectedItem->GetIcon()->GetPosition(),
                          reinterpret_cast< bVector3& >(pos));
    if (GPS_Engage(pos, 0.0f) == 0) {
        DialogInterface::ShowOneButton(GetPackageName(), "", static_cast< eDialogTitle >(1),
                                       0x417b2601, 0x34dc1bec, 0x7afdf4cc);
        goto refresh_and_end;
    }
    SetGPSing(SelectedItem->GetIcon());
    FEngSetLastButton(GetPackageName(), 0);
    cFEng::Get()->QueuePackageMessage(0x911ab364, GetPackageName(), nullptr);
    goto refresh_and_end;

set_last_button_and_leave:
    FEngSetLastButton(GetPackageName(), 0);
leave_screen:
    if (!bInToggleMode) {
        cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        return;
    }
    bInToggleMode = false;
    cFEng::Get()->QueuePackageMessage(0x947e6205, GetPackageName(), nullptr);
    goto finish_toggle;

maybe_view_switch:
    if (bInToggleMode || CurrentView == 3) {
        return;
    }

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
    goto refresh_and_end;
}

handle_toggle:
    if (!bInToggleMode) {
        bInToggleMode = true;
        cFEng::Get()->QueuePackageMessage(0x5c28136d, GetPackageName(), nullptr);
        FEWidget* w = pCurrentOption;
        if (w != nullptr) {
            w->SetFocus(GetPackageName());
        }
        goto refresh_and_end;
    }
    bInToggleMode = false;
    cFEng::Get()->QueuePackageMessage(0x947e6205, GetPackageName(), nullptr);
    goto finish_toggle;

finish_toggle : {
    FEWidget* w = pCurrentOption;
    if (w != nullptr) {
        w->UnsetFocus();
    }
}

refresh_and_end:
    RefreshHeader();
    return;

zoom_prev:
    if (!bInToggleMode) {
        ScrollZoom(eSD_PREV);
    }
    return;

zoom_next:
    if (!bInToggleMode) {
        ScrollZoom(eSD_NEXT);
    }
    return;

world_map_off:
    new EWorldMapOff();
    return;
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
        float factor = GetZoomFactor(static_cast<eWorldMapZoomLevels>(zoom));
        float factorInv = 1.0f / factor;
        bVector2 scale;
        scale.y = factorInv;
        scale.x = factorInv;
        MapStreamer->ZoomTo(scale);
        PanToCursor(factor);
        switch (CurrentView) {
        case 0:
        case 1:
            FEDatabase->GetGameplaySettings()->LastMapZoom = static_cast<unsigned char>(CurrentZoom);
            break;
        case 3:
            FEDatabase->GetGameplaySettings()->LastPursuitMapZoom = static_cast<unsigned char>(CurrentZoom);
            break;
        }
    }
}

float WorldMap::GetZoomFactor(eWorldMapZoomLevels level) {
    float factor = 1.0f;
    switch (level) {
    case WMZ_LEVEL_1: factor = 2.0f; break;
    case WMZ_LEVEL_2: factor = 4.0f; break;
    case WMZ_LEVEL_4: factor = 8.0f; break;
    default: break;
    }
    return factor;
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
    bVector2 bottom_right;
    FEngGetBottomRight(static_cast< FEObject* >(TrackMap), bottom_right.x, bottom_right.y);

    bool changed = false;
    float min_x = MapTopLeft.x + 8.0f;
    if (x < min_x) {
        x = min_x;
        changed = true;
    } else {
        float max_x = bottom_right.x - 8.0f;
        if (x > max_x) {
            x = max_x;
            changed = true;
        } else {
            float min_y = MapTopLeft.y + 26.0f;
            if (y < min_y) {
                y = min_y;
                changed = true;
            } else {
                float max_y = bottom_right.y - 32.0f;
                if (y > max_y) {
                    y = max_y;
                    changed = true;
                }
            }
        }
    }
    return changed;
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
        bVector2 pan;
        pan.y = 0.0f;
        pan.x = 0.0f;
        MapStreamer->GetPan(pan);
        bVector2 map_center;
        FEngGetCenter(static_cast< FEObject* >(TrackMap), map_center.x, map_center.y);
        FEngGetTopLeft(static_cast< FEObject* >(TrackMap), MapTopLeft.x, MapTopLeft.y);
        bVector2 pos;
        pos = CursorMoveFrom;
        bVector2 delta = pos - map_center;
        delta *= zoom;
        bVector2 map_br = delta + map_center;
        pos = map_br;
        pan.x *= MapSize.x;
        pan.y *= MapSize.y;
        pan = pan * zoom;
        pos = pos - pan;
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
    bVector2* pExcess = &excess;
    bVector2* pBottomRight = &bottom_right;
    FEngGetBottomRight(static_cast< FEObject* >(TrackMap), pBottomRight->x, pBottomRight->y);
    if (CurrentZoom != 0 && (x != 0.0f || y != 0.0f)) {
        if (dx < MapTopLeft.x + 8.0f) {
            pExcess->x = (MapTopLeft.x + 8.0f) - dx;
        } else if (dx > pBottomRight->x + -8.0f) {
            pExcess->x = dx - (pBottomRight->x + -8.0f);
        } else if (dy < MapTopLeft.y + 26.0f) {
            pExcess->y = (MapTopLeft.y + 26.0f) - dy;
        } else if (dy > pBottomRight->y + -32.0f) {
            pExcess->y = dy - (pBottomRight->y + -32.0f);
        }
        if (pExcess->x != 0.0f || pExcess->y != 0.0f) {
            bVector2 cur_pan;
            bVector2* pCurPan = &cur_pan;
            MapStreamer->GetPan(*pCurPan);
            if (pExcess->x != 0.0f) {
                pExcess->x = x / MapSize.x;
            }
            if (pExcess->y != 0.0f) {
                pExcess->y = y / MapSize.y;
            }
            float factor = MapStreamer->GetZoomFactor();
            *pCurPan += *pExcess;
            float max_pan = 0.5f - 1.0f / factor * 0.5f;
            pCurPan->x = bClamp(pCurPan->x, -max_pan, max_pan);
            pCurPan->y = bClamp(pCurPan->y, -max_pan, max_pan);
            bVector2 prev_pan;
            bVector2* pPrevPan = &prev_pan;
            MapStreamer->GetPan(*pPrevPan);
            bVector2 pan_to = *pCurPan + *pPrevPan;
            *pCurPan = pan_to * 0.5f;
            pCurPan->x += 0.5f;
            pCurPan->y += 0.5f;
            MapStreamer->SetPan(*pCurPan);
        }
    }
    dx = bClamp(dx, MapTopLeft.x + 8.0f, pBottomRight->x + -8.0f);
    dy = bClamp(dy, MapTopLeft.y + 26.0f, pBottomRight->y + -32.0f);
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
    bVector2* pCursor = &cursor;
    bVector2* pPan = &pan;
    bVector2* pMap_c = &map_c;
    FEngGetCenter(Cursor, pCursor->x, pCursor->y);
    MapStreamer->GetPan(*pPan);
    pPan->x += 0.5f;
    pPan->y += 0.5f;
    float zoom = MapStreamer->GetZoomFactor();
    FEngGetCenter(static_cast< FEObject* >(TrackMap), pMap_c->x, pMap_c->y);
    bVector2 offset;
    bVector2* pOffset = &offset;
    pOffset->x = pCursor->x - pMap_c->x;
    pOffset->y = pCursor->y - pMap_c->y;
    pOffset->x = pOffset->x / MapSize.x;
    pOffset->y = pOffset->y / MapSize.y;
    float max_pan = 1.0f / to_zoom * 0.5f;
    float zoom_factor = 1.0f / zoom;
    pOffset->x *= zoom_factor;
    pOffset->y *= zoom_factor;
    bVector2 scaled_offset;
    bVector2* pScaledOffset = &scaled_offset;
    reinterpret_cast< unsigned int* >(pScaledOffset)[0] =
        reinterpret_cast< const unsigned int* >(pOffset)[0];
    reinterpret_cast< unsigned int* >(pScaledOffset)[1] =
        reinterpret_cast< const unsigned int* >(pOffset)[1];
    bVector2 pan_to;
    bVector2* pPanTo = &pan_to;
    pPanTo->x = pPan->x + pScaledOffset->x;
    pPanTo->y = pPan->y + pScaledOffset->y;
    CursorMoveFrom.y = pPanTo->y * MapSize.y + MapTopLeft.y;
    CursorMoveFrom.x = pPanTo->x * MapSize.x + MapTopLeft.x;
    pPanTo->x = bClamp(pPanTo->x, max_pan, 1.0f - max_pan);
    pPanTo->y = bClamp(pPanTo->y, max_pan, 1.0f - max_pan);
    MapStreamer->PanTo(*pPanTo);
}

void WorldMap::PanToPlayer() {
    IPlayer* player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable* isimable = player->GetSimable();
    bVector2 target_pos;
    bVector2 target_dir;
    bVector2* pTargetPos = &target_pos;
    GetVehicleVectors(pTargetPos, &target_dir, isimable);
    pTargetPos->x = (pTargetPos->x - pCurrentTrack->TrackMapCalibrationUpperLeft.x) / pCurrentTrack->TrackMapCalibrationMapWidthMetres;
    pTargetPos->y = (pCurrentTrack->TrackMapCalibrationUpperLeft.y - pTargetPos->y) / pCurrentTrack->TrackMapCalibrationMapWidthMetres + 1.0f;
    float max_pan = 1.0f / GetZoomFactor(static_cast< eWorldMapZoomLevels >(CurrentZoom)) * 0.5f;
    pTargetPos->x = bClamp(pTargetPos->x, max_pan, 1.0f - max_pan);
    pTargetPos->y = bClamp(pTargetPos->y, max_pan, 1.0f - max_pan);
    MapStreamer->SetPan(*pTargetPos);
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
    bVector2 target_dir;
    bVector2 target_pos;
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
        UMath::Vector3* pPos = &pos;
        UMath::Vector3 dir;
        UMath::Vector3* pDir = &dir;
        *pPos = rb->GetRoadBlockCentre();
        *pDir = rb->GetRoadBlockDir();
        bVector2 target_pos;
        bVector2* pTargetPos = &target_pos;
        bVector2 target_dir;
        bVector2* pTargetDir = &target_dir;
        pTargetPos->x = pPos->z;
        pTargetPos->y = -pPos->x;
        pTargetDir->x = pDir->z;
        pTargetDir->y = -pDir->x;
        bVector2 world_pos;
        bVector2* pWorldPos = &world_pos;
        pWorldPos->x = pTargetPos->x;
        pWorldPos->y = pTargetPos->y;
        ConvertPos(*pTargetPos);
        float rot = ConvertRot(*pTargetDir);
        FEImage* icon = FEngFindImage(GetPackageName(), FEngHashString("MMICON_ROADBLOCK_%d", img_num));
        img_num++;
        MapItem* item = new MapItem(WMIT_ROADBLOCK, static_cast< FEObject* >(icon), *pTargetPos,
                                    *pWorldPos, rot, nullptr);
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
            bVector2 world_pos = pos2D;
            float rot = 0.0f;
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
