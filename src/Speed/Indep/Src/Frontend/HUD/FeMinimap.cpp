#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
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
#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern void FEngSetRotationZ(FEObject *obj, float rot);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetCenter(FEObject *obj, float x, float y);
extern void FEngSetColor(FEObject *obj, unsigned int color);
extern FEColor FEngGetObjectColor(FEObject *obj);
extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern unsigned long FEHashUpper(const char *str);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern float MinimapPivotX;
extern float MinimapPivotY;
extern float MinimapDispX;
extern float MinimapMaxSpeed;
extern bool MinimapShowNonPursuitCops;
extern bool MinimapShowPursuitCops;
extern RaceParameters TheRaceParameters;

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

int LoaderMiniMap(bChunk *chunk) {
    return gChoppedMiniMapManager->Loader(chunk);
}

int UnloaderMiniMap(bChunk *chunk) {
    return gChoppedMiniMapManager->Unloader(chunk);
}

static bChunkLoader bChunkLoaderMiniMap(0x3A100, LoaderMiniMap, UnloaderMiniMap);

extern unsigned int FEngHashString(const char *, ...);
extern void FEngGetCenter(FEObject *obj, float &x, float &y);
extern char *bStrStr(const char *, const char *);

bool GIcon::IsFlagSet(unsigned int mask) const {
    return (mFlags & mask) != 0;
}

void Physics::Tunings::Default() {
    bMemSet(this, 0, 0x1C);
}

void MD5::Reset() {
    uCount = 0;
    uRegs[0] = 0x67452301;
    uRegs[1] = 0xEFCDAB89;
    uRegs[2] = 0x98BADCFE;
    uRegs[3] = 0x10325476;
    computed = false;
}

namespace UTL {
namespace Collections {

template <>
IPlayer *ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::Last(ePlayerList idx) {
    ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::List &l = _mLists._buckets[idx];
    if (l.size() != 0) {
        return l[l.size() - 1];
    }
    return nullptr;
}

} // namespace Collections
} // namespace UTL

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
    if (!IsElementVisible() || !player) {
        return;
    }

    ISimable *isimable = player->GetSimable();
    if (!isimable) {
        return;
    }

    MinimapRotateWithPlayer = 1;
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        MinimapRotateWithPlayer = 0;
    } else {
        unsigned char rotate_with_player =
            GRaceStatus::Get().GetRaceParameters() == nullptr ? FEDatabase->GetGameplaySettings()->ExploringMiniMapMode
                                                              : FEDatabase->GetGameplaySettings()->RacingMiniMapMode;
        if (!rotate_with_player) {
            MinimapRotateWithPlayer = 0;
        }
    }

    SetupMinimap(player);

    IVehicle *ivehicle = nullptr;
    float speed = 0.0f;
    bVector2 target_pos;
    bVector2 target_dir;
    isimable = player->GetSimable();
    GetVehicleVectors(&target_pos, &target_dir, isimable);

    if (isimable->QueryInterface(&ivehicle)) {
        speed = bAbs(ivehicle->GetSpeed());
    }

    mPolyRotation = bAngToDeg(bATan(target_dir.y, target_dir.x));
    ConvertPos(target_pos, mTrackTargetNormalized, CurrentTrack);

    if (speed > MinimapMaxSpeed) {
        speed = MinimapMaxSpeed;
    }
    if (speed < 0.0f) {
        speed = 0.0f;
    }

    mSpeedZoomScale = 2.0f - speed / MinimapMaxSpeed;
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

void Minimap::SetupMinimap(IPlayer *player) {
    const int num_chops = 8;
    char texture_name[128];
    FEVector2 top_left;
    FEVector2 bottom_right;
    short chop_nums[4];
    bVector2 map_pos;
    bVector2 target_pos;
    bVector2 target_dir;

    CurrentTrack = TrackInfo::GetTrackInfo(TheRaceParameters.TrackNumber);

    ISimable *isimable = player->GetSimable();
    GetVehicleVectors(&target_pos, &target_dir, isimable);
    ConvertPos(target_pos, map_pos, CurrentTrack);

    map_pos.x *= static_cast<float>(num_chops);
    map_pos.y *= static_cast<float>(num_chops);

    int XSection = static_cast<int>(map_pos.x);
    int YSection = static_cast<int>(map_pos.y);
    float XSection_decimal = map_pos.x - static_cast<float>(XSection);
    float YSection_decimal = map_pos.y - static_cast<float>(YSection);

    if (XSection_decimal < 0.5f) {
        if (YSection_decimal < 0.5f) {
            chop_nums[0] = (YSection - 1) * 8 + XSection - 1;
            chop_nums[1] = (YSection - 1) * 8 + XSection;
            chop_nums[2] = YSection * 8 + XSection - 1;
            chop_nums[3] = YSection * 8 + XSection;
        } else {
            chop_nums[0] = YSection * 8 + XSection - 1;
            chop_nums[1] = YSection * 8 + XSection;
            chop_nums[2] = (YSection + 1) * 8 + XSection - 1;
            chop_nums[3] = (YSection + 1) * 8 + XSection;
            YSection_decimal -= 1.0f;
        }
    } else {
        if (YSection_decimal < 0.5f) {
            chop_nums[0] = (YSection - 1) * 8 + XSection;
            chop_nums[1] = (YSection - 1) * 8 + XSection + 1;
            chop_nums[2] = YSection * 8 + XSection;
            chop_nums[3] = YSection * 8 + XSection + 1;
            XSection_decimal -= 1.0f;
        } else {
            chop_nums[0] = YSection * 8 + XSection;
            chop_nums[1] = YSection * 8 + XSection + 1;
            chop_nums[2] = (YSection + 1) * 8 + XSection;
            chop_nums[3] = (YSection + 1) * 8 + XSection + 1;
            YSection_decimal -= 1.0f;
            XSection_decimal -= 1.0f;
        }
    }

    gChoppedMiniMapManager->UncompressMaps(chop_nums, 4);

    for (unsigned int i = 0; i < 4; i++) {
        gChoppedMiniMapManager->GetTextureName(texture_name, 0x80, chop_nums[i]);
        unsigned int hash = FEngHashString(texture_name);
        FEngSetTextureHash(TrackmapArt[i], hash);
    }

    float SectionSize = mSpeedZoomScale;
    float uvScale = SectionSize - 1.0f;

    top_left.x = uvScale;
    top_left.y = uvScale;
    bottom_right.x = 1.0f;
    bottom_right.y = 1.0f;
    TrackmapArt[0]->SetTopLeft(top_left, false);
    TrackmapArt[0]->SetBottomRight(bottom_right, false);

    top_left.x = 0.0f;
    top_left.y = uvScale;
    bottom_right.x = 1.0f - uvScale;
    bottom_right.y = 1.0f;
    TrackmapArt[1]->SetTopLeft(top_left, false);
    TrackmapArt[1]->SetBottomRight(bottom_right, false);

    top_left.x = uvScale;
    top_left.y = 0.0f;
    bottom_right.x = 1.0f;
    bottom_right.y = 1.0f - uvScale;
    TrackmapArt[2]->SetTopLeft(top_left, false);
    TrackmapArt[2]->SetBottomRight(bottom_right, false);

    top_left.x = 0.0f;
    top_left.y = 0.0f;
    bottom_right.x = 1.0f - uvScale;
    bottom_right.y = 1.0f - uvScale;
    TrackmapArt[3]->SetTopLeft(top_left, false);
    TrackmapArt[3]->SetBottomRight(bottom_right, false);

    float xDisp = -(XSection_decimal * SectionSize);
    float yDisp = -(YSection_decimal * SectionSize);

    top_left.x = TrackmapArtUVs[0][0].x + xDisp;
    top_left.y = TrackmapArtUVs[0][0].y + yDisp;
    bottom_right.x = TrackmapArtUVs[0][1].x + xDisp;
    bottom_right.y = TrackmapArtUVs[0][1].y + yDisp;
    TrackmapArt[0]->SetUVs(0, top_left, bottom_right);

    top_left.x = TrackmapArtUVs[1][0].x + xDisp;
    top_left.y = TrackmapArtUVs[1][0].y + yDisp;
    bottom_right.x = TrackmapArtUVs[1][1].x + xDisp;
    bottom_right.y = TrackmapArtUVs[1][1].y + yDisp;
    TrackmapArt[1]->SetUVs(0, top_left, bottom_right);

    top_left.x = TrackmapArtUVs[2][0].x + xDisp;
    top_left.y = TrackmapArtUVs[2][0].y + yDisp;
    bottom_right.x = TrackmapArtUVs[2][1].x + xDisp;
    bottom_right.y = TrackmapArtUVs[2][1].y + yDisp;
    TrackmapArt[2]->SetUVs(0, top_left, bottom_right);

    top_left.x = TrackmapArtUVs[3][0].x + xDisp;
    top_left.y = TrackmapArtUVs[3][0].y + yDisp;
    bottom_right.x = TrackmapArtUVs[3][1].x + xDisp;
    bottom_right.y = TrackmapArtUVs[3][1].y + yDisp;
    TrackmapArt[3]->SetUVs(0, top_left, bottom_right);

    FEObjData *data = TrackmapLayout->GetObjData();
    xDisp *= -128.0f;
    yDisp *= -128.0f;
    data->Pos.x = mMapDefaultPos.x - xDisp;
    data->Pos.y = mMapDefaultPos.y - yDisp;
    data->Pos.z = mMapDefaultPos.z;
    data = TrackmapLayout->GetObjData();
    data->Pivot.x = xDisp + MinimapPivotX;
    data->Pivot.y = yDisp + MinimapPivotY;
    data->Pivot.z = 0.0f;
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
    InitStaticMiniMapItems();
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

void Minimap::UpdateCopElements(IVehicle *ivehicle) {
    unsigned int artIter = 0;
    bool helicopterFound = false;
    eVehicleList list_id = VEHICLE_AICOPS;
    IPursuit *ipursuit = nullptr;

    mCopFlashCounter++;
    if (mCopFlashCounter > 7) {
        mCopFlashCounter = 0;
    }

    IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();
    ipursuit = ivehicleAI->GetPursuit();

    if (MinimapShowNonPursuitCops || ipursuit) {
        const IVehicle::List &vehicles = IVehicle::GetList(list_id);
        for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
            IVehicle *copVehicle = *iter;
            if (!copVehicle->IsActive()) {
                continue;
            }
            if (artIter > 7) {
                break;
            }

            bVector2 target_pos;
            bVector2 target_dir;
            ISimable *isimable = copVehicle->GetSimable();
            GetVehicleVectors(&target_pos, &target_dir, isimable);

            IPursuitAI *ipursuitai = nullptr;
            copVehicle->QueryInterface(&ipursuitai);
            FEObject *copArtToUse;

            if (copVehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                copArtToUse = mHeliElementArt;
                if (MinimapShowNonPursuitCops || (ipursuitai && ipursuitai->GetInPursuit())) {
                    AITarget *target = ipursuitai->GetPursuitTarget();
                    if (!target || target->GetSpeed() > 0.25f) {
                        unsigned int tracking_hash = FEHashUpper("TRACKING");
                        if (!FEngIsScriptSet(mHeliLineOfSiteArt, tracking_hash)) {
                            FEngSetScript(mHeliLineOfSiteArt, tracking_hash, true);
                        }
                    } else {
                        if (!FEngIsScriptSet(mHeliLineOfSiteArt, 0x1744B3)) {
                            FEngSetScript(mHeliLineOfSiteArt, 0x1744B3, true);
                        }
                    }
                }
                helicopterFound = true;
                UpdateElementArt(&target_pos, &target_dir, copArtToUse, false);
                UpdateElementArt(&target_pos, &target_dir, mHeliLineOfSiteArt, false);
            } else {
                if (MinimapShowNonPursuitCops || (ipursuitai && ipursuitai->GetInPursuit() && MinimapShowPursuitCops)) {
                    copArtToUse = mCopElementArt[artIter];
                    UpdateElementArt(&target_pos, &target_dir, copArtToUse, false);
                } else {
                    FEngSetInvisible(mCopElementArt[artIter]);
                }

                unsigned int copFlasherColour = 0xFFCCCCCC;
                if (ipursuitai && ipursuitai->GetInPursuit()) {
                    if (mCopFlashCounter < 3) {
                        copFlasherColour = 0xFF0000FF;
                    } else if (mCopFlashCounter - 4U < 3) {
                        copFlasherColour = 0xFFA00000;
                    }
                }
                FEngSetColor(mCopElementArt[artIter], copFlasherColour);
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

void Minimap::UpdateElementArt(bVector2 *elementPos, bVector2 *elementDir, FEObject *elementArt, bool pulse) {
    bVector2 mapPos;
    ConvertPos(*elementPos, mapPos, CurrentTrack);

    float epoly_x = (mapPos.x - mTrackTargetNormalized.x) * mSpeedZoomScale;
    float epoly_y = (mapPos.y - mTrackTargetNormalized.y) * mSpeedZoomScale;
    const float sa = bSin(bDegToRad(mPolyRotation));
    const float ca = bCos(bDegToRad(mPolyRotation));
    float rot_epoly_x = epoly_y * ca - epoly_x * sa;
    float rot_epoly_y = epoly_x * ca + epoly_y * sa;
    float distance = bSqrt(rot_epoly_y * rot_epoly_y + rot_epoly_x * rot_epoly_x);
    float alpha = 1.0f;

    if (distance > 0.0f && distance > 0.06f && distance < 0.23f) {
        float scaleDist = distance;
        rot_epoly_x *= 0.06f / scaleDist;
        rot_epoly_y *= 0.06f / scaleDist;
        distance = 0.06f;

        if (scaleDist > 0.125f) {
            alpha = 1.0f - (scaleDist - 0.125f) * 9.523809f;
        }
        if (pulse) {
            alpha = 1.0f;
        }
    }

    if (distance <= 0.06f) {
        float screen_x = mTrackMapCentre.x + rot_epoly_y * 1024.0f;
        float screen_y = mTrackMapCentre.y + rot_epoly_x * 1024.0f;
        FEngSetCenter(elementArt, screen_x, screen_y);
        FEngSetVisible(elementArt);
        FEngSetRotationZ(elementArt, bAngToDeg(bATan(elementDir->y, elementDir->x)) - mPolyRotation);

        unsigned int color = static_cast<unsigned long>(FEngGetObjectColor(elementArt));
        FEngSetColor(elementArt, color & 0x00FFFFFF | static_cast<unsigned int>(alpha * 255.0f) << 24);

        if (pulse) {
            FEngSetVisible(mGPSSelectionElementArt);
            FEngSetCenter(mGPSSelectionElementArt, screen_x, screen_y);
        }
    } else {
        FEngSetInvisible(elementArt);
    }
}

void Minimap::UpdateGameplayIcons(IPlayer *player) {
    int iconsPlaced[GIcon::kType_Count];
    GIcon *sortedIcons[200];

    FEngSetInvisible(mGPSSelectionElementArt);
    bMemSet(iconsPlaced, 0, sizeof(iconsPlaced));

    int numIcons = GManager::Get().GatherVisibleIcons(sortedIcons, player);
    for (int onIcon = 0; onIcon < numIcons; onIcon++) {
        GIcon *icon = sortedIcons[onIcon];
        GIcon::Type iconType = icon->GetType();
        GameplayIconInfo &iconInfo = kGameplayIconInfo[iconType];
        FEImage *image;

        if (iconInfo.mItemType != 0 && static_cast< unsigned int >(iconsPlaced[iconType]) < 8) {
            if (FEDatabase->GetGameplaySettings()->IsMapItemEnabled(static_cast<eWorldMapItemType>(iconInfo.mItemType))) {
                image = mGameplayIcons[iconType][static_cast< unsigned int >(iconsPlaced[iconType])];
                iconsPlaced[iconType]++;
                if (image) {
                    UpdateIconElement(image, icon);
                }
            }
        }
    }

    for (int onType = 0; onType < GIcon::kType_Count; onType++) {
        for (int onHideIcon = iconsPlaced[onType]; static_cast< unsigned int >(onHideIcon) < 8; onHideIcon++) {
            if (mGameplayIcons[onType][onHideIcon]) {
                FEngSetInvisible(mGameplayIcons[onType][onHideIcon]);
            }
        }
    }
}

void Minimap::UpdateAiRacerElements() {
    unsigned int artIter = 0;
    eVehicleList listid = TheOnlineManager.IsOnlineRace() ? VEHICLE_REMOTE : VEHICLE_AIRACERS;
    const IVehicle::List &vehicles = IVehicle::GetList(listid);

    for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive()) {
            bVector2 target_pos;
            bVector2 target_dir;
            GetVehicleVectors(&target_pos, &target_dir, ivehicle->GetSimable());
            UpdateElementArt(&target_pos, &target_dir, mRacerElementArt[artIter], false);
            artIter++;
        }
    }

    for (unsigned int i = artIter; i < 8; i++) {
        FEngSetInvisible(mRacerElementArt[i]);
    }
}
