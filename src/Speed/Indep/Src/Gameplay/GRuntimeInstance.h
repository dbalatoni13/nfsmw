#ifndef GAMEPLAY_GRUNTIMEINSTANCE_H
#define GAMEPLAY_GRUNTIMEINSTANCE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

class GMarker;

// total size: 0x28
class GRuntimeInstance : public Attrib::Gen::gameplay {
  public:
    // total size: 0x8
    struct ConnectedInstance {
        bool operator<(const ConnectedInstance &other) const {
            return mIndexedKey < other.mIndexedKey;
        }

        unsigned int mIndexedKey;       // offset 0x0, size 0x4
        GRuntimeInstance *mInstance; // offset 0x4, size 0x4
    };

    typedef unsigned int PackedIndexedKey;

    GRuntimeInstance(const Attrib::Key &key, GameplayObjType type);

    virtual ~GRuntimeInstance();

    void SetConnectionBuffer(ConnectedInstance *destBuffer, unsigned int numEntries);

    void AllocateConnectionBuffer(unsigned int numEntries);

    void ConnectToInstance(const Attrib::Key &key, int index, GRuntimeInstance *instance);

    void LockConnections();

    GRuntimeInstance *GetConnectedInstance(const Attrib::Key &key, int index) const;

    void ResetConnections();

    void DisconnectInstances();

    unsigned int MakePackedKey(unsigned int key, int index) const;

    void AddToTypeList(GameplayObjType type);

    void RemoveFromTypeList();

    unsigned int GetConnectionCount() const;

    GRuntimeInstance *GetConnectionAt(unsigned int index) const;

    bool IsDerivedFromTemplate(unsigned int templateKey) const;

    bool GetPosition(UMath::Vector3 &pos);

    bool GetDirection(UMath::Vector3 &dir);

    virtual GameplayObjType GetType() const {
        return kGameplayObjType_Invalid;
    }

    bool GetFlag(unsigned int flag) const {
        return (mFlags & flag) != 0;
    }

    void SetFlag(unsigned int flag) {
        mFlags = mFlags | static_cast<unsigned short>(flag);
    }

    GRuntimeInstance *GetNextRuntimeInstance() const {
        return mNext;
    }

    static GRuntimeInstance *sRingListHead[6];

  private:
    unsigned short mFlags;            // offset 0x14, size 0x2
    unsigned short mNumConnected;     // offset 0x16, size 0x2
    ConnectedInstance *mConnected;    // offset 0x18, size 0x4
    GRuntimeInstance *mPrev;          // offset 0x1C, size 0x4
    GRuntimeInstance *mNext;          // offset 0x20, size 0x4
};

#endif
