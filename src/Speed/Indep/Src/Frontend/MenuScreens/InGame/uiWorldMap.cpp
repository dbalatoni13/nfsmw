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
unsigned char FEngGetLastButton(const char* pkg_name);
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

void WorldMap::AddIcons(GRace::Type type) {
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
