#ifndef PHYSICS_SMOKEABLEINFO_H
#define PHYSICS_SMOKEABLEINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

    // UCrc32 GetModelName() const {}

    // UCrc32 GetCollisionName() const {}

    // const UMath::Vector4 &GetOrientation() const {}

    // const UMath::Vector4 &GetPosition() const {}

    // unsigned int GetUniqueID() const {}

    // unsigned int GetExcludeFlags() const {}

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
};

#endif
