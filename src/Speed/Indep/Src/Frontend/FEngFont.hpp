#ifndef __RPLATFONT_HPP
#define __RPLATFONT_HPP

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"

// File: speed/indep/src/frontend/FEngFont.hpp
// total size: 0x30
// Decl: speed/indep/src/frontend/FEngFont.hpp:22
class FEngFont : public bTNode<FEngFont> {
  public:
    FEngFont(bChunk *chunk); // Decl: speed/indep/src/frontend/FEngFont.hpp:24
    ~FEngFont();             // Decl: speed/indep/src/frontend/FEngFont.hpp:25

    void RenderString(const FEColor &Color, const i16 *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached,
                      FEPackageRenderInfo *pkg_render_info); // Decl: speed/indep/src/frontend/FEngFont.hpp:27
    TextureInfo *GetTextureInfo() {                          // Decl: speed/indep/src/frontend/FEngFont.hpp:28
        return pTextureInfo;
    }

    void NotifyTextureLoading(TexturePack *texture_pack, bool loading);

    uint32 GetHashID() { // Decl: speed/indep/src/frontend/FEngFont.hpp:29
        return FontHash;
    }

    float CalculateXOffset(uint32 ulJustification, float fLineWidth);  // Decl: speed/indep/src/frontend/FEngFont.hpp:31
    float CalculateYOffset(uint32 ulJustification, float fTextHeight); // Decl: speed/indep/src/frontend/FEngFont.hpp:32

    u16 ConvertCharacter(u16 c);                                                  // Decl: speed/indep/src/frontend/FEngFont.hpp:34
    float GetNextWordWidth(int16 *pcString, u32 flags);                           // Decl: speed/indep/src/frontend/FEngFont.hpp:35
    float GetCharacterWidth(i16 Char, i16 PrevChar, u32 Flags);                   // Decl: speed/indep/src/frontend/FEngFont.hpp:36
    float GetLineWidth(int16 *pcString, u32 flags, u32 maxWidth, bool word_wrap); // Decl: speed/indep/src/frontend/FEngFont.hpp:37
    float GetTextHeight(const int16 *pcString, int32 ilLeading, u32 flags, u32 maxWidth,
                        bool word_wrap); // Decl: speed/indep/src/frontend/FEngFont.hpp:38

    float GetTextWidth(const int16 *pcString, u32 flags); // Decl: speed/indep/src/frontend/FEngFont.hpp:40

    float GetHeight(); // Decl: speed/indep/src/frontend/FEngFont.hpp:42

    static bool IsJoyEventTexture(i16 *pInputString, u32 Flags); // Decl: speed/indep/src/frontend/FEngFont.hpp:49

    static i16 *const SkipJoyEventTexture(i16 *pInputString, u32 Flags); // Decl: speed/indep/src/frontend/FEngFont.hpp:52

    float GetJoyEventTextureWidth(i16 *pInputString); // Decl: speed/indep/src/frontend/FEngFont.hpp:55

  private:
    TextureInfo *const GetJoyEventTextureInfo(i16 *pInputString); // Decl: speed/indep/src/frontend/FEngFont.hpp:59

    float RenderJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                FEPackageRenderInfo *pkg_render_info); // Decl: speed/indep/src/frontend/FEngFont.hpp:60

    void PrintCharacter(const RealFontOld::Glyph *pGlyph, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                        FEPackageRenderInfo *pkg_render_info) {} // Decl: speed/indep/src/frontend/FEngFont.hpp:67
    float PrintSpecialCharacter(TextureInfo *texture_info, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                FEPackageRenderInfo *pkg_render_info) {} // Decl: speed/indep/src/frontend/FEngFont.hpp:68
    i16 *const HandleJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached, float &advance,
                                     FEPackageRenderInfo *pkg_render_info); // Decl: speed/indep/src/frontend/FEngFont.hpp:69

    TextureInfo *pTextureInfo; // offset 0x8, size 0x4
    RealFontOld::Font *pFont;  // offset 0xC, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:73
    float mfZValue;            // offset 0x10, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:74
    uint32 FontHash;           // offset 0x14, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:75
    uint32 TextureHash;        // offset 0x18, size 0x4
    char *pFontName;           // offset 0x1C, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:76
    char *pTextureName;        // offset 0x20, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:77
    float Height;              // offset 0x24, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:78
    float fBaselineOffset;     // offset 0x28, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:79
    float fLeadingScale;       // offset 0x2C, size 0x4, Decl: speed/indep/src/frontend/FEngFont.hpp:80
};

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading);
FEngFont *FindFont(uint32 handle);

#endif
