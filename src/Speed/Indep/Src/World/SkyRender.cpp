#include "Sun.hpp"
#include "Scenery.hpp"
#include "TimeOfDay.hpp"
#include "World.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern short SpecularOffset;
extern unsigned short matAng_33578;
int DrawSky = 1;
int DrawSkyEnvMap = 1;
int deblayer[5] = {1, 1, 1, 1, 1};
float skylayer[5][8] = {
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
};
eModel SkySpecularModel;
eModel SkydomeModel;
extern bVector3 SunPos;
extern float SunPosY;
extern float WorldTimeElapsed;
float MainSkyScale = 1.0f;
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
extern float lbl_8040B274;
extern float lbl_8040B0FC;
extern float lbl_8040B108;
extern float lbl_8040B10C;
extern float lbl_8040B2A0;
extern float lbl_8040B2A4;
extern float lbl_8040B2A8;
extern float lbl_8040B2AC;
extern float lbl_8040B2B0;
extern float lbl_8040B2B4;
extern float lbl_8040B2B8;
extern float lbl_8040B2BC;
extern char lbl_8040B110[];
extern char lbl_8040B128[];
extern char lbl_8040B13C[];
extern char lbl_8040B154[];
extern char lbl_8040B170[];
extern char lbl_8040B198[];

float MinSkySpecular = 0.0f;
float MaxSkySpecular = 1.0f;
float SkyRenderForceOvercast = 0.0f;
unsigned int BaseSkyHash[2];
unsigned int SkyHash[10];
TextureInfo *CurrentSkyTextures[10];
void (*UserSkyLoadCallback)(int) = 0;
int UserSkyLoadCallbackParam;
int bSkyTexturesLoaded = 0;
eReplacementTextureTable SKYtextable[2];

static bMatrix4 MakeIdentityMatrix() {
    bMatrix4 matrix;
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&matrix));
    return matrix;
}

static bMatrix4 MakeReflectedIdentityMatrix() {
    bMatrix4 matrix = MakeIdentityMatrix();
    matrix.v2.z = -1.0f;
    return matrix;
}

bMatrix4 SkydomeLocalWorld = MakeIdentityMatrix();
bMatrix4 SkydomeLocalReflectedWorld = MakeReflectedIdentityMatrix();
bMatrix4 SkySpecularLocalWorld = MakeReflectedIdentityMatrix();

void GetSunPos(eView *view, float *x, float *y, float *z);
eSolid *eFindSolid(unsigned int name_hash);
SceneryInstance *FindSceneryInstance(unsigned int scenery_name_hash);
void *FindSceneryInfo(unsigned int scenery_name_hash);
void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(unsigned int), unsigned int param0, int memory_pool_num);
void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes);

enum SKY_LAYER {
    SKY_LAYER_BLUE = 0,
    SKY_LAYER_CLOUDS = 1,
    SKY_LAYER_OVERCAST = 2,
    SKY_LAYER_LOWREZ = 3,
    SKY_LAYER_REFLECTION = 4,
    SKY_NUM_LAYERS = 5,
};

void SkyLoadCallback(unsigned int param);
void ReplaceSkyTextures(SKY_LAYER layer);

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag) asm("Render__18eViewPlatInterfaceP6eModelP8bMatrix4P13eLightContextUiT2");

} // namespace

int SkyInitModel(eModel *model, bMatrix4 *local_world, unsigned int scenery_name_hash) {
    if (model->GetNameHash() != 0) {
        return 1;
    }

    if (eFindSolid(scenery_name_hash) == 0) {
        return 0;
    }

    model->Init(scenery_name_hash);
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(local_world));

    SceneryInstance *scenery_instance = FindSceneryInstance(scenery_name_hash);
    if (scenery_instance != 0) {
        short rotation0 = scenery_instance->Rotation[0];
        short rotation1 = scenery_instance->Rotation[1];
        short rotation2 = scenery_instance->Rotation[2];
        float rotation_scale = lbl_8040B0FC;
        float row_w = lbl_8040B108;

        local_world->v0.x = static_cast<float>(rotation0) * rotation_scale;
        local_world->v0.y = static_cast<float>(rotation1) * rotation_scale;
        local_world->v0.z = static_cast<float>(rotation2) * rotation_scale;
        local_world->v0.w = row_w;

        rotation0 = scenery_instance->Rotation[3];
        rotation1 = scenery_instance->Rotation[4];
        rotation2 = scenery_instance->Rotation[5];
        local_world->v1.x = static_cast<float>(rotation0) * rotation_scale;
        local_world->v1.y = static_cast<float>(rotation1) * rotation_scale;
        local_world->v1.z = static_cast<float>(rotation2) * rotation_scale;
        local_world->v1.w = row_w;

        rotation0 = scenery_instance->Rotation[6];
        rotation1 = scenery_instance->Rotation[7];
        rotation2 = scenery_instance->Rotation[8];
        local_world->v2.x = static_cast<float>(rotation0) * rotation_scale;
        local_world->v2.y = static_cast<float>(rotation1) * rotation_scale;
        local_world->v2.z = static_cast<float>(rotation2) * rotation_scale;
        local_world->v2.w = row_w;
        local_world->v3 = *reinterpret_cast<bVector4 *>(scenery_instance->Position);
        float matrix_w = lbl_8040B10C;
        local_world->v3.w = matrix_w;

        eModel **scenery_info_models = reinterpret_cast<eModel **>(reinterpret_cast<char *>(FindSceneryInfo(scenery_name_hash)) + 0x28);
        unsigned int detail_level = 0;

        do {
            if (*scenery_info_models != 0) {
                (*scenery_info_models)->UnInit();
            }
            detail_level++;
            scenery_info_models++;
        } while (detail_level < 4);
    }

    return 1;
}

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
        float scale = lbl_8040B278;
        bool rotate = false;
        float x = cameraMatrix->v3.x;
        float y = cameraMatrix->v3.y;
        float z = cameraMatrix->v3.z;
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
            if (layer == SKY_LAYER_BLUE) {
                scale = lbl_8040B280;
            } else if (layer == SKY_LAYER_CLOUDS) {
            } else if (layer == SKY_LAYER_OVERCAST) {
                rotate = true;
                scale = lbl_8040B284;
            } else if (layer == SKY_LAYER_REFLECTION) {
                rotate = true;
                matrix->v2.z = lbl_8040B28C;
                scale = lbl_8040B288;
                scaleZ = lbl_8040B290;
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
                    float skyScale = MainSkyScale * scale;

                    matrix->v3.z = z + scaleZ;
                    matrix->v0.x *= skyScale;
                    matrix->v1.y *= skyScale;
                    matrix->v2.z *= skyScale;
                    if (rotate) {
                        bMatrix4 rotation;
                        int angle = matAng_33578 + static_cast<int>(WorldTimeElapsed * lbl_8040B29C);

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

void RefreshCurrentSkyTextures() {
    for (int n = 0; n < 10; n++) {
        CurrentSkyTextures[n] = GetTextureInfo(SkyHash[n], 1, 0);
    }
}

void InitSkyHash(void (*callback)(int), int callback_param) {
    if (bSkyTexturesLoaded == 0) {
        eTimeOfDay tod;

        bSkyTexturesLoaded = 1;
        UserSkyLoadCallback = callback;
        UserSkyLoadCallbackParam = callback_param;
        tod = GetCurrentTimeOfDay();
        bMemSet(SkyHash, 0, 0x28);
        BaseSkyHash[0] = bStringHash(lbl_8040B110);
        BaseSkyHash[1] = bStringHash(lbl_8040B128);

        SkyHash[0] = bStringHash(lbl_8040B13C);
        SkyHash[1] = bStringHash(lbl_8040B128);
        SkyHash[2] = bStringHash(lbl_8040B110);
        SkyHash[3] = bStringHash(lbl_8040B128);
        SkyHash[4] = 0;
        SkyHash[5] = 0;
        SkyHash[6] = bStringHash(lbl_8040B154);
        SkyHash[7] = bStringHash(lbl_8040B170);
        SkyHash[8] = bStringHash(lbl_8040B198);
        SkyHash[9] = bStringHash(lbl_8040B198);

        eLoadStreamingTexture(SkyHash, 10, SkyLoadCallback, 0, 0);
    }
}

void SkyLoadCallback(unsigned int param) {
    RefreshCurrentSkyTextures();
    UserSkyLoadCallback(UserSkyLoadCallbackParam);
    UserSkyLoadCallback = 0;
}

void NotifySkyLoader() {
    SkyInitModel(&SkydomeModel, &SkydomeLocalWorld, 0xBE43EDBB);
    SkyInitModel(&SkySpecularModel, &SkySpecularLocalWorld, 0x90F70174);
    SkySpecularLocalWorld.v2.z = -1.0f;
    PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&SkydomeLocalWorld), *reinterpret_cast<Mtx44 *>(&SkydomeLocalReflectedWorld));
    SkydomeLocalReflectedWorld.v2.z = -1.0f;
}

void UnloadSkyTextures() {
    if (bSkyTexturesLoaded != 0) {
        eUnloadStreamingTexture(SkyHash, 10);
        bSkyTexturesLoaded = 0;
    }
}

void NotifySkyUnloader() {
    SkydomeModel.UnInit();
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&SkydomeLocalWorld));
    SkySpecularModel.UnInit();
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&SkySpecularLocalWorld));
}

void ReplaceSkyTextures(SKY_LAYER layer) {
    SKYtextable[0].hOldNameHash = BaseSkyHash[0];
    SKYtextable[1].hOldNameHash = BaseSkyHash[1];

    if (SkyHash[layer * 2] != SKYtextable[0].hNewNameHash) {
        SKYtextable[0].SetNewNameHash(SkyHash[layer * 2]);
    }

    if (SkyHash[layer * 2 + 1] != SKYtextable[1].hNewNameHash) {
        SKYtextable[1].SetNewNameHash(SkyHash[layer * 2 + 1]);
    }

    SKYtextable[0].pTextureInfo = reinterpret_cast<TextureInfo *>(-1);
    SKYtextable[1].pTextureInfo = reinterpret_cast<TextureInfo *>(-1);
    SkydomeModel.AttachReplacementTextureTable(SKYtextable, 2, 0);
}

void GetLayerMod(eView *view, SKY_LAYER layer, float *r, float *g, float *b, float *a) {
    float overcast = lbl_8040B2B8;
    float clear = lbl_8040B2BC;
    int env_map = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(view) + 0x60);

    if (env_map != 0) {
        overcast = *reinterpret_cast<float *>(env_map + 0x50);
        clear = lbl_8040B2BC - overcast;
    }

    if (lbl_8040B2B8 < SkyRenderForceOvercast) {
        clear = lbl_8040B2BC - SkyRenderForceOvercast;
        overcast = SkyRenderForceOvercast;
    }

    float *layer_mod = &skylayer[layer][0];
    *r = layer_mod[0] * overcast + layer_mod[1] * clear;
    *g = layer_mod[2] * overcast + layer_mod[3] * clear;
    *b = layer_mod[4] * overcast + layer_mod[5] * clear;
    *a = layer_mod[6] * overcast + layer_mod[7] * clear;
}
