#include "./Ecstasy.hpp"

#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Animation/AnimEngineManager.hpp"
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
