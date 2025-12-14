#ifndef ANIMATION_ANIMWORLDSCENE_H
#define ANIMATION_ANIMWORLDSCENE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimWorldTypes.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x18
class CAnimWorldScene : public bTNode<CAnimWorldScene> {
  private:
    int mHandle;                                         // offset 0x8, size 0x4
    bTList<CWorldAnimEntityTree> mInstancedAnimTreeList; // offset 0xC, size 0x8
};

#endif
