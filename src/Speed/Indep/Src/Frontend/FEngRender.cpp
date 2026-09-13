#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include <cfloat>

// TODO: import from Movie_GC.cpp
extern void GCDrawMovie();
// TODO: import from Platform_G.cpp
extern void FinishedRenderingFEngLayer();

float ObjectSortLastZ = -999999.0f;

uint32 FEngColorToEpolyColor(FEColor c) {
    return (c.a / 2) | ((c.b / 2) << 8) | ((c.g / 2) << 16) | ((c.r / 2) << 24);
}

uint32 next_power_of_2(uint32 number) {
    if (number) {
        uint32 shift = 2;
        number--;
        while (number >>= 1) {
            shift <<= 1;
        }
        return shift;
    }
    return 0;
}

FEClipInfo *cFEngRender::MakeRenderMatrix(FEObjData *pData, bMatrix4 *trans, FEColor &color, int32 GroupIndex, float extra_scale) {
    int do_pivot = 0;
    if (pData->Pivot.x != 0.0f || pData->Pivot.y != 0.0f || pData->Pivot.z != 0.0f) {
        do_pivot = 1;
    }

    int do_scale = 0;
    if (pData->Size.x != 1.0f || pData->Size.y != 1.0f || pData->Size.z != 1.0f || extra_scale != 1.0f) {
        do_scale = 1;
    }

    int do_rotate = 0;
    if (pData->Rot.x != 0.0f || pData->Rot.y != 0.0f || pData->Rot.z != 0.0f || pData->Rot.w != 1.0f) {
        do_rotate = 1;
    }

    int do_epoly = 0;
    if (true) { // TODO: ?
        do_epoly = 1;
    }

    bMatrix4 feng_to_epoly;
    bIdentity(&feng_to_epoly);
    feng_to_epoly.v3.x = pData->Pos.x;
    feng_to_epoly.v3.y = pData->Pos.y;
    feng_to_epoly.v3.z = pData->Pos.z;

    bMatrix4 pivot;
    bMatrix4 pivotm1;
    bMatrix4 scale;
    bMatrix4 rotate;

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

    if (do_scale) {
        bIdentity(&scale);
        scale.v0.x = pData->Size.x * extra_scale;
        scale.v1.y = pData->Size.y * extra_scale;
        scale.v2.z = pData->Size.z * extra_scale;
    }

    if (do_rotate) {
        bIdentity(&rotate);
        bQuaternion quat;
        quat.x = pData->Rot.x;
        quat.y = pData->Rot.y;
        quat.z = pData->Rot.z;
        quat.w = pData->Rot.w;
        quat.GetMatrix(&rotate);
    }
    if (do_epoly) {
        bIdentity(trans);
        bMulMatrix(trans, trans, &feng_to_epoly);
    }
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
        color.r = (static_cast<int>(Con.r) * pData->Col.r + 128) >> 8;
        color.g = (static_cast<int>(Con.g) * pData->Col.g + 128) >> 8;
        color.b = (static_cast<int>(Con.b) * pData->Col.b + 128) >> 8;
        color.a = (static_cast<int>(Con.a) * pData->Col.a + 128) >> 8;
        bMulMatrix(trans, &Con.matrix, trans);
        if (Con.clipObject != nullptr) {
            clip_info = &Con.clipInfo;
        }
    } else {
        color = pData->Col;
    }

    return clip_info;
}

void cFEngRender::RenderMovie(FEMovie *movie, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    if (gMoviePlayer != nullptr) {
        if (gMoviePlayer->GetStatus() == 5) {
            GCDrawMovie();
        }
    }
}

static void rotate_uvs(bVector2 *uvs, float angle_radians, float px, float py) {
    float half_width = (uvs[2].x - uvs[0].x) * 0.5f;
    float half_height = (uvs[2].y - uvs[0].y) * 0.5f;

    for (int i = 0; i < 4; i++) {
        uvs[i].x -= half_width;
        uvs[i].y -= half_height;
        uvs[i].x -= px;
        uvs[i].y -= py;
    }

    const float sin_angle = bSin(bRadToAng(angle_radians));
    const float cos_angle = bCos(bRadToAng(angle_radians));

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

// UNSOLVED
void cFEngRender::RenderMultiImage(FEMultiImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEMultiImageData *image_data = reinterpret_cast<FEMultiImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = SCREEN_WIDTH / 2;
    screen.v3.y = SCREEN_HEIGHT / 2;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(image_data, &trans, fe_color, image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);
    TextureInfo *texture_info_mask = GetTextureInfo(image->GetTexture(0), 1, 0);

    if (cached == nullptr) {
        cached = CreateCachedRender(image, texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    uint32 tw = texture_info->Width;
    uint32 th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    uint32 t2w = next_power_of_2(tw);
    uint32 t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    uint32 color = FEngColorToEpolyColor(fe_color);
    uint32 Colours[4];
    Colours[0] = color;
    Colours[1] = color;
    Colours[2] = color;
    Colours[3] = color;

    float s0 = image_data->UpperLeft.x * convertu;
    float s1 = image_data->LowerRight.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float t1 = image_data->LowerRight.y * convertv;

    uint32 tw_m = texture_info->Width;
    uint32 th_m = texture_info->Height;
    float ftw_m = static_cast<float>(tw_m);
    float fth_m = static_cast<float>(th_m);

    uint32 t2w_m = next_power_of_2(tw_m);
    uint32 t2h_m = next_power_of_2(th_m);
    float ft2w_m = static_cast<float>(t2w_m);
    float ft2h_m = static_cast<float>(t2h_m);

    float convertu_m = ftw_m / ft2w_m;
    float convertv_m = fth_m / ft2h_m;

    float ss2 = image_data->TopLeftUV[0].x * convertu_m;
    float ss3 = image_data->BottomRightUV[0].x * convertu_m;
    float sst2 = image_data->TopLeftUV[0].y * convertv_m;
    float sst3 = image_data->BottomRightUV[0].y * convertv_m;

    bVector2 uvs[4];
    uvs[0].x = ss2 * tw_m;
    uvs[0].y = sst2 * th_m;
    uvs[1].x = ss3 * tw_m;
    uvs[1].y = sst2 * th_m;
    uvs[2].x = ss3 * tw_m;
    uvs[2].y = sst3 * th_m;
    uvs[3].x = ss2 * tw_m;
    uvs[3].y = sst3 * th_m;

    rotate_uvs(uvs, bDegToRad(image_data->PivotRot.z), ftw_m * image_data->PivotRot.x - ftw_m * 0.5f, fth_m * image_data->PivotRot.y - fth_m * 0.5f);

    uvs[0].x /= ftw_m;
    uvs[0].y /= fth_m;
    uvs[1].x /= ftw_m;
    uvs[1].y /= fth_m;
    uvs[2].x /= ftw_m;
    uvs[2].y /= fth_m;
    uvs[3].x /= ftw_m;
    uvs[3].y /= fth_m;

    cached->SetTransform(&trans);
    cached->AddPolyWithRotatedMask(-0.5f, -0.5f, 0.5f, 0.5f, 1.0f, s0, t0, s1, t1, uvs[0].x, uvs[0].y, uvs[1].x, uvs[1].y, uvs[2].x, uvs[2].y,
                                   uvs[3].x, uvs[3].y, Colours, texture_info, texture_info_mask);
    cached->SetTexture(texture_info);
    cached->Render();
}

void cFEngRender::RenderImage(FEImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEImageData *image_data = reinterpret_cast<FEImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = SCREEN_WIDTH / 2;
    screen.v3.y = SCREEN_HEIGHT / 2;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(image_data, &trans, fe_color, image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);

    if (cached == nullptr) {
        cached = CreateCachedRender(image, texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    uint32 tw = texture_info->Width;
    uint32 th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    uint32 t2w = next_power_of_2(tw);
    uint32 t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    uint32 color = FEngColorToEpolyColor(fe_color);
    uint32 Colours[4];
    Colours[0] = color;
    Colours[1] = color;
    Colours[2] = color;
    Colours[3] = color;

    float s0 = image_data->UpperLeft.x * convertu;
    float s1 = image_data->LowerRight.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float t1 = image_data->LowerRight.y * convertv;

    cached->SetTransform(&trans);
    cached->AddPoly(-0.5f, -0.5f, 0.5f, 0.5f, 1.0f, s0, t0, s1, t1, Colours, clip_info, pkg_render_info);
    cached->SetTexture(texture_info);
    cached->Render();
}

void cFEngRender::RenderCBVImage(FEColoredImage *image, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEColoredImageData *image_data = reinterpret_cast<FEColoredImageData *>(image->pData);

    bMatrix4 screen;
    bIdentity(&screen);
    screen.v3.x = SCREEN_WIDTH / 2;
    screen.v3.y = SCREEN_HEIGHT / 2;
    screen.v3.z = 0.0f;

    bMatrix4 trans;
    FEColor fe_color;

    FEClipInfo *clip_info = MakeRenderMatrix(image_data, &trans, fe_color, image->RenderContext, 1.0f);

    bMulMatrix(&trans, &screen, &trans);

    TextureInfo *texture_info = GetTextureInfo(image->Handle, 1, 0);

    if (cached == nullptr) {
        cached = CreateCachedRender(image, texture_info);
    } else {
        cached->Clear(pkg_render_info);
    }

    uint32 tw = texture_info->Width;
    uint32 th = texture_info->Height;
    float ftw = static_cast<float>(tw);
    float fth = static_cast<float>(th);

    uint32 t2w = next_power_of_2(tw);
    uint32 t2h = next_power_of_2(th);
    float ft2w = static_cast<float>(t2w);
    float ft2h = static_cast<float>(t2h);

    float convertu = ftw / ft2w;
    float convertv = fth / ft2h;

    uint32 Colours[4];
    Colours[0] = FEngColorToEpolyColor(image_data->VertexColors[0]);
    Colours[1] = FEngColorToEpolyColor(image_data->VertexColors[1]);
    Colours[2] = FEngColorToEpolyColor(image_data->VertexColors[2]);
    Colours[3] = FEngColorToEpolyColor(image_data->VertexColors[3]);

    float s0 = image_data->UpperLeft.x * convertu;
    float s1 = image_data->LowerRight.x * convertu;
    float t0 = image_data->UpperLeft.y * convertv;
    float t1 = image_data->LowerRight.y * convertv;

    cached->SetTransform(&trans);
    cached->AddPoly(-0.5f, -0.5f, 0.5f, 0.5f, 1.0f, s0, t0, s1, t1, Colours, clip_info, pkg_render_info);
    cached->SetTexture(texture_info);
    cached->Render();
}

void cFEngRender::RenderString(FEString *string, FERenderObject *cached, FEPackageRenderInfo *pkg_render_info) {
    FEngFont *font = FindFont(string->Handle);
    if (font == nullptr) {
        return;
    }

    TextureInfo *texture_info = font->GetTextureInfo();
    if (texture_info != nullptr) {

        if (cached == nullptr) {
            cached = CreateCachedRender(string, texture_info);
        } else {
            cached->Clear(pkg_render_info);
        }

        float extra_scale = 1.0f;
        if ((GetCurrentLanguage() == eLANGUAGE_KOREAN || GetCurrentLanguage() == eLANGUAGE_CHINESE) &&
            string->Handle == STRINGHASH_FONT_CONDUITMDITCTT38BI) {
            extra_scale = 2.0f;
        }

        FEObjData *obj_data = reinterpret_cast<FEObjData *>(string->pData);
        int label_hash = string->GetLabelHash();
        const int16 *characters = nullptr;
        int16 localized_string_buffer[1024];

        if (!(string->Flags & 2)) {
            if (GetLocalizedWideString(localized_string_buffer, sizeof(localized_string_buffer), label_hash)) {
                characters = localized_string_buffer;
            }
        }
        if (characters == nullptr) {
            characters = string->GetString();
        }

        bMatrix4 screen;
        bIdentity(&screen);
        screen.v3.x = SCREEN_WIDTH / 2;
        screen.v3.y = SCREEN_HEIGHT / 2;
        screen.v3.z = 0.0f;

        bMatrix4 trans;
        FEColor fe_color;

        FEClipInfo *clip_info = MakeRenderMatrix(obj_data, &trans, fe_color, string->RenderContext, extra_scale);
        bMulMatrix(&trans, &screen, &trans);

        float fMaxWidth = static_cast<float>(string->MaxWidth);
        if (fMaxWidth == 0.0f) {
            fMaxWidth = FLT_MAX;
        }

        float LineWidth = font->GetLineWidth(characters, 0, 0, false);
        float fLineScale;

        if (string->MaxWidth != 0 && LineWidth > fMaxWidth && !(string->Format & FESTRING_FORMAT_WORDWRAP)) {
            fLineScale = fMaxWidth / LineWidth;
            bMatrix4 scale;
            bIdentity(&scale);
            scale.v0.x = fLineScale;
            bMulMatrix(&trans, &trans, &scale);
        }

        font->RenderString(fe_color, characters, string, &trans, cached, pkg_render_info);
    }
}

void cFEngRender::RenderModel(FEModel *model, FERenderObject *cached) {}

void cFEngRender::RenderObject(FEObject *object, FEPackageRenderInfo *pkg_render_info) {
    if (object->Flags & 8) {
        return;
    }
    if (object->Type == FE_Movie) {
        object->Flags |= FF_Dirty;
    }
    ProfileNode profile_node("TODO", 0); // TODO
    FERenderObject *cached = FindCachedRender(object);
    if ((cached != nullptr) && cached->IsReadyToRender() && !(object->Flags & FF_Dirty)) {
        cached->Render();
    } else {
        switch (object->Type) {
            case FE_Image:
                RenderImage(reinterpret_cast<FEImage *>(object), cached, pkg_render_info);
                break;
            case FE_ColoredImage:
                RenderCBVImage(reinterpret_cast<FEColoredImage *>(object), cached, pkg_render_info);
                break;
            case FE_String:
                RenderString(reinterpret_cast<FEString *>(object), cached, pkg_render_info);
                break;
            case FE_Model:
                RenderModel(reinterpret_cast<FEModel *>(object), cached);
                break;
            case FE_Movie:
                RenderMovie(reinterpret_cast<FEMovie *>(object), cached, pkg_render_info);
                break;
            case FE_MultiImage:
                RenderMultiImage(reinterpret_cast<FEMultiImage *>(object), cached, pkg_render_info);
                break;
        }
    }
}

void cFEngRender::RemoveCachedRender(FEObject *object, FEPackageRenderInfo *sp) {
    FERenderObject *cached = FindCachedRender(object);
    if (cached != nullptr) {
        object->Cached = nullptr;
        cached->Clear(sp);
        delete cached;
    }
}

FERenderObject *cFEngRender::FindCachedRender(FEObject *object) {
    return object->Cached;
}

FERenderObject *cFEngRender::CreateCachedRender(FEObject *object, TextureInfo *texture_info) {
    FERenderObject *ret = new FERenderObject(object, texture_info);
    object->Cached = ret;
    return ret;
}

cFEngRender::cFEngRender() : Highwater(0) {
    FERenderObject::Initialize();
    bMemSet(RContexts, 0, sizeof(RContexts));
}

RenderContext *cFEngRender::GetRenderContext(uint16 RenderContext) {
    return &RContexts[RenderContext];
}

void cFEngRender::GenerateRenderContext(uint16 GroupContext, FEObject *pObject) {
    if (Highwater < GroupContext) {
        Highwater = GroupContext;
    }
    FEColor color;
    RenderContext &Con = RContexts[GroupContext];
    MakeRenderMatrix(pObject->GetObjData(), &Con.matrix, color, pObject->RenderContext, 1.0f);

    Con.b = bClamp(color.b, 0, 0xff);
    Con.g = bClamp(color.g, 0, 0xff);
    Con.r = bClamp(color.r, 0, 0xff);
    Con.a = bClamp(color.a, 0, 0xff);

    if (Con.group != reinterpret_cast<FEGroup *>(pObject)) {
        Con.group = reinterpret_cast<FEGroup *>(pObject);
        Con.clipObject = nullptr;
        for (pObject = Con.group->GetFirstChild(); pObject != nullptr; pObject = pObject->GetNext()) {
        }
    }
}

void cFEngRender::PrepForPackage(FEPackage *pPackage) {
    ObjectSortLastZ = -999999.0f;
    extern FEPackage *ObjectSortRenderingPackage;
    ObjectSortRenderingPackage = pPackage;
}

void cFEngRender::PackageFinished(FEPackage *pPackage) {}

void cFEngRender::AddToRenderList(FEObject *pObject) {
    float obj_z = pObject->GetObjData()->Pos.z;
    if (pObject->RenderContext != 0) {
        RenderContext &context = RContexts[pObject->RenderContext];
        obj_z += context.matrix.v3.z;
    }
    FEPackageRenderInfo *ri;
    if (FEngIsVisible(pObject)) {
        if (obj_z != ObjectSortLastZ) {
            ObjectSortLastZ = obj_z;
            FinishedRenderingFEngLayer();
        }

        extern FEPackage *ObjectSortRenderingPackage;
        ri = HACK_FEPkgMgr_GetPackageRenderInfo(ObjectSortRenderingPackage);
        RenderObject(pObject, ri);
    }
}
