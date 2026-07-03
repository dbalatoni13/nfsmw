#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "FacePixelate.hpp"

bool FacePixelation::mPixelationOn = false;

float FacePixelation::mWidth = 16.0f;
float FacePixelation::mHeight = 16.0f;

bVector3 FacePixelation::mWorldPos;

void FacePixelation::SetLocation(bVector3 &worldPos) {
    FacePixelation::mWorldPos = worldPos;
}

void FacePixelation::GetData(float *x, float *y, float *width, float *height) {
    *x = this->mScreenX;
    *y = this->mScreenY;
    *width = FacePixelation::mWidth;
    *height = FacePixelation::mHeight;
}

FacePixelation::FacePixelation(eView *view) {
    this->MyView = view;
    this->mScreenX = 0.0f;
    this->mScreenY = 0.0f;
}

void FacePixelation::Render() {
    if (!IsGameFlowInGame() || !FacePixelation::mPixelationOn) {
        return;
    }
    bVector3 FaceScreenPosition;

    this->MyView->GetScreenPosition(&FaceScreenPosition, &FacePixelation::mWorldPos);
    this->Update(FaceScreenPosition.x, FaceScreenPosition.y);
}
