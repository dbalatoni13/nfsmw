#ifndef PHYSICS_SMOKEABLEINFO_H
#define PHYSICS_SMOKEABLEINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

    SmokeableSection() {}

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
    UMath::Quaternion mOrientation;
    UMath::Vector4 mPosition;
    UCrc32 mModel;
    UCrc32 mCollisionName;
    UCrc32 mAttributes;
    uint32 mSceneryOverrideInfoNumber;
    uint32 mUniqueID;
    uint32 mExcludeFlags;
    SceneryModel *mSimModel;
    uint32 pad;

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

#endif
