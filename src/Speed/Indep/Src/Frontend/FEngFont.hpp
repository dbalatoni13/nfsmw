#ifndef FRONTEND_FENG_FONT_H
#define FRONTEND_FENG_FONT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"

// total size: 0x30
class FEngFont : public bTNode<FEngFont> {
  public:
    FEngFont(bChunk *chunk);
    ~FEngFont();
    void RenderString(const FEColor &Color, const i16 *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached,
                      FEPackageRenderInfo *pkg_render_info);
    inline TextureInfo *GetTextureInfo() {
        return pTextureInfo;
    }
    void NotifyTextureLoading(TexturePack *texture_pack, bool loading);
    inline uint32 GetHashID() {
        return FontHash;
    }
    float CalculateXOffset(uint32 ulJustification, float fLineWidth);
    float CalculateYOffset(uint32 ulJustification, float fTextHeight);
    u16 ConvertCharacter(u16 c);
    float GetNextWordWidth(const i16 *pcString, u32 flags);
    float GetCharacterWidth(i16 Char, i16 PrevChar, u32 Flags);
    float GetLineWidth(const i16 *pcString, u32 flags, u32 maxWidth, bool word_wrap);
    float GetTextHeight(const i16 *pcString, int ilLeading, u32 flags, u32 maxWidth, bool word_wrap);
    float GetTextWidth(const i16 *pcString, u32 flags);
    float GetHeight();
    static bool IsJoyEventTexture(const i16 *pInputString, u32 Flags);
    static const i16 *SkipJoyEventTexture(const i16 *pInputString, u32 Flags);
    float GetJoyEventTextureWidth(const i16 *pInputString);

  private:
    const TextureInfo *GetJoyEventTextureInfo(const i16 *pInputString);
    float RenderJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                FEPackageRenderInfo *pkg_render_info);
    inline void PrintCharacter(const RealFontOld::Glyph *pGlyph, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                               FEPackageRenderInfo *pkg_render_info) {}
    inline float PrintSpecialCharacter(TextureInfo *texture_info, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                       FEPackageRenderInfo *pkg_render_info) {}
    const i16 *HandleJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached, float &advance,
                                     FEPackageRenderInfo *pkg_render_info);

    // Members
    TextureInfo *pTextureInfo; // offset 0x8, size 0x4
    RealFontOld::Font *pFont;  // offset 0xC, size 0x4
    float mfZValue;            // offset 0x10, size 0x4
    uint32 FontHash;           // offset 0x14, size 0x4
    uint32 TextureHash;        // offset 0x18, size 0x4
    char *pFontName;           // offset 0x1C, size 0x4
    char *pTextureName;        // offset 0x20, size 0x4
    float Height;              // offset 0x24, size 0x4
    float fBaselineOffset;     // offset 0x28, size 0x4
    float fLeadingScale;       // offset 0x2C, size 0x4
};

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading);
FEngFont *FindFont(uint32 handle);

#endif
