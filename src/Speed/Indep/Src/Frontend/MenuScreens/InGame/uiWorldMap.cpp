#include "uiWorldMap.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

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
unsigned int FEngGetLastButton(const char* pkg_name);
void FEngSetRotationZ(FEObject* obj, float rot);
void FEngSetPosition(FEObject* obj, float x, float y);
const char* GetLocalizedString(unsigned int hash);

extern unsigned int iCurrentViewBin;

GIcon* WorldMap::mGPSingIcon;

void WorldMap::SetGPSing(GIcon* icon) {
    mGPSingIcon = icon;
}

void WorldMap::ClearGPSing() {
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
    case ZOOM_1X: return 1.0f;
    case ZOOM_2X: return 2.0f;
    case ZOOM_4X: return 4.0f;
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

void WorldMap::ClampToMapBounds(float& x, float& y) {
    if (x < MapTopLeft.x) x = MapTopLeft.x;
    if (x > MapTopLeft.x + MapSize.x) x = MapTopLeft.x + MapSize.x;
    if (y < MapTopLeft.y) y = MapTopLeft.y;
    if (y > MapTopLeft.y + MapSize.y) y = MapTopLeft.y + MapSize.y;
}

void WorldMap::UpdateAnalogInput() {
}

void WorldMap::UpdateCursor(bool snap) {
}

void WorldMap::MoveCursor(float dx, float dy) {
}

void WorldMap::SnapCursor() {
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

void WorldMap::AddIcons(GIcon::Type type) {
}

void WorldMap::SetupNavigation() {
}

void WorldMap::SetupEvent() {
}

void WorldMap::SetupPursuit() {
}

void WorldMap::ConvertPos(bVector2& pos) {
}

void WorldMap::ConvertRot(bVector2& rot) {
}

void WorldMap::DrawItemType() {
}

void WorldMap::DrawItemStats() {
}

void WorldMap::RefreshHeader() {
}
