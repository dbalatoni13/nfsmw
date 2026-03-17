#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static unsigned int AccumulationBufferNeedsFlush = 0;

void TickSFX() {}

void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view) {
    if (!boundary || !view || boundary->NumPoints <= 0) {
        return;
    }

    bVector3 bbox_min(boundary->BBoxMin.x, boundary->BBoxMin.y, 0.0f);
    bVector3 bbox_max(boundary->BBoxMax.x, boundary->BBoxMax.y, 1.0f);
    if (view->GetPixelSize(&bbox_min, &bbox_max) > 0) {
        AccumulationBufferNeedsFlush = 1;
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
