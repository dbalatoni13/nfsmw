#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Clans.hpp"
#include "Skids.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/Espresso.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

void SkidSegment::SetPoints(bVector3 *position, bVector3 *delta_position) {
    const float scale_factor = 64.0f;
    float x = position->x;
    float y = position->y;
    float z = position->z;
    int dx = static_cast<int>(delta_position->x * scale_factor);
    int dy = static_cast<int>(delta_position->y * scale_factor);
    int dz = static_cast<int>(delta_position->z * scale_factor);

    Position[0] = x;
    Position[1] = y;
    Position[2] = z;
    DeltaPosition[0] = static_cast<signed char>(dx);
    DeltaPosition[1] = static_cast<signed char>(dy);
    DeltaPosition[2] = static_cast<signed char>(dz);
}

bTList<SkidSet> SkidSetList;
SlotPool *SkidSetSlotPool = nullptr;
int PlotSkidsInCaffeine = 0;
int PlotSkidPointsInCaffeine = 0;

// TODO use these
// static const float MaxSkidLength;
// static const float MinSkidLength;
static const float SkidWayTooCurvyFactor = 0.2f;
static const float SkidTooCurvyFactor = 0.002f;
static const float SkidExtraHeight = 0.05f;
static const float MaxSkidDistanceFromCar = 4.0f;
static const float SkidFarClip = 4.0f;
static const float SkidFullIntensityFarClip = 10.0f;
// static const int ClearSkids;

TextureInfo *SkidTextureInfo[29];

void SkidSegment::GetPoints(bVector3 *position, bVector3 *delta_position) {
    const float scale_factor = 1.0f / 64.0f;
    float x;
    float y;
    float z;
    float dx;
    float dy;
    float dz;

    dx = static_cast<float>(DeltaPosition[0]) * scale_factor;
    dy = static_cast<float>(DeltaPosition[1]) * scale_factor;
    y = Position[1];
    dz = static_cast<float>(DeltaPosition[2]) * scale_factor;
    z = Position[2];
    x = Position[0];

    position->x = x;
    position->y = y;
    position->z = z;

    if (delta_position) {
        delta_position->x = dx;
        delta_position->y = dy;
        delta_position->z = dz;
    }
}

void SkidSegment::GetEndPoints(bVector3 *left_point, bVector3 *right_point) {
    const float scale_factor = 1.0f / 64.0f;
    float x;
    float y;
    float z;
    float dx;
    float dy;
    float dz;

    x = Position[0];
    dx = static_cast<float>(DeltaPosition[0]) * scale_factor;
    y = Position[1];
    z = Position[2];
    dy = static_cast<float>(DeltaPosition[1]) * scale_factor;
    dz = static_cast<float>(DeltaPosition[2]) * scale_factor;
    left_point->x = x + dx;
    left_point->y = y + dy;
    left_point->z = z + dz;
    right_point->x = x - dx;
    right_point->y = y - dy;
    right_point->z = z - dz;
}

SkidSet::SkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    TheTerrainType = terrain_type;
    NumSkidSegments = 0;
    pSkidMaker = skid_maker;
    Position = *position;
    bInitializeBoundingBox(&BBoxMin, &BBoxMax, position);
    bCopy(&BBoxCentre, position);

    Clan *clan = GetClan(position);
    pClan = clan;
    pClanNode = pClan->SkidSetList.AddTail(this);

    AddSegment(position, delta_position, false, intensity);
}

SkidSet::~SkidSet() {
    if (pSkidMaker) {
        pSkidMaker->MakeNoSkid();
    }

    pClan->SkidSetList.Remove(pClanNode);
}

int SkidSet::AddSegment(bVector3 *position, bVector3 *delta_position, bool skid_is_flaming, float intensity) {
    (void)skid_is_flaming;

    bVector3 new_segment_normal;
    bVector3 new_segment_forward;
    float length = 0.0f;
    if (NumSkidSegments > 0) {
        new_segment_forward = *position - *SkidSegments[NumSkidSegments - 1].GetPosition();
        length = bLength(&new_segment_forward);
        bNormalize(&new_segment_normal, &new_segment_forward);
    }

    int expand_last_skid_segment = 0;
    float error;
    if (NumSkidSegments > 1) {
        error = 1.0f - bDot(&new_segment_normal, &LastNormal);
        float new_segment_length = LastSegmentLength + length;
        if (error > SkidWayTooCurvyFactor) {
            FinishedAddingSkids();
            return 0;
        }

        if (new_segment_length < 0.25f) {
            expand_last_skid_segment = 1;
        }
        if (error < SkidTooCurvyFactor) {
            expand_last_skid_segment = 1;
        }
        if (new_segment_length > 3.0f) {
            expand_last_skid_segment = 0;
        }
    }

    if (RemoteCaffeinating && PlotSkidPointsInCaffeine) {
        unsigned int obj = espCreateObject("SkidPoints", "SkidPoint", nullptr);
        espSetObjectPosition(obj, reinterpret_cast<FloatVector *>(position));
        espSetAttributeFloat(obj, "Dot", error);
    }

    SkidSegment *skid_segment;
    if (!expand_last_skid_segment && NumSkidSegments == 8) {
        return 1;
    }

    if (expand_last_skid_segment) {
        skid_segment = &SkidSegments[NumSkidSegments - 1];
        LastSegmentLength += length;
    } else {
        skid_segment = &SkidSegments[NumSkidSegments];
        NumSkidSegments += 1;
        if (NumSkidSegments > 1) {
            LastNormal = new_segment_normal;
        }
        LastSegmentLength = length;
    }

    skid_segment->SetPoints(position, delta_position);
    skid_segment->SetIntensity(static_cast<unsigned char>(intensity * 255.0f));

    bExpandBoundingBox(&BBoxMin, &BBoxMax, position, length);
    pClan->ExpandBoundingBox(&BBoxMin, &BBoxMax);

    bAdd(&BBoxCentre, &BBoxMin, &BBoxMax);
    bScale(&BBoxCentre, &BBoxCentre, 0.5f);
    return 0;
}

void SkidSet::FinishedAddingSkids() {
    if (pSkidMaker) {
        pSkidMaker->pSkidSet = nullptr;
        pSkidMaker = nullptr;

        if (RemoteCaffeinating && PlotSkidsInCaffeine && NumSkidSegments > 1) {
            unsigned int obj = espCreateObject("Skids", "Skid", 0);
            espSetObjectPosition(obj, reinterpret_cast<FloatVector *>(&Position));
            espCreateUserMesh(obj, NumSkidSegments - 1);

            // TODO check if this is right (in Undercover)
            for (int n = 0; n < NumSkidSegments - 1; n++) {
                SkidSegment *skid_segment = &SkidSegments[n];
                SkidSegment *next_skid_segment = &SkidSegments[n + 1];
                bVector3 vertices[4];
                skid_segment->GetEndPoints(&vertices[0], &vertices[3]);
                next_skid_segment->GetEndPoints(&vertices[1], &vertices[2]);
                FloatVector float_vertices[4];

                for (int x = 0; x < 4; x++) {
                    // TODO is this right?
                    bVector3 vertex = vertices[x] - Position;
                    float_vertices[x].x = vertex.x;
                    float_vertices[x].y = vertex.y;
                    float_vertices[x].z = vertex.z;
                }

                espSetUserMeshFace(obj, n, float_vertices);
            }
        }
    }
}

void SkidSet::Render(eView *view, unsigned char intensityReduction) {
    if (!SkidTextureInfo[TheTerrainType]) {
        return;
    }

    bMatrix4 *identity_matrix = eGetIdentityMatrix();
    ePoly poly;
    float extra_height = 0.05f;

    for (int n = 0; n < NumSkidSegments - 1; n++) {
        SkidSegment *skid_segment = &SkidSegments[n];
        SkidSegment *next_skid_segment = &SkidSegments[n + 1];

        skid_segment->GetEndPoints(&poly.Vertices[0], &poly.Vertices[3]);
        next_skid_segment->GetEndPoints(&poly.Vertices[1], &poly.Vertices[2]);
        poly.Vertices[0].z += extra_height;
        poly.Vertices[1].z += extra_height;
        poly.Vertices[2].z += extra_height;
        poly.Vertices[3].z += extra_height;

        unsigned char alpha0 = skid_segment->GetIntensity();
        unsigned char alpha1 = next_skid_segment->GetIntensity();
        if (intensityReduction > alpha0) {
            alpha0 = 0;
        } else {
            alpha0 -= intensityReduction;
        }
        if (intensityReduction > alpha1) {
            alpha1 = 0;
        } else {
            alpha1 -= intensityReduction;
        }

        *reinterpret_cast<unsigned int *>(&poly.Colours[0][0]) = 0x80808080;
        *reinterpret_cast<unsigned int *>(&poly.Colours[1][0]) = 0x80808080;
        *reinterpret_cast<unsigned int *>(&poly.Colours[2][0]) = 0x80808080;
        *reinterpret_cast<unsigned int *>(&poly.Colours[3][0]) = 0x80808080;
        poly.Colours[0][3] = alpha0;
        poly.Colours[1][3] = alpha1;
        poly.Colours[2][3] = alpha1;
        poly.Colours[3][3] = alpha0;
        view->Render(&poly, SkidTextureInfo[TheTerrainType], identity_matrix, 0, 0.0f);
    }
}

SkidSet *CreateNewSkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    if (bIsSlotPoolFull(SkidSetSlotPool)) {
        SkidSet *oldest_skid_set = SkidSetList.RemoveTail();
        if (oldest_skid_set) {
            delete oldest_skid_set;
        }
    }

    SkidSet *skid_set = new SkidSet(skid_maker, position, delta_position, terrain_type, intensity);
    SkidSetList.AddHead(skid_set);
    return skid_set;
}

void SkidMaker::MakeSkid(Car *pCar, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    bool make_flaming_skids = false;
    if (pCar) {
        float distance_from_car = bDistBetween(0, position);
        if (distance_from_car > 4.0f) {
            pCar->GetGeometryPosition();
            return;
        }
    }

    if (!pSkidSet) {
        pSkidSet = CreateNewSkidSet(this, position, delta_position, terrain_type, intensity);
    } else if (pSkidSet->GetTerrainType() != terrain_type || pSkidSet->AddSegment(position, delta_position, make_flaming_skids, intensity) != 0) {
        bVector3 last_position;
        bVector3 last_delta_position;

        pSkidSet->GetLastPoints(&last_position, &last_delta_position);
        float last_intensity = pSkidSet->GetLastIntensity();
        pSkidSet->FinishedAddingSkids();
        SkidSet *new_skid_set = CreateNewSkidSet(this, &last_position, &last_delta_position, terrain_type, last_intensity);
        new_skid_set->AddSegment(position, delta_position, make_flaming_skids, intensity);
        pSkidSet = new_skid_set;
    }
}

void SkidMaker::MakeNoSkid() {
    if (pSkidSet) {
        pSkidSet->FinishedAddingSkids();
    }
}

void InitSkids(int max_skids) {
    if (!SkidSetSlotPool) {
        SkidSetSlotPool = bNewSlotPool(0xF0, max_skids, "SkidSetSlotPool", GetVirtualMemoryAllocParams());
        SkidSetSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    }

    for (int n = 0; n < 29; n++) {
        SkidTextureInfo[n] = 0;
        SkidTextureInfo[n] = GetTextureInfo(bStringHash("SKID_ROAD"), 1, 0);
    }

    PlotSkidsInCaffeine = false;
    PlotSkidPointsInCaffeine = false;
    if (RemoteCaffeinating) {
        PlotSkidsInCaffeine = espGetLayerState("Skids") != 0;
        if (espGetLayerState("SkidPoints")) {
            PlotSkidPointsInCaffeine = true;
        }
    }
}

void CloseSkids() {
    if (SkidSetSlotPool) {
        bDeleteSlotPool(SkidSetSlotPool);
        SkidSetSlotPool = nullptr;
    }

    for (int n = 0; n < 29; n++) {
        SkidTextureInfo[n] = nullptr;
    }
}

void DeleteThisSkid(SkidSet *skid_set) {
    SkidSetList.Remove(skid_set);
    delete skid_set;
}

void DeleteAllSkids() {
    while (!SkidSetList.IsEmpty()) {
        delete SkidSetList.RemoveTail();
    }
}

void RenderSkids(eView *view, Clan *clan) {
    ProfileNode profile_node("TODO", 0);

    for (bPNode *p = clan->SkidSetList.GetHead(); p != clan->SkidSetList.EndOfList(); p = p->GetNext()) {
        SkidSet *skid_set = static_cast<SkidSet *>(p->GetObject());
        eVisibleState visibility = view->GetVisibleState(skid_set->GetBBoxMin(), skid_set->GetBBoxMax(), 0);
        if (visibility != EVISIBLESTATE_NOT) {
            int pixel_size = view->GetPixelSize(bDistBetween(skid_set->GetBBoxCentre(), view->GetCamera()->GetPosition()), 1.0f);
            if (4.0f < static_cast<float>(pixel_size)) {
                unsigned char intensityReduction;
                if (10.0f < static_cast<float>(pixel_size)) {
                    intensityReduction = 0;
                } else {
                    intensityReduction = static_cast<unsigned char>(static_cast<int>(256.0f - (pixel_size - 4.0f) * 42.666668f) & 0xff);
                }

                skid_set->Render(view, intensityReduction);
                SkidSetList.Remove(skid_set);
                SkidSetList.AddHead(skid_set);
            }
        }
    }
}
