#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include <new>

extern unsigned long FEHashUpper(const char *str);

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

extern bTList<FEngFont> FEngFonts;
extern unsigned int FontReplacementTable[];
extern int NumFontReplacements;

ExtraFontData *FindExtraFontData(unsigned int font_hash) {
    for (int i = 0; i < 4; i++) {
        if (ExtraFontDataTable[i].FontHash == font_hash) {
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
    FEngFont *font = FEngFonts.GetHead();
    while (font != FEngFonts.EndOfList()) {
        font->NotifyTextureLoading(texture_pack, loading);
        font = font->GetNext();
    }
}

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