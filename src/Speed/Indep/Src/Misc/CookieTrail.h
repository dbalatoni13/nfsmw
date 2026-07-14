#ifndef MISC_COOKIE_TRAIL_H
#define MISC_COOKIE_TRAIL_H

#include <cstddef>

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x810
template <typename T, int U> class CookieTrail {
  public:
    CookieTrail() : mCount(0), mLast(-1), mCapacity(U) {}

    int Capacity() {
        return mCapacity;
    }

    T &Oldest() {
        if (mCount < mCapacity) {
            return mData[0];
        }
        int next = mLast + 1;
        return mData[next - (next / mCapacity) * mCapacity];
    }

    const T &Newest() const {
        return mData[mLast];
    }

    T &Newest() {
        return mData[mLast];
    }

    T &NthOldest(int n) {
        return mData[mCount < mCapacity ? (n % mCount) : ((mLast + 1 + n) % mCapacity)];
    }

    int Count() const {
        return mCount;
    }

    void Clear() {
        mCount = 0;
        mLast = -1;
    }

    void AddNew(const T &t) {
        mLast = (mLast + 1) % mCapacity;
        if (mCount < mCapacity) {
            mCount++;
        }
        mData[mLast] = t;
    }

  private:
    int mCount;          // offset 0x0, size 0x4
    int mLast;           // offset 0x4, size 0x4
    const int mCapacity; // offset 0x8, size 0x4
    int pad;             // offset 0xC, size 0x4
    T mData[U];          // offset 0x10, size 0x800
};

// TODO move?
// total size: 0x40
struct NavCookie {
    UMath::Vector2 Left;               // offset 0x0, size 0x8
    UMath::Vector2 Right;              // offset 0x8, size 0x8
    UMath::Vector2 Forward;            // offset 0x10, size 0x8
    float Length;                      // offset 0x18, size 0x4
    float Curvature;                   // offset 0x1C, size 0x4
    float LeftOffset;                  // offset 0x20, size 0x4
    float RightOffset;                 // offset 0x24, size 0x4
    unsigned int Flags;                // offset 0x28, size 0x4
    float Padding;                     // offset 0x2C, size 0x4
    UMath::Vector3 Centre;             // offset 0x30, size 0xC
    short SegmentParameter;            // offset 0x3C, size 0x2
    unsigned short SegmentNumber : 15; // offset 0x3E, size 0x2
    unsigned short SegmentNodeInd : 1; // offset 0x3E, size 0x2

    void SetSegmentParameter(float t) {
        SegmentParameter = static_cast<short>(bClamp(t, 0.0f, 1.0f) * 65535.0f);
    }

    float GetSegmentParameter() const {
        const float recip = 1.0f / 65535.0f;
        return static_cast<float>(SegmentParameter) * recip;
    }
};

#endif
