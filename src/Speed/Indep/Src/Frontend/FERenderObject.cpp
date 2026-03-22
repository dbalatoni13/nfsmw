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
    mPolyCount = 0;
    mulFlags &= ~2;
    mulNumTimesRendered = 0;
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
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
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
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
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
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
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
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
                    new_num_verts++;
                    bFlag = true;
                }
                pDst[new_num_verts] = pSrc[k];
                pDstUVs[new_num_verts] = pSrcUVs[k];
                pDstColors[new_num_verts] = pSrcColors[k];
                new_num_verts++;
            } else {
                if (bFlag) {
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[k] - pSrcUVs[last_vert];
                    pDstUVs[new_num_verts] *= t;
                    pDstUVs[new_num_verts].x += pSrcUVs[last_vert].x;
                    pDstUVs[new_num_verts].y += pSrcUVs[last_vert].y;
                    pDstColors[new_num_verts] = V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    pDstColors[new_num_verts] += pSrcColors[last_vert];
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
    bVector3 *pDst = nv;
    bVector2 *pDstUVs = nuv;
    bVector4 *pDstColors = ncolors;
    bVector3 *pSrc = v;
    bVector2 *pSrcUVs = uv;
    bVector4 *pSrcColors = colors;
    unsigned long num_verts;

    num_verts = ClipLeft(pDst, pDstUVs, pDstColors, pSrc, pSrcUVs, pSrcColors, 4, pClipInfo->constants[3]);
    if (!num_verts) return 0;
    num_verts = ClipTop(pSrc, pSrcUVs, pSrcColors, pDst, pDstUVs, pDstColors, num_verts, pClipInfo->constants[0]);
    if (!num_verts) return 0;
    num_verts = ClipRight(pDst, pDstUVs, pDstColors, pSrc, pSrcUVs, pSrcColors, num_verts, pClipInfo->constants[1]);
    if (!num_verts) return 0;
    num_verts = ClipBottom(pSrc, pSrcUVs, pSrcColors, pDst, pDstUVs, pDstColors, num_verts, pClipInfo->constants[2]);
    if (!num_verts) return 0;
    return num_verts;
}

unsigned int FERenderObject::ClipGeneral(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv,
                                         bVector4 *colors, bVector3 *nv, bVector2 *nuv,
                                         bVector4 *ncolors) {
    bVector3 *pSrc = v;
    bVector2 *pSrcUVs = uv;
    bVector4 *pSrcColors = colors;
    bVector3 *pDst = nv;
    bVector2 *pDstUVs = nuv;
    bVector4 *pDstColors = ncolors;
    unsigned long num_verts = 4;

    for (int i = 0; i < 4; i++) {
        bVector3 normal(pClipInfo->normals[i]);
        float constant = pClipInfo->constants[i];
        bool bFlag;
        unsigned long last_vert;
        unsigned long new_num_verts;

        last_vert = num_verts - 1;

        if (bDot(&normal, &pSrc[last_vert]) + constant > -0.5f) {
            pDst[0] = pSrc[last_vert];
            pDstUVs[0] = pSrcUVs[last_vert];
            pDstColors[0] = pSrcColors[last_vert];
            new_num_verts = 1;
            bFlag = true;
        } else {
            new_num_verts = 0;
            bFlag = false;
        }

        if (num_verts != 0) {
            for (unsigned long k = 0; k < num_verts; k++) {
                if (bDot(&normal, &pSrc[k]) + constant > -0.5f) {
                    if (!bFlag) {
                        bVector3 diff = pSrc[k] - pSrc[last_vert];
                        pDst[new_num_verts] = diff;
                        float t = -(bDot(&normal, &pSrc[last_vert]) + constant) / bDot(&normal, &diff);
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
                        pDst[new_num_verts] = diff;
                        float t = -(bDot(&normal, &pSrc[last_vert]) + constant) / bDot(&normal, &diff);
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

        void *pTmp;
        pTmp = pSrc; pSrc = pDst; pDst = static_cast<bVector3 *>(pTmp);
        pTmp = pSrcUVs; pSrcUVs = pDstUVs; pDstUVs = static_cast<bVector2 *>(pTmp);
        pTmp = pSrcColors; pSrcColors = pDstColors; pDstColors = static_cast<bVector4 *>(pTmp);

        num_verts = new_num_verts;
        if (!num_verts) return 0;
    }

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

void FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z,
                             float s0, float t0, float s1, float t1,
                             unsigned int *in_colors, FEClipInfo *pClipInfo,
                             FEPackageRenderInfo *pkg_render_info) {
    if (!pClipInfo) {
        AddPoly(x0, y0, x1, y1, z, s0, t0, s1, t1, in_colors, pkg_render_info);
        return;
    }

    bVector3 v[8];
    bVector2 uv[8];
    bVector4 colors[8];
    bVector3 nv[8];
    bVector2 nuv[8];
    bVector4 ncolors[8];
    unsigned int packed_colors[8];
    unsigned char *color_bytes = reinterpret_cast<unsigned char *>(in_colors);

    v[0].x = x0;
    v[0].y = y0;
    v[0].z = z;
    v[1].x = x1;
    v[1].y = y0;
    v[1].z = z;
    v[2].x = x1;
    v[2].y = y1;
    v[2].z = z;
    v[3].x = x0;
    v[3].y = y1;
    v[3].z = z;

    uv[0].x = s0;
    uv[0].y = t0;
    uv[1].x = s1;
    uv[1].y = t0;
    uv[2].x = s1;
    uv[2].y = t1;
    uv[3].x = s0;
    uv[3].y = t1;

    for (unsigned int i = 0; i < 4; i++) {
        colors[i].x = color_bytes[i * 4 + 0] * (1.0f / 255.0f);
        colors[i].y = color_bytes[i * 4 + 1] * (1.0f / 255.0f);
        colors[i].z = color_bytes[i * 4 + 2] * (1.0f / 255.0f);
        colors[i].w = color_bytes[i * 4 + 3] * (1.0f / 255.0f);
    }

    bMulMatrix(&v[0], &mstTransform, &v[0]);
    bMulMatrix(&v[1], &mstTransform, &v[1]);
    bMulMatrix(&v[2], &mstTransform, &v[2]);
    bMulMatrix(&v[3], &mstTransform, &v[3]);

    unsigned int num_verts = (pClipInfo->flags & 1) ? ClipAligned(pClipInfo, v, uv, colors, nv, nuv, ncolors)
                                                    : ClipGeneral(pClipInfo, v, uv, colors, nv, nuv, ncolors);
    if (!num_verts || num_verts == 2) {
        return;
    }

    for (unsigned int i = 0; i < num_verts; i++) {
        unsigned char *packed_bytes = reinterpret_cast<unsigned char *>(&packed_colors[i]);
        packed_bytes[0] = static_cast<unsigned char>(colors[i].x * 255.0f);
        packed_bytes[1] = static_cast<unsigned char>(colors[i].y * 255.0f);
        packed_bytes[2] = static_cast<unsigned char>(colors[i].z * 255.0f);
        packed_bytes[3] = static_cast<unsigned char>(colors[i].w * 255.0f);
    }

    for (unsigned int i = 0; i < num_verts - 2; i++) {
        FERenderEPoly *render = new FERenderEPoly();
        ePoly *pPoly = &render->EPoly;
        render->pTextureMask = nullptr;
        render->pTexture = nullptr;
        mobPolyList.AddTail(render);
        mPolyCount++;

        pPoly->Vertices[0] = v[0];
        pPoly->Vertices[1] = v[i + 1];
        pPoly->Vertices[2] = v[i + 2];
        pPoly->Vertices[3] = v[i + 2];

        pPoly->UVs[0][0] = uv[0].x;
        pPoly->UVs[0][1] = uv[0].y;
        pPoly->UVs[0][2] = uv[i + 1].x;
        pPoly->UVs[0][3] = uv[i + 1].y;
        pPoly->UVs[1][0] = uv[i + 2].x;
        pPoly->UVs[1][1] = uv[i + 2].y;
        pPoly->UVs[1][2] = uv[i + 2].x;
        pPoly->UVs[1][3] = uv[i + 2].y;

        reinterpret_cast<unsigned int *>(pPoly->Colours)[0] = packed_colors[0];
        reinterpret_cast<unsigned int *>(pPoly->Colours)[1] = packed_colors[i + 1];
        reinterpret_cast<unsigned int *>(pPoly->Colours)[2] = packed_colors[i + 2];
        reinterpret_cast<unsigned int *>(pPoly->Colours)[3] = packed_colors[i + 2];

        pPoly->SetFlailer(1);
        pPoly->SetFlags(1);
    }
}

void FERenderObject::AddPolyWithRotatedMask(float x0, float y0, float x1, float y1, float z,
                                             float s0, float t0, float s1, float t1,
                                             float ms0, float mt0, float ms1, float mt1,
                                             float ms2, float mt2, float ms3, float mt3,
                                             unsigned int *colors, TextureInfo *texture,
                                             TextureInfo *textureMask) {
    FERenderEPoly *render = new FERenderEPoly();
    ePoly *pPoly = &render->EPoly;
    render->pTexture = texture;
    render->pTextureMask = textureMask;
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

    pPoly->SetFlailer(5);

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

    pPoly->UVs[2][0] = ms0;
    pPoly->UVs[2][1] = mt0;
    pPoly->UVs[2][2] = ms1;
    pPoly->UVs[2][3] = mt1;
    pPoly->UVs[3][0] = ms2;
    pPoly->UVs[3][1] = mt2;
    pPoly->UVs[3][2] = ms3;
    pPoly->UVs[3][3] = mt3;

    reinterpret_cast<unsigned int *>(pPoly->Colours)[0] = colors[0];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[1] = colors[1];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[2] = colors[2];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[3] = colors[3];

    pPoly->SetFlags(1);
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
