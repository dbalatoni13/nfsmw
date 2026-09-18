// librealfontz.a(oldfontchar.cpp)

#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"

namespace RealFontOld {

const Glyph *BSearch(wchar_t unicode, const Glyph *table, unsigned int count, int stride) {
    while (count != 0) {
        const Glyph *mid = reinterpret_cast<const Glyph *>(reinterpret_cast<const char *>(table) + (count >> 1) * stride);
        int diff = unicode - mid->mUnicode;
        if (diff == 0) {
            return mid;
        }
        if (diff > 0) {
            table = reinterpret_cast<const Glyph *>(reinterpret_cast<const char *>(mid) + stride);
            count--;
        }
        count >>= 1;
    }
    return 0;
}

} // namespace RealFontOld
