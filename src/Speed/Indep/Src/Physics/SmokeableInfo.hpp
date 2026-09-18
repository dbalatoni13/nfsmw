#ifndef PHYSICS_SMOKEABLEINFO_H
#define PHYSICS_SMOKEABLEINFO_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UQueue.h"
#include "Speed/Indep/Libs/Support/Utility/UBitArray.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

class HeirarchyModel;
class SceneryModel;

// total size: 0x28
class SmokeableSection {
  public:
    SmokeableSection()
        : LastLoadTime(0.0f), SectionID(-1) {}

    SmokeableSection(int section_id)
        : LastLoadTime(0.0f), SectionID(section_id) {}

    float LastLoadTime;                  // offset 0x0, size 0x4
    int SectionID;                       // offset 0x4, size 0x4
    BitArray<unsigned int, 256> Rebuilds; // offset 0x8, size 0x20
};

// total size: 0xF10
class SmokeableSectionQ {
  public:
    SmokeableSection *FindOrAdd(int section_id);
    SmokeableSection *Find(int section_id);

    void Reset() {
        mQueue.reset();
    }

  private:
    UCircularQueue<SmokeableSection, 96> mQueue; // offset 0x0, size 0xF10
};

extern SmokeableSectionQ TheSmokeableSections;

void ResetPropTimers();

// total size: 0x18
class SmackableAvoidable : public AIAvoidable {
  public:
    USE_FASTALLOC(SmackableAvoidable);

    SmackableAvoidable(HeirarchyModel *model);

    bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) override;

    void SetRefrence(UTL::COM::IUnknown *pUnk) {
        SetAvoidableObject(pUnk);
    }

  private:
    HeirarchyModel *mModel; // offset 0x14, size 0x4
};

// total size: 0xEC
class HeirarchyModel : public Sim::Model, public IBody, public ITriggerableModel, public Attrib::Gen::smackable {
  public:
    enum Flags {
        F_CAMERA_AVOIDABLE = 1,
    };

    USE_FASTALLOC(HeirarchyModel);

    HeirarchyModel(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry, UCrc32 rendernode, HeirarchyModel *parent,
                   const Attrib::Collection *attribs, const ModelHeirarchy *heirarchy, unsigned int heirarchynode, bool visible);

    ~HeirarchyModel() override;

    void SetCameraAvoidable(bool b);
    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep);
    virtual bool OnRemoveOffScreen(float dT);
    void OnProcessFrame(float dT) override;
    void HidePart(const UCrc32 &nodename) override;
    void ShowPart(const UCrc32 &nodename) override;
    bool IsPartVisible(const UCrc32 &nodename) const override;
    int FindHeirarchyChild(const UCrc32 &nodename) const;
    IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) override;
    void OnBeginDraw() override;
    void OnEndDraw() override;
    void GetTransform(UMath::Matrix4 &matrix) const override;
    void GetAngularVelocity(UMath::Vector3 &velocity) const override;
    void RemoveTrigger();
    void DisableTrigger();
    void SetTrigger(const UMath::Matrix4 &matrix, bool virgin);
    void PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) override;
    void OnEndSimulation() override;
    void OnBeginSimulation() override;
    bool OnDraw(Sim::Packet *service) override;

    // IBody
    void GetDimension(UMath::Vector3 &dim) const override { GetCollisionGeometry()->GetHalfDimensions(dim); }
    const Attrib::Instance &GetAttributes() const override { return *this; }
    unsigned int GetWorldID() const override { return Sim::Model::GetWorldID(); }
    void GetLinearVelocity(UMath::Vector3 &velocity) const override { Sim::Model::GetLinearVelocity(velocity); }

  protected:
    void SetAvoidable(bool b) {
        bool avoidable = mAvoidable != NULL;
        if (avoidable == b) {
            return;
        }

        if (b) {
            mAvoidable = new SmackableAvoidable(this);
        } else {
            delete mAvoidable;
            mAvoidable = NULL;
        }
    }

    SmackableTrigger *GetTrigger() {
        return mTrigger;
    }

  private:
    UMath::Vector4 mTriggerAvoid;          // offset 0xC0, size 0x10
    const ModelHeirarchy *mHeirarchy;      // offset 0xD0, size 0x4
    bHash32 mRenderMesh;                   // offset 0xD4, size 0x4
    SmackableTrigger *mTrigger;            // offset 0xD8, size 0x4
    float mOffScreenTimer;                 // offset 0xDC, size 0x4
    unsigned short mHeirarchyNode;         // offset 0xE0, size 0x2
    unsigned short mFlags;                 // offset 0xE2, size 0x2
    unsigned int mChildVisibility;         // offset 0xE4, size 0x4
    SmackableAvoidable *mAvoidable;        // offset 0xE8, size 0x4
};


// total size: 0xF8
class PlaceableScenery : public HeirarchyModel, public IPlaceableScenery {
  public:
    PlaceableScenery(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry, const Attrib::Collection *attribs,
                     const ModelHeirarchy *heirarchy);


    static PlaceableScenery *Construct(const char *name, unsigned int attributes);
    void ReleaseModel() override;
    void PickUp() override;
    bool Place(const UMath::Matrix4 &transform, bool snap_to_ground) override;
    void Destroy() override {
        Sim::Model::ReleaseModel();
    }
    bool OnRemoveOffScreen(float dT) override {
        return false;
    }

  private:
};

// total size: 0x40
class SmokeableSpawner {
  public:
    static const Attrib::Collection *FindAttributes(UCrc32 name);

    static void Init();

    void EndianSwap();

    void OnUnload();

    const ModelHeirarchy *GetRenderHeirarchy() const;

    bHash32 GetRenderMesh() const;

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

    void SetSimModel(struct SceneryModel *model) {
        mSimModel = model;
    }

  private:
    UMath::Quaternion mOrientation;       // offset 0x0, size 0x10
    UMath::Vector4 mPosition;             // offset 0x10, size 0x10
    UCrc32 mModel;                        // offset 0x20, size 0x4
    UCrc32 mCollisionName;                // offset 0x24, size 0x4
    UCrc32 mAttributes;                   // offset 0x28, size 0x4
    uint32 mSceneryOverrideInfoNumber;    // offset 0x2C, size 0x4
    uint32 mUniqueID;                     // offset 0x30, size 0x4
    uint32 mExcludeFlags;                 // offset 0x34, size 0x4
    struct SceneryModel *mSimModel;       // offset 0x38, size 0x4
    uint32 pad;                           // offset 0x3C, size 0x4
};

// total size: 0x8010
class SmokeableSpawnerPack : public bTNode<SmokeableSpawnerPack> {
  public:
    int GetMemoryImageSize() {
        return sizeof(SmokeableSpawnerPack);
    }

    static int Loader(bChunk *chunk);
    static int Unloader(bChunk *chunk);

    void OnUnload();
    void EndianSwap();
    void OnMoved();
    void OnLoad(unsigned int exclude_flags);

    static bChunkLoader mLoader;

    short ScenerySectionNumber;       // offset 0x8, size 0x2
    short FirstSmokeableSpawnerID;    // offset 0xA, size 0x2
    short NumSmokeableSpawners;       // offset 0xC, size 0x2
    char EndianSwapped;               // offset 0xE, size 0x1
    char Pad;                         // offset 0xF, size 0x1
    SmokeableSpawner SmokeableSpawners[512]; // offset 0x10, size 0x8000
};

// total size: 0xFC
class SceneryModel : public HeirarchyModel, public ISceneryModel {
  public:
    USE_FASTALLOC(SceneryModel);

    static int SceneryCount() {
        return mSceneryCount;
    }

    bool IsHidden() const override {
        bool visible = mInstanceVisible || !Sim::Model::IsHidden();
        return !visible;
    }

    bool IsExcluded(unsigned int scenery_exclusion_flag) const override {
        unsigned int flags = mSpawner ? mSpawner->GetExcludeFlags() : 0;
        return (flags & scenery_exclusion_flag) == 0 ? false : true;
    }

    unsigned int GetSpawnerID() const override {
        return mSpawner ? mSpawner->GetUniqueID() : -1;
    }

    SceneryModel(SmokeableSpawner *spawner, const CollisionGeometry::Bounds *geometry, const Attrib::Collection *attribs, bool hidden);

    ~SceneryModel() override;

    void ShowInstance(bool show);
    void OnBeginDraw() override;
    void HideModel() override;
    void EndOverride();
    void StartOverride();
    void GetTransform(UMath::Matrix4 &matrix) const override;
    void InitScene();
    bool GetSceneryTransform(UMath::Matrix4 &matrix) const override;
    void OnEndSimulation() override;
    static SceneryModel *Construct(SmokeableSpawner *data, const Attrib::Collection *attributes, bool hidden);
    void WakeUp() override;
    void RestoreScene() override;
    void ReleaseModel() override;
    static void InitSystem();
    static void RestoreSystem();

    static int mSceneryCount;

    void SetSpawner(SmokeableSpawner *spawner) {
        mSpawner = spawner;
    }

  private:
    bool mInstanceVisible;     // offset 0xF4, size 0x1
    SmokeableSpawner *mSpawner; // offset 0xF8, size 0x4
};

void ResetPropTimers();

#endif
