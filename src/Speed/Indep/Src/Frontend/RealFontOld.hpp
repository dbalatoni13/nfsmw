#ifndef FRONTEND_REAL_FONT_OLD_H
#define FRONTEND_REAL_FONT_OLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

// TODO: this should be a library: D:/env/egami/realgraph/6

namespace RealFontOld {

typedef enum {
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
    FONT_STATEMAX = 24
} FontState;

typedef enum {
    FONT_FALSE = 0,
    FONT_TRUE = 1,
    FONT_FILTER_POINT = 0,
    FONT_FILTER_BILINEAR = 1,
    FONT_DEPTHWRITE_DISABLE = 0,
    FONT_DEPTHWRITE_ENABLE = 1,
    FONT_BLEND_OFF = 0,
    FONT_BLEND_ALPHA = 1,
    FONT_BLEND_ALPHATEST = 2
} FontValue;

// total size: 0x10
struct Glyph {
    unsigned short mUnicode;   // offset 0x0
    unsigned char mWidth;      // offset 0x2
    unsigned char mHeight;     // offset 0x3
    unsigned short mU;         // offset 0x4
    unsigned short mV;         // offset 0x6
    signed char mAdvanceY;     // offset 0x8
    signed char mOffsetX;      // offset 0x9
    signed char mOffsetY;      // offset 0xA
    unsigned char mNumKern;    // offset 0xB
    unsigned short mKernIndex; // offset 0xC
    short mAdvanceX;           // offset 0xE
};

// total size: 0xC
struct FontGlyphList {
    // Members
    const Glyph *mGlyph; // offset 0x0, size 0x4
    float mX;            // offset 0x4, size 0x4
    float mY;            // offset 0x8, size 0x4
};

typedef enum { DRAWLISTSTART_NEW = 0, DRAWLISTSTART_CONTINUE = 1 } DrawListStart;

// total size: 0x80
class Font {
  public:
    static Font *Create(void *data);
    static void Destroy(Font *font);
    Font() {}
    ~Font() {}

    // static void SetDriver(const FontDriver *driver) {}

    // int GetState(enum FontState state) const {}

    bool SetState(FontState state, int value) {}

    float GetStateF(FontState state) const {}

    bool SetStateF(FontState state, float value) {}

    // Shape *GetShape() const {}

    // void SetShape(const Shape *shp) {}

    // unsigned int GetColour() const {}

    // void SetColour(const ARGB argb) {}

    void SetColour(unsigned int argb) {}

    int GetHeight() const {}

    void SetHeight(int height) {}

    int GetVersion() const {}

    const RealFontOld::Glyph *GetGlyph(int unicode) const {}

    int GetState();
    bool SetState();
    float GetStateF();
    bool SetStateF();
    // Shape *GetShape();
    void SetShape();
    unsigned int GetColour();
    void SetColour();
    int GetHeight();
    void SetHeight();
    int GetVersion();
    RealFontOld::Glyph *GetGlyph();
    int GetKern(const RealFontOld::Glyph *glyph, wchar_t wch);
    void GetRect();
    // void GetRect();
    // void GetRect();
    // void GetRect();
    void RenderText();
    // void RenderText();
    void RenderTextF();
    // Shape *MakeShape();
    // Shape *MakeShape();
    int GetDrawListSize();
    // int GetDrawListSize();
    void BuildDrawList();
    // void BuildDrawList();

    // TODO:  private:
    void Print();

    // Members
    char mSignature[4];      // offset 0x0, size 0x4
    unsigned int mSize;      // offset 0x4, size 0x4
    unsigned short mVersion; // offset 0x8, size 0x2
    unsigned short mNum;     // offset 0xA, size 0x2
    int mFlags;              // offset 0xC, size 0x4
    signed char mCenterX;    // offset 0x10, size 0x1
    signed char mCenterY;    // offset 0x11, size 0x1
    unsigned char mAscent;   // offset 0x12, size 0x1
    unsigned char mDescent;  // offset 0x13, size 0x1
    int mGlyphTbl;           // offset 0x14, size 0x4
    int mKernTbl;            // offset 0x18, size 0x4
    int mShape;              // offset 0x1C, size 0x4
    int mStates[24];         // offset 0x20, size 0x60
};

// total size: 0x1
struct FontDrawList {
    // Functions
    inline FontDrawList() {}

    inline ~FontDrawList() {}
};

// total size: 0x40
struct FontDriver {
    // Members
    int mVersion;                                                                              // offset 0x0, size 0x4
    int mFlags;                                                                                // offset 0x4, size 0x4
    void (*mDraw)(Font *, const FontGlyphList *, int);                                         // offset 0x8, size 0x4
    bool (*mSetState)(Font *, FontState, int);                                                 // offset 0xC, size 0x4
    void (*mStartDraw)(Font *);                                                                // offset 0x10, size 0x4
    void (*mEndDraw)(Font *);                                                                  // offset 0x14, size 0x4
    void (*mCreateFont)(Font *);                                                               // offset 0x18, size 0x4
    void (*mDestroyFont)(Font *);                                                              // offset 0x1C, size 0x4
    int (*mDrawListGetSize)(int);                                                              // offset 0x20, size 0x4
    void (*mDrawListBuild)(Font *, const FontGlyphList *, int, FontDrawList *, DrawListStart); // offset 0x24, size 0x4
    bool (*mDrawListSetState)(FontDrawList *, FontState, int);                                 // offset 0x28, size 0x4
    void (*mDrawListDraw)(FontDrawList *, float, float);                                       // offset 0x2C, size 0x4
    int mReserved[4];                                                                          // offset 0x30, size 0x10
};

const RealFontOld::Glyph *BSearch(short unicode, const RealFontOld::Glyph *table, unsigned int count, int stride = 0);

} // namespace RealFontOld

#endif
