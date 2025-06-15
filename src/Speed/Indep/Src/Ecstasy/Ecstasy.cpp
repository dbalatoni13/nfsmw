#include "./Ecstasy.hpp"

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Animation/AnimEngineManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/World/SimpleModelAnim.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "./eModel.hpp"
#include "./EcstasyE.hpp"
#include "./Texture.hpp"
#include "./eSolid.hpp"
#include "./eLight.hpp"
#include "./eEnvMapE.hpp"

float renderModifier;
unsigned int numCopsActiveCherry;
unsigned int numCopsActiveTotal;
unsigned int numCopsActiveView;
unsigned char * FrameMemoryBuffer[2];
unsigned int FrameMemoryBufferSize;
int FrameMemoryBufferAmountUsed[2];
unsigned int FrameMallocMaxFailAmount;
int Eframelargest;
int Eframecurrent;

unsigned int FrameMallocAllocNum;
unsigned int FrameMallocFailed;
unsigned int FrameMallocFailAmount;

TextureInfo *OtherEcstacyTextures[30];
unsigned int OtherEcstacyTextures_name_hash[30];
unsigned int numOtherTex;

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

//UNSOLVED: https://decomp.me/scratch/D4lah
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
    numCopsActiveCherry = eGetView(2, false)->NumCopsCherry;
    numCopsActiveTotal = eGetView(2, false)->NumCopsTotal;
    numCopsActiveView = eGetView(2, false)->NumCopsInView;
    renderModifier = 1.0f - numCopsActiveCherry * 0.2f;
    eGetView(1, false)->NumCopsCherry = 0;
    eGetView(1, false)->NumCopsTotal = 0;
    eGetView(1, false)->NumCopsInView = 0;
    if (!(renderModifier >= 0.25f)) {
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

void eNotifyTextureLoading(TexturePack * texture_pack, TextureInfo * texture_info, bool loading) {
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
    
    FrameMemoryBuffer[0] = (unsigned char *)__vn(buffer_size, "d:/mw/speed/gamecube/src/ecstasy/Ecstasy.cpp", 315);
    FrameMemoryBuffer[1] = (unsigned char *)__vn(buffer_size, "d:/mw/speed/gamecube/src/ecstasy/Ecstasy.cpp", 316);

    FrameMemoryBufferSize = buffer_size;
    CurrentBufferPos = FrameMemoryBuffer[0];
    CurrentBufferEnd = FrameMemoryBuffer[0] + buffer_size;
    FrameMallocAllocNum = 0;
}

void eSwapFrameMallocBuffers() {
    unsigned char * buffer0; // r30
    unsigned char * buffer1; // r3
    int amount_used; // r4

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

void SetupSceneryCullInfo(eView *view /* r30 */, SceneryCullInfo &info /* r29 */, int exclude_flags /* r31 */) {
    EVIEWMODE view_mode; // r3
    int view_id; // r11

    view_mode = eGetCurrentViewMode();
    exclude_flags |= 0x10;
    view_id = view->GetID();
    if (view_id - 1 <= 1U) exclude_flags |= 2;
    if (view_mode - 3 <= 1U) exclude_flags |= 1;
    if (view_id - 0x10 <= 0x5U) exclude_flags |= 0x100;

    if (!GRaceStatus::fObj->Exists() || GRaceStatus::fObj->Get().GetPlayMode() == 1) exclude_flags |= 4;
    if (exclude_flags & 0x1800) exclude_flags |= 0x40;
    if (exclude_flags & 0x1000) exclude_flags |= 2;
    if (view_id == 3) exclude_flags |= 0x20;

    SetupSceneryCullInfoPlat(view, info);
    info.ExcludeFlags = exclude_flags;
}


void eSwapDynamicFrameMemory() {}

//UNSOLVED: https://decomp.me/scratch/QlAlG
struct TextureInfo *eGetOtherEcstacyTexture(unsigned int name_hash /* r3 */) {
    if (numOtherTex < 0) return NULL;

    for (unsigned int i = 0; i < numOtherTex; i++) {
        if (OtherEcstacyTextures_name_hash[i] == name_hash) {
            return OtherEcstacyTextures[i];
        }
    }

    return NULL;
}

bool eIsWidescreen() { return false; } // Unlocks fullscreen on GameCube for free?
