#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"

// File: speed/indep/src/frontend/FEngFont.cpp
// total size: 0xC
// Decl: speed/indep/src/frontend/FEngFont.cpp:309
struct ExtraFontData {
    uint32 FontHash;      // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/FEngFont.cpp:310
    float BaselineOffset; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/FEngFont.cpp:311
    float LeadingScale;   // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/FEngFont.cpp:312
};

uint32 FontReplacementTable[2] = {0x9583AA1A, __CONDUITMDITC_TT21I__};
static ExtraFontData ExtraFontDataTable[] = {
    {0xDCA5485A, 18.0f, 2.0f},
    {0x833A8678, 22.0f, 2.0f},
    {0xF88A75F9, 18.0f, 1.0f},
    {0x71C777D7, 23.0f, 1.0f},
};

bTList<FEngFont> FEngFonts;

// Decl: /speed/indep/src/frontend/FEngFont.cpp:43
inline bool IsNewlineChar(i16 c) {
    return c == '\n' || c == '^';
}

ExtraFontData *FindExtraFontData(uint32 font_hash) {
    for (int i = 0; i < 4; i++) {
        if (font_hash == ExtraFontDataTable[i].FontHash) {
            return &ExtraFontDataTable[i];
        }
    }
    return nullptr;
}

// Decl: speed/indep/src/frontend/FEngFont.cpp:177

FEngFont *FindFont(uint32 font_hash) {
    uint32 hash = font_hash;
restart:
    {
        FEngFont *f = FEngFonts.GetHead();
        while (f != FEngFonts.EndOfList()) {
            if (f->GetHashID() == hash) {
                return f;
            }
            f = f->GetNext();
        }
    }
    {
        int i = 0;
        while (i <= 1) {
            uint32 match_font = FontReplacementTable[i];
            uint32 replace_font = FontReplacementTable[i + 1];
            if (hash == match_font) {
                hash = replace_font;
                goto restart;
            }
            i += 2;
        }
    }
    return 0;
}

int LoaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() == 0x30201) {

        FEngFont *font = FNEW FEngFont(chunk);

        FEngFonts.AddHead(font);

        return 1;
    }

    return 0;
}

int UnloaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() == 0x30201) {

        uint32 hashID = FEHashUpper(static_cast<char *>(chunk->GetData()));
        FEngFont *font = FindFont(hashID);

        if (font) {

            FEngFonts.Remove(font);
            delete font;
        }

        return 1;
    }

    return 0;
}

FEngFont::FEngFont(bChunk *chunk) {
    pTextureInfo = 0;
    pFont = 0;
    mfZValue = 0.0f;
    Height = 0.0f;
    FontHash = 0;
    TextureHash = 0;
    pFontName = static_cast<char *>(chunk->GetData());
    pTextureName = pFontName + 0x100;

    uint32 raw_font_hash = FEHashUpper(pFontName);
    int n = 0;
    while (n < bStrLen(pFontName)) {
        for (int language_id = 0; language_id <= 15; language_id++) {
            if (bStrICmp(pFontName + n, GetLanguageName(static_cast<eLanguages>(language_id))) == 0) {
                if (n > 0 && pFontName[n - 1] == '_') {
                    pFontName[n - 1] = 0;
                    break;
                }
            }
        }
        n++;
    }

    pFont = RealFontOld::Font::Create(pTextureName + 0x100);
    FontHash = FEHashUpper(pFontName);
    TextureHash = FEHashUpper(pTextureName);
    Height = static_cast<float>(pFont->mStates[1]);
    pTextureInfo = ::GetTextureInfo(TextureHash, 0, 0);
    ExtraFontData *efd = FindExtraFontData(raw_font_hash);
    if (efd) {
        fBaselineOffset = efd->BaselineOffset;
        fLeadingScale = efd->LeadingScale;
    } else {
        fBaselineOffset = 0.0f;
        fLeadingScale = 1.0f;
    }
}

FEngFont::~FEngFont() {
    RealFontOld::Font::Destroy(pFont);
}

void FEngFont::NotifyTextureLoading(TexturePack *texture_pack, bool loading) {
    TextureInfo *info = FixupTextureInfoNull(pTextureInfo, TextureHash, texture_pack, loading);
    if (info != pTextureInfo) {
        pTextureInfo = info;
    }
}

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading) {
    {
        FEngFont *font = FEngFonts.GetHead();
        while (font != FEngFonts.EndOfList()) {
            font->NotifyTextureLoading(texture_pack, loading);
            font = font->GetNext();
        }
    }
}

bool FEngFont::IsJoyEventTexture(const i16 *pInputString, u32 Flags) {
    bool bRet = false;

    if (pInputString != nullptr && (Flags & 0x820) == 0) {

        if (*pInputString == '$') {

            int count = 0;

            short c = *++pInputString;

            while (c != 0 && c != '$') {
                count++;
                c = *++pInputString;
            }

            bRet = count != 0;
        }
    }

    return bRet;
}

const i16 *FEngFont::SkipJoyEventTexture(const i16 *pInputString, u32 Flags) {
    if (!pInputString || (Flags & 0x820)) {
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

float FEngFont::GetJoyEventTextureWidth(const i16 *pInputString) {
    float result = 0.0f;
    const TextureInfo *info = GetJoyEventTextureInfo(pInputString);
    if (info) {
        result = static_cast<float>(*(reinterpret_cast<const i16 *>(reinterpret_cast<const char *>(info) + 0x44)));
    }
    return result;
}

const TextureInfo *FEngFont::GetJoyEventTextureInfo(const i16 *pInputString) {
    unsigned int texture_hash;

    if (*pInputString == '$') {

        const i16 *ptr = pInputString + 1;

        i16 data[64];
        i16 *ptr_to_data = data;
        bMemSet(ptr_to_data, 0, 0x80);

        unsigned int bytes_copied = 0;

        char buffer[128];

        if (ptr[0] != '$' && ptr[0] != 0) {

            while (true) {

                *ptr_to_data++ = *ptr;
                bytes_copied += 2;

                i16 next = *++ptr;
                if (next == '$') {
                    break;
                }
                if (next == 0) {
                    break;
                }
                if (bytes_copied > 0x7F) {
                    break;
                }
            }
        }

        WideToCharString(buffer, 0x80, data);

        bStrCmp(buffer, "ICON_SPACER");
    }

    return ::GetTextureInfo(0, 1, 0);
}

inline float FEngFont::PrintSpecialCharacter(TextureInfo *texture_info, float fX, float fY, unsigned int *render_colors, FERenderObject *cached,
                                            FEPackageRenderInfo *pkg_render_info) {
    float width = static_cast<float>(texture_info->Width);
    float height = static_cast<float>(texture_info->Height);

    float s0 = 0.0f;
    float t0 = 0.0f;
    float s1 = 1.0f;
    float t1 = 1.0f;

    float x0 = fX;
    float y0 = -(height * 0.5f);

    cached->AddPoly(x0, y0, x0 + width, y0 + height, 1.0f, s0, t0, s1, t1, render_colors, texture_info, pkg_render_info);

    return width;
}

const i16 *FEngFont::HandleJoyEventTexture(const i16 *input, float fX, float fY, unsigned int *render_colors, FERenderObject *cached, float &advance,
                                           FEPackageRenderInfo *pkg_render_info) {

    const i16 *ptr = input;

    i16 data[64];
    i16 *ptr_to_data = data;
    bMemSet(ptr_to_data, 0, 0x80);

    unsigned int bytes_copied = 0;

    char buffer[128];

    u16 c = *ptr;
    i16 sc = c;
    if (sc != '$' && sc != 0) {

        while (true) {

            *ptr_to_data++ = c;
            bytes_copied += 2;

            c = *++ptr;
            sc = c;
            if (sc == '$') {
                break;
            }
            if (sc == 0) {
                break;
            }
            if (bytes_copied > 0x7F) {
                break;
            }
        }
    }

    WideToCharString(buffer, 0x80, data);

    TextureInfo *pTextureInfo = ::GetTextureInfo(bStringHashUpper(buffer), 1, 0);

    advance = PrintSpecialCharacter(pTextureInfo, fX, fY, render_colors, cached, pkg_render_info);

    return ptr + 1;
}

inline void FEngFont::PrintCharacter(const RealFontOld::Glyph *pGlyph, float fX, float fY, unsigned int *render_colors, FERenderObject *cached,
                              FEPackageRenderInfo *pkg_render_info) {
    float s0 = static_cast<float>(static_cast<int>(pGlyph->mU)) / static_cast<float>(pTextureInfo->Width);
    float s1 = static_cast<float>((pGlyph->mU + 1) + pGlyph->mWidth) / static_cast<float>(pTextureInfo->Width);
    float t0 = static_cast<float>(static_cast<int>(pGlyph->mV)) / static_cast<float>(pTextureInfo->Height);
    float t1 = static_cast<float>(static_cast<int>(pGlyph->mV) + pGlyph->mHeight) / static_cast<float>(pTextureInfo->Height);

    float width = static_cast<float>(pGlyph->mWidth);
    if (width < 4.0f) {
        width = 4.0f;
    }

    float base_offset = fBaselineOffset;

    float x0 = fX + static_cast<float>(pGlyph->mOffsetX);
    float x1 = x0 + width;
    float y0 = fY + static_cast<float>(pGlyph->mOffsetY) + base_offset;
    float y1 = y0 + static_cast<float>(static_cast<int>(pGlyph->mHeight));

    cached->AddPoly(x0, y0, x1, y1, 1.0f, s0, t0, s1, t1, render_colors, pkg_render_info);
}

void FEngFont::RenderString(const FEColor &Color, const i16 *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached,
                            FEPackageRenderInfo *pkg_render_info) {
    unsigned int ulJustification = obj->Format;
    int ilLeading = static_cast<int>(static_cast<float>(obj->Leading) * fLeadingScale);
    u32 flags = obj->Flags;
    u32 max_width = obj->MaxWidth;
    unsigned int render_color = FEngColorToEpolyColor(Color);
    unsigned int render_colors[4];
    render_colors[0] = render_color;
    render_colors[1] = render_color;
    render_colors[2] = render_color;
    render_colors[3] = render_color;
    float lineWidth = GetLineWidth(pcString, flags, obj->MaxWidth, (ulJustification & 0x10) != 0);
    float fCursorY = CalculateYOffset(ulJustification, GetTextHeight(pcString, ilLeading, flags, obj->MaxWidth, (ulJustification & 0x10) != 0));
    float fCursorX = CalculateXOffset(ulJustification, lineWidth);
    float fStartX = fCursorX;
    bool texturizer;
    u16 c;

    if (GetTextureInfo()) {
        u32 k;

        cached->SetTransform(matrix);

        texturizer = true;
        c = *pcString++;
        for (k = 0; c != 0; c = *pcString++, k++) {
            const RealFontOld::Glyph *pGlyph;
            float kern;
            u16 prevChar;

            if (c != ' ' || fStartX != fCursorX || (obj->Format & 0x10) == 0) {
                if ((flags & 0x20) == 0 && IsNewlineChar(c)) {
                    if (*pcString == 0) {
                        break;
                    }
                    fCursorX = CalculateXOffset(ulJustification, GetLineWidth(pcString, flags, max_width, (ulJustification & 0x10) != 0));
                    fCursorY += Height + static_cast<float>(ilLeading);
                    fStartX = fCursorX;
                    continue;
                } else {
                    if (max_width != 0 && c == ' ' && (obj->Format & 0x10)) {
                        float next_word_size = GetNextWordWidth(pcString - 1, obj->Flags);
                        if ((fCursorX - fStartX) + next_word_size > static_cast<float>(max_width)) {
                            fCursorX = CalculateXOffset(ulJustification, GetLineWidth(pcString, flags, max_width, (ulJustification & 0x10) != 0));
                            fCursorY += Height + static_cast<float>(ilLeading);
                            fStartX = fCursorX;
                            continue;
                        }
                    }

                    if ((flags & 0x820) == 0 && c == '$') {
                        i16 c1 = *pcString;
                        if (c1 == '$') {
                            c = *pcString;
                            pcString++;
                            k++;
                            texturizer = false;
                        } else if (texturizer) {
                            float advance = 0.0f;
                            pcString = HandleJoyEventTexture(pcString, fCursorX, fCursorY, render_colors, cached, advance, pkg_render_info);
                            fCursorX += advance;
                            continue;
                        }
                    }

                    c = ConvertCharacter(c);
                    pGlyph = pFont->GetGlyph(static_cast<int>(c));
                    kern = 0.0f;
                    prevChar = pcString[-2];
                    if (pGlyph) {
                        float width;
                        if (prevChar != 0 && k != 0) {
                            kern = static_cast<float>(pFont->GetKern(pGlyph, prevChar));
                        }
                        PrintCharacter(pGlyph, fCursorX + kern, fCursorY, render_colors, cached, pkg_render_info);
                        width = 0.0f;
                        if (k == 0) {
                            fCursorX += GetCharacterWidth(static_cast<i16>(c), 0, ulJustification);
                        } else {
                            fCursorX += GetCharacterWidth(static_cast<i16>(c), pcString[-2], ulJustification);
                        }
                        fCursorX += width;
                    }
                }
            }
        }
        cached->Render();
    }
}

float FEngFont::GetNextWordWidth(const i16 *pcString, u32 flags) {
    float next_word_size = 0.0f;
    const i16 *prev_char = pcString - 1;
    const i16 *next_char = pcString;
    if ((flags & 0x200) == 0) {
        do {
            next_word_size += GetCharacterWidth(*next_char, *prev_char, flags);
            prev_char = next_char;

            ++next_char;
        } while (*next_char != ' ' && *next_char != 0 && !IsNewlineChar(*next_char));
    }
    return next_word_size;
}

float FEngFont::GetCharacterWidth(short Char, short PrevChar, u32 Flags) {
    float Width = 0.0f;
    if ((Flags & 0x20) == 0) {
        if (IsNewlineChar(Char)) {
            return 0.0f;
        }
    }
    if (Char == '\r') {
        return 0.0f;
    }
    Char = ConvertCharacter(static_cast<unsigned short>(Char));
    if ((Flags & 0x80) && Char == 0xA0) {
        PrevChar = 0;
        Char = 0x20;
    }
    const RealFontOld::Glyph *pGlyph = pFont->GetGlyph(static_cast<int>(Char));
    if (!pGlyph) {
        return 0.0f;
    }
    if (PrevChar != 0) {
        Width += static_cast<float>(pFont->GetKern(pGlyph, PrevChar));
    }
    return Width + static_cast<float>(pGlyph->mAdvanceX);
}

float FEngFont::GetLineWidth(const i16 *pcString, u32 flags, u32 maxWidth, bool word_wrap) {
    if (pcString == nullptr) {
        return 0.0f;
    }

    float width = 0.0f;
    float lastSpaceWidth = width;
    short c = *pcString++;

    u32 k = 0;
    while (c != 0) {

        if (IsNewlineChar(c)) {
            break;
        }

        if (c == ' ') {

            lastSpaceWidth = width;
        }

        if (k == 0) {

            width += GetCharacterWidth(c, 0, flags);

        } else {

            width += GetCharacterWidth(c, static_cast<short>(reinterpret_cast<int>(pcString - 1)), flags);
        }

        if (maxWidth != 0 && width > static_cast<float>(maxWidth) && word_wrap) {

            if (0.0f < lastSpaceWidth) {

                width = lastSpaceWidth;
            }

            break;
        }

        c = *pcString++;
        k++;
    }

    return width;
}

float FEngFont::GetTextWidth(const i16 *pcString, u32 flags) {
    float width = GetLineWidth(pcString, 0, 0, false);
    short c = *pcString;
    pcString++;
    if (c != 0) {
        do {
            if ((flags & 0x20) == 0) {
                if (IsNewlineChar(c)) {
                    goto next;
                }
            }
            {
                float newWidth = GetLineWidth(pcString, 0, 0, false);
                if (newWidth > width) {
                    width = newWidth;
                }
            }
        next:
            c = *pcString;
            pcString++;
        } while (c != 0);
    }
    return width;
}

float FEngFont::GetHeight() {
    return Height;
}

float FEngFont::GetTextHeight(const i16 *pcString, int ilLeading, u32 flags, u32 maxWidth, bool word_wrap) {
    if (pcString == nullptr) {
        return 0.0f;
    }
    u32 k;
    float height = 0.0f;

    bool lastCharNotReturn = true;
    bool newLine = false;

    float curLineWidth = height;

    short prev = 0;
    short c = *pcString++;
    while (c != 0) {

        if (IsNewlineChar(c)) {

            newLine = true;

        } else if (c != '') {

            const RealFontOld::Glyph *pGlyph = pFont->GetGlyph(static_cast<int>(static_cast<unsigned int>(c) & 0xFF));
            if (pGlyph) {
                lastCharNotReturn = true;
            }

            if (word_wrap && maxWidth != 0) {

                if (c == ' ') {

                    float next_word_size = GetNextWordWidth(pcString - 1, flags);

                    if (curLineWidth + next_word_size > static_cast<float>(maxWidth)) {

                        newLine = true;
                    }
                }

                curLineWidth += GetCharacterWidth(c, prev, flags);
            }
        } else {
            // '' se salta la comprobacion de newLine: el objetivo salta directo
            // a la cola del bucle (.L_8016F07C), no cae en el bloque de abajo.
            goto next_character;
        }

        if (newLine == 1) {

            newLine = false;
            curLineWidth = 0.0f;

            height += static_cast<float>(ilLeading);
            height += Height;
            lastCharNotReturn = false;
        }

    next_character:
        prev = c;
        c = *pcString++;
    }

    if (lastCharNotReturn) {

        height += Height;
    }

    return height;
}

u16 FEngFont::ConvertCharacter(u16 c) {
    u16 result = c;

    if (result > 0xFF7F) {
        result = result & 0xFF;
    }

    if (result == 0x99) {
        result = 0x2122;
    } else if (result == 0x9C) {
        result = 0x153;
    } else if (result == 0xA0) {
        result = 0x20;
    }

    return result;
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
