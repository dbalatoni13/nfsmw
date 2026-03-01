#ifndef EAGL4ANIM_RAWSTATECHAN_H
#define EAGL4ANIM_RAWSTATECHAN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "FnAnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0xC
class RawStateChan : public AnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void SetNumFrames(unsigned short n) {}

    unsigned short GetNumFrames() const {}

    void SetNumKeys(unsigned short n) {}

    unsigned short GetNumKeys() const {}

    void SetKeySize(unsigned char n) {}

    unsigned char GetKeySize() const {}

    void SetNumFields(unsigned char n) {}

    unsigned char GetNumFields() const {}

    const unsigned short *GetDecodeData() const {}

    unsigned short *GetDecodeData() {}

    int GetSize() const {}

    static int ComputeSize(int numKeys, int numFields, int keySize) {}

    const unsigned char *GetKeyData(int keyIdx) const {}

    unsigned char *GetKeyData(int keyIdx) {}

    void GetDecodeData(int idx, unsigned char &storedNumBitsInPowersOf2, unsigned char &destNumBytes, unsigned char &destByteOffset) const {}

    void SetDecodeData(int idx, unsigned char storedNumBitsInPowersOf2, unsigned char destNumBytes, unsigned char destByteOffset) {}

  private:
    unsigned short mNumFrames;     // offset 0x4, size 0x2
    unsigned short mNumKeys;       // offset 0x6, size 0x2
    unsigned char mNumFields;      // offset 0x8, size 0x1
    unsigned char mKeySize;        // offset 0x9, size 0x1
    unsigned short mDecodeData[1]; // offset 0xA, size 0x2
};

// total size: 0x14
class FnRawStateChan : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnRawStateChan() : mKeyIdx(0) {
        mType = AnimTypeId::ANIM_RAWSTATE;
    }

    // Overrides: FnAnimSuper
    ~FnRawStateChan() override {}

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {}

    // Overrides: FnAnim
    void Eval(float, float time, float *dofs) override {}

    void Decode(unsigned char *src, unsigned char *dest) const;

    // Overrides: FnAnim
    bool EvalState(float time, State *s) override;

    // Overrides: FnAnim
    bool FindTime(const StateTest &test, float startTime, float &resultTime) override;

    // Overrides: FnAnim
    inline void Eval(float, float time, float *dofs, float) override {}

  private:
    int mKeyIdx; // offset 0x10, size 0x4
};

}; // namespace EAGL4Anim

#endif
