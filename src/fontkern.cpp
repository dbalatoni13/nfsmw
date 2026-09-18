// librealfontz.a(oldfontkern.cpp)

#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"

namespace RealFontOld {

// total size: 0x4 - la entrada de kerning de las dos tablas (12 y 16 bits).
struct FontKern {
    unsigned short mUnicode; // offset 0x0
    signed char mKern;       // offset 0x2
    unsigned char mIndex;    // offset 0x3
};

static const FontKern *findkern16(int count, const FontKern *kern, unsigned int unicode) {
    for (int i = 0; i < count; i++) {
        if (kern[i].mUnicode == unicode) {
            return &kern[i];
        }
    }
    return 0;
}

static const FontKern *findkern12(int count, const FontKern *kern, unsigned int unicode, int index) {
    const FontKern *k = kern;
    for (int i = 0; i < count; i++, k++) {
        if (k->mUnicode == unicode && k->mIndex == index) {
            return k;
        }
    }
    return 0;
}

int Font::GetKern(const Glyph *glyph, wchar_t wch) const {
    int kern = 0;
    if (glyph->mNumKern != 0) {
        const FontKern *found;
        if (mFlags & 0x40000) {
            found = findkern16(glyph->mNumKern,
                               reinterpret_cast<const FontKern *>(reinterpret_cast<const char *>(this) + mKernTbl + 4) +
                                   glyph->mKernIndex,
                               wch);
        } else {
            found = findkern12(*reinterpret_cast<const int *>(reinterpret_cast<const char *>(this) + mKernTbl),
                               reinterpret_cast<const FontKern *>(reinterpret_cast<const char *>(this) + mKernTbl + 4),
                               wch, glyph->mUnicode);
        }
        if (found != 0) {
            kern = found->mKern;
        }
    }
    return kern;
}

} // namespace RealFontOld
