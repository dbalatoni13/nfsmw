#ifndef WORLD_WORLD_MODEL_H
#define WORLD_WORLD_MODEL_H

#include "dolphin/types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpaceNode.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <cstring>

extern SlotPool *SpaceNodeSlotPool;
extern SlotPool *WorldModelSlotPool;

class WorldModel : public bTNode<WorldModel> {
  public:
    WorldModel(unsigned int name_hash, bMatrix4 *matrix, bool add_lightning);
    WorldModel(const struct ModelHeirarchy *heirarchy, unsigned int heirarchy_index, bool add_lightning);
    ~WorldModel();

    void *operator new(std::size_t size) {
        return bOMalloc(WorldModelSlotPool);
    }

    bool IsEnabled() {
        return this->mEnabled;
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
                PSMTX44Copy(
                    *reinterpret_cast<const Mtx44 *>(matrix),
                    *reinterpret_cast<Mtx44 *>(&this->mMatrix)
                );
            }
        } else {
            this->mEnabled = false;
        }
    }

private:
    // total size: 0x88
    eModel *pModel;                          // offset 0x8, size 0x4
    eModel *pReflectionModel;                // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy;        // offset 0x10, size 0x4
    unsigned int mHeirarchyIndex;            // offset 0x14, size 0x4
    unsigned int mChildVisibility;           // offset 0x18, size 0x4
    unsigned int mLastVisibleFrame;          // offset 0x1C, size 0x4
    unsigned int mLastRenderFrame;           // offset 0x20, size 0x4
    float mDistanceToGameView;               // offset 0x24, size 0x4
public:
    bool mEnabled;                           // offset 0x28, size 0x1
private:
    bool mRenderInSplitScreen;               // offset 0x2C, size 0x1
    bool mInvisibleInside;                   // offset 0x30, size 0x1
    bool mAddLighting;                       // offset 0x34, size 0x1
    unsigned char mDamageLevel;              // offset 0x38, size 0x1
    unsigned char mCastsShadow;              // offset 0x39, size 0x1
    SpaceNode *pSpaceNode;            // offset 0x3C, size 0x4
    bMatrix4 mMatrix;                        // offset 0x40, size 0x40
    eLightMaterial *mLightMaterial;          // offset 0x80, size 0x4
    unsigned int mLightMaterialSkinHash;     // offset 0x84, size 0x4
};

#endif
