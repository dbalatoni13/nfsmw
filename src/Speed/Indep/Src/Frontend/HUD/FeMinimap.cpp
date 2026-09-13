#include "Speed/Indep/Src/Frontend/HUD/feMinimap.hpp"

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float MinimapPivotX;
extern float MinimapPivotY;
extern float MinimapDispX;
extern float MinimapMaxSpeed;
extern bool MinimapShowNonPursuitCops;
extern bool MinimapShowPursuitCops;
extern RaceParameters TheRaceParameters;

Minimap::GameplayIconInfo Minimap::kGameplayIconInfo[GIcon::kType_Count]; // size: 0x0, address: 0x8041BCA0, Decl: 69

void GetVehicleVectors(bVector2 *pos, bVector2 *dir, ISimable *isimable) {
    UMath::Vector3 position = isimable->GetPosition();
    pos->y = -position.x;
    pos->x = position.z;
    UMath::Vector3 forwardVec;
    ICollisionBody *irigidbody;
    if (isimable->QueryInterface(&irigidbody)) {
        forwardVec = irigidbody->GetForwardVector();
        dir->y = -forwardVec.x;
        dir->x = forwardVec.z;
    }
}

static int LoaderMiniMap(bChunk *chunk) {
    return gChoppedMiniMapManager->Loader(chunk);
}

static int UnloaderMiniMap(bChunk *chunk) {
    return gChoppedMiniMapManager->Unloader(chunk);
}

static bChunkLoader bChunkLoaderMiniMap(0x3A100, LoaderMiniMap, UnloaderMiniMap);

Minimap::Minimap(const char *pkg_name, int player_number) : HudElement(pkg_name, 0x40010000), TrackmapArtUVs() {
    mCopFlashCounter = -1;
    mMapDefaultPos.z = 0.0f;
    mMapDefaultPos.y = 0.0f;
    mMapDefaultPos.x = 0.0f;
    mSpeedZoomScale = 0.0f;
    mPolyRotation = 0.0f;
    MinimapPivotX = 0.0f;
    mTrackTargetNormalized.x = 0.0f;
    mTrackTargetNormalized.y = 0.0f;
    mTrackMapCentre.x = 0.0f;
    mTrackMapCentre.y = 0.0f;

    for (int i = 0; i < mMapPieces; i++) {
        TrackmapArt[i] = static_cast<FEMultiImage *>(RegisterMultiImage(FEngHashString("TRACK_MAP%d", i + 1)));
        if (TrackmapArt[i] != nullptr) {
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

    for (int i = 0; i < mMaxElements; i++) {
        mCopElementArt[i] = RegisterImage(FEngHashString("MMICON_COPCAR_%d", i));
        mRacerElementArt[i] = RegisterImage(FEngHashString("MMICON_AIRACER_%d", i));
        for (int onType = 0; onType < GIcon::kType_Count; onType++) {
            if (kGameplayIconInfo[onType].mItemType != 0) {
                if (i == 0 || (bStrStr(kGameplayIconInfo[onType].mElementString, "%d") != nullptr)) {
                    mGameplayIcons[onType][i] = RegisterImage(FEngHashString(kGameplayIconInfo[onType].mElementString, i));
                    if (mGameplayIcons[onType][i] != nullptr) {
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

    if (TrackmapLayout != nullptr) {
        mMapDefaultPos = TrackmapLayout->GetObjData()->Pos;
        mTrackMapCentre.x = FEngGetCenterX(TrackmapLayout);
        mTrackMapCentre.y = FEngGetCenterY(TrackmapLayout);
    }

    InitStaticMiniMapItems();
}

Minimap::~Minimap() {
    gChoppedMiniMapManager->RemoveUncompressedMaps();
}

// UNSOLVED
void Minimap::SetupMinimap(IPlayer *player) {
    CurrentTrack = TrackInfo::GetTrackInfo(TheRaceParameters.TrackNumber);

    const int num_chops = 8;
    i16 chop_nums[4] = {0};
    bVector2 map_pos;
    bVector2 target_pos;
    bVector2 target_dir;

    ISimable *isimable = player->GetSimable();
    GetVehicleVectors(&target_pos, &target_dir, isimable);
    ConvertPos(target_pos, map_pos, CurrentTrack);

    map_pos.x *= num_chops;
    map_pos.y *= num_chops;

    int XSection = static_cast<int>(map_pos.x);
    float XSection_decimal = map_pos.x - XSection;
    int YSection = static_cast<int>(map_pos.y);
    float YSection_decimal = map_pos.y - YSection;
    float SectionSize = 128.0f;

    if (XSection_decimal < 0.5f) {
        if (YSection_decimal < 0.5f) {
            chop_nums[0] = (YSection - 1) * num_chops + XSection - 1;
            chop_nums[1] = (YSection - 1) * num_chops + XSection;
            chop_nums[2] = YSection * num_chops + XSection - 1;
            chop_nums[3] = YSection * num_chops + XSection;
        } else {
            chop_nums[0] = YSection * num_chops + XSection - 1;
            chop_nums[1] = YSection * num_chops + XSection;
            chop_nums[2] = (YSection + 1) * num_chops + XSection - 1;
            chop_nums[3] = (YSection + 1) * num_chops + XSection;
            YSection_decimal -= 1.0f;
        }
    } else {
        if (YSection_decimal < 0.5f) {
            chop_nums[0] = (YSection - 1) * num_chops + XSection;
            chop_nums[1] = (YSection - 1) * num_chops + XSection + 1;
            chop_nums[2] = YSection * num_chops + XSection;
            chop_nums[3] = YSection * num_chops + XSection + 1;
            XSection_decimal -= 1.0f;
        } else {
            chop_nums[0] = YSection * num_chops + XSection;
            chop_nums[1] = YSection * num_chops + XSection + 1;
            chop_nums[2] = (YSection + 1) * num_chops + XSection;
            chop_nums[3] = (YSection + 1) * num_chops + XSection + 1;
            XSection_decimal -= 1.0f;
            YSection_decimal -= 1.0f;
        }
    }

    gChoppedMiniMapManager->UncompressMaps(chop_nums, 4);

    float xDisp;
    float yDisp;
    char texture_name[128];
    for (int i = 0; i < mMapPieces; i++) {
        gChoppedMiniMapManager->GetTextureName(texture_name, sizeof(texture_name), chop_nums[i]);
        uint32 texture_hash = FEngHashString(texture_name);
        FEngSetTextureHash(TrackmapArt[i], texture_hash);
    }
    float uvScale = mSpeedZoomScale - 1;

    xDisp = XSection_decimal * mSpeedZoomScale;
    yDisp = YSection_decimal * mSpeedZoomScale;

    FEVector2 top_left = FEVector2(uvScale, uvScale);
    FEVector2 bottom_right = FEVector2(1.0f, 1.0f);
    TrackmapArt[0]->SetTopLeft(top_left, false);
    TrackmapArt[0]->SetBottomRight(bottom_right, false);

    top_left = FEVector2(0.0f, uvScale);
    bottom_right = FEVector2(1.0f - uvScale, 1.0f);
    TrackmapArt[1]->SetTopLeft(top_left, false);
    TrackmapArt[1]->SetBottomRight(bottom_right, false);

    top_left = FEVector2(uvScale, 0.0f);
    bottom_right = FEVector2(1.0f, 1.0f - uvScale);
    TrackmapArt[2]->SetTopLeft(top_left, false);
    TrackmapArt[2]->SetBottomRight(bottom_right, false);

    top_left = FEVector2(0.0f, 0.0f);
    bottom_right = FEVector2(1.0f - uvScale, 1.0f - uvScale);
    TrackmapArt[3]->SetTopLeft(top_left, false);
    TrackmapArt[3]->SetBottomRight(bottom_right, false);

    top_left = FEVector2(-xDisp, -yDisp);
    bottom_right = FEVector2(-xDisp, -yDisp);
    xDisp *= SectionSize;
    yDisp *= SectionSize;
    TrackmapArt[0]->SetUVs(0, TrackmapArtUVs[0][0] + top_left, TrackmapArtUVs[0][1] + bottom_right);
    TrackmapArt[1]->SetUVs(0, TrackmapArtUVs[1][0] + top_left, TrackmapArtUVs[1][1] + bottom_right);
    TrackmapArt[2]->SetUVs(0, TrackmapArtUVs[2][0] + top_left, TrackmapArtUVs[2][1] + bottom_right);
    TrackmapArt[3]->SetUVs(0, TrackmapArtUVs[3][0] + top_left, TrackmapArtUVs[3][1] + bottom_right);

    TrackmapLayout->GetObjData()->Pos = FEVector3(mMapDefaultPos.x - xDisp, mMapDefaultPos.y - yDisp, mMapDefaultPos.z);
    TrackmapLayout->GetObjData()->Pivot = FEVector3(xDisp + MinimapPivotX, yDisp + MinimapPivotY, 0.0f);
}

void Minimap::RefreshMapItems() {
    for (MiniMapItem *item = StaticMiniMapItems.GetHead(); item != StaticMiniMapItems.EndOfList(); item = item->GetNext()) {
        FEngSetInvisible(item->pIcon);
    }
    StaticMiniMapItems.DeleteAllElements();
    InitStaticMiniMapItems();
}

void Minimap::ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track) {
    minimapPos.x = (worldPos.x - *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xAC)) /
                   *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4);
    minimapPos.y = (*reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB0) - worldPos.y) /
                       *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4) +
                   1.0f;
}

// UNSOLVED MinimapRotateWithPlayer
void Minimap::Update(IPlayer *player) {
    if (IsElementVisible() && (player != nullptr)) {
        if (player->GetSimable() == nullptr) {
            return;
        }

        MinimapRotateWithPlayer = 1;
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            MinimapRotateWithPlayer = 0;
        } else if (((GRaceStatus::Get().GetRaceParameters() == nullptr) ? FEDatabase->GetGameplaySettings()->ExploringMiniMapMode
                                                                        : FEDatabase->GetGameplaySettings()->RacingMiniMapMode) == 0) {
            MinimapRotateWithPlayer = 0;
        }

        SetupMinimap(player);

        bVector2 target_pos;
        bVector2 target_dir;
        float speed = 0.0f;
        ISimable *isimable = player->GetSimable();
        IVehicle *ivehicle = nullptr;

        GetVehicleVectors(&target_pos, &target_dir, isimable);

        if (isimable->QueryInterface(&ivehicle)) {
            speed = bAbs(ivehicle->GetSpeed());
        }

        mPolyRotation = bAngToDeg(bATan(target_dir.y, target_dir.x));
        ConvertPos(target_pos, mTrackTargetNormalized, CurrentTrack);

        if (speed > MinimapMaxSpeed) {
            speed = MinimapMaxSpeed;
        } else if (speed < 0.0f) {
            speed = 0.0f;
        }

        mSpeedZoomScale = 1.0f - speed / MinimapMaxSpeed;
        if (mSpeedZoomScale < 1.0f) {
            mSpeedZoomScale = 1.0f;
        }

        UpdateTrackMapArt();
        if (!MinimapRotateWithPlayer) {
            mPolyRotation = 0.0f;
        }

        UpdateCopElements(ivehicle);
        UpdateAiRacerElements();
        UpdatePlayer2Element();
        UpdateRaceElements();
        UpdateGameplayIcons(player);
    }
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

void Minimap::UpdateElementArt(bVector2 *elementPos, bVector2 *elementDir, FEObject *elementArt, bool pulse) {
    bVector2 mapPos;
    ConvertPos(*elementPos, mapPos, CurrentTrack);

    float epoly_x = (mapPos.x - mTrackTargetNormalized.x) * mSpeedZoomScale;
    float epoly_y = (mapPos.y - mTrackTargetNormalized.y) * mSpeedZoomScale;
    const float sa = bSin(bDegToRad(mPolyRotation));
    const float ca = bCos(bDegToRad(mPolyRotation));
    float rot_epoly_x = epoly_x * ca + epoly_y * sa;
    float rot_epoly_y = epoly_y * ca - epoly_x * sa;
    float distance = bSqrt(rot_epoly_x * rot_epoly_x + rot_epoly_y * rot_epoly_y);
    float alpha = 1.0f;

    if (distance > 0.0f) {
        if (distance > 0.06f) {
            float scaleDist = 0.06f / distance;

            rot_epoly_y *= scaleDist;
            rot_epoly_x *= scaleDist;

            if (distance > 0.125f) {
                alpha = 1.0f - (distance - 0.125f) * 9.523809f;
            }
            if (distance > 0.23f) {
                alpha = 0.0f;
            }

            if (pulse) {
                alpha = 1.0f;
            }

            distance = 0.06f;
        }
    }

    if (distance <= 0.06f) {
        float screen_x = mTrackMapCentre.x + rot_epoly_x * 1024.0f;
        float screen_y = mTrackMapCentre.y + rot_epoly_y * 1024.0f;
        FEngSetCenter(elementArt, screen_x, screen_y);
        FEngSetVisible(elementArt);
        FEngSetRotationZ(elementArt, bAngToDeg(bATan(elementDir->y, elementDir->x)) - mPolyRotation);

        float element_angle;
        uint32 color = FEngGetColor(elementArt) & 0x00FFFFFF;

        FEngSetColor(elementArt, color | static_cast<int>(alpha * 255.0f) << 24);

        if (pulse) {
            FEngSetVisible(mGPSSelectionElementArt);
            FEngSetCenter(mGPSSelectionElementArt, screen_x, screen_y);
        }
    } else {
        FEngSetInvisible(elementArt);
    }
}

void Minimap::UpdateCopElements(IVehicle *ivehicle) {
    unsigned int artIter = 0;
    bool helicopterFound = false;

    if (mCopFlashCounter++ > 7) {
        mCopFlashCounter = 0;
    }

    eVehicleList list_id = VEHICLE_AICOPS;
    IPursuit *ipursuit = ivehicle->GetAIVehiclePtr()->GetPursuit();

    if (MinimapShowNonPursuitCops || ((ipursuit != nullptr) && !ipursuit->IsPursuitBailed())) {
        const IVehicle::List &vehicles = IVehicle::GetList(list_id);
        for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
            if (!(*iter)->IsActive()) {
                continue;
            }
            if (artIter > 7) {
                break;
            }

            bVector2 target_pos;
            bVector2 target_dir;
            ISimable *isimable = (*iter)->GetSimable();
            GetVehicleVectors(&target_pos, &target_dir, isimable);

            IPursuitAI *ipursuitai = nullptr;
            (*iter)->QueryInterface(&ipursuitai);
            FEObject *copArtToUse;

            if ((*iter)->GetVehicleClass() == VehicleClass::CHOPPER) {
                if (MinimapShowNonPursuitCops || ((ipursuitai != nullptr) && ipursuitai->WasWithinEngagementRadius())) {
                    AITarget *target = ipursuitai->GetPursuitTarget();
                    if ((target != nullptr) && ipursuitai->GetTimeSinceTargetSeen() <= 0.25f) {
                        if (!FEngIsScriptSet(mHeliLineOfSiteArt, FEHashUpper("TRACKING"))) {
                            FEngSetScript(mHeliLineOfSiteArt, FEHashUpper("TRACKING"), true);
                        }
                    } else {
                        if (!FEngIsScriptSet(mHeliLineOfSiteArt, FEHASH_INIT)) {
                            FEngSetScript(mHeliLineOfSiteArt, FEHASH_INIT, true);
                        }
                    }
                }
                helicopterFound = true;
                copArtToUse = mHeliElementArt;
                UpdateElementArt(&target_pos, &target_dir, copArtToUse, false);
                UpdateElementArt(&target_pos, &target_dir, mHeliLineOfSiteArt, false);
            } else {
                if (MinimapShowNonPursuitCops || ((ipursuitai != nullptr) && ipursuitai->WasWithinEngagementRadius() && MinimapShowPursuitCops)) {
                    copArtToUse = mCopElementArt[artIter];
                    UpdateElementArt(&target_pos, &target_dir, copArtToUse, false);
                } else {
                    FEngSetInvisible(mCopElementArt[artIter]);
                }
            }

            if ((*iter)->GetVehicleClass() != VehicleClass::CHOPPER) {
                unsigned int copFlasherColour = 0xFFCCCCCC;
                if ((ipursuitai != nullptr) && ipursuitai->GetInPursuit()) {
                    if (mCopFlashCounter < 3) {
                        copFlasherColour = 0xFF0000FF;
                    } else if (mCopFlashCounter >= 4 && mCopFlashCounter < 7) {
                        copFlasherColour = 0xFFA00000;
                    }
                }
                FEngSetColor(mCopElementArt[artIter], copFlasherColour);
            }
            if ((*iter)->GetVehicleClass() != VehicleClass::CHOPPER) {
                artIter++;
            }
        }
    }

    for (unsigned int i = artIter; i < 8; i++) {
        FEngSetInvisible(mCopElementArt[i]);
    }
    if (!helicopterFound) {
        FEngSetInvisible(mHeliElementArt);
        FEngSetInvisible(mHeliLineOfSiteArt);
    }
}

void Minimap::UpdateAiRacerElements() {
    unsigned int artIter = 0;
    eVehicleList listid = TheOnlineManager.IsOnlineRace() ? VEHICLE_REMOTE : VEHICLE_AIRACERS;
    const IVehicle::List &vehicles = IVehicle::GetList(listid);

    for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        if ((*iter)->IsActive()) {
            bVector2 target_pos;
            bVector2 target_dir;
            ISimable *isimable = (*iter)->GetSimable();
            GetVehicleVectors(&target_pos, &target_dir, isimable);
            UpdateElementArt(&target_pos, &target_dir, mRacerElementArt[artIter], false);
            artIter++;
        }
    }

    for (unsigned int i = artIter; i < 8; i++) {
        FEngSetInvisible(mRacerElementArt[i]);
    }
}

void Minimap::UpdatePlayer2Element() {
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        IPlayer *player2 = IPlayer::Last(PLAYER_LOCAL);
        bVector2 target_pos;
        bVector2 target_dir;
        ISimable *isimable = player2->GetSimable();
        GetVehicleVectors(&target_pos, &target_dir, isimable);
        UpdateElementArt(&target_pos, &target_dir, mPlayerCarIndicator2, false);
    }
}

bool GPS_IsEngaged();

void Minimap::UpdateIconElement(FEImage *image, GIcon *icon) {
    bVector2 pos2D;
    bVector2 dir2D;
    icon->GetPosition2D(pos2D);
    dir2D.x = 1.0f;
    dir2D.y = 0.0f;
    if (icon->GetType() != GIcon::kType_AreaUnlock && !GPS_IsEngaged() && icon->GetIsGPSing()) {
        icon->ClearGPSing();
    }
    UpdateElementArt(&pos2D, &dir2D, image, icon->GetIsGPSing());
    FEngSetRotationZ(image, 0.0f);
}

void Minimap::UpdateRaceElements() {
    UpdateMiniMapItems();
}

void Minimap::AdjustForWidescreen(bool moveOutwards) {
    float adjustment;
    if (moveOutwards) {
        adjustment = -120.0f;
        MinimapPivotX = adjustment;
        MinimapDispX = -0.9375f;
    } else {
        adjustment = 120.0f;
        MinimapPivotX = 0.0f;
        MinimapDispX = 0.9375f;
    }
    mTrackMapCentre.x += adjustment;

    for (int i = 0; i < mMapPieces; i++) {
        TrackmapArt[i]->GetObjData()->Pos.x += adjustment;
    }
    mPlayerCarIndicator->GetObjData()->Pos.x += adjustment;
    mPlayerCarIndicator->GetObjData()->Pos.y = mTrackMapCentre.y;
}

void Minimap::UpdateMiniMapItems() {
    for (MiniMapItem *item = StaticMiniMapItems.GetHead(); item != StaticMiniMapItems.EndOfList(); item = item->GetNext()) {
        if (item->bHidden) {
            FEngSetInvisible(item->pIcon);
        } else {
            FEngSetVisible(item->pIcon);

            bVector2 dir(0.0f, 1.0f);
            UpdateElementArt(&item->ItemPosition, &dir, item->pIcon, false);
            FEngSetRotationZ(item->pIcon, 0.0f);
        }
    }
}

void Minimap::InitStaticMiniMapItems() {}

void Minimap::UpdateGameplayIcons(IPlayer *player) {
    int iconsPlaced[GIcon::kType_Count];

    FEngSetInvisible(mGPSSelectionElementArt);
    bMemSet(iconsPlaced, 0, sizeof(iconsPlaced));

    GIcon *sortedIcons[200];
    int numIcons = GManager::Get().GatherVisibleIcons(sortedIcons, player);
    for (int onIcon = 0; onIcon < numIcons; onIcon++) {
        GIcon *icon = sortedIcons[onIcon];
        GIcon::Type iconType = icon->GetType();
        GameplayIconInfo &iconInfo = kGameplayIconInfo[iconType];
        FEImage *image;

        if (iconInfo.mItemType != 0 && iconsPlaced[iconType] < mMaxElements) {
            if (FEDatabase->GetGameplaySettings()->IsMapItemEnabled(iconInfo.mItemType)) {
                image = mGameplayIcons[iconType][iconsPlaced[iconType]];
                iconsPlaced[iconType]++;
                if (image != nullptr) {
                    UpdateIconElement(image, icon);
                }
            }
        }
    }

    for (int onType = 0; onType < GIcon::kType_Count; onType++) {
        for (int onHideIcon = iconsPlaced[onType]; onHideIcon < mMaxElements; onHideIcon++) {
            if (mGameplayIcons[onType][onHideIcon] != nullptr) {
                FEngSetInvisible(mGameplayIcons[onType][onHideIcon]);
            }
        }
    }
}
