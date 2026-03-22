#include "Sun.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern short SpecularOffset;
extern unsigned short matAng_33578;
extern int DrawSky;
extern int DrawSkyEnvMap;
extern int deblayer[5];
extern eModel SkySpecularModel;
extern eModel SkydomeModel;
extern bVector3 SunPos;
extern float SunPosY;
extern float WorldTimeElapsed;
extern float MainSkyScale;
extern float lbl_8040B278;
extern float lbl_8040B27C;
extern float lbl_8040B280;
extern float lbl_8040B284;
extern float lbl_8040B288;
extern float lbl_8040B28C;
extern float lbl_8040B290;
extern float lbl_8040B294;
extern float lbl_8040B298;
extern float lbl_8040B29C;
extern float lbl_8040B2A0;
extern float lbl_8040B2A4;
extern float lbl_8040B2A8;
extern float lbl_8040B2AC;
extern float lbl_8040B2B0;
extern float lbl_8040B2B4;

void GetSunPos(eView *view, float *x, float *y, float *z);

enum SKY_LAYER {
    SKY_LAYER_BLUE = 0,
    SKY_LAYER_CLOUDS = 1,
    SKY_LAYER_OVERCAST = 2,
    SKY_LAYER_LOWREZ = 3,
    SKY_LAYER_REFLECTION = 4,
    SKY_NUM_LAYERS = 5,
};

void ReplaceSkyTextures(SKY_LAYER layer);

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

void StuffSkyLayer(eView *view, SKY_LAYER layer) {
    if (deblayer[layer] != 0) {
        bMatrix4 *cameraMatrix = view->GetCamera()->GetCameraMatrix();
        bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
        float scaleZ = lbl_8040B27C;
        float scale = lbl_8040B280;
        bool rotate = false;
        float z = cameraMatrix->v3.z;
        float x = cameraMatrix->v3.x;
        float y = cameraMatrix->v3.y;
        int view_id = view->GetID();

        if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sizeof(bMatrix4);
            matrix = 0;
        } else {
            CurrentBufferPos += sizeof(bMatrix4);
        }

        if (matrix != 0) {
            PSMTX44Identity(*reinterpret_cast<Mtx44 *>(matrix));
            ReplaceSkyTextures(layer);
            if (layer != SKY_LAYER_BLUE && layer != SKY_LAYER_CLOUDS) {
                if (layer == SKY_LAYER_OVERCAST) {
                    rotate = true;
                    scale = lbl_8040B284;
                } else if (layer == SKY_LAYER_REFLECTION) {
                    rotate = true;
                    matrix->v2.z = lbl_8040B28C;
                    scale = lbl_8040B288;
                    scaleZ = lbl_8040B290;
                }
            } else {
                scale = lbl_8040B278;
            }

            if (view_id - 0x10U < 6) {
                scale = lbl_8040B294;
                if (DrawSkyEnvMap == 0) {
                    return;
                }
            } else if (view_id == 3) {
                scale = lbl_8040B298;
            }

            matrix->v3.x = x;
            matrix->v3.y = y;
            matrix->v3.z = z;

            if (view_id - 1U < 3) {
                if (DrawSky != 0) {
                    scale *= MainSkyScale;
                    matrix->v3.z = z + scaleZ;
                    matrix->v0.x *= scale;
                    matrix->v2.z *= scale;
                    matrix->v1.y *= scale;
                    if (rotate) {
                        bMatrix4 rotation;
                        unsigned int angle = static_cast<unsigned int>(matAng_33578) + static_cast<unsigned int>(WorldTimeElapsed * lbl_8040B29C);

                        matAng_33578 = static_cast<unsigned short>(angle);
                        eCreateRotationZ(&rotation, static_cast<unsigned short>(angle));
                        eMulMatrix(matrix, &rotation, matrix);
                    }

                    Render(view, &SkydomeModel, matrix, 0, 0x20000, 0);
                }
            } else {
                matrix->v0.x *= scale;
                matrix->v1.y *= scale;
                matrix->v2.z *= scale;
                matrix->v3.z = z + scaleZ;
                Render(view, &SkydomeModel, matrix, 0, 0x20000, 0);
            }
        }
    }
}
