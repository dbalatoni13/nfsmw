#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

// total size: 0xC
// Decl: 309
struct ExtraFontData {
    uint32 FontHash;      // offset 0x0, size 0x4, Decl: 310
    float BaselineOffset; // offset 0x4, size 0x4, Decl: 311
    float LeadingScale;   // offset 0x8, size 0x4, Decl: 312
};

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

// Decl: 177
uint32 FontReplacementTable[2] = {0x9583AA1A, 0x5B9D88B9};

// UNSOLVED
FEngFont *FindFont(uint32 font_hash) {
    while (true) {
        for (FEngFont *f = FEngFonts.GetHead(); f != FEngFonts.EndOfList(); f = f->GetNext()) {
            if (f->GetHashID() == font_hash) {
                return f;
            }
        }

        for (int i = 0; i < NUM_ENTRIES(FontReplacementTable); i += 2) {
            uint32 match_font = FontReplacementTable[i];
            uint32 replace_font = FontReplacementTable[i + 1];
            if (font_hash == match_font) {
                font_hash = replace_font;
            }
        }
        return nullptr;
    }
}

int LoaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FENG_FONT) {
        FEngFont *font = FNEW FEngFont(chunk);
        FEngFonts.AddHead(font);
        return 1;
    }
    return 0;
}

int UnloaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FENG_FONT) {
        char *fontName = static_cast<char *>(chunk->GetData());
        uint32 hashID = FEHashUpper(fontName);
        FEngFont *font = FindFont(hashID);
        if (font != nullptr) {
            FEngFonts.Remove(font);
            delete font;
        }
        return 1;
    }
    return 0;
}

// UNSOLVED
FEngFont::FEngFont(bChunk *chunk)
    : pTextureInfo(nullptr), pFont(nullptr), mfZValue(0.0f), FontHash(0), TextureHash(0), pFontName(static_cast<char *>(chunk->GetData()) + 0),
      pTextureName(static_cast<char *>(chunk->GetData()) + 0x100), Height(0.0f), fBaselineOffset(0.0f), fLeadingScale(0.0f) {
    uint32 raw_font_hash = FEHashUpper(pFontName);
    ExtraFontData *efd;

    int n = 0;
    do {
        int len = bStrLen(pFontName);
        if (len <= n) {
            pFont = RealFontOld::Font::Create(pTextureName + 0x100);
            FontHash = FEHashUpper(pFontName);
            TextureHash = FEHashUpper(pTextureName);
            Height = static_cast<float>(pFont->GetHeight());
            pTextureInfo = ::GetTextureInfo(TextureHash, 0, 0);
            efd = FindExtraFontData(raw_font_hash);
            if (efd == nullptr) {
                fBaselineOffset = 0.0f;
            } else {
                fBaselineOffset = efd->BaselineOffset;
                fLeadingScale = efd->LeadingScale;
            }
            if (efd == nullptr) {
                fLeadingScale = 1.0f;
            }
            return;
        }
        for (int language_id = 0; n = n + 1, language_id < eLANGUAGE_MAX; language_id++) {
            const char *lang_name = GetLanguageName(static_cast<eLanguages>(language_id));
            if (bStrICmp(pFontName + n - 1, lang_name) == 0 && (n - 1) > 0 && pFontName[n - 2] == '_') {
                pFontName[n - 2] = '\0';
                break;
            }
        }
    } while (true);
}

FEngFont::~FEngFont() {
    RealFontOld::Font::Destroy(pFont);
}

void FEngFont::NotifyTextureLoading(TexturePack *texture_pack, bool loading) {
    TextureInfo *texture_info = FixupTextureInfoNull(pTextureInfo, TextureHash, texture_pack, loading);
    if (texture_info != pTextureInfo) {
        pTextureInfo = texture_info;
    }
}

void FEngFontNotifyTextureLoading(TexturePack *texture_pack, bool loading) {
    for (FEngFont *font = FEngFonts.GetHead(); font != FEngFonts.EndOfList(); font = font->GetNext()) {
        font->NotifyTextureLoading(texture_pack, loading);
    }
}

bool FEngFont::IsJoyEventTexture(const i16 *pInputString, u32 Flags) {
    bool bRet = false;
    if (pInputString != nullptr && !(Flags & 0x820) && *pInputString == '$') {
        int EventStrLength = 0;

        pInputString++;
        while (*pInputString != 0 && *pInputString != '$') {
            pInputString++;
            EventStrLength++;
        }
        bRet = EventStrLength != 0;
    }
    return bRet;
}

const i16 *FEngFont::SkipJoyEventTexture(const i16 *pInputString, u32 Flags) {
    const i16 *pRet = pInputString;
    if (pInputString != nullptr && !(Flags & 0x820) && *pInputString == '$') {
        pRet++;
        if (*pRet == '$') {
            return pRet;
        }
        while (*pRet != 0 && *pRet != '$') {
            pRet++;
        }
        return pRet + 1;
    }
    return pRet;
}

float FEngFont::GetJoyEventTextureWidth(const i16 *pInputString) {
    float Width = 0.0f;
    const TextureInfo *pTextureInfo = GetJoyEventTextureInfo(pInputString);
    if (pTextureInfo != nullptr) {
        Width = pTextureInfo->Width;
    }
    return Width;
}

// UNSOLVED
const TextureInfo *FEngFont::GetJoyEventTextureInfo(const i16 *pInputString) {
    unsigned int texture_hash;
    if (*pInputString == '$') {
        const i16 *ptr = pInputString + 1;
        i16 data[64];
        i16 *ptr_to_data = data;
        bMemSet(ptr_to_data, 0, 0x80);
        unsigned int bytes_copied = 0;
        if (ptr[0] != '$' && ptr[0] != 0) {
            while (true) {
                i16 c = *ptr;
                bytes_copied += 2;
                ptr++;
                i16 next = *ptr;
                *ptr_to_data = c;
                ptr_to_data++;
                if (next == '$')
                    break;
                if (next == 0 || bytes_copied > 0x7F)
                    break;
            }
        }
        char buffer[128];
        WideToCharString(buffer, 0x80, data);
        bStrCmp(buffer, "ICON_SPACER");
    }
    return ::GetTextureInfo(0, 1, 0);
}

// UNSOLVED
const i16 *FEngFont::HandleJoyEventTexture(const i16 *input, float fX, float fY, unsigned int *render_colors, FERenderObject *cached, float &advance,
                                           FEPackageRenderInfo *pkg_render_info) {
    const i16 *ptr = input;
    short data[64];
    short *ptr_to_data = data;
    bMemSet(ptr_to_data, 0, 0x80);
    unsigned int bytes_copied = 0;
    if (*ptr != '$' && *ptr != 0) {
        while (true) {
            *ptr_to_data = *ptr;
            bytes_copied += 2;
            ptr++;
            ptr_to_data++;
            if (*ptr == '$')
                break;
            if (*ptr == 0 || bytes_copied > 0x7F)
                break;
        }
    }
    char buffer[128];
    WideToCharString(buffer, sizeof(buffer), data);
    unsigned int hash = bStringHashUpper(buffer);
    TextureInfo *pTexInfo = ::GetTextureInfo(hash, 1, 0);
    float width = pTexInfo->Width;
    float height = pTexInfo->Height;
    float y0 = -(height * 0.5f);
    cached->AddPoly(fX, y0, fX + width, y0 + height, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, render_colors, pTexInfo, pkg_render_info);
    advance = width;
    return ptr + 1;
}

// UNSOLVED
void FEngFont::RenderString(const FEColor &Color, const i16 *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached,
                            FEPackageRenderInfo *pkg_render_info) {
    u32 flags = obj->Flags;
    u32 format = obj->Format;
    bool word_wrap = (format & 0x10) != 0;
    int leading = static_cast<int>(static_cast<float>(obj->Leading) * fLeadingScale);
    unsigned int render_color = FEngColorToEpolyColor(Color);
    unsigned int render_colors[4];
    render_colors[0] = render_color;
    render_colors[1] = render_color;
    render_colors[2] = render_color;
    render_colors[3] = render_color;

    float line_width = GetLineWidth(pcString, flags, obj->MaxWidth, word_wrap);
    float current_y = CalculateYOffset(format, GetTextHeight(pcString, leading, flags, obj->MaxWidth, word_wrap));
    float current_x = CalculateXOffset(format, line_width);

    if (pTextureInfo) {
        cached->SetTransform(matrix);

        float line_start_x = current_x;
        short current = *pcString;
        const i16 *next = pcString + 1;
        int character_index = 0;
        bool allow_joy_event_texture = true;

        while (current != 0) {
            if (current != ' ' || current_x != line_start_x || !word_wrap) {
                if ((flags & 0x20) == 0 && IsNewlineChar(current)) {
                    if (*next == 0) {
                        break;
                    }

                    current_x = CalculateXOffset(format, GetLineWidth(next, flags, obj->MaxWidth, word_wrap));
                    current_y += Height + static_cast<float>(leading);
                    line_start_x = current_x;
                } else {
                    if (obj->MaxWidth != 0 && current == ' ' && word_wrap) {
                        float next_word_width = GetNextWordWidth(next - 1, flags);
                        if ((current_x - line_start_x) + next_word_width > static_cast<float>(obj->MaxWidth)) {
                            current_x = CalculateXOffset(format, GetLineWidth(next, flags, obj->MaxWidth, word_wrap));
                            current_y += Height + static_cast<float>(leading);
                            line_start_x = current_x;
                        }
                    }

                    if ((flags & 0x820) == 0 && current == '$') {
                        if (*next == '$') {
                            current = *next;
                            character_index++;
                            allow_joy_event_texture = false;
                            next++;
                        } else if (allow_joy_event_texture) {
                            float advance = 0.0f;
                            next = HandleJoyEventTexture(next, current_x, current_y, render_colors, cached, advance, pkg_render_info);
                            current_x += advance;
                            goto next_character;
                        }
                    }

                    unsigned short unicode = ConvertCharacter(static_cast<unsigned short>(current));
                    int glyph_stride = (pFont->mFlags & 0x40000) ? 0x10 : 0x0C;
                    const RealFontOld::Glyph *glyph = pFont->GetGlyph(static_cast<int>(unicode));
                    if (!glyph) {
                        glyph = RealFontOld::BSearch(
                            static_cast<short>(unicode),
                            reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl), pFont->mNum,
                            glyph_stride);
                    }

                    if (glyph) {
                        float kern = 0.0f;
                        if (character_index != 0 && next[-2] != 0) {
                            kern = static_cast<float>(pFont->GetKern(glyph, next[-2]));
                        }

                        float texture_width = static_cast<float>(pTextureInfo->Width);
                        float texture_height = static_cast<float>(pTextureInfo->Height);
                        float glyph_width = static_cast<float>(glyph->mWidth);
                        if (glyph_width < 4.0f) {
                            glyph_width = 4.0f;
                        }

                        float x0 = current_x + kern + static_cast<float>(glyph->mOffsetX);
                        float y0 = current_y + static_cast<float>(glyph->mOffsetY) + fBaselineOffset;
                        cached->AddPoly(x0, y0, x0 + glyph_width, y0 + static_cast<float>(glyph->mHeight), 1.0f,
                                        static_cast<float>(glyph->mU) / texture_width, static_cast<float>(glyph->mV) / texture_height,
                                        static_cast<float>(glyph->mU + glyph->mWidth + 1) / texture_width,
                                        static_cast<float>(glyph->mV + glyph->mHeight) / texture_height, render_colors, pkg_render_info);

                        short prev_char = 0;
                        if (character_index != 0) {
                            prev_char = next[-2];
                        }
                        current_x += GetCharacterWidth(static_cast<short>(unicode), prev_char, format);
                    }
                }
            }

        next_character:
            current = *next;
            character_index++;
            next++;
        }

        cached->Render();
    }
}

// UNSOLVED
float FEngFont::GetNextWordWidth(const i16 *pcString, u32 flags) {
    float next_word_size = 0.0f;
    const i16 *prev_char = pcString - 1;
    const i16 *next_char = pcString;
    while ((flags & 0x200) == 0) {
        next_word_size += GetCharacterWidth(*next_char, *prev_char, flags);
        if (next_char[1] == ' ' || next_char[1] == 0)
            break;
        if (IsNewlineChar(next_char[1]))
            break;
        prev_char = next_char;
        next_char++;
    }
    return next_word_size;
}

// UNSOLVED
float FEngFont::GetCharacterWidth(short Char, short PrevChar, u32 Flags) {
    float Width = 0.0f;
    if ((Flags & 0x20) == 0) {
        if (IsNewlineChar(Char)) {
            return Width;
        }
    }
    if (Char == '\r') {
        return Width;
    }
    unsigned int unicode = ConvertCharacter(static_cast<unsigned short>(Char));
    if ((Flags & 0x80) && unicode == 0xA0) {
        PrevChar = 0;
        unicode = 0x20;
    }
    const RealFontOld::Glyph *pGlyph = pFont->GetGlyph(static_cast<int>(unicode));
    if (pGlyph == nullptr) {
        pGlyph =
            RealFontOld::BSearch(static_cast<short>(unicode),
                                 reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl), pFont->mNum);
    }
    if (pGlyph != nullptr) {
        if (PrevChar != 0) {
            Width += static_cast<float>(pFont->GetKern(pGlyph, PrevChar));
        }
        Width += static_cast<float>(pGlyph->mAdvanceX);
    }
    return Width;
}

// UNSOLVED
float FEngFont::GetLineWidth(const i16 *pcString, u32 flags, u32 maxWidth, bool word_wrap) {
    float lastSpaceWidth = 0.0f;
    float width = 0.0f;
    if (pcString == nullptr) {
        return width;
    }
    short c = *pcString;
    pcString++;
    u32 k = 0;
    if (c != 0) {
        do {
            if (IsNewlineChar(c))
                break;
            if (c == ' ') {
                lastSpaceWidth = width;
            }
            short prev;
            if (k == 0) {
                prev = 0;
            } else {
                prev = *(pcString - 2);
            }
            width += GetCharacterWidth(c, prev, flags);
            if (maxWidth != 0 && static_cast<float>(maxWidth) < width && word_wrap) {
                if (0.0f < lastSpaceWidth) {
                    width = lastSpaceWidth;
                }
                break;
            }
            c = *pcString;
            k++;
            pcString++;
        } while (c != 0);
    }
    return width;
}

// UNSOLVED
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

// UNSOLVED
float FEngFont::GetTextHeight(const i16 *pcString, int ilLeading, u32 flags, u32 maxWidth, bool word_wrap) {
    float height = 0.0f;
    if (!pcString) {
        return height;
    }
    bool lastCharNotReturn = true;
    bool newLine = false;
    float curLineWidth = 0.0f;
    short prev = 0;
    short c = *pcString;
    const i16 *next = pcString + 1;
    if (c != 0) {
        do {
            if (IsNewlineChar(c)) {
                newLine = true;
                if (newLine) {
                    newLine = false;
                    lastCharNotReturn = false;
                    curLineWidth = 0.0f;
                    height = static_cast<float>(ilLeading) + height + Height;
                }
            } else if (c != '\r') {
                unsigned int unicode = static_cast<unsigned int>(c) & 0xFF;
                const RealFontOld::Glyph *pGlyph = pFont->GetGlyph(static_cast<int>(unicode));
                if (!pGlyph) {
                    pGlyph = RealFontOld::BSearch(
                        static_cast<short>(unicode),
                        reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl), pFont->mNum);
                }
                if (pGlyph) {
                    lastCharNotReturn = true;
                }
                if (word_wrap && maxWidth != 0) {
                    if (c == ' ') {
                        float next_word_size = GetNextWordWidth(next - 1, flags);
                        if (static_cast<float>(maxWidth) < curLineWidth + next_word_size) {
                            newLine = true;
                        }
                    }
                    curLineWidth += GetCharacterWidth(c, prev, flags);
                }
                if (newLine) {
                    newLine = false;
                    lastCharNotReturn = false;
                    curLineWidth = 0.0f;
                    height = static_cast<float>(ilLeading) + height + Height;
                }
            }
            short s = *next;
            next++;
            prev = c;
            c = s;
        } while (c != 0);
    }
    if (lastCharNotReturn) {
        height += Height;
    }
    return height;
}

// UNSOLVED
u16 FEngFont::ConvertCharacter(u16 c) {
    // const i16 tmchar;
    // const i16 oechar;

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

float FEngFont::CalculateXOffset(uint32 ulJustification, float fLineWidth) {
    if (ulJustification & FESTRING_JUSTIFY_HCENTER) {
        return fLineWidth * -0.5f;
    }
    if (ulJustification & FESTRING_JUSTIFY_HRIGHT) {
        return -fLineWidth;
    }
    return 0.0f;
}

float FEngFont::CalculateYOffset(uint32 ulJustification, float fLineHeight) {
    if (ulJustification & FESTRING_JUSTIFY_VCENTER) {
        return fLineHeight * -0.5f;
    }
    if (ulJustification & FESTRING_JUSTIFY_VBOTTOM) {
        return -fLineHeight;
    }
    return 0.0f;
}
