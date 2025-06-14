#include "./Ecstasy.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Animation/AnimEngineManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/World/SimpleModelAnim.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
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
    CurrentBufferEnd = FrameMemoryBufferSize + buffer1;
    CurrentBufferStart = buffer1;
    FrameMemoryBuffer[1] = buffer0;
    FrameMallocAllocNum = 0;
    CurrentBufferPos = buffer1;
    FrameMemoryBuffer[0] = buffer1;
}
