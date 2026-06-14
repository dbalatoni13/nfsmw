#include "SkyRender.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Sun.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Scenery.hpp"
#include "TimeOfDay.hpp"
#include "Rain.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

uint32 GlobHorizEnable = 0;

// STRIPPED
void SetGlobHorizON() {}

// STRIPPED
void SetGlobHorizOFF() {}

eModel SkydomeModel;

bMatrix4 SkydomeLocalWorld;
bMatrix4 SkydomeLocalReflectedWorld;

float MainSkyScale = 1.0f;

static const float EnvSkyScale = 0.0065f;

static const float RefSkyScale = 0.035f;

static const float RVMSkyScale = 0.0095f;

eModel SkySpecularModel;
bMatrix4 SkySpecularLocalWorld;

static const float SKY_ENVMAP_HEIGHT_ADJUST = 50.0f;
static const float SKY_ENVMAP_HEIGHT_ADJUST2 = -50.0f;
static const float SKY_REFLECTION_HEIGHT_ADJUST = 130.0f;

float platfomSkyScale = 1.0f;

float skylayer[5][4][2] = {
    {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}}, {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}}, {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
};

int32 DrawSky = 1;

int32 DrawSkyEnvMap = 1;

int32 DrawSkySpecular = 1;
int32 DrawSkyInReflection = 0;

int32 UseMainSkyInEnvMap = 0;

// STRIPPED
void DisableHorizonTrans() {}

uint32 BaseSkyHash[2];
uint32 BaseSpecHash;
uint32 SkyHash[10];
TextureInfo *CurrentSkyTextures[10];

uint32 SKYHashIndex = 0;

int HorizClear = 1;
eReplacementTextureTable SKYtextable[2];

eReplacementTextureTable SPECtextable[1];

int PrintSkyRender = 0;

static const int VISUALIZE_WHICH_SKY = 0;

int SkyInitModel(eModel *model, bMatrix4 *local_world, unsigned int scenery_name_hash) {
    SceneryInstance *scenery_instance;

    if (model->GetNameHash() != 0) {
        return 1;
    }

    if (eFindSolid(scenery_name_hash) == nullptr) {
        return 0;
    }

    model->Init(scenery_name_hash);
    eIdentity(local_world);

    scenery_instance = FindSceneryInstance(scenery_name_hash);
    if (scenery_instance != nullptr) {
        scenery_instance->GetMatrix(local_world);

        SceneryInfo *scenery_info = FindSceneryInfo(scenery_name_hash);
        for (unsigned int i = 0; i < 4; i++) {
            if (scenery_info->pModel[i] != nullptr) {
                scenery_info->pModel[i]->UnInit();
            }
        }
    }

    return 1;
}

// STRIPPED
void SetPlatformSkyScale(float scale) {}

// STRIPPED
void SetPlatformSkyRVMScale(float rvmScale) {}

// STRIPPED
void SetRenderSkyReflection(int onOff) {}

void RefreshCurrentSkyTextures() {
    for (int n = 0; n < 10; n++) {
        CurrentSkyTextures[n] = GetTextureInfo(SkyHash[n], 1, 0);
    }
}

int SkyTexLoaded = 0;
void (*UserSkyLoadCallback)(intptr_t) = nullptr;
intptr_t UserSkyLoadCallbackParam;

void SkyLoadCallback(uintptr_t param) {
    RefreshCurrentSkyTextures();
    UserSkyLoadCallback(UserSkyLoadCallbackParam);
    UserSkyLoadCallback = nullptr;
}

// STRIPPED
TextureInfo *GetCurrentSkyTexture(SKY_LAYER l, SKY_TEX_TYPE type) {}

bool bSkyTexturesLoaded = false;

void InitSkyHash(void (*callback)(intptr_t), intptr_t callback_param) {
    if (!bSkyTexturesLoaded) {
        eTimeOfDay tod;

        bSkyTexturesLoaded = true;
        UserSkyLoadCallback = callback;
        UserSkyLoadCallbackParam = callback_param;

        tod = GetCurrentTimeOfDay();

        bMemSet(SkyHash, 0, sizeof(SkyHash));

        BaseSkyHash[0] = bStringHash("SKY_MIDDAY_A_CLOUDS_A");
        BaseSkyHash[1] = bStringHash("SKY_MIDDAY_A_CAP_A");

        SkyHash[0] = bStringHash("SKY_MIDDAY_A_GRADIENT_A");
        SkyHash[1] = bStringHash("SKY_MIDDAY_A_CAP_A");
        SkyHash[2] = bStringHash("SKY_MIDDAY_A_CLOUDS_A");
        SkyHash[3] = bStringHash("SKY_MIDDAY_A_CAP_A");
        SkyHash[4] = 0;
        SkyHash[5] = 0;
        SkyHash[6] = bStringHash("SKY_MIDDAY_A_ENVIRONMENT_A");
        SkyHash[7] = bStringHash("SKY_MIDDAY_A_ENVIRONMENT_CAP_A_GAMECUBE");
        SkyHash[8] = bStringHash("SKY_MIDDAY_A_REFLECTION");
        SkyHash[9] = bStringHash("SKY_MIDDAY_A_REFLECTION");

        eLoadStreamingTexture(SkyHash, 10, SkyLoadCallback, 0, 0);
    }
}

void NotifySkyLoader() {
    SkyInitModel(&SkydomeModel, &SkydomeLocalWorld, STRINGHASH_SKYDOME);
    SkyInitModel(&SkySpecularModel, &SkySpecularLocalWorld, STRINGHASH_SKY_SPECULAR);
    SkySpecularLocalWorld.v2.z = -1.0f;
    SkydomeLocalReflectedWorld = SkydomeLocalWorld;
    SkydomeLocalReflectedWorld.v2.z = -1.0f;
}

void UnloadSkyTextures() {
    if (bSkyTexturesLoaded) {
        eUnloadStreamingTexture(SkyHash, NUM_ELEMENTS(SkyHash));
        bSkyTexturesLoaded = false;
    }
}

void NotifySkyUnloader() {
    SkydomeModel.UnInit();
    eIdentity(&SkydomeLocalWorld);
    SkySpecularModel.UnInit();
    eIdentity(&SkySpecularLocalWorld);
}

bVector3 SunPos;

bAngle SpecularOffset = 17500;

bAngle CloudOffest = 40;

static const int LockSky = 1;
static const int MoveClouds = 1;
static const float MoveCloudsStep = 300.0f;

void ReplaceSkyTextures(SKY_LAYER layer) {
    SKYtextable[0].SetOldNameHash(BaseSkyHash[0]);
    SKYtextable[1].SetOldNameHash(BaseSkyHash[1]);

    SKYtextable[0].SetNewNameHash(SkyHash[layer * 2]);
    SKYtextable[1].SetNewNameHash(SkyHash[layer * 2 + 1]);

    SKYtextable[0].InvalidateTexture();
    SKYtextable[1].InvalidateTexture();

    SkydomeModel.AttachReplacementTextureTable(SKYtextable, 2, 0);
}

int deblayer[5] = {1, 1, 1, 1, 1};

// UNSOLVED
void StuffSkyLayer(eView *view, SKY_LAYER layer) {
    if (!deblayer[layer]) {
        return;
    }

    static unsigned short matAng = 0;
    float time = WorldTimeElapsed;
    Camera *view_camera = view->GetCamera();
    bVector3 CamPosWORLD = *view_camera->GetPosition();
    bMatrix4 *SkydomeLocalWorld = eFrameMallocMatrix(1);

    if (SkydomeLocalWorld == nullptr) {
        return;
    }

    eIdentity(SkydomeLocalWorld);

    float ScaleFactor = 1.0f;
    int MoveMent = 0;
    float heightAdjust = 0.0f;
    int view_id = view->GetID();

    ReplaceSkyTextures(layer);

    if (layer == SKY_LAYER_BLUE) {
        ScaleFactor = 1.1f;
    } else if (layer != SKY_LAYER_CLOUDS) {
        if (layer == SKY_LAYER_OVERCAST) {
            MoveMent = 1;
            ScaleFactor = 0.8f;
        } else if (layer == SKY_LAYER_REFLECTION) {
            ScaleFactor = RefSkyScale;
            MoveMent = 1;
            SkydomeLocalWorld->v2.z = -1.0f;
            heightAdjust = SKY_REFLECTION_HEIGHT_ADJUST;
        }
    }

    if (view_id >= EVIEW_FIRST_ENVMAP && view_id <= EVIEW_LAST_ENVMAP) { // TODO
        ScaleFactor = EnvSkyScale;
        if (!DrawSkyEnvMap) {
            return;
        }
    } else if (view_id == EVIEW_PLAYER1_RVM) {
        ScaleFactor = RVMSkyScale;
    }

    if (LockSky) {
        SkydomeLocalWorld->v3.x = CamPosWORLD.x;
        SkydomeLocalWorld->v3.y = CamPosWORLD.y;
        SkydomeLocalWorld->v3.z = CamPosWORLD.z;
    }

    if (view_id >= EVIEW_FIRST_PLAYER && view_id <= EVIEW_LAST_PLAYER) {
        if (DrawSky) {
            SkydomeLocalWorld->v3.z = CamPosWORLD.z + heightAdjust;
            SkydomeLocalWorld->v0.x *= MainSkyScale * ScaleFactor;
            SkydomeLocalWorld->v2.z *= MainSkyScale * ScaleFactor;
            SkydomeLocalWorld->v1.y *= MainSkyScale * ScaleFactor;

            if (MoveMent) {
                bMatrix4 LocalRot;

                if (MoveClouds) {
                    matAng += static_cast<unsigned short>(time * MoveCloudsStep);
                } else {
                    matAng = CloudOffest;
                }
                eCreateRotationZ(&LocalRot, matAng);
                eMulMatrix(SkydomeLocalWorld, &LocalRot, SkydomeLocalWorld);
            }

            view->Render(&SkydomeModel, SkydomeLocalWorld, nullptr, 0x20000, nullptr);
        }
    } else {
        SkydomeLocalWorld->v3.z = CamPosWORLD.z + heightAdjust;
        SkydomeLocalWorld->v0.x *= ScaleFactor;
        SkydomeLocalWorld->v1.y *= ScaleFactor;
        SkydomeLocalWorld->v2.z *= ScaleFactor;

        view->Render(&SkydomeModel, SkydomeLocalWorld, nullptr, 0x20000, nullptr);
    }
}

// UNSOLVED
void StuffSpecular(eView *view) {
    ProfileNode profile_node("TODO", 0);
    int view_id = view->GetID();
    Camera *view_camera = view->GetCamera();
    bVector3 CamPosWORLD(*view_camera->GetPosition());
    bMatrix4 *SkydomeLocalWorld = eFrameMallocMatrix(1);

    if (SkydomeLocalWorld == nullptr) {
        return;
    }

    eIdentity(SkydomeLocalWorld);
    SkydomeLocalWorld->v3.x = CamPosWORLD.x;
    SkydomeLocalWorld->v3.y = CamPosWORLD.y;
    SkydomeLocalWorld->v3.z = CamPosWORLD.z;

    if (view_id != EVIEW_PLAYER1_SPECULAR && view_id != EVIEW_PLAYER2_SPECULAR) {
        return;
    }

    GetSunPos(view, &SunPos.x, &SunPos.y, &SunPos.z);
    SunPos.z = 0.0f;
    SkydomeLocalWorld->v0.x *= 0.035f;
    SkydomeLocalWorld->v1.y *= 0.035f;
    SkydomeLocalWorld->v2.z *= -0.035f;
    SkydomeLocalWorld->v3.z += 50.0f;

    {
        bVector3 MySunDir = SunPos - CamPosWORLD;
        bNormalize(&MySunDir, &MySunDir);
        bVector3 Ahead(0.0f, 1.0f, 0.0f);
        float matCos = bDot(&MySunDir, &Ahead);
        unsigned short matAng = bACos(matCos);
        if (MySunDir.y < 0.0f) {
            matAng = -matAng;
        }

        bMatrix4 matmat;
        bMatrix4 LocalRot;

        eCreateRotationZ(&LocalRot, static_cast<unsigned short>(matAng + SpecularOffset));
        eMulMatrix(SkydomeLocalWorld, &LocalRot, SkydomeLocalWorld);
        view->Render(&SkySpecularModel, SkydomeLocalWorld, nullptr, 0, nullptr);

        bMatrix4 *SkydomeLocalWorld2 = eFrameMallocMatrix(1);

        eIdentity(SkydomeLocalWorld2);
        SkydomeLocalWorld2->v3.x = CamPosWORLD.x;
        SkydomeLocalWorld2->v3.y = CamPosWORLD.y;
        SkydomeLocalWorld2->v3.z = -50.0f + CamPosWORLD.z;

        SkydomeLocalWorld2->v0.x *= 0.035f;
        SkydomeLocalWorld2->v1.y *= 0.035f;
        SkydomeLocalWorld2->v2.z *= 0.035f;
        eMulMatrix(SkydomeLocalWorld2, &LocalRot, SkydomeLocalWorld2);
        view->Render(&SkySpecularModel, SkydomeLocalWorld2, nullptr, 0, nullptr);
    }
}

float MinSkySpecular = 32.0f;
float MaxSkySpecular = 200.0f;

float SkyRenderForceOvercast = -1.0f;

void GetLayerMod(eView *view, SKY_LAYER l, float *r, float *g, float *b, float *a) {
    float weight = 0.0f;
    float invW = 1.0f;

    if (view->Precipitation != nullptr) {
        weight = view->Precipitation->GetCloudIntensity();
        invW = 1.0f - weight;
    }

    if (SkyRenderForceOvercast > 0.0f) {
        weight = SkyRenderForceOvercast;
        invW = 1.0f - weight;
    }

    *r = skylayer[l][0][0] * weight + skylayer[l][0][1] * invW;
    *g = skylayer[l][1][0] * weight + skylayer[l][1][1] * invW;
    *b = skylayer[l][2][0] * weight + skylayer[l][2][1] * invW;
    *a = skylayer[l][3][0] * weight + skylayer[l][3][1] * invW;
}
