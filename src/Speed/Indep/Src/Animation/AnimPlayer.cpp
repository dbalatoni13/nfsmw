#include "AnimPlayer.hpp"
#include "AnimDirectory.hpp"
#include "AnimScene.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

bool gPlayAnimStream = IsSoundEnabled ? true : false;
CAnimPlayer TheAnimPlayer;

void AnimLoader_NextStep();
extern int AnimCfg_DisableWorldAnimations;

class CAnimResourceFileProxy : public bTNode<CAnimResourceFileProxy> {
  public:
    enum MemoryPool {
        Main = 0,
        TrackStream = 1,
        CarPool = 2,
    };

    CAnimResourceFileProxy(ResourceFile *res_file_ptr, void *mem_ptr, MemoryPool mem_pool_used)
        : mResourceFile(res_file_ptr), mMemPointer(mem_ptr), mMemoryPool(mem_pool_used) {}
    ~CAnimResourceFileProxy() {}

    ResourceFile *mResourceFile;
    void *mMemPointer;
    MemoryPool mMemoryPool;
};

bTList<CAnimResourceFileProxy> gAnimLoader_ResourceFileList;

bool CAnimPlayer::m_audioQueued = false;
bool CAnimSettings::mDebugPrintEnabled = false;
uint32 CAnimMarker::mMarkerHash_StartCountdown = 0;

AnimSceneLoadInfo gAnimLoader_Info;
static bool gAnimLoader_InProgress = 0;
static int gAnimLoader_CurSharedFilePosition = 0;
static int gAnimLoader_CurSceneFilePosition = 0;
static void *gAnimLoader_MemPointer = 0;
static void *gAnimLoader_MovingPointer = 0;
static CAnimResourceFileProxy::MemoryPool gAnimLoader_UsingMemoryPool = CAnimResourceFileProxy::Main;
int gAnimCfg_Small_NIS_Size = 20000;
extern int CarLoaderMemoryPoolNumber;
void UpdateCopDoorPositions(float time_step);

// STRIPPED
void CAnimSettings::SetDebugPrintEnabled(bool enable) {
    mDebugPrintEnabled = enable;
}

bool CAnimSettings::IsDebugPrintEnabled() {
    return mDebugPrintEnabled;
}

// STRIPPED
char *GetDescription(uint32 anim_id) {
    CAnimSceneData *scene_data = CAnimSceneData::FindAnimSceneData(anim_id);

    if (scene_data) {
        return scene_data->GetSceneInfo()->Description;
    }

    return nullptr;
}

CAnimPlayer::CAnimPlayer() {
    CAnimMarker::mMarkerHash_StartCountdown = bStringHash("StartCountdown");
    mWorldAnimScene = nullptr;
}

CAnimPlayer::~CAnimPlayer() {
    KillWorldAnimScene(true, false);
}

// STRIPPED
int GetAnimLoader_MaxSize() {}

void AnimLoader_Init() {
    gAnimLoader_InProgress = true;
    gAnimLoader_CurSharedFilePosition = 0;
    gAnimLoader_CurSceneFilePosition = 0;
    gAnimLoader_MovingPointer = gAnimLoader_MemPointer;
}

void AnimLoader_IncrementAndAlignUp(int &ref, int size) {
    ref += size;
    int remainder = ref & 0xF;
    if (remainder != 0) {
        int temp = ref + 0x10;
        ref = temp - remainder;
    }
}

int AnimLoader_SizeNeeded() {
    int cur_shared_file_position = 0;
    int cur_scene_file_position = 0;
    int size_needed = 0;

    while (true) {
        if (cur_shared_file_position < static_cast<int>(gAnimLoader_Info.mSharedFileCount)) {
            char *filename = TheAnimDirectory->GetFileName(cur_shared_file_position + gAnimLoader_Info.mSharedFileStartIndex);
            int file_size = bFileSize(filename);
            cur_shared_file_position++;
            AnimLoader_IncrementAndAlignUp(size_needed, file_size);
        } else if (cur_scene_file_position < static_cast<int>(gAnimLoader_Info.mSceneFileCount)) {
            char *filename = TheAnimDirectory->GetFileName(cur_scene_file_position + gAnimLoader_Info.mSceneFileStartIndex);
            int file_size = bFileSize(filename);
            cur_scene_file_position++;
            AnimLoader_IncrementAndAlignUp(size_needed, file_size);
        } else {
            return size_needed;
        }
    }
}

void AnimLoader_Callback(int);

void AnimLoader_LoadResourceFile(const char *filename) {
    int file_size = bFileSize(filename);
    ResourceFile *res_file = CreateResourceFile(filename, RESOURCE_FILE_NIS, 0, 0, file_size);
    res_file->AssignMemory(gAnimLoader_MovingPointer, 0, filename);
    res_file->BeginLoading(reinterpret_cast<void (*)(void *)>(AnimLoader_Callback), nullptr);
    CAnimResourceFileProxy *proxy = new CAnimResourceFileProxy(res_file, gAnimLoader_MovingPointer, gAnimLoader_UsingMemoryPool);
    gAnimLoader_ResourceFileList.AddTail(proxy);
    AnimLoader_IncrementAndAlignUp(reinterpret_cast<int &>(gAnimLoader_MovingPointer), file_size);
}

void AnimLoader_NextStep() {
    if (gAnimLoader_InProgress) {
        if (gAnimLoader_CurSharedFilePosition < static_cast<int>(gAnimLoader_Info.mSharedFileCount)) {
            AnimLoader_LoadResourceFile(TheAnimDirectory->GetFileName(gAnimLoader_CurSharedFilePosition + gAnimLoader_Info.mSharedFileStartIndex));
            gAnimLoader_CurSharedFilePosition++;
        } else if (gAnimLoader_CurSceneFilePosition < static_cast<int>(gAnimLoader_Info.mSceneFileCount)) {
            AnimLoader_LoadResourceFile(TheAnimDirectory->GetFileName(gAnimLoader_CurSceneFilePosition + gAnimLoader_Info.mSceneFileStartIndex));
            gAnimLoader_CurSceneFilePosition++;
        } else {
            gAnimLoader_InProgress = false;
        }
    }
}

void AnimLoader_Callback(int) {
    AnimLoader_NextStep();
}

// STRIPPED
void AnimLoader_UnloadAll() {}

// STRIPPED
void CAnimPlayer::StreamCued() {}

// UNSOLVED
bool CAnimPlayer::Load(uint32 anim_id, int camera_track_number, bool DisableZoneSwitching) {
    if (gAnimLoader_InProgress) {
        return false;
    }

    CAnimSceneData *scene_data = CAnimSceneData::FindAnimSceneData(anim_id);
    if (scene_data) {
        return false;
    }

    if (!TheAnimDirectory) {
        return false;
    }

    uint32 scene_count = TheAnimDirectory->GetSceneCount();
    bool scene_found = false;
    uint32 scene_slot = 0;

    if (scene_slot < scene_count) {
        do {
            AnimSceneLoadInfo info;
            TheAnimDirectory->GetSceneLoadInfo(scene_slot, info);
            if (info.mAnimSceneHash == anim_id) {
                scene_found = true;
                gAnimLoader_Info = info;
                break;
            }
            scene_slot++;
        } while (scene_slot < scene_count);
    }

    bool loading_has_begun = false;

    if (!scene_found) {
        return loading_has_begun;
    }

    int size_needed = AnimLoader_SizeNeeded();
    if (size_needed >= 0xDBBA1) {
        gAnimLoader_MemPointer = nullptr;
    } else if (size_needed > gAnimCfg_Small_NIS_Size) {
        TheTrackStreamer.BlockUntilLoadingComplete();
        TheTrackStreamer.MakeSpaceInPool(size_needed, true);
        gAnimLoader_MemPointer = TheTrackStreamer.AllocateUserMemory(size_needed, "NISMemory", 0);
        gAnimLoader_UsingMemoryPool = CAnimResourceFileProxy::CarPool;
        if (!gAnimLoader_MemPointer) {
            TheCarLoader.MakeSpaceInPool(size_needed);
            gAnimLoader_MemPointer = TheCarLoader.AllocateUserMemory(size_needed, "NISMemory");
            gAnimLoader_UsingMemoryPool = CAnimResourceFileProxy::TrackStream;
        }
    } else {
        gAnimLoader_UsingMemoryPool = CAnimResourceFileProxy::Main;
        gAnimLoader_MemPointer = bMalloc(size_needed, 0x2000);
    }

    if (gAnimLoader_MemPointer) {
        AnimLoader_Init();
        loading_has_begun = true;
        AnimLoader_NextStep();
    }
    return loading_has_begun;
}

bool CAnimPlayer::Unload(uint32 anim_id) {
    if (gAnimLoader_InProgress) {
        return false;
    }

    TheTrackStreamer.EnableZoneSwitching();
    CAnimSceneData *scene_data = CAnimSceneData::FindAnimSceneData(anim_id);

    if (scene_data) {
        CAnimResourceFileProxy *proxy = gAnimLoader_ResourceFileList.GetHead();
        while (proxy != gAnimLoader_ResourceFileList.EndOfList()) {
            CAnimResourceFileProxy *next_proxy = proxy->GetNext();
            if (proxy->mResourceFile) {
                UnloadResourceFile(proxy->mResourceFile);
                if (proxy->mMemoryPool == CAnimResourceFileProxy::TrackStream) {
                    void *mem = proxy->mResourceFile->GetMemory();
                    if (mem) {
                        TheTrackStreamer.FreeUserMemory(mem);
                    }
                } else if (proxy->mMemoryPool == CAnimResourceFileProxy::CarPool) {
                    void *mem = proxy->mResourceFile->GetMemory();
                    if (mem) {
                        bFree(mem);
                    }
                } else {
                    bFree(proxy->mResourceFile->GetMemory());
                }
                gAnimLoader_ResourceFileList.Remove(proxy);
                delete proxy;
                return true;
            }
            proxy = next_proxy;
        }
    }

    return false;
}

// STRIPPED
bool CAnimPlayer::UnloadAll() {}

// STRIPPED
bool CAnimPlayer::UnloadCategory(eAnimCategory category_id) {}

bool CAnimPlayer::IsLoaded(uint32 anim_id) {
    if (gAnimLoader_InProgress) {
        return false;
    }
    CAnimSceneData *scene_data = CAnimSceneData::FindAnimSceneData(anim_id);
    if (scene_data) {
        m_audioQueued = false;
        return true;
    }
    return false;
}

int CAnimPlayer::CreateAnimInstance(uint32 anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index) {
    CAnimSceneData *anim_scene_data = CAnimSceneData::FindAnimSceneData(anim_id);
    int result = 0;
    if (anim_scene_data) {
        result = CreateAnimScene(anim_scene_data, camera_track_number, anim_candidate_type, anim_candidate_index);
    }
    return result;
}

void CAnimPlayer::DeleteAnimInstance(AnimHandle anim_handle) {
    DestroyAnimScene(anim_handle);
}

int CAnimPlayer::CreateAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index) {
    CAnimScene *anim_scene = new CAnimScene(anim_scene_data, camera_track_number, anim_candidate_type, anim_candidate_index);
    if (anim_scene->Init()) {
        mInstancedAnimSceneList.AddTail(anim_scene);
        return anim_scene->GetHandle();
    }
    return 0;
}

void CAnimPlayer::DestroyAnimScene(AnimHandle anim_handle) {
    CAnimScene *anim_scene = FindAnimScene(anim_handle);

    if (anim_scene && anim_scene->Purge()) {
        anim_scene->Remove();
        delete anim_scene;
    }
}

int CAnimPlayer::CreateAndPlayAnim(uint32 anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index) {
    AnimHandle anim_handle = CreateAnimInstance(anim_id, camera_track_number, anim_candidate_type, anim_candidate_index);
    if (anim_handle == 0) {
        return 0;
    }
    Play(anim_handle);
    return anim_handle;
}

CAnimScene *CAnimPlayer::FindAnimScene(AnimHandle anim_handle) {
    CAnimScene *anim_scene = mInstancedAnimSceneList.GetHead();
    while (anim_scene != mInstancedAnimSceneList.EndOfList()) {
        if (anim_handle == anim_scene->GetHandle()) {
            return anim_scene;
        }
        anim_scene = anim_scene->GetNext();
    }
    return nullptr;
}

// STRIPPED
int CAnimPlayer::FindHandle(eAnimCategory category_id) {}

// STRIPPED
int CAnimPlayer::GetSceneType(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::SetPropertyEnabled(AnimHandle anim_handle, eAnimProperty property_id, bool enable) {}

// STRIPPED
bool CAnimPlayer::IsPropertyEnabled(AnimHandle anim_handle, eAnimProperty property_id) {}

// STRIPPED
bool CAnimPlayer::AreAnyWithThisPropertyEnabled(eAnimProperty property_id) {}

// STRIPPED
bool CAnimPlayer::AreAnyControllingCamera() {}

// STRIPPED
bool CAnimPlayer::SetTime(AnimHandle anim_handle, float time) {}

// STRIPPED
bool CAnimPlayer::GetTime(AnimHandle anim_handle, float &time) {}

// STRIPPED
bool CAnimPlayer::Cue(AnimHandle anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene) {
        return scene->Cue();
    }

    return false;
}

bool CAnimPlayer::Play(AnimHandle anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene) {
        return scene->Play();
    }

    return false;
}

bool CAnimPlayer::Stop(AnimHandle anim_handle, bool delete_instance) {
    CAnimScene *scene = FindAnimScene(anim_handle);
    if (scene) {
        bool stopped = scene->Stop();
        if (stopped && delete_instance) {
            DeleteAnimInstance(anim_handle);
        }
        return stopped;
    }
    return false;
}

// STRIPPED
bool CAnimPlayer::Pause(int anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene) {
        return scene->Pause();
    }

    return false;
}

// STRIPPED
bool CAnimPlayer::UnPause(AnimHandle anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene) {
        return scene->UnPause();
    }

    return false;
}

// STRIPPED
bool CAnimPlayer::StopAll(bool delete_instances) {}

// STRIPPED
bool CAnimPlayer::StopCategory(eAnimCategory category_id, bool delete_instances) {}

bool CAnimPlayer::PauseAll() {
    bool any_success = false;

    for (CAnimScene *anim_scene = mInstancedAnimSceneList.GetHead(); anim_scene != mInstancedAnimSceneList.EndOfList();
         anim_scene = anim_scene->GetNext()) {

        bool success = anim_scene->Pause();
        if (!any_success) {
            any_success = success;
        }
    }

    return any_success;
}

bool CAnimPlayer::UnPauseAll() {
    bool any_success = false;

    for (CAnimScene *anim_scene = mInstancedAnimSceneList.GetHead(); anim_scene != mInstancedAnimSceneList.EndOfList();
         anim_scene = anim_scene->GetNext()) {

        bool success = anim_scene->UnPause();
        if (!any_success) {
            any_success = success;
        }
    }

    return any_success;
}

// STRIPPED
bool CAnimPlayer::IsCued(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::IsPlaying(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::IsStopped(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::IsPaused(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::IsFinished(AnimHandle anim_handle) {}

// STRIPPED
bool CAnimPlayer::AreAllPlaying() {}

// STRIPPED
bool CAnimPlayer::AreAllStopped() {}

// STRIPPED
bool CAnimPlayer::AreAllPaused() {}

void CAnimPlayer::UpdateTime(float time_step) {
    if (mWorldAnimScene != nullptr) {
        mWorldAnimScene->UpdateTime(time_step);
    }
    UpdateCopDoorPositions(time_step);
}

void CAnimPlayer::BeginOnlinePause() {}

void CAnimPlayer::EndOnlinePause() {}

bool CAnimPlayer::Init() {
    return true;
}

// STRIPPED
void CAnimPlayer::Purge() {}

void CAnimPlayer::InitWorldAnimScene() {
    if (!DisableWorldAnimations && !AnimCfg_DisableWorldAnimations) {
        int numEntries = TheWorldAnimInstanceDirectory.GetNumInstanceEntries();
        if (!mWorldAnimScene && numEntries > 0) {
            mWorldAnimScene = new CAnimWorldScene();
        }
    }
}

CAnimWorldScene *CAnimPlayer::GetWorldAnimScene() {
    return mWorldAnimScene;
}

void CAnimPlayer::KillWorldAnimScene(bool full_unload, bool quickrace_drag_restart) {
    if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
        if (mWorldAnimScene) {
            goto clear;
        }
        return;
    } else {
        if (!mWorldAnimScene) {
            goto deinit;
        }
    }

clear:
    mWorldAnimScene->ClearAllAnimations();
    if (mWorldAnimScene) {
        delete mWorldAnimScene;
    }
    mWorldAnimScene = nullptr;

deinit:
    TheWorldAnimInstanceDirectory.DeInit(full_unload, quickrace_drag_restart);
}

extern float NISCopCarDoorOpenAmount[4];

float gCopCarDoorAnim_CurrentTime[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float gCopCarDoorAnim_AnimLength[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float gCopCarDoorAnim_StartPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float gCopCarDoorAnim_Delta[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void StartCopDoorAnim(int door, float startPos, float AnimLength, float endPos) {
    if (door < 0 || 3 < door) {
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
    for (int door = 0; door < 4; door++) {
        float animLength = gCopCarDoorAnim_AnimLength[door];
        if (animLength != 0.0f) {
            gCopCarDoorAnim_CurrentTime[door] = gCopCarDoorAnim_CurrentTime[door] + time;
            if (gCopCarDoorAnim_CurrentTime[door] <= 0.0f) {
                NISCopCarDoorOpenAmount[door] = gCopCarDoorAnim_StartPos[door];
            } else if (gCopCarDoorAnim_CurrentTime[door] < animLength) {
                float ratio = gCopCarDoorAnim_CurrentTime[door] / animLength;
                NISCopCarDoorOpenAmount[door] = gCopCarDoorAnim_StartPos[door] + ratio * gCopCarDoorAnim_Delta[door];
            } else {
                gCopCarDoorAnim_AnimLength[door] = 0.0f;
                NISCopCarDoorOpenAmount[door] = gCopCarDoorAnim_StartPos[door] + gCopCarDoorAnim_Delta[door];
            }
        }
    }
}
