#ifndef WORLD_WORLD_MODEL_H
#define WORLD_WORLD_MODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpaceNode.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <cstring>

extern SlotPool *SpaceNodeSlotPool;
extern SlotPool *WorldModelSlotPool;

// total size: 0x88

class WorldModel : public bTNode<WorldModel> {
  public:
    typedef unsigned int NodeVisibility;

    WorldModel(unsigned int name_hash, bMatrix4 *matrix, bool add_lighting);

    WorldModel(unsigned int *lod_name_hash, bMatrix4 *matrix, bool add_lighting);

    WorldModel(SpaceNode *spacenode, unsigned int name_hash, bool add_lighting);

    WorldModel(SpaceNode *spacenode, unsigned int *lod_name_hash, bool add_lighting);

    WorldModel(const struct ModelHeirarchy *heirarchy, unsigned int heirarchy_index, bool add_lighting);

    void Construct(SpaceNode *spacenode, bMatrix4 *matrix, const ModelHeirarchy *heirarchy, unsigned int rootnode, bool add_lighting);

    ~WorldModel();

    void *operator new(std::size_t size) {
        return bOMalloc(WorldModelSlotPool);
    }

    bool IsEnabled() {
        return this->mEnabled;
    }

    void SetEnabledFlag(bool f) {
        this->mEnabled = f;
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

    void SetChildVisibility(unsigned int state) {
        this->mChildVisibility = state;
    }

    void SetMatrix(bMatrix4 *matrix) {
        if (matrix != nullptr) {
            this->mEnabled = true;
            if (this->pSpaceNode) {
                this->pSpaceNode->SetLocalMatrix(matrix);
            } else {
                this->mMatrix = *matrix;
            }
        } else {
            this->mEnabled = false;
        }
    }

    eModel *GetModel();

    void AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures);

    unsigned int GetNameHash();

    const char *GetName();

    static int DoesArtExist(unsigned int name_hash);

    void GetLocalBoundingBox(bVector3 *min_ext, bVector3 *max_ext);

    void RenderNode(const ModelHeirarchy *heirarchy, unsigned int nodeIndex, eView *view, int exc_flag, bMatrix4 *blended_matrices,
                    const bMatrix4 *matrix);

    void RenderModel(eModel *render_model, eView *view, int exc_flag, bMatrix4 *blended_matrices, const bMatrix4 *matrix);

    void Render(eView *view, int exc_flag);

  private:
    eModel *pModel;                   // offset 0x8, size 0x4
    eModel *pReflectionModel;         // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy; // offset 0x10, size 0x4
    unsigned int mHeirarchyIndex;     // offset 0x14, size 0x4
    unsigned int mChildVisibility;    // offset 0x18, size 0x4
    unsigned int mLastVisibleFrame;   // offset 0x1C, size 0x4
    unsigned int mLastRenderFrame;    // offset 0x20, size 0x4
    float mDistanceToGameView;        // offset 0x24, size 0x4
  public:
    bool mEnabled; // offset 0x28, size 0x1 TODO
  private:
    bool mRenderInSplitScreen;           // offset 0x2C, size 0x1
    bool mInvisibleInside;               // offset 0x30, size 0x1
    bool mAddLighting;                   // offset 0x34, size 0x1
    unsigned char mDamageLevel;          // offset 0x38, size 0x1
    unsigned char mCastsShadow;          // offset 0x39, size 0x1
    SpaceNode *pSpaceNode;               // offset 0x3C, size 0x4
    bMatrix4 mMatrix;                    // offset 0x40, size 0x40
    eLightMaterial *mLightMaterial;      // offset 0x80, size 0x4
    unsigned int mLightMaterialSkinHash; // offset 0x84, size 0x4
};

void InitWorldModels();

#endif
