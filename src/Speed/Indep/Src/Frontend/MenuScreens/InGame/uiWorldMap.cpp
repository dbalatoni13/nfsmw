#include "uiWorldMap.hpp"

#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOff.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/feMinimap.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

struct FEMultiImage;
struct MapItem;

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *simable);
bool GPS_IsEngaged();
bool GPS_Engage(const UMath::Vector3 &target, float maxDeviation);

class WorldMap : public UIWidgetMenu {
  public:
    static GIcon *GetGPSingIcon();
    static void SetGPSing(GIcon *icon);
    static void ClearGPSing();

    WorldMap(ScreenConstructorData *sd);
    ~WorldMap() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void ScrollZoom(eScrollDir dir);
    float GetZoomFactor(eWorldMapZoomLevels zoom);
    void UpdateIconVisibility(eWorldMapItemType type, bool vis);
    void ClearItems();
    bool ClampToMapBounds(float &x, float &y);
    void UpdateAnalogInput();
    void UpdateCursor(bool moved);
    void MoveCursor(float x, float y);
    bool SnapCursor();
    void PanToCursor(float dt);
    void PanToPlayer();
    void Setup() override;
    void AddMapItemOption(unsigned int name_hash, eWorldMapItemType type);
    void AddPlayerCar();
    void AddCops();
    void AddRoadBlocks();
    void AddIcon(eWorldMapItemType type, unsigned int icon_hash, GIcon *icon);
    void AddIcons(GIcon::Type type);
    void SetupNavigation();
    void SetupEvent();
    void SetupPursuit();
    void ConvertPos(bVector2 &pos);
    float ConvertRot(bVector2 &dir);
    void DrawItemType();
    void DrawItemStats();
    void RefreshHeader();

    FEObject *Cursor;
    ActionQueue *mActionQ;
    bVector2 CurrentVelocity;
    Timer TimeSinceLastMove;
    bVector2 CursorMoveFrom;
    TrackInfo *pCurrentTrack;
    FEMultiImage *TrackMap;
    bVector2 MapTopLeft;
    bVector2 MapSize;
    bTList<MapItem> TheMapItems;
    MapItem *SelectedItem;
    UITrackMapStreamer *MapStreamer;
    uint32 CurrentView;
    int CurrentZoom;
    int CurrentRaceType;
    bool bInToggleMode;
    bool bCursorOn;
    bool bCursorMoving;
    bool bLeftHeldOnMap;
    float fSnapDist;

    static GIcon *mGPSingIcon;
};

GIcon *WorldMap::mGPSingIcon = nullptr;

struct MapItem : public bTNode<MapItem> {
    MapItem() {}

    MapItem(eWorldMapItemType type, FEObject *iconObj, bVector2 &map_pos, bVector2 &world_pos, float rot,
            GIcon *icon)
        : pIcon(iconObj), InitialPos(map_pos), WorldPos(world_pos), Rot(rot), TheType(type), TheIcon(icon),
          bHidden(0) {
        if (!FEDatabase->GetGameplaySettings()->IsMapItemEnabled(type)) {
            bHidden = 1;
            Hide();
        } else {
            bHidden = 0;
            Show();
        }
        FEngGetSize(pIcon, InitialSize.x, InitialSize.y);
        FEngSetCenter(pIcon, InitialPos.x, InitialPos.y);
        FEngSetRotationZ(pIcon, Rot);
    }

    virtual ~MapItem() {}

    void GetInitialPos(bVector2 &pos) {
        pos = InitialPos;
    }

    void GetWorldPos(bVector2 &pos) {
        pos = WorldPos;
    }

    void GetCurrentPos(bVector2 &pos) {
        FEngGetCenter(pIcon, pos.x, pos.y);
    }

    virtual void UpdatePos(bVector2 &pos) {
        FEngSetCenter(pIcon, pos.x, pos.y);
    }

    virtual void UpdateScale(float scale) {
        FEngSetSize(pIcon, InitialSize.x * scale, InitialSize.y * scale);
    }

    virtual void Draw() {}

    virtual void Show() {
        FEngSetVisible(pIcon);
    }

    virtual void Hide() {
        FEngSetInvisible(pIcon);
    }

    virtual void ResetSize() {
        FEngSetSize(pIcon, InitialSize.x, InitialSize.y);
    }

    GIcon *GetIcon() {
        return TheIcon;
    }

    void SetHidden(bool b) {
        bHidden = b;
    }

    bool IsHidden() {
        return bHidden;
    }

    eWorldMapItemType GetType() {
        return TheType;
    }

    FEObject *pIcon;              // offset 0x8, size 0x4
    bVector2 InitialPos;          // offset 0xC, size 0x8
    bVector2 InitialSize;         // offset 0x14, size 0x8
    bVector2 WorldPos;            // offset 0x1C, size 0x8
    float Rot;                    // offset 0x24, size 0x4
    eWorldMapItemType TheType;    // offset 0x28, size 0x4
    GIcon *TheIcon;               // offset 0x2C, size 0x4
    bool bHidden;                 // offset 0x30, size 0x1
};

struct CopItem : public MapItem {
    CopItem(FEObject *icon, bVector2 &pos, bVector2 &world_pos, float rot, eWorldMapItemType type)
        : MapItem(type, icon, pos, world_pos, rot, nullptr), FlashTimer(-1) {}

    ~CopItem() override {}

    void Draw() override;

    int FlashTimer;
};

struct HeliItem : public CopItem {
    HeliItem(FEImage *view, FEObject *icon, bVector2 &pos, bVector2 &world_pos, float rot)
        : CopItem(icon, pos, world_pos, rot, WMIT_COP_HELI), pViewCone(view) {
        InitialSize.x = FEngGetScaleX(pIcon);
        InitialSize.y = FEngGetScaleY(pIcon);
        FEngSetCenter(pViewCone, pos.x, pos.y);
        FEngSetRotationZ(pViewCone, rot);
    }

    ~HeliItem() override {}

    void UpdatePos(bVector2 &pos) override {
        FEngSetCenter(pIcon, pos.x, pos.y);
        FEngSetCenter(pViewCone, pos.x, pos.y);
    }

    void UpdateScale(float scale) override {
        FEngSetScaleX(pIcon, InitialSize.x * scale);
        FEngSetScaleY(pIcon, InitialSize.y * scale);
    }

    void Show() override {
        MapItem::Show();
        FEngSetVisible(pViewCone);
    }

    void Hide() override {
        MapItem::Hide();
        FEngSetInvisible(pViewCone);
    }

    void ResetSize() override {
        FEngSetScaleX(pIcon, InitialSize.x);
        FEngSetScaleY(pIcon, InitialSize.y);
    }

    void Draw() override;

    FEImage *pViewCone;
};

class ItemTypeToggle : public FEButtonWidget {
  public:
    ItemTypeToggle() : FEButtonWidget(true) {}

    ItemTypeToggle(unsigned int name_hash, eWorldMapItemType type, bool vis)
        : FEButtonWidget(true), ItemType(type), NameHash(name_hash), pIcon(nullptr), bVisibility(vis),
          bExiting(false) {}

    ~ItemTypeToggle() override {}

    void Act(const char *parent_pkg, uint32 data) override;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Position() override;
    void Show() override;
    void Hide() override;
    // El objetivo NO redefine SetFocus: la ranura +0x54 de _vt.14ItemTypeToggle
    // apunta a SetFocus__14FEButtonWidgetPCc. Con la redefinicion vacia en clase
    // GCC 2.9 emite ADEMAS el cuerpo fuera de linea para la vtable (4 B) y el
    // enlace sale +4 de .text. Verificado con las reubicaciones de la vtable.
    void UnsetFocus() override;

    void SetIcon(FEImage *icon, unsigned int texHash, unsigned int texColour);

    void SetIconGroup(FEObject *obj) {
        pIconGroup = obj;
    }

    eWorldMapItemType GetType() {
        return ItemType;
    }

    bool GetVisibility() {
        return bVisibility;
    }

    void StartExit() {
        bExiting = true;
    }

    eWorldMapItemType ItemType;
    unsigned int NameHash;
    FEImage *pIcon;
    FEObject *pIconGroup;
    bool bVisibility;
    bool bExiting;
};

void CopItem::Draw() {
    if (bHidden == 0) {
        unsigned int color = 0xFFCCCCCC;
        if (FlashTimer <= 2) {
            color = 0xFF0000FF;
        } else if (static_cast<unsigned int>(FlashTimer - 5) <= 1) {
            color = 0xFFA00000;
        }

        FEngSetColor(pIcon, color);

        FlashTimer++;
        if (FlashTimer > 8) {
            FlashTimer = 1;
        }
    }
}

void HeliItem::Draw() {
    if (bHidden == 0) {
        float width = bSin(RealTimer.GetSeconds()) * 32.0f + 32.0f;
        FEngSetSizeX(pViewCone, width);

        FlashTimer++;
        if (FlashTimer > 32) {
            FlashTimer = 1;
        }
    }
}

void ItemTypeToggle::Act(const char *parent_pkg, uint32 data) {
    if (data == __BUTTON_PRESSED__) {
        bVisibility = !bVisibility;
        FEDatabase->GetGameplaySettings()->SetMapItem(GetType(), bVisibility);
        g_pEAXSound->PlayUISoundFX(UISND_COMMON_LEFT);
        Draw();
    }
}

void ItemTypeToggle::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {
    if (FEngTestForIntersection(mouse_x, mouse_y, GetTitleObject())) {
        cFEng::Get()->QueueGameMessage(0x0C407210, parent_pkg, 0xFF);
    }
}

void ItemTypeToggle::Draw() {
    const u32 FEObj_Highlight = 0x249DB7B7;
    FEngSetLanguageHash(GetTitleObject(), NameHash);

    if (bVisibility) {
        const u32 FEObj_NORMAL = 0x6EBBFB68;
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
        if (!FEngIsScriptSet(GetTitleObject(), FEObj_Highlight)) {
            FEngSetScript(GetTitleObject(), FEObj_NORMAL, true);
        }
    } else {
        const u32 FEObj_GREY = 0x163C76;
        FEngSetScript(pIconGroup, FEObj_GREY, true);
        if (!FEngIsScriptSet(GetTitleObject(), FEObj_Highlight)) {
            FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        }
    }
}

void ItemTypeToggle::Position() {
    FEButtonWidget::Position();
    FEngSetTopLeft(pIconGroup, GetTopLeftX() - 22.0f, GetTopLeftY() + 11.0f);
}

void ItemTypeToggle::UnsetFocus() {
    if (GetVisibility() || bExiting) {
        const u32 FEObj_NORMAL = 0x6EBBFB68;
        FEButtonWidget::UnsetFocus();
        FEngSetScript(pIconGroup, FEObj_NORMAL, true);
    } else {
        const u32 FEObj_GREY = 0x163C76;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pIconGroup, FEObj_GREY, true);
        if (GetBacking()) {
            FEngSetScript(GetBacking(), FEObj_GREY, true);
        }
    }
}

void ItemTypeToggle::SetIcon(FEImage *icon, uint32 texHash, uint32 texColour) {
    uint32 color = texColour;
    uint32 tex_hash = texHash;
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
    FEngSetColor(pIcon, color);
    FEngSetTextureHash(pIcon, tex_hash);
}

void ItemTypeToggle::Show() {
    FEButtonWidget::Show();
    FEngSetVisible(pIcon);
}

void ItemTypeToggle::Hide() {
    FEButtonWidget::Hide();
    FEngSetInvisible(pIcon);
}

void WorldMap::SetGPSing(GIcon *icon) {
    if (icon) {
        mGPSingIcon = icon;
        icon->SetGPSing();
    }
}

void WorldMap::ClearGPSing() {
    if (mGPSingIcon) {
        mGPSingIcon->ClearGPSing();
        mGPSingIcon = nullptr;
    }
}

WorldMap::WorldMap(ScreenConstructorData *sd)
    : UIWidgetMenu(sd), MapSize(0.0f, 0.0f), CurrentRaceType(-1), fSnapDist(20.0f), CurrentVelocity(0.0f, 0.0f), CursorMoveFrom(0.0f, 0.0f),
      MapTopLeft(0.0f, 0.0f), bLeftHeldOnMap(false), Cursor(nullptr), mActionQ(nullptr), TimeSinceLastMove(), pCurrentTrack(nullptr),
      TrackMap(nullptr), SelectedItem(nullptr), MapStreamer(nullptr), CurrentView(0), CurrentZoom(0), bInToggleMode(false), bCursorMoving(false) {
    int player_port = FEDatabase->GetPlayersJoystickPort(0);
    mActionQ = new ActionQueue(player_port, 0x82d21520, "WorldMapMain", false);
    mActionQ->Enable(true);
    iMaxWidgetsOnScreen = 10;
    Setup();
    RefreshHeader();
}

WorldMap::~WorldMap() {
    delete mActionQ;
    delete MapStreamer;
    MapStreamer = nullptr;

    IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
    if (iplayer != nullptr) {
        IHud *hud = iplayer->GetHud();
        hud->RefreshMiniMapItems();
    }
}

void WorldMap::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if ((bInToggleMode || (msg != 0x72619778 && msg != 0x911C0A4B)) && msg != 0x0C407210) {
        UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case 0x35F8620B:
            if (pCurrentOption) {
                pCurrentOption->UnsetFocus();
            }
            break;

        case 0xC98356BA:
            if (cFEng::Get()->IsPackageInControl(GetPackageName())) {
                UpdateCursor(false);
                MapStreamer->UpdateAnimation();
                UpdateCursor(true);

                float zoom = MapStreamer->GetZoomFactor();
                float max_zoom = GetZoomFactor(WMZ_LEVEL_4);

                bVector2 pan(0.0f, 0.0f);
                MapStreamer->GetPan(pan);
                bVector2 map_center;
                FEngGetCenter(TrackMap, map_center.x, map_center.y);
                bVector2 map_br;
                FEngGetBottomRight(TrackMap, map_br.x, map_br.y);

                for (MapItem *item = TheMapItems.GetHead(); item != TheMapItems.EndOfList(); item = item->GetNext()) {
                    bVector2 pos(0.0f, 0.0f);
                    item->GetInitialPos(pos);

                    bVector2 delta = pos - map_center;
                    delta *= zoom;
                    pos = map_center + delta;

                    bVector2 dpan = pan;
                    float px = dpan.x;
                    dpan.x = px * MapSize.x;
                    float py = dpan.y;
                    dpan.y = py * MapSize.y;
                    delta = dpan * zoom;
                    pos -= delta;

                    item->UpdatePos(pos);
                    float icon_scale = (zoom - 1.0f) / (max_zoom - 1.0f) * 0.5f + 1.0f;
                    item->UpdateScale(icon_scale);

                    item->GetCurrentPos(pos);

                    if (ClampToMapBounds(pos.x, pos.y)) {
                        item->Hide();
                    } else if (!item->IsHidden()) {
                        item->Show();
                    }

                    item->Draw();
                }
            }
            break;

        case 0x0C407210:
            if (bInToggleMode) {
                if (pCurrentOption) {
                    ItemTypeToggle *tog = static_cast<ItemTypeToggle *>(pCurrentOption);
                    tog->Act(GetPackageName(), msg);
                    UpdateIconVisibility(static_cast<ItemTypeToggle *>(pCurrentOption)->GetType(),
                                         static_cast<ItemTypeToggle *>(pCurrentOption)->GetVisibility());

                    RefreshHeader();
                }
                break;
            }

            {
                IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
                if (!iplayer) {
                    break;
                }

                ISimable *isimable = iplayer->GetSimable();
                if (!isimable) {
                    break;
                }

                if (SelectedItem && SelectedItem->GetIcon()) {
                    DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_confirmation,
                                                    LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xA16CA7BD, 0xB4EDEB6D, 0xB4EDEB6D,
                                                    static_cast<eDialogFirstButtons>(1), 0x96AC0A32);
                } else if (mGPSingIcon) {
                    DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_confirmation,
                                                    LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL, 0xA16CA7BD, 0xB4EDEB6D, 0xB4EDEB6D,
                                                    static_cast<eDialogFirstButtons>(1), 0xA6BE2EBB);
                }
            }
            break;

        case 0xA16CA7BD:
            if (GPS_IsEngaged()) {
                GPS_Disengage();
                ClearGPSing();
            }

            if (SelectedItem && SelectedItem->GetIcon()) {
                UMath::Vector3 pos;
                eUnSwizzleWorldVector(SelectedItem->GetIcon()->GetPosition(), reinterpret_cast<bVector3 &>(pos));

                if (!GPS_Engage(pos, 0.0f)) {
                    DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x34DC1BEC,
                                                   0x7AFDF4CC);
                } else {
                    SetGPSing(SelectedItem->GetIcon());

                    FEngSetLastButton(GetPackageName(), 0);
                    cFEng::Get()->QueuePackageMessage(0x911AB364, GetPackageName(), nullptr);
                }
            }

            RefreshHeader();
            break;

        case 0xB5AF2461:
            FEngSetLastButton(GetPackageName(), 0);
            // fallthrough

        case 0x911AB364:
            if (bInToggleMode) {
                const u32 FEObj_editkeycomplete = 0x947E6205;
                bInToggleMode = false;
                cFEng::Get()->QueuePackageMessage(FEObj_editkeycomplete, GetPackageName(), nullptr);

                if (pCurrentOption) {
                    pCurrentOption->UnsetFocus();
                }

                RefreshHeader();
                break;
            }

            {
                const u32 FEObj_leavescreen = 0x587C018B;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;

        case 0x9120409E:
        case 0xB5971BF1:
            if (bInToggleMode) {
                break;
            }

            if (CurrentView == WMV_PURSUIT) {
                break;
            }

            {
                const unsigned int _UNSNAP = 0x7EFE8FF4;
                FEngSetScript(Cursor, _UNSNAP, true);
            }
            SelectedItem = nullptr;
            ClearItems();
            AddPlayerCar();

            if (CurrentView == WMV_NAVIGATION) {
                CurrentView = WMV_EVENT;
                SetupEvent();
                SetInitialOption(0);
            } else if (CurrentView == WMV_EVENT) {
                CurrentView = WMV_NAVIGATION;
                SetupNavigation();
                SetInitialOption(0);
            }

            FEDatabase->GetGameplaySettings()->LastMapView = CurrentView;

            if (pCurrentOption) {
                pCurrentOption->UnsetFocus();
            }

            RefreshHeader();
            break;

        case 0xC519BFC3:
            if (!bInToggleMode) {
                const u32 FEObj_editkey = 0x5C28136D;
                bInToggleMode = true;

                cFEng::Get()->QueuePackageMessage(FEObj_editkey, GetPackageName(), nullptr);
                if (pCurrentOption) {
                    pCurrentOption->SetFocus(GetPackageName());
                }
            } else {
                const u32 FEObj_editkeycomplete = 0x947E6205;
                bInToggleMode = false;

                cFEng::Get()->QueuePackageMessage(FEObj_editkeycomplete, GetPackageName(), nullptr);
                if (pCurrentOption) {
                    pCurrentOption->UnsetFocus();
                }
            }

            RefreshHeader();
            break;

        case 0x72619778:
        case 0x911C0A4B:
            RefreshHeader();
            break;

        case 0x5073EF13:
            if (!bInToggleMode) {
                ScrollZoom(eSD_PREV);
            }
            break;

        case 0xD9FEEC59:
            if (!bInToggleMode) {
                ScrollZoom(eSD_NEXT);
            }
            break;

        case 0xE1FDE1D1:
            new EWorldMapOff();
            break;

        case 0xC519BFC4:
            break;
    }
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
        MapStreamer->ZoomTo(bVector2(factorInv, factorInv));
        PanToCursor(factor);
        switch (CurrentView) {
            case 0:
            case 1:
                FEDatabase->GetGameplaySettings()->LastMapZoom = CurrentZoom;
                break;
            case 3:
                FEDatabase->GetGameplaySettings()->LastPursuitMapZoom = CurrentZoom;
                break;
        }
    }
}

float WorldMap::GetZoomFactor(eWorldMapZoomLevels zoom) {
    float factor = 1.0f;

    switch (zoom) {
        case WMZ_LEVEL_1:
            factor = 2.0f;
            break;
        case WMZ_LEVEL_2:
            factor = 3.3f;
            break;
        case WMZ_LEVEL_4:
            factor = 5.0f;
            break;
    }

    return factor;
}

void WorldMap::UpdateIconVisibility(eWorldMapItemType type, bool vis) {
    for (MapItem *item = TheMapItems.GetHead(); item != TheMapItems.EndOfList(); item = item->GetNext()) {
        if (item->GetType() == type) {
            if (vis) {
                item->SetHidden(false);
                item->Show();
            } else {
                item->SetHidden(true);
                item->Hide();
            }
        }
    }
}

void WorldMap::ClearItems() {
    for (MapItem *item = TheMapItems.GetHead(); item != TheMapItems.EndOfList(); item = item->GetNext()) {
        item->Hide();
        item->ResetSize();
    }

    TheMapItems.DeleteAllElements();

    for (FEWidget *widget = Options.GetHead(); widget != Options.EndOfList(); widget = widget->GetNext()) {
        static_cast<ItemTypeToggle *>(widget)->StartExit();
    }

    ClearWidgets();
}

bool WorldMap::ClampToMapBounds(float &x, float &y) {
    bool clamped = false;

    bVector2 br;
    FEngGetBottomRight(TrackMap, br.x, br.y);
    if (x < MapTopLeft.x + 8.0f) {
        x = MapTopLeft.x + 8.0f;
        clamped = true;
    } else if (x > br.x + -8.0f) {
        x = br.x + -8.0f;
        clamped = true;
    } else if (y < MapTopLeft.y + 26.0f) {
        y = MapTopLeft.y + 26.0f;
        clamped = true;
    } else if (y > br.y + -32.0f) {
        y = br.y + -32.0f;
        clamped = true;
    }

    return clamped;
}

void WorldMap::UpdateAnalogInput() {
    if (mActionQ) {
        while (!mActionQ->IsEmpty() && !bInToggleMode) {
            ActionRef action = mActionQ->GetAction();
            float scale = 14.0f;
            switch (action.ID()) {
                case FRONTENDACTION_RUP:
                    CurrentVelocity.y = -action.Data() * scale;
                    break;
                case FRONTENDACTION_RDOWN:
                    CurrentVelocity.y = action.Data() * scale;
                    break;
                case FRONTENDACTION_RLEFT:
                    CurrentVelocity.x = -action.Data() * scale;
                    break;
                case FRONTENDACTION_RRIGHT:
                    CurrentVelocity.x = action.Data() * scale;
                    break;
            }
            mActionQ->PopAction();
        }
    }
}

void WorldMap::UpdateCursor(bool zoom_thing) {
    UpdateAnalogInput();

    if (!MapStreamer->IsZooming()) {
        float zoom = MapStreamer->GetZoomFactor();

        bVector2 pan(0.0f, 0.0f);
        MapStreamer->GetPan(pan);

        bVector2 map_center;
        bVector2 map_br;
        FEngGetCenter(TrackMap, map_center.x, map_center.y);
        FEngGetTopLeft(TrackMap, MapTopLeft.x, MapTopLeft.y);

        bVector2 pos = CursorMoveFrom;
        bVector2 delta = pos - map_center;
        delta *= zoom;
        pos = map_center + delta;

        bVector2 dpan = pan;
        float px = dpan.x;
        dpan.x = px * MapSize.x;
        float py = dpan.y;
        dpan.y = py * MapSize.y;
        delta = dpan * zoom;
        pos = pos - delta;

        ClampToMapBounds(pos.x, pos.y);
        FEngSetCenter(Cursor, pos.x, pos.y);
    } else if (!zoom_thing) {
        if (CurrentVelocity.x != 0.0f || CurrentVelocity.y != 0.0f) {
            if (!bCursorMoving) {
                const u32 FEObj_cursoractive = 0x9F710838;
                cFEng::Get()->QueuePackageMessage(FEObj_cursoractive, GetPackageName(), nullptr);
                bCursorMoving = true;
            }

            MoveCursor(CurrentVelocity.x, CurrentVelocity.y);

            if (SelectedItem) {
                bVector2 cursor;
                bVector2 pos;
                FEngGetCenter(Cursor, cursor.x, cursor.y);
                SelectedItem->GetCurrentPos(pos);

                if (bDistBetween(cursor, pos) >= fSnapDist) {
                    const unsigned int _UNSNAP = 0x7EFE8FF4;
                    FEngSetScript(Cursor, _UNSNAP, true);
                    SelectedItem = nullptr;
                    RefreshHeader();
                }
            }
        } else {
            if (bCursorMoving) {
                const u32 FEObj_cursoridle = 0x7E6687DA;
                cFEng::Get()->QueuePackageMessage(FEObj_cursoridle, GetPackageName(), nullptr);
                bCursorMoving = false;
            }

            if (SnapCursor()) {
                RefreshHeader();
            }
        }
    }
}

void WorldMap::MoveCursor(float x, float y) {
    float dx = FEngGetCenterX(Cursor) + x;
    float dy = FEngGetCenterY(Cursor) + y;

    bVector2 excess(0.0f, 0.0f);

    bVector2 bottom_right;
    FEngGetBottomRight(TrackMap, bottom_right.x, bottom_right.y);

    if (CurrentZoom != WMZ_ALL && (x != 0.0f || y != 0.0f)) {
        if (dx < MapTopLeft.x + 8.0f) {
            excess.x = MapTopLeft.x + 8.0f - dx;
        } else if (dx > bottom_right.x + -8.0f) {
            excess.x = dx - (bottom_right.x + -8.0f);
        } else if (dy < MapTopLeft.y + 26.0f) {
            excess.y = MapTopLeft.y + 26.0f - dy;
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
            cur_pan = cur_pan + prev_pan;

            cur_pan.x *= 0.5f;
            cur_pan.y *= 0.5f;

            cur_pan.x += 0.5f;
            cur_pan.y += 0.5f;

            MapStreamer->SetPan(cur_pan);
        }
    }

    FEngSetCenter(Cursor, bClamp(dx, MapTopLeft.x + 8.0f, bottom_right.x + -8.0f),
                  bClamp(dy, MapTopLeft.y + 26.0f, bottom_right.y + -32.0f));
}

bool WorldMap::SnapCursor() {
    bVector2 cursor;
    FEngGetCenter(Cursor, cursor.x, cursor.y);
    bVector2 item_pos;
    MapItem *snap_to = nullptr;
    float last_closest = 100000000.0f;
    for (MapItem *item = TheMapItems.GetHead(); item != TheMapItems.EndOfList(); item = item->GetNext()) {
        bVector2 pos;
        item->GetCurrentPos(pos);
        float cur_dist = bDistBetween(cursor, pos);

        if (!item->IsHidden() && cur_dist < fSnapDist && cur_dist < last_closest) {
            snap_to = item;

            last_closest = cur_dist;
            item_pos = pos;
        }
    }

    if (snap_to != nullptr) {
        FEngSetCenter(Cursor, item_pos.x, item_pos.y);
        if (snap_to == SelectedItem) {
            return false;
        }

        SelectedItem = snap_to;

        const unsigned int _SNAP = 0x001CBF71;
        FEngSetScript(Cursor, _SNAP, true);
        return true;
    }

    if (SelectedItem != nullptr) {
        const unsigned int _UNSNAP = 0x7EFE8FF4;

        FEngSetScript(Cursor, _UNSNAP, true);
        SelectedItem = snap_to;
        return true;
    }

    return false;
}

void WorldMap::PanToCursor(float to_zoom) {
    bVector2 cursor;
    FEngGetCenter(Cursor, cursor.x, cursor.y);
    bVector2 pan;
    MapStreamer->GetPan(pan);
    pan.x += 0.5f;
    pan.y += 0.5f;
    float zoom = MapStreamer->GetZoomFactor();

    bVector2 map_c;
    FEngGetCenter(TrackMap, map_c.x, map_c.y);

    bVector2 offset = cursor - map_c;

    offset.x /= MapSize.x;
    offset.y /= MapSize.y;

    zoom = 1.0f / zoom;
    bVector2 pan_to = pan + offset * zoom;
    CursorMoveFrom.x = pan_to.x * MapSize.x + MapTopLeft.x;
    CursorMoveFrom.y = pan_to.y * MapSize.y + MapTopLeft.y;

    float max_pan = 1.0f / to_zoom * 0.5f;
    pan_to.x = bClamp(pan_to.x, max_pan, 1.0f - max_pan);
    pan_to.y = bClamp(pan_to.y, max_pan, 1.0f - max_pan);

    MapStreamer->PanTo(pan_to);
}

void WorldMap::PanToPlayer() {
    bVector2 pos;
    bVector2 dir;
    ISimable *simable = (*IPlayer::GetList(PLAYER_LOCAL).begin())->GetSimable();
    GetVehicleVectors(&pos, &dir, simable);

    pos.x = (pos.x - pCurrentTrack->TrackMapCalibrationUpperLeft.x) / pCurrentTrack->TrackMapCalibrationMapWidthMetres;
    pos.y = (pCurrentTrack->TrackMapCalibrationUpperLeft.y - pos.y) / pCurrentTrack->TrackMapCalibrationMapWidthMetres + 1.0f;

    float edge = 1.0f / GetZoomFactor(static_cast<eWorldMapZoomLevels>(CurrentZoom)) * 0.5f;
    pos.x = bMin(1.0f - edge, bMax(pos.x, edge));
    pos.y = bMin(1.0f - edge, bMax(pos.y, edge));

    MapStreamer->SetPan(pos);
}

void WorldMap::Setup() {
    SetInitialPositions();

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x5bc), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x682), 0x682);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0xfbb0b78e), 0xfbb0b78e);

    TrackMap = static_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), 0x0f365871));
    FEngGetTopLeft(static_cast<FEObject *>(TrackMap), MapTopLeft.x, MapTopLeft.y);
    FEngGetSize(static_cast<FEObject *>(TrackMap), MapSize.x, MapSize.y);
    Cursor = FEngFindObject(GetPackageName(), 0xf156f6c5);

    int region_unlock = 0;
    if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 13) {
        region_unlock = 1;
    } else if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 8) {
        region_unlock = 2;
    }

    MapStreamer = new ("MapStreamer", 0) UITrackMapStreamer();
    GRaceParameters *params = GRaceStatus::Get().GetRaceParameters();
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

    {
        IPlayer *player = *IPlayer::GetList(PLAYER_LOCAL).begin();
        ISimable *isimable = player->GetSimable();
        IVehicle *ivehicle;
        if (isimable->QueryInterface(&ivehicle)) {
            IVehicleAI *ivehicleai = ivehicle->GetAIVehiclePtr();
            if (ivehicleai->GetPursuit() != nullptr) {
                CurrentView = 3;
            }
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
    float zoomFactor = 1.0f / GetZoomFactor(static_cast<eWorldMapZoomLevels>(CurrentZoom));
    MapStreamer->SetZoom(bVector2(zoomFactor, zoomFactor));
    SetInitialOption(0);
    RefreshHeader();
}

void WorldMap::AddMapItemOption(unsigned int name_hash, eWorldMapItemType type) {
    ItemTypeToggle *option = new ("ItemTypeToggle", 0)
        ItemTypeToggle(name_hash, type, FEDatabase->GetGameplaySettings()->IsMapItemEnabled(type));

    Minimap::GameplayIconInfo &iconInfo = Minimap::GetGameplayIconInfo(type);

    unsigned int tex_hash = 0;
    unsigned int colour = static_cast<unsigned int>(-1);

    FEObject *iconObj = FEngFindObject(GetPackageName(), FEngHashString(iconInfo.mElementString, 0));
    if (iconObj) {
        colour = FEngGetColor(iconObj);
        tex_hash = FEngGetTextureHash(reinterpret_cast<FEImage *>(iconObj));
    }

    option->SetIcon(GetCurrentFEImage("OPTION_ICON_"), tex_hash, colour);
    option->SetIconGroup(GetCurrentFEObject("ICON_VIS_GROUP_"));
    AddButtonOption(option);
}

void WorldMap::AddPlayerCar() {
    const uint32 FEObj_PlayerCarIndicator = 0xdd9ef5ff;
    FEImage *icon = FEngFindImage(GetPackageName(), FEObj_PlayerCarIndicator);
    IPlayer *player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable *isimable = player->GetSimable();
    bVector2 target_pos;
    bVector2 target_dir;
    bVector2 world_pos;
    GetVehicleVectors(&target_pos, &target_dir, isimable);
    world_pos = target_pos;
    ConvertPos(target_pos);
    float rot = ConvertRot(target_dir);
    TheMapItems.AddTail(new ("MapItem", 0) MapItem(WMIT_PLAYER_CAR, icon, target_pos, world_pos, rot, nullptr));
}

void WorldMap::AddCops() {
    int img_num = 0;
    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_AICOPS);

    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        if (!(*iter)->IsActive()) {
            continue;
        }

        IPursuitAI *ipursuitai;
        MapItem *item = nullptr;
        bVector2 target_pos;
        bVector2 target_dir;
        bVector2 world_pos;

        (*iter)->QueryInterface(&ipursuitai);

        ISimable *isimable = (*iter)->GetSimable();
        GetVehicleVectors(&target_pos, &target_dir, isimable);
        world_pos = target_pos;
        ConvertPos(target_pos);
        float rot = ConvertRot(target_dir);

        if (ipursuitai == nullptr || !ipursuitai->WasWithinEngagementRadius()) {
            continue;
        }

        if ((*iter)->GetVehicleClass() == VehicleClass::CHOPPER) {
            AddMapItemOption(0xEAD9BD85, WMIT_COP_HELI);

            const unsigned int FEObj_HELICOPTERICONGROUP = 0xE26BE422;
            FEObject *icon = FEngFindObject(GetPackageName(), FEObj_HELICOPTERICONGROUP);
            const unsigned int FEObj_HelicopterLineOfSight = 0x21390E47;
            FEImage *view = FEngFindImage(GetPackageName(), static_cast<int>(FEObj_HelicopterLineOfSight));

            item = new ("HeliItem", 0) HeliItem(view, icon, target_pos, world_pos, rot);
        } else {
            FEImage *icon = FEngFindImage(GetPackageName(), FEngHashString("MMICON_COPCAR_%d", img_num++));

            item = new ("CopItem", 0) CopItem(icon, target_pos, world_pos, rot, WMIT_COP_CAR);
        }

        TheMapItems.AddTail(item);
    }

    if (img_num > 0) {
        AddMapItemOption(0xEAD6EF6C, WMIT_COP_CAR);
    }
}

void WorldMap::AddRoadBlocks() {
    int img_num = 0;
    const IRoadBlock::List &blocks = IRoadBlock::GetList();

    for (IRoadBlock::List::const_iterator i = blocks.begin(); i != blocks.end(); ++i) {
        IRoadBlock *rb = *i;
        UMath::Vector3 pos = rb->GetRoadBlockCentre();
        UMath::Vector3 dir = rb->GetRoadBlockDir();
        bVector2 target_pos;
        bVector2 target_dir;
        bVector2 world_pos;

        target_pos.y = -pos.x;
        target_pos.x = pos.z;
        target_dir.y = -dir.x;
        target_dir.x = dir.z;
        world_pos = target_pos;

        ConvertPos(target_pos);
        float rot = ConvertRot(target_dir);

        FEImage *icon = FEngFindImage(GetPackageName(), FEngHashString("MMICON_ROADBLOCK_%d", img_num++));

        TheMapItems.AddTail(new ("MapItem", 0) MapItem(WMIT_ROADBLOCK, icon, target_pos, world_pos, rot, nullptr));
    }

    if (img_num > 0) {
        AddMapItemOption(0x411F1F86, WMIT_ROADBLOCK);
    }
}

void WorldMap::AddIcon(eWorldMapItemType type, uint32 icon_hash, GIcon *icon) {
    if (icon_hash != 0 && icon != nullptr) {
        FEImage *image = FEngFindImage(GetPackageName(), icon_hash);
        if (image == nullptr) {
            return;
        }
        bVector2 pos2D;
        bVector2 dir2D;
        icon->GetPosition2D(pos2D);
        dir2D.x = 1.0f;
        dir2D.y = 0.0f;
        bVector2 world_pos = pos2D;
        ConvertPos(pos2D);
        MapItem *item = new ("MapItem", 0) MapItem(type, image, pos2D, world_pos, 0.0f, icon);
        TheMapItems.AddTail(item);
    }
}

void WorldMap::AddIcons(GIcon::Type desiredIconType) {
    GIcon *sortedIcons[200];
    int numIcons = GManager::Get().GatherVisibleIcons(sortedIcons, IPlayer::First(PLAYER_LOCAL));
    int numIconsPlaced = 0;

    for (int onIcon = 0; onIcon < numIcons; onIcon++) {
        GIcon *icon = sortedIcons[onIcon];
        GIcon::Type iconType = icon->GetType();
        Minimap::GameplayIconInfo &iconInfo = Minimap::GetGameplayIconInfo((Minimap::Type)iconType);

        if (iconInfo.mItemType == WMIT_NONE) {
            continue;
        }

        if (iconType != desiredIconType) {
            continue;
        }

        AddIcon(iconInfo.mItemType, FEngHashString(iconInfo.mElementString, numIconsPlaced++), icon);
    }

    if (numIconsPlaced > 0) {
        Minimap::GameplayIconInfo &desiredIconInfo = Minimap::GetGameplayIconInfo((Minimap::Type)desiredIconType);
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
    FEngSetInvisible(GetPackageName(), 0xA808E057);
    FEngSetInvisible(GetPackageName(), 0x95FDFC4E);

    AddIcons(GIcon::kType_GateSafehouse);
    AddIcons(GIcon::kType_PursuitBreaker);
    AddIcons(GIcon::kType_HidingSpot);
    AddCops();
    AddRoadBlocks();
}

void WorldMap::ConvertPos(bVector2 &pos) {
    pos.x = (pos.x - pCurrentTrack->TrackMapCalibrationUpperLeft.x) / pCurrentTrack->TrackMapCalibrationMapWidthMetres;
    pos.y = (pCurrentTrack->TrackMapCalibrationUpperLeft.y - pos.y) / pCurrentTrack->TrackMapCalibrationMapWidthMetres + 1.0f;
    pos.x = MapTopLeft.x + pos.x * MapSize.x;
    pos.y = MapTopLeft.y + pos.y * MapSize.y;
}

float WorldMap::ConvertRot(bVector2 &dir) {
    return static_cast<unsigned int>(bATan(dir.y, dir.x)) * 0.0054931640625f;
}

void WorldMap::DrawItemType() {
    Minimap::GameplayIconInfo &info = Minimap::GetGameplayIconInfo(SelectedItem->GetType());

    FEngSetLanguageHash(GetPackageName(), 0x9331FD4F, info.mWorldMapTitle);

    if (info.mWorldMapTitle) {
        FEngSetVisible(GetPackageName(), 0x9331FD4F);
    } else {
        FEngSetInvisible(GetPackageName(), 0x9331FD4F);
    }
}

void WorldMap::DrawItemStats() {
    IPlayer *player = *IPlayer::GetList(PLAYER_LOCAL).begin();
    ISimable *isimable = player->GetSimable();
    UMath::Vector3 player_pos = isimable->GetPosition();

    bVector2 real_player;
    bVector2 real_trigger;

    real_player.y = -player_pos.x;
    real_player.x = player_pos.z;

    SelectedItem->GetWorldPos(real_trigger);

    float distance = bDistBetween(real_trigger, real_player);

    bool kph = true;
    const char *distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569A26A);
    } else {
        kph = false;
        distUnits = GetLocalizedString(0x867DCFD9);
    }

    if (SelectedItem->GetType() != WMIT_PLAYER_CAR) {
        float length = distance * (kph ? 0.001f : 0.000625f);
        FEPrintf(GetPackageName(), 0xFEEEB39B, "%$.1f %s", length, distUnits);
        FEngSetVisible(GetPackageName(), 0xFEEEB39B);
    } else {
        FEngSetInvisible(GetPackageName(), 0xFEEEB39B);
    }

    Minimap::GameplayIconInfo &desiredIconInfo = Minimap::GetGameplayIconInfo(SelectedItem->GetType());

    if (desiredIconInfo.mworldIconTexHash) {
        FEngSetTextureHash(GetPackageName(), 0x9A5AB124, desiredIconInfo.mworldIconTexHash);
        FEngSetVisible(GetPackageName(), 0x9A5AB124);
    } else {
        FEngSetInvisible(GetPackageName(), 0x9A5AB124);
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

    uint32 zoom_hash = 0x213587bf;
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

    const u32 FEObj_GREY = 0x163c76;
    const u32 FEObj_NORMAL = FEHASH_NORMAL;

    // TODO
    uint32 gps_group = 0;
    uint32 txt_gps = 0;

    if (pCurrentOption != nullptr && bInToggleMode) {
        ItemTypeToggle *tog = static_cast<ItemTypeToggle *>(pCurrentOption);
        if (tog->GetVisibility()) {
            FEngSetScript(GetPackageName(), 0x32490131, FEObj_NORMAL, true);
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x2c35ec64);
        } else {
            FEngSetScript(GetPackageName(), 0x32490131, FEObj_NORMAL, true);
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0xba0a6a2b);
        }
        FEngSetLanguageHash(GetPackageName(), 0x51f0064f, 0x58b828ed);
    } else {
        IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
        if (iplayer == nullptr) {
            return;
        }

        ISimable *isimable = iplayer->GetSimable();
        if (isimable == nullptr) {
            return;
        }

        if (SelectedItem != nullptr && SelectedItem->GetIcon() != nullptr) {
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x43512519);
            FEngSetScript(GetPackageName(), 0x32490131, FEObj_NORMAL, true);
        } else if (mGPSingIcon != nullptr) {
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0xf1d0d8a5);
            FEngSetScript(GetPackageName(), 0x32490131, FEObj_NORMAL, true);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0x29456cc8, 0x43512519);
            FEngSetScript(GetPackageName(), 0x32490131, FEObj_GREY, true);
        }
        FEngSetLanguageHash(GetPackageName(), 0x51f0064f, 0x001335f0);
    }
}
