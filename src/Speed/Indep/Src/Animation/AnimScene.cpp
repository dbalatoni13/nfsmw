#include "AnimScene.hpp"
#include "AnimDirectory.hpp"
#include "AnimEntity_BasicCharacter.hpp"
#include "AnimEntityCreationContext.hpp"
#include "AnimLocator.hpp"
#include "AnimPlayer.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern AnimDirectory *TheAnimDirectory;
// mIsRaceStart is CAnimEntityCreationContext::mIsRaceStart
unsigned int skel_ROOT_hash;
extern int AnimCfg_DebugOutput;
extern float NISCopCarDoorOpenAmount[4];

void bConvertToBond(bMatrix4 &dest, const bMatrix4 &v);
CAnimSkeleton *GetSkeletonFromList(unsigned int namehash);
void DumpAnimBanks();

bTList<CAnimSceneData> g_loadedAnimSceneDataList;
CarAnimationState gCarAnimationStates[16];

float gCopCarDoorAnim_CurrentTime[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float gCopCarDoorAnim_AnimLength[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float gCopCarDoorAnim_StartPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float gCopCarDoorAnim_Delta[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

SlotPool *AnimPartSlotPool = nullptr;

int bEnableNisTextDisplay = 0;

bool CAnimEntityCreationContext::mIsRaceStart = false;
int CAnimScene::mHandleCounter = 0;

char Car_Name[16][16] = {
    "ZPM_Car1",
    "ZPM_Car2",
    "ZPM_Car3",
    "ZPM_Car4",
    "ZPM_Car5",
    "ZPM_Car6",
    "ZPM_Car7",
    "ZPM_Car8",
    "ZPM_Cop1",
    "ZPM_Cop2",
    "ZPM_Cop3",
    "ZPM_Cop4",
    "ZPM_Cop5",
    "ZPM_Cop6",
    "ZPM_Cop7",
    "ZPM_Cop8",
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

void StartCopDoorAnim(int door, float startPos, float AnimLength, float endPos) {
    if (static_cast<unsigned int>(door) > 3) {
        return;
    }
    gCopCarDoorAnim_CurrentTime[door] = 0.0f;
    if (AnimLength > 0.0f) {
        gCopCarDoorAnim_AnimLength[door] = AnimLength;
        gCopCarDoorAnim_StartPos[door] = startPos;
        gCopCarDoorAnim_Delta[door] = endPos - startPos;
    } else {
        gCopCarDoorAnim_AnimLength[door] = 0.0f;
    }
    NISCopCarDoorOpenAmount[door] = startPos;
}

void UpdateCopDoorPositions(float time) {
    int door = 0;
    do {
        float animLength = gCopCarDoorAnim_AnimLength[door];
        if (animLength != 0.0f) {
            float currentTime = gCopCarDoorAnim_CurrentTime[door] + time;
            gCopCarDoorAnim_CurrentTime[door] = currentTime;
            if (currentTime <= 0.0f) {
                NISCopCarDoorOpenAmount[door] = gCopCarDoorAnim_StartPos[door];
            } else if (currentTime < animLength) {
                float ratio = currentTime / animLength;
                NISCopCarDoorOpenAmount[door] = ratio * gCopCarDoorAnim_Delta[door] + gCopCarDoorAnim_StartPos[door];
            } else {
                gCopCarDoorAnim_AnimLength[door] = 0.0f;
                NISCopCarDoorOpenAmount[door] = gCopCarDoorAnim_StartPos[door] + gCopCarDoorAnim_Delta[door];
            }
        }
        door = door + 1;
    } while (door < 4);
}

CAnimSceneData::CAnimSceneData(bChunk *chunk)
    : mChunk(chunk), //
      mNisScene(nullptr) {}

CAnimSceneData::~CAnimSceneData() {}

CAnimSceneData *CAnimSceneData::FindAnimSceneData(unsigned int scene_name_hash) {
    CAnimSceneData *scene_data = static_cast<CAnimSceneData *>(g_loadedAnimSceneDataList.GetHead());
    while (scene_data != g_loadedAnimSceneDataList.EndOfList()) {
        if (scene_name_hash == scene_data->mNisScene->mSceneNameHash) {
            return scene_data;
        }
        scene_data = scene_data->GetNext();
    }
    return nullptr;
}

void CAnimSceneData::EndianSwapHeaderData() {
    bEndianSwap32(mNisScene);
    bEndianSwap32(&mNisScene->SceneType);
    bEndianSwap32(&mNisScene->HaveLayout);
    bEndianSwap32(&mNisScene->HaveCarAnimation);
    bEndianSwap32(&mNisScene->NumberOfCars);
    bEndianSwap32(&mNisScene->StartFrame);
    bEndianSwap32(&mNisScene->VanishFrame);
}

void CAnimSceneData::InitHeaderData(void *data, int size) {
    mNisScene = reinterpret_cast<NisScene *>(data);
    mNisScene->Description = reinterpret_cast<char *>(data) + 0x40;
    EndianSwapHeaderData();
}

void CAnimSceneData::AddEntityData(void *data, int size) {
    CAnimEntityFactory::EndianSwapEntityData(data, size);

    CAnimEntityData *aed = new CAnimEntityData(*reinterpret_cast<unsigned int *>(data), data, size);
    mAnimEntityDataList.AddTail(aed);
}

CAnimSceneData *CreateAnimSceneData(bChunk *nested_chunk, bChunk *sub_chunk) {
    CAnimSceneData *scene_data = new CAnimSceneData(nested_chunk);

    if (scene_data != nullptr) {
        scene_data->InitHeaderData(sub_chunk + 1, sub_chunk->Size);
        g_loadedAnimSceneDataList.AddTail(scene_data);
        return scene_data;
    }
    return nullptr;
}

int LoaderAnimSceneData(bChunk *chunk) {
    if (chunk->ID == 0x80037020) {
        bChunk *sub_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();
        CAnimSceneData *scene_data = nullptr;

        while (sub_chunk != last_chunk) {
            unsigned int chunk_id = sub_chunk->ID;

            switch (chunk_id) {
                case 0x37030:
                    scene_data = CreateAnimSceneData(chunk, sub_chunk);
                    break;
                case 0x37040:
                    if (scene_data != nullptr) {
                        char *data = sub_chunk->GetAlignedData(16);
                        scene_data->AddEntityData(data, sub_chunk->Size - (data - sub_chunk->GetData()));
                    }
                    break;
            }

            sub_chunk = sub_chunk->GetNext();
        }
        return 1;
    }

    return 0;
}

int UnloaderAnimSceneData(bChunk *chunk) {
    if (chunk->ID == 0x80037020) {
        CAnimSceneData *scene_data = static_cast<CAnimSceneData *>(g_loadedAnimSceneDataList.GetHead());
        while (scene_data != g_loadedAnimSceneDataList.EndOfList()) {
            CAnimSceneData *next = scene_data->GetNext();
            if (scene_data->mChunk == chunk) {
                scene_data->Remove();
                delete scene_data;
                break;
            }
            scene_data = next;
        }
        return 1;
    }

    return 0;
}

CAnimProperty::CAnimProperty(eAnimProperty type, bool enabled)
    : mType(type),          //
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

int CAnimScene::GetHandle() {
    return mHandle;
}

CAnimScene::CAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index)
    : mHandle(0),                            //
      mAnimSceneData(anim_scene_data),       //
      mPlayStatus(Stopped),                  //
      mTimeElapsed(0.0f),                    //
      mTimeDelta(0.0f),                      //
      mTimeStart(0.0f),                      //
      mTimeTotalLength(0.0f),                //
      mIsBoundToGame(false),                 //
      mCameraTrackNumber(camera_track_number), //
      mControllingCamera(false),             //
      mSpaceNode(nullptr),                   //
      mAnimCandidateType(anim_candidate_type), //
      mAnimCandidateIndex(anim_candidate_index) {
    mHandle = GenerateHandle();
    bIdentity(&mSceneRotationMatrix);
    bIdentity(&mSceneTranslationMatrix);
    bIdentity(&mSceneTransformMatrix);
}

CAnimScene::~CAnimScene() {}

int CAnimScene::GenerateHandle() {
    mHandleCounter++;
    if (mHandleCounter > 0xFFFF) {
        mHandleCounter = 1;
    }
    return mHandleCounter;
}

unsigned int CAnimScene::GetAnimID() {
    return mAnimSceneData->mNisScene->mSceneNameHash;
}

unsigned int CAnimScene::GetSceneHash() {
    return GetAnimID();
}

int CAnimScene::GetSceneType() {
    return mAnimSceneData->mNisScene->SceneType;
}

void CAnimScene::GetSceneName(char *ret_name) {
    unsigned int scene_hash = GetSceneHash();
    TheAnimDirectory->GetNameOfSceneHash(scene_hash, ret_name);
}

int CAnimScene::GetCameraTrackNumber() {
    return mCameraTrackNumber;
}

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

bool CAnimScene::IsPlaying() {
    return (unsigned int)mPlayStatus > (unsigned int)Paused;
}

bool CAnimScene::IsPaused() {
    return mPlayStatus == Paused;
}

bool CAnimScene::SetPropertyEnabled(eAnimProperty property_id, bool enable) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property != nullptr) {
        anim_property->SetEnabled(enable);
        return true;
    }

    AddProperty(property_id, enable);
    return false;
}

bool CAnimScene::IsPropertyEnabled(eAnimProperty property_id) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property == nullptr) {
        return false;
    }
    return anim_property->IsEnabled();
}

bool CAnimScene::BindToGame() {
    if (!mIsBoundToGame) {
        mControllingCamera = true;
        NisScene *scene_info = mAnimSceneData->mNisScene;
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
            mControllingCamera = false;
        }
        AnimatedCars_UnBind();
        mIsBoundToGame = false;
    }
    return true;
}

void CAnimScene::ResetTime() {
    SetTime(mTimeStart);
}

void CAnimScene::JumpToEnd() {
    SetTime(mTimeTotalLength);
}

void CAnimScene::SetTime(float time) {
    bPNode *node = mInstancedAnimEntityList.GetTail();
    while (node != mInstancedAnimEntityList.EndOfList()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->SetTime(time);
        node = node->GetPrev();
    }
    AnimatedCars_SetTime(time);
    mTimeElapsed = time;
    mTimeDelta = 0.0f;
}

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
    }

    bPNode *node = mInstancedAnimEntityList.GetTail();
    while (node != mInstancedAnimEntityList.EndOfList()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->UpdateTimeStep(mTimeDelta);
        node = node->GetPrev();
    }

    int scene_type = mAnimSceneData->mNisScene->SceneType;
    if (scene_type == 0) {
        if (GetTimeElapsed() > GetTimeTotalLength()) {
            if (IsControllingCamera()) {
                mControllingCamera = false;
            }
        }
    } else if (scene_type == 2 || scene_type == 4) {
        if (GetTimeElapsed() > GetTimeTotalLength()) {
            IsControllingCamera();
        }
    }

    mTimeElapsed += mTimeDelta;
    mTimeDelta = time_step;
}

void CAnimScene::RenderEffects(eView *view, int is_reflection) {
    bPNode *node = mInstancedAnimEntityList.GetTail();
    while (node != mInstancedAnimEntityList.EndOfList()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->RenderEffects(view, is_reflection);
        node = node->GetPrev();
    }
}

void CAnimScene::AddProperty(eAnimProperty property_id, bool enabled) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property == nullptr) {
        anim_property = new CAnimProperty(property_id, enabled);
        bNode *tail = mAnimPropertyList.HeadNode.Prev;
        tail->Next = anim_property;
        mAnimPropertyList.HeadNode.Prev = anim_property;
        anim_property->Prev = tail;
        anim_property->Next = &mAnimPropertyList.HeadNode;
    }
}

CAnimProperty *CAnimScene::FindProperty(eAnimProperty property_id) {
    CAnimProperty *anim_property = static_cast<CAnimProperty *>(mAnimPropertyList.GetHead());

    while (anim_property != mAnimPropertyList.EndOfList()) {
        if (anim_property->GetType() == property_id) {
            return anim_property;
        }
        anim_property = anim_property->GetNext();
    }
    return nullptr;
}

void CAnimScene::ChangePlayStatus(ePlayStatus new_status) {
    ePlayStatus current_status = mPlayStatus;

    switch (current_status) {
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
        case Paused:
            return;
        case Stopped:
            goto do_stop;
        case Playing:
            break;
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
    do_stop:
        mPlayStatus = new_status;
        UnBindToGame();
        ResetTime();
        return;
    default:
        return;
    }
    mPlayStatus = new_status;
}

bool CAnimScene::Init() {
    bMatrix4 scene_rotation_matrix;
    bMatrix4 scene_translation_matrix;
    bMatrix4 scene_transform_matrix;
    NisScene *scene_info = mAnimSceneData->mNisScene;
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

    SpaceNode *space_node = CreateSpaceNode(nullptr);
    mSpaceNode = space_node;
    space_node->SetLocalMatrix(&scene_transform_matrix);

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

    char *overlay_name = mAnimSceneData->mNisScene->SeeulatorOverlayName;
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
    if (mAnimSceneData->mNisScene->SeeulatorOverlayName[0] != '\0') {
        TheVisibleSectionManager.UnactivateOverlay();
    }
    return true;
}

void CAnimScene::ForcePlayerToAnimCarPosition(int player_num, int car_num) {}

void CAnimScene::ClearCarAnimStates() {
    for (int i = 0; i <= 15; i++) {
        gCarAnimationStates[i].CarIndex = -1;
    }
}

void CAnimScene::InitCarAnimStatesFromStartingPositions() {
    if (mAnimCandidateType != 4) {
        int i = 0;
        do {
            char channelName[32];
            bSPrintf(channelName, "car%d", i + 1);
            IVehicle *NISCar = INIS::Get()->GetCar(UCrc32(channelName));
            if (NISCar != nullptr) {
                gCarAnimationStates[i].CarIndex = i;
                gCarAnimationStates[i].mIVehicle = NISCar;
                NISCar->Deactivate();
            }
            i = i + 1;
        } while (i < 8);
    }
}

void CAnimScene::InitCarAnimStatesFromNIS() {
    {
        int i = 0;
        do {
            char channelName[32];
            bSPrintf(channelName, "car%d", i + 1);
            IVehicle *NISCar = INIS::Get()->GetCar(UCrc32(channelName));
            if (NISCar != nullptr) {
                gCarAnimationStates[i].CarIndex = i;
                gCarAnimationStates[i].mIVehicle = NISCar;
                NISCar->Deactivate();
            }
            i = i + 1;
        } while (i < 8);
    }

    IVehicle *copCar = INIS::Get()->GetCar(UCrc32("cop1"));
    if (copCar != nullptr) {
        gCarAnimationStates[8].CarIndex = 8;
        gCarAnimationStates[8].mIVehicle = copCar;
    }

    {
        for (int i = 1; i < 8; i++) {
            char channelName[24];
            bSPrintf(channelName, "cop%d", i + 1);
            IVehicle *copVehicle = INIS::Get()->GetCar(UCrc32(channelName));
            int carIndex = i + 8;
            if (copVehicle != nullptr) {
                gCarAnimationStates[carIndex].CarIndex = carIndex;
                gCarAnimationStates[carIndex].mIVehicle = copVehicle;
            }
        }
    }
}

void CAnimScene::SetCarAnimationPositions() {
    ClearCarAnimStates();
    int sceneType = mAnimSceneData->mNisScene->SceneType;
    if (sceneType == 3) {
        InitCarAnimStatesFromStartingPositions();
        ForcePlayerToAnimCarPosition(0, 1);
    } else if (sceneType == 0) {
        InitCarAnimStatesFromStartingPositions();
    } else if (sceneType != 1 && (sceneType == 2 || sceneType == 4)) {
        InitCarAnimStatesFromNIS();
    }
}

void CAnimScene::CreateCarAnimationControllers() {
    if (mAnimSceneData->mNisScene->HaveCarAnimation == 0) {
        return;
    }

    CAnimSkeleton *skel = GetSkeletonFromList(skel_ROOT_hash);
    if (skel == nullptr) {
        return;
    }

    {
        int i = 0;
        do {
            if (gCarAnimationStates[i].CarIndex != -1) {
                char nameToHash[34];
                char *baseCarName = Car_Name[i];
                NisScene *scene = mAnimSceneData->mNisScene;
                bSPrintf(nameToHash, "%s%s", scene->mSceneName, baseCarName);
                unsigned int name_hash = bStringHash(nameToHash);
                scene = mAnimSceneData->mNisScene;
                bSPrintf(nameToHash, "%s%s_t", scene->mSceneName, baseCarName);
                unsigned int name_hash_t = bStringHash(nameToHash);
                scene = mAnimSceneData->mNisScene;
                bSPrintf(nameToHash, "%s%s_q", scene->mSceneName, baseCarName);
                unsigned int name_hash_q = bStringHash(nameToHash);

                if (name_hash != 0 && name_hash_t != 0 && name_hash_q != 0) {
                    CAnimCtrl *new_anim_ctrl = new ("Car CAnimCtrl") CAnimCtrl();
                    new_anim_ctrl->SetNameHash(name_hash);
                    new_anim_ctrl->SetTimeScale(0.5f);
                    new_anim_ctrl->SetFlags(new_anim_ctrl->GetFlags() | 8);
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
            i = i + 1;
        } while (i < 16);
    }

    AddProperty(eAnimProp_ControlRaceCars, true);
}

void CAnimScene::ClearCarAnimationControllers() {
    int i = 0;
    do {
        if (gCarAnimationStates[i].AnimCtrl != nullptr) {
            gCarAnimationStates[i].AnimCtrl->GetAnimPart()->Purge();
            gCarAnimationStates[i].AnimCtrl->Cleanup();
            if (gCarAnimationStates[i].AnimCtrl != nullptr) {
                delete gCarAnimationStates[i].AnimCtrl;
            }
            gCarAnimationStates[i].AnimCtrl = nullptr;
        }
        gCarAnimationStates[i].HaveLastCarPosition = 0;
        gCarAnimationStates[i].CarIndex = -1;
        i = i + 1;
    } while (i < 16);
}

void CAnimScene::AnimatedCars_SetMainAndWheels(int current_car, CAnimCtrl *main_anim_ctrl, float time_step) {
    bMatrix4 animated_car_matrix(reinterpret_cast<bMatrix4 *>(main_anim_ctrl->GetAnimPart()->GetGlobalMatrices())[1]);
    bMatrix4 AI_Space_Matrix;
    bMulMatrix(&animated_car_matrix, &mSceneRotationMatrix, &animated_car_matrix);
    bMulMatrix(&animated_car_matrix, &mSceneTranslationMatrix, &animated_car_matrix);
    bConvertToBond(animated_car_matrix, animated_car_matrix);

    float ground_elevation = 0.0f;
    bool point_valid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(
        *reinterpret_cast<UMath::Vector3 *>(&animated_car_matrix.v3), ground_elevation, nullptr);

    if (point_valid) {
        animated_car_matrix.v3.y = ground_elevation + 5.0f;
    }

    int haveLastPos = gCarAnimationStates[current_car].HaveLastCarPosition;
    bool initial = haveLastPos == 0;
    INISCarControl *iniscar;
    if (gCarAnimationStates[current_car].mIVehicle->QueryInterface(&iniscar)) {
        if (!iniscar->SetNISPosition(
                reinterpret_cast<UMath::Matrix4 &>(animated_car_matrix),
                initial, time_step)) {
            gCarAnimationStates[current_car].HaveLastCarPosition = -1;
        }
    }
    gCarAnimationStates[current_car].HaveLastCarPosition++;
}

void CAnimScene::AnimatedCars_ResetToBeginning() {
    AnimatedCars_ClearLastPose();
    int current_car = 0;
    do {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        if (main_anim_ctrl != nullptr) {
            main_anim_ctrl->SetEvalTime(0.0f);
        }
        current_car = current_car + 1;
    } while (current_car < 16);
    AnimatedCars_Update(0.0f);
}

void CAnimScene::AnimatedCars_ClearLastPose() {
    for (int i = 0; i <= 15; i++) {
        gCarAnimationStates[i].HaveLastCarPosition = 0;
    }
}

void CAnimScene::AnimatedCars_SetTime(float time) {
    int current_car = 0;
    do {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        gCarAnimationStates[current_car].HaveLastCarPosition = 0;
        if (main_anim_ctrl != nullptr) {
            main_anim_ctrl->SetEvalTime(0.0f);
            main_anim_ctrl->AdvanceAnimTime(0.0f);
            main_anim_ctrl->UpdateAnimPose(true);
        }
        current_car = current_car + 1;
    } while (current_car < 16);
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

    int current_car = 0;
    do {
        CAnimCtrl *main_anim_ctrl = gCarAnimationStates[current_car].AnimCtrl;
        if (main_anim_ctrl != nullptr && gCarAnimationStates[current_car].CarIndex >= 0) {
            main_anim_ctrl->AdvanceAnimTime(time_step);
            main_anim_ctrl->UpdateAnimPose(true);
            AnimatedCars_SetMainAndWheels(current_car, main_anim_ctrl, time_step);
        }
        current_car = current_car + 1;
    } while (current_car < 16);
}

void CAnimScene::AnimatedCars_Bind() {
    if (!IsPropertyEnabled(eAnimProp_ControlRaceCars)) {
        return;
    }

    int i = 0;
    do {
        if (gCarAnimationStates[i].CarIndex >= 0) {
            gCarAnimationStates[i].mIVehicle->SetSpeed(0.0f);

            IRigidBody *irb;
            if (gCarAnimationStates[i].mIVehicle->QueryInterface(&irb)) {
                UMath::Vector3 zeroVec = {};
                irb->SetLinearVelocity(zeroVec);
                irb->SetAngularVelocity(zeroVec);
            }

            IInput *pInput;
            if (gCarAnimationStates[i].mIVehicle->QueryInterface(&pInput)) {
                pInput->SetControlHandBrake(1.0f);
                pInput->SetControlGas(0.0f);
                pInput->SetControlBrake(1.0f);
            }
        }
        i = i + 1;
    } while (i < 16);
}

void CAnimScene::AnimatedCars_UnBind() {
    if (IsPropertyEnabled(eAnimProp_ControlRaceCars)) {
        SetPropertyEnabled(eAnimProp_ControlRaceCars, false);
    }

    int i = 0;
    do {
        int car_index = gCarAnimationStates[i].CarIndex;
        if (car_index >= 0) {
            gCarAnimationStates[i].mIVehicle->SetAnimating(false);
            if (i > 7) {
                gCarAnimationStates[i].mIVehicle->Deactivate();
            }
        }
        i = i + 1;
    } while (i < 16);
}

IAnimEntity *CAnimScene::GetAnimEntityWithModelName(const char *name) {
    unsigned int hashID = bStringHash(name);
    bPNode *node = mInstancedAnimEntityList.GetTail();
    while (node != mInstancedAnimEntityList.EndOfList()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        if (iae->GetWorldModel() != nullptr) {
            eModel *model = iae->GetWorldModel()->GetModel();
            if (model->GetNameHash() == hashID) {
                return iae;
            }
        }
        node = node->GetPrev();
    }
    return nullptr;
}

void CAnimScene::CreateAnimEntities() {
    bTList<CAnimEntityData> *aed_list = mAnimSceneData->GetAnimEntityDataList();
    CAnimEntityData *anim_entity_data = aed_list->GetHead();

    while (anim_entity_data != aed_list->EndOfList()) {
        int type = anim_entity_data->mType;
        void *data = anim_entity_data->mData;

        IAnimEntity *iae = CAnimEntityFactory::CreateAnimEntity(type);
        if (iae->Init(data, mSpaceNode)) {
            mInstancedAnimEntityList.AddTail(iae);
        } else {
            iae->Purge();
            delete iae;
        }
        anim_entity_data = anim_entity_data->GetNext();
    }
}

void CAnimScene::ClearAnimEntities() {
    bPNode *node = mInstancedAnimEntityList.GetTail();
    while (node != mInstancedAnimEntityList.EndOfList()) {
        IAnimEntity *iae = reinterpret_cast<IAnimEntity *>(node->GetObject());
        iae->Purge();
        delete iae;
        node = node->GetPrev();
    }
    while (!mInstancedAnimEntityList.IsEmpty()) {
        mInstancedAnimEntityList.RemoveTail();
    }
}

void RenderAnimSceneEffects(eView *view, int exc_flag) {
    if (INIS::Get() != nullptr) {
        CAnimScene *scene = INIS::Get()->GetAnimScene();
        if (scene != nullptr) {
            scene->RenderEffects(view, exc_flag & 0x800);
        }
    }
}

float CAnimScene::GetTimeElapsed() {
    return mTimeElapsed;
}

float CAnimScene::GetTimeStart() {
    return mTimeStart;
}

float CAnimScene::GetTimeTotalLength() {
    return mTimeTotalLength;
}

bool CAnimScene::IsControllingCamera() {
    return mControllingCamera;
}

bool CAnimScene::IsCameraFixingElevation() {
    return true;
}

const bMatrix4 &CAnimScene::GetSceneRotationMatrix() {
    return mSceneRotationMatrix;
}

const bMatrix4 &CAnimScene::GetSceneTransformMatrix() {
    return mSceneTransformMatrix;
}
