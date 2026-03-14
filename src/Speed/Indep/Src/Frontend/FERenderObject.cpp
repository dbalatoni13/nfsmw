#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *FERenderEPolySlotPool;
extern SlotPool *FERenderEPolySlotPoolOverflow;
extern void PSMTX44Identity(float *mtx);
extern void FEBeginBatchRender(eView *view, int polyCount);
extern void FEEndBatchRender(eView *view);
extern void FERender(eView *view, ePoly *poly, TextureInfo *tex, TextureInfo *mask, int param);
extern void FERender(eView *view, ePoly *poly, TextureInfo *tex, int param);

extern unsigned int ClipTop(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                            bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                            unsigned int num_verts, float value);
extern unsigned int ClipBottom(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                               bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                               unsigned int num_verts, float value);
extern unsigned int ClipLeft(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                             bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                             unsigned int num_verts, float value);
extern unsigned int ClipRight(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                              bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                              unsigned int num_verts, float value);

void FERenderObject::Initialize() {
    mpobFERenderObjectSlotPool = bNewSlotPool(0x64, 0x180, "FERenderObjectSlotPool", 0);
    FERenderEPolySlotPool = bNewSlotPool(0xA4, 0x348, "FERenderEPolySlotPool", 0);
}

FERenderObject::FERenderObject(FEObject *obj, TextureInfo *tex) {
    mpobOwner = obj;
    mpobTexture = tex;
    mobPolyList.InitList();
    mulNumTimesRendered = 0;
    mulFlags = 0;
    mPolyCount = 0;
    PSMTX44Identity(reinterpret_cast<float *>(&mstTransform));
}

FERenderObject::~FERenderObject() {
    while (mobPolyList.GetHead() != mobPolyList.EndOfList()) {
        FERenderEPoly *poly = mobPolyList.GetHead();
        poly->Remove();
        delete poly;
    }
}

void *FERenderEPoly::operator new(unsigned int) {
    if (FERenderEPolySlotPool->NumAllocatedSlots != FERenderEPolySlotPool->TotalNumSlots) {
        return FERenderEPolySlotPool->FastMalloc();
    }
    if (!FERenderEPolySlotPoolOverflow) {
        FERenderEPolySlotPoolOverflow = bNewSlotPool(0xA4, 0x200, "FERenderEPolySlotPoolOverflow", 0);
        FERenderEPolySlotPoolOverflow->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    }
    return FERenderEPolySlotPoolOverflow->Malloc();
}

void FERenderEPoly::operator delete(void *p) {
    if (FERenderEPolySlotPool->GetSlotNumber(p) >= 0) {
        FERenderEPolySlotPool->Free(p);
    } else {
        FERenderEPolySlotPoolOverflow->Free(p);
        if (FERenderEPolySlotPoolOverflow->NumAllocatedSlots == 0) {
            bDeleteSlotPool(FERenderEPolySlotPoolOverflow);
            FERenderEPolySlotPoolOverflow = nullptr;
        }
    }
}

bVector4 V4Mult(const bVector4 &v, float d) {
    return bVector4(v.x * d, v.y * d, v.z * d, v.w * d);
}

void FERenderObject::SetTransform(bMatrix4 *pMatrix) {
    bMemCpy(&mstTransform, pMatrix, sizeof(bMatrix4));
}

void FERenderObject::Render() {
    eView *view = &eViews[0];
    FEBeginBatchRender(view, mPolyCount);
    FERenderEPoly *render = mobPolyList.GetHead();
    while (render != mobPolyList.EndOfList()) {
        TextureInfo *texture = render->pTexture;
        if (!texture) {
            texture = mpobTexture;
        }
        if (render->EPoly.GetFlags() & 0x4) {
            FERender(view, &render->EPoly, texture, render->pTextureMask, 0);
        } else {
            FERender(view, &render->EPoly, texture, 0);
        }
        render = render->GetNext();
    }
    FEEndBatchRender(view);
    ReadyToRender();
}

void FERenderObject::Clear(FEPackageRenderInfo *pkg_render_info) {
    while (mobPolyList.GetHead() != mobPolyList.EndOfList()) {
        FERenderEPoly *render = mobPolyList.GetHead();
        render->Remove();
        delete render;
    }
    mulNumTimesRendered = 0;
    mulFlags &= ~2;
    mPolyCount = 0;
}

unsigned int ClipLeft(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                     bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                     unsigned int num_verts, float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    unsigned long last_vert;

    last_vert = num_verts - 1;

    if (pSrc[last_vert].x >= value) {
        pDst[0] = pSrc[last_vert];
        pDstUVs[0] = pSrcUVs[last_vert];
        pDstColors[0] = pSrcColors[last_vert];
        new_num_verts = 1;
        bFlag = true;
    } else {
        bFlag = false;
    }

    if (num_verts != 0) {
        for (unsigned long k = 0; k < num_verts; k++) {
            if (pSrc[k].x >= value) {
                if (!bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].x) / diff.x;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].x) / diff.x;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = false;
                }
            }
            last_vert = k;
        }
    }

    return new_num_verts;
}

unsigned int ClipTop(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                     bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                     unsigned int num_verts, float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    unsigned long last_vert;

    last_vert = num_verts - 1;

    if (pSrc[last_vert].y >= value) {
        pDst[0] = pSrc[last_vert];
        pDstUVs[0] = pSrcUVs[last_vert];
        pDstColors[0] = pSrcColors[last_vert];
        new_num_verts = 1;
        bFlag = true;
    } else {
        bFlag = false;
    }

    if (num_verts != 0) {
        for (unsigned long k = 0; k < num_verts; k++) {
            if (pSrc[k].y >= value) {
                if (!bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].y) / diff.y;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].y) / diff.y;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = false;
                }
            }
            last_vert = k;
        }
    }

    return new_num_verts;
}

unsigned int ClipRight(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                       bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                       unsigned int num_verts, float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    unsigned long last_vert;

    last_vert = num_verts - 1;

    if (pSrc[last_vert].x <= value) {
        pDst[0] = pSrc[last_vert];
        pDstUVs[0] = pSrcUVs[last_vert];
        pDstColors[0] = pSrcColors[last_vert];
        new_num_verts = 1;
        bFlag = true;
    } else {
        bFlag = false;
    }

    if (num_verts != 0) {
        for (unsigned long k = 0; k < num_verts; k++) {
            if (pSrc[k].x <= value) {
                if (!bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].x) / diff.x;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].x) / diff.x;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = false;
                }
            }
            last_vert = k;
        }
    }

    return new_num_verts;
}

unsigned int ClipBottom(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors,
                        bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
                        unsigned int num_verts, float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    unsigned long last_vert;

    last_vert = num_verts - 1;

    if (pSrc[last_vert].y <= value) {
        pDst[0] = pSrc[last_vert];
        pDstUVs[0] = pSrcUVs[last_vert];
        pDstColors[0] = pSrcColors[last_vert];
        new_num_verts = 1;
        bFlag = true;
    } else {
        bFlag = false;
    }

    if (num_verts != 0) {
        for (unsigned long k = 0; k < num_verts; k++) {
            if (pSrc[k].y <= value) {
                if (!bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].y) / diff.y;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    bVector3 diff = pSrc[k] - pSrc[last_vert];
                    float t = (value - pSrc[last_vert].y) / diff.y;
                    pDst[new_num_verts] = diff;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = (pSrcUVs[k] - pSrcUVs[last_vert]) * t + pSrcUVs[last_vert];
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t) + pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = false;
                }
            }
            last_vert = k;
        }
    }

    return new_num_verts;
}

unsigned int FERenderObject::ClipAligned(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv,
                                         bVector4 *colors, bVector3 *nv, bVector2 *nuv,
                                         bVector4 *ncolors) {
    unsigned int num_verts;
    num_verts = ClipLeft(nv, nuv, ncolors, v, uv, colors, 4, pClipInfo->constants[3]);
    if (!num_verts) return 0;
    num_verts = ClipTop(v, uv, colors, nv, nuv, ncolors, num_verts, pClipInfo->constants[0]);
    if (!num_verts) return 0;
    num_verts = ClipRight(nv, nuv, ncolors, v, uv, colors, num_verts, pClipInfo->constants[1]);
    if (!num_verts) return 0;
    num_verts = ClipBottom(v, uv, colors, nv, nuv, ncolors, num_verts, pClipInfo->constants[2]);
    if (!num_verts) return 0;
    return num_verts;
}

FERenderEPoly *FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z,
                                       float s0, float t0, float s1, float t1,
                                       unsigned int *colors, FEPackageRenderInfo *pkg_render_info) {
    FERenderEPoly *render = new FERenderEPoly();
    ePoly *pPoly = &render->EPoly;
    render->pTextureMask = nullptr;
    render->pTexture = nullptr;
    mobPolyList.AddTail(render);
    mPolyCount++;

    pPoly->Vertices[0].x = x0;
    pPoly->Vertices[0].y = y0;
    pPoly->Vertices[0].z = z;
    pPoly->Vertices[1].x = x1;
    pPoly->Vertices[1].y = y0;
    pPoly->Vertices[1].z = z;
    pPoly->Vertices[2].x = x1;
    pPoly->Vertices[2].y = y1;
    pPoly->Vertices[2].z = z;
    pPoly->Vertices[3].x = x0;
    pPoly->Vertices[3].y = y1;
    pPoly->Vertices[3].z = z;

    bMulMatrix(&pPoly->Vertices[0], &mstTransform, &pPoly->Vertices[0]);
    bMulMatrix(&pPoly->Vertices[1], &mstTransform, &pPoly->Vertices[1]);
    bMulMatrix(&pPoly->Vertices[2], &mstTransform, &pPoly->Vertices[2]);
    bMulMatrix(&pPoly->Vertices[3], &mstTransform, &pPoly->Vertices[3]);

    pPoly->Vertices[0].z = z;
    pPoly->Vertices[1].z = z;
    pPoly->Vertices[2].z = z;
    pPoly->Vertices[3].z = z;

    pPoly->UVs[0][0] = s0;
    pPoly->UVs[0][1] = t0;
    pPoly->UVs[0][2] = s1;
    pPoly->UVs[0][3] = t0;
    pPoly->UVs[1][0] = s1;
    pPoly->UVs[1][1] = t1;
    pPoly->UVs[1][2] = s0;
    pPoly->UVs[1][3] = t1;

    reinterpret_cast<unsigned int *>(pPoly->Colours)[0] = colors[0];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[1] = colors[1];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[2] = colors[2];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[3] = colors[3];

    pPoly->SetFlailer(1);
    pPoly->SetFlags(1);

    return render;
}

void FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z,
                             float s0, float t0, float s1, float t1,
                             unsigned int *colors, TextureInfo *texture,
                             FEPackageRenderInfo *pkg_render_info) {
    FERenderEPoly *render = AddPoly(x0, y0, x1, y1, z, s0, t0, s1, t1, colors, pkg_render_info);
    if (render) {
        render->pTexture = texture;
    }
}

extern void *bOMalloc(SlotPool *pool);
extern void bMemSet(void *dst, int val, unsigned int size);

cFEngRender::cFEngRender() {
    Highwater = 0;
    FERenderObject::Initialize();
    bMemSet(RContexts, 0, sizeof(RContexts));
}

FERenderObject *cFEngRender::CreateCachedRender(FEObject *object, TextureInfo *texture_info) {
    void *mem = bOMalloc(mpobFERenderObjectSlotPool);
    FERenderObject *ret = new (mem) FERenderObject(object, texture_info);
    object->Cached = ret;
    return ret;
}

void cFEngRender::RemoveCachedRender(FEObject *object, FEPackageRenderInfo *sp) {
    FERenderObject *cached = FindCachedRender(object);
    if (cached) {
        object->Cached = nullptr;
        cached->Clear(sp);
        delete cached;
    }
}

void cFEngRender::RenderObject(FEObject *object, FEPackageRenderInfo *pkg_render_info) {
    if (object->Flags & 8) {
        return;
    }
    if (object->Type == 7) {
        object->Flags |= 0x2000000;
    }
    FERenderObject *cached = FindCachedRender(object);
    if (cached && (cached->mulFlags & 2) && !(object->Flags & 0x2000000)) {
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