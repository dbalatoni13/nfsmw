#ifndef EAGL4ANIM_DELTACHAN_H
#define EAGL4ANIM_DELTACHAN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "DeltaCompressedData.h"
#include "FnAnimMemoryMap.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0xC
class DeltaChan : public AnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    unsigned short GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumFrames(unsigned short numFrames) {
        mNumFrames = numFrames;
    }

    int GetNumDofs() const {
        return mData->GetNumDofs();
    }

    unsigned short *GetDofIndices() {
        return reinterpret_cast<unsigned short *>(&mNumFrames + 1); // TODO why doesn't &this[1] work here?
    }

    const unsigned short *GetDofIndices() const {
        return reinterpret_cast<const unsigned short *>(&mNumFrames + 1);
    }

    DeltaCompressedData *GetDeltaData() const {
        return mData;
    }

    void SetDeltaData(DeltaCompressedData *data) {
        mData = data;
    }

  private:
    DeltaCompressedData *mData; // offset 0x4, size 0x4
    unsigned short mNumFrames;  // offset 0x8, size 0x2
};

// total size: 0x24
class FnDeltaChan : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnDeltaChan();

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {
        timeLength = static_cast<float>(reinterpret_cast<DeltaChan *>(mpAnim)->GetNumFrames());

        return true;
    }

    // Overrides: FnAnimSuper
    ~FnDeltaChan() override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    void EvalToPrevValues(int frame);

    void EvalToPrevValues(int frame, int numDofPerBone, int, const unsigned short *);

  protected:
    int mPrevFrame;            // offset 0x10, size 0x4
    float *mPrevValues;        // offset 0x14, size 0x4
    int mNumDofs;              // offset 0x18, size 0x4
    unsigned short *mDofMask;  // offset 0x1C, size 0x4
    const BoneMask *mBoneMask; // offset 0x20, size 0x4
};

// total size: 0x24
class FnDeltaLerpChan : public FnDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnDeltaLerpChan() {
        mType = AnimTypeId::ANIM_DELTALERP;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *evalBuffer) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

    // Overrides: FnAnim
    bool EvalWeights(float currTime, float *weights) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currTime, float *vel) override;

  protected:
    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);
};

// total size: 0x24
class FnDeltaQuatChan : public FnDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnDeltaQuatChan() {
        mType = AnimTypeId::ANIM_DELTAQUAT;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *evalBuffer) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

  protected:
    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);
};

// total size: 0x10
class KeyDeltaChan : public AnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    unsigned short GetNumKeys() const {
        return mNumKeys;
    }

    void SetNumKeys(unsigned short numKeys) {
        mNumKeys = numKeys;
    }

    int GetNumDofs() const {
        return mData->GetNumDofs();
    }

    unsigned short *GetDofIndices() {
        return reinterpret_cast<unsigned short *>(&mNumKeys + 1);
    }

    const unsigned short *GetDofIndices() const {
        return reinterpret_cast<const unsigned short *>(&mNumKeys + 1);
    }

    DeltaCompressedData *GetDeltaData() const {
        return mData;
    }

    void SetDeltaData(DeltaCompressedData *data) {
        mData = data;
    }

    unsigned short *GetKeyTimes() const {
        return mKeyTimes;
    }

    void SetKeyTimes(unsigned short *times) {
        mKeyTimes = times;
    }

  private:
    DeltaCompressedData *mData; // offset 0x4, size 0x4
    unsigned short *mKeyTimes;  // offset 0x8, size 0x4
    unsigned short mNumKeys;    // offset 0xC, size 0x2
};

// total size: 0x24
class FnKeyDeltaChan : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnKeyDeltaChan()
        : mPrevKey(-1),         //
          mPrevValues(nullptr), //
          mNumDofs(0),          //
          mDofMask(nullptr),    //
          mBoneMask(nullptr) {}

    // Overrides: FnAnimSuper
    ~FnKeyDeltaChan() override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {
        KeyDeltaChan *keyChan = reinterpret_cast<KeyDeltaChan *>(mpAnim);
        int numKeys = keyChan->GetNumKeys();
        timeLength = static_cast<float>(keyChan->GetKeyTimes()[numKeys - 2] + 1);

        return true;
    }

  protected:
    void EvalToPrevValues(int key);

    void EvalToPrevValues(int key, int numDofPerBone, int, const unsigned short *);

    int FindLowerKey(float currTime);

    int mPrevKey;              // offset 0x10, size 0x4
    float *mPrevValues;        // offset 0x14, size 0x4
    int mNumDofs;              // offset 0x18, size 0x4
    unsigned short *mDofMask;  // offset 0x1C, size 0x4
    const BoneMask *mBoneMask; // offset 0x20, size 0x4
};

// total size: 0x24
class KeyLerpChan : public KeyDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}
};

// total size: 0x24
class FnKeyLerpChan : public FnKeyDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnKeyLerpChan() {
        mType = AnimTypeId::ANIM_KEYLERP;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *evalBuffer) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

  protected:
    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);
};

// total size: 0x24
class FnKeyQuatChan : public FnKeyDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnKeyQuatChan() {
        mType = AnimTypeId::ANIM_KEYQUAT;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *evalBuffer) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

  protected:
    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);
};

// total size: 0x10
class KeyQuatChan : public KeyDeltaChan {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}
};

// huh
inline FnDeltaChan::FnDeltaChan()
    : mPrevFrame(-1),       //
      mBoneMask(nullptr),   //
      mPrevValues(nullptr), //
      mNumDofs(0),          //
      mDofMask(nullptr) {}

}; // namespace EAGL4Anim

#endif
