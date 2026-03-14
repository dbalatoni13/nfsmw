#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"

extern void FEngSetRotationZ(FEObject *obj, float rot);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern float MinimapPivotX;
extern float MinimapDispX;

void LoaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Loader(chunk);
}

void UnloaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Unloader(chunk);
}

Minimap::Minimap(const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0)
{
}

void Minimap::Update(IPlayer *player) {
}

void Minimap::ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track) {
    minimapPos.x = (worldPos.x - *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xAC)) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4);
    minimapPos.y = (*reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB0) - worldPos.y) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4) + 1.0f;
}

void Minimap::UpdateRaceElements() {
    UpdateMiniMapItems();
}

void Minimap::UpdateTrackMapArt() {
    if (MinimapRotateWithPlayer == 0) {
        FEngSetRotationZ(mPlayerCarIndicator, mPolyRotation);
        FEngSetRotationZ(TrackmapLayout, 0.0f);
        FEngSetRotationZ(TrackmapNorth, 0.0f);
    } else {
        FEngSetRotationZ(mPlayerCarIndicator, 0.0f);
        FEngSetRotationZ(TrackmapLayout, -mPolyRotation);
        FEngSetRotationZ(TrackmapNorth, -mPolyRotation);
    }
}

void Minimap::AdjustForWidescreen(bool moveOutwards) {
    float offset;
    if (moveOutwards) {
        offset = -120.0f;
        MinimapPivotX = offset;
        MinimapDispX = -0.9375f;
    } else {
        MinimapPivotX = 0.0f;
        MinimapDispX = 0.9375f;
        offset = 120.0f;
    }
    mTrackMapCentre.x += offset;
    for (unsigned int i = 0; i < 4; i++) {
        reinterpret_cast<FEObjData *>(TrackmapArt[i]->pData)->Pos.x += offset;
    }
    reinterpret_cast<FEObjData *>(mPlayerCarIndicator->pData)->Pos.x += offset;
    reinterpret_cast<FEObjData *>(mPlayerCarIndicator->pData)->Pos.y = mTrackMapCentre.y;
}

void Minimap::InitStaticMiniMapItems() {}

void Minimap::UpdateMiniMapItems() {
    bVector2 defaultDir;
    for (MiniMapItem *item = static_cast<MiniMapItem *>(StaticMiniMapItems.GetHead());
         item != StaticMiniMapItems.EndOfList();
         item = static_cast<MiniMapItem *>(item->GetNext())) {
        if (item->mHidden) {
            FEngSetInvisible(item->mpIcon);
        } else {
            FEngSetVisible(item->mpIcon);
            defaultDir.x = 0.0f;
            defaultDir.y = 1.0f;
            UpdateElementArt(&item->mPos, &defaultDir, item->mpIcon, false);
            FEngSetRotationZ(item->mpIcon, 0.0f);
        }
    }
}
