#include "Speed/Indep/Src/Debug/Common/DebugDraw.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"

extern float Tweak_drawRange;

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
            while (iNumVertsLeft > 0) {
                poly.Vertices[0] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert]);
                poly.Vertices[1] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert + 1]);
                poly.Vertices[2] = Coord4ToSwizzledbVec(&this->fTriVertList[iCurVert + 2]);

                iNumVertsLeft -= 3;
                iCurVert += 3;
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
