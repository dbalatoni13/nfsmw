#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

extern float CAMERA_WOOSH_OFFSET;
extern int IsSoundEnabled;

void TestSmackableForWoosh(IModel *model, int carid) {
    UMath::Matrix4 transform;
    eDRIVE_BY_TYPE bytype;
    int numcars;
    int foundcarid;

    if (!IsSoundEnabled) {
        return;
    }

    if (model->IsHidden()) {
        return;
    }

    if (model->GetAttributes().GetClass() != Attrib::Gen::smackable::ClassKey()) {
        return;
    }

    Attrib::Gen::smackable smackattribs(model->GetAttributes());

    if (smackattribs.IsWooshable()) {
        model->GetTransform(transform);

        {
            float maxradius = 12.0f;
            UMath::Vector4 cam_pos;
            UMath::Vector4 objpos;
            bVector3 WorldCarPos;
            float dissquared;

            if (transform[1].y > 0.8f) {
                if (bLength(*SndCamera::GetV3WorldCarVel(carid)) >= 3.0f) {
                    WorldCarPos = bVector3(SndCamera::GetWorldCarPos(carid)->x, SndCamera::GetWorldCarPos(carid)->y, 0.0f);
                    cam_pos.x = -WorldCarPos.y;
                    cam_pos.y = 0.0f;
                    cam_pos.z = WorldCarPos.x;
                    objpos = transform.v3;
                    objpos.y = 0.0f;
                    dissquared = VU0_v4distancesquarexyz(objpos, cam_pos);
                    if (dissquared <= maxradius * maxradius) {
                        bytype = smackattribs.WooshType();
                        goto CreateWoosh;
                    }
                }
            } else {
                bVector3 bcam_pos;

                maxradius = 4.0f;
                objpos = transform.v3;
                bScaleAdd(&bcam_pos, SndCamera::GetCamPos(carid), SndCamera::GetCamDir(carid), CAMERA_WOOSH_OFFSET);
                cam_pos.x = -bcam_pos.y;
                cam_pos.y = bcam_pos.z;
                cam_pos.z = bcam_pos.x;
                dissquared = VU0_v4distancesquarexyz(objpos, cam_pos);
                if (dissquared <= maxradius * maxradius) {
                    bytype = DRIVE_BY_CAMERA_BY;
                    goto CreateWoosh;
                }
            }
        }
    }

    return;

CreateWoosh:
    stDriveByInfo tmpdrivebypackage;
    CSTATE_Base *ReturnedObj;

    tmpdrivebypackage.eDriveByType = bytype;
    tmpdrivebypackage.pEAXCar = SndCamera::GetPlayerCar(carid);
    tmpdrivebypackage.ClosingVelocity = tmpdrivebypackage.pEAXCar->GetPhysCar()->GetVelocityMagnitude();
    tmpdrivebypackage.vLocation = bVector3(0.0f, 0.0f, 0.0f);
    tmpdrivebypackage.UniqueID = reinterpret_cast<unsigned int>(model->GetInstanceHandle());
    ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
    if (ReturnedObj) {
        ReturnedObj->Attach(&tmpdrivebypackage);
    }
}
