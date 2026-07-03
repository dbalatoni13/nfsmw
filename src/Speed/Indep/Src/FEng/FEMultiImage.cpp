#include "Speed/Indep/Src/FEng/FEMultiImage.h"

u32 FEMultiImage::GetTexture(u32 tex_num) {
    if (tex_num >= FEMI_NUM_TEXTURES) {
        return 0xFFFFFFFF;
    }

    return hTexture[tex_num];
}

void FEMultiImage::SetUVs(u32 tex_num, FEVector2 top_left, FEVector2 bottom_right) {
    if (tex_num >= FEMI_NUM_TEXTURES)
        return;
    GetMultiImageData()->TopLeftUV[tex_num].x = top_left.x;
    GetMultiImageData()->TopLeftUV[tex_num].y = top_left.y;
    GetMultiImageData()->BottomRightUV[tex_num].x = bottom_right.x;
    GetMultiImageData()->BottomRightUV[tex_num].y = bottom_right.y;
}

void FEMultiImage::GetUVs(u32 tex_num, FEVector2 &top_left, FEVector2 &bottom_right) {
    if (tex_num >= FEMI_NUM_TEXTURES)
        return;
    top_left.x = GetMultiImageData()->TopLeftUV[tex_num].x;
    top_left.y = GetMultiImageData()->TopLeftUV[tex_num].y;
    bottom_right.x = GetMultiImageData()->BottomRightUV[tex_num].x;
    bottom_right.y = GetMultiImageData()->BottomRightUV[tex_num].y;
}
