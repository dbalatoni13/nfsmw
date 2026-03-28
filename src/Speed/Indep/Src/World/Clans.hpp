#ifndef WORLD_CLANS_H
#define WORLD_CLANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Skids.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct eView;

extern SlotPool *ClanSlotPool;

// total size: 0x48
class Clan : public bTNode<Clan> {
  public:
    void *operator new(size_t size) {
        return bOMalloc(ClanSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(ClanSlotPool, ptr);
    }

    Clan(bVector3 *position, uint32 hash);
    ~Clan();

    bVector3 *GetBBoxMin() {
        return &BBoxMin;
    }

    bVector3 *GetBBoxMax() {
        return &BBoxMax;
    }

    int GetLastUpdateTime() {
        return LastUpdateTime;
    }

    uint32 GetHash() {
        return Hash;
    }

    void SetLastUpdateTime(int time) {
        LastUpdateTime = time;
    }

    void ExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max) {
        bExpandBoundingBox(&BBoxMin, &BBoxMax, bbox_min, bbox_max);
    }

    bPList<SkidSet> SkidSetList; // offset 0x8, size 0x8

  private:
    uint32 Hash;        // offset 0x10, size 0x4
    int LastUpdateTime; // offset 0x14, size 0x4
    bVector3 Position;  // offset 0x18, size 0x10
    bVector3 BBoxMin;   // offset 0x28, size 0x10
    bVector3 BBoxMax;   // offset 0x38, size 0x10
};

void InitClans();
void FlushClans();
void CloseClans();
Clan *GetClan(bVector3 *position);
void RenderClans(eView *view);

#endif
