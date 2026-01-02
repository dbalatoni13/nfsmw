#include "eLight.hpp"
#include "Ecstasy.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "eMath.hpp"

bTList<eLightMaterial> LightMaterialList;    // size: 0x8, address: 0x80460DDC
eLightMaterial DefaultLightMaterialData;     // size: 0xA8, address: 0x80460DE4
static eLightMaterial *DefaultLightMaterial; // size: 0x4, address: 0x8041A63C
int DrawLightFlares;

int LoaderLights(bChunk *bchunk /* r30 */) {}

// UNSOLVED but identical
int UnloaderLights(bChunk *bchunk) {
    if (bchunk->GetID() == BCHUNK_LIGHT_MATERIALS) {
        eLightMaterial *light_material = reinterpret_cast<eLightMaterial *>(bchunk->GetData());
        if (light_material->Version == 3) {
            light_material->Remove();
            if (light_material->NameHash == BINHASH(DEFAULT)) {
                DefaultLightMaterial = &DefaultLightMaterialData;
            }
        }
    } else if (bchunk->GetID() == BCHUNK_LIGHT_FLARES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        eLightFlarePackHeader *pack_header;
        while (chunk != last_chunk) {
            if (chunk->GetID() == BCHUNK_LIGHT_FLARE_PACK_HEADER) {
                pack_header = reinterpret_cast<eLightFlarePackHeader *>(chunk->GetAlignedData(16));
                if (pack_header->Version != 3) {
                    return 1;
                }
                pack_header->Remove();

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(pack_header->ScenerySectionNumber);
                user_info->pLightFlarePack = nullptr;
                TheVisibleSectionManager.UnallocateUserInfo(pack_header->ScenerySectionNumber);
                return 1;
            }
            chunk = chunk->GetNext();
        }
    } else if (bchunk->GetID() == BCHUNK_LIGHT_SOURCES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        while (chunk != last_chunk) {
            if (chunk->GetID() == BCHUNK_LIGHT_SOURCE_PACK_HEADER) {
                eLightPack *light_pack = reinterpret_cast<eLightPack *>(chunk->GetAlignedData(16));
                if (light_pack->Version != 4) {
                    return 1;
                }
                light_pack->Remove();

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(light_pack->ScenerySectionNumber);
                user_info->pLightPack = nullptr;
                TheVisibleSectionManager.UnallocateUserInfo(light_pack->ScenerySectionNumber);
                return 1;
            }
        }
    } else {
        return 0;
    }
    return 1;
}

void SetSelectCarLighting(int, float, int) {}

void SphericalToCartesian(bVector3 *v, float theta, float phi, float radius) {
    float cos_theta;
    float sin_theta;
    float cos_phi;
    float sin_phi;

    bSinCos(&sin_theta, &cos_theta, bDegToAng(theta));
    bSinCos(&sin_phi, &cos_phi, bDegToAng(phi));
    v->x = radius * cos_theta * sin_phi;
    v->y = radius * sin_theta * sin_phi;
    v->z = radius * cos_phi;
}

void CartesianToSpherical(bVector3 *v, float x, float y, float z) {
    float r;
    float t;
    float p;

    r = bSqrt(x * x + y * y + z * z);
    t = bATan(x, y);
    p = bACos(z / r);
    v->x = t;
    v->y = p;
    v->z = r;
}

void elRotateLightContext(eDynamicLightContext *new_light_context, eDynamicLightContext *light_context, bMatrix4 *rotate_matrix) {
    bMatrix4 invrotate_matrix;
    bVector4 light_directions[3];

    *new_light_context = *light_context;

    eInvertRotationMatrix(&invrotate_matrix, rotate_matrix);
    light_directions[0].x = light_context->LocalDirectionMatrix.v0.x;
    light_directions[0].y = light_context->LocalDirectionMatrix.v1.x;
    light_directions[0].z = light_context->LocalDirectionMatrix.v2.x;

    light_directions[1].x = light_context->LocalDirectionMatrix.v0.y;
    light_directions[1].y = light_context->LocalDirectionMatrix.v1.y;
    light_directions[1].z = light_context->LocalDirectionMatrix.v2.y;

    light_directions[2].x = light_context->LocalDirectionMatrix.v0.z;
    light_directions[2].y = light_context->LocalDirectionMatrix.v1.z;
    light_directions[2].z = light_context->LocalDirectionMatrix.v2.z;

    light_directions[0].w = 0.0f;
    light_directions[1].w = 0.0f;
    light_directions[2].w = 0.0f;

    eMulVector(&light_directions[0], &invrotate_matrix, &light_directions[0]);
    eMulVector(&light_directions[1], &invrotate_matrix, &light_directions[1]);
    eMulVector(&light_directions[2], &invrotate_matrix, &light_directions[2]);

    new_light_context->LocalDirectionMatrix.v0.x = light_directions[0].x;
    new_light_context->LocalDirectionMatrix.v1.x = light_directions[0].y;
    new_light_context->LocalDirectionMatrix.v2.x = light_directions[0].z;

    new_light_context->LocalDirectionMatrix.v0.y = light_directions[1].x;
    new_light_context->LocalDirectionMatrix.v1.y = light_directions[1].y;
    new_light_context->LocalDirectionMatrix.v2.y = light_directions[1].z;

    new_light_context->LocalDirectionMatrix.v0.z = light_directions[2].x;
    new_light_context->LocalDirectionMatrix.v1.z = light_directions[2].y;
    new_light_context->LocalDirectionMatrix.v2.z = light_directions[2].z;
}

int elSetupEnvMap(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position) {
    bMatrix4 world_local;

    eInvertRotationMatrix(&world_local, local_world);
    if (camera_world_position) {
        bVector4 v;
        v.x = camera_world_position->x - local_world->v3.x;
        v.y = camera_world_position->y - local_world->v3.y;
        v.z = camera_world_position->z - local_world->v3.z;
        v.w = 1.0f;
        eMulVector(&v, &world_local, &v);

        bool in_front_end = IsGameFlowInFrontEnd();
        float distance_min = 2.78f;
        float distance_max = in_front_end ? 8.0f : 2.78f;
        float distance = bLength(&v);

        if (distance < distance_min) {
            distance = distance_min;
        } else if (distance > distance_max) {
            distance = distance_max;
        }

        v.w = 0.0f;
        bNormalize(&v, &v);
        bScale(&v, &v, distance);
        v.w = 1.0f;
        light_context->LocalEyePosition = v;
        light_context->LocalEyePosition.w = 1.0f;
    }
    if (world_view && camera_world_position) {
        bMatrix4 *envmap_matrix = &light_context->EnvMapMatrix;
        bVector4 *camera_local_space;
        eMulMatrix(envmap_matrix, local_world, world_view);

        envmap_matrix->v3.x = light_context->LocalEyePosition.x;
        envmap_matrix->v3.y = light_context->LocalEyePosition.y;
        envmap_matrix->v3.z = light_context->LocalEyePosition.z;

        envmap_matrix->v0.w = 0.0f;
        envmap_matrix->v1.w = 0.0f;
        envmap_matrix->v2.w = 0.0f;
        envmap_matrix->v3.w = 0.0f;
    }
    return 1;
}

void elResetLightContext(eDynamicLightContext *light_context) {
    bMemSet(light_context, 0, sizeof(*light_context));
}

int elSetupLights(eDynamicLightContext *light_context /* r27 */, eShaperLightRig *shaper_lights /* r26 */, bVector3 *local_pos /* r22 */,
                  bMatrix4 *local_world /* r1+0x4B8 */, bMatrix4 *world_view /* r4 */, eView *view /* r8 */) {}

int elSetupLightContext(eDynamicLightContext *light_context /* r31 */, eShaperLightRig *shaper_lights /* r28 */, bMatrix4 *local_world /* r30 */,
                        bMatrix4 *world_view /* r27 */, bVector4 *camera_world_position /* r29 */, eView *view /* r26 */) {
    // ProfileNode profile_node;

    if (!light_context || !local_world || !shaper_lights) {
        return 0;
    } else {
        elResetLightContext(light_context);
        elSetupEnvMap(light_context, local_world, world_view, camera_world_position);
        elSetupLights(light_context, shaper_lights, reinterpret_cast<bVector3 *>(&local_world->v3), local_world, world_view, view);
        return 1;
    }
}

int elCloneLightContext(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position,
                        eView *view, eDynamicLightContext *old_context) {
    elRotateLightContext(light_context, old_context, local_world);
    elSetupEnvMap(light_context, local_world, world_view, camera_world_position);
    return 1;
}

void UpdateLightFlareParameters() {}

float indep_fpow(float n, float p) {
    return ePowf(n, p);
}

eLightFlare PoolLightFlareList[50];      // size: 0x960, address: 0x804615F4
unsigned int PoolOfFlaresXcludeView[50]; // size: 0xC8, address: 0x8046B140
int ActivePoolIndex;                     // size: 0x4, address: 0x8041AB30

eLightFlare *eGetNextLightFlareInPool(unsigned int XcludeViewIDs /* r7 */) {
    if (ActivePoolIndex == 50) {
        return nullptr;
    }
    PoolOfFlaresXcludeView[ActivePoolIndex] = XcludeViewIDs;
    return &PoolLightFlareList[ActivePoolIndex++];
}

void eInitLightFlarePool() {
    static BOOL done = false;
    int PoolIndex;

    if (done) {
        return;
    }
    done = true;
    PoolIndex = 0;
    for (int i = 0; i < 50; i++) {
        eLightFlare *light_flare = &PoolLightFlareList[PoolIndex];
        light_flare->Flags = 0;
        light_flare->DirectionX = 0.0f;
        light_flare->DirectionY = 0.0f;
        light_flare->DirectionZ = -1.0f;
        light_flare->ReflectPosZ = 999.0f;
        PoolIndex++;
    }
}

int blink[4]; // size: 0x10, address: 0x8041AB38

void eResestLightFlarePool() {
    static float time[3];

    time[0] += WorldTimeElapsed;
    time[1] += WorldTimeElapsed;
    time[2] += WorldTimeElapsed;
    if (time[0] > 0.5f) {
        time[0] = 0.0f;
        blink[0] = ~blink[0];
    }
    if (time[1] > 0.4f) {
        time[1] = 0.0f;
        blink[1] = ~blink[1];
    }
    if (time[2] > 0.45f) {
        time[2] = 0.0f;
        blink[2] = ~blink[2];
    }
    ActivePoolIndex = 0;
}

// UNSOLVED
void eRenderLightFlarePool(eView *view /* r30 */) {
    bMatrix4 *local_world = eGetIdentityMatrix();
    unsigned int vid = 1 << view->GetID();
    for (int i = 0; i < ActivePoolIndex; i++) {
        float intensity_scale = 1.0f;
        eLightFlare *light_flare = &PoolLightFlareList[i];
        if ((light_flare->Type == 18) && (blink[0] == 0)) {
            continue;
        }
        int amBlink = bAbs((int)light_flare->PositionX % 3);

        if (((light_flare->Type != 17) || (blink[amBlink] != 0)) && (light_flare->Type != 19 || (blink[2] != 0))) {
            if (light_flare->Type == 21) {
                intensity_scale = GetSunIntensity(eGetView(1, false));
            }
            if ((vid & PoolOfFlaresXcludeView[i]) == 0) {
                if ((view->GetID() == EVIEW_FIRST_RVM) || (view->GetID() > EVIEW_SHADOWMATTE && (view->GetID() < NUM_EVIEWS))) {
                    eRenderLightFlare(view, light_flare, local_world, intensity_scale, REF_NONE, FLARE_ENV, 0.0f, 0, 1.0f);
                } else {
                    eRenderLightFlare(view, light_flare, local_world, 1.0f, REF_NONE,
                                      view->GetID() == EVIEW_FIRST_PLAYER ? FLARE_CAT_P1 : FLARE_CAT_P2, 0.0f, 0, 1.0f);
                }
            }
        }
    }
}

unsigned int eLightFlareTextureNameHashes[3]; // size: 0xC, address: 0x8041A69C
TextureInfo *eLightFlareTextureInfos[3];      // size: 0xC, address: 0x8046B134

void eLightUpdateTextures() {
    for (unsigned int i = 0; i < 3; i++) {
        eLightFlareTextureInfos[i] = GetTextureInfo(eLightFlareTextureNameHashes[i], 0, 0);
    }
}

void eRenderWorldLightFlares(eView *view /* r26 */, flareType type /* r24 */) {
    bMatrix4 *local_world; // r23
    int num_visible;
    int num_scenery_culled;
    int total;
    DrivableScenerySection *scenery_section;
    bVector2 *camera_position;

    if (DrawLightFlares != 0 && IsGameFlowInGame()) {
        camera_position = reinterpret_cast<bVector2 *>(view->GetCamera()->GetPosition());
        local_world = eGetIdentityMatrix();
        scenery_section = TheVisibleSectionManager.FindDrivableSection(camera_position);
        if (!scenery_section) {
            return;
        }
        for (int i = 0; i < scenery_section->NumVisibleSections; i++) {
            VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(scenery_section->GetVisibleSection(i));
            if (user_info) {
                eLightFlarePackHeader *pack = user_info->pLightFlarePack;
                if (pack && view->GetVisibleState(&pack->BBoxMin, &pack->BBoxMax, nullptr) != EVISIBLESTATE_NOT) {
                    for (eLightFlare *light_flare = pack->LightFlareList.GetHead(); light_flare != pack->LightFlareList.EndOfList();
                         light_flare = light_flare->GetNext()) {
                        eRenderLightFlare(view, light_flare, local_world, 1.0f, REF_NONE, type, 0.0f, 0, 1.0f);
                    }
                }
            }
        }
    }
}

void RestoreShaperRig(eShaperLightRig *ShaperRigP, unsigned int slot, eShaperLightRig *ShaperRigBP) {
    ShaperRigP->Lights[slot].CameraSpace = ShaperRigBP->Lights[slot].CameraSpace;
    ShaperRigP->Lights[slot].Red = ShaperRigBP->Lights[slot].Red;
    ShaperRigP->Lights[slot].Green = ShaperRigBP->Lights[slot].Green;
    ShaperRigP->Lights[slot].Blue = ShaperRigBP->Lights[slot].Blue;
    ShaperRigP->Lights[slot].Scale = ShaperRigBP->Lights[slot].Scale;
    ShaperRigP->Lights[slot].Theta = ShaperRigBP->Lights[slot].Theta;
    ShaperRigP->Lights[slot].Phi = ShaperRigBP->Lights[slot].Phi;
}

void AddQuickDynamicLight(eShaperLightRig *ShaperRigP, unsigned int slot, float r, float g, float b, float intensity, bVector3 *position) {
    unsigned int mask = 1 << (slot - 2);

    ShaperRigP->NumOverideSlots |= mask;
    ShaperRigP->position = *position;

    ShaperRigP->Lights[slot].CameraSpace = LIGHT_WORLD_POSITION;
    ShaperRigP->Lights[slot].Red = r;
    ShaperRigP->Lights[slot].Green = g;
    ShaperRigP->Lights[slot].Blue = b;
    ShaperRigP->Lights[slot].Scale = intensity;
}

void AdjustQuickDynamicLight(eShaperLightRig *ShaperRigP, bVector3 *MyPosition) {
    for (int i = 0; i < 0; i++) {
        unsigned int slot;
        unsigned int mask;
        if (0 == 0) {
            bVector3 direction = ShaperRigP->position - *MyPosition;
            bVector3 spherical_direction;
            bVector3 idk;
        }
    }
}

void eLightMaterial::BuildData() {
    this->UpdatePlatInfo();
}

eLightMaterial *elGetLightMaterial(unsigned int name_hash /* r10 */) {
    for (eLightMaterial *elm = LightMaterialList.GetHead(); elm != LightMaterialList.EndOfList(); elm = elm->GetNext()) {
        if (elm->NameHash == name_hash) {
            return elm;
        }
    }
    return DefaultLightMaterial;
}

eLightMaterial *elGetDefaultLightMaterial() {
    return DefaultLightMaterial;
}

void elInit() {
    bMemSet(DefaultLightMaterial, 0, sizeof(*DefaultLightMaterial));
    elInitPlat();
    eInitLightFlarePool();
}

void elBeginFrame() {}
