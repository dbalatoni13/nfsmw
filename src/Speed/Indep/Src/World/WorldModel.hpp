#ifndef WORLDMODEL_HPP
#define WORLDMODEL_HPP

#include "SpaceNode.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *WorldModelSlotPool;

// total size: 0x88

class WorldModel : public bTNode<WorldModel> {
  public:
    typedef unsigned int NodeVisibility;

    WorldModel(uint32 name_hash, bMatrix4 *matrix, bool add_lighting);

    WorldModel(unsigned int *lod_name_hash, bMatrix4 *matrix, bool add_lighting);

    WorldModel(SpaceNode *spacenode, uint32 name_hash, bool add_lighting);

    WorldModel(SpaceNode *spacenode, unsigned int *lod_name_hash, bool add_lighting);

    WorldModel(const ModelHeirarchy *heirarchy, uint32 heirarchy_index, bool add_lighting);

    ~WorldModel();

    USE_SLOTALLOC(WorldModelSlotPool);

    void SetMatrix(bMatrix4 *matrix) {
        if (matrix != nullptr) {
            this->mEnabled = true;
            if (this->pSpaceNode != nullptr) {
                this->pSpaceNode->SetLocalMatrix(matrix);
            } else {
                this->mMatrix = *matrix;
            }
        } else {
            this->mEnabled = false;
        }
    }

    bMatrix4 *GetMatrix() {
        if (this->pSpaceNode != nullptr) {
            return this->pSpaceNode->GetLocalMatrix();
        } else {
            return &this->mMatrix;
        }
    }

    bVector3 *GetPosition() {
        if (this->pSpaceNode != nullptr) {
            return reinterpret_cast<bVector3 *>(&this->pSpaceNode->GetLocalMatrix()->v3);
        } else {
            return reinterpret_cast<bVector3 *>(&this->mMatrix.v3);
        }
    }

    bool IsEnabled() {
        return this->mEnabled;
    }

    void SetEnabledFlag(bool f) {
        this->mEnabled = f;
    }

    bool CanRenderInSplitScreen() {
        return this->mRenderInSplitScreen;
    }

    bool InvisibleInside() {
        return this->mInvisibleInside;
    }

    float DistanceToGameView() const {
        return this->mDistanceToGameView;
    }

    unsigned int GetLastVisibleFrame() const {
        return this->mLastVisibleFrame;
    }

    unsigned int GetLastRenderFrame() const {
        return this->mLastRenderFrame;
    }

    void SetChildVisibility(uint32 state) {
        this->mChildVisibility = state;
    }

    eModel *GetModel();

    void AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures);

    void AttachLightMaterial(eLightMaterial *lm, uint32 toskin) {
        this->mLightMaterial = lm;
        this->mLightMaterialSkinHash = toskin;
    }

    uint32 GetNameHash();

    const char *GetName();

    static int DoesArtExist(uint32 name_hash);

    void GetLocalBoundingBox(bVector3 *min_ext, bVector3 *max_ext);

    void RenderNode(const ModelHeirarchy *heirarchy, unsigned int nodeIndex, eView *view, int exc_flag, bMatrix4 *blended_matrices,
                    const bMatrix4 *matrix);

    void RenderModel(eModel *render_model, eView *view, int exc_flag, bMatrix4 *blended_matrices, const bMatrix4 *matrix);

    void Render(eView *view, int exc_flag);

  private:
    const ModelHeirarchy *GetHeirarchy() const {
        return this->mHeirarchy;
    }

    void Construct(SpaceNode *spacenode, bMatrix4 *matrix, const ModelHeirarchy *heirarchy, uint32 rootnode, bool add_lighting);

    eModel *pModel;                   // offset 0x8, size 0x4
    eModel *pReflectionModel;         // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy; // offset 0x10, size 0x4
    unsigned int mHeirarchyIndex;     // offset 0x14, size 0x4
    uint32 mChildVisibility;          // offset 0x18, size 0x4
    uint32 mLastVisibleFrame;         // offset 0x1C, size 0x4
    uint32 mLastRenderFrame;          // offset 0x20, size 0x4
    float mDistanceToGameView;        // offset 0x24, size 0x4
  public:
    bool mEnabled; // offset 0x28, size 0x1 TODO
  private:
    bool mRenderInSplitScreen; // offset 0x2C, size 0x1
    bool mInvisibleInside;     // offset 0x30, size 0x1
    bool mAddLighting;         // offset 0x34, size 0x1
    uint8 mDamageLevel;        // offset 0x38, size 0x1
#ifndef EA_BUILD_A124
    unsigned char mCastsShadow; // offset 0x39, size 0x1
#endif
    SpaceNode *pSpaceNode;          // offset 0x3C, size 0x4
    bMatrix4 mMatrix;               // offset 0x40, size 0x40
    eLightMaterial *mLightMaterial; // offset 0x80, size 0x4
    uint32 mLightMaterialSkinHash;  // offset 0x84, size 0x4
};

void InitWorldModels();

#endif
