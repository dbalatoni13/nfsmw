#include "GRuntimeInstance.h"

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
    GManager::Get().RegisterInstance(this);
    AddToTypeList(type);
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
    SetConnectionBuffer(buffer, numEntries);
    SetFlag(4);
}

void GRuntimeInstance::ConnectToInstance(const Attrib::Key &key, int index, GRuntimeInstance *instance) {
    unsigned int packedKey = MakePackedKey(key, index);
    ConnectedInstance &connected = mConnected[mNumConnected];
    connected.mIndexedKey = packedKey;
    connected.mInstance = instance;
    mNumConnected++;
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
        if (connected.mIndexedKey == targetKey) {
            return connected.mInstance;
        }
        if (connected.mIndexedKey < targetKey) {
            lower = middle + 1;
        } else {
            upper = middle - 1;
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
    if (head) {
        mNext = head;
        mPrev = head->mPrev;
        head->mPrev->mNext = this;
        head->mPrev = this;
    } else {
        mNext = this;
        mPrev = this;
        head = this;
    }
}

void GRuntimeInstance::RemoveFromTypeList() {
    for (unsigned int onType = 0; onType < kGameplayObjType_Count; onType++) {
        if (sRingListHead[onType] == this) {
            if (mNext == this) {
                sRingListHead[onType] = nullptr;
            } else {
                sRingListHead[onType] = mNext;
                mPrev->mNext = mNext;
                mNext->mPrev = mPrev;
            }
            mNext = nullptr;
            mPrev = nullptr;
            return;
        }
    }
    if (mNext) {
        mPrev->mNext = mNext;
        mNext->mPrev = mPrev;
        mNext = nullptr;
        mPrev = nullptr;
    }
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
    }
    if (GetType() == kGameplayObjType_Trigger) {
        GTrigger *trigger = static_cast<GTrigger *>(this);
        trigger->GetPosition(pos);
        return true;
    }
    return false;
}

bool GRuntimeInstance::GetDirection(UMath::Vector3 &dir) {
    if (GetType() == kGameplayObjType_Marker) {
        GMarker *marker = static_cast<GMarker *>(this);
        dir = marker->GetDirection();
        return true;
    }
    if (GetType() == kGameplayObjType_Trigger) {
        GTrigger *trigger = static_cast<GTrigger *>(this);
        dir = trigger->GetDirection();
        return true;
    }
    return false;
}

GCollectionKey::GCollectionKey(GRuntimeInstance *inst) : mCollectionKey(inst->GetCollection()) {}

GCollectionKey::operator GRuntimeInstance *() const {
    return GManager::Get().FindInstance(mCollectionKey);
}