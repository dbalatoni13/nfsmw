#ifndef WORLD_CLANS_H
#define WORLD_CLANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct SkidSet;
class eView;

void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *bbox2_min, const bVector3 *bbox2_max);

class Clan : public bTNode<Clan> {
  public:
    // total size: 0x48
    bPList<SkidSet> SkidSetList; // offset 0x8, size 0x8

  private:
    unsigned int Hash;           // offset 0x10, size 0x4
    int LastUpdateTime;          // offset 0x14, size 0x4
    bVector3 Position;           // offset 0x18, size 0x10
    bVector3 BBoxMin;            // offset 0x28, size 0x10
    bVector3 BBoxMax;            // offset 0x38, size 0x10

  public:
    void *operator new(size_t size);
    void operator delete(void *ptr);

    Clan(bVector3 *position, unsigned int hash);
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
    unsigned int GetHash() {
        return Hash;
    }
    void SetLastUpdateTime(int time) {
        LastUpdateTime = time;
    }
    void ExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max) {
        bExpandBoundingBox(&BBoxMin, &BBoxMax, bbox_min, bbox_max);
    }
};

void InitClans();
void FlushClans();
void CloseClans();
Clan *GetClan(bVector3 *position);
void RenderClans(eView *view);

#endif
