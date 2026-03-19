#include "GRuntimeInstance.h"

#include "GActivity.h"
#include "GCharacter.h"
#include "GManager.h"
#include "GMarker.h"
#include "GTrigger.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

#include <algorithm>

GRuntimeInstance *GRuntimeInstance::sRingListHead[6];

GRuntimeInstance::GRuntimeInstance(const Attrib::Key &key, GameplayObjType type)
    : Attrib::Gen::gameplay(key, 0, nullptr) //
{
    mFlags = 0;
    mNumConnected = 0;
    mConnected = nullptr;
    mPrev = nullptr;
    mNext = nullptr;
    AddToTypeList(type);
    GManager::Get().RegisterInstance(this);
    SetFlag(1);
}

GRuntimeInstance::~GRuntimeInstance() {
    GManager::Get().UnregisterInstance(this);
    DisconnectInstances();
    RemoveFromTypeList();
    mFlags = mFlags & ~1;
}

void GRuntimeInstance::SetConnectionBuffer(ConnectedInstance *destBuffer, unsigned int numEntries) {
    mConnected = destBuffer;
    mNumConnected = 0;
}

void GRuntimeInstance::AllocateConnectionBuffer(unsigned int numEntries) {
    ConnectedInstance *buffer = new ConnectedInstance[numEntries];
    mConnected = buffer;
    mNumConnected = 0;
    SetFlag(4);
}

void GRuntimeInstance::ConnectToInstance(const Attrib::Key &key, int index, GRuntimeInstance *instance) {
    unsigned int packedKey = MakePackedKey(key, index);
    unsigned short connectedIndex = mNumConnected;
    ConnectedInstance &connected = mConnected[connectedIndex];

    connected.mIndexedKey = packedKey;
    connected.mInstance = instance;
    mNumConnected = connectedIndex + 1;
}

void GRuntimeInstance::LockConnections() {
    std::sort(mConnected, mConnected + mNumConnected);
    SetFlag(2);
}

GRuntimeInstance *GRuntimeInstance::GetConnectedInstance(const Attrib::Key &key, int index) const {
    unsigned int targetKey = MakePackedKey(key, index);
    int lower = 0;
    int upper = mNumConnected - 1;

    while (lower <= upper) {
        int middle = (lower + upper) / 2;
        ConnectedInstance &connected = mConnected[middle];

        if (targetKey > connected.mIndexedKey) {
            lower = middle + 1;
        } else if (targetKey < connected.mIndexedKey) {
            upper = middle - 1;
        } else {
            return connected.mInstance;
        }
    }

    return nullptr;
}

void GRuntimeInstance::ResetConnections() {
    mNumConnected = 0;
    mFlags = mFlags & ~2;
}

void GRuntimeInstance::DisconnectInstances() {
    if (GetFlag(4)) {
        if (mConnected) {
            delete[] mConnected;
        }
        mFlags = mFlags & ~4;
    }
    mNumConnected = 0;
    mConnected = nullptr;
    mFlags = mFlags & ~2;
}

unsigned int GRuntimeInstance::MakePackedKey(unsigned int key, int index) const {
    unsigned int key24 = GManager::Get().Get24BitAttributeKey(key);
    unsigned char index8 = static_cast<unsigned char>(index);
    return (key24 << 8) | index8;
}

void GRuntimeInstance::AddToTypeList(GameplayObjType type) {
    GRuntimeInstance *&head = sRingListHead[type];
    if (!head) {
        mNext = this;
        mPrev = this;
        head = this;
    } else {
        mNext = head;
        mPrev = head->mPrev;
        mPrev->mNext = this;
        mNext->mPrev = this;
    }
}

void GRuntimeInstance::RemoveFromTypeList() {
    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;

    for (unsigned int onType = 0; onType < kGameplayObjType_Count; onType++) {
        if (sRingListHead[onType] == this) {
            GRuntimeInstance *newHead = nullptr;

            if (mNext != this) {
                newHead = mNext;
            }

            sRingListHead[onType] = newHead;
            mPrev = nullptr;
            mNext = nullptr;
            return;
        }
    }

    mPrev = nullptr;
    mNext = nullptr;
}

unsigned int GRuntimeInstance::GetConnectionCount() const {
    return mNumConnected;
}

GRuntimeInstance *GRuntimeInstance::GetConnectionAt(unsigned int index) const {
    return mConnected[index].mInstance;
}

bool GRuntimeInstance::IsDerivedFromTemplate(unsigned int templateKey) const {
    unsigned int parentKey = GetParent();
    while (parentKey) {
        if (parentKey == templateKey) {
            return true;
        }
        {
            Attrib::Gen::gameplay parentObj(parentKey, 0, nullptr);
            parentKey = parentObj.GetParent();
        }
    }
    return false;
}

bool GRuntimeInstance::GetPosition(UMath::Vector3 &pos) {
    if (GetType() == kGameplayObjType_Marker) {
        GMarker *marker = static_cast<GMarker *>(this);
        pos = marker->GetPosition();
        return true;
    } else if (GetType() == kGameplayObjType_Trigger) {
        GTrigger *trigger = static_cast<GTrigger *>(this);
        trigger->GetPosition(pos);
        return true;
    } else {
        return false;
    }
}

bool GRuntimeInstance::GetDirection(UMath::Vector3 &dir) {
    if (GetType() == kGameplayObjType_Marker) {
        GMarker *marker = static_cast<GMarker *>(this);
        dir = marker->GetDirection();
        return true;
    } else if (GetType() == kGameplayObjType_Trigger) {
        GTrigger *trigger = static_cast<GTrigger *>(this);
        dir = trigger->GetDirection();
        return true;
    } else {
        return false;
    }
}

template <typename T>
struct GRuntimeInstanceFindObjectTraits;

template <>
struct GRuntimeInstanceFindObjectTraits<GActivity> {
    enum { kType = kGameplayObjType_Activity };
};

template <>
struct GRuntimeInstanceFindObjectTraits<GCharacter> {
    enum { kType = kGameplayObjType_Character };
};

template <>
struct GRuntimeInstanceFindObjectTraits<GMarker> {
    enum { kType = kGameplayObjType_Marker };
};

template <>
struct GRuntimeInstanceFindObjectTraits<GTrigger> {
    enum { kType = kGameplayObjType_Trigger };
};

template <typename T>
T *GRuntimeInstance::FindObject(unsigned int key) {
    T *instance = static_cast<T *>(sRingListHead[GRuntimeInstanceFindObjectTraits<T>::kType]);

    while (instance) {
        if (instance->GetCollection() == key) {
            return instance;
        }

        instance = static_cast<T *>(instance->GetNextRuntimeInstance());
        if (instance == sRingListHead[GRuntimeInstanceFindObjectTraits<T>::kType]) {
            return nullptr;
        }
    }

    return nullptr;
}

template GActivity *GRuntimeInstance::FindObject<GActivity>(unsigned int key);
template GCharacter *GRuntimeInstance::FindObject<GCharacter>(unsigned int key);
template GMarker *GRuntimeInstance::FindObject<GMarker>(unsigned int key);
template GTrigger *GRuntimeInstance::FindObject<GTrigger>(unsigned int key);

GCollectionKey::GCollectionKey(GRuntimeInstance *inst) : mCollectionKey(inst->GetCollection()) {}

GCollectionKey::operator GRuntimeInstance *() const {
    return GManager::Get().FindInstance(mCollectionKey);
}
