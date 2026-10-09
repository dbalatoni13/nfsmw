#include "AnimScene.hpp"
#include "AnimDirectory.hpp"
#include "AnimEntity_BasicCharacter.hpp"
#include "AnimLocator.hpp"
#include "AnimPlayer.hpp"
#include "Speed/Indep/Src/Animation/AnimBank.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

uint32 skel_ROOT_hash = bStringHash("ROOT");
extern int AnimCfg_DebugOutput;

bTList<CAnimSceneData> g_loadedAnimSceneDataList;
CarAnimationState gCarAnimationStates[16];

SlotPool *AnimPartSlotPool = nullptr;

int bEnableNisTextDisplay = 0;

bool CAnimEntityCreationContext::mIsRaceStart = false;
int CAnimScene::mHandleCounter = 0;

char Car_Name[16][16] = {
    "ZPM_Car1", "ZPM_Car2", "ZPM_Car3", "ZPM_Car4", "ZPM_Car5", "ZPM_Car6", "ZPM_Car7", "ZPM_Car8",
    "ZPM_Cop1", "ZPM_Cop2", "ZPM_Cop3", "ZPM_Cop4", "ZPM_Cop5", "ZPM_Cop6", "ZPM_Cop7", "ZPM_Cop8",
};

CarAnimationState::CarAnimationState() {
    AnimCtrl = nullptr;
    HaveLastCarPosition = 0;
    CarIndex = -1;
}

void ResetCarAnimState(IVehicle *vehicle) {
    for (int i = 0; i <= 15; i++) {
        if (gCarAnimationStates[i].CarIndex >= 0 && gCarAnimationStates[i].mIVehicle == vehicle) {
            gCarAnimationStates[i].HaveLastCarPosition = 0;
            return;
        }
    }
}

CAnimSceneData *CAnimSceneData::FindAnimSceneData(uint32 anim_id) {
    CAnimSceneData *anim_scene_data = static_cast<CAnimSceneData *>(g_loadedAnimSceneDataList.GetHead());
    for (; anim_scene_data != g_loadedAnimSceneDataList.EndOfList(); anim_scene_data = anim_scene_data->GetNext()) {
        unsigned int cur_id = anim_scene_data->GetAnimID();
        if (anim_id == cur_id) {
            return anim_scene_data;
        }
    }
    return nullptr;
}

CAnimSceneData::CAnimSceneData(bChunk *chunk)
    : mChunk(chunk), //
      mNisScene(nullptr) {}

CAnimSceneData::~CAnimSceneData() {}

void CAnimSceneData::EndianSwapHeaderData() {
    bPlatEndianSwap(reinterpret_cast<int32 *>(mNisScene));
    bPlatEndianSwap(&mNisScene->SceneType);
    bPlatEndianSwap(&mNisScene->HaveLayout);
    bPlatEndianSwap(&mNisScene->HaveCarAnimation);
    bPlatEndianSwap(&mNisScene->NumberOfCars);
    bPlatEndianSwap(&mNisScene->StartFrame);
    bPlatEndianSwap(&mNisScene->VanishFrame);
}

void CAnimSceneData::InitHeaderData(void *data, int size) {
    mNisScene = reinterpret_cast<NisScene *>(data);
    mNisScene->Description = reinterpret_cast<char *>(&mNisScene[1]); // wtf?
    EndianSwapHeaderData();
}

void CAnimSceneData::AddEntityData(void *data, int size) {
    CAnimEntityFactory::EndianSwapEntityData(data, size);

    CAnimEntityData *aed = BNEW CAnimEntityData(*reinterpret_cast<unsigned int *>(data), data, size);
    mAnimEntityDataList.AddTail(aed);
}

// STRIPPED
void CAnimSceneData::RemoveAllEntityData() {}

CAnimSceneData *CreateAnimSceneData(bChunk *nested_chunk, bChunk *sub_chunk) {
    CAnimSceneData *anim_scene_data = BNEW CAnimSceneData(nested_chunk);

    if (anim_scene_data) {
        void *data = sub_chunk->GetData();
        unsigned int size = sub_chunk->GetSize();
        anim_scene_data->InitHeaderData(data, size);
        g_loadedAnimSceneDataList.AddTail(anim_scene_data);
        return anim_scene_data;
    }
    return nullptr;
}

int LoaderAnimSceneData(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_ANIM_SCENE) {
        CAnimSceneData *anim_scene_data = nullptr;
        bChunk *sub = chunk->GetFirstChunk();
        bChunk *last = chunk->GetLastChunk();

        while (sub != last) {
            switch (sub->GetID()) {
                case BCHUNK_ANIM_SCENE_DATA:
                    anim_scene_data = CreateAnimSceneData(chunk, sub);
                    break;
                case BCHUNK_ANIM_SCENE_ENTITY_DATA:
                    if (anim_scene_data) {
                        anim_scene_data->AddEntityData(sub->GetAlignedData(16), sub->GetAlignedSize(16));
                    }
                    break;
            }

            sub = sub->GetNext();
        }
        return 1;
    }

    return 0;
}

int UnloaderAnimSceneData(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_ANIM_SCENE) {
        CAnimSceneData *anim_scene_data = g_loadedAnimSceneDataList.GetHead();
        while (anim_scene_data != g_loadedAnimSceneDataList.EndOfList()) {
            CAnimSceneData *next_anim_scene_data = anim_scene_data->GetNext();
            if (anim_scene_data->GetChunk() == chunk) {
                g_loadedAnimSceneDataList.Remove(anim_scene_data);
                delete anim_scene_data;
                break;
            }
            anim_scene_data = next_anim_scene_data;
        }
        return 1;
    }

    return 0;
}

// STRIPPED
CAnimMarker::CAnimMarker(uint32 name_hash, float time) {}

// STRIPPED
CAnimMarker::~CAnimMarker() {}

// STRIPPED
uint32 CAnimMarker::GetNameHash() {
    return mNameHash;
}

// STRIPPED
float CAnimMarker::GetTime() {
    return mTime;
};

CAnimProperty::CAnimProperty(eAnimProperty type, bool enabled)
    : mType(type), //
      mEnabled(enabled) {}

CAnimProperty::~CAnimProperty() {}

eAnimProperty CAnimProperty::GetType() {
    return mType;
}

void CAnimProperty::SetEnabled(bool enabled) {
    mEnabled = enabled;
}

bool CAnimProperty::IsEnabled() {
    return mEnabled != 0;
}

int CAnimScene::GenerateHandle() {
    mHandleCounter++;
    if (mHandleCounter > 0xFFFF) {
        mHandleCounter = 1;
    }
    return mHandleCounter;
}

CAnimScene::CAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index)
    : mHandle(0),                              //
      mAnimSceneData(anim_scene_data),         //
      mPlayStatus(Stopped),                    //
      mTimeElapsed(0.0f),                      //
      mTimeDelta(0.0f),                        //
      mTimeStart(0.0f),                        //
      mTimeTotalLength(0.0f),                  //
      mIsBoundToGame(false),                   //
      mCameraTrackNumber(camera_track_number), //
      mControllingCamera(false),               //
      mSpaceNode(nullptr),                     //
      mAnimCandidateType(anim_candidate_type), //
      mAnimCandidateIndex(anim_candidate_index) {
    mHandle = GenerateHandle();
    bIdentity(&mSceneRotationMatrix);
    bIdentity(&mSceneTranslationMatrix);
    bIdentity(&mSceneTransformMatrix);
}

CAnimScene::~CAnimScene() {}

int CAnimScene::GetHandle() {
    return mHandle;
}

uint32 CAnimScene::GetAnimID() {
    return mAnimSceneData->GetSceneInfo()->mSceneNameHash;
}

uint32 CAnimScene::GetSceneHash() {
    return GetAnimID();
}

int CAnimScene::GetSceneType() {
    return mAnimSceneData->GetSceneInfo()->SceneType;
}

void CAnimScene::GetSceneName(char *ret_name) {
    TheAnimDirectory->GetNameOfSceneHash(GetSceneHash(), ret_name);
}

int CAnimScene::GetCameraTrackNumber() {
    return mCameraTrackNumber;
}

const char *CAnimScene::GetAnimDescription() {
    return mAnimSceneData->GetSceneInfo()->Description;
}

bool CAnimScene::SetPropertyEnabled(eAnimProperty property_id, bool enable) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property) {
        anim_property->SetEnabled(enable);
        return true;
    }

    AddProperty(property_id, enable);
    return false;
}

bool CAnimScene::IsPropertyEnabled(eAnimProperty property_id) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (!anim_property) {
        return false;
    }
    return anim_property->IsEnabled();
}

// STRIPPED
bool CAnimScene::IsBoundToGame() {
    return mIsBoundToGame;
}

bool CAnimScene::BindToGame() {
    if (!mIsBoundToGame) {
        SetCameraControl(true);
        NisScene *scene_info = mAnimSceneData->GetSceneInfo();
        mTimeStart = static_cast<float>(scene_info->StartFrame) * (1.0f / 30.0f);
        mTimeTotalLength = static_cast<float>(scene_info->VanishFrame) * (1.0f / 30.0f);
        AnimatedCars_Bind();
        mIsBoundToGame = true;
    }
    return true;
}

bool CAnimScene::UnBindToGame() {
    if (mIsBoundToGame) {
        if (IsControllingCamera()) {
            SetCameraControl(false);
        }
        AnimatedCars_UnBind();
        mIsBoundToGame = false;
    }
    return true;
}

void CAnimScene::ChangePlayStatus(ePlayStatus new_status) {
    switch (mPlayStatus) {
        case Stopped:
            if (new_status < Stopped) {
                return;
            }
            if (new_status <= Paused) {
                return;
            }
            if (new_status != Playing) {
                return;
            }
            ResetTime();
            BindToGame();
            mPlayStatus = new_status;
            return;
        case Paused:
            switch (new_status) {
                case Playing:
                    break;
                case Paused:
                    return;
                case Stopped:
                    mPlayStatus = new_status;
                    UnBindToGame();
                    ResetTime();
                    return;
                default:
                    return;
            }
            break;
        case Playing:
            if (new_status == Paused) {
                break;
            }
            if (new_status > Paused) {
                return;
            }
            if (new_status != Stopped) {
                return;
            }
            mPlayStatus = new_status;
            UnBindToGame();
            ResetTime();
            return;
        default:
            return;
    }
    mPlayStatus = new_status;
}

// STRIPPED
bool CAnimScene::Cue() {}

bool CAnimScene::Play() {
    ChangePlayStatus(Playing);
    return true;
}

bool CAnimScene::Stop() {
    ChangePlayStatus(Stopped);
    return true;
}

bool CAnimScene::Pause() {
    ChangePlayStatus(Paused);
    return true;
}

bool CAnimScene::UnPause() {
    ChangePlayStatus(Playing);
    return true;
}

// STRIPPED
bool CAnimScene::IsCued() {}

bool CAnimScene::IsPlaying() {
    return (uint32)mPlayStatus > (uint32)Paused;
}

// STRIPPED
bool CAnimScene::IsStopped() {}

bool CAnimScene::IsPaused() {
    return mPlayStatus == Paused;
}

void CAnimScene::ResetTime() {
    SetTime(mTimeStart);
}

void CAnimScene::JumpToEnd() {
    SetTime(mTimeTotalLength);
}

void CAnimScene::SetTime(float time) {
    for (bPNode *node = mInstancedAnimEntityList.GetTail(); node != mInstancedAnimEntityList.EndOfList(); node = node->GetPrev()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->SetTime(time);
    }
    AnimatedCars_SetTime(time);
    mTimeElapsed = time;
    mTimeDelta = 0.0f;
}

// STRIPPED
void CAnimScene::GetTime(float &time) {}

void CAnimScene::UpdateTime(float time_step) {
    if (!IsPlaying()) {
        if (IsPaused()) {
            AnimatedCars_Update(0.0f);
        }
        return;
    }

    if (bEnableNisTextDisplay) {
        char scene_name[16];
        GetSceneName(scene_name);

#ifdef EA_BUILD_A124
#if 0 // TODO: ScreenPrintf comes from zFe2
        ScreenPrintf(-300, -0xb9, "NIS Time %3.1f sec - Frame %3.1f");
        ScreenPrintf(-300, -0xa5, "NIS Scene: %s");
        ScreenPrintf(-300, -0x91, "NIS CameraTrack: %d", GetCameraTrackNumber());
        ScreenPrintf(-300, -0x7d, "NIS Description: %s", GetAnimDescription());
#endif
#endif
    }

    for (bPNode *node = mInstancedAnimEntityList.GetTail(); node != mInstancedAnimEntityList.EndOfList(); node = node->GetPrev()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->UpdateTimeStep(mTimeDelta);
    }

    if (mAnimSceneData->GetSceneInfo()->SceneType == 0) {
        if (IsFinished()) {
            if (IsControllingCamera()) {
                SetCameraControl(false);
            }
        }
    } else if (mAnimSceneData->GetSceneInfo()->SceneType == 2 || mAnimSceneData->GetSceneInfo()->SceneType == 4) {
        if (IsFinished()) {
            IsControllingCamera();
        }
    }

    mTimeElapsed += mTimeDelta;
    mTimeDelta = time_step;
}

void CAnimScene::RenderEffects(eView *view, int is_reflection) {
    for (bPNode *node = mInstancedAnimEntityList.GetTail(); node != mInstancedAnimEntityList.EndOfList(); node = node->GetPrev()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->RenderEffects(view, is_reflection);
    }
}

void CAnimScene::AddProperty(eAnimProperty property_id, bool enabled) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (!anim_property) {
        CAnimProperty *anim_property = BNEW CAnimProperty(property_id, enabled);
        mAnimPropertyList.AddTail(anim_property);
    }
}

// STRIPPED
void CAnimScene::RemoveProperties() {}

CAnimProperty *CAnimScene::FindProperty(eAnimProperty property_id) {
    CAnimProperty *anim_property = static_cast<CAnimProperty *>(mAnimPropertyList.GetHead());

    for (; anim_property != mAnimPropertyList.EndOfList(); anim_property = anim_property->GetNext()) {
        eAnimProperty cur_property = anim_property->GetType();
        if (cur_property == property_id) {
            return anim_property;
        }
    }
    return nullptr;
}

bool CAnimScene::Init() {
    bMatrix4 scene_rotation_matrix;
    bMatrix4 scene_translation_matrix;
    bMatrix4 scene_transform_matrix;
    NisScene *scene_info = mAnimSceneData->GetSceneInfo();
    if (scene_info->SceneType == 0) {
        CAnimEntityCreationContext::SetRaceStartContext(true);
    } else {
        CAnimEntityCreationContext::SetRaceStartContext(false);
    }
    bool find_start_line = false;
    if (scene_info->SceneType == 0 || scene_info->SceneType == 3 || scene_info->SceneType == 2) {
        find_start_line = true;
    }

    CAnimLocator::GetInitialAnimMatricies(&scene_rotation_matrix, &scene_translation_matrix, find_start_line);
    SetSceneRotationMatrix(scene_rotation_matrix);
    SetSceneTranslationMatrix(scene_translation_matrix);
    bMulMatrix(&scene_transform_matrix, &scene_translation_matrix, &scene_rotation_matrix);
    SetSceneTransformMatrix(scene_transform_matrix);

    mSpaceNode = CreateSpaceNode(nullptr);
    mSpaceNode->SetLocalMatrix(&scene_transform_matrix);

    ClearCarAnimationControllers();
    SetCarAnimationPositions();
    CreateCarAnimationControllers();
    CreateAnimEntities();

    if (scene_info->SceneType == 0 || scene_info->SceneType == 3) {
        AddProperty(eAnimProp_ControlRaceCountdown, true);
        AddProperty(eAnimProp_UnBindRaceCars, true);
    }

    BindToGame();
    AnimatedCars_ResetToBeginning();
    InitCharacterEffects();

    char *overlay_name = mAnimSceneData->GetSceneInfo()->SeeulatorOverlayName;
    if (overlay_name[0] != '\0') {
        TheVisibleSectionManager.ActivateOverlay(overlay_name);
    }
    return true;
}

bool CAnimScene::Purge() {
    ClearAnimEntities();
    ClearCarAnimationControllers();
    DeleteSpaceNode(mSpaceNode);
    CloseCharacterEffects();
    char *overlay_name = mAnimSceneData->GetSceneInfo()->SeeulatorOverlayName;
    if (overlay_name[0] != '\0') {
        TheVisibleSectionManager.UnactivateOverlay();
    }
    return true;
}

// STRIPPED
void CAnimScene::ForceCarToAnimCarPosition(Car *car, int car_num) {}

void CAnimScene::ForcePlayerToAnimCarPosition(int player_num, int car_num) {}

// STRIPPED
// int GetPositionInRace(RacingCar *pRacingCar) {}

void CAnimScene::ClearCarAnimStates() {
    for (int i = 0; i <= 15; i++) {
        gCarAnimationStates[i].CarIndex = -1;
    }
}

void CAnimScene::InitCarAnimStatesFromStartingPositions() {
    if (mAnimCandidateType != 4) {
        for (int i = 0; i < 8; i++) {
            char channelName[32];
            bSPrintf(channelName, "car%d", i + 1);
            IVehicle *NISCar = INIS::Get()->GetCar(UCrc32(channelName));
            if (NISCar) {
                gCarAnimationStates[i].CarIndex = i;
                gCarAnimationStates[i].mIVehicle = NISCar;
                NISCar->Deactivate();
            }
        }
    }
}

void CAnimScene::InitCarAnimStatesFromNIS() {
    for (int i = 0; i < 8; i++) {
        char channelName[32];
        bSPrintf(channelName, "car%d", i + 1);
        IVehicle *NISCar = INIS::Get()->GetCar(UCrc32(channelName));
        if (NISCar) {
            gCarAnimationStates[i].CarIndex = i;
            gCarAnimationStates[i].mIVehicle = NISCar;
            NISCar->Deactivate();
        }
    }

    IVehicle *copCar = INIS::Get()->GetCar(UCrc32("cop1"));
    if (copCar) {
        gCarAnimationStates[8].CarIndex = 8;
        gCarAnimationStates[8].mIVehicle = copCar;
    }

    for (int i = 1; i < 8; i++) {
        char channelName[24];
        bSPrintf(channelName, "cop%d", i + 1);
        IVehicle *copVehicle = INIS::Get()->GetCar(UCrc32(channelName));
        if (copVehicle) {
            gCarAnimationStates[i + 8].CarIndex = i + 8;
            gCarAnimationStates[i + 8].mIVehicle = copVehicle;
        }
    }
}

// STRIPPED
int CAnimScene::FindCurrentWorldCarIndex(Car *car) {}

void CAnimScene::SetCarAnimationPositions() {
    ClearCarAnimStates();

    if (mAnimSceneData->GetSceneInfo()->SceneType == 3) {
        InitCarAnimStatesFromStartingPositions();
        ForcePlayerToAnimCarPosition(0, 1);
    } else if (mAnimSceneData->GetSceneInfo()->SceneType == 0) {
        InitCarAnimStatesFromStartingPositions();
    } else if (mAnimSceneData->GetSceneInfo()->SceneType != 1 &&
               (mAnimSceneData->GetSceneInfo()->SceneType == 2 || mAnimSceneData->GetSceneInfo()->SceneType == 4)) {
        InitCarAnimStatesFromNIS();
    }
}

void CAnimScene::CreateCarAnimationControllers() {
    if (mAnimSceneData->GetSceneInfo()->HaveCarAnimation == 0) {
        return;
    }

    CAnimSkeleton *skel = GetSkeletonFromList(skel_ROOT_hash);
    if (!skel) {
        return;
    }

    for (int i = 0; i < 16; i++) {
        if (gCarAnimationStates[i].CarIndex != -1) {
            char nameToHash[34];
            char *baseCarName = Car_Name[i];
            bSPrintf(nameToHash, "%s%s", mAnimSceneData->GetSceneInfo()->mSceneName, baseCarName);
            unsigned int name_hash = bStringHash(nameToHash);
            bSPrintf(nameToHash, "%s%s_t", mAnimSceneData->GetSceneInfo()->mSceneName, baseCarName);
            unsigned int name_hash_t = bStringHash(nameToHash);
            bSPrintf(nameToHash, "%s%s_q", mAnimSceneData->GetSceneInfo()->mSceneName, baseCarName);
            unsigned int name_hash_q = bStringHash(nameToHash);

            if (name_hash != 0 && name_hash_t != 0 && name_hash_q != 0) {
                CAnimCtrl *new_anim_ctrl = new ("Car CAnimCtrl") CAnimCtrl();
                new_anim_ctrl->SetNameHash(name_hash);
                new_anim_ctrl->SetTimeScale(0.5f);
                new_anim_ctrl->SetFlags(8);
                new_anim_ctrl->GetAnimPart()->Init(skel);
                new_anim_ctrl->CreateFnAnimFromNamehash(name_hash_t, 0);
                new_anim_ctrl->CreateFnAnimFromNamehash(name_hash_q, 1);
                if (new_anim_ctrl->GetAllocated() != 0) {
                    gCarAnimationStates[i].AnimCtrl = new_anim_ctrl;
                    gCarAnimationStates[i].mIVehicle->Activate();
                } else {
                    if (AnimCfg_DebugOutput != 0) {
                        DumpAnimBanks();
                    }
                    new_anim_ctrl->Purge();
                    new_anim_ctrl->Cleanup();
                    delete new_anim_ctrl;
                    gCarAnimationStates[i].AnimCtrl = nullptr;
                }
            }
        }
    }

    AddProperty(eAnimProp_ControlRaceCars, true);
}

void CAnimScene::ClearCarAnimationControllers() {
    for (int i = 0; i < 16; i++) {
        if (gCarAnimationStates[i].AnimCtrl) {
            gCarAnimationStates[i].AnimCtrl->GetAnimPart()->Purge();
            gCarAnimationStates[i].AnimCtrl->Cleanup();
            if (gCarAnimationStates[i].AnimCtrl) {
                delete gCarAnimationStates[i].AnimCtrl;
            }
            gCarAnimationStates[i].AnimCtrl = nullptr;
        }
        gCarAnimationStates[i].HaveLastCarPosition = 0;
        gCarAnimationStates[i].CarIndex = -1;
    }
}

void CAnimScene::AnimatedCars_SetMainAndWheels(int current_car, CAnimCtrl *main_anim_ctrl, float time_step) {
    bMatrix4 *animated_car_global_matrices = reinterpret_cast<bMatrix4 *>(main_anim_ctrl->GetAnimPart()->GetGlobalMatrices());
    bMatrix4 animated_car_matrix(animated_car_global_matrices[1]);
    bMatrix4 AI_Space_Matrix;
    bMulMatrix(&animated_car_matrix, &mSceneRotationMatrix, &animated_car_matrix);
    bMulMatrix(&animated_car_matrix, &mSceneTranslationMatrix, &animated_car_matrix);
    eUnSwizzleWorldMatrix(animated_car_matrix, animated_car_matrix);

    float ground_elevation = 0.0f;
    bool point_valid =
        WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(*reinterpret_cast<UMath::Vector3 *>(&animated_car_matrix.v3), ground_elevation, nullptr);

    if (point_valid) {
        animated_car_matrix.v3.y = ground_elevation + 5.0f;
    }

    bool initial = gCarAnimationStates[current_car].HaveLastCarPosition == 0;
    INISCarControl *iniscar;
    if (gCarAnimationStates[current_car].mIVehicle->QueryInterface(&iniscar)) {
        if (!iniscar->SetNISPosition(reinterpret_cast<UMath::Matrix4 &>(animated_car_matrix), initial, time_step)) {
            gCarAnimationStates[current_car].HaveLastCarPosition = -1;
        }
    }
    gCarAnimationStates[current_car].HaveLastCarPosition++;
}

void CAnimScene::AnimatedCars_ResetToBeginning() {
    AnimatedCars_ClearLastPose();
    for (int current_car = 0; current_car < 16; current_car++) {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        if (main_anim_ctrl) {
            main_anim_ctrl->SetEvalTime(0.0f);
        }
    }
    AnimatedCars_Update(0.0f);
}

void CAnimScene::AnimatedCars_ClearLastPose() {
    for (int current_car = 0; current_car < 16; current_car++) {
        gCarAnimationStates[current_car].HaveLastCarPosition = 0;
    }
}

void CAnimScene::AnimatedCars_SetTime(float time) {
    for (int current_car = 0; current_car < 16; current_car++) {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        gCarAnimationStates[current_car].HaveLastCarPosition = 0;
        if (main_anim_ctrl) {
            main_anim_ctrl->SetEvalTime(0.0f);
            main_anim_ctrl->AdvanceAnimTime(0.0f);
            main_anim_ctrl->UpdateAnimPose(true);
        }
    }
    AnimatedCars_Update(time);
}

void CAnimScene::AnimatedCars_Update(float time_step) {
    if (!IsPropertyEnabled(eAnimProp_ControlRaceCars)) {
        return;
    }

    if (IsPropertyEnabled(eAnimProp_UnBindRaceCars) && mTimeElapsed > mTimeTotalLength) {
        AnimatedCars_UnBind();
        return;
    }

    for (int current_car = 0; current_car < 16; current_car++) {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        if (main_anim_ctrl && gCarAnimationStates[current_car].CarIndex >= 0) {
            main_anim_ctrl->AdvanceAnimTime(time_step);
            main_anim_ctrl->UpdateAnimPose(true);
            AnimatedCars_SetMainAndWheels(current_car, main_anim_ctrl, time_step);
        }
    }
}

void CAnimScene::AnimatedCars_Bind() {
    if (!IsPropertyEnabled(eAnimProp_ControlRaceCars)) {
        return;
    }

    for (int i = 0; i < 16; i++) {
        int car_index = gCarAnimationStates[i].CarIndex;
        if (car_index >= 0) {
            IRigidBody *irb;
            IInput *pInput;
            gCarAnimationStates[i].mIVehicle->SetSpeed(0.0f);

            if (gCarAnimationStates[i].mIVehicle->QueryInterface(&irb)) {
                UMath::Vector3 zeroVec = {};
                irb->SetLinearVelocity(zeroVec);
                irb->SetAngularVelocity(zeroVec);
            }

            if (gCarAnimationStates[i].mIVehicle->QueryInterface(&pInput)) {
                pInput->SetControlHandBrake(1.0f);
                pInput->SetControlGas(0.0f);
                pInput->SetControlBrake(1.0f);
            }
        }
    }
}

void CAnimScene::AnimatedCars_UnBind() {
    if (IsPropertyEnabled(eAnimProp_ControlRaceCars)) {
        SetPropertyEnabled(eAnimProp_ControlRaceCars, false);
    }

    for (int i = 0; i < 16; i++) {
        int car_index = gCarAnimationStates[i].CarIndex;
        if (car_index >= 0) {
            gCarAnimationStates[i].mIVehicle->SetAnimating(false);
            if (i > 7) {
                gCarAnimationStates[i].mIVehicle->Deactivate();
            }
        }
    }
}

IAnimEntity *CAnimScene::GetAnimEntityWithModelName(const char *name) {
    uint32 hashID = bStringHash(name);
    for (bPNode *node = mInstancedAnimEntityList.GetTail(); node != mInstancedAnimEntityList.EndOfList(); node = node->GetPrev()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        if (iae->GetWorldModel()) {
            if (iae->GetWorldModel()->GetModel()->GetNameHash() == hashID) {
                return iae;
            }
        }
    }
    return nullptr;
}

void CAnimScene::CreateAnimEntities() {
    bTList<CAnimEntityData> *aed_list = mAnimSceneData->GetAnimEntityDataList();
    CAnimEntityData *anim_entity_data = aed_list->GetHead();

    for (; anim_entity_data != aed_list->EndOfList(); anim_entity_data = anim_entity_data->GetNext()) {
        int type = anim_entity_data->GetType();
        void *data = anim_entity_data->GetData();

        IAnimEntity *iae = CAnimEntityFactory::CreateAnimEntity(type);
        if (iae->Init(data, mSpaceNode)) {
            mInstancedAnimEntityList.AddTail(iae);
        } else {
            iae->Purge();
            delete iae;
        }
    }
}

void CAnimScene::ClearAnimEntities() {
    for (bPNode *node = mInstancedAnimEntityList.GetTail(); node != mInstancedAnimEntityList.EndOfList(); node = node->GetPrev()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->Purge();
        delete iae;
    }
    while (!mInstancedAnimEntityList.IsEmpty()) {
        mInstancedAnimEntityList.RemoveTail();
    }
}

void RenderAnimSceneEffects(eView *view, int exc_flag) {
    if (INIS::Get()) {
        CAnimScene *scene = INIS::Get()->GetAnimScene();
        if (scene) {
            scene->RenderEffects(view, exc_flag & 0x800);
        }
    }
}
