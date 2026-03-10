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

Camera *GetCurrentCamera();
unsigned short ConvertLensLengthToFovAngle(float f_lens_mm);

extern bool bMirrorICEData;
int Tweak_ForceICEReplay;

namespace ICEReplay {

bool CameraCutIsGood(ICEData *camera, float param, ICEAnchor *p_car) {
    Camera *old_cam = GetCurrentCamera();
    bool is_it_good = false;
    char sStat[5] = "....";

    if (old_cam == nullptr) {
        return is_it_good;
    }

    bMatrix4 mCarToWorld;
    bMatrix4 mWorldToCar;

    bCopy(&mCarToWorld, reinterpret_cast<const bMatrix4 *>(p_car->GetGeometryOrientation()),
          reinterpret_cast<const bVector3 *>(p_car->GetGeometryPosition()));
    eInvertTransformationMatrix(&mWorldToCar, &mCarToWorld);

    bVector3 *p_pos = old_cam->GetPosition();
    bVector3 *p_tar = old_cam->GetTarget();
    bVector3 v_pos;
    bVector3 v_tar;
    eMulVector(&v_pos, &mWorldToCar, p_pos);
    eMulVector(&v_tar, &mWorldToCar, p_tar);

    bVector3 v_old_dir;
    bSub(&v_old_dir, &v_tar, &v_pos);
    bNormalize(&v_old_dir, &v_old_dir);

    bVector3 *p_old_vel = old_cam->GetVelocityPosition();
    bVector3 v_old_vel;
    eMulVector(&v_old_vel, &mWorldToCar, p_old_vel);
    bNormalize(&v_old_vel, &v_old_vel);

    bVector3 v_eye;
    bVector3 v_look;
    camera->GetEye(0, reinterpret_cast<ICE::Vector3 *>(&v_eye));
    camera->GetLook(0, reinterpret_cast<ICE::Vector3 *>(&v_look));

    bVector3 v_new_dir;
    bSub(&v_new_dir, &v_look, &v_eye);
    bNormalize(&v_new_dir, &v_new_dir);

    bVector3 v_eye1;
    camera->GetEye(1, reinterpret_cast<ICE::Vector3 *>(&v_eye1));
    bVector3 v_new_vel;
    bSub(&v_new_vel, &v_eye1, &v_eye);
    bNormalize(&v_new_vel, &v_new_vel);

    bVector3 v_world_eye;
    eMulVector(&v_world_eye, &mCarToWorld, &v_eye);

    {
        float dir_dot = bDot(&v_old_dir, &v_new_dir);
        float vel_dot = bDot(&v_old_vel, &v_new_vel);
        const float fMinAngleTreshold = -0.5f;
        const float fMaxAngleTreshold = 0.866f;
        const float fPercentTreshold = 0.5f;

        if (v_old_dir.y * v_new_dir.y < 0.0f) {
            return is_it_good;
        }

        if (dir_dot <= fMinAngleTreshold) {
            return is_it_good;
        }

        if (fMaxAngleTreshold <= dir_dot) {
            unsigned short half_angle_old = old_cam->GetFov() >> 1;
            float old_tan = bTan(half_angle_old);
            float old_len = bLength(&v_pos);

            unsigned short fov_angle = ConvertLensLengthToFovAngle(camera->fLens[0]);
            unsigned short half_angle_new = fov_angle >> 1;
            float new_tan = bTan(half_angle_new);
            float new_len = bLength(&v_eye);

            float old_dist = 1.0f / (old_len * old_tan);
            float new_dist = 1.0f / (new_len * new_tan);
            float max_len = bMax(old_dist, new_dist);
            float percent = bAbs(new_dist - old_dist) / max_len;

            if (percent <= fPercentTreshold) {
                return is_it_good;
            }
        } else if (dir_dot <= 0.0f) {
            is_it_good = true;
            return is_it_good;
        }

        if (vel_dot <= 0.0f) {
            return is_it_good;
        }
    }

    is_it_good = true;
    return is_it_good;
}

ICETrack *ChooseGoodCamera(ICEAnchor *p_car, ICEGroup *p_replay_cameras, int num_replay_cameras) {
    struct ReplayCandidate {
        ICETrack *pTrack;
        float fScore;
        bool bMirror;
    };

    ICETrack *good_track = 0;

    if (p_car != 0) {
        float total_score = 0.0f;
        float *scores = new float[num_replay_cameras];

        for (int group_number = 0; group_number < num_replay_cameras; group_number++) {
            ICEGroup *group = &p_replay_cameras[group_number];
            ICEReplayCategory *category = ICE::GetReplayCategory(group->GetHandle());
            float score = category->GetScore(p_car);

            if (Tweak_ForceICEReplay != 0) {
                score += 0.1f;
            }

            if (score < 0.1f) {
                scores[group_number] = 0.0f;
            } else {
                total_score += score;
                scores[group_number] = score;
            }
        }

        if (0.0f < total_score) {
            int num_tracks = 0;

            for (int group_number = 0; group_number < num_replay_cameras; group_number++) {
                if (0.0f < scores[group_number]) {
                    ICEGroup *group = &p_replay_cameras[group_number];
                    num_tracks += group->GetNumTracks();
                }
            }

            if (num_tracks > 0) {
                int num_candidates = 0;
                ReplayCandidate *candidates = new ReplayCandidate[num_tracks];

                for (int group_number = 0; group_number < num_replay_cameras; group_number++) {
                    float score = scores[group_number];

                    if (0.0f < score) {
                        int old_num_candidates = num_candidates;
                        ICEGroup *group = &p_replay_cameras[group_number];

                        for (int track_number = 0; track_number < group->GetNumTracks(); track_number++) {
                            ICETrack *track = group->GetTrack(track_number);
                            ICEData *camera_data = track->GetKey(0);

                            if (camera_data != 0 && camera_data->nType != 0) {
                                ICEReplayCategory *category = ICE::GetReplayCategory(group->GetHandle());
                                bMirrorICEData = category->GetMirror(p_car);

                                if (10.0f <= score ||
                                    (CameraCutIsGood(camera_data, 0.0f, p_car) &&
                                     !WasRecentlyUsed(track))) {
                                    candidates[num_candidates].pTrack = track;
                                    candidates[num_candidates].fScore = score;
                                    candidates[num_candidates].bMirror = bMirrorICEData;
                                    num_candidates++;
                                }

                                bMirrorICEData = 0;
                            }
                        }

                        int candidates_from_this_group = num_candidates - old_num_candidates;
                        if (candidates_from_this_group > 0) {
                            float recip = 1.0f / static_cast<float>(candidates_from_this_group);
                            for (int i = old_num_candidates; i < num_candidates; i++) {
                                candidates[i].fScore *= recip;
                            }
                        }
                    }
                }

                if (num_candidates > 0) {
                    float random_score = bRandom(total_score);

                    for (int i = 0; i < num_candidates; i++) {
                        float score = candidates[i].fScore;
                        if (random_score < score) {
                            good_track = candidates[i].pTrack;
                            bMirrorICEData = candidates[i].bMirror;
                            break;
                        }
                        random_score -= score;
                    }

                    if (good_track == 0) {
                        bMirrorICEData = candidates[0].bMirror;
                        good_track = candidates[0].pTrack;
                    }
                }

                if (candidates != 0) {
                    delete[] candidates;
                }
            }
        }

        if (scores != 0) {
            delete[] scores;
        }
    }

    return good_track;
}

} // namespace ICEReplay
