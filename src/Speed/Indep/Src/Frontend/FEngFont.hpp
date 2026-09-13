#ifndef __RPLATFONT_HPP
#define __RPLATFONT_HPP

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"

// total size: 0x30
// Decl: 22
class FEngFont : public bTNode<FEngFont> {
  public:
    FEngFont(bChunk *chunk); // Decl: 24
    ~FEngFont();             // Decl: 25

    void RenderString(const FEColor &Color, const i16 *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached,
                      FEPackageRenderInfo *pkg_render_info); // Decl: 27
    TextureInfo *GetTextureInfo() {                          // Decl: 28
        return pTextureInfo;
    }

    void NotifyTextureLoading(TexturePack *texture_pack, bool loading);

    uint32 GetHashID() { // Decl: 29
        return FontHash;
    }

    float CalculateXOffset(uint32 ulJustification, float fLineWidth);  // Decl: 31
    float CalculateYOffset(uint32 ulJustification, float fTextHeight); // Decl: 32

    u16 ConvertCharacter(u16 c);                                                        // Decl: 34
    float GetNextWordWidth(const int16 *pcString, u32 flags);                           // Decl: 35
    float GetCharacterWidth(i16 Char, i16 PrevChar, u32 Flags);                         // Decl: 36
    float GetLineWidth(const int16 *pcString, u32 flags, u32 maxWidth, bool word_wrap); // Decl: 37
    float GetTextHeight(const int16 *pcString, int32 ilLeading, u32 flags, u32 maxWidth,
                        bool word_wrap); // Decl: 38

    float GetTextWidth(const int16 *pcString, u32 flags); // Decl: 40

    float GetHeight(); // Decl: 42

    static bool IsJoyEventTexture(const i16 *pInputString, u32 Flags); // Decl: 49

    static const i16 *SkipJoyEventTexture(const i16 *pInputString, u32 Flags); // Decl: 52

    float GetJoyEventTextureWidth(const i16 *pInputString); // Decl: 55

  private:
    const TextureInfo *GetJoyEventTextureInfo(const i16 *pInputString); // Decl: 59

    float RenderJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                FEPackageRenderInfo *pkg_render_info); // Decl: 60

    void PrintCharacter(const RealFontOld::Glyph *pGlyph, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                        FEPackageRenderInfo *pkg_render_info) {} // Decl: 67
    float PrintSpecialCharacter(TextureInfo *texture_info, float fX, float fY, uint32 *render_colors, FERenderObject *cached,
                                FEPackageRenderInfo *pkg_render_info) {} // Decl: 68
    const i16 *HandleJoyEventTexture(const i16 *input, float fX, float fY, uint32 *render_colors, FERenderObject *cached, float &advance,
                                     FEPackageRenderInfo *pkg_render_info); // Decl: 69

    TextureInfo *pTextureInfo; // offset 0x8, size 0x4
    RealFontOld::Font *pFont;  // offset 0xC, size 0x4, Decl: 73
    float mfZValue;            // offset 0x10, size 0x4, Decl: 74
    uint32 FontHash;           // offset 0x14, size 0x4, Decl: 75
    uint32 TextureHash;        // offset 0x18, size 0x4
    char *pFontName;           // offset 0x1C, size 0x4, Decl: 76
    char *pTextureName;        // offset 0x20, size 0x4, Decl: 77
    float Height;              // offset 0x24, size 0x4, Decl: 78
    float fBaselineOffset;     // offset 0x28, size 0x4, Decl: 79
    float fLeadingScale;       // offset 0x2C, size 0x4, Decl: 80
};

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading);
FEngFont *FindFont(uint32 handle);
bool IsNewlineChar(i16 c);

#endif
