#include "Speed/Indep/Src/Debug/Common/DebugDraw.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

extern float Tweak_drawRange;
extern int sDebugDrawMaxPrims;
extern int sDebugDrawMaxLinePrims;

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
        UMath::Vector4 pts[12];
        COORD2 txtOrigin = {0.5f, 0.5f};
        UMath::Vector2 txtXY[12];
        float txtRadius = 1.0f / 12.0f;
        float angle = 0.0f;
        UMath::Vector4 tpts[12];
        UMath::Vector4 cp;

        for (int i = 0; i < 12; ++i) {
            txtXY[i].x = UMath::Cosa(angle) * 0.5f + 0.5f;
            txtXY[i].y = UMath::Sina(angle) * 0.5f + 0.5f;
            pts[i].x = radius * UMath::Cosa(angle);
            pts[i].y = 0.0f;
            pts[i].z = radius * UMath::Sina(angle);
            pts[i].w = 1.0f;
            angle += txtRadius;
        }

        UMath::RotateTranslate(12, tpts, mat, pts);

        cp.x = 0.0f;
        cp.y = 0.0f;
        cp.z = 0.0f;
        cp.w = 1.0f;
        UMath::RotateTranslate(cp, mat, cp);

        for (int i = 0; i < 11; ++i) {
            Triangle(&cp, &tpts[i], &tpts[i + 1], &txtOrigin, &txtXY[i],
                     &txtXY[i + 1], c, lifeSpan, texture);
        }
        Triangle(&cp, &tpts[11], &tpts[0], &txtOrigin, &txtXY[11], &txtXY[0],
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

    uv0.x = 0.0f;
    uv0.y = 1.0f;
    uv1.x = 1.0f;
    uv1.y = 1.0f;
    uv2.x = 1.0f;
    uv2.y = 0.0f;
    uv3.x = 0.0f;
    uv3.y = 0.0f;
    Triangle(pt0, pt1, pt2, &uv0, &uv1, &uv2, c, lifeSpan, texture);
    Triangle(pt2, pt3, pt0, &uv2, &uv3, &uv0, c, lifeSpan, texture);
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
    UMath::Vector4 pt4;
    UMath::Vector4 vector4;

    pt4.x = pt.x;
    pt4.y = pt.y;
    vector4.x = vec.x;
    vector4.y = vec.y;
    pt4.z = pt.z;
    pt4.w = 1.0f;
    vector4.w = 1.0f;
    vector4.z = vec.z;
    Vector(pt4, vector4, scale, c, lifeSpan, texture);
}

void DebugDraw::DrawAll() {
    bMatrix4 *matrix;
    eView *view = eGetView(EVIEW_PLAYER1, false);
    static int lastSimFrame;
    int simDecayFrames;

    if (!view) {
        return;
    }
    matrix = static_cast<bMatrix4 *>(eFrameMalloc(sizeof(bMatrix4)));
    if (!matrix) {
        return;
    }

    bIdentity(matrix);
    view->BiasMatrixForZSorting(matrix, 0.96f);

    {
        if (this->fNumTriPrims > 0) {
            ePoly poly;
            int iCurVert;
            int iNumVertsLeft;

            iCurVert = 0;
            iNumVertsLeft = this->fNumTriPrims * 3;
            for (; iNumVertsLeft > 0; iNumVertsLeft -= 3, iCurVert += 3) {
                poly.Vertices[0] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert]);
                poly.Vertices[1] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert + 1]);
                poly.Vertices[2] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert + 2]);

                view->Render(&poly, this->fTextureInfo, matrix, 0, 0.0f);
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
                    UMath::Vector4 *pVertex = &this->fLinVertList[iCurVert];
                    unsigned int *pColour = &this->fLinColourList[iCurVert];
                    unsigned int colour0 = pColour[0];
                    unsigned int colour1;

                    iNumVertsLeft -= 2;
                    verts[0].u = 0.5f;
                    verts[0].v = 0.5f;
                    iCurVert += 2;
                    colour1 = pColour[1];
                    verts[0].y = -pVertex->x;
                    verts[0].x = pVertex->z;
                    verts[0].z = pVertex->y;
                    verts[0].colour = (colour0 & 0xff) << 16 | colour0 & 0xff000000 |
                                      (colour0 & 0xff0000) >> 16 | colour0 & 0xff00;
                    verts[1].y = -pVertex[1].x;
                    verts[1].x = pVertex[1].z;
                    verts[1].z = pVertex[1].y;
                    verts[1].colour = (colour1 & 0xff) << 16 | colour1 & 0xff000000 |
                                      (colour1 & 0xff0000) >> 16 | colour1 & 0xff00;
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
                    while (this->fTriPrimList[endIndex].fLifeSpan == 0) {
                        endIndex--;
                        if (endIndex < s) {
                            break;
                        }
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
                    while (this->fLinPrimList[endIndex].fLifeSpan == 0) {
                        endIndex--;
                        if (endIndex < s) {
                            break;
                        }
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
