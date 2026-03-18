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
    float delta_x = delta_position->x * kSkidSegmentScale_Skids;
    float delta_y = delta_position->y * kSkidSegmentScale_Skids;
    float delta_z = delta_position->z * kSkidSegmentScale_Skids;
    int *this_words = reinterpret_cast<int *>(this);
    int *position_words = reinterpret_cast<int *>(position);
    int position_y = position_words[1];
    int position_z = position_words[2];

    this_words[0] = position_words[0];
    DeltaPosition[2] = static_cast<signed char>(delta_z);
    this_words[1] = position_y;
    this_words[2] = position_z;
    DeltaPosition[0] = static_cast<signed char>(delta_x);
    DeltaPosition[1] = static_cast<signed char>(delta_y);
}

void SkidSegment::GetPoints(bVector3 *position, bVector3 *delta_position) {
    position->x = Position[0];
    position->y = Position[1];
    position->z = Position[2];

    if (delta_position) {
        delta_position->x = static_cast<float>(DeltaPosition[0]) * kInverseSkidSegmentScale_Skids;
        delta_position->y = static_cast<float>(DeltaPosition[1]) * kInverseSkidSegmentScale_Skids;
        delta_position->z = static_cast<float>(DeltaPosition[2]) * kInverseSkidSegmentScale_Skids;
    }
}

void SkidSegment::GetEndPoints(bVector3 *left_point, bVector3 *right_point) {
    float delta_x = static_cast<float>(DeltaPosition[0]) * kInverseSkidSegmentScale_Skids;
    float delta_y = static_cast<float>(DeltaPosition[1]) * kInverseSkidSegmentScale_Skids;
    float delta_z = static_cast<float>(DeltaPosition[2]) * kInverseSkidSegmentScale_Skids;

    left_point->x = Position[0] + delta_x;
    left_point->y = Position[1] + delta_y;
    left_point->z = Position[2] + delta_z;

    right_point->x = Position[0] - delta_x;
    right_point->y = Position[1] - delta_y;
    right_point->z = Position[2] - delta_z;
}

SkidSet::SkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity)
    : pClan(0)
    , pClanNode(0)
    , pSkidMaker(skid_maker)
    , TheTerrainType(terrain_type)
    , NumSkidSegments(0)
{
    Position = *position;
    bInitializeBoundingBox(&BBoxMin, &BBoxMax, position);
    BBoxCentre = *position;

    if (pSkidMaker) {
        pSkidMaker->pSkidSet = this;
    }

    pClan = GetClan(position);
    if (pClan) {
        pClanNode = pClan->SkidSetList.AddTail(this);
    }

    AddSegment(position, delta_position, false, intensity);
}

SkidSet::~SkidSet() {
    if (pSkidMaker) {
        pSkidMaker->MakeNoSkid();
    }

    if (pClan && pClanNode) {
        pClan->SkidSetList.Remove(pClanNode);
        pClanNode = 0;
    }
}

int SkidSet::AddSegment(bVector3 *position, bVector3 *delta_position, bool skid_is_flaming, float intensity) {
    (void)skid_is_flaming;

    float segment_length = 0.0f;
    bVector3 direction;
    if (NumSkidSegments > 0) {
        bVector3 last_position;
        bVector3 last_delta_position;
        SkidSegments[NumSkidSegments - 1].GetPoints(&last_position, &last_delta_position);

        direction.x = position->x - last_position.x;
        direction.y = position->y - last_position.y;
        direction.z = position->z - last_position.z;
        segment_length = bSqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        bNormalize(&direction, &direction);
    }

    bool extend_last_segment = false;
    if (NumSkidSegments > 1) {
        float normal_delta =
            1.0f - (direction.x * LastNormal.x + direction.y * LastNormal.y + direction.z * LastNormal.z);
        float total_length = LastSegmentLength + segment_length;
        if (normal_delta > kSkidDirectionBreakThreshold_Skids) {
            FinishedAddingSkids();
            return 0;
        }

        extend_last_segment = normal_delta < kSkidDirectionMergeThreshold_Skids || total_length < kSkidLengthMergeThreshold_Skids;
        if (total_length > kSkidLengthSplitThreshold_Skids) {
            extend_last_segment = false;
        }
    }

    SkidSegment *segment = 0;
    if (extend_last_segment) {
        segment = &SkidSegments[NumSkidSegments - 1];
        LastSegmentLength += segment_length;
    } else {
        if (NumSkidSegments == kNumSkidSegments_Skids) {
            return 1;
        }

        segment = &SkidSegments[NumSkidSegments];
        NumSkidSegments += 1;
        if (NumSkidSegments > 1) {
            LastNormal = direction;
        }
        LastSegmentLength = segment_length;
    }

    segment->SetPoints(position, delta_position);
    segment->Intensity = static_cast<unsigned char>(intensity * kSkidIntensityScale_Skids);

    bExpandBoundingBox(&BBoxMin, &BBoxMax, position, segment_length);
    if (pClan) {
        bExpandBoundingBox(&pClan->BBoxMin, &pClan->BBoxMax, &BBoxMin, &BBoxMax);
    }

    BBoxCentre.x = (BBoxMin.x + BBoxMax.x) * 0.5f;
    BBoxCentre.y = (BBoxMin.y + BBoxMax.y) * 0.5f;
    BBoxCentre.z = (BBoxMin.z + BBoxMax.z) * 0.5f;
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

    bMatrix4 *identity_matrix = eGetIdentityMatrix();
    ePoly poly;
    float extra_height = 0.05f;

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

        alpha0 = skid_segment->Intensity;
        alpha1 = next_skid_segment->Intensity;
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

        *reinterpret_cast<unsigned int *>(&poly.Colours[0][0]) = 0x80808000 | alpha0;
        *reinterpret_cast<unsigned int *>(&poly.Colours[1][0]) = 0x80808000 | alpha1;
        *reinterpret_cast<unsigned int *>(&poly.Colours[2][0]) = 0x80808000 | alpha1;
        *reinterpret_cast<unsigned int *>(&poly.Colours[3][0]) = 0x80808000 | alpha0;
        reinterpret_cast<eViewSkidRenderShim *>(view)->Render(&poly, SkidTextureInfo[TheTerrainType], identity_matrix, 0,
                                                              0.05f);
    }
}

SkidSet *CreateNewSkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    if (SkidSetSlotPool->IsFull() && !SkidSetList.IsEmpty()) {
        DeleteThisSkid(SkidSetList.GetTail());
    }

    SkidSet *skid_set = new SkidSet(skid_maker, position, delta_position, terrain_type, intensity);
    SkidSetList.AddHead(skid_set);
    return skid_set;
}

void SkidMaker::MakeSkid(Car *pCar, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity) {
    if (pCar && bLength(delta_position) > 4.0f) {
        return;
    }

    if (!pSkidSet) {
        pSkidSet = CreateNewSkidSet(this, position, delta_position, terrain_type, intensity);
    } else if (pSkidSet->TheTerrainType != terrain_type || pSkidSet->AddSegment(position, delta_position, false, intensity) != 0) {
        bVector3 last_position;
        bVector3 last_delta_position;
        SkidSegment *last_segment = &pSkidSet->SkidSegments[pSkidSet->NumSkidSegments - 1];
        float last_intensity;

        last_segment->GetPoints(&last_position, &last_delta_position);
        last_intensity = static_cast<float>(last_segment->Intensity) * (1.0f / 255.0f);
        pSkidSet->FinishedAddingSkids();
        pSkidSet = CreateNewSkidSet(this, &last_position, &last_delta_position, terrain_type, last_intensity);
        pSkidSet->AddSegment(position, delta_position, false, intensity);
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
