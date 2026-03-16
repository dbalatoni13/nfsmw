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

FEClipInfo *cFEngRender::MakeRenderMatrix(FEObjData *pData, bMatrix4 *trans, FEColor &color,
                                          int GroupIndex, float extra_scale) {
    int do_pivot = 0;
    if (pData->Pivot.x != 0.0f || pData->Pivot.y != 0.0f || pData->Pivot.z != 0.0f) {
        do_pivot = 1;
    }

    int do_scale = 0;
    if (pData->Size.x != 1.0f || pData->Size.y != 1.0f || pData->Size.z != 1.0f ||
        extra_scale != 1.0f) {
        do_scale = 1;
    }

    int do_rotate = 0;
    if (pData->Rot.x != 0.0f || pData->Rot.y != 0.0f || pData->Rot.z != 0.0f ||
        pData->Rot.w != 1.0f) {
        do_rotate = 1;
    }

    bMatrix4 feng_to_epoly;
    bIdentity(&feng_to_epoly);
    feng_to_epoly.v3.x = pData->Pos.x;
    feng_to_epoly.v3.y = pData->Pos.y;
    feng_to_epoly.v3.z = pData->Pos.z;

    bMatrix4 pivot;
    bMatrix4 pivotm1;
    if (do_pivot) {
        bIdentity(&pivot);
        bIdentity(&pivotm1);
        pivotm1.v3.x = -pData->Pivot.x;
        pivotm1.v3.y = -pData->Pivot.y;
        pivotm1.v3.z = -pData->Pivot.z;
        pivot.v3.x = pData->Pivot.x;
        pivot.v3.y = pData->Pivot.y;
        pivot.v3.z = pData->Pivot.z;
    }

    bMatrix4 scale;
    if (do_scale) {
        bIdentity(&scale);
        scale.v0.x = pData->Size.x * extra_scale;
        scale.v1.y = pData->Size.y * extra_scale;
        scale.v2.z = pData->Size.z * extra_scale;
    }

    bMatrix4 rotate;
    if (do_rotate) {
        bIdentity(&rotate);
        bQuaternion quat;
        quat.x = pData->Rot.x;
        quat.y = pData->Rot.y;
        quat.z = pData->Rot.z;
        quat.w = pData->Rot.w;
        quat.GetMatrix(&rotate);
    }

    bIdentity(trans);
    bMulMatrix(trans, trans, &feng_to_epoly);
    if (do_pivot) {
        bMulMatrix(trans, trans, &pivot);
    }
    if (do_rotate) {
        bMulMatrix(trans, trans, &rotate);
    }
    if (do_pivot) {
        bMulMatrix(trans, trans, &pivotm1);
    }
    if (do_scale) {
        bMulMatrix(trans, trans, &scale);
    }

    FEClipInfo *clip_info = nullptr;
    if (GroupIndex != 0) {
        RenderContext &Con = RContexts[GroupIndex];
        color.r = (static_cast<int>(Con.r) * pData->Col.r + 0x80) >> 8;
        color.g = (static_cast<int>(Con.g) * pData->Col.g + 0x80) >> 8;
        color.b = (static_cast<int>(Con.b) * pData->Col.b + 0x80) >> 8;
        color.a = (static_cast<int>(Con.a) * pData->Col.a + 0x80) >> 8;
        bMulMatrix(trans, &Con.matrix, trans);
        if (Con.clipObject) {
            clip_info = &Con.clipInfo;
        }
    } else {
        color = pData->Col;
    }

    return clip_info;
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