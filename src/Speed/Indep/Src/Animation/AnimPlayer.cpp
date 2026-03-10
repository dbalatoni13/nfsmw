#include "AnimPlayer.hpp"
#include "AnimDirectory.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct CAnimMarker {
    static unsigned int mMarkerHash_StartCountdown;
};

extern AnimDirectory *TheAnimDirectory;
int AnimLoader_NextStep();
extern int AnimCfg_DisableWorldAnimations;

struct AnimDirectoryLayout {
    unsigned int mAnimSceneCount;
    AnimSceneLoadInfo mAnimSceneLoadInfo[ANIM_SCENE_MAX];
    AnimFileLoadInfo mAnimFileLoadInfo;
};

struct CAnimSceneNodeLayout {
    void *vtable;
    bNode node;
    int mHandle;
};

struct CAnimResourceFileProxy;
extern bTList<CAnimResourceFileProxy> gAnimLoader_ResourceFileList;
extern AnimSceneLoadInfo gAnimLoader_Info;
static bool gAnimLoader_InProgress;
static int gAnimLoader_CurSharedFilePosition;
static int gAnimLoader_CurSceneFilePosition;
static void *gAnimLoader_MemPointer;
static void *gAnimLoader_MovingPointer;
void UpdateCopDoorPositions(float time_step);

bool CAnimSettings::mDebugPrintEnabled = false;

bool CAnimSettings::IsDebugPrintEnabled() {
    return mDebugPrintEnabled;
}

CAnimPlayer::CAnimPlayer()
    : mWorldAnimScene(nullptr) {
    CAnimMarker::mMarkerHash_StartCountdown = bStringHash("StartCountdown");
}

CAnimPlayer::~CAnimPlayer() {
    KillWorldAnimScene(true, false);
    while (!mInstancedAnimSceneList.IsEmpty()) {
        delete mInstancedAnimSceneList.RemoveHead();
    }
}

void AnimLoader_Init() {
    gAnimLoader_InProgress = true;
    gAnimLoader_MovingPointer = gAnimLoader_MemPointer;
    gAnimLoader_CurSharedFilePosition = 0;
    gAnimLoader_CurSceneFilePosition = 0;
}

void AnimLoader_IncrementAndAlignUp(int &pos, int size) {
    unsigned int value = pos + size;

    pos = value;
    if ((value & 0xF) != 0) {
        pos = (value + 0x10) - (value & 0xF);
    }
}

int AnimLoader_SizeNeeded() {
    int cur_shared_file_position = 0;
    int cur_scene_file_position = 0;
    int size_needed = 0;

    while (true) {
        int file_index;
        if (cur_shared_file_position < static_cast< int >(gAnimLoader_Info.mSharedFileCount)) {
            file_index = cur_shared_file_position + gAnimLoader_Info.mSharedFileStartIndex;
            cur_shared_file_position++;
        } else {
            if (cur_scene_file_position >= static_cast< int >(gAnimLoader_Info.mSceneFileCount)) {
                return size_needed;
            }
            file_index = cur_scene_file_position + gAnimLoader_Info.mSceneFileStartIndex;
            cur_scene_file_position++;
        }
        AnimLoader_IncrementAndAlignUp(
            size_needed,
            bFileSize(reinterpret_cast< AnimDirectoryLayout * >(TheAnimDirectory)->mAnimFileLoadInfo.mAnimFileNameTable[file_index]));
    }
}

void AnimLoader_Callback(int param_1) {
    AnimLoader_NextStep();
}

void CAnimPlayer::InitWorldAnimScene() {
    if (!DisableWorldAnimations && !AnimCfg_DisableWorldAnimations &&
        TheWorldAnimInstanceDirectory.GetNumInstanceEntries() > 0 && mWorldAnimScene == nullptr) {
        mWorldAnimScene = new CAnimWorldScene();
    }
}

CAnimWorldScene *CAnimPlayer::GetWorldAnimScene() {
    return mWorldAnimScene;
}

void CAnimPlayer::KillWorldAnimScene(bool full_unload, bool quickrace_drag_restart) {
    CAnimWorldScene *world_anim_scene;

    if (!DisableWorldAnimations && !AnimCfg_DisableWorldAnimations) {
        world_anim_scene = mWorldAnimScene;
        if (world_anim_scene == nullptr) {
            goto deinit;
        }
    } else {
        world_anim_scene = mWorldAnimScene;
        if (world_anim_scene == nullptr) {
            return;
        }
    }

    world_anim_scene->ClearAllAnimations();
    world_anim_scene = mWorldAnimScene;
    if (world_anim_scene != nullptr) {
        delete world_anim_scene;
    }
    mWorldAnimScene = nullptr;

deinit:
    TheWorldAnimInstanceDirectory.DeInit(full_unload, quickrace_drag_restart);
}

bool CAnimPlayer::Init() {
    return true;
}

void CAnimPlayer::UpdateTime(float time_step) {
    if (mWorldAnimScene != nullptr) {
        mWorldAnimScene->UpdateTime(time_step);
    }
    UpdateCopDoorPositions(time_step);
}

bool CAnimPlayer::Play(int anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene == nullptr) {
        return false;
    }
    return scene->Play();
}

bool CAnimPlayer::Stop(int anim_handle, bool delete_instance) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene == nullptr) {
        return false;
    }

    bool stopped = scene->Stop();
    if (stopped && delete_instance) {
        DeleteAnimInstance(anim_handle);
    }
    return stopped;
}

CAnimScene *CAnimPlayer::FindAnimScene(int anim_handle) {
    CAnimScene *list_end = nullptr;
    if (this != nullptr) {
        list_end = reinterpret_cast< CAnimScene * >(reinterpret_cast< char * >(this) - 4);
    }

    bNode *node = mInstancedAnimSceneList.HeadNode.Next;
    CAnimScene *scene = nullptr;
    if (node != nullptr) {
        scene = reinterpret_cast< CAnimScene * >(reinterpret_cast< char * >(node) - 4);
    }

    if (scene != list_end) {
        do {
            if (scene->GetHandle() == anim_handle) {
                return scene;
            }
            node = reinterpret_cast< CAnimSceneNodeLayout * >(scene)->node.Next;
            scene = nullptr;
            if (node != nullptr) {
                scene = reinterpret_cast< CAnimScene * >(reinterpret_cast< char * >(node) - 4);
            }
        } while (scene != list_end);
    }

    return nullptr;
}

void CAnimPlayer::DestroyAnimScene(int anim_handle) {
    CAnimScene *scene = FindAnimScene(anim_handle);

    if (scene != nullptr && scene->Purge()) {
        bNode *prev = reinterpret_cast< CAnimSceneNodeLayout * >(scene)->node.Prev;
        bNode *next = reinterpret_cast< CAnimSceneNodeLayout * >(scene)->node.Next;
        prev->Next = next;
        next->Prev = prev;
        delete scene;
    }
}

void CAnimPlayer::DeleteAnimInstance(int anim_handle) {
    DestroyAnimScene(anim_handle);
}
