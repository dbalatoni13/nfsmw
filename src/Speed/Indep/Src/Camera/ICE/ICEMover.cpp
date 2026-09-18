#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif

static ICE::Vector3 vIceAccelLagMin(-1.0f, -1.0f, -0.2f);
static ICE::Vector3 vIceAccelLagMax(1.0f, 1.0f, 1.0f);
static ICE::Vector3 vIceAccelLagScale(0.05f, 0.05f, 0.02f);

bAngle ConvertLensLengthToFovAngle(float f_lens) {

    return ICE::ATan(f_lens) * 2;
}

float ConvertFovAngleToLensLength(bAngle a_fov) {

    float f_tan = bTan(a_fov / 2);

    if (f_tan <= 0.0f) {
        f_tan = 0.0001f;
    }

    return 15.960001f / f_tan;
}

float ConvertLensDeltaToFovDelta(float f_lens_mm, float f_lens_slope) {

    bAngle a = ICE::ATan(f_lens_mm) * 2;
    bAngle b = ICE::ATan(f_lens_mm + f_lens_slope * 0.01f) * 2;

    return (ICE::SignExtendAng(b) - ICE::SignExtendAng(a)) * 100.0f;
}

float ConvertApertureNumberToFStop(float f_aperture) {

    const float f_stops[37] = {
        1.0f,        1.122462f,  1.2599211f, 1.4142135f, 1.587401f,  1.7817974f, 2.0f,        2.244924f,  2.5198421f, 2.828427f,
        3.174802f, 3.5635948f, 4.0f,         4.489848f,  5.0396843f, 5.656854f,  6.349604f, 7.1271896f, 8.0f,         8.979696f,
        10.079369f, 11.313708f, 12.699208f, 14.254379f, 16.0f, 17.959393f, 20.158737f, 22.627417f, 25.398417f, 28.508759f,
        32.0f, 35.918785f, 40.317474f, 45.254833f, 50.796833f, 57.017517f, 64.0f,
    };

    int n = ICE::FloatToInt(f_aperture + 0.5f);

    return f_stops[bClamp(n, 0, 36)];
}

static void CreateLookAtMatrix(ICE::Matrix4 *mat, ICE::Vector3 &eye, ICE::Vector3 &center, unsigned short dutch) {
    ICE::Vector3 look;
    ICE::Sub(&look, &center, &eye);
    ICE::Normalize(&look);

    ICE::Vector3 world_up(0.0f, 0.0f, 1.0f);
    ICE::Vector3 up(-world_up.x, -world_up.y, -world_up.z);
    ICE::Vector3 side;
    ICE::Cross(&side, &up, &look);
    ICE::Cross(&up, &look, &side);
    ICE::Normalize(&side);
    ICE::Normalize(&up);

    bMatrix4 *m = reinterpret_cast<bMatrix4 *>(mat);
    m->v0.x = side.x; m->v0.y = up.x; m->v0.z = look.x; m->v0.w = 0.0f;
    m->v1.x = side.y; m->v1.y = up.y; m->v1.z = look.y; m->v1.w = 0.0f;
    m->v2.x = side.z; m->v2.y = up.z; m->v2.z = look.z; m->v2.w = 0.0f;
    m->v3.x = 0.0f; m->v3.y = 0.0f; m->v3.z = 0.0f; m->v3.w = 1.0f;

    eRotateZ(m, m, dutch);

    bMatrix4 tl;
    tl.v0.x = 1.0f;
    tl.v0.y = 0.0f;
    tl.v0.z = 0.0f;
    tl.v0.w = 0.0f;
    tl.v1.x = 0.0f;
    tl.v1.y = 1.0f;
    tl.v1.z = 0.0f;
    tl.v1.w = 0.0f;
    tl.v2.x = 0.0f;
    tl.v2.y = 0.0f;
    tl.v2.z = 1.0f;
    tl.v2.w = 0.0f;
    tl.v3.x = -eye.x;
    tl.v3.y = -eye.y;
    tl.v3.z = -eye.z;
    tl.v3.w = 1.0f;
    bMulMatrix(m, m, &tl);
}

ICEMover::ICEMover(int nView, ICEAnchor *pCar) : CameraMover(nView, CM_ICE), pCar(pCar) {

    pCamera->ClearVelocity();

    pEye = new ("ICEMover", 0) ICE::Cubic3D;
    pLook = new ("ICEMover", 0) ICE::Cubic3D;
    pDutch = new ("ICEMover", 0) ICE::Cubic1D;
    pFov = new ("ICEMover", 0) ICE::Cubic1D;
    pNearClip = new ("ICEMover", 0) ICE::Cubic1D;
    pNoiseAmplitude = new ("ICEMover", 0) ICE::Cubic1D;
    pNoiseFrequency = new ("ICEMover", 0) ICE::Cubic1D;
    pFocalDistance = new ("ICEMover", 0) ICE::Cubic1D;
    pAperture = new ("ICEMover", 0) ICE::Cubic1D;
    pLetterbox = new ("ICEMover", 0) ICE::Cubic1D;
    pSimSpeed = new ("ICEMover", 0) ICE::Cubic1D;

    pAccelOffset = new ("ICEMover", 0) ICE::Cubic3D(1);
    pICEData = 0;
    nSpaceEye = 0;
    nSpaceLook = 0;
    bViolatesTopology = false;

    fParameter0 = 0.0f;
    fParameter1 = 0.0f;

    ICE::HideOverlay();

    ICE::Identity(&mHybridToWorld);

    ICE::Copy(&vSmoothCarPos, pCar->GetGeometryPosition());
    ICE::Copy(&vSmoothCarFwd, pCar->GetForwardVector());

    SetDesired(true, true);
}

ICEMover::~ICEMover() {
    ICE::HideOverlay();

    delete pEye;
    delete pLook;
    delete pDutch;
    delete pFov;
    delete pNearClip;
    delete pNoiseAmplitude;
    delete pNoiseFrequency;
    delete pFocalDistance;
    delete pAperture;
    delete pLetterbox;
    delete pSimSpeed;

    delete pAccelOffset;

    pCamera->SetSimTimeMultiplier(1.0f);
    pCamera->SetLetterBox(0.0f);
    pCamera->SetDepthOfField(0.0f);
    pCamera->SetFocalDistance(0.0f);
    pCamera->ClearVelocity();
}

void ICEMover::EyeCubicInit(ICE::Cubic3D *pEye, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity) {
    ICE::Vector3 vEye;

    ICE::ScaleAdd(&vEye, reinterpret_cast<ICE::Vector3 *>(pCamera->GetPosition()),
                  reinterpret_cast<ICE::Vector3 *>(pCamera->GetVelocityPosition()), 0.033333335f);
    if (pMatrix) {
        ICE::MulMatrix(&vEye, pMatrix, &vEye);
    }
    pEye->SetVal(&vEye);

    ICE::Vector3 vEyeRel(*reinterpret_cast<ICE::Vector3 *>(pCamera->GetVelocityPosition()));

    if (pVelocity) {
        ICE::Sub(&vEyeRel, &vEyeRel, pVelocity);
    }

    ICE::Vector4 vEyeVel;

    ICE::Scale(reinterpret_cast<ICE::Vector3 *>(&vEyeVel), &vEyeRel, pEye->x.duration);
    vEyeVel.w = 0.0f;
    if (pMatrix) {
        ICE::MulMatrix(&vEyeVel, pMatrix, &vEyeVel);
    }
    pEye->SetdVal(reinterpret_cast<ICE::Vector3 *>(&vEyeVel));
}

void ICEMover::LookCubicInit(ICE::Cubic3D *pLook, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity) {
    ICE::Vector3 vLook;

    ICE::ScaleAdd(&vLook, reinterpret_cast<ICE::Vector3 *>(pCamera->GetTarget()),
                  reinterpret_cast<ICE::Vector3 *>(pCamera->GetVelocityTarget()), 0.033333335f);
    if (pMatrix) {
        ICE::MulMatrix(&vLook, pMatrix, &vLook);
    }
    pLook->SetVal(&vLook);

    ICE::Vector3 vLookRel(*reinterpret_cast<ICE::Vector3 *>(pCamera->GetVelocityTarget()));

    if (pVelocity) {
        ICE::Sub(&vLookRel, &vLookRel, pVelocity);
    }

    ICE::Vector4 vLookVel;

    ICE::Scale(reinterpret_cast<ICE::Vector3 *>(&vLookVel), &vLookRel, pLook->x.duration);
    vLookVel.w = 0.0f;
    if (pMatrix) {
        ICE::MulMatrix(&vLookVel, pMatrix, &vLookVel);
    }
    pLook->SetdVal(reinterpret_cast<ICE::Vector3 *>(&vLookVel));
}

void ICEMover::DutchCubicInit(ICE::Cubic1D *pDutch) {
    float fDutch = 0.0f;
    float fDutchVel = 0.0f;

    pDutch->SetVal(fDutch);
    pDutch->SetdVal(fDutchVel);
}

void ICEMover::FovCubicInit(ICE::Cubic1D *pFov) {
    float fFov = ICE::IntToFloat(pCamera->GetFieldOfView()) + ICE::IntToFloat(pCamera->GetVelocityFov()) * 0.033333335f;
    float fFovVel = ICE::IntToFloat(pCamera->GetVelocityFov()) * pFov->duration;

    pFov->SetVal(fFov);
    pFov->SetdVal(fFovVel);
}

void ICEMover::SetDesired(bool b_snap, bool b_refresh) {
    ICEData *pOldKey = pICEData;
    ICEData *pKey = 0;
    ICETrack *pTrack = 0;

    pKey = TheICEManager.GetCameraData(&pTrack, &fParameter0, &fParameter1);
    bool b_new_key = pICEData != pKey;

    if (!b_refresh && !b_new_key) {
        return;
    }
    bool b_hard_cut = b_new_key && pKey && pICEData && !ICE::KeysShared(pICEData, 1, pKey, 0);
    b_hard_cut |= pKey && !pICEData && !(pKey->bSmooth & 1);
    b_hard_cut |= pICEData && !pKey && !(pICEData->bSmooth & 1);

    pICEData = pKey;

    if (b_hard_cut) {

        FlushAccumulationBuffer();
        ICE::Copy(&vSmoothCarPos, pCar->GetGeometryPosition());
        ICE::Copy(&vSmoothCarFwd, pCar->GetForwardVector());
    }

    if (b_new_key) {

        int nIndex = TheICEManager.GetCameraIndex((fParameter0 + fParameter1) * 0.5f, pTrack);
        ICE::FireEventTag(nIndex);
    }

    if (pKey) {

        nSpaceEye = pKey->nSpaceEye;
        nSpaceLook = pKey->nSpaceLook;

        ICE::Vector3 vEye[2];
        ICE::Vector3 vLook[2];
        ICE::Vector3 vEyeSlope[2];
        ICE::Vector3 vLookSlope[2];
        float fDutchSlope[2];
        float fLensSlope[2];

        pKey->GetEye(0, &vEye[0]);
        pKey->GetEye(1, &vEye[1]);
        pKey->GetLook(0, &vLook[0]);
        pKey->GetLook(1, &vLook[1]);

        TheICEManager.GetSlope(&vEyeSlope[0], &vLookSlope[0], &fDutchSlope[0], &fLensSlope[0], pKey, 0, pTrack);
        TheICEManager.GetSlope(&vEyeSlope[1], &vLookSlope[1], &fDutchSlope[1], &fLensSlope[1], pKey, 1, pTrack);

        pEye->SetVal(&vEye[0]);
        pEye->SetdVal(&vEyeSlope[0]);
        pEye->SetValDesired(&vEye[1]);
        pEye->SetdValDesired(&vEyeSlope[1]);

        pLook->SetVal(&vLook[0]);
        pLook->SetdVal(&vLookSlope[0]);
        pLook->SetValDesired(&vLook[1]);
        pLook->SetdValDesired(&vLookSlope[1]);

        pDutch->SetVal(pKey->fDutch[0]);
        pDutch->SetdVal(fDutchSlope[0]);
        pDutch->SetValDesired(pKey->fDutch[1]);
        pDutch->SetdValDesired(fDutchSlope[1]);

        pFov->SetVal(ConvertLensLengthToFovAngle(pKey->fLens[0]));
        pFov->SetdVal(ConvertLensDeltaToFovDelta(pKey->fLens[0], fLensSlope[0]));
        pFov->SetValDesired(ConvertLensLengthToFovAngle(pKey->fLens[1]));
        pFov->SetdValDesired(ConvertLensDeltaToFovDelta(pKey->fLens[1], fLensSlope[1]));

        if (b_hard_cut) {
            ICE::Identity(&mHybridToWorld);
            ICE::Copy(&mHybridToWorld, pCar->GetGeometryOrientation(), pCar->GetGeometryPosition());
        }

        if (b_new_key) {

            TheICEManager.SetSmoothExit((pKey->bSmooth >> 1) & 1);

            if (pKey->bSmooth & 1) {

                ICE::Matrix4 mCarToWorld;
                ICE::Matrix4 mWorldToCar;

                ICE::Copy(&mCarToWorld, pCar->GetGeometryOrientation(), pCar->GetGeometryPosition());
                ICE::Invert(&mWorldToCar, &mCarToWorld);

                ICE::Matrix4 mWorldToHybrid;
                ICE::Invert(&mWorldToHybrid, &mHybridToWorld);

                ICE::Cubic3D cEye(1);
                ICE::Cubic3D cLook(1);
                ICE::Cubic1D cDutch(1);
                ICE::Cubic1D cFov(1);
                ICE::Matrix4 mWorldToAnim;
                ICE::Matrix4 *pAnimMatrix = 0;

                if (nSpaceEye == 3 || nSpaceLook == 3) {

                    ICEScene *pScene = ICE::FindAnimScene();
                    if (pScene) {

                        pAnimMatrix = &mWorldToAnim;
                        ICE::Invert(pAnimMatrix, reinterpret_cast<const ICE::Matrix4 *>(&pScene->GetSceneTransformMatrix()));
                    }
                }
                ICE::Matrix4 *pMatrixEye;

                switch (nSpaceEye) {
                case 0:
                    pMatrixEye = &mWorldToCar; break;
                case 3: pMatrixEye = pAnimMatrix; break;
                case 2: pMatrixEye = &mWorldToHybrid; break;
                default: pMatrixEye = 0; break;
                }
                ICE::Matrix4 *pMatrixLook;

                switch (nSpaceLook) {
                case 0:
                    pMatrixLook = &mWorldToCar; break;
                case 3: pMatrixLook = pAnimMatrix; break;
                case 2: pMatrixLook = &mWorldToHybrid; break;
                default: pMatrixLook = 0; break;
                }

                EyeCubicInit(&cEye, pMatrixEye, nSpaceEye == 0 ? pCar->GetVelocity() : 0);
                LookCubicInit(&cLook, pMatrixLook, nSpaceLook == 0 ? pCar->GetVelocity() : 0);
                DutchCubicInit(&cDutch);
                FovCubicInit(&cFov);

                float f_param = TheICEManager.GetParameter();
                float d0 = ICE::Abs(f_param - fParameter0);
                float d1 = ICE::Abs(f_param - fParameter1);

                if (d0 < d1) {

                    ICE::Vector3 vVal;
                    ICE::Vector3 vdVal;

                    cEye.GetVal(&vVal);
                    cEye.GetdVal(&vdVal);
                    pEye->SetVal(&vVal);
                    pEye->SetdVal(&vdVal);

                    ICE::Vector3 vValLook;
                    ICE::Vector3 vdValLook;

                    cLook.GetVal(&vValLook);
                    cLook.GetdVal(&vdValLook);
                    pLook->SetVal(&vValLook);
                    pLook->SetdVal(&vdValLook);

                    pDutch->SetVal(cDutch.GetVal());
                    pDutch->SetdVal(cDutch.GetdVal());
                    pFov->SetVal(cFov.GetVal());
                    pFov->SetdVal(cFov.GetdVal());
                } else {

                    ICE::Vector3 vVal;
                    ICE::Vector3 vdVal;

                    cEye.GetVal(&vVal);
                    cEye.GetdVal(&vdVal);
                    pEye->SetValDesired(&vVal);
                    pEye->SetdValDesired(&vdVal);

                    ICE::Vector3 vValLook;
                    ICE::Vector3 vdValLook;

                    cLook.GetVal(&vValLook);
                    cLook.GetdVal(&vdValLook);
                    pLook->SetValDesired(&vValLook);
                    pLook->SetdValDesired(&vdValLook);

                    pDutch->SetValDesired(cDutch.GetVal());
                    pDutch->SetdValDesired(cDutch.GetdVal());
                    pFov->SetValDesired(cFov.GetVal());
                    pFov->SetdValDesired(cFov.GetdVal());
                }
            }
        }

        pEye->MakeCoeffs();
        pLook->MakeCoeffs();
        pDutch->MakeCoeffs();
        pFov->MakeCoeffs();

        pNearClip->SetVal(pKey->fNearClip[0]);
        pNearClip->SetValDesired(pKey->fNearClip[1]);
        pNearClip->MakeCoeffs();

        pNoiseAmplitude->SetVal(pKey->fNoiseAmplitude[0]);
        pNoiseAmplitude->SetValDesired(pKey->fNoiseAmplitude[1]);
        pNoiseAmplitude->MakeCoeffs();

        pNoiseFrequency->SetVal(pKey->fNoiseFrequency[0]);
        pNoiseFrequency->SetValDesired(pKey->fNoiseFrequency[1]);
        pNoiseFrequency->MakeCoeffs();

        pFocalDistance->SetVal(pKey->fFocalDistance[0]);
        pFocalDistance->SetValDesired(pKey->fFocalDistance[1]);
        pFocalDistance->MakeCoeffs();

        pAperture->SetVal(ICE::IntToFloat(pKey->fAperture[0]));
        pAperture->SetValDesired(ICE::IntToFloat(pKey->fAperture[1]));
        pAperture->MakeCoeffs();

        pLetterbox->SetVal(ICE::IntToFloat(pKey->fLetterbox[0]));
        pLetterbox->SetValDesired(ICE::IntToFloat(pKey->fLetterbox[1]));
        pLetterbox->MakeCoeffs();

        pSimSpeed->SetVal(ICE::IntToFloat(pKey->fSimSpeed[0]));
        pSimSpeed->SetValDesired(ICE::IntToFloat(pKey->fSimSpeed[1]));
        pSimSpeed->MakeCoeffs();

        int nState = TheICEManager.GetState();

        if (nState == 0 || nState == 5 || nState == 7) {

            if (!pOldKey || pOldKey->nOverlay != pKey->nOverlay) {

                ICE::HideOverlay();
                ICE::ShowOverlay(pKey->nOverlay);
            }
        }
    }
}

void ICEMover::GetEye(ICE::Vector3 *vEye, float f_param) {
    if (pICEData && pICEData->bCubicEye) {

        pEye->GetVal(vEye, f_param);
    } else {
        ICE::Vector3 v0;
        ICE::Vector3 v1;

        pEye->GetVal(&v0);
        pEye->GetValDesired(&v1);
        ICE::Scale(vEye, &v0, 1.0f - f_param);
        ICE::ScaleAdd(vEye, vEye, &v1, f_param);
    }
}

void ICEMover::GetLook(ICE::Vector3 *vLook, float f_param) {
    if (pICEData && pICEData->bCubicLook) {

        pLook->GetVal(vLook, f_param);
    } else {
        ICE::Vector3 v0;
        ICE::Vector3 v1;

        pLook->GetVal(&v0);
        pLook->GetValDesired(&v1);
        ICE::Scale(vLook, &v0, 1.0f - f_param);
        ICE::ScaleAdd(vLook, vLook, &v1, f_param);
    }
}

float ICEMover::GetDutch(float f_param) {
    float dutch;

    if (pICEData && pICEData->bCubicEye) {

        dutch = pDutch->GetVal(f_param);
    } else {
        float v0 = pDutch->GetVal();
        float v1 = pDutch->GetValDesired();

        dutch = v0 * (1.0f - f_param) + v1 * f_param;
    }

    return dutch;
}

unsigned short ICEMover::GetFOV(float f_param) {
    float fov;

    if (pICEData && pICEData->bCubicEye) {

        fov = pFov->GetVal(f_param);
    } else {
        float v0 = pFov->GetVal();
        float v1 = pFov->GetValDesired();

        fov = v0 * (1.0f - f_param) + v1 * f_param;
    }

    return ICE::FloatToInt(fov);
}

void ICEMover::Update(float dT) {
    ICETrack *pTrack = TheICEManager.GetPlaybackTrack();

    if (static_cast<float>(TheICEManager.GetUseRealTimeRaw()) == 0.0f) {
        if (IsGameFlowPaused()) {
            return;
        }
    }

    int bGeneric = 0;
    if (pTrack) {
        bGeneric = pTrack->GetContext() == 3;
        if (pTrack->GetContext() != 2) {
            bMirrorICEData = 0;
        }
    }

    *reinterpret_cast<int *>(&bViolatesTopology) = 0;

    SetDesired(false, TheICEManager.RefreshCameraSplines());

    ICE::Matrix4 mAnimToWorld;
    ICE::Identity(&mAnimToWorld);

    if (nSpaceEye == 3 || nSpaceLook == 3) {
        ICEScene *scene = ICE::FindAnimScene();
        if (!scene) {
            return;
        }
        ICE::Copy(&mAnimToWorld, reinterpret_cast<const ICE::Matrix4 *>(&scene->GetSceneTransformMatrix()));
    }

    float fParam = pTrack ? pTrack->GetParameter() : TheICEManager.GetParameter();
    float fT = 0.0f;
    if (ICE::Abs(fParameter1 - fParameter0) > 0.0001f) {
        fT = (fParam - fParameter0) / (fParameter1 - fParameter0);
    }

    if (fParam >= 1.0f) {
        TheICEManager.SetGenericCameraToPlay("", "");
    }

    float fSim = pSimSpeed->GetVal(fT);
    if (fSim >= 0.0f) {
        if (fSim >= 100.0f) {
            fSim = 100.0f;
        }
        fSim *= 0.01f;
        pCamera->SetSimTimeMultiplier(fSim);
    } else {
        fSim = 0.0f;
    }
    if (fSim < 0.01f) {
        fSim = 0.01f;
    }

    float fNear = pNearClip->GetVal(fT);
    if (fNear >= 0.0f) {
        pCamera->SetNearZ(fNear);
    }

    unsigned short fov = GetFOV(fT);
    if (fov) {
        pCamera->SetFieldOfView(fov);
    }

    ICE::Vector3 vEye;
    GetEye(&vEye, fT);

    ICE::Vector3 vLook;
    GetLook(&vLook, fT);

    ICE::Matrix4 mSpace;
    if (pICEData && pICEData->bIgnoreOrientation) {
        ICE::Copy(&mSpace, &mHybridToWorld, pCar->GetGeometryPosition());
    } else if (pICEData && pICEData->bCarSpaceLag && bGeneric) {
        float k = fParam * 0.9f + 0.1f;
        ICE::Lerp(&vSmoothCarPos, pCar->GetGeometryPosition(), &vSmoothCarPos, k);
        ICE::Lerp(&vSmoothCarFwd, pCar->GetForwardVector(), &vSmoothCarFwd, k);
        /* MEDIDO: el DWARF del original usa aqui `Normalize(dst, src)` de dos
         * argumentos y DOS `Copy(Vector4*, const Vector3*, float)` --cada una con
         * su bCopy dentro-- donde nosotros escribimos las cuatro asignaciones de
         * campo de v0 y v3. Parece codigo que falta y NO LO ES: aplicar las dos
         * Copy y el Normalize de dos argumentos da `faltan 0, sobran 0` y el mismo
         * 99,9069% exacto, y ademas las Copy NI SIQUIERA APARECEN en nuestro
         * DWARF, asi que la forma tampoco reproduce la del original.
         * Los 3.868 B de esta funcion son 15 diferencias de REGISTRO y nada mas. */
        ICE::Identity(&mSpace);
        ICE::Normalize(&vSmoothCarFwd);
        mSpace.v0.x = vSmoothCarFwd.x;
        mSpace.v0.y = vSmoothCarFwd.y;
        mSpace.v0.z = vSmoothCarFwd.z;
        mSpace.v0.w = 0.0f;
        ICE::Cross(reinterpret_cast<ICE::Vector3 *>(&mSpace.v1), reinterpret_cast<const ICE::Vector3 *>(&mSpace.v2),
                   reinterpret_cast<const ICE::Vector3 *>(&mSpace.v0));
        ICE::Cross(reinterpret_cast<ICE::Vector3 *>(&mSpace.v2), reinterpret_cast<const ICE::Vector3 *>(&mSpace.v0),
                   reinterpret_cast<const ICE::Vector3 *>(&mSpace.v1));
        mSpace.v3.x = vSmoothCarPos.x;
        mSpace.v3.y = vSmoothCarPos.y;
        mSpace.v3.z = vSmoothCarPos.z;
        mSpace.v3.w = 1.0f;
    } else {
        ICE::Copy(&mSpace, pCar->GetGeometryOrientation(), pCar->GetGeometryPosition());
    }

    switch (nSpaceEye) {
    case 0:
        ICE::MulMatrix(&vEye, &mSpace, &vEye);
        break;
    case 3:
        ICE::MulMatrix(&vEye, &mAnimToWorld, &vEye);
        break;
    case 2:
        ICE::Add(&vEye, &vEye, pCar->GetGeometryPosition());
        break;
    default:
        break;
    }

    switch (nSpaceLook) {
    case 0:
        ICE::MulMatrix(&vLook, &mSpace, &vLook);
        break;
    case 3:
        ICE::MulMatrix(&vLook, &mAnimToWorld, &vLook);
        break;
    case 2:
        ICE::Add(&vLook, &vLook, pCar->GetGeometryPosition());
        break;
    default:
        break;
    }

    if (pICEData && pICEData->bCarSpaceLag) {
        ICE::Vector3 vOff;
        pAccelOffset->SetValDesired(pCar->GetAcceleration());
        pAccelOffset->Update(dT * fSim, 0.0f, 0.0f);
        pAccelOffset->GetVal(&vOff);
        ICE::Scale(&vOff, &vOff, &vIceAccelLagScale);
        ICE::Clamp(&vOff, &vIceAccelLagMin, &vIceAccelLagMax);
        ICE::Sub(&vEye, &vEye, &vOff);
        ICE::Sub(&vLook, &vLook, &vOff);
    }

    int nState = TheICEManager.GetState();
    unsigned short dutch = static_cast<unsigned short>(ICE::FloatToInt(GetDutch(fT) * 65536.0f));
    ICE::Matrix4 mCam;
    CreateLookAtMatrix(&mCam, vEye, vLook, dutch);

    float fAmp = pNoiseAmplitude->GetVal(fT);
    float fFreq = pNoiseFrequency->GetVal(fT);
    if (fAmp < 0.0f) {
        fAmp = 0.0f;
    }
    if (fFreq < 0.0f) {
        fFreq = 0.0f;
    }

    if (pICEData && pICEData->nShakeType) {
        ICEShakeTrack *pShake = TheICEManager.GetShakeTrack(pICEData->nShakeType);
        if (pShake) {
            float length = pTrack ? pTrack->GetLength() : TheICEManager.GetParameterLength();
            float current_sec = fFreq * 30.0f;
            // split medido: como argumento anidado el binding copia el resultado
            // de FloatToInt (un mr de mas); con la variable intermedia el lwz
            // cae en el pseudo de frame y SignedMod::a se liga sin copia.
            int frame = ICE::FloatToInt(fParam * length * current_sec);
            frame = ICE::SignedMod(frame, pShake->GetNumKeys());
            ICEShakeData *pKey = pShake->GetKey(frame);
            ICE::Vector3 r(pKey->q[0], pKey->q[1], pKey->q[2]);
            ICE::Scale(&r, &r, fAmp);
            float w = ICE::Sqrt(1.0f - ICE::Length(&r));
            bQuaternion q(r.x, r.y, r.z, w);
            ICE::Matrix4 shake_matrix;
            ICE::QuatToMatrix(&shake_matrix, &q);
            ICE::Vector3 t(pKey->p[0], pKey->p[1], pKey->p[2]);
            ICE::Scale(&t, &t, fAmp);
            ICE::Copy(&shake_matrix, &shake_matrix, &t);
            ICE::MulMatrix(&mCam, &shake_matrix, &mCam);
        }
    } else {
        pCamera->SetNoiseAmplitude1(0.0f, 0.0f, fAmp, fAmp);
        pCamera->SetNoiseFrequency1(0.0f, 0.0f, fFreq * 1.618f, fFreq * 1.382f);
        pCamera->SetNoiseAmplitude2(0.0f, 0.0f, 0.0f, 0.0f);
        pCamera->SetNoiseFrequency2(0.0f, 0.0f, 0.0f, 0.0f);

        float length = pTrack ? pTrack->GetLength() : TheICEManager.GetParameterLength();
        float current_sec = fFreq * 30.0f;
        pCamera->ApplyNoise(reinterpret_cast<bMatrix4 *>(&mCam), fParam * length, 1.0f);
    }

    if (pTrack && pTrack->GetContext() == 2) {
        bViolatesTopology = MinGapTopology(reinterpret_cast<bMatrix4 *>(&mCam),
                                           reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()));
        bViolatesTopology = bViolatesTopology | MinGapCars(reinterpret_cast<bMatrix4 *>(&mCam),
                                                          reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()),
                                                          reinterpret_cast<bVector3 *>(pCar->GetVelocity()));
    } else {
        bool bWorld = pICEData && pICEData->bConstrainToWorld && nState <= 8;
        if (bWorld) {
            MinGapTopology(reinterpret_cast<bMatrix4 *>(&mCam), reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()));
        }
        bool bCars = pICEData && pICEData->bConstrainToCars && nState <= 8;
        if (bCars) {
            MinGapCars(reinterpret_cast<bMatrix4 *>(&mCam), reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()),
                       reinterpret_cast<bVector3 *>(pCar->GetVelocity()));
        }
    }

    float fLetter = pLetterbox->GetVal(fT);
    if (fLetter >= 0.0f) {
        pCamera->SetLetterBox(fLetter * 0.00125f);
    }

    float fAperture = pAperture->GetVal(fT);
    if (fAperture >= 0.0f && fAperture < ICE::IntToFloat(37)) {
        const float CoC = 0.03f;
        float focal = pFocalDistance->GetVal(fT);
        if (focal < 0.01f) {
            focal = 0.01f;
        }
        focal *= 1000.0f;
        float lens = ConvertFovAngleToLensLength(GetFOV(fT));
        float fstop = ConvertApertureNumberToFStop(fAperture);
        float hyperFocal = (lens * lens) / (fstop * CoC) + lens;
        float dofNear = ((hyperFocal - lens) * focal) / (hyperFocal + focal - (lens + lens)) * 0.001f;
        float dofFar = 100000.0f;
        if (hyperFocal - focal > 0.00001f) {
            dofFar = ((hyperFocal - lens) * focal / (hyperFocal - focal)) * 0.001f;
        }
        if (dofFar < dofNear) {
            dofFar = dofNear + 0.01f;
        }
        pCamera->SetFocalDistance((dofFar + dofNear) * 0.5f);
        pCamera->SetDepthOfField(dofFar - dofNear);
    } else {
        pCamera->SetFocalDistance(0.0f);
        pCamera->SetDepthOfField(0.0f);
    }

    pCamera->SetTargetDistance(ICE::DistBetween(&vEye, &vLook));
    pCamera->SetCameraMatrix(*reinterpret_cast<bMatrix4 *>(&mCam), dT * fSim);
}
