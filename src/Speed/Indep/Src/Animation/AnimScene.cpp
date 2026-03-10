#include "AnimScene.hpp"

struct NisScene {
    unsigned int mSceneNameHash;
    char mSceneName[16];
    char *Description;
    int SceneType;
    int HaveLayout;
    int HaveCarAnimation;
    int NumberOfCars;
    int StartFrame;
    int VanishFrame;
    char SeeulatorOverlayName[16];
};

struct CAnimSceneDataLayout {
    bNode node;
    bChunk *mChunk;
    NisScene *mNisScene;
};

struct CAnimEntityDataLayout {
    bNode node;
    int mType;
    void *mData;
    int mSize;
};

bTList<CAnimSceneData> g_loadedAnimSceneDataList;
int mHandleCounter = 0;

CAnimSceneData::CAnimSceneData(bChunk *chunk)
    : mChunk(chunk),            //
      mNisScene(nullptr) {
    mAnimEntityDataList.HeadNode.Next = &mAnimEntityDataList.HeadNode;
    mAnimEntityDataList.HeadNode.Prev = &mAnimEntityDataList.HeadNode;
}

CAnimSceneData::~CAnimSceneData() {
    while (mAnimEntityDataList.HeadNode.Next != &mAnimEntityDataList.HeadNode) {
        delete mAnimEntityDataList.RemoveHead();
    }
}

CAnimSceneData *CAnimSceneData::FindAnimSceneData(unsigned int scene_name_hash) {
    if (g_loadedAnimSceneDataList.HeadNode.Next != &g_loadedAnimSceneDataList.HeadNode) {
        CAnimSceneData *scene_data = reinterpret_cast< CAnimSceneData * >(g_loadedAnimSceneDataList.HeadNode.Next);
        NisScene *scene_info = scene_data->mNisScene;

        while (true) {
            if (scene_name_hash == scene_info->mSceneNameHash) {
                return scene_data;
            }
            scene_data = scene_data->GetNext();
            if (scene_data == reinterpret_cast< CAnimSceneData * >(&g_loadedAnimSceneDataList)) {
                break;
            }
            scene_info = scene_data->mNisScene;
        }
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
    *reinterpret_cast< int * >(reinterpret_cast< char * >(data) + 0x14) = reinterpret_cast< int >(data) + 0x40;
    mNisScene = reinterpret_cast< NisScene * >(data);
    EndianSwapHeaderData();
}

void CAnimSceneData::AddEntityData(void *data, int size) {
    CAnimEntityFactory::EndianSwapEntityData(data, size);

    CAnimEntityDataLayout *entity_data = reinterpret_cast< CAnimEntityDataLayout * >(new CAnimEntityData());
    bNode *tail = mAnimEntityDataList.HeadNode.Prev;

    entity_data->mType = *reinterpret_cast< int * >(data);
    entity_data->mData = data;
    entity_data->mSize = size;
    tail->Next = &entity_data->node;
    mAnimEntityDataList.HeadNode.Prev = &entity_data->node;
    entity_data->node.Next = &mAnimEntityDataList.HeadNode;
    entity_data->node.Prev = tail;
}

CAnimSceneData *CreateAnimSceneData(bChunk *nested_chunk, bChunk *sub_chunk) {
    CAnimSceneData *scene_data = new CAnimSceneData(nested_chunk);

    if (scene_data != nullptr) {
        scene_data->InitHeaderData(sub_chunk + 1, sub_chunk->Size);
        g_loadedAnimSceneDataList.HeadNode.Prev->Next = scene_data;
        scene_data->Prev = g_loadedAnimSceneDataList.HeadNode.Prev;
        g_loadedAnimSceneDataList.HeadNode.Prev = scene_data;
        scene_data->Next = &g_loadedAnimSceneDataList.HeadNode;
    }

    return scene_data;
}

int LoaderAnimSceneData(bChunk *chunk) {
    int result = 0;

    if (chunk->ID == 0x80037020) {
        bChunk *sub_chunk = chunk + 1;
        bChunk *last_chunk = reinterpret_cast< bChunk * >(reinterpret_cast< int * >(&chunk[1].ID) + chunk->Size / 4);
        CAnimSceneData *scene_data = nullptr;

        while (sub_chunk != last_chunk) {
            unsigned int chunk_id = sub_chunk->ID;
            int sub_size = sub_chunk->Size;

            if (chunk_id == 0x37030) {
                scene_data = CreateAnimSceneData(chunk, sub_chunk);
            } else if (chunk_id == 0x37040 && scene_data != nullptr) {
                void *data =
                    reinterpret_cast< void * >(reinterpret_cast< uintptr_t >(&sub_chunk[2].Size + 1) & static_cast< uintptr_t >(0xFFFFFFF0));
                scene_data->AddEntityData(data, sub_chunk->Size - (reinterpret_cast< int >(data) - reinterpret_cast< int >(sub_chunk + 1)));
            }

            sub_chunk = reinterpret_cast< bChunk * >(reinterpret_cast< int * >(&sub_chunk[1].ID) + sub_size / 4);
        }
        result = 1;
    }

    return result;
}

int UnloaderAnimSceneData(bChunk *chunk) {
    int result = 0;

    if (chunk->ID == 0x80037020) {
        result = 1;
        if (g_loadedAnimSceneDataList.HeadNode.Next != &g_loadedAnimSceneDataList.HeadNode) {
            CAnimSceneData *scene_data = reinterpret_cast< CAnimSceneData * >(g_loadedAnimSceneDataList.HeadNode.Next);

            while (reinterpret_cast< CAnimSceneDataLayout * >(scene_data)->mChunk != chunk) {
                scene_data = scene_data->GetNext();
                if (scene_data == reinterpret_cast< CAnimSceneData * >(&g_loadedAnimSceneDataList)) {
                    return 1;
                }
            }

            scene_data->Remove();
            delete scene_data;
        }
    }

    return result;
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

int CAnimScene::GenerateHandle() {
    mHandleCounter++;
    if (mHandleCounter > 0xFFFF) {
        mHandleCounter = 1;
    }
    return mHandleCounter;
}

unsigned int CAnimScene::GetAnimID() {
    return reinterpret_cast< CAnimSceneDataLayout * >(mAnimSceneData)->mNisScene->mSceneNameHash;
}

unsigned int CAnimScene::GetSceneHash() {
    return GetAnimID();
}

int CAnimScene::GetSceneType() {
    return reinterpret_cast< CAnimSceneDataLayout * >(mAnimSceneData)->mNisScene->SceneType;
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
    return mPlayStatus > Paused;
}

bool CAnimScene::IsPaused() {
    return mPlayStatus == Paused;
}

bool CAnimScene::SetPropertyEnabled(eAnimProperty property_id, bool enable) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property == nullptr) {
        AddProperty(property_id, enable);
    } else {
        anim_property->SetEnabled(enable);
    }

    return anim_property != nullptr;
}

bool CAnimScene::IsPropertyEnabled(eAnimProperty property_id) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property == nullptr) {
        return false;
    }
    return anim_property->IsEnabled();
}

void CAnimScene::ResetTime() {
    SetTime(mTimeStart);
}

void CAnimScene::JumpToEnd() {
    SetTime(mTimeTotalLength);
}

void CAnimScene::AddProperty(eAnimProperty property_id, bool enabled) {
    CAnimProperty *anim_property = FindProperty(property_id);

    if (anim_property == nullptr) {
        anim_property = new CAnimProperty(property_id, enabled);
        bNode *tail = mAnimPropertyList.HeadNode.Prev;
        tail->Next = anim_property;
        mAnimPropertyList.HeadNode.Prev = anim_property;
        anim_property->Next = &mAnimPropertyList.HeadNode;
        anim_property->Prev = tail;
    }
}

CAnimProperty *CAnimScene::FindProperty(eAnimProperty property_id) {
    CAnimProperty *anim_property = reinterpret_cast< CAnimProperty * >(mAnimPropertyList.HeadNode.Next);

    while (true) {
        if (anim_property == reinterpret_cast< CAnimProperty * >(&mAnimPropertyList)) {
            return nullptr;
        }
        if (anim_property->GetType() == property_id) {
            break;
        }
        anim_property = anim_property->GetNext();
    }
    return anim_property;
}
