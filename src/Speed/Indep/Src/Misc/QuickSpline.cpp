#include "QuickSpline.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "SpeedChunks.hpp"

BOOL BasisMatrixInitDone = false;

bMatrix4 OverhauserBasisMatricies[3];
bPList<QuickSpline> LoadedSplineList;

void InitBasisMatricies() {
    if (BasisMatrixInitDone) {
        return;
    }

    bFill(&OverhauserBasisMatricies[0].v0, 0.0f, -1.0f, 2.0f, -1.0f);
    bFill(&OverhauserBasisMatricies[0].v1, 2.0f, 0.0f, -5.0f, 3.0f);
    bFill(&OverhauserBasisMatricies[0].v2, 0.0f, 1.0f, 4.0f, -3.0f);
    bFill(&OverhauserBasisMatricies[0].v3, 0.0f, 0.0f, -1.0f, 1.0f);

    OverhauserBasisMatricies[0].v0 *= 0.5f;
    OverhauserBasisMatricies[0].v1 *= 0.5f;
    OverhauserBasisMatricies[0].v2 *= 0.5f;
    OverhauserBasisMatricies[0].v3 *= 0.5f;

    bFill(&OverhauserBasisMatricies[1].v0, -1.0f, 4.0f, -3.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[1].v1, 0.0f, -10.0f, 9.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[1].v2, 1.0f, 8.0f, -9.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[1].v3, 0.0f, -2.0f, 3.0f, 0.0f);

    OverhauserBasisMatricies[1].v0 *= 0.5f;
    OverhauserBasisMatricies[1].v1 *= 0.5f;
    OverhauserBasisMatricies[1].v2 *= 0.5f;
    OverhauserBasisMatricies[1].v3 *= 0.5f;

    bFill(&OverhauserBasisMatricies[2].v0, 4.0f, -6.0f, 0.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[2].v1, -10.0f, 18.0f, 0.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[2].v2, 8.0f, -18.0f, 0.0f, 0.0f);
    bFill(&OverhauserBasisMatricies[2].v3, -2.0f, 6.0f, 0.0f, 0.0f);

    OverhauserBasisMatricies[2].v0 *= 0.5f;
    OverhauserBasisMatricies[2].v1 *= 0.5f;
    OverhauserBasisMatricies[2].v2 *= 0.5f;
    OverhauserBasisMatricies[2].v3 *= 0.5f;

    bTransposeMatrix(&OverhauserBasisMatricies[0], &OverhauserBasisMatricies[0]);
    bTransposeMatrix(&OverhauserBasisMatricies[1], &OverhauserBasisMatricies[1]);
    bTransposeMatrix(&OverhauserBasisMatricies[2], &OverhauserBasisMatricies[2]);

    BasisMatrixInitDone = true;
}

// Stripped
QuickSpline::QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, int max_control_points) {}

// Stripped
QuickSpline::QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, bVector4 *control_point_buffer,
                         int num_buffer_entries) {}

// Stripped
QuickSpline::~QuickSpline() {}

// Stripped
void QuickSpline::Init(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type) {}

// Stripped
void QuickSpline::DoSnapshot(struct ReplaySnapshot *snapshot, int num_fields) {}

// Stripped
void QuickSpline::SetNumControlPoints(int num_control_points) {}

void QuickSpline::FixupEndpoints() {
    int num_control_points = NumControlPoints;
    if (num_control_points == 0) {
        return;
    }
    bVector4 *control_points = pControlPoints;

    switch (EndPointType) {
        case QUICKSPLINE_LOOP:
            control_points[-1] = control_points[num_control_points - 1];
            control_points[num_control_points] = control_points[0];
            control_points[num_control_points + 1] = control_points[1];
            if (Length != 0.0f) {
                control_points[-1].w = control_points[num_control_points - 1].w - Length;
                control_points[num_control_points].w = Length + control_points->w;
                control_points[num_control_points + 1].w = Length + control_points[1].w;
            }
            break;
        case QUICKSPLINE_LINE:
            control_points[-1] = control_points[0];
            control_points[num_control_points] = control_points[num_control_points - 1];
            if (Length != 0.0f) {
                control_points[-1].w = -control_points[1].w;
                control_points[num_control_points].w = (2 * Length) - control_points[num_control_points - 1].w;
            }
            break;
        case QUICKSPLINE_EXTRAPOLATED: {
            bVector4 v;
            v = control_points[1] - control_points[0];
            bScaleAdd(&control_points[-1], &control_points[0], &v, -1.0f);

            v = control_points[num_control_points - 1] - control_points[num_control_points - 2];
            bScaleAdd(&control_points[num_control_points], &control_points[num_control_points - 1], &v, 1.0f);
            break;
        }
    }
}

// Stripped
void QuickSpline::EvaluateSpline(bVector4 *point, float t, int deriv, const bVector4 *control_points) {}

// Stripped
float QuickSpline::ClampParam(float param) {
    return 0.0f;
}

// Stripped
void QuickSpline::GenerateExtrapolatedControlPoints(bVector4 *extrapolated_control_points, int control_point_num) {}

// Stripped
bVector4 *QuickSpline::GetPoint(bVector4 *point, float param, int deriv) {
    return nullptr;
}

// Stripped
int QuickSpline::FindClosestControlPoint(const bVector3 &point, int start_point_number) {
    return 0;
}

// Stripped
int QuickSpline::FindClosestControlPoint(const bVector3 &point) {
    return 0;
}

// Stripped
float QuickSpline::FindClosestParam(const bVector3 &point, float initial_param, float error_tolerance, int use_euans_hack) {
    return 0.0f;
}

// Stripped
float QuickSpline::FindClosestLateralOffset(const bVector3 &point, float initial_param, float *closest_param, float error_tolerance,
                                            int use_euans_hack) {
    return 0.0f;
}

// Stripped
float QuickSpline::GetParam(float f_distance) {
    return 0.0f;
}

void QuickSpline::CalibrateLength(int num_steps) {}

void QuickSpline::MemoryImageLoad(bVector4 *control_point_buffer) {
    pControlPoints = &control_point_buffer[1];
    ControlPointsDirty = false;
    pBasisMatricies = nullptr;
    pControlPointBuffer = control_point_buffer;
    if (BasisType == QUICKSPLINE_OVERHAUSER) {
        pBasisMatricies = OverhauserBasisMatricies;
    }
    InitBasisMatricies();
}

// Stripped
void QuickSpline::MemoryImageUnload() {}

bool QuickSpline::HasKink() {
    if (ControlPointsDirty) {
        FixupEndpoints();
        ControlPointsDirty = false;
    }
    int nPoints = NumControlPoints + 3;
    if (nPoints < 3) {
        return false;
    }
    bool b_ret = false;
    bVector3 *pPoints = reinterpret_cast<bVector3 *>(pControlPointBuffer);
    for (int i = 2; i < nPoints; i++) {
        bVector3 v0 = pPoints[i - 1] - pPoints[i - 2];
        bVector3 v1 = pPoints[i] - pPoints[i - 1];
        if (bDot(&v0, &v1) < 0.0f) {
            b_ret = true;
        }
    }

    return b_ret;
}

// Stripped
void QuickSpline::PlotToCaffeine() {}

// Stripped
void QuickSpline::EmptyCaffeineLayer() {}

// Stripped
void QuickSpline::MakeControlPointsEquiDistant() {}

// Stripped
void QuickSpline::ZeroAllZValues() {}

BOOL LoaderQuickSpline(bChunk *pChunk) {
    if (pChunk->GetID() == BCHUNK_QUICK_SPLINES) {
        for (bChunk *c = pChunk->GetFirstChunk(); c < pChunk->GetLastChunk(); c = c->GetNext()) {
            bChunk *pInstance = c->GetFirstChunk();
            bChunk *pControlPoints = pInstance->GetNext();
            QuickSpline *pSpline = reinterpret_cast<QuickSpline *>(pInstance->GetAlignedData(16));
            bPlatEndianSwap(&pSpline->nHash);
            // TODO this breaks if the enum size is not 4
            bPlatEndianSwap(reinterpret_cast<unsigned int *>(&pSpline->EndPointType));
            bPlatEndianSwap(reinterpret_cast<unsigned int *>(&pSpline->BasisType));
            bPlatEndianSwap(&pSpline->MaxParam);
            bPlatEndianSwap(&pSpline->MinParam);
            bPlatEndianSwap(&pSpline->Length);
            bPlatEndianSwap(&pSpline->MaxControlPoints);
            bPlatEndianSwap(&pSpline->NumControlPoints);

            for (int i = 0; i < pSpline->MaxControlPoints + 3; i++) {
                bPlatEndianSwap(&pSpline->pControlPoints[i]);
            }
            pSpline->MemoryImageLoad(reinterpret_cast<bVector4 *>(pControlPoints->GetAlignedData(16)));
            LoadedSplineList.AddTail(pSpline);
            pSpline->HasKink();
        }
        return true;
    } else {
        return false;
    }
}

void UnloadSpline(QuickSpline *pSplineToRemove) {
    unsigned int hash_to_remove = pSplineToRemove->GetHash();

    for (bPNode *pNode = LoadedSplineList.GetHead(); pNode != LoadedSplineList.EndOfList(); pNode = pNode->GetNext()) {
        QuickSpline *pSpline = reinterpret_cast<QuickSpline *>(pNode->GetObject());
        if (hash_to_remove == pSpline->GetHash()) {
            LoadedSplineList.Remove(pNode);
            return;
        }
    }
}

BOOL UnloaderQuickSpline(bChunk *pChunk) {
    if (pChunk->GetID() == BCHUNK_QUICK_SPLINES) {
        for (bChunk *c = pChunk->GetFirstChunk(); c < pChunk->GetLastChunk(); c = c->GetNext()) {
            bChunk *pInstance = c->GetFirstChunk();
            QuickSpline *pSpline = reinterpret_cast<QuickSpline *>(pInstance->GetAlignedData(16));
            UnloadSpline(pSpline);
        }
        return true;
    } else {
        return false;
    }
}
