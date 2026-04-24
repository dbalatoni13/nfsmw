#ifndef FRONTEND_REAL_FONT_OLD_H
#define FRONTEND_REAL_FONT_OLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum FontState {
    FONT_COLOUR = 0,
    FONT_HEIGHT = 1,
    FONT_Z = 2,
    FONT_W = 3,
    FONT_SHAPE = 4,
    FONT_UNDEF = 5,
    FONT_SCALE_X = 6,
    FONT_SCALE_Y = 7,
    FONT_SCALE_U = 8,
    FONT_SCALE_V = 9,
    FONT_FILTER = 10,
    FONT_DEPTHWRITE = 11,
    FONT_DRAWLISTMAX = 12,
    FONT_ANGLE = 13,
    FONT_DANGLE = 14,
    FONT_BLEND = 15,
    FONT_STATEMAX = 24,
};

namespace RealFontOld {

// total size: 0x10
struct Glyph {
    unsigned short mUnicode; // offset 0x0
    unsigned char mWidth;    // offset 0x2
    unsigned char mHeight;   // offset 0x3
    unsigned short mU;       // offset 0x4
    unsigned short mV;       // offset 0x6
    signed char mAdvanceY;   // offset 0x8
    signed char mOffsetX;    // offset 0x9
    signed char mOffsetY;    // offset 0xA
    unsigned char mNumKern;  // offset 0xB
    unsigned short mKernIndex; // offset 0xC
    short mAdvanceX;         // offset 0xE
};

// total size: 0x80
struct Font {
    char mSignature[4];      // offset 0x0
    unsigned int mSize;      // offset 0x4
    unsigned short mVersion; // offset 0x8
    unsigned short mNum;     // offset 0xA
    int mFlags;              // offset 0xC
    signed char mCenterX;    // offset 0x10
    signed char mCenterY;    // offset 0x11
    unsigned char mAscent;   // offset 0x12
    unsigned char mDescent;  // offset 0x13
    int mGlyphTbl;           // offset 0x14
    int mKernTbl;            // offset 0x18
    int mShape;              // offset 0x1C
    int mStates[24];         // offset 0x20

    static Font* Create(void* data);
    static void Destroy(Font* font);

    int GetState(FontState state) const { return mStates[state]; }
    int GetHeight() const { return GetState(FONT_HEIGHT); }
    short GetKern(const Glyph* glyph, short prev_char) const;

    const Glyph* GetGlyph(int unicode) const {
        const int GlyphSize = (mFlags & 0x40000) ? 0x10 : 0x0C;
        const Glyph* GlyphTbl = reinterpret_cast<const Glyph*>(reinterpret_cast<const char*>(this) + mGlyphTbl);
        if (static_cast<unsigned int>(unicode - 0x20) < mNum) {
            const Glyph* ch = reinterpret_cast<const Glyph*>(reinterpret_cast<const char*>(GlyphTbl) + (unicode - 0x20) * GlyphSize);
            if (ch->mUnicode == static_cast<unsigned short>(unicode)) {
                return ch;
            }
        }
        return nullptr;
    }
};

const Glyph* BSearch(short unicode, const Glyph* table, unsigned int count, int stride = 0);

} // namespace RealFontOld

inline bool IsNewlineChar(short c) {
    bool result = false;
    if (c == '\n' || c == '^') {
        result = true;
    }
    return result;
}

#endif
