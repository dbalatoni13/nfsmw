#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

extern int iRam8047ff04;
extern int FacePixelation_mPixelationOn asm("_14FacePixelation.mPixelationOn");
extern float FacePixelation_mWidth asm("_14FacePixelation.mWidth");
extern float FacePixelation_mHeight asm("_14FacePixelation.mHeight");
extern bVector3 FacePixelation_mWorldPos asm("_14FacePixelation.mWorldPos");
void eViewPlatInterface_GetScreenPosition(eViewPlatInterface *view, bVector3 *screen_position, const bVector3 *world_position)
    asm("GetScreenPosition__18eViewPlatInterfaceP8bVector3PC8bVector3");

FacePixelation::FacePixelation(eView *view) {
    MyView = view;
    mScreenX = 0.0f;
    mScreenY = 0.0f;
}

void FacePixelation::SetLocation(bVector3 &worldPos) {
    FacePixelation_mWorldPos = worldPos;
}

void FacePixelation::GetData(float *x, float *y, float *width, float *height) {
    *x = mScreenX;
    *y = mScreenY;
    *width = FacePixelation_mWidth;
    *height = FacePixelation_mHeight;
}

void FacePixelation::Render() {
    if (iRam8047ff04 == 6 && FacePixelation_mPixelationOn != 0) {
        bVector3 screen_position;

        eViewPlatInterface_GetScreenPosition(MyView, &screen_position, &FacePixelation_mWorldPos);
        mScreenX = screen_position.x;
        mScreenY = screen_position.y;
    }
}
