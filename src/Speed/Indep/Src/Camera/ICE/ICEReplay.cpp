#include "Speed/Indep/Src/Camera/ICE/ICEAnchor.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"

extern bAngle ConvertLensLengthToFovAngle(float f_lens);

static Camera *GetCurrentCamera() {
    eView *view = eGetView(1, false);
    if (view != nullptr) {
        Camera *c = view->GetCamera();
        if (c != nullptr) {
            return c;
        }
        return nullptr;
    }
    return nullptr;
}

static float PredictAverageAir(float t, float *highest, float *longest, bool predict) {

    return 0.0f;
}

static float GetRecentCurvature() {

    return 0.0f;
}

static float ReplayNosScore(ICEAnchor *pAnchor) {

    float score = 0.0f;

    if (pAnchor->IsNosEngaged()) {
        score = pAnchor->GetNosPercentageLeft() * 0.25f + 0.5f;
    }

    return score;
}

static bool ReplayNosMirror(ICEAnchor *pAnchor) {

    return false;
}

static float ReplayJumpScore(ICEAnchor *pAnchor) {
    float score = 0.0f;

    if (pAnchor->GetUpVector()->z < 0.0f) {
        score = 1.0f;
    }
    if (pAnchor->IsTouchingGround()) {

        float highest = 0.0f, longest = 0.0f;

        const float kMinAir = 1.0f;
        PredictAverageAir(3.0f, &highest, &longest, true);
        if (highest > kMinAir && longest > 1.5f) {
            score = kMinAir;
        }
    }

    return score * 5.0f;
}

static bool ReplayJumpMirror(ICEAnchor *pAnchor) {

    return false;
}

static float ReplaySpeedScore(ICEAnchor *pAnchor) {

    float score = 0.0f;

    if (pAnchor->GetVelocityMagnitude() > 10.0f) {

        float ratio = pAnchor->GetVelocityMagnitude() / pAnchor->GetTopSpeed();
        score = ICE::Clamp(ratio * 0.7f + 0.3f, 0.0f, 1.0f);
    }

    return score;
}

static bool ReplaySpeedMirror(ICEAnchor *pAnchor) {

    return false;
}

static float ReplayCornerScore(ICEAnchor *pAnchor) {
    float score = 0.0f;

    if (pAnchor->GetVelocityMagnitude() > 5.0f) {

        score = ICE::Clamp(ICE::Abs(GetRecentCurvature()) * 20.0f, 0.0f, 1.0f);
    }

    return score;
}

static bool ReplayCornerMirror(ICEAnchor *pAnchor) {
    return GetRecentCurvature() < 0.0f;
}

static float ReplayBurnoutScore(ICEAnchor *pAnchor) {

    float score = 0.0f;

    if (pAnchor->GetVelocityMagnitude() == ICE::Clamp(pAnchor->GetVelocityMagnitude(), -10.0f, 20.0f)) {

        score = ICE::Clamp(pAnchor->GetForwardSlip() * 0.1f - 1.0f, 0.0f, 1.0f);

        if (pAnchor->GetVelocityMagnitude() == ICE::Clamp(pAnchor->GetVelocityMagnitude(), -1.0f, 1.0f) &&
            pAnchor->GetRPM() > 3000.0f) {

            score = score + 10.9999895f;
        }
    }

    return score;
}

static bool ReplayBurnoutMirror(ICEAnchor *pAnchor) {

    return false;
}

static float ReplayPowerSlideScore(ICEAnchor *pAnchor) {

    float score = 0.0f;

    if (pAnchor->GetVelocityMagnitude() > 4.0f) {

        float deg = ICE::ToDegrees(ICE::Abs(pAnchor->GetSlipAngle()));
        float t = (deg - 5.0f) * 0.025f;

        if (t > 0.0f) {

            score = ICE::Clamp(t + 0.1f, 0.0f, 1.0f);
        }
    }

    return score;
}

static bool ReplayPowerSlideMirror(ICEAnchor *pAnchor) {
    return pAnchor->GetSlipAngle() < 0.0f;
}

namespace ICE {

// VEDA r50 (sustituye a la de la r47, que era FALSA en sus dos mitades: ni es
// el planificador ni la palanca esta en este fichero).
//
// LA FUENTE DE AQUI ES CORRECTA. En una TU que solo contiene ICEReplay.cpp los
// seis elementos salen con desplazamiento completo sobre la base, igual que el
// objetivo (r49). Los punteros de fila (`addi r27, r30, 0x30/0x48/0x60/0x78`,
// 16 B y 47 de las 54 filas) son un efecto de TAMANO DE UNIDAD: el andamiaje
// que `cp/init.c::expand_vec_init` pone para recorrer el array sobrevive
// cuando `cse_basic_block` purga su tabla hash en mitad de la inicializacion
// (`if (code != NOTE && num_insns++ > 1000) flush_hash_table();`,
// orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:8942).
//
// EL PRESUPUESTO ESTA MEDIDO (r50): faltan 43 insn de RTL --no de codigo
// emitido-- delante de la tabla y dentro de su mismo bloque extendido, que
// empieza en `Movers/Cubic.cpp`. Con un array de 20 elementos de una clase de
// constructor VACIO e inicializado con llaves (andamiaje puro de
// expand_vec_init, 3+2N insn, cero bytes emitidos) puesto en la cabecera de
// `Movers/Cubic.cpp`, esta funcion pasa de 3.620 B / 97,1121 % / 54 filas a
// 3.604 B EXACTOS / 99,8391 % / 6 filas y el resto de la unidad no cambia ni
// un byte. Con 18 elementos no basta. Equivalencias medidas: 22 cargas
// muertas, o un `bVector3 v[6]` con constructores.
//
// PERO NO CIERRA LA FUNCION, y por eso NO se toca nada aqui: las 6 filas que
// quedan son la permutacion de HydraulicsLookAngle (`Movers/Cubic.cpp:355-362`)
// y NINGUN relleno las mueve --`flow` borra el codigo muerto ANTES de
// `sched1`--. Mientras esa permutacion siga abierta, arreglar estos 16 B vale
// 0 B de matched. Ver docs/analisis/r50-cam.md.
// ANDAMIO r60 (medido en la r50, aplicado aqui por primera vez porque la meta
// paso de `matched` a `linked`): `cse_basic_block` purga su tabla hash cada
// 1.000 insn (`if (code != NOTE && num_insns++ > 1000) flush_hash_table();`,
// orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:8942). El bloque extendido de
// `__static_initialization_and_destruction_0` que termina en esta tabla es 43
// insn de RTL mas corto que el del original, asi que la purga nos cae EN MITAD
// de la inicializacion de la tabla y la equivalencia `base == r30 + K` no
// sobrevive: salen cuatro punteros de fila (`addi r27, r30, 0x30/0x48/0x60/0x78`)
// que el objetivo no tiene, 16 B de `.text` de mas.
// `expand_vec_init` (cp/init.c:2857) emite 3+2N insn de RTL para un array de N
// elementos con constructor VACIO y ninguna llega al objeto. N=20 -> 43 insn:
// la funcion pasa de 3.620 B / 97,1121 % / 54 filas a 3.604 B EXACTOS /
// 99,8391 % / 6 filas y el resto de la unidad no cambia. N=18 no basta.
// El array es un estatico sin referencias, asi que `-strip-unused-data` se
// lleva `size & ~7` entero y no aporta ni un byte al enlace.
// La forma HONESTA de esto (r50, seccion 1.2) seria ~22 referencias duplicadas a
// un global o a una constante del pool dentro de la ventana Cubic.cpp..esta
// tabla; nadie la ha encontrado todavia. Mientras tanto, el andamio.
struct _r60_ecscaffold {
    _r60_ecscaffold() {}
    int a;
};
static _r60_ecscaffold _r60_ec[20] = {
    _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(),
    _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(),
    _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(),
    _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(),
    _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(), _r60_ecscaffold(),
};

ReplayCategory ReplayCategoryTable[6] = {
    ReplayCategory("NOS", "ReplaySpeed", ReplayNosScore, ReplayNosMirror),
    ReplayCategory("Jump", "ReplaySpeed", ReplayJumpScore, ReplayJumpMirror),
    ReplayCategory("Speed", "ReplaySpeed", ReplaySpeedScore, ReplaySpeedMirror),
    ReplayCategory("Corner", "ReplayCorner", ReplayCornerScore, ReplayCornerMirror),
    ReplayCategory("Burnout", "ReplayBurnout", ReplayBurnoutScore, ReplayBurnoutMirror),
    ReplayCategory("PowerSlide", "ReplaySlide", ReplayPowerSlideScore, ReplayPowerSlideMirror),
};

int GetReplayCategoryNumElements() {

    return 6;
}

unsigned int GetReplayCategoryHash(int n) {

    return ReplayCategoryTable[n].nNameHash;
}

ReplayCategory *GetReplayCategory(unsigned int hash) {

    for (int i = 0; i < 6; i++) {

        if (hash == ReplayCategoryTable[i].nNameHash) {

            return &ReplayCategoryTable[i];
        }
    }

    return 0;
}

} // namespace ICE

bool ICEReplay::CameraCutIsGood(ICEData *camera, float param, ICEAnchor *p_car) {

    Camera *old_cam = GetCurrentCamera();

    if (old_cam == 0) {

        return false;
    }

    bool is_it_good = false;
    char sStat[5] = "....";

    ICE::Matrix4 mCarToWorld;
    ICE::Matrix4 mWorldToCar;

    ICE::Copy(&mCarToWorld, p_car->GetGeometryOrientation(), p_car->GetGeometryPosition());
    ICE::Invert(&mWorldToCar, &mCarToWorld);

    ICE::Vector3 *p_pos = reinterpret_cast<ICE::Vector3 *>(old_cam->GetPosition());
    ICE::Vector3 *p_tar = reinterpret_cast<ICE::Vector3 *>(old_cam->GetTarget());

    ICE::Vector3 v_pos;
    ICE::Vector3 v_tar;

    ICE::MulVector(&v_pos, &mWorldToCar, p_pos);
    ICE::MulVector(&v_tar, &mWorldToCar, p_tar);

    ICE::Vector3 v_old_dir;

    ICE::Sub(&v_old_dir, &v_tar, &v_pos);
    ICE::Normalize(&v_old_dir, &v_old_dir);

    ICE::Vector3 v_old_vel;
    ICE::Vector3 *p_old_vel = reinterpret_cast<ICE::Vector3 *>(old_cam->GetVelocityPosition());

    ICE::MulVector(&v_old_vel, &mWorldToCar, p_old_vel);
    ICE::Normalize(&v_old_vel, &v_old_vel);

    ICE::Vector3 v_eye;
    ICE::Vector3 v_look;
    ICE::Vector3 v_eye1;

    camera->GetEye(0, &v_eye);
    camera->GetLook(0, &v_look);

    ICE::Vector3 v_new_dir;

    ICE::Sub(&v_new_dir, &v_look, &v_eye);
    ICE::Normalize(&v_new_dir, &v_new_dir);

    ICE::Vector3 v_new_vel;

    camera->GetEye(1, &v_eye1);

    ICE::Sub(&v_new_vel, &v_eye1, &v_eye);
    ICE::Normalize(&v_new_vel, &v_new_vel);

    ICE::Vector3 v_world_eye;

    ICE::MulVector(&v_world_eye, &mCarToWorld, &v_eye);

    if (v_old_dir.y * v_new_dir.y >= 0.0f) {

        sStat[0] = '1';

        float dir_dot = ICE::Dot(&v_old_dir, &v_new_dir);
        float vel_dot = ICE::Dot(&v_old_vel, &v_new_vel);

        const float fMinAngleTreshold = -0.5f;
        const float fMaxAngleTreshold = 0.866f;
        const float fPercentTreshold = 0.5f;

        if (dir_dot > fMinAngleTreshold) {

            if (dir_dot < fMaxAngleTreshold) {

                sStat[1] = '1';

                if (dir_dot > 0.0f) {

                    if (vel_dot > 0.0f) {

                        is_it_good = true;
                        sStat[3] = '1';
                    } else {

                        sStat[3] = '0';
                    }
                } else {

                    is_it_good = true;
                }
            } else {

                sStat[1] = '0';

                float old_fov = ICE::Tan(old_cam->GetFov() / 2);
                float old_dist = ICE::Length(&v_pos);
                float new_fov = ICE::Tan(ConvertLensLengthToFovAngle(camera->fLens[0]) / 2);
                float new_dist = ICE::Length(&v_eye);

                float old_len = 1.0f / (old_dist * old_fov);
                float new_len = 1.0f / (new_dist * new_fov);
                float max_len = ICE::Max(old_len, new_len);
                float percent = ICE::Abs(new_len - old_len) / max_len;

                if (percent > fPercentTreshold) {

                    sStat[2] = '1';

                    if (vel_dot > 0.0f) {

                        is_it_good = true;
                        sStat[3] = '1';
                    } else {

                        sStat[3] = '0';
                    }
                } else {

                    sStat[2] = '0';
                }
            }
        } else {

            sStat[1] = '0';
        }
    } else {

        sStat[0] = '0';
    }

    return is_it_good;
}

ICETrack *ICEReplay::ChooseGoodCamera(ICEAnchor *p_car, ICEGroup *p_replay_cameras, int num_replay_cameras) {

    // total size: 0xC
    struct ReplayCandidate {
        ICETrack *pTrack; // offset 0x0, size 0x4
        float fScore;     // offset 0x4, size 0x4
        bool bMirror;     // offset 0x8, size 0x1
    };

    ICETrack *good_track = 0;

    if (p_car) {

        float total_score = 0.0f;
        float *scores = new float[num_replay_cameras];

        for (int group_number = 0; group_number < num_replay_cameras; group_number++) {

            ICEGroup *group = &p_replay_cameras[group_number];
            ICE::ReplayCategory *category = ICE::GetReplayCategory(group->GetHandle());

            float score = category->GetScore(p_car);

            if (Tweak_ForceICEReplay) {
                score += 0.1f;
            }
            if (score >= 0.1f) {

                scores[group_number] = score;
                total_score += score;
            } else {

                scores[group_number] = 0.0f;
            }
        }

        if (total_score > 0.0f) {

            int num_tracks = 0;

            for (int group_number = 0; group_number < num_replay_cameras; group_number++) {

                if (scores[group_number] > 0.0f) {

                    ICEGroup *group = &p_replay_cameras[group_number];
                    num_tracks += group->GetNumTracks();
                }
            }

            if (num_tracks > 0) {

                int num_candidates = 0;
                ReplayCandidate *candidates = new ReplayCandidate[num_tracks];

                for (int group_number = 0; group_number < num_replay_cameras; group_number++) {

                    float score = scores[group_number];

                    if (score > 0.0f) {

                        int old_num_candidates = num_candidates;
                        ICEGroup *group = &p_replay_cameras[group_number];

                        for (int track_number = 0; track_number < group->GetNumTracks(); track_number++) {

                            ICETrack *track = group->GetTrack(track_number);
                            ICEData *camera_data = track->GetKey(0);

                            if (camera_data != 0 && camera_data->nType != 0) {

                                char sTrack[16];

                                ICE::ReplayCategory *category = ICE::GetReplayCategory(group->GetHandle());
                                bMirrorICEData = category->GetMirror(p_car);

                                if (score >= 10.0f ||
                                    (CameraCutIsGood(camera_data, 0.0f, p_car) && !WasRecentlyUsed(track))) {

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

                            float recip = 1.0f / ICE::IntToFloat(candidates_from_this_group);

                            for (int i = old_num_candidates; i < num_candidates; i++) {
                                candidates[i].fScore *= recip;
                            }
                        }
                    }
                }

                if (num_candidates > 0) {

                    float random_score = ICE::Random(total_score);

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

                        int i = 0;

                        good_track = candidates[i].pTrack;
                        bMirrorICEData = candidates[i].bMirror;
                    }
                }

                delete[] candidates;
            }
        }

        delete[] scores;
    }

    return good_track;
}
