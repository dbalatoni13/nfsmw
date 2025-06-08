#include "./Ecstasy.hpp"

#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Animation/AnimEngineManager.hpp"
#include "Speed/Indep/Src/World/SimpleModelAnim.hpp"
#include "./eModel.hpp"
#include "./EcstasyE.hpp"
#include "./Texture.hpp"
#include "./eSolid.hpp"
#include "./eLight.hpp"
#include "./eEnvMapE.hpp"

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
