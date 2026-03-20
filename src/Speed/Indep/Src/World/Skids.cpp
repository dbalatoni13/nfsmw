#include "Skids.hpp"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bVector.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void bInitializeBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point);
void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point, float extra_width);
void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *bbox2_min, const bVector3 *bbox2_max);
int bIsSlotPoolFull(SlotPool *slot_pool);
extern bVector3 ZeroVector;

static const int kNumSkidSegments_Skids = 8;
static const int kNumSkidTextures_Skids = 29;
static const float kSkidSegmentScale_Skids = 64.0f;
static const float kInverseSkidSegmentScale_Skids = 1.0f / 64.0f;
static const float kSkidDirectionBreakThreshold_Skids = 0.2f;
static const float kSkidDirectionMergeThreshold_Skids = 0.002f;
static const float kSkidLengthMergeThreshold_Skids = 0.25f;
static const float kSkidLengthSplitThreshold_Skids = 3.0f;
static const float kSkidIntensityScale_Skids = 255.0f;

class eViewSkidRenderShim : public eView {
  public:
    void Render(ePoly *poly, TextureInfo *texture_info, bMatrix4 *matrix, int flags, float z_bias);
};

SlotPool *SkidSetSlotPool = 0;
int PlotSkidsInCaffeine = 0;
int PlotSkidPointsInCaffeine = 0;
bTList<SkidSet> SkidSetList;
TextureInfo *SkidTextureInfo[kNumSkidTextures_Skids];

inline void *SkidSet::operator new(size_t) {
    return bMalloc(SkidSetSlotPool);
}

inline void SkidSet::operator delete(void *ptr) {
    bFree(SkidSetSlotPool, ptr);
}

void SkidSegment::SetPoints(bVector3 *position, bVector3 *delta_position) {
    float delta_x;
    float delta_y;
    float delta_z;
    float position_y = position->y;
    float position_z = position->z;

    delta_x = delta_position->x * kSkidSegmentScale_Skids;
    delta_y = delta_position->y * kSkidSegmentScale_Skids;
    delta_z = delta_position->z * kSkidSegmentScale_Skids;

    Position[0] = position->x;
    DeltaPosition[2] = static_cast<signed char>(delta_z);
    Position[1] = position_y;
    Position[2] = position_z;
    DeltaPosition[0] = static_cast<signed char>(delta_x);
    DeltaPosition[1] = static_cast<signed char>(delta_y);
}

void SkidSegment::GetPoints(bVector3 *position, bVector3 *delta_position) {
    const float scale_factor = kInverseSkidSegmentScale_Skids;
    float x;
    float y;
    float z;
    float dx;
    float dy;
    float dz;

    dx = static_cast<float>(DeltaPosition[0]) * scale_factor;
    x = Position[0];
    dy = static_cast<float>(DeltaPosition[1]) * scale_factor;
    dz = static_cast<float>(DeltaPosition[2]) * scale_factor;
    z = Position[2];
    y = Position[1];

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
    const float scale_factor = kInverseSkidSegmentScale_Skids;
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
    left_point->x = x + dx;
    left_point->y = y + dy;
    dz = static_cast<float>(DeltaPosition[2]) * scale_factor;
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
    BBoxCentre = *position;

    pClan = GetClan(position);
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

    bVector3 new_segment_forward;
    float length = 0.0f;
    if (NumSkidSegments > 0) {
        bVector3 new_segment_normal = *position - *SkidSegments[NumSkidSegments - 1].GetPosition();
        bVector3 new_segment_delta(new_segment_normal);
        length = bLength(&new_segment_delta);
        bNormalize(&new_segment_forward, &new_segment_delta);
    }

    int expand_last_skid_segment = 0;
    if (NumSkidSegments > 1) {
        float error = 1.0f - bDot(&new_segment_forward, &LastNormal);
        float new_segment_length = LastSegmentLength + length;
        if (error > kSkidDirectionBreakThreshold_Skids) {
            FinishedAddingSkids();
            return 0;
        }

        if (new_segment_length < kSkidLengthMergeThreshold_Skids) {
            expand_last_skid_segment = 1;
        }
        if (error < kSkidDirectionMergeThreshold_Skids) {
            expand_last_skid_segment = 1;
        }
        if (new_segment_length > kSkidLengthSplitThreshold_Skids) {
            expand_last_skid_segment = 0;
        }
    }

    SkidSegment *skid_segment;
    if (expand_last_skid_segment) {
        skid_segment = &SkidSegments[NumSkidSegments - 1];
        LastSegmentLength += length;
    } else if (NumSkidSegments == kNumSkidSegments_Skids) {
        return 1;
    } else {
        skid_segment = &SkidSegments[NumSkidSegments];
        NumSkidSegments += 1;
        if (NumSkidSegments > 1) {
            LastNormal = new_segment_forward;
        }
        LastSegmentLength = length;
    }

    skid_segment->SetPoints(position, delta_position);
    skid_segment->SetIntensity(static_cast<unsigned char>(intensity * kSkidIntensityScale_Skids));

    bExpandBoundingBox(&BBoxMin, &BBoxMax, position, length);
    pClan->ExpandBoundingBox(&BBoxMin, &BBoxMax);

    bAdd(&BBoxCentre, &BBoxMin, &BBoxMax);
    bScale(&BBoxCentre, &BBoxCentre, 0.5f);
    return 0;
}

void SkidSet::FinishedAddingSkids() {
    if (pSkidMaker) {
        pSkidMaker->pSkidSet = 0;
        pSkidMaker = 0;
    }
}

void SkidSet::Render(eView *view, unsigned char intensityReduction) {
    if (!SkidTextureInfo[TheTerrainType]) {
        return;
    }

    ePoly poly;
    bMatrix4 *identity_matrix = &eMathIdentityMatrix;
    float extra_height = 0.05f;
    const unsigned int skid_colour = 0x80808080;

    for (int n = 0; n < NumSkidSegments - 1; n++) {
        SkidSegment *skid_segment = &SkidSegments[n];
        SkidSegment *next_skid_segment = &SkidSegments[n + 1];
        unsigned char alpha0;
        unsigned char alpha1;

        skid_segment->GetEndPoints(&poly.Vertices[0], &poly.Vertices[3]);
        next_skid_segment->GetEndPoints(&poly.Vertices[1], &poly.Vertices[2]);
        poly.Vertices[0].z += extra_height;
        poly.Vertices[1].z += extra_height;
        poly.Vertices[2].z += extra_height;
        poly.Vertices[3].z += extra_height;

        alpha0 = skid_segment->GetIntensity();
        alpha1 = next_skid_segment->GetIntensity();
        if (alpha0 < intensityReduction) {
            alpha0 = 0;
        } else {
            alpha0 -= intensityReduction;
        }
        if (alpha1 < intensityReduction) {
            alpha1 = 0;
        } else {
            alpha1 -= intensityReduction;
        }

        *reinterpret_cast<unsigned int *>(&poly.Colours[0][0]) = skid_colour;
        *reinterpret_cast<unsigned int *>(&poly.Colours[1][0]) = skid_colour;
        *reinterpret_cast<unsigned int *>(&poly.Colours[2][0]) = skid_colour;
        *reinterpret_cast<unsigned int *>(&poly.Colours[3][0]) = skid_colour;
        poly.Colours[0][3] = alpha0;
        poly.Colours[1][3] = alpha1;
        poly.Colours[2][3] = alpha1;
        poly.Colours[3][3] = alpha0;
        reinterpret_cast<eViewSkidRenderShim *>(view)->Render(&poly, SkidTextureInfo[TheTerrainType], identity_matrix, 0,
                                                              0.05f);
    }
}

SkidSet *CreateNewSkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    if (bIsSlotPoolFull(SkidSetSlotPool)) {
        SkidSet *oldest_skid_set = SkidSetList.GetTail();
        if (oldest_skid_set != SkidSetList.EndOfList()) {
            DeleteThisSkid(oldest_skid_set);
        }
    }

    SkidSet *skid_set = new SkidSet(skid_maker, position, delta_position, terrain_type, intensity);
    SkidSetList.AddHead(skid_set);
    return skid_set;
}

void SkidMaker::MakeSkid(Car *pCar, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    bool make_flaming_skids = false;
    if (pCar) {
        float distance_from_car = bDistBetween(&ZeroVector, delta_position);
        if (distance_from_car > 4.0f) {
            return;
        }
    }

    if (!pSkidSet) {
        pSkidSet = CreateNewSkidSet(this, position, delta_position, terrain_type, intensity);
    } else if (pSkidSet->GetTerrainType() != terrain_type ||
               pSkidSet->AddSegment(position, delta_position, make_flaming_skids, intensity) != 0) {
        bVector3 last_position;
        bVector3 last_delta_position;
        float last_intensity;

        pSkidSet->GetLastPoints(&last_position, &last_delta_position);
        last_intensity = pSkidSet->GetLastIntensity();
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
        SkidSetSlotPool->Flags = static_cast<SlotPoolFlags>(SkidSetSlotPool->Flags & ~1);
    }

    for (int i = 0; i < kNumSkidTextures_Skids; i++) {
        SkidTextureInfo[i] = 0;
        SkidTextureInfo[i] = GetTextureInfo(bStringHash("SKID_ROAD"), 1, 0);
    }

    PlotSkidsInCaffeine = 0;
    PlotSkidPointsInCaffeine = 0;
}

void CloseSkids() {
    if (SkidSetSlotPool) {
        bDeleteSlotPool(SkidSetSlotPool);
        SkidSetSlotPool = 0;
    }

    for (int i = 0; i < kNumSkidTextures_Skids; i++) {
        SkidTextureInfo[i] = 0;
    }
}

void DeleteThisSkid(SkidSet *skid_set) {
    SkidSetList.Remove(skid_set);
    if (skid_set) {
        delete skid_set;
    }
}

void DeleteAllSkids() {
    while (!SkidSetList.IsEmpty()) {
        DeleteThisSkid(SkidSetList.GetTail());
    }
}

void RenderSkids(eView *view, Clan *clan) {
    ProfileNode profile_node("TODO", 0);

    for (bPNode *p = clan->SkidSetList.GetHead(); p != clan->SkidSetList.EndOfList(); p = p->GetNext()) {
        SkidSet *skid_set = reinterpret_cast<SkidSet *>(p->GetObject());
        eVisibleState visibility = view->GetVisibleState(&skid_set->BBoxMin, &skid_set->BBoxMax, 0);
        if (visibility != EVISIBLESTATE_NOT) {
            int pixel_size = view->GetPixelSize(1.0f, bDistBetween(&skid_set->BBoxCentre, view->GetCamera()->GetPosition()));
            if (4.0f < static_cast<float>(pixel_size)) {
                unsigned char intensityReduction = 0;
                if (static_cast<float>(pixel_size) <= 10.0f) {
                    intensityReduction = static_cast<unsigned char>(static_cast<int>(256.0f - (pixel_size - 4.0f) * 42.666668f) & 0xff);
                }

                skid_set->Render(view, intensityReduction);
                SkidSetList.Remove(skid_set);
                SkidSetList.AddHead(skid_set);
            }
        }
    }
}
