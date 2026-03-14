#ifndef FRONTEND_FENG_FONT_H
#define FRONTEND_FENG_FONT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct Font;
struct bChunk;
struct FEColor;
struct FEString;
struct FERenderObject;
struct FEPackageRenderInfo;
struct TextureInfo;
struct TexturePack;
struct Glyph;
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
    void RenderString(const FEColor &Color, const short *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info);

    TextureInfo *GetTextureInfo() { return pTextureInfo; }
    unsigned int GetHashID() { return FontHash; }

    TextureInfo *pTextureInfo; // offset 0x8, size 0x4
    Font *pFont;               // offset 0xC, size 0x4
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
