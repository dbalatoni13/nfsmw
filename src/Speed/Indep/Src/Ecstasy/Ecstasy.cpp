#include "./Ecstasy.hpp"

#include "./EcstasyE.hpp"
#include "./Texture.hpp"
#include "./eEnvMapE.hpp"
#include "./eLight.hpp"
#include "./eModel.hpp"
#include "./eSolid.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Animation/AnimEngineManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Src/World/SimpleModelAnim.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

float renderModifier = 1.0f;
unsigned int numCopsActiveCherry;
unsigned int numCopsActiveTotal;
unsigned int numCopsActiveView;
unsigned char *FrameMemoryBuffer[2];
unsigned int FrameMemoryBufferSize = 0;
int FrameMemoryBufferAmountUsed[2];
//
unsigned int FrameMallocAllocNum = 0;
unsigned int FrameMallocFailed = 0;
unsigned int FrameMallocFailAmount = 0;
//
unsigned int FrameMallocMaxFailAmount = 0;

int Eframelargest = 0;
int Eframecurrent = 0;

TextureInfo *OtherEcstacyTextures[30];
unsigned int OtherEcstacyTextures_name_hash[30];

unsigned int numOtherTex = 0;

void eAllocateFrameMallocBuffers(unsigned int total_size /* r3 */);

int eInitEngine() {
    eAllocateFrameMallocBuffers(0xE1000);
    ePolySlotPool = bNewSlotPool(0x94, 0x20, "ePolySlotPool", 0);
    ePolySlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
    eInitModels();
    eInitTextures();
    eInitSolids();
    epInitViews();
    elInit();
    eInitEnvMap();
    InitNFSAnimEngine();
    SimpleModelAnim::Init();
    return eInitEnginePlat();
}

// STRIPPED
int eCloseEngine() {}

void ePreDisplay() {
    TickSFX();
    RenderSunAsFlare();
    SimpleModelAnim::Update();
    DoTunnelBloom(eGetView(1, false));
    DoTunnelBloom(eGetView(2, false));
    if (!IsRainDisabled()) {
        DoTinting(eGetView(1, false));
    }
    UpdateLightFlareParameters();
    numCopsActiveView = eGetView(1, false)->NumCopsInView;
    numCopsActiveTotal = eGetView(1, false)->NumCopsTotal;
    numCopsActiveCherry = eGetView(1, false)->NumCopsCherry;
    eGetView(1, false)->NumCopsInView = 0;
    eGetView(1, false)->NumCopsTotal = 0;
    eGetView(1, false)->NumCopsCherry = 0;
    renderModifier = 1.0f - numCopsActiveCherry / 5.0f;
    if (renderModifier < 0.25f) {
        renderModifier = 0.25f;
    }
}

void ePostDisplay() {
    UpdateTextureAnimations();
    UpdateAllScreenEFX();
    eResestLightFlarePool();
    if (QueryFlushAccumulationBuffer()) {
        AccumulationBufferFlushed();
    }
}

void eNotifyTextureLoading(TexturePack *texture_pack, TextureInfo *texture_info, bool loading) {
    eSolidNotifyTextureLoading(texture_pack, texture_info, loading);
    FEngFontNotifyTextureLoading(texture_pack, loading);
}

void eFixUpTables() {
    if (eDisableFixUpTables == 0) {
        if (eDirtySolids != 0 || eDirtyTextures != 0) {
            eFixUpTablesPlat();
            eDirtySolids = 0;
            eDirtyTextures = 0;
            eDirtyAnimations = 0;
        }
    }
}

void eAllocateFrameMallocBuffers(unsigned int total_size) {
    unsigned int buffer_size = (total_size >> 1) & 0x7FFFFFF0; // r29

    FrameMemoryBuffer[0] = new (__FILE__, __LINE__) unsigned char[buffer_size];
    FrameMemoryBuffer[1] = new (__FILE__, __LINE__) unsigned char[buffer_size];

    FrameMemoryBufferSize = buffer_size;
    CurrentBufferPos = FrameMemoryBuffer[0];
    CurrentBufferEnd = FrameMemoryBuffer[0] + buffer_size;
    FrameMallocAllocNum = 0;
}

// STRIPPED
void eFreeFrameMallocBuffers() {}

void eSwapFrameMallocBuffers() {
    unsigned char *buffer0; // r30
    unsigned char *buffer1; // r3
    int amount_used;        // r4

    if (FrameMallocFailed != 0 && FrameMallocFailAmount > FrameMallocMaxFailAmount) {
        FrameMallocMaxFailAmount = FrameMallocFailAmount;
    }

    buffer0 = FrameMemoryBuffer[0];
    buffer1 = FrameMemoryBuffer[1];

    FrameMallocFailed = 0;
    FrameMallocFailAmount = 0;

    amount_used = CurrentBufferPos - buffer0;
    if (amount_used > Eframelargest) {
        Eframelargest = amount_used;
    }

    Eframecurrent = amount_used;
    FrameMemoryBufferAmountUsed[1] = FrameMemoryBufferAmountUsed[0];
    FrameMemoryBufferAmountUsed[0] = amount_used;
    FrameMemoryBuffer[1] = buffer0;
    FrameMemoryBuffer[0] = buffer1;
    CurrentBufferStart = buffer1;
    CurrentBufferPos = buffer1;
    CurrentBufferEnd = FrameMemoryBufferSize + buffer1;
    FrameMallocAllocNum = 0;
}

// STRIPPED
int eGetSizeofFrameMallocBuffers(int *pamount_used /* r3 */) {}

void SetupSceneryCullInfo(eView *view /* r30 */, SceneryCullInfo &info /* r29 */, int exclude_flags /* r31 */) {
    EVIEWMODE view_mode; // r3
    int view_id;         // r11

    view_mode = eGetCurrentViewMode();
    exclude_flags |= 0x10;
    view_id = view->GetID();
    if (view_id == 1 || view_id == 2)
        exclude_flags |= 2;
    if (view_mode - 3 <= 1U)
        exclude_flags |= 1;
    if (view_id - 16 <= 5U)
        exclude_flags |= 0x100;

    if (GRaceStatus::fObj->Exists() && GRaceStatus::fObj->Get().GetPlayMode() == 1)
        exclude_flags |= 4;
    if (exclude_flags & 0x1800)
        exclude_flags |= 0x40;
    if (exclude_flags & 0x1000)
        exclude_flags |= 2;
    if (view_id == 3)
        exclude_flags |= 0x20;

    SetupSceneryCullInfoPlat(view, info);
    info.ExcludeFlags = exclude_flags;
}

void eSwapDynamicFrameMemory() {}

// STRIPPED
void eTagHeadlightCallback(SceneryDrawInfo *info) {
    info->SceneryInst->Rotation[1] = 512;
}

// STRIPPED
void RenderMWDebugTopology(eView *view /* r3 */) {};

// STRIPPED
void eDebugRender(eView *view /* r4 */) {}

// STRIPPED
void eAddOtherEcstacyTexture(unsigned int name_hash /* r3 */, TextureInfo *tex /* r4 */) {};

struct TextureInfo *eGetOtherEcstacyTexture(unsigned int name_hash) {
    if (numOtherTex < 0)
        return NULL;

    for (unsigned int i = 0; i < numOtherTex; i++) {
        if (OtherEcstacyTextures_name_hash[i] == name_hash) {
            return OtherEcstacyTextures[i];
        }
    }

    return nullptr;
}

// STRIPPED
void eRemoveOtherEcstacyTexture(unsigned int name_hash /* r3 */) {}

bool eIsWidescreen() {
    return false;
} // Unlocks fullscreen on GameCube for free?

enum polyCountEnum {
    NUM_POLY_COUNT_TYPES = 10,
    POLY_COUNT_RVM_WORLDMODEL = 9,
    POLY_COUNT_REFLECTION_WORLDMODEL = 8,
    POLY_COUNT_MAIN_WORLDMODEL = 7,
    POLY_COUNT_RVM_SCENERY = 6,
    POLY_COUNT_REFLECTION_SCENERY = 5,
    POLY_COUNT_MAIN_SCENERY = 4,
    POLY_COUNT_ENVMAP_SCENERY = 3,
    POLY_COUNT_RVM_CAR = 2,
    POLY_COUNT_REFLECTION_CAR = 1,
    POLY_COUNT_MAIN_CAR = 0,
};

// STRIPPED
void AddPolyCount(polyCountEnum type /* r3 */, struct eModel *model /* r4 */) {}
void ClearPolyCounts() {}
int GetPolyCount(polyCountEnum type /* r3 */) {}
int GetModelCount(polyCountEnum type /* r3 */) {}
char *GetPCType(polyCountEnum type /* r3 */) {}
