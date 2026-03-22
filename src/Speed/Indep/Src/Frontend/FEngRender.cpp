#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float ObjectSortLastZ;
extern FEPackage *ObjectSortRenderingPackage;
extern void GCDrawMovie(FEObject *obj, FERenderObject *renderObj);
extern void FinishedRenderingFEngLayer();
extern FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg);
extern TextureInfo *GetTextureInfo(unsigned int hash, int, int);


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

void cFEngRender::RenderImage(FEImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEImageData *image_data = reinterpret_cast<FEImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = 320.0f;
    screen.v3.y = 240.0f;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(
        reinterpret_cast<FEObjData *>(image_data), &trans, fe_color,
        image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);

    if (!cached) {
        cached = CreateCachedRender(reinterpret_cast<FEObject *>(image), texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    unsigned int tw = texture_info->Width;
    unsigned int th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    unsigned int t2w = next_power_of_2(tw);
    unsigned int t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    unsigned int color = FEngColorToEpolyColor(fe_color);
    unsigned int Colours[4];
    Colours[0] = color;
    Colours[1] = color;
    Colours[2] = color;
    Colours[3] = color;

    float s0 = image_data->UpperLeft.x * convertu;
    float s1 = image_data->LowerRight.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float t1 = image_data->LowerRight.y * convertv;

    cached->SetTransform(&trans);
    cached->AddPoly(-0.5f, -0.5f, 0.5f, 0.5f, 1.0f,
                    s0, t0, s1, t1,
                    Colours, clip_info, pkg_render_info);
    cached->SetTexture(texture_info);
    cached->Render();
}

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

void cFEngRender::RenderMultiImage(FEMultiImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEMultiImageData *image_data = reinterpret_cast<FEMultiImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = 320.0f;
    screen.v3.y = 240.0f;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(
        reinterpret_cast<FEObjData *>(image_data), &trans, fe_color,
        image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);
    TextureInfo *texture_info_mask = GetTextureInfo(image->GetTexture(0), 1, 0);

    if (!cached) {
        cached = CreateCachedRender(reinterpret_cast<FEObject *>(image), texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    unsigned int tw = texture_info->Width;
    unsigned int th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    unsigned int t2w = next_power_of_2(tw);
    unsigned int t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    unsigned int color = FEngColorToEpolyColor(fe_color);
    unsigned int Colours[4];
    Colours[0] = color;
    Colours[1] = color;
    Colours[2] = color;
    Colours[3] = color;

    float s0 = image_data->UpperLeft.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float s1 = image_data->LowerRight.x * convertu;
    float t1 = image_data->LowerRight.y * convertv;

    unsigned int tw_m = texture_info->Width;
    unsigned int th_m = texture_info->Height;
    float ftw_m = static_cast<float>(tw_m);
    float fth_m = static_cast<float>(th_m);

    unsigned int t2w_m = next_power_of_2(tw_m);
    unsigned int t2h_m = next_power_of_2(th_m);
    float ft2w_m = static_cast<float>(t2w_m);
    float ft2h_m = static_cast<float>(t2h_m);

    float convertu_m = ftw_m / ft2w_m;
    float convertv_m = fth_m / ft2h_m;

    float ss2 = image_data->TopLeftUV[0].x * convertu_m;
    float sst2 = image_data->TopLeftUV[0].y * convertv_m;
    float ss3 = image_data->BottomRightUV[0].x * convertu_m;
    float sst3 = image_data->BottomRightUV[0].y * convertv_m;

    bVector2 uvs[4];
    uvs[0].x = ss2 * ftw_m;
    uvs[0].y = sst2 * fth_m;
    uvs[1].x = ss3 * ftw_m;
    uvs[1].y = sst2 * fth_m;
    uvs[2].x = ss3 * ftw_m;
    uvs[2].y = sst3 * fth_m;
    uvs[3].x = ss2 * ftw_m;
    uvs[3].y = sst3 * fth_m;

    rotate_uvs(uvs,
               bDegToRad(image_data->PivotRot.z),
               image_data->PivotRot.x * ftw_m - ftw_m * 0.5f,
               image_data->PivotRot.y * fth_m - fth_m * 0.5f);

    uvs[0].x /= ftw_m;
    uvs[0].y /= fth_m;
    uvs[1].x /= ftw_m;
    uvs[1].y /= fth_m;
    uvs[2].x /= ftw_m;
    uvs[2].y /= fth_m;
    uvs[3].x /= ftw_m;
    uvs[3].y /= fth_m;

    cached->SetTransform(&trans);
    cached->AddPolyWithRotatedMask(
        -0.5f, -0.5f, 0.5f, 0.5f, 1.0f,
        s0, t0, s1, t1,
        uvs[0].x, uvs[0].y,
        uvs[1].x, uvs[1].y,
        uvs[2].x, uvs[2].y,
        uvs[3].x, uvs[3].y,
        Colours, texture_info, texture_info_mask);
    cached->SetTexture(texture_info);
    cached->Render();
}

void cFEngRender::RenderCBVImage(FEColoredImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEColoredImageData *image_data = reinterpret_cast<FEColoredImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = 320.0f;
    screen.v3.y = 240.0f;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(
        reinterpret_cast<FEObjData *>(image_data), &trans, fe_color,
        image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);

    if (!cached) {
        cached = CreateCachedRender(reinterpret_cast<FEObject *>(image), texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    unsigned int tw = texture_info->Width;
    unsigned int th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    unsigned int t2w = next_power_of_2(tw);
    unsigned int t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    unsigned int Colours[4];
    Colours[0] = FEngColorToEpolyColor(image_data->VertexColors[0]);
    Colours[1] = FEngColorToEpolyColor(image_data->VertexColors[1]);
    Colours[2] = FEngColorToEpolyColor(image_data->VertexColors[2]);
    Colours[3] = FEngColorToEpolyColor(image_data->VertexColors[3]);

    float s0 = image_data->UpperLeft.x * convertu;
    float s1 = image_data->LowerRight.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float t1 = image_data->LowerRight.y * convertv;

    cached->SetTransform(&trans);
    cached->AddPoly(-0.5f, -0.5f, 0.5f, 0.5f, 1.0f,
                    s0, t0, s1, t1,
                    Colours, clip_info, pkg_render_info);
    cached->SetTexture(texture_info);
    cached->Render();
}

void cFEngRender::RenderString(FEString *string, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEngFont *font = FindFont(string->Handle);
    if (!font) {
        return;
    }
    TextureInfo *texture_info = font->GetTextureInfo();
    if (!texture_info) {
        return;
    }

    if (!cached) {
        cached = CreateCachedRender(reinterpret_cast<FEObject *>(string), texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    float extra_scale = 1.0f;
    int lang = GetCurrentLanguage();
    if ((lang == 8 || GetCurrentLanguage() == 9) && string->Handle == 0x9583AA1A) {
        extra_scale = 2.0f;
    }

    const short *characters = nullptr;
    FEObjData *obj_data = reinterpret_cast<FEObjData *>(string->pData);
    short localized_string_buffer[1024];
    unsigned int labelHash = string->GetLabelHash();

    if (!(string->Flags & 2)) {
        if (GetLocalizedWideString(localized_string_buffer, 0x800, labelHash)) {
            characters = localized_string_buffer;
        }
    }
    if (!characters) {
        characters = string->GetString();
    }

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = 320.0f;
    screen.v3.y = 240.0f;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    MakeRenderMatrix(obj_data, &trans, fe_color, string->RenderContext, extra_scale);
    bMulMatrix(&trans, &screen, &trans);

    float fMaxWidth = static_cast<float>(string->MaxWidth);
    if (fMaxWidth == 0.0f) {
        fMaxWidth = 3.4028235e+38f;
    }

    float LineWidth = font->GetLineWidth(characters, 0, 0, false);

    if (string->MaxWidth != 0 && LineWidth > fMaxWidth && !(string->Format & 0x10)) {
        float fLineScale = fMaxWidth / LineWidth;
        bMatrix4 scale;
        bIdentity(&scale);
        scale.v0.x = fLineScale;
        bMulMatrix(&trans, &trans, &scale);
    }

    font->RenderString(fe_color, characters, string, &trans, cached, pkg_render_info);
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

void cFEngRender::RenderObject(FEObject *object, FEPackageRenderInfo *pkg_render_info) {
    if (object->Flags & 8) {
        return;
    }
    if (object->Type == 7) {
        object->Flags |= 0x2000000;
    }
    ProfileNode profile_node("TODO", 0);
    FERenderObject *cached = FindCachedRender(object);
    if (cached && cached->IsReadyToRender() && !(object->Flags & 0x2000000)) {
        cached->Render();
    } else {
        switch (object->Type) {
            case 1:
                RenderImage(reinterpret_cast<FEImage *>(object), cached, pkg_render_info);
                break;
            case 9:
                RenderCBVImage(reinterpret_cast<FEColoredImage *>(object), cached, pkg_render_info);
                break;
            case 2:
                RenderString(reinterpret_cast<FEString *>(object), cached, pkg_render_info);
                break;
            case 3:
                RenderModel(reinterpret_cast<FEModel *>(object), cached);
                break;
            case 7:
                RenderMovie(reinterpret_cast<FEMovie *>(object), cached, pkg_render_info);
                break;
            case 12:
                RenderMultiImage(reinterpret_cast<FEMultiImage *>(object), cached, pkg_render_info);
                break;
        }
    }
}
