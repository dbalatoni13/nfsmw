#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include <new>

extern unsigned long FEHashUpper(const char *str);
extern int bStrLen(const char *str);
extern int bStrICmp(const char *s1, const char *s2);
extern const char *GetLanguageName(int language_id);
extern TextureInfo *GetTextureInfo(unsigned int hash, int param2, int param3);
extern void bMemSet(void *dst, int val, unsigned int size);
extern void WideToCharString(char *dst, unsigned int dstSize, const short *src);
extern int bStrCmp(const char *s1, const char *s2);
extern unsigned int bStringHashUpper(const char *str);
extern unsigned int FEngColorToEpolyColor(FEColor c);

TextureInfo *FixupTextureInfoNull(TextureInfo *info, unsigned int hash, TexturePack *pack, bool loading);

struct ExtraFontData {
    unsigned int FontHash;
    float BaselineOffset;
    float LeadingScale;
};

static ExtraFontData ExtraFontDataTable[] = {
    {0xDCA5485A, 18.0f, 2.0f},
    {0x833A8678, 22.0f, 2.0f},
    {0xF88A75F9, 18.0f, 1.0f},
    {0x71C777D7, 23.0f, 1.0f},
};

bTList<FEngFont> FEngFonts;
extern unsigned int FontReplacementTable[];
extern int NumFontReplacements;

ExtraFontData *FindExtraFontData(unsigned int font_hash) {
    for (int i = 0; i < 4; i++) {
        if (font_hash == ExtraFontDataTable[i].FontHash) {
            return &ExtraFontDataTable[i];
        }
    }
    return nullptr;
}

FEngFont *FindFont(unsigned int font_hash) {
    {
        FEngFont *f = FEngFonts.GetHead();
        while (f != FEngFonts.EndOfList()) {
            if (f->GetHashID() == font_hash) {
                return f;
            }
            f = f->GetNext();
        }
    }
    {
        for (int i = 0; i < NumFontReplacements; i++) {
            unsigned int match_font = FontReplacementTable[i * 2];
            unsigned int replace_font = FontReplacementTable[i * 2 + 1];
            if (font_hash == match_font) {
                FEngFont *f = FEngFonts.GetHead();
                while (f != FEngFonts.EndOfList()) {
                    if (f->GetHashID() == replace_font) {
                        return f;
                    }
                    f = f->GetNext();
                }
            }
        }
    }
    return nullptr;
}

void *FEngMalloc(unsigned int size, const char *file, int line);
void FEngFree(void *ptr, const char *file, int line);

int LoaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() != 0x30201) {
        return 0;
    }
    void *mem = FEngMalloc(sizeof(FEngFont), nullptr, 0);
    FEngFont *font = new(mem) FEngFont(chunk);
    FEngFonts.AddHead(font);
    ExtraFontData *extra = FindExtraFontData(chunk->GetID());
    if (extra) {
        font->fBaselineOffset = extra->BaselineOffset;
        font->fLeadingScale = extra->LeadingScale;
    }
    return 1;
}

int UnloaderFEngFont(bChunk *chunk) {
    if (chunk->GetID() != 0x30201) {
        return 0;
    }
    unsigned int hashID = FEHashUpper(static_cast<char *>(chunk->GetData()));
    FEngFont *font = FindFont(hashID);
    if (font) {
        FEngFonts.Remove(font);
        delete font;
    }
    return 1;
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

float FEngFont::GetHeight() {
    return Height;
}

FEngFont::FEngFont(bChunk *chunk)
    : pTextureInfo(nullptr) //
    , pFont(nullptr) //
    , mfZValue(0.0f) //
    , FontHash(0) //
    , TextureHash(0) //
    , pFontName(static_cast<char *>(chunk->GetData()) + 0) //
    , pTextureName(static_cast<char *>(chunk->GetData()) + 0x100) //
    , Height(0.0f) //
    , fBaselineOffset(0.0f) //
    , fLeadingScale(0.0f)
{
    unsigned int raw_font_hash = FEHashUpper(pFontName);

    int n = 0;
    do {
        int len = bStrLen(pFontName);
        if (len <= n) {
            pFont = RealFontOld::Font::Create(pTextureName + 0x100);
            FontHash = FEHashUpper(pFontName);
            TextureHash = FEHashUpper(pTextureName);
            Height = static_cast<float>(pFont->GetHeight());
            pTextureInfo = ::GetTextureInfo(TextureHash, 0, 0);
            ExtraFontData *efd = FindExtraFontData(raw_font_hash);
            if (!efd) {
                fBaselineOffset = 0.0f;
            } else {
                fBaselineOffset = efd->BaselineOffset;
                fLeadingScale = efd->LeadingScale;
            }
            if (!efd) {
                fLeadingScale = 1.0f;
            }
            return;
        }
        for (int language_id = 0; n = n + 1, language_id < 0x10; language_id++) {
            const char *lang_name = GetLanguageName(language_id);
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

const TextureInfo *FEngFont::GetJoyEventTextureInfo(const short *pInputString) {
    unsigned int texture_hash;
    if (*pInputString == '$') {
        short data[64];
        short *ptr_to_data = data;
        bMemSet(ptr_to_data, 0, 0x80);
        const short *ptr = pInputString + 1;
        unsigned int bytes_copied = 0;
        if (ptr[0] != '$' && ptr[0] != 0) {
            while (true) {
                short c = *ptr;
                bytes_copied += 2;
                ptr++;
                short next = *ptr;
                *ptr_to_data = c;
                ptr_to_data++;
                if (next == '$') break;
                if (next == 0 || bytes_copied > 0x7F) break;
            }
        }
        char buffer[128];
        WideToCharString(buffer, 0x80, data);
        bStrCmp(buffer, "ICON_SPACER");
    }
    return ::GetTextureInfo(0, 1, 0);
}

const short *FEngFont::HandleJoyEventTexture(const short *input, float fX, float fY, unsigned int *render_colors, FERenderObject *cached, float &advance, FEPackageRenderInfo *pkg_render_info) {
    const short *ptr = input;
    short data[64];
    short *ptr_to_data = data;
    bMemSet(ptr_to_data, 0, 0x80);
    short c = *input;
    unsigned int bytes_copied = 0;
    if (c != '$' && c != 0) {
        while (true) {
            *ptr_to_data = c;
            bytes_copied += 2;
            ptr++;
            c = *ptr;
            ptr_to_data++;
            if (c == '$') break;
            if (c == 0 || bytes_copied > 0x7F) break;
        }
    }
    char buffer[128];
    WideToCharString(buffer, 0x80, data);
    unsigned int hash = bStringHashUpper(buffer);
    TextureInfo *pTexInfo = ::GetTextureInfo(hash, 1, 0);
    float width = static_cast<float>(*reinterpret_cast<short *>(reinterpret_cast<char *>(pTexInfo) + 0x44));
    float height = static_cast<float>(*reinterpret_cast<short *>(reinterpret_cast<char *>(pTexInfo) + 0x46));
    float y0 = -(height * 0.5f);
    cached->AddPoly(fX, y0, fX + width, y0 + height, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, render_colors, pTexInfo, pkg_render_info);
    advance = width;
    return ptr + 1;
}

float FEngFont::GetNextWordWidth(const short *pcString, unsigned long flags) {
    float next_word_size = 0.0f;
    const short *prev_char = pcString - 1;
    const short *next_char = pcString;
    while ((flags & 0x200) == 0) {
        next_word_size += GetCharacterWidth(*next_char, *prev_char, flags);
        short next = next_char[1];
        if (next == ' ' || next == 0) break;
        if (IsNewlineChar(next)) break;
        prev_char = next_char;
        next_char++;
    }
    return next_word_size;
}

float FEngFont::GetCharacterWidth(short Char, short PrevChar, unsigned long Flags) {
    float Width = 0.0f;
    if ((Flags & 0x20) == 0) {
        if (IsNewlineChar(Char)) {
            return Width;
        }
    }
    if (Char == '\r') {
        return Width;
    }
    unsigned short converted = ConvertCharacter(static_cast<unsigned short>(Char));
    unsigned int unicode = converted;
    if ((Flags & 0x80) && unicode == 0xA0) {
        PrevChar = 0;
        unicode = 0x20;
    }
    const RealFontOld::Glyph *pGlyph = pFont->GetGlyph(static_cast<int>(unicode));
    if (!pGlyph) {
        pGlyph = RealFontOld::BSearch(static_cast<short>(unicode),
            reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl),
            pFont->mNum);
    }
    if (pGlyph) {
        if (PrevChar != 0) {
            Width += static_cast<float>(pFont->GetKern(pGlyph, PrevChar));
        }
        Width += static_cast<float>(pGlyph->mAdvanceX);
    }
    return Width;
}

float FEngFont::GetLineWidth(const short *pcString, unsigned long flags, unsigned long maxWidth, bool word_wrap) {
    float lastSpaceWidth = 0.0f;
    float width = 0.0f;
    if (!pcString) {
        return width;
    }
    short c = *pcString;
    pcString++;
    unsigned long k = 0;
    if (c != 0) {
        do {
            if (IsNewlineChar(c)) break;
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

float FEngFont::GetTextWidth(const short *pcString, unsigned long flags) {
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

float FEngFont::GetTextHeight(const short *pcString, int ilLeading, unsigned long flags, unsigned long maxWidth, bool word_wrap) {
    float height = 0.0f;
    if (!pcString) {
        return height;
    }
    bool lastCharNotReturn = true;
    bool newLine = false;
    float curLineWidth = 0.0f;
    short prev = 0;
    short c = *pcString;
    const short *next = pcString + 1;
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
                    pGlyph = RealFontOld::BSearch(static_cast<short>(unicode),
                        reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl),
                        pFont->mNum);
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

void FEngFont::RenderString(const FEColor &Color, const short *pcString, FEString *obj, bMatrix4 *matrix, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    unsigned long flags = obj->Flags;
    unsigned long format = obj->Format;
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
        const short *next = pcString + 1;
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
                        glyph = RealFontOld::BSearch(static_cast<short>(unicode),
                            reinterpret_cast<const RealFontOld::Glyph *>(reinterpret_cast<const char *>(pFont) + pFont->mGlyphTbl),
                            pFont->mNum,
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
                        cached->AddPoly(x0,
                                        y0,
                                        x0 + glyph_width,
                                        y0 + static_cast<float>(glyph->mHeight),
                                        1.0f,
                                        static_cast<float>(glyph->mU) / texture_width,
                                        static_cast<float>(glyph->mV) / texture_height,
                                        static_cast<float>(glyph->mU + glyph->mWidth + 1) / texture_width,
                                        static_cast<float>(glyph->mV + glyph->mHeight) / texture_height,
                                        render_colors,
                                        pkg_render_info);

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
