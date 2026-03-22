#include "Sun.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern short SpecularOffset;
extern eModel SkySpecularModel;
extern bVector3 SunPos;
extern float SunPosY;
extern float lbl_8040B2A0;
extern float lbl_8040B2A4;
extern float lbl_8040B2A8;
extern float lbl_8040B2AC;
extern float lbl_8040B2B0;
extern float lbl_8040B2B4;

void GetSunPos(eView *view, float *x, float *y, float *z);

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag) asm("Render__18eViewPlatInterfaceP6eModelP8bMatrix4P13eLightContextUiT2");

} // namespace

void StuffSpecular(eView *view) {
    bMatrix4 *cameraMatrix = view->GetCamera()->GetCameraMatrix();
    bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
    bMatrix4 rotation;
    float x = cameraMatrix->v3.x;
    float y = cameraMatrix->v3.y;
    float z = cameraMatrix->v3.z;
    bVector3 toSun;
    short angle;

    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bMatrix4);
        matrix = 0;
    } else {
        CurrentBufferPos += sizeof(bMatrix4);
    }

    if (matrix != 0) {
        PSMTX44Identity(*reinterpret_cast<Mtx44 *>(matrix));
        matrix->v3.x = x;
        matrix->v3.y = y;
        matrix->v3.z = z;

        if (view->GetID() - 6U < 2) {
            GetSunPos(view, &SunPos.x, &SunPos.y, &SunPos.z);
            SunPosY = lbl_8040B2A0;
            matrix->v0.x *= lbl_8040B2A8;
            matrix->v1.y *= lbl_8040B2A8;
            matrix->v2.z = lbl_8040B2A4 * lbl_8040B2A8;
            matrix->v3.z += lbl_8040B2AC;

            toSun.x = SunPos.x - x;
            toSun.y = SunPos.y - y;
            toSun.z = lbl_8040B2A0;
            bNormalize(&toSun, &toSun);

            toSun.x = lbl_8040B2A0;
            toSun.y = lbl_8040B2B0;
            toSun.z = lbl_8040B2A0;
            angle = 0x4000 - bASin(toSun.x);
            if (toSun.x < lbl_8040B2A0) {
                angle = -angle;
            }

            eCreateRotationZ(&rotation, angle + SpecularOffset);
            eMulMatrix(matrix, &rotation, matrix);
            Render(view, &SkySpecularModel, matrix, 0, 0, 0);

            matrix = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
            if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += sizeof(bMatrix4);
                matrix = 0;
            } else {
                CurrentBufferPos += sizeof(bMatrix4);
            }

            if (matrix != 0) {
                PSMTX44Identity(*reinterpret_cast<Mtx44 *>(matrix));
                matrix->v3.x = x;
                matrix->v3.y = y;
                matrix->v0.x *= lbl_8040B2A8;
                matrix->v1.y *= lbl_8040B2A8;
                matrix->v2.z *= lbl_8040B2A8;
                matrix->v3.z = z + lbl_8040B2B4;
                eMulMatrix(matrix, &rotation, matrix);
                Render(view, &SkySpecularModel, matrix, 0, 0, 0);
            }
        }
    }
}
