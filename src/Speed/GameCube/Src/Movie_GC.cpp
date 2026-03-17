#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

namespace RealShape {
struct Shape;
}

struct GCHW_VD {
    GCHW_VD(RealShape::Shape *yuvshp, bool isVP6Movie);
    ~GCHW_VD();
    void iDraw();
};

GCHW_VD *gGCVD;

void GCDrawMovie() {
    if (gGCVD) {
        gGCVD->iDraw();
    }
}

void PlatSetFirstMovieFrame(TextureInfo *texture_info, RealShape::Shape *yuv_shape, bool isVP6Movie) {
    if (!gGCVD) {
        gGCVD = new GCHW_VD(yuv_shape, isVP6Movie);
    }
}

void PlatFinishMovie() {
    if (gGCVD) {
        delete gGCVD;
        gGCVD = 0;
    }
}
