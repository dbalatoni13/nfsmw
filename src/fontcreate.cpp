// librealfontz.a(oldfontcreate.cpp)

#include "Speed/Indep/Src/Frontend/RealFontOld.hpp"

namespace RealFontOld {

// _11RealFontOld.gFontDriver = .sdata:0x804FF658, scope global. El
// almacenamiento lo pone realcore/systemvars.cpp; aqui solo se declara.
extern const FontDriver *gFontDriver;

Font *Font::Create(void *data) {
    Font *font = reinterpret_cast<Font *>(data);
    if (gFontDriver != 0 && gFontDriver->mCreateFont != 0) {
        gFontDriver->mCreateFont(font);
    }
    return font;
}

void Font::Destroy(Font *font) {
    if (gFontDriver != 0 && gFontDriver->mDestroyFont != 0) {
        gFontDriver->mDestroyFont(font);
    }
}

} // namespace RealFontOld
