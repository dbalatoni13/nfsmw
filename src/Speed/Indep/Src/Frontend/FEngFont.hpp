#ifndef FRONTEND_FENG_FONT_H
#define FRONTEND_FENG_FONT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"

struct bChunk;
struct FEColor;
struct FEString;
struct FERenderObject;
struct FEPackageRenderInfo;
struct TextureInfo;
struct TexturePack;
struct bMatrix4;

// total size: 0x30
struct FEngFont : public bTNode<FEngFont> {
    float GetHeight();
    float GetTextWidth(const short *pcString, unsigned long flags);
    float GetTextHeight(const short *pcString, int ilLeading, unsigned long flags, unsigned long maxWidth, bool word_wrap);
    float CalculateXOffset(unsigned int ulJustification, float fTextWidth);
    float CalculateYOffset(unsigned int ulJustification, float fTextHeight);

    FEngFont(bChunk *chunk);
    ~FEngFont();

    void NotifyTextureLoading(TexturePack *texture_pack, bool loading);

    static bool IsJoyEventTexture(const short *pInputString, unsigned long Flags);
    static const short *SkipJoyEventTexture(const short *pInputString, unsigned long Flags);
    float GetJoyEventTextureWidth(const short *pInputString);
    const TextureInfo *GetJoyEventTextureInfo(const short *pInputString);
    const short *HandleJoyEventTexture(const short *input, float fX, float fY, unsigned int *render_colors, FERenderObject *cached, float &advance, FEPackageRenderInfo *pkg_render_info);

    void RenderString(const FEColor &Color, const short *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);

    float GetNextWordWidth(const short *pcString, unsigned long flags);
    float GetCharacterWidth(short Char, short PrevChar, unsigned long Flags);
    float GetLineWidth(const short *pcString, unsigned long flags, unsigned long maxWidth, bool word_wrap);

    unsigned short ConvertCharacter(unsigned short c);

    TextureInfo *GetTextureInfo() { return pTextureInfo; }
    unsigned int GetHashID() { return FontHash; }

    TextureInfo *pTextureInfo;          // offset 0x8, size 0x4
    RealFontOld::Font *pFont;           // offset 0xC, size 0x4
    float mfZValue;            // offset 0x10, size 0x4
    unsigned int FontHash;     // offset 0x14, size 0x4
    unsigned int TextureHash;  // offset 0x18, size 0x4
    char *pFontName;           // offset 0x1C, size 0x4
    char *pTextureName;        // offset 0x20, size 0x4
    float Height;              // offset 0x24, size 0x4
    float fBaselineOffset;     // offset 0x28, size 0x4
    float fLeadingScale;       // offset 0x2C, size 0x4
};

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading);
FEngFont *FindFont(unsigned int handle);

#endif
