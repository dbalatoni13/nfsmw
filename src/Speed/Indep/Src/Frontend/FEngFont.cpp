#include "Speed/Indep/Src/Frontend/FEngFont.hpp"

TextureInfo *FixupTextureInfoNull(TextureInfo *info, unsigned int hash, TexturePack *pack, bool loading);

float FEngFont::GetHeight() {
    return Height;
}

unsigned short FEngFont::ConvertCharacter(unsigned short c) {
    if (c > 0xFF7F) {
        c = c & 0xFF;
    }
    if (c == 0x99) {
        return 0x2122;
    }
    if (c == 0x9C) {
        return 0x153;
    }
    if (c != 0xA0) {
        return c;
    }
    return 0x20;
}

float FEngFont::CalculateXOffset(unsigned int ulJustification, float fTextWidth) {
    if (ulJustification & 1) {
        return fTextWidth * -0.5f;
    }
    if (ulJustification & 2) {
        return -fTextWidth;
    }
    return 0.0f;
}

float FEngFont::CalculateYOffset(unsigned int ulJustification, float fTextHeight) {
    if (ulJustification & 4) {
        return fTextHeight * -0.5f;
    }
    if (ulJustification & 8) {
        return -fTextHeight;
    }
    return 0.0f;
}

void FEngFont::NotifyTextureLoading(TexturePack *texture_pack, bool loading) {
    TextureInfo *info = FixupTextureInfoNull(pTextureInfo, TextureHash, texture_pack, loading);
    if (info != pTextureInfo) {
        pTextureInfo = info;
    }
}

bool FEngFont::IsJoyEventTexture(const short *pInputString, unsigned long Flags) {
    int count = 0;
    if (!pInputString || (Flags & 0x820)) {
        return false;
    }
    if (*pInputString != '$') {
        return false;
    }
    pInputString++;
    short c = *pInputString;
    while (c != 0 && c != '$') {
        pInputString++;
        count++;
        c = *pInputString;
    }
    return count != 0;
}

const short *FEngFont::SkipJoyEventTexture(const short *pInputString, unsigned long Flags) {
    if (!pInputString) {
        return pInputString;
    }
    if (Flags & 0x820) {
        return pInputString;
    }
    if (*pInputString != '$') {
        return pInputString;
    }
    pInputString++;
    if (*pInputString == '$') {
        return pInputString;
    }
    while (*pInputString != 0 && *pInputString != '$') {
        pInputString++;
    }
    return pInputString + 1;
}

float FEngFont::GetJoyEventTextureWidth(const short *pInputString) {
    float result = 0.0f;
    const TextureInfo *info = GetJoyEventTextureInfo(pInputString);
    if (info) {
        result = static_cast<float>(*(reinterpret_cast<const short *>(reinterpret_cast<const char *>(info) + 0x44)));
    }
    return result;
}