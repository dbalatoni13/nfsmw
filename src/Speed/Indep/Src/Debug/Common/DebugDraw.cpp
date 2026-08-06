#include "Speed/Indep/Src/Debug/Common/DebugDraw.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

#include <string.h>

extern float Tweak_drawRange;
extern int sDebugDrawMaxPrims;
extern int sDebugDrawMaxLinePrims;

extern const COORD2 D_005425E0 = {0.5f, 1.0f};
extern const COORD2 D_005425E8 = {1.0f, 0.0f};
extern const COORD2 D_005425F0 = {0.0f, 1.0f};
extern const COORD2 D_005425F8 = {1.0f, 1.0f};

namespace UMemory {
void Free(void *ptr) {
    bFree(ptr);
}
}

DebugDraw *DebugDraw::fgDbgDraw;

inline bVector3 Coord4ToSwizzledbVec(const COORD4 *c) {
    bVector3 vec;
    bVector3 temp;

    temp.x = c->x;
    temp.y = c->y;
    temp.z = c->z;
    eSwizzleWorldVector(temp, vec);
    return vec;
}

void DebugDraw::CacheCameraInfo() {
    eView *view = eGetView(EVIEW_PLAYER1, false);

    if (!view->Active) {
        this->fCachedCameraPos = UMath::Vector4::kIdentity;
        this->fCachedCameraFwdVec = UMath::Vector4::kIdentity;
        this->fCachedCameraUpVec = UMath::Vector4::kIdentity;
    }

    {
        bVector3 cPos = *view->GetCamera()->GetPosition();
        bVector3 posV3;

        eUnSwizzleWorldVector(cPos, posV3);
        this->fCachedCameraPos = UMath::Vector4Make(1.0f, posV3);
    }

    {
        bVector3 cDir = *view->GetCamera()->GetDirection();
        bVector3 dirV3;

        eUnSwizzleWorldVector(cDir, dirV3);
        this->fCachedCameraFwdVec = UMath::Vector4Make(1.0f, dirV3);
    }

    {
        bVector3 cUp = *view->GetCamera()->GetUpVec();
        bVector3 upV3;

        eUnSwizzleWorldVector(cUp, upV3);
        bNeg(&upV3, &upV3);
        this->fCachedCameraUpVec = UMath::Vector4Make(1.0f, upV3);
    }
}

DebugDraw::DebugDraw() {
    fZTest = true;
    fCheckDrawOverlap = true;
    fEnabled = false;
    fNumTriPrims = 0;
    fNumLinPrims = 0;
    fTextureInfo = nullptr;
}

void DebugDraw::AllocLists() {
    unsigned int nameHash;
    TextureInfo *textureInfo;
    DrawPrimTri *triPrimList;
    unsigned int *colourList;
    UMath::Vector4 *vertList;
    DrawPrimLin *linPrimList;
    DrawPrimTri *triPrim;
    DrawPrimLin *linPrim;
    int i;

    if (!fTextureInfo) {
        nameHash = bStringHash("GREYMAP");
        textureInfo = GetTextureInfo(nameHash, 1, 0);
        i = sDebugDrawMaxPrims;
        fTextureInfo = textureInfo;

        triPrimList = new ("DrawPrimTri[]", 0) DrawPrimTri[i];
        triPrim = triPrimList;
        i = sDebugDrawMaxPrims - 1;
        if (sDebugDrawMaxPrims != 0) {
            do {
                triPrim->fLifeSpan = 0;
                i--;
                triPrim->fTimeType = 0;
                triPrim++;
            } while (i != -1);
        }

        i = sDebugDrawMaxPrims;
        fTriPrimList = triPrimList;
        colourList = static_cast<unsigned int *>(bMalloc(i * 0xc, "DebugDraw TriCols", 0, 0));
        i = sDebugDrawMaxPrims;
        fTriColourList = colourList;
        vertList = static_cast<UMath::Vector4 *>(bMalloc(i * 0x30, "DebugDraw TriVerts", 0, 0));
        i = sDebugDrawMaxLinePrims;
        fTriVertList = vertList;

        linPrimList = new ("DrawPrimLin[]", 0) DrawPrimLin[i];
        linPrim = linPrimList;
        i = sDebugDrawMaxLinePrims - 1;
        if (sDebugDrawMaxLinePrims != 0) {
            do {
                linPrim->fLifeSpan = 0;
                i--;
                linPrim->fTimeType = 0;
                linPrim++;
            } while (i != -1);
        }

        i = sDebugDrawMaxLinePrims;
        fLinPrimList = linPrimList;
        colourList = static_cast<unsigned int *>(bMalloc(i * 0x8, "DebugDraw LinCols", 0, 0));
        i = sDebugDrawMaxLinePrims;
        fLinColourList = colourList;
        vertList = static_cast<UMath::Vector4 *>(bMalloc(i * 0x20, "DebugDraw LinVerts", 0, 0));
        fLinVertList = vertList;
    }
}

void DebugDraw::DeAllocLists() {
    DrawPrimLin *ptr;

    if (!fTextureInfo) {
        fEnabled = false;
    } else {
        if (!fTriPrimList) {
            ptr = fLinPrimList;
        } else {
            delete[] fTriPrimList;
            ptr = fLinPrimList;
        }
        if (ptr) {
            delete[] ptr;
        }
        UMemory::Free(fTriColourList);
        UMemory::Free(fTriVertList);
        UMemory::Free(fLinColourList);
        UMemory::Free(fLinVertList);
        fEnabled = false;
    }
    fNumTriPrims = 0;
    fNumLinPrims = 0;
    fTextureInfo = nullptr;
}

bool DebugDraw::InView(const UMath::Vector3 &pt, float radius) {
    UMath::Vector3 camPos;
    UMath::Vector3 camVec;
    UMath::Vector3 objVec;

    camPos = UMath::Vector4To3(Get().GetCameraPos());
    camVec = UMath::Vector4To3(Get().GetCameraFwdVec());
    UMath::Sub(pt, camPos, objVec);

    if (radius > UMath::Length(objVec)) {
        return true;
    }

    {
        const float threshold = 0.707f;
        float dot;

        if (threshold > UMath::Abs(camVec.y)) {
            objVec.y = camVec.y = 0.0f;
            dot = UMath::Dot(objVec, camVec);
            if (dot < 0.0f) {
                return false;
            }
        }
    }

    return true;
}

bool DebugDraw::OutOfRange(const UMath::Vector3 &pt, float radius) {
    UMath::Vector3 pos;
    UMath::Vector3 diff;
    float distSq;
    float range;
    bool outOfRange;

    pos = UMath::Vector4To3(Get().GetCameraPos());
    UMath::Sub(pos, pt, diff);
    distSq = diff.x * diff.x + diff.z * diff.z;
    range = Tweak_drawRange + radius;
    outOfRange = range * range < distSq;
    return outOfRange;
}

bool DebugDraw::OutOfRange(const UMath::Vector4 &pt, float radius) {
    return OutOfRange(UMath::Vector4To3(pt), radius);
}

float DebugDraw::GetDrawRange() {
    return Tweak_drawRange;
}

short int DebugDraw::GetTimeType() {
    return fTimeType;
}

void DebugDraw::LineSeg(const UMath::Vector3 &pt0, const UMath::Vector3 &pt1,
                        unsigned int c, short int lifeSpan) {
    UMath::Vector4 pt14 = UMath::Vector4Make(1.0f, pt0);
    UMath::Vector4 pt24 = UMath::Vector4Make(1.0f, pt1);
    LineSeg(UMath::Matrix4::kIdentity, pt14, pt24, c, lifeSpan);
}

void DebugDraw::Line(const UMath::Matrix4 &mat, const UMath::Vector3 &pt1,
                     const UMath::Vector3 &pt2, float scale, unsigned int c,
                     short int lifeSpan) {
    if (fEnabled) {
        UMath::Vector4 pt14 = UMath::Vector4Make(1.0f, pt1);
        UMath::Vector4 pt24 = UMath::Vector4Make(1.0f, pt2);
        Line(mat, pt14, pt24, scale, c, lifeSpan);
    }
}

void DebugDraw::Line(const UMath::Matrix4 &mat, const UMath::Vector4 &pt1,
                     const UMath::Vector4 &pt2, float scale, unsigned int c,
                     short int lifeSpan) {
    if (fEnabled) {
        const UMath::Vector4 inVec = Get().GetCameraFwdVec();
        UMath::Vector4 widthVec;
        UMath::Vector4 vec;
        UMath::Vector4 pts[4];

        UMath::Clear(vec);
        UMath::Subxyz(pt2, pt1, vec);
        float length = UMath::Lengthxyz(vec);
        if (length < 0.0001f) {
            Quad(pt1, scale, c, lifeSpan, -1);
        } else {
            UMath::UnitCrossxyz(inVec, vec, widthVec);
            UMath::ScaleAddxyz(widthVec, -scale, pt1, pts[0]);
            pts[0].w = 1.0f;
            UMath::ScaleAddxyz(widthVec, scale, pt1, pts[1]);
            pts[1].w = 1.0f;
            UMath::ScaleAddxyz(widthVec, scale, pt2, pts[2]);
            pts[2].w = 1.0f;
            UMath::ScaleAddxyz(widthVec, -scale, pt2, pts[3]);
            pts[3].w = 1.0f;
            Quad(mat, pts, pts + 1, pts + 2, pts + 3, c, lifeSpan, -1);
        }
    }
}

void DebugDraw::LineSeg(const UMath::Matrix4 &mat, const UMath::Vector4 &pt0,
                        const UMath::Vector4 &pt1, unsigned int c,
                        short int lifeSpan) {
    if (fEnabled) {
        int i = fNumLinPrims;
        if (i < sDebugDrawMaxLinePrims) {
            UMath::Vector4 tpt0;
            UMath::Vector4 tpt1;
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(pt0), mat,
                                  UMath::Vector4To3(tpt0));
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(pt1), mat,
                                  UMath::Vector4To3(tpt1));

            UMath::Vector4 *pVVar6 = fLinVertList + i * 2;
            unsigned int *puVar2 = fLinColourList;
            DrawPrimLin *pDVar3 = fLinPrimList;
            pVVar6->x = tpt0.x;
            puVar2[i * 2] = c;
            pVVar6->y = tpt0.y;
            pVVar6->z = tpt0.z;
            pDVar3[i].fLifeSpan = lifeSpan;
            puVar2[i * 2 + 1] = c;
            pVVar6->w = tpt0.w;
            pVVar6[1].x = tpt1.x;
            pVVar6[1].y = tpt1.y;
            pVVar6[1].z = tpt1.z;
            pVVar6[1].w = tpt1.w;
            pDVar3[i].fTimeType = fgDbgDraw->GetTimeType();
            fNumLinPrims = i + 1;
        }
    }
}

void DebugDraw::Cone(const UMath::Matrix4 &mat, float height, float radius,
                     unsigned int c, short int lifeSpan) {
    int numsteps = 20;
    int i = 0;
    do {
        float angle;
        float nextangle;
        UMath::Vector3 p0;
        UMath::Vector3 p1;
        UMath::Vector3 p2;

        angle = static_cast<float>(i) / static_cast<float>(numsteps);
        i++;
        nextangle = static_cast<float>(i) / static_cast<float>(numsteps);
        memset(&p0, 0, 0xc);
        p1.x = UMath::Sina(angle) * radius;
        p1.y = -UMath::Cosa(angle) * radius;
        p1.z = height;
        p2.x = UMath::Sina(nextangle) * radius;
        p2.y = -UMath::Cosa(nextangle) * radius;
        p2.z = height;
        Triangle(mat, &p0, &p1, &p2, c, lifeSpan, -1);
    } while (i < numsteps);
}

void DebugDraw::Sphere(const UMath::Matrix4 &mat, float radius, unsigned int c,
                       short int lifeSpan, bool bShadow, int texture) {
    if (!fEnabled) {
        return;
    }
    if (OutOfRange(mat.v3, radius)) {
        return;
    }

    {
        const int kMaxNumFacets = 12;
        const int kMinNumFacets = 2;
        int numFacets;
        UMath::Matrix4 tmpMat;

        numFacets = static_cast<int>(static_cast<float>(std::log(radius)) + 9.0f);
        tmpMat = mat;
        tmpMat.v3.y -= radius;
        numFacets = UMath::Min(numFacets, kMaxNumFacets);
        numFacets = UMath::Max(numFacets, kMinNumFacets);
        const float angleStep = 0.5f / numFacets;
        float angle = 0.0f;
        for (int i = numFacets; i > 0; --i) {
            float localRadTop;
            float localRadBot;
            float sectionHeight;

            localRadTop = radius * UMath::Cosa(angle - 0.25 + angleStep);
            localRadBot = radius * UMath::Cosa(angle - 0.25);
            sectionHeight = UMath::Abs(radius * (UMath::Sina(angle - 0.25) -
                                                 UMath::Sina(angle - 0.25 + angleStep)));
            CylinderSection(tmpMat, localRadTop, localRadBot, sectionHeight, c, lifeSpan, texture);
            angle += angleStep;
            tmpMat.v3.y += sectionHeight;
        }

        if (bShadow == true) {
            UMath::Vector4 trans1;
            UMath::Vector4 trans2;

            trans1 = mat.v3;
            trans1.w = 1.0f;
            trans2 = trans1;
            trans2.w = 1.0f;
            bool hit;
            {
                WCollisionMgr collisionMgr(0, 3);
                hit = collisionMgr.GetWorldHeightAtPoint(UMath::Vector4To3(trans1), trans2.y, nullptr);
            }
            if (hit) {
                UTransform t(UMath::Matrix3::kIdentity, UMath::Vector4To3(trans2));

                Box(t.fTransform, 1.0f, 0.01f, 1.0f, 0x1f888888, 1, false, -1, true);
                UMath::Subxyz(trans2, trans1, trans2);
                trans2.w = 1.0f;
                Vector(trans1, trans2, 1.0f, 0x1f888888, 1, -1);
            }
        }
    }
}

void DebugDraw::Sphere(const UMath::Vector3 &basePt, float radius, unsigned int c,
                       short int lifeSpan, bool bShadow, int texture) {
    if (fEnabled) {
        UMath::Matrix4 mat;
        UMath::Vector4 res = UMath::Vector4Make(1.0f, basePt);

        mat.v0 = UMath::Matrix4::kIdentity.v0;
        mat.v1 = UMath::Matrix4::kIdentity.v1;
        mat.v2 = UMath::Matrix4::kIdentity.v2;
        mat.v3 = res;
        Sphere(mat, radius, c, lifeSpan, bShadow, texture);
    }
}

void DebugDraw::Circle(const UMath::Matrix4 &mat, float radius, unsigned int c,
                       short int lifeSpan, int texture) {
    if (fEnabled) {
        const int kNumFacets = 12;
        UMath::Vector4 pts[kNumFacets];
        COORD2 txtOrigin = {0.5f, 0.5f};
        UMath::Vector2 txtXY[kNumFacets];
        float txtRadius = 0.5f;
        float angle = 0.0f;
        UMath::Vector4 tpts[kNumFacets];
        UMath::Vector4 cp;

        for (int i = 0; i < kNumFacets; ++i) {
            txtXY[i].x = UMath::Cosa(angle) * txtRadius + txtRadius;
            txtXY[i].y = UMath::Sina(angle) * txtRadius + txtRadius;
            pts[i].x = radius * UMath::Cosa(angle);
            pts[i].y = 0.0f;
            pts[i].z = radius * UMath::Sina(angle);
            pts[i].w = 1.0f;
            angle += 1.0f / kNumFacets;
        }

        UMath::RotateTranslate(kNumFacets, tpts, mat, pts);

        cp.x = 0.0f;
        cp.y = 0.0f;
        cp.z = 0.0f;
        cp.w = 1.0f;
        UMath::RotateTranslate(cp, mat, cp);

        for (int i = 0; i < kNumFacets - 1; ++i) {
            Triangle(&cp, &tpts[i], &tpts[i + 1], &txtOrigin, &txtXY[i],
                     &txtXY[i + 1], c, lifeSpan, texture);
        }
        Triangle(&cp, &tpts[kNumFacets - 1], &tpts[0], &txtOrigin,
                 &txtXY[kNumFacets - 1], &txtXY[0],
                 c, lifeSpan, texture);
    }
}

void DebugDraw::Quad(const UMath::Vector4 *pt0, const UMath::Vector4 *pt1,
                     const UMath::Vector4 *pt2, const UMath::Vector4 *pt3,
                     unsigned int c, short int lifeSpan, int texture) {
    COORD2 uv0;
    COORD2 uv1;
    COORD2 uv2;
    COORD2 uv3;

    uv0 = D_005425F0;
    uv1 = D_005425F8;
    uv2 = D_005425E8;
    memset(&uv3, 0, sizeof(uv3));
    Triangle(pt0, pt1, pt2, &uv0, &uv1, &uv2, c, lifeSpan, texture);
    Triangle(pt2, pt3, pt0, &uv2, &uv3, &uv0, c, lifeSpan, texture);
}

void DebugDraw::Triangle(const UMath::Vector4 *pt0, const UMath::Vector4 *pt1,
                         const UMath::Vector4 *pt2, const COORD2 *uv0,
                         const COORD2 *uv1, const COORD2 *uv2, unsigned int c,
                         short int lifeSpan, int texture) {
    Triangle(UMath::Matrix4::kIdentity, pt0, pt1, pt2, uv0, uv1, uv2, c,
             static_cast<int>(lifeSpan), texture);
}

void DebugDraw::Triangle(const UMath::Matrix4 &pmat, const UMath::Vector4 *pt0,
                         const UMath::Vector4 *pt1, const UMath::Vector4 *pt2,
                         const COORD2 *, const COORD2 *, const COORD2 *,
                         unsigned int c, short int lifeSpan,
                         int texture) {
    if (fEnabled) {
        int i = fNumTriPrims;
        if (i < sDebugDrawMaxPrims) {
            UMath::Matrix4 mat = pmat;
            UMath::Vector4 tpt0;
            UMath::Vector4 tpt1;
            UMath::Vector4 tpt2;

            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt0), mat,
                                  UMath::Vector4To3(tpt0));
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt1), mat,
                                  UMath::Vector4To3(tpt1));
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt2), mat,
                                  UMath::Vector4To3(tpt2));

            DrawPrimTri *pDVar4 = fTriPrimList;
            UMath::Vector4 *pVVar7 = fTriVertList + i * 3;
            unsigned int *puVar8 = fTriColourList + i * 3;
            pDVar4[i].fLifeSpan = lifeSpan;
            pVVar7->x = tpt0.x;
            *puVar8 = c;
            pVVar7->y = tpt0.y;
            puVar8[2] = c;
            pVVar7->z = tpt0.z;
            puVar8[1] = c;
            pVVar7->w = tpt0.w;
            pVVar7[1].x = tpt1.x;
            pVVar7[1].y = tpt1.y;
            pVVar7[1].z = tpt1.z;
            pVVar7[1].w = tpt1.w;
            pVVar7[2].x = tpt2.x;
            pVVar7[2].y = tpt2.y;
            pVVar7[2].z = tpt2.z;
            pVVar7[2].w = tpt2.w;
            pDVar4[i].fTimeType = fgDbgDraw->GetTimeType();
            fNumTriPrims = i + 1;
        }
    }
}

void DebugDraw::Triangle(const UMath::Matrix4 &mat, const UMath::Vector3 *pt0,
                         const UMath::Vector3 *pt1, const UMath::Vector3 *pt2,
                         unsigned int c, short int lifeSpan, int texture) {
    if (fEnabled) {
        COORD2 uv0;
        COORD2 uv1;
        COORD2 uv2;
        memset(&uv0, 0, sizeof(uv0));
        memset(&uv1, 0, sizeof(uv1));
        memset(&uv2, 0, sizeof(uv2));
        UMath::Vector4 pt04 = UMath::Vector4Make(1.0f, *pt0);
        UMath::Vector4 pt14 = UMath::Vector4Make(1.0f, *pt1);
        UMath::Vector4 pt24 = UMath::Vector4Make(1.0f, *pt2);
        Triangle(mat, &pt04, &pt14, &pt24, &uv0, &uv1, &uv2, c, lifeSpan,
                 texture);
    }
}

void DebugDraw::Triangle(const UMath::Matrix4 &mat, const UMath::Vector4 *pt0,
                         const UMath::Vector4 *pt1, const UMath::Vector4 *pt2,
                         unsigned int c, short int lifeSpan, int texture) {
    if (fEnabled) {
        COORD2 uv0;
        COORD2 uv1;
        COORD2 uv2;
        memset(&uv0, 0, sizeof(uv0));
        memset(&uv1, 0, sizeof(uv1));
        memset(&uv2, 0, sizeof(uv2));

        int i = fNumTriPrims;
        if (i < sDebugDrawMaxPrims) {
            UMath::Vector4 tpt0;
            UMath::Vector4 tpt1;
            UMath::Vector4 tpt2;
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt0), mat,
                                  UMath::Vector4To3(tpt0));
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt1), mat,
                                  UMath::Vector4To3(tpt1));
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*pt2), mat,
                                  UMath::Vector4To3(tpt2));

            DrawPrimTri *pDVar4 = fTriPrimList;
            UMath::Vector4 *pVVar7 = fTriVertList + i * 3;
            unsigned int *puVar8 = fTriColourList + i * 3;
            pDVar4[i].fLifeSpan = lifeSpan;
            pVVar7->x = tpt0.x;
            *puVar8 = c;
            pVVar7->y = tpt0.y;
            puVar8[2] = c;
            pVVar7->z = tpt0.z;
            puVar8[1] = c;
            pVVar7->w = tpt0.w;
            pVVar7[1].x = tpt1.x;
            pVVar7[1].y = tpt1.y;
            pVVar7[1].z = tpt1.z;
            pVVar7[1].w = tpt1.w;
            pVVar7[2].x = tpt2.x;
            pVVar7[2].y = tpt2.y;
            pVVar7[2].z = tpt2.z;
            pVVar7[2].w = tpt2.w;
            pDVar4[i].fTimeType = fgDbgDraw->GetTimeType();
            fNumTriPrims = i + 1;
        }
    }
}

void DebugDraw::Quad(const UMath::Matrix4 &mat, const UMath::Vector4 *pt0,
                     const UMath::Vector4 *pt1, const UMath::Vector4 *pt2,
                     const UMath::Vector4 *pt3, unsigned int c,
                     short int lifeSpan, int texture) {
    COORD2 uv0 = D_005425F0;
    COORD2 uv1 = D_005425F8;
    COORD2 uv2 = D_005425E8;
    COORD2 uv3;
    memset(&uv3, 0, sizeof(uv3));
    Triangle(mat, pt0, pt1, pt2, &uv0, &uv1, &uv2, c, lifeSpan, texture);
    Triangle(mat, pt2, pt3, pt0, &uv2, &uv3, &uv0, c, lifeSpan, texture);
}

void DebugDraw::Quad(const UMath::Vector4 &pt, float size, unsigned int c,
                     short int lifeSpan, int texture) {
    if (fEnabled) {
        const UMath::Vector4 inVec = Get().GetCameraFwdVec();
        const UMath::Vector4 upVec = Get().GetCameraUpVec();
        UMath::Vector4 rightVec;
        UMath::Vector4 centerPos;
        UMath::Vector4 pt04;
        UMath::Vector4 pt14;
        UMath::Vector4 pt24;
        UMath::Vector4 pt34;
        COORD2 uv0;
        COORD2 uv1;
        COORD2 uv2;
        COORD2 uv3;
        bCross(reinterpret_cast<bVector3 *>(&rightVec),
               reinterpret_cast<const bVector3 *>(&upVec),
               reinterpret_cast<const bVector3 *>(&inVec));
        centerPos = pt;

        bScaleAdd(reinterpret_cast<bVector3 *>(&pt04),
                  reinterpret_cast<const bVector3 *>(&centerPos),
                  reinterpret_cast<const bVector3 *>(&upVec), size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt04),
                  reinterpret_cast<const bVector3 *>(&pt04),
                  reinterpret_cast<const bVector3 *>(&rightVec), -size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt14),
                  reinterpret_cast<const bVector3 *>(&centerPos),
                  reinterpret_cast<const bVector3 *>(&upVec), size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt14),
                  reinterpret_cast<const bVector3 *>(&pt14),
                  reinterpret_cast<const bVector3 *>(&rightVec), size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt24),
                  reinterpret_cast<const bVector3 *>(&centerPos),
                  reinterpret_cast<const bVector3 *>(&upVec), -size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt24),
                  reinterpret_cast<const bVector3 *>(&pt24),
                  reinterpret_cast<const bVector3 *>(&rightVec), size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt34),
                  reinterpret_cast<const bVector3 *>(&centerPos),
                  reinterpret_cast<const bVector3 *>(&upVec), -size);
        bScaleAdd(reinterpret_cast<bVector3 *>(&pt34),
                  reinterpret_cast<const bVector3 *>(&pt34),
                  reinterpret_cast<const bVector3 *>(&rightVec), -size);

        uv0.x = 0.0f;
        uv0.y = 1.0f;
        uv1.x = 1.0f;
        uv1.y = 1.0f;
        uv2.x = 1.0f;
        uv2.y = 0.0f;
        memset(&uv3, 0, sizeof(uv3));
        Triangle(&pt04, &pt14, &pt24, &uv0, &uv1, &uv2, c, lifeSpan, texture);
        Triangle(&pt24, &pt34, &pt04, &uv2, &uv3, &uv0, c, lifeSpan, texture);
    }
}

void DebugDraw::Box(const UMath::Vector4 *tpts, unsigned int c, short int lifeSpan,
                    bool bShadow, int texture) {
    if (fEnabled) {
        Quad(&tpts[0], &tpts[1], &tpts[2], &tpts[3], c, lifeSpan, texture);
        Quad(&tpts[4], &tpts[5], &tpts[6], &tpts[7], c, lifeSpan, texture);
        Quad(&tpts[0], &tpts[4], &tpts[7], &tpts[3], c, lifeSpan, texture);
        Quad(&tpts[1], &tpts[5], &tpts[4], &tpts[0], c, lifeSpan, texture);
        Quad(&tpts[2], &tpts[6], &tpts[5], &tpts[1], c, lifeSpan, texture);
        Quad(&tpts[3], &tpts[7], &tpts[6], &tpts[2], c, lifeSpan, texture);

        if (bShadow) {
            UMath::Vector4 trans1;
            UMath::Vector4 trans2;

            trans1.y = tpts[0].y;
            trans1.x = (tpts[0].x + tpts[2].x) * 0.5f;
            trans1.z = (tpts[0].z + tpts[2].z) * 0.5f;
            trans1.w = 1.0f;
            trans2.y = trans1.y;
            trans2.x = trans1.x;
            trans2.w = 1.0f;
            trans2.z = trans1.z;
            bool hit;
            {
                WCollisionMgr collisionMgr(0, 3);
                hit = collisionMgr.GetWorldHeightAtPoint(UMath::Vector4To3(trans1), trans2.y, nullptr);
            }
            if (hit) {
                UTransform t(UMath::Matrix3::kIdentity, UMath::Vector4To3(trans2));

                Box(t.fTransform, 1.0f, 0.01f, 1.0f, 0x1f888888, 1, false, -1, true);
                UMath::Subxyz(trans2, trans1, trans2);
                trans2.w = 1.0f;
                Vector(trans1, trans2, 1.0f, 0x1f888888, 1, -1);
            }
        }
    }
}

void DebugDraw::CylinderSection(const UMath::Matrix4 &mat, float radiusTop, float radiusBot,
                                float height, unsigned int c, short int lifeSpan, int texture) {
    const int kMaxNumFacets = 12;
    const int kMinNumFacets = 4;
    int numFacets;
    UMath::Vector4 pts[12];
    UMath::Vector4 topTpts[12];
    UMath::Vector4 botTpts[12];

    if (fEnabled) {
        float angle = 0.0f;
        numFacets = static_cast<int>(static_cast<float>(std::log(radiusTop + radiusBot)) * 5.0f + 6.0f);
        numFacets = UMath::Min(numFacets, kMaxNumFacets);
        numFacets = UMath::Max(numFacets, kMinNumFacets);

        if (numFacets > 0) {
            const float numFacetsFloat = static_cast<float>(numFacets);
            for (int i = 0; i < numFacets; ++i) {
                pts[i].x = radiusBot * UMath::Cosa(angle);
                pts[i].y = 0.0f;
                pts[i].z = radiusBot * UMath::Sina(angle);
                pts[i].w = 1.0f;
                angle += 1.0f / numFacetsFloat;
            }
        }
        UMath::RotateTranslate(numFacets, botTpts, mat, pts);

        angle = 0.0f;
        if (numFacets > 0) {
            const float numFacetsFloat = static_cast<float>(numFacets);
            for (int i = 0; i < numFacets; ++i) {
                pts[i].x = radiusTop * UMath::Cosa(angle);
                pts[i].y = height;
                pts[i].z = radiusTop * UMath::Sina(angle);
                pts[i].w = 1.0f;
                angle += 1.0f / numFacetsFloat;
            }
        }
        UMath::RotateTranslate(numFacets, topTpts, mat, pts);

        for (int i = 0; i < numFacets - 1; ++i) {
            Quad(&topTpts[i], &topTpts[i + 1], &botTpts[i + 1], &botTpts[i], c, lifeSpan, texture);
        }
        Quad(&topTpts[numFacets - 1], &topTpts[0], &botTpts[0], &botTpts[numFacets - 1],
             c, lifeSpan, texture);
    }
}

void DebugDraw::Box(const UMath::Matrix4 &mat, float width, float height, float depth,
                    unsigned int c, short int lifeSpan, bool bShadow, int texture, bool frombase) {
    if (fEnabled) {
        UMath::Vector4 tpts[8];
        UMath::Vector4 pts[8];

        if (frombase) {
            memset(&pts[0], 0, sizeof(pts[0]));
            pts[0].x = -width * 0.5f;
            pts[0].z = -depth * 0.5f;
            pts[0].w = 1.0f;
            memset(&pts[1], 0, sizeof(pts[1]));
            pts[1].x = -width * 0.5f;
            pts[1].z = depth * 0.5f;
            pts[1].w = 1.0f;
            memset(&pts[2], 0, sizeof(pts[2]));
            pts[2].x = width * 0.5f;
            pts[2].z = depth * 0.5f;
            pts[2].w = 1.0f;
            memset(&pts[3], 0, sizeof(pts[3]));
            pts[3].x = width * 0.5f;
            pts[3].z = -depth * 0.5f;
            pts[3].w = 1.0f;
            memset(&pts[4], 0, sizeof(pts[4]));
            pts[4].x = -width * 0.5f;
            pts[4].y = height;
            pts[4].z = -depth * 0.5f;
            pts[4].w = 1.0f;
            memset(&pts[5], 0, sizeof(pts[5]));
            pts[5].x = -width * 0.5f;
            pts[5].y = height;
            pts[5].z = depth * 0.5f;
            pts[5].w = 1.0f;
            memset(&pts[6], 0, sizeof(pts[6]));
            pts[6].x = width * 0.5f;
            pts[6].y = height;
            pts[6].z = depth * 0.5f;
            pts[6].w = 1.0f;
            memset(&pts[7], 0, sizeof(pts[7]));
            pts[7].x = width * 0.5f;
            pts[7].y = height;
            pts[7].z = -depth * 0.5f;
            pts[7].w = 1.0f;
        } else {
            memset(&pts[0], 0, sizeof(pts[0]));
            pts[0].x = -width * 0.5f;
            pts[0].y = -height * 0.5f;
            pts[0].z = -depth * 0.5f;
            pts[0].w = 1.0f;
            memset(&pts[1], 0, sizeof(pts[1]));
            pts[1].x = -width * 0.5f;
            pts[1].y = -height * 0.5f;
            pts[1].z = depth * 0.5f;
            pts[1].w = 1.0f;
            memset(&pts[2], 0, sizeof(pts[2]));
            pts[2].x = width * 0.5f;
            pts[2].y = -height * 0.5f;
            pts[2].z = depth * 0.5f;
            pts[2].w = 1.0f;
            memset(&pts[3], 0, sizeof(pts[3]));
            pts[3].x = width * 0.5f;
            pts[3].y = -height * 0.5f;
            pts[3].z = -depth * 0.5f;
            pts[3].w = 1.0f;
            memset(&pts[4], 0, sizeof(pts[4]));
            pts[4].x = -width * 0.5f;
            pts[4].y = height * 0.5f;
            pts[4].z = -depth * 0.5f;
            pts[4].w = 1.0f;
            memset(&pts[5], 0, sizeof(pts[5]));
            pts[5].x = -width * 0.5f;
            pts[5].y = height * 0.5f;
            pts[5].z = depth * 0.5f;
            pts[5].w = 1.0f;
            memset(&pts[6], 0, sizeof(pts[6]));
            pts[6].x = width * 0.5f;
            pts[6].y = height * 0.5f;
            pts[6].z = depth * 0.5f;
            pts[6].w = 1.0f;
            memset(&pts[7], 0, sizeof(pts[7]));
            pts[7].x = width * 0.5f;
            pts[7].y = height * 0.5f;
            pts[7].z = -depth * 0.5f;
            pts[7].w = 1.0f;
        }

        UMath::Vector4 *result = tpts;
        int count = 8;
        const UMath::Vector4 *v = pts;
        do {
            VU0_MATRIX4_vect3mult(UMath::Vector4To3(*v), mat,
                                  UMath::Vector4To3(*result));
            ++v;
            ++result;
            --count;
        } while (count != 0);
        Box(tpts, c, lifeSpan, bShadow, texture);
    }
}

void DebugDraw::Cylinder(const UMath::Matrix4 &mat, float radius, float height,
                         unsigned int c, short int lifeSpan, bool bShadow, int texture) {
    if (fEnabled) {
        CylinderSection(mat, radius, radius, height, c, lifeSpan, texture);
        Circle(mat, radius, c, lifeSpan, texture);

        UMath::Matrix4 matTop;
        matTop = mat;
        matTop.v3.y += height;
        Circle(matTop, radius, c, lifeSpan, texture);

        if (bShadow == true) {
            UMath::Vector4 trans1;
            UMath::Vector4 trans2;

            trans1 = mat.v3;
            trans1.w = 1.0f;
            trans2 = trans1;
            trans2.w = 1.0f;
            bool hit;
            {
                WCollisionMgr collisionMgr(0, 3);
                hit = collisionMgr.GetWorldHeightAtPoint(UMath::Vector4To3(trans1), trans2.y, nullptr);
            }
            if (hit) {
                UTransform t(UMath::Matrix3::kIdentity, UMath::Vector4To3(trans2));

                Box(t.fTransform, 1.0f, 0.01f, 1.0f, 0x1f888888, 1, false, -1, true);
                UMath::Subxyz(trans2, trans1, trans2);
                trans2.w = 1.0f;
                Vector(trans1, trans2, 1.0f, 0x1f888888, 1, -1);
            }
        }
    }
}

void DebugDraw::Cylinder(const UMath::Vector3 &basePt, float radius, float height,
                         unsigned int c, short int lifeSpan, bool bShadow, int texture) {
    if (fEnabled) {
        UMath::Matrix4 mat;
        UMath::Vector4 res = UMath::Vector4Make(1.0f, basePt);

        mat.v0 = UMath::Matrix4::kIdentity.v0;
        mat.v1 = UMath::Matrix4::kIdentity.v1;
        mat.v2 = UMath::Matrix4::kIdentity.v2;
        mat.v3 = res;
        Cylinder(mat, radius, height, c, lifeSpan, bShadow, texture);
    }
}

void DebugDraw::Vector(const UMath::Vector3 &pt, const UMath::Vector3 &vec, float scale,
                       unsigned int c, short int lifeSpan, int texture) {
    UMath::Vector4 pt4 = UMath::Vector4Make(pt, 1.0f);
    UMath::Vector4 vector4 = UMath::Vector4Make(vec, 1.0f);
    Vector(pt4, vector4, scale, c, lifeSpan, texture);
}

void DebugDraw::Vector(const UMath::Vector4 &pt, const UMath::Vector4 &vec, float scale,
                       unsigned int c, short int lifeSpan, int texture) {
    if (fEnabled) {
        UMath::Vector4 pts[3];
        UMath::Vector4 inVec;
        UMath::Vector4 widthVec;
        COORD2 uv0;
        COORD2 uv1;
        COORD2 uv2;

        UMath::Crossxyz(Get().GetCameraFwdVec(), vec, widthVec);
        UMath::Scale(widthVec, scale * 0.05f, widthVec);
        pts[0].x = pt.x;
        pts[0].y = pt.y;
        pts[0].z = pt.z;
        pts[0].w = 1.0f;
        UMath::ScaleAdd(vec, scale, pt, inVec);
        UMath::Subxyz(inVec, widthVec, pts[1]);
        UMath::Addxyz(inVec, widthVec, pts[2]);

        memset(&uv0, 0, sizeof(uv0));
        uv1 = D_005425E0;
        uv2 = D_005425E8;
        Triangle(&pts[0], &pts[1], &pts[2], &uv0, &uv1, &uv2, c, lifeSpan, texture);
    }
}

void DebugDraw::DrawAll() {
    eView *view = eGetView(EVIEW_PLAYER1, false);
    bMatrix4 *pL2W;
    static int lastSimFrame;
    int simDecayFrames;

    if (!view) {
        return;
    }
    pL2W = static_cast<bMatrix4 *>(eFrameMalloc(sizeof(bMatrix4)));
    if (!pL2W) {
        return;
    }

    bIdentity(pL2W);
    view->BiasMatrixForZSorting(pL2W, 0.96f);

    {
        if (this->fNumTriPrims > 0) {
            ePoly poly;
            int iCurVert;
            int iNumVertsLeft;

            iCurVert = 0;
            iNumVertsLeft = this->fNumTriPrims * 3;
            if (iNumVertsLeft > 0) {
                while (true) {
                    poly.Vertices[0] = Coord4ToSwizzledbVec(this->fTriVertList + iCurVert);
                    poly.Vertices[1] = Coord4ToSwizzledbVec(this->fTriVertList + iCurVert + 1);
                    poly.Vertices[2] = Coord4ToSwizzledbVec(this->fTriVertList + iCurVert + 2);
                    iNumVertsLeft -= 3;
                    iCurVert += 3;
                    unsigned int colour = this->fTriColourList[iCurVert];
                    reinterpret_cast<unsigned int *>(poly.Colours)[3] =
                        colour;
                    reinterpret_cast<unsigned int *>(poly.Colours)[2] =
                        colour;
                    reinterpret_cast<unsigned int *>(poly.Colours)[1] =
                        colour;
                    reinterpret_cast<unsigned int *>(poly.Colours)[0] =
                        colour;

                    view->Render(&poly, this->fTextureInfo, pL2W, 0, 0.0f);
                    if (iNumVertsLeft < 1) {
                        break;
                    }
                }
            }
        }
    }

    {
        int iCurVert;
        int iNumVertsLeft;

        iNumVertsLeft = this->fNumLinPrims;
        if (iNumVertsLeft > 0) {
            ePolyVertex verts[2];

            iNumVertsLeft <<= 1;
            iCurVert = 0;
            if (iNumVertsLeft > 2) {
                do {
                    iNumVertsLeft -= 2;
                    verts[0].u = 0.5f;
                    verts[0].v = 0.5f;
                    iCurVert += 2;
                    verts[0].y = -this->fLinVertList[iCurVert - 2].x;
                    verts[0].x = this->fLinVertList[iCurVert - 2].z;
                    verts[0].z = this->fLinVertList[iCurVert - 2].y;
                    verts[0].colour = (this->fLinColourList[iCurVert - 2] & 0xff) << 16 |
                                      this->fLinColourList[iCurVert - 2] & 0xff000000 |
                                      (this->fLinColourList[iCurVert - 2] & 0xff0000) >> 16 |
                                      this->fLinColourList[iCurVert - 2] & 0xff00;
                    verts[1].y = -this->fLinVertList[iCurVert - 1].x;
                    verts[1].x = this->fLinVertList[iCurVert - 1].z;
                    verts[1].z = this->fLinVertList[iCurVert - 1].y;
                    verts[1].colour = (this->fLinColourList[iCurVert - 2] & 0xff) << 16 |
                                      this->fLinColourList[iCurVert - 2] & 0xff000000 |
                                      (this->fLinColourList[iCurVert - 2] & 0xff0000) >> 16 |
                                      this->fLinColourList[iCurVert - 2] & 0xff00;
                    verts[1].u = 0.5f;
                    verts[1].v = 0.5f;

                    eDrawKLine(verts, 1);
                } while (iNumVertsLeft > 2);
            }
        }
    }

    {
        int iCurVert;
        int iNumVertsLeft;

        iCurVert = 0;
        iNumVertsLeft = this->fNumTriPrims;
        simDecayFrames = eGetFrameCounter() - lastSimFrame;
        lastSimFrame = eGetFrameCounter();
        for (; iCurVert < iNumVertsLeft; iCurVert++) {
            if (this->fTriPrimList[iCurVert].fTimeType == 1) {
                if (this->fTriPrimList[iCurVert].fLifeSpan > 0) {
                    this->fTriPrimList[iCurVert].fLifeSpan -= simDecayFrames;
                }
                if (this->fTriPrimList[iCurVert].fLifeSpan < 0) {
                    this->fTriPrimList[iCurVert].fLifeSpan = 0;
                }
            } else if (this->fTriPrimList[iCurVert].fLifeSpan > 0) {
                this->fTriPrimList[iCurVert].fLifeSpan--;
            }
        }
    }

    {
        int endIndex;

        endIndex = this->fNumTriPrims - 1;
        if (this->fNumTriPrims > 0) {
            int s = 0;
            do {
            if (this->fTriPrimList[s].fLifeSpan == 0) {
                if (s <= endIndex) {
                    while (s <= endIndex && this->fTriPrimList[endIndex].fLifeSpan == 0) {
                        endIndex--;
                    }
                }
                if (s < endIndex) {
                    this->fTriPrimList[s] = this->fTriPrimList[endIndex];
                    this->fTriColourList[s * 3] = this->fTriColourList[endIndex * 3];
                    this->fTriColourList[s * 3 + 1] = this->fTriColourList[endIndex * 3 + 1];
                    this->fTriColourList[s * 3 + 2] = this->fTriColourList[endIndex * 3 + 2];
                    this->fTriVertList[s * 3] = this->fTriVertList[endIndex * 3];
                    this->fTriVertList[s * 3 + 1] = this->fTriVertList[endIndex * 3 + 1];
                    this->fTriVertList[s * 3 + 2] = this->fTriVertList[endIndex * 3 + 2];
                    endIndex--;
                }
            }
                s++;
            } while (s < this->fNumTriPrims);
        }
        this->fNumTriPrims = endIndex + 1;
    }

    {
        int iCurVert;
        int iNumVertsLeft;

        iCurVert = 0;
        iNumVertsLeft = this->fNumLinPrims;
        for (; iCurVert < iNumVertsLeft; iCurVert++) {
            if (this->fLinPrimList[iCurVert].fTimeType == 1) {
                if (this->fLinPrimList[iCurVert].fLifeSpan > 0) {
                    this->fLinPrimList[iCurVert].fLifeSpan -= simDecayFrames;
                }
                if (this->fLinPrimList[iCurVert].fLifeSpan < 0) {
                    this->fLinPrimList[iCurVert].fLifeSpan = 0;
                }
            } else if (this->fLinPrimList[iCurVert].fLifeSpan > 0) {
                this->fLinPrimList[iCurVert].fLifeSpan--;
            }
        }
    }

    {
        int endIndex;

        endIndex = this->fNumLinPrims - 1;
        if (this->fNumLinPrims > 0) {
            int s = 0;
            do {
            if (this->fLinPrimList[s].fLifeSpan == 0) {
                if (s <= endIndex) {
                    while (s <= endIndex && this->fLinPrimList[endIndex].fLifeSpan == 0) {
                        endIndex--;
                    }
                }
                if (s < endIndex) {
                    this->fLinPrimList[s] = this->fLinPrimList[endIndex];
                    this->fLinColourList[s * 2] = this->fLinColourList[endIndex * 2];
                    this->fLinColourList[s * 2 + 1] = this->fLinColourList[endIndex * 2 + 1];
                    this->fLinVertList[s * 2] = this->fLinVertList[endIndex * 2];
                    this->fLinVertList[s * 2 + 1] = this->fLinVertList[endIndex * 2 + 1];
                    endIndex--;
                }
            }
                s++;
            } while (s < this->fNumLinPrims);
        }
        this->fNumLinPrims = endIndex + 1;
    }

    this->CacheCameraInfo();
}
