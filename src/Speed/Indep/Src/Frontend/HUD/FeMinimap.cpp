#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"

extern void FEngSetRotationZ(FEObject *obj, float rot);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern float MinimapPivotX;
extern float MinimapDispX;

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *isimable) {
    UMath::Vector3 position = isimable->GetPosition();
    pos->y = -position.x;
    pos->x = position.z;
    ICollisionBody *irigidbody;
    if (isimable->QueryInterface(&irigidbody)) {
        UMath::Vector3 forwardVec = irigidbody->GetForwardVector();
        dir->y = -forwardVec.x;
        dir->x = forwardVec.z;
    }
}

void LoaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Loader(chunk);
}

void UnloaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Unloader(chunk);
}

extern unsigned int FEngHashString(const char *, ...);
extern void FEngGetCenter(FEObject *obj, float &x, float &y);
extern char *bStrStr(const char *, const char *);

Minimap::Minimap(const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40010000)
{
    for (int i = 3; i >= 0; i--) {
        for (int j = 1; j >= 0; j--) {
            TrackmapArtUVs[i][j].x = 0.0f;
            TrackmapArtUVs[i][j].y = 0.0f;
        }
    }

    mCopFlashCounter = -1;
    mMapDefaultPos.z = 0.0f;
    mMapDefaultPos.y = 0.0f;
    mMapDefaultPos.x = 0.0f;
    mSpeedZoomScale = 0.0f;
    mPolyRotation = 0.0f;
    MinimapPivotX = 0.0f;
    mTrackMapCentre.x = 0.0f;
    mTrackTargetNormalized.y = 0.0f;
    mTrackTargetNormalized.x = 0.0f;
    mTrackMapCentre.y = 0.0f;

    for (unsigned int i = 0; i < 4; i++) {
        TrackmapArt[i] = static_cast<FEMultiImage *>(RegisterMultiImage(FEngHashString("TRACK_MAP%d", i + 1)));
        if (TrackmapArt[i]) {
            TrackmapArt[i]->GetUVs(0, TrackmapArtUVs[i][0], TrackmapArtUVs[i][1]);
        }
    }

    TrackmapLayout = RegisterObject(FEngHashString("TRACK_MAP"));
    TrackmapNorth = RegisterImage(FEngHashString("MINIMAP_NORTH_INDICATOR"));
    mPlayerCarIndicator = RegisterImage(FEngHashString("PLAYERCARINDICATOR"));
    mPlayerCarIndicator2 = RegisterImage(FEngHashString("PLAYERCARINDICATOR2"));
    RegisterObject(FEngHashString("TRACKMAPTARGETRING"));
    RegisterObject(FEngHashString("MAP_COLOR_TINT"));

    bMemSet(mGameplayIcons, 0, sizeof(mGameplayIcons));

    mHeliElementArt = RegisterGroup(FEngHashString("HELICOPTER_ICON_GROUP"));
    mHeliLineOfSiteArt = RegisterImage(FEngHashString("HELICOPTER_LINE_OF_SIGHT"));

    for (unsigned int i = 0; i < 8; i++) {
        mCopElementArt[i] = RegisterImage(FEngHashString("MMICON_COPCAR_%d", i));
        mRacerElementArt[i] = RegisterImage(FEngHashString("MMICON_AIRACER_%d", i));
        for (int onType = 0; onType < 17; onType++) {
            if (kGameplayIconInfo[onType].mItemType != 0) {
                if (i == 0 || bStrStr(kGameplayIconInfo[onType].mElementString, "%d")) {
                    mGameplayIcons[onType][i] = RegisterImage(FEngHashString(kGameplayIconInfo[onType].mElementString, i));
                    if (mGameplayIcons[onType][i]) {
                        FEngSetInvisible(mGameplayIcons[onType][i]);
                    }
                } else {
                    mGameplayIcons[onType][i] = nullptr;
                }
            }
        }
    }

    mCheckpointElementArt = RegisterImage(FEngHashString("MMICON_CHECKPOINT"));
    mGPSSelectionElementArt = RegisterImage(0xE8741681);

    if (TrackmapLayout) {
        mMapDefaultPos = TrackmapLayout->GetObjData()->Pos;
        float x, y;
        FEngGetCenter(TrackmapLayout, x, y);
        mTrackMapCentre.x = x;
        mTrackMapCentre.y = y;
    }
}

Minimap::~Minimap() {
    gChoppedMiniMapManager->UncompressMaps(nullptr, 0);
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
        offset = 120.0f;
        MinimapPivotX = 0.0f;
        MinimapDispX = 0.9375f;
    }
    mTrackMapCentre.x += offset;
    for (unsigned int i = 0; i < 4; i++) {
        reinterpret_cast<FEObjData *>(TrackmapArt[i]->pData)->Pos.x += offset;
    }
    reinterpret_cast<FEObjData *>(mPlayerCarIndicator->pData)->Pos.x += offset;
    reinterpret_cast<FEObjData *>(mPlayerCarIndicator->pData)->Pos.y = mTrackMapCentre.y;
}

void Minimap::RefreshMapItems() {
    MiniMapItem *item = StaticMiniMapItems.GetHead();
    while (item != StaticMiniMapItems.EndOfList()) {
        FEngSetInvisible(item->mpIcon);
        item = item->GetNext();
    }
    StaticMiniMapItems.DeleteAllElements();
}

extern bool GPS_IsEngaged();

void Minimap::UpdateIconElement(FEImage *image, GIcon *icon) {
    bVector2 pos2D;
    bVector2 dir2D;
    icon->GetPosition2D(pos2D);
    dir2D.x = 1.0f;
    dir2D.y = 0.0f;
    if (icon->GetType() != GIcon::kType_AreaUnlock && !GPS_IsEngaged() && icon->GetIsGPSing()) {
        icon->ClearGPSing();
    }
    bool pulse = icon->GetIsGPSing();
    UpdateElementArt(&pos2D, &dir2D, image, pulse);
    FEngSetRotationZ(image, 0.0f);
}

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

void Minimap::InitStaticMiniMapItems() {}

void Minimap::UpdatePlayer2Element() {
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        IPlayer *player2 = IPlayer::Last(PLAYER_LOCAL);
        ISimable *isimable = player2->GetSimable();
        bVector2 target_pos;
        bVector2 target_dir;
        bVector2 *pPos = &target_pos;
        bVector2 *pDir = &target_dir;
        GetVehicleVectors(pPos, pDir, isimable);
        UpdateElementArt(pPos, pDir, mPlayerCarIndicator2, false);
    }
}
