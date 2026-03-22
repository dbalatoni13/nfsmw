#ifndef FENG_FESCRIPT_H
#define FENG_FESCRIPT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"
#include "FEEvent.h"
#include "FEKeyTrack.h"
#include "FEList.h"

template <class T, int N> struct ObjectPool;

// total size: 0x34
class FEScript : public FEMinNode {
  public:
    int Length;               // offset 0xC, size 0x4
    int CurTime;              // offset 0x10, size 0x4
    unsigned long Flags;      // offset 0x14, size 0x4
    FEScript *pChainTo;       // offset 0x18, size 0x4
    unsigned long TrackCount; // offset 0x1C, size 0x4
    FEKeyTrack *pTracks;      // offset 0x20, size 0x4
    FEEventList Events;       // offset 0x24, size 0x8
    char *pName;              // offset 0x2C, size 0x4
    unsigned long ID;         // offset 0x30, size 0x4

    inline const char* GetName() const { return pName; }
    inline FEScript* GetNext() const { return static_cast<FEScript*>(FEMinNode::GetNext()); }
    inline FEScript* GetPrev() const { return static_cast<FEScript*>(FEMinNode::GetPrev()); }

    inline FEScript() { Init(); }
    static void* operator new(unsigned int);
    static void operator delete(void* pNode);

    void Init();
    ~FEScript() override;
    FEScript(FEScript& Src, bool bReference);
    void SetTrackCount(long Count);
    FEKeyTrack* FindTrack(FEKeyTrack_Indices TrackIndex) const;
    void SetName(const char* pNewName);

    static ObjectPool<FEScript, 32> NodePool;
};

#endif
