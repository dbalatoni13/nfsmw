
// zGameplay emite _Rb_global<bool>::_Rebalance_for_erase como el original: sin
// esta macro STLport no expone el cuerpo de <stl/_tree.c>. Va en el primer .cpp
// de la SourceList porque despues las guardas de <stl/_tree.h> ya han cerrado.
#define _STLP_NO_FORCE_INSTANTIATE 1

#include "GRuntimeInstance.h"

#include "GActivity.h"
#include "GCharacter.h"
#include "GHandler.h"
#include "GManager.h"
#include "GMarker.h"
#include "GReflected.h"
#include "GState.h"
#include "GTrigger.h"

GRuntimeInstance *GRuntimeInstance::sRingListHead[kGameplayObjType_Count] = {};

GRuntimeInstance::GRuntimeInstance(const unsigned int &key, GameplayObjType type)
    : Attrib::Gen::gameplay(key, 0, NULL), mFlags(0), mNumConnected(0) {
    mConnected = NULL;
    mPrev = NULL;
    mNext = NULL;
    AddToTypeList(type);
    GManager::Get().RegisterInstance(this);
    SetFlag(0x1);
}

GRuntimeInstance::~GRuntimeInstance() {
    GManager::Get().UnregisterInstance(this);
    DisconnectInstances();
    RemoveFromTypeList();
    mFlags &= ~1;
}

void GRuntimeInstance::SetConnectionBuffer(ConnectedInstance *buffer, unsigned int maxConnections) {
    mConnected = buffer;
    mNumConnected = 0;
}

void GRuntimeInstance::AllocateConnectionBuffer(unsigned int maxConnections) {
    mConnected = new ConnectedInstance[maxConnections];
    mNumConnected = 0;
    SetFlag(0x4);
}

void GRuntimeInstance::ConnectToInstance(const unsigned int &key, int index, GRuntimeInstance *instance) {
    PackedIndexedKey packedKey = MakePackedKey(key, index);
    ConnectedInstance &slot = mConnected[mNumConnected++];
    slot.mIndexedKey = packedKey;
    slot.mInstance = instance;
}

void GRuntimeInstance::LockConnections() {
    std::sort(mConnected, mConnected + mNumConnected);
    SetFlag(0x2);
}

GRuntimeInstance *GRuntimeInstance::GetConnectedInstance(const unsigned int &key, int index) const {
    PackedIndexedKey packedKey = MakePackedKey(key, index);
    int first = 0;
    int last = mNumConnected - 1;
    while (first <= last) {
        int mid = (first + last) >> 1;
        const ConnectedInstance &slot = mConnected[mid];
        if (packedKey > slot.mIndexedKey) {
            first = mid + 1;
        } else if (packedKey < slot.mIndexedKey) {
            last = mid - 1;
        } else {
            return slot.mInstance;
        }
    }
    return NULL;
}

void GRuntimeInstance::ResetConnections() {
    mNumConnected = 0;
    mFlags &= ~0x2;
}

void GRuntimeInstance::DisconnectInstances() {
    if (mFlags & 0x4) {
        if (mConnected != NULL) {
            delete[] mConnected;
        }
        mFlags &= ~0x4;
    }
    mConnected = NULL;
    mNumConnected = 0;
    mFlags &= ~0x2;
}

GRuntimeInstance::PackedIndexedKey GRuntimeInstance::MakePackedKey(unsigned int key, int index) const {
    return ((key >> GManager::Get().mAttributeKeyShiftTo24) << 8) | (index & 0xFF);
}

void GRuntimeInstance::AddToTypeList(GameplayObjType type) {
    if (sRingListHead[type] == NULL) {
        mNext = this;
        mPrev = this;
        sRingListHead[type] = this;
        return;
    }
    mNext = sRingListHead[type];
    mPrev = sRingListHead[type]->mPrev;
    mPrev->mNext = this;
    mNext->mPrev = this;
}

void GRuntimeInstance::RemoveFromTypeList() {
    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;
    for (unsigned int i = 0; i < kGameplayObjType_Count; i++) {
        if (sRingListHead[i] == this) {
            sRingListHead[i] = (mNext == this) ? NULL : mNext;
        }
    }
    mNext = NULL;
    mPrev = NULL;
}

unsigned int GRuntimeInstance::GetConnectionCount() const {
    return mNumConnected;
}

GRuntimeInstance *GRuntimeInstance::GetConnectionAt(unsigned int index) const {
    return mConnected[index].mInstance;
}

bool GRuntimeInstance::IsDerivedFromTemplate(unsigned int templateKey) const {
    unsigned int parent = GetParent();
    while (parent != 0) {
        if (parent == templateKey) {
            return true;
        }
        Attrib::Gen::gameplay instance(parent, 0, NULL);
        parent = instance.GetParent();
    }
    return false;
}

bool GRuntimeInstance::GetPosition(UMath::Vector3 &position) {
    switch (GetType()) {
    case kGameplayObjType_Marker:
        position = static_cast<GMarker *>(this)->mPosition;
        return true;
    case kGameplayObjType_Trigger:
        static_cast<GTrigger *>(this)->GetPosition(position);
        return true;
    default:
        return false;
    }
}

bool GRuntimeInstance::GetDirection(UMath::Vector3 &direction) {
    switch (GetType()) {
    case kGameplayObjType_Marker:
        direction = static_cast<GMarker *>(this)->mDirection;
        return true;
    case kGameplayObjType_Trigger:
        direction = static_cast<GTrigger *>(this)->GetDirection();
        return true;
    default:
        return false;
    }
}

template class GObjectIterator<GState>;

template class GObjectIterator<GHandler>;

template class GObjectIterator<GTrigger>;

template class GObjectIterator<GActivity>;

GCollectionKey::GCollectionKey(GRuntimeInstance *instance) {
    mCollectionKey = (instance == NULL) ? 0 : instance->GetCollection();
}

template <class T>
T *GRuntimeInstance::FindObject(unsigned int key) {
    GameplayObjType type = GetGameplayType<T>();
    GRuntimeInstance *instance = sRingListHead[type];
    while (instance != NULL) {
        if (instance->GetCollection() == key) {
            return static_cast<T *>(instance);
        }
        instance = instance->mNext;
        if (instance == sRingListHead[type]) {
            break;
        }
    }
    return NULL;
}

template GTrigger *GRuntimeInstance::FindObject<GTrigger>(unsigned int key);

template GActivity *GRuntimeInstance::FindObject<GActivity>(unsigned int key);

template GCharacter *GRuntimeInstance::FindObject<GCharacter>(unsigned int key);

template GMarker *GRuntimeInstance::FindObject<GMarker>(unsigned int key);

template <class T>
GObjectIterator<T>::GObjectIterator(unsigned int flagMask) {
    mInstance = NULL;
    mFlagMask = flagMask;
    mInstance = GRuntimeInstance::sRingListHead[GetGameplayType<T>()];
    if (mInstance != NULL) {
        if ((mInstance->mFlags & mFlagMask) == 0) {
            Advance();
        }
    }
}

template <class T>
void GObjectIterator<T>::Advance() {
    GameplayObjType type = GetGameplayType<T>();
    do {
        mInstance = mInstance->mNext;
        if (mInstance == GRuntimeInstance::sRingListHead[type]) {
            mInstance = NULL;
            return;
        }
    } while ((mInstance->mFlags & mFlagMask) == 0);
}

GCollectionKey::operator GRuntimeInstance *(void) const {
    return GManager::Get().FindInstance(mCollectionKey);
}

