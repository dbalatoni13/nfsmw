#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"

extern float ObjectSortLastZ;
extern FEPackage *ObjectSortRenderingPackage;
extern void GCDrawMovie(FEObject *obj, FERenderObject *renderObj);

unsigned int next_power_of_2(unsigned int number) {
    if (!number) {
        return 0;
    }
    number--;
    unsigned int shift = 2;
    while (number >>= 1) {
        shift <<= 1;
    }
    return shift;
}

void cFEngRender::RenderMovie(FEMovie *movie, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    if (gMoviePlayer) {
        if (static_cast<unsigned int>(gMoviePlayer->GetStatus() - 3) < 3) {
            GCDrawMovie(reinterpret_cast<FEObject *>(movie), cached);
        }
    }
}

void cFEngRender::RenderModel(FEModel* model, FERenderObject* renderObj) {}

FERenderObject *cFEngRender::FindCachedRender(FEObject *object) {
    return object->Cached;
}

RenderContext *cFEngRender::GetRenderContext(unsigned short ctx) {
    return &RContexts[ctx];
}

void cFEngRender::PrepForPackage(FEPackage *pPackage) {
    ObjectSortLastZ = -999999.0f;
    ObjectSortRenderingPackage = pPackage;
}

void cFEngRender::PackageFinished(FEPackage* pkg) {}