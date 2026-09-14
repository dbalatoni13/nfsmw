#ifndef PHYSICS_SMOKEABLEINFO_H
#define PHYSICS_SMOKEABLEINFO_H

#include "Speed/Indep/Libs/Support/Utility/UBitArray.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UQueue.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct SceneryModel;

struct SmokeableSection {
    float LastLoadTime;
    int SectionID;
    BitArray< unsigned int, 256 > Rebuilds;

    SmokeableSection()
        : LastLoadTime(0.0f) //
        , SectionID(-1)
        , Rebuilds() {}

    SmokeableSection(int section_id)
        : LastLoadTime(0.0f) //
        , SectionID(section_id)
        , Rebuilds() {}

    SmokeableSection(const SmokeableSection &_ctor_arg)
        : LastLoadTime(_ctor_arg.LastLoadTime) //
        , SectionID(_ctor_arg.SectionID)
        , Rebuilds(_ctor_arg.Rebuilds) {}

    SmokeableSection &operator=(const SmokeableSection &_ctor_arg) {
        LastLoadTime = _ctor_arg.LastLoadTime;
        SectionID = _ctor_arg.SectionID;
        Rebuilds = _ctor_arg.Rebuilds;
        return *this;
    }
};

class SmokeableSectionQ {
  public:
    SmokeableSection *FindOrAdd(int section_id);
    SmokeableSection *Find(int section_id);

    void Reset() {
        mQueue.reset();
    }

  private:
    UCircularQueue< SmokeableSection, 96 > mQueue;
};

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// total size: 0x40
class SmokeableSpawner {
  public:
    static const Attrib::Collection *FindAttributes(UCrc32 name);

    static void Init();

    void EndianSwap();

    void OnUnload();

    const struct ModelHeirarchy *GetRenderHeirarchy() const;

    struct bHash32 GetRenderMesh() const;

    void ShowInstance() const;

    bool IsInstanceVisible() const;

    void HideInstance() const;

    void OnMoved();

    void OnLoad(unsigned int exclude_flags, bool hidden);

    UCrc32 GetModelName() const {
        return mModel;
    }

    UCrc32 GetCollisionName() const {
        return mCollisionName;
    }

    const UMath::Vector4 &GetOrientation() const {
        return mOrientation;
    }

    const UMath::Vector4 &GetPosition() const {
        return mPosition;
    }

    unsigned int GetUniqueID() const {
        return mUniqueID;
    }

    unsigned int GetExcludeFlags() const {
        return mExcludeFlags;
    }

  private:
    UMath::Quaternion mOrientation;    // offset 0x0, size 0x10
    UMath::Vector4 mPosition;          // offset 0x10, size 0x10
    UCrc32 mModel;                     // offset 0x20, size 0x4
    UCrc32 mCollisionName;             // offset 0x24, size 0x4
    UCrc32 mAttributes;                // offset 0x28, size 0x4
    uint32 mSceneryOverrideInfoNumber; // offset 0x2C, size 0x4
    uint32 mUniqueID;                  // offset 0x30, size 0x4
    uint32 mExcludeFlags;              // offset 0x34, size 0x4
    struct SceneryModel *mSimModel;    // offset 0x38, size 0x4
    uint32 pad;                        // offset 0x3C, size 0x4

    friend class SceneryModel;
};

struct SmokeableSpawnerPack : public bTNode< SmokeableSpawnerPack > {
    short ScenerySectionNumber;
    short FirstSmokeableSpawnerID;
    short NumSmokeableSpawners;
    char EndianSwapped;
    char Pad;
    SmokeableSpawner SmokeableSpawners[512];

    void OnLoad(unsigned int exclude_flags);
    void OnUnload();
    void OnMoved();
    void EndianSwap();

    static int Loader(bChunk *chunk);
    static int Unloader(bChunk *chunk);
    static bChunkLoader mLoader;
};

extern SmokeableSectionQ TheSmokeableSections;

void ResetPropTimers();

#endif
