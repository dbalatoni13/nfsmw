#include "SimpleModelAnim.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

enum SimpleModelAnimType {
    SIMPLEMODELANIM_ROTATEX = 1,
    SIMPLEMODELANIM_ROTATEY = 2,
    SIMPLEMODELANIM_ROTATEZ = 4
};

struct SimpleModelAnimInfo {
    unsigned int mHash;
    SimpleModelAnimType mRotationType;
    float mRotationSpeed;
    float mRotationAngle;
    float mLastAnimTime;
};

SimpleModelAnimInfo gSimpleSolidHashList[2] = {
    {0, SIMPLEMODELANIM_ROTATEY, 50.0f, 0.0f, 0.0f},
    {0, SIMPLEMODELANIM_ROTATEZ, 50.0f, 0.0f, 0.0f},
};

namespace SimpleModelAnim {

void Init() {
    gSimpleSolidHashList[0].mHash = bStringHash("XO_WINDMILL_BLADE_1B_JG_00");
    gSimpleSolidHashList[1].mHash = bStringHash("XB_DONUTSIGNB_1B_BK_00");
}

void Reset() {
    for (unsigned int ix = 0; ix < 2; ix++) {
        SimpleModelAnimInfo &modelAnim = gSimpleSolidHashList[ix];
        modelAnim.mLastAnimTime = WorldTimer.GetSeconds();
    }
}

void Update() {
    for (unsigned int ix = 0; ix < 2; ix++) {
        SimpleModelAnimInfo &modelAnim = gSimpleSolidHashList[ix];
        float elapsed = WorldTimer.GetSeconds() - modelAnim.mLastAnimTime;
        modelAnim.mLastAnimTime = WorldTimer.GetSeconds();
        modelAnim.mRotationAngle = elapsed * modelAnim.mRotationSpeed + modelAnim.mRotationAngle;
        if (modelAnim.mRotationAngle >= 360.0f) {
            modelAnim.mRotationAngle -= 360.0f;
        }
    }
}

void Animate(eModel *model, eSolid *solid, bMatrix4 *local_world) {
    for (unsigned int ix = 0; ix < 2; ix++) {
        SimpleModelAnimInfo &modelAnim = gSimpleSolidHashList[ix];
        if (solid->NameHash == modelAnim.mHash) {
            bMatrix4 *modelMatrix = model->GetPivotMatrix();
            bMatrix4 localInverse(*local_world);

            bInvertMatrix(&localInverse, &localInverse);
            eMulMatrix(modelMatrix, local_world, &localInverse);

            if (modelAnim.mRotationType & SIMPLEMODELANIM_ROTATEX) {
                eRotateX(modelMatrix, modelMatrix, bDegToAng(modelAnim.mRotationAngle));
            }

            if (modelAnim.mRotationType & SIMPLEMODELANIM_ROTATEY) {
                eRotateY(modelMatrix, modelMatrix, bDegToAng(modelAnim.mRotationAngle));
            }

            if (modelAnim.mRotationType & SIMPLEMODELANIM_ROTATEZ) {
                eRotateZ(modelMatrix, modelMatrix, bDegToAng(modelAnim.mRotationAngle));
            }

            eMulMatrix(local_world, modelMatrix, local_world);
        }
    }
}

} // namespace SimpleModelAnim
