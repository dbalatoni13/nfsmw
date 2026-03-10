#include "ICEReplay.hpp"
#include "ICEMover.hpp"

float PredictAverageAir(float fSeconds, float *pHighest, float *pLongest, bool b_grounded_abort);
float GetRecentCurvature();

static float sPredictedAir;
static float sRecentCurvature;

static float ReplayNosScore(ICEAnchor *p_car) {
    float ret = 0.0f;

    if (p_car->IsNosEngaged()) {
        ret = p_car->GetNosPercentageLeft() * 0.25f + 0.5f;
    }

    return ret;
}

static bool ReplayNosMirror(ICEAnchor *) {
    return false;
}

static float ReplayJumpScore(ICEAnchor *p_car) {
    float ret = p_car->GetUpVector()->z < 0.0f ? 1.0f : 0.0f;

    if (p_car->IsTouchingGround() != 0) {
        float highest = 0.0f;
        float longest = 0.0f;

        PredictAverageAir(3.0f, &highest, &longest, true);
        if (1.0f < highest && 1.5f < longest) {
            ret = 1.0f;
        }
    }

    return ret * 5.0f;
}

static bool ReplayJumpMirror(ICEAnchor *) {
    return false;
}

static float ReplaySpeedScore(ICEAnchor *p_car) {
    float ret = 0.0f;

    if (10.0f < p_car->GetVelocityMagnitude()) {
        ret = bClamp((p_car->GetVelocityMagnitude() / p_car->GetTopSpeed()) * 0.7f + 0.3f, 0.0f, 1.0f);
    }

    return ret;
}

static bool ReplaySpeedMirror(ICEAnchor *) {
    return false;
}

static float ReplayCornerScore(ICEAnchor *p_car) {
    float ret = 0.0f;

    if (5.0f < p_car->GetVelocityMagnitude()) {
        ret = bClamp(bAbs(GetRecentCurvature()) * 20.0f, 0.0f, 1.0f);
    }

    return ret;
}

static bool ReplayCornerMirror(ICEAnchor *) {
    return GetRecentCurvature() < 0.0f;
}

static float ReplayBurnoutScore(ICEAnchor *p_car) {
    float speed = p_car->GetVelocityMagnitude();

    if (speed != bClamp(speed, -10.0f, 20.0f)) {
        return 0.0f;
    }

    {
        float score = bClamp(p_car->GetForwardSlip() * 0.1f - 1.0f, 0.0f, 1.0f);

        if (speed != bClamp(speed, -1.0f, 1.0f)) {
            return score;
        }
        if (p_car->GetRPM() <= 3000.0f) {
            return score;
        }

        return score + 10.9999895f;
    }
}

static bool ReplayBurnoutMirror(ICEAnchor *) {
    return false;
}

static float ReplayPowerSlideScore(ICEAnchor *p_car) {
    if (p_car->GetVelocityMagnitude() <= 4.0f) {
        return 0.0f;
    }

    {
        float score = (bAbs(p_car->GetSlipAngle()) * 360.0f - 5.0f) * 0.025f;

        if (score <= 0.0f) {
            return 0.0f;
        }

        return bClamp(score + 0.1f, 0.0f, 1.0f);
    }
}

static bool ReplayPowerSlideMirror(ICEAnchor *p_car) {
    return p_car->GetSlipAngle() < 0.0f;
}

static ICEReplayCategory ReplayCategories[] = {
    {"NOS", 0x40DFADC5, "ReplayNos", 0xF6F4912B, ReplayNosScore, ReplayNosMirror},
    {"Jump", 0xC9CFEAFB, "ReplayJump", 0x17911633, ReplayJumpScore, ReplayJumpMirror},
    {"Speed", 0x41862FE6, "ReplaySpeed", 0xC0E543F7, ReplaySpeedScore, ReplaySpeedMirror},
    {"Corner", 0xA38C8885, "ReplayCorner", 0xE12BD1A8, ReplayCornerScore, ReplayCornerMirror},
    {"Burnout", 0x6512F314, "ReplayBurnout", 0x87EA4B2F, ReplayBurnoutScore, ReplayBurnoutMirror},
    {"PowerSlide", 0x9FD09E0E, "ReplaySlide", 0xD7BFCC6F, ReplayPowerSlideScore, ReplayPowerSlideMirror},
};

namespace ICE {

int GetReplayCategoryNumElements() {
    return 6;
}

unsigned int GetReplayCategoryHash(int category) {
    return ReplayCategories[category].nCategoryHash;
}

ICEReplayCategory *GetReplayCategory(unsigned int category_hash) {
    for (int i = 0; i < 6; i++) {
        if (category_hash == ReplayCategories[i].nCategoryHash) {
            return &ReplayCategories[i];
        }
    }

    return 0;
}

} // namespace ICE

namespace ICEReplay {

ICETrack *RecentlyUsedTracks[3];
int nRecentlyUsedIndex;

bool WasRecentlyUsed(ICETrack *track) {
    for (int i = 0; i < 3; i++) {
        if (track == RecentlyUsedTracks[i]) {
            return true;
        }
    }

    return false;
}

void ClearRecentlyUsed() {
    nRecentlyUsedIndex = 0;

    for (int i = 0; i < 3; i++) {
        RecentlyUsedTracks[i] = 0;
    }
}

} // namespace ICEReplay

float PredictAverageAir(float fSeconds, float *pHighest, float *pLongest, bool b_grounded_abort) {
    return sPredictedAir;
}

float GetRecentCurvature() {
    return sRecentCurvature;
}
