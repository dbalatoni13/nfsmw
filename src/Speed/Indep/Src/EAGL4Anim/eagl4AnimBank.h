#ifndef EAGL4ANIM_EAGL4ANIMBANK_H
#define EAGL4ANIM_EAGL4ANIMBANK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AttributeDictionary.h"
#include "Csis.h"
#include "FnAnim.h"
#include "FnAnimMemoryMap.h"
#include "eagl4supportdlopen.h"
#include "eagl4supportprintmessage.h"

namespace EAGL4Anim {

class BankStat;

// total size: 0x1C
class AnimBank {
  public:
    int GetSize() const {
        return mSize;
    }

    int GetNumAnims() const {
        return mNumAnims;
    }

    // const AttributeDictionary *GetAttributeDictionary() const {}

    // CsisDictionary *GetCsisDictionary() const {}

    void SetBankStat(BankStat *bankStat) {}

    AnimMemoryMap *GetAnim(int i) const {
        return mAnims[i];
    }

    // const char *GetAnimName(int i) const {}

    // AnimMemoryMap *GetAnim(const char *name) {}

    // static FnAnim *NewFnAnimByType(AnimTypeId::Type type) {}

    static void DeleteFnAnim(FnAnim *fnAnim) {}

    static void Constructor(void *ptr, EAGL4::DynamicLoader *loader, const char *pSymbolName);

    static void Destructor(void *ptr);

    int GetAnimIndex(const char *name);

    const AttributeBlock *GetAnimAttributes(int i) const;

    FnAnimMemoryMap *NewFnAnim(int i) const;

    static FnAnimMemoryMap *NewFnAnim(AnimMemoryMap *animMem);

  private:
    int mSize;                                 // offset 0x0, size 0x4
    int mNumAnims;                             // offset 0x4, size 0x4
    AttributeDictionary *mAttributeDictionary; // offset 0x8, size 0x4
    CsisDictionary *mCsisDictionary;           // offset 0xC, size 0x4
    AnimMemoryMap **mAnims;                    // offset 0x10, size 0x4
    const char **mNames;                       // offset 0x14, size 0x4
    BankStat *mStat;                           // offset 0x18, size 0x4
};

// total size: 0x18
class BankStat {
  public:
    BankStat() {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void IncrementLoads(AnimBank *ab) {}

    void IncrementAnimNameLookUps() {}

    bool InMemory() {}

    void UnLoadBank() {}

    ~BankStat();

    void Init(AnimBank *ab, const char *name);

    bool IsTracking(AnimBank *ab, const char *name) const;

    AnimStat *GetAnimStat(AnimMemoryMap *a);

    void Dump() const;

    void Dump(PRINTCHANNEL channel) const;

    void Reset();

  private:
    char *mBankFileName;     // offset 0x0, size 0x4
    int mNumLoads;           // offset 0x4, size 0x4
    int mNumAnims;           // offset 0x8, size 0x4
    int mNumAnimNameLookUps; // offset 0xC, size 0x4
    AnimStat *mAnimStats;    // offset 0x10, size 0x4
    AnimBank *mBank;         // offset 0x14, size 0x4
};

}; // namespace EAGL4Anim

#endif
