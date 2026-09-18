#ifndef GAMEPLAY_GRUNTIMEINSTANCE_H
#define GAMEPLAY_GRUNTIMEINSTANCE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GUserIncludes.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

class GActivity;
class GCharacter;
class GHandler;
class GMarker;
class GState;
class GTrigger;

template <class T> class GObjectIterator;

template <class T> inline GameplayObjType GetGameplayType();
template <> inline GameplayObjType GetGameplayType<GActivity>() { return kGameplayObjType_Activity; }
template <> inline GameplayObjType GetGameplayType<GCharacter>() { return kGameplayObjType_Character; }
template <> inline GameplayObjType GetGameplayType<GHandler>() { return kGameplayObjType_Handler; }
template <> inline GameplayObjType GetGameplayType<GMarker>() { return kGameplayObjType_Marker; }
template <> inline GameplayObjType GetGameplayType<GState>() { return kGameplayObjType_State; }
template <> inline GameplayObjType GetGameplayType<GTrigger>() { return kGameplayObjType_Trigger; }

class GCharacter;

// total size: 0x28
class GRuntimeInstance : public Attrib::Gen::gameplay {
  public:
    // total size: 0x8
    struct ConnectedInstance {
        // Definido aqui a proposito: en el original no hay simbolo __lt__ para este
        // tipo, o sea que estaba inlineado. Declararlo sin definir hacia que las 12
        // comparaciones de std::sort salieran como llamadas a un simbolo inexistente.
        bool operator<(const ConnectedInstance &other) const {
            return mIndexedKey < other.mIndexedKey;
        }

        unsigned int mIndexedKey;     // offset 0x0, size 0x4
        GRuntimeInstance *mInstance;  // offset 0x4, size 0x4
    };

    typedef unsigned int PackedIndexedKey;

    void *operator new(size_t size, void *ptr) {
        return ptr;
    }

    void operator delete(void *mem, void *ptr) {
    }

    USE_FASTALLOC(GRuntimeInstance);

    GRuntimeInstance(const unsigned int &key, GameplayObjType type);
    virtual ~GRuntimeInstance();
    virtual GameplayObjType GetType() const = 0;

    void SetConnectionBuffer(ConnectedInstance *buffer, unsigned int maxConnections);
    void AllocateConnectionBuffer(unsigned int maxConnections);
    void ConnectToInstance(const unsigned int &key, int index, GRuntimeInstance *instance);
    void LockConnections();
    GRuntimeInstance *GetConnectedInstance(const unsigned int &key, int index) const;
    void ResetConnections();
    void DisconnectInstances();
    bool IsDerivedFromTemplate(unsigned int templateKey) const;
    bool GetPosition(UMath::Vector3 &position);
    bool GetDirection(UMath::Vector3 &direction);

    bool GetFlag(unsigned int flag) const {
        return (mFlags & flag) != 0;
    }

    template <class T>
    static T *FindObject(unsigned int key);

    static GRuntimeInstance *sRingListHead[kGameplayObjType_Count];

    friend class GObjectIterator<GState>;
    friend class GObjectIterator<GHandler>;
    friend class GObjectIterator<GTrigger>;
    friend class GObjectIterator<GActivity>;
    friend class GManager;
    friend class GActivity;

  protected:
    PackedIndexedKey MakePackedKey(unsigned int key, int index) const;
    void AddToTypeList(GameplayObjType type);
    void RemoveFromTypeList();
    unsigned int GetConnectionCount() const;
    GRuntimeInstance *GetConnectionAt(unsigned int index) const;
    void SetFlag(unsigned int flag) {
        mFlags |= flag;
    }

  protected:
    unsigned short mFlags;                // offset 0x14, size 0x2
    unsigned short mNumConnected;         // offset 0x16, size 0x2
    ConnectedInstance *mConnected;        // offset 0x18, size 0x4
    GRuntimeInstance *mPrev;              // offset 0x1C, size 0x4
    GRuntimeInstance *mNext;              // offset 0x20, size 0x4
};

// total size: 0x8
template <class T> class GObjectIterator {
  public:
    GObjectIterator(unsigned int flagMask);
    void Advance();

    bool IsValid() const {
        return mInstance != NULL;
    }

    T *GetInstance() const {
        return static_cast<T *>(mInstance);
    }

  private:
    GRuntimeInstance *mInstance; // offset 0x0, size 0x4
    unsigned int mFlagMask;      // offset 0x4, size 0x4
};

#endif
