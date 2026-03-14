#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"

extern float ObjectSortLastZ;
extern FEPackage *ObjectSortRenderingPackage;
extern void GCDrawMovie(FEObject *obj, FERenderObject *renderObj);
extern void FinishedRenderingFEngLayer();
extern FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg);

unsigned int FEngColorToEpolyColor(FEColor c) {
    return (c.a / 2) | ((c.b / 2) << 8) | ((c.g / 2) << 16) | ((c.r / 2) << 24);
}

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

static void rotate_uvs(bVector2 *uvs, float angle_radians, float px, float py) {
    float half_width = (uvs[2].x - uvs[0].x) * 0.5f;
    float half_height = (uvs[2].y - uvs[0].y) * 0.5f;

    for (int i = 0; i < 4; i++) {
        uvs[i].x -= half_width;
        uvs[i].y -= half_height;
        uvs[i].x -= px;
        uvs[i].y -= py;
    }

    float sin_angle = bSin(bRadToAng(angle_radians));
    float cos_angle = bCos(bRadToAng(angle_radians));

    const float s2r = uvs[0].x;
    const float t2r = uvs[0].y;
    const float s3r = uvs[1].x;
    const float t3r = uvs[1].y;
    const float s4r = uvs[2].x;
    const float t4r = uvs[2].y;
    const float s5r = uvs[3].x;
    const float t5r = uvs[3].y;

    uvs[0].x = s2r * cos_angle + t2r * sin_angle + px + half_width;
    uvs[0].y = t2r * cos_angle - s2r * sin_angle + py + half_height;
    uvs[1].x = s3r * cos_angle + t3r * sin_angle + px + half_width;
    uvs[1].y = t3r * cos_angle - s3r * sin_angle + py + half_height;
    uvs[2].x = s4r * cos_angle + t4r * sin_angle + px + half_width;
    uvs[2].y = t4r * cos_angle - s4r * sin_angle + py + half_height;
    uvs[3].x = s5r * cos_angle + t5r * sin_angle + px + half_width;
    uvs[3].y = t5r * cos_angle - s5r * sin_angle + py + half_height;
}

void cFEngRender::AddToRenderList(FEObject *obj) {
    float z = reinterpret_cast<FEObjData *>(obj->pData)->Pos.z;
    if (obj->RenderContext != 0) {
        RenderContext *pctx = &RContexts[obj->RenderContext];
        z += pctx->matrix.v3.z;
    }
    bool visible = obj && !(obj->Flags & 1);
    if (visible) {
        if (z != ObjectSortLastZ) {
            ObjectSortLastZ = z;
            FinishedRenderingFEngLayer();
        }
        FEPackageRenderInfo *info = HACK_FEPkgMgr_GetPackageRenderInfo(ObjectSortRenderingPackage);
        RenderObject(obj, info);
    }
}

void cFEngRender::GenerateRenderContext(unsigned short ctx, FEObject *obj) {
    if (Highwater < ctx) {
        Highwater = ctx;
    }
    RenderContext *rc = &RContexts[ctx];
    FEColor color;
    MakeRenderMatrix(reinterpret_cast<FEObjData *>(obj->pData), &rc->matrix, color,
                     obj->RenderContext, 1.0f);
    int val;
    val = 0;
    if (color.b > 0) val = color.b;
    if (val > 0xff) val = 0xff;
    rc->b = static_cast<unsigned char>(val);
    val = 0;
    if (color.g > 0) val = color.g;
    if (val > 0xff) val = 0xff;
    rc->g = static_cast<unsigned char>(val);
    val = 0;
    if (color.r > 0) val = color.r;
    if (val > 0xff) val = 0xff;
    rc->r = static_cast<unsigned char>(val);
    val = 0;
    if (color.a > 0) val = color.a;
    if (val > 0xff) val = 0xff;
    rc->a = static_cast<unsigned char>(val);
    if (rc->group != reinterpret_cast<FEGroup *>(obj)) {
        rc->group = reinterpret_cast<FEGroup *>(obj);
        rc->clipObject = nullptr;
        FEMinNode *child =
            *reinterpret_cast<FEMinNode **>(reinterpret_cast<char *>(obj) + 0x60);
        for (; child; child = child->GetNext()) {
        }
    }
}