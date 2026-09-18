// TODO: this file isn't real.
#ifndef _PLAT_G
#define _PLAT_G

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

eLanguages GC_GetOSLanguage();

void GCDrawMovie();

// El ELF manda: PlatSetFirstMovieFrame__FP11TextureInfoPQ29RealShape5Shapeb.
namespace RealShape {
struct Shape;
}

void PlatSetFirstMovieFrame(struct TextureInfo *texture_info /* r3 */, RealShape::Shape *yuv_shape /* r30 */, bool isVP6Movie /* r31 */);

unsigned int RCMP_GetMaxFramesOutStanding();

void PlatFinishMovie();

#endif
