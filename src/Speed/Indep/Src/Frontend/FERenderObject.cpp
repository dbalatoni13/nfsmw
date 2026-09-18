#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

SlotPool *mpobFERenderObjectSlotPool = nullptr;
SlotPool *FERenderEPolySlotPool = nullptr;
SlotPool *FERenderEPolySlotPoolOverflow = nullptr;
// extern void PSMTX44Identity(float *mtx);
// extern void FEBeginBatchRender(eView *view, int polyCount);
// extern void FEEndBatchRender(eView *view);
// extern void FERender(eView *view, ePoly *poly, TextureInfo *tex, TextureInfo *mask, int param);
// extern void FERender(eView *view, ePoly *poly, TextureInfo *tex, int param);

// extern unsigned int ClipTop(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
//                             unsigned int num_verts, float value);
// extern unsigned int ClipBottom(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
//                                unsigned int num_verts, float value);
// extern unsigned int ClipLeft(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
//                              unsigned int num_verts, float value);
// extern unsigned int ClipRight(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors,
//                               unsigned int num_verts, float value);


void FERenderObject::Initialize() {
    mpobFERenderObjectSlotPool = bNewSlotPool(0x64, 0x180, "FERenderObjectSlotPool", 0);
    FERenderEPolySlotPool = bNewSlotPool(0xA4, 0x348, "FERenderEPolySlotPool", 0);
}

// STRIPPED
void FERenderObject::Terminate() {}

FERenderObject::FERenderObject(FEObject *pOwner, TextureInfo *pTexture)
    : mpobOwner(pOwner), mpobTexture(pTexture), mobPolyList(), mulNumTimesRendered(0), mulFlags(0), mPolyCount(0) {
    bIdentity(&mstTransform);
}

// El bucle que borra los polys sale del destructor implicito del miembro
// bTList<FERenderEPoly> (bList.hpp:203): el cuerpo del original esta vacio.
FERenderObject::~FERenderObject() {}

void FERenderObject::SetTransform(bMatrix4 *pMatrix) {
    bMemCpy(&mstTransform, pMatrix, sizeof(bMatrix4));
}

void *FERenderEPoly::operator new(size_t size) {
    if (!FERenderEPolySlotPool->IsFull()) {
        return FERenderEPolySlotPool->FastMalloc();
    }
    if (FERenderEPolySlotPoolOverflow == nullptr) {
        FERenderEPolySlotPoolOverflow = bNewSlotPool(sizeof(FERenderEPoly), 0x200, "FERenderEPolySlotPoolOverflow", 0);
        FERenderEPolySlotPoolOverflow->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    }
    return FERenderEPolySlotPoolOverflow->Malloc();
}

void FERenderEPoly::operator delete(void *ptr) {
    if (FERenderEPolySlotPool->GetSlotNumber(ptr) >= 0) {
        FERenderEPolySlotPool->Free(ptr);
    } else {
        FERenderEPolySlotPoolOverflow->Free(ptr);
        if (FERenderEPolySlotPoolOverflow->IsEmpty()) {
            bDeleteSlotPool(FERenderEPolySlotPoolOverflow);
            FERenderEPolySlotPoolOverflow = nullptr;
        }
    }
}

FERenderEPoly *FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *colors,
                                       FEPackageRenderInfo *pkg_render_info) {
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
    pPoly->SetFlags(EPOLY_APPLYASPECT);

    return render;
}

void FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *colors,
                             TextureInfo *texture, FEPackageRenderInfo *pkg_render_info) {
    FERenderEPoly *render = AddPoly(x0, y0, x1, y1, z, s0, t0, s1, t1, colors, pkg_render_info);
    if (render) {
        render->pTexture = texture;
    }
}

void FERenderObject::AddPolyWithRotatedMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2,
                                            float t2, float s3, float t3, float s4, float t4, float s5, float t5, uint32 *colors,
                                            TextureInfo *texture, TextureInfo *textureMask) {
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

    pPoly->SetFlags(5);

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
    pPoly->UVsMask[0][0] = s2;
    pPoly->UVsMask[0][1] = t2;
    pPoly->UVsMask[0][2] = s3;
    pPoly->UVsMask[0][3] = t3;
    pPoly->UVsMask[1][0] = s4;
    pPoly->UVsMask[1][1] = t4;
    pPoly->UVsMask[1][2] = s5;
    pPoly->UVsMask[1][3] = t5;

    reinterpret_cast<unsigned int *>(pPoly->Colours)[0] = colors[0];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[1] = colors[1];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[2] = colors[2];
    reinterpret_cast<unsigned int *>(pPoly->Colours)[3] = colors[3];

    pPoly->SetFlailer(1);
}

// STRIPPED
void FERenderObject::AddPolyWithMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2, float t2,
                                     float s3, float t3, uint32 *colors, TextureInfo *texture, TextureInfo *textureMask) {}

bVector4 V4Mult(const bVector4 &v, float d) {
    return bVector4(v.x * d, v.y * d, v.z * d, v.w * d);
}

void FERenderObject::AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *in_colors,
                             FEClipInfo *pClipInfo, FEPackageRenderInfo *pkg_render_info) {
    if (!pClipInfo) {
        AddPoly(x0, y0, x1, y1, z, s0, t0, s1, t1, in_colors, pkg_render_info);
        return;
    }

    bVector3 v[8];
    bVector2 uv[8];
    bVector4 colors[8];
    u32 i;
    unsigned char *pcColors = reinterpret_cast<unsigned char *>(in_colors);

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

    for (i = 0; i < 4; i++) {
        colors[i] = bVector4(static_cast<unsigned int>(pcColors[i * 4 + 0]) * (1.0f / 255.0f),
                             static_cast<unsigned int>(pcColors[i * 4 + 1]) * (1.0f / 255.0f),
                             static_cast<unsigned int>(pcColors[i * 4 + 2]) * (1.0f / 255.0f),
                             static_cast<unsigned int>(pcColors[i * 4 + 3]) * (1.0f / 255.0f));
    }

    bMulMatrix(&v[0], &mstTransform, &v[0]);
    bMulMatrix(&v[1], &mstTransform, &v[1]);
    bMulMatrix(&v[2], &mstTransform, &v[2]);
    bMulMatrix(&v[3], &mstTransform, &v[3]);

    // Bucle de 4 vueltas con cuerpo vacio: el objetivo lo emite (li r31,0 / addi /
    // cmplwi 3 / ble) entre los bMulMatrix y la declaracion de nv[]. GCC 2.9 no
    // borra un bucle vacio cuyo indice sigue vivo despues.
    for (i = 0; i < 4; i++) {
    }

    bVector3 nv[8];
    bVector2 nuv[8];
    bVector4 ncolors[8];
    u32 num_verts;
    unsigned int new_colors[8];

    if (pClipInfo->flags & 1) {
        num_verts = ClipAligned(pClipInfo, v, uv, colors, nv, nuv, ncolors);
    } else {
        num_verts = ClipGeneral(pClipInfo, v, uv, colors, nv, nuv, ncolors);
    }

    if (!num_verts) {
        return;
    }

    pcColors = reinterpret_cast<unsigned char *>(new_colors);

    for (i = 0; i < num_verts; i++) {
        pcColors[i * 4 + 0] = static_cast<unsigned char>(ncolors[i].x * 255.0f);
        pcColors[i * 4 + 1] = static_cast<unsigned char>(ncolors[i].y * 255.0f);
        pcColors[i * 4 + 2] = static_cast<unsigned char>(ncolors[i].z * 255.0f);
        pcColors[i * 4 + 3] = static_cast<unsigned char>(ncolors[i].w * 255.0f);
    }

    for (i = 0; i < num_verts - 2; i++) {
        FERenderEPoly *render = new FERenderEPoly();
        ePoly *pPoly = &render->EPoly;
        render->pTextureMask = nullptr;
        render->pTexture = nullptr;
        mobPolyList.AddTail(render);
        mPolyCount++;

        pPoly->Vertices[0] = v[0];

        pPoly->UVs[0][0] = uv[0].x;
        pPoly->UVs[0][1] = uv[0].y;
        reinterpret_cast<unsigned int *>(pPoly->Colours)[0] = new_colors[0];

        pPoly->Vertices[1] = v[i + 1];

        pPoly->UVs[0][2] = uv[i + 1].x;
        pPoly->UVs[0][3] = uv[i + 1].y;
        reinterpret_cast<unsigned int *>(pPoly->Colours)[1] = new_colors[i + 1];

        pPoly->Vertices[2] = v[i + 2];

        pPoly->UVs[1][0] = uv[i + 2].x;
        pPoly->UVs[1][1] = uv[i + 2].y;
        reinterpret_cast<unsigned int *>(pPoly->Colours)[2] = new_colors[i + 2];

        // Copiar desde pPoly (no desde v[]/uv[]) es lo que el objetivo hace: los
        // desplazamientos son constantes sobre la MISMA base, asi que CSE devuelve
        // el valor recien guardado y no vuelve a leer de la pila. Leyendo de v[] /
        // uv[] las escrituras por puntero invalidan la entrada y salen 6 cargas de
        // mas. El tercer destino cae en UVsMask[0][0] (ePoly+0x60), justo detras de
        // UVs[1][3]: el original escribe ahi.
        pPoly->Vertices[3] = pPoly->Vertices[2];

        pPoly->UVs[1][2] = pPoly->UVs[1][0];
        pPoly->UVs[1][3] = pPoly->UVs[1][1];
        pPoly->UVsMask[0][0] = pPoly->UVs[1][0];
        reinterpret_cast<unsigned int *>(pPoly->Colours)[3] = new_colors[i + 2];

        pPoly->SetFlailer(1);
        pPoly->SetFlags(1);
    }
}

// Depende de que eViewPlatInterface::FEBeginBatchRender/FEEndBatchRender NO sean
// `static` (EcstasyData.hpp): el objetivo pasa `this` en r3. Con `static` GCC mete
// mPolyCount en r3, no emite el `this` y esta funcion baja a 90,87%.
void FERenderObject::Render() {
    eView *view = &eViews[0];
    view->FEBeginBatchRender(mPolyCount);
    FERenderEPoly *render = mobPolyList.GetHead();
    while (render != mobPolyList.EndOfList()) {
        TextureInfo *texture = mpobTexture;
        if (render->pTexture) {
            texture = render->pTexture;
        }
        if (render->EPoly.GetFlags() & 0x4) {
            view->FERender(&render->EPoly, texture, render->pTextureMask, 0);
        } else {
            view->FERender(&render->EPoly, texture, 0);
        }
        render = render->GetNext();
    }
    view->FEEndBatchRender();
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

uint32 FERenderObject::ClipGeneral(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv,
                                   bVector4 *ncolors) {
    bVector3 *pDst = nv;
    bVector2 *pDstUVs = nuv;
    bVector4 *pDstColors = ncolors;
    bVector3 *pSrc = v;
    bVector2 *pSrcUVs = uv;
    bVector4 *pSrcColors = colors;
    u32 num_verts = 4;
    u32 new_num_verts;

    // La condicion combinada va en la CABECERA (no `if (!num_verts) break;`): el
    // objetivo prueba primero i>3 y luego num_verts, que es la rotacion de bucle de
    // GCC para `i < 4 && num_verts`. Ademas asi `i` deja de estar vivo tras la
    // prueba y el asignador le da r8 en vez de una ranura de pila.
    for (int i = 0; i < 4 && num_verts; i++) {
        bVector3 normal(pClipInfo->normals[i]);
        float constant = pClipInfo->constants[i];
        bool bFlag;
        u32 last_vert;

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

        for (u32 k = 0; k < num_verts; k++) {
            if (bDot(&normal, &pSrc[k]) + constant > -0.5f) {
                if (!bFlag) {
                    float t;
                    pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                    t = -(constant + bDot(&pSrc[last_vert], &normal)) / bDot(&pDst[new_num_verts], &normal);
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                    pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
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
                    t = -(constant + bDot(&pSrc[last_vert], &normal)) / bDot(&pDst[new_num_verts], &normal);
                    pDst[new_num_verts] *= t;
                    pDst[new_num_verts] += pSrc[last_vert];
                    pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                    pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                    new_num_verts++;
                    bFlag = false;
                }
            }
            last_vert = k;
        }

        void *pTmp;
        pTmp = pDst;
        pDst = pSrc;
        pSrc = static_cast<bVector3 *>(pTmp);
        pTmp = pDstUVs;
        pDstUVs = pSrcUVs;
        pSrcUVs = static_cast<bVector2 *>(pTmp);
        pTmp = pDstColors;
        pDstColors = pSrcColors;
        pSrcColors = static_cast<bVector4 *>(pTmp);

        num_verts = new_num_verts;
    }

    return new_num_verts;
}

//==========================================================================================================================

uint32 ClipLeft(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors, uint32 num_verts,
                float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    u32 last_vert;

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

    for (u32 k = 0; k < num_verts; k++) {
        if (pSrc[k].x >= value) {
            if (!bFlag) {
                float t;
                pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                pDst[new_num_verts] *= t;
                pDst[new_num_verts] += pSrc[last_vert];
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
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
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                new_num_verts++;
                bFlag = false;
            }
        }
        last_vert = k;
    }

    return new_num_verts;
}

uint32 ClipTop(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors, uint32 num_verts,
               float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    u32 last_vert;

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

    for (u32 k = 0; k < num_verts; k++) {
        if (pSrc[k].y >= value) {
            if (!bFlag) {
                float t;
                pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                pDst[new_num_verts] *= t;
                pDst[new_num_verts] += pSrc[last_vert];
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
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
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                new_num_verts++;
                bFlag = false;
            }
        }
        last_vert = k;
    }

    return new_num_verts;
}

uint32 ClipRight(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors, uint32 num_verts,
                 float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    u32 last_vert;

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

    for (u32 k = 0; k < num_verts; k++) {
        if (pSrc[k].x <= value) {
            if (!bFlag) {
                float t;
                pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                t = (value - pSrc[last_vert].x) / pDst[new_num_verts].x;
                pDst[new_num_verts] *= t;
                pDst[new_num_verts] += pSrc[last_vert];
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
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
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                new_num_verts++;
                bFlag = false;
            }
        }
        last_vert = k;
    }

    return new_num_verts;
}

uint32 ClipBottom(bVector3 *pDst, bVector2 *pDstUVs, bVector4 *pDstColors, bVector3 *pSrc, bVector2 *pSrcUVs, bVector4 *pSrcColors, uint32 num_verts,
                  float value) {
    unsigned int new_num_verts = 0;
    bool bFlag;
    u32 last_vert;

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

    for (u32 k = 0; k < num_verts; k++) {
        if (pSrc[k].y <= value) {
            if (!bFlag) {
                float t;
                pDst[new_num_verts] = pSrc[k] - pSrc[last_vert];
                t = (value - pSrc[last_vert].y) / pDst[new_num_verts].y;
                pDst[new_num_verts] *= t;
                pDst[new_num_verts] += pSrc[last_vert];
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
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
                pDstUVs[new_num_verts] = pSrcUVs[last_vert] + (pSrcUVs[k] - pSrcUVs[last_vert]) * t;
                pDstColors[new_num_verts] = pSrcColors[last_vert] + V4Mult(pSrcColors[k] - pSrcColors[last_vert], t);
                new_num_verts++;
                bFlag = false;
            }
        }
        last_vert = k;
    }

    return new_num_verts;
}

uint32 FERenderObject::ClipAligned(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv,
                                   bVector4 *ncolors) {
    bVector3 *pDst = nv;
    bVector2 *pDstUVs = nuv;
    bVector4 *pDstColors = ncolors;
    bVector3 *pSrc = v;
    bVector2 *pSrcUVs = uv;
    bVector4 *pSrcColors = colors;
    u32 num_verts;

    num_verts = ClipLeft(pDst, pDstUVs, pDstColors, pSrc, pSrcUVs, pSrcColors, 4, pClipInfo->constants[3]);
    if (!num_verts)
        return 0;
    num_verts = ClipTop(pSrc, pSrcUVs, pSrcColors, pDst, pDstUVs, pDstColors, num_verts, pClipInfo->constants[0]);
    if (!num_verts)
        return 0;
    num_verts = ClipRight(pDst, pDstUVs, pDstColors, pSrc, pSrcUVs, pSrcColors, num_verts, pClipInfo->constants[1]);
    if (!num_verts)
        return 0;
    num_verts = ClipBottom(pSrc, pSrcUVs, pSrcColors, pDst, pDstUVs, pDstColors, num_verts, pClipInfo->constants[2]);
    if (!num_verts)
        return 0;
    return num_verts;
}
