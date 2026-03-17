#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static unsigned int AccumulationBufferNeedsFlush = 0;

struct TerrainType;
struct eLightContext;

class TopologyCoordinate {
  public:
    bool HasTopology(const bVector2 *point);
    float GetElevation(const bVector3 *point, TerrainType *terrain_type, bVector3 *normal, bool *valid);

  private:
    int mData[2];
};

class eViewRenderShim : public eView {
  public:
    void Render(eModel *model, bMatrix4 *matrix, eLightContext *light_context, unsigned int a4, unsigned int a5, unsigned int a6);
};

extern eModel *pVisibleZoneBoundaryModel;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;

void TickSFX() {}

void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view) {
    if (!boundary || !view || boundary->NumPoints <= 0) {
        return;
    }

    float phase = static_cast<float>((static_cast<int>(WorldTimer.GetPackedTime() * 0.00025f * 262144.0f) & 0xffff)) * 6.1035156e-05f;
    for (int i = 0; i < boundary->NumPoints; i++) {
        int next_index = (i + 1) % boundary->NumPoints;
        bVector2 segment;
        segment.x = boundary->Points[next_index].x - boundary->Points[i].x;
        segment.y = boundary->Points[next_index].y - boundary->Points[i].y;

        float segment_length = bLength(&segment);
        bVector2 direction;
        bNormalize(&direction, &segment);
        if (phase < segment_length) {
            do {
                bVector2 point2;
                bScaleAdd(&point2, &boundary->Points[i], &direction, phase);

                TopologyCoordinate topology_coordinate;
                if (topology_coordinate.HasTopology(&point2)) {
                    bVector3 point3(point2.x, point2.y, 0.0f);
                    point3.z = topology_coordinate.GetElevation(&point3, 0, 0, 0);
                    if (view->GetPixelSize(&point3, 1.0f) > 0) {
                        unsigned char *matrix_memory = CurrentBufferPos;
                        unsigned char *next_buffer_pos = CurrentBufferPos + sizeof(bMatrix4);
                        if (CurrentBufferEnd <= next_buffer_pos) {
                            FrameMallocFailed = 1;
                            FrameMallocFailAmount += sizeof(bMatrix4);
                            matrix_memory = 0;
                            next_buffer_pos = CurrentBufferPos;
                        }

                        CurrentBufferPos = next_buffer_pos;
                        if (matrix_memory) {
                            bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(matrix_memory);
                            PSMTX44Identity(*reinterpret_cast<Mtx44 *>(matrix));
                            matrix->v3.x = point3.x;
                            matrix->v3.y = point3.y;
                            matrix->v3.z = point3.z;
                            matrix->v3.w = 1.0f;
                            reinterpret_cast<eViewRenderShim *>(view)->Render(pVisibleZoneBoundaryModel, matrix, 0, 0, 0, 0);
                        }
                    }
                }

                phase += 4.0f;
            } while (phase < segment_length);
        }

        phase -= segment_length;
    }
}

void DoTunnelBloom(eView *view) {
    if (!view || !view->Active || !view->ScreenEffects) {
        return;
    }

    ScreenEffectDef *glare = &view->ScreenEffects->SE_data[SE_GLARE];
    if (glare->intensity > 0.0f) {
        glare->intensity = bMax(0.0f, glare->intensity - 0.05f);
        glare->a = glare->intensity;
        AccumulationBufferNeedsFlush = 1;
    }
}

void DoTinting(eView *view) {
    if (!view || !view->ScreenEffects) {
        return;
    }

    ScreenEffectDef *tint = &view->ScreenEffects->SE_data[SE_TINT];
    if (tint->intensity > 0.0f) {
        tint->a = tint->intensity;
    }
}

void UpdateAllScreenEFX() {
    for (int view_index = 1; view_index <= 2; view_index++) {
        eView *view = eGetView(view_index, false);
        DoTunnelBloom(view);
        DoTinting(view);
    }
}

void AccumulationBufferFlushed() {
    AccumulationBufferNeedsFlush = 0;
}

unsigned int QueryFlushAccumulationBuffer() {
    return AccumulationBufferNeedsFlush;
}
