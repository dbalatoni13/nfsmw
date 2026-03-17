#ifndef WORLD_CLANS_H
#define WORLD_CLANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct SkidSet;
struct eView;

struct Clan : public bTNode<Clan> {
    // total size: 0x48
    bPList<SkidSet> SkidSetList; // offset 0x8, size 0x8
    unsigned int Hash;           // offset 0x10, size 0x4
    int LastUpdateTime;          // offset 0x14, size 0x4
    bVector3 Position;           // offset 0x18, size 0x10
    bVector3 BBoxMin;            // offset 0x28, size 0x10
    bVector3 BBoxMax;            // offset 0x38, size 0x10
};

void InitClans();
void FlushClans();
void CloseClans();
Clan *GetClan(bVector3 *position);
void RenderClans(eView *view);

#endif
