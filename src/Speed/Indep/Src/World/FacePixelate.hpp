#ifndef FACEPIXELATE_HPP
#define FACEPIXELATE_HPP

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

// total size: 0xC
class FacePixelation {
  public:
    FacePixelation(eView *view);
    ~FacePixelation();
    void GetData(float *x, float *y, float *width, float *height);

    bool IsEnabled() {
        return mPixelationOn;
    }

    void Update(float x, float y) {
        mScreenX = x;
        mScreenY = y;
    }

    void Render();
    static void SetLocation(bVector3 &worldPos);
    static void SetDimensions(float width, float height) {
        mWidth = width;
        mHeight = height;
    }

    static void Enable() {
        mPixelationOn = true;
    }

    static void Disable() {
        mPixelationOn = false;
    }

  private:
    eView *MyView;  // offset 0x0, size 0x4
    float mScreenX; // offset 0x4, size 0x4
    float mScreenY; // offset 0x8, size 0x4

    static bool mPixelationOn; // size: 0x1, address: 0x80438AB0

    static float mWidth;  // size: 0x4, address: 0x80438AB4
    static float mHeight; // size: 0x4, address: 0x80438AB8

    static bVector3 mWorldPos; // size: 0x10, address: 0x804A7E04
};

#endif
