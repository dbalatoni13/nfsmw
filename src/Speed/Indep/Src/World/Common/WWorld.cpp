#include "Speed/Indep/Src/World/WWorld.h"

#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

WWorld* WWorld::fgWorld;

WWorld::WWorld()
    : fAttributes(0xeec2271a, 0, nullptr) //
    , fRootWorldGroup(nullptr) //
    , fCarpData(nullptr) //
    , fCarpDataSize(0) //
{
}

void WWorld::Init() {
    if (!fgWorld) {
        fgWorld = new WWorld();
        SimSurface::InitSystem();
        WSurface::InitSystem();
    }
}

int WWorld::Loader(bChunk* chunk) {
    if (chunk->GetID() != 0x3B800) {
        return 0;
    }
    fCarpData = chunk->GetAlignedData(16);
    fCarpDataSize = chunk->GetAlignedSize(16);
    Open();
    return 1;
}

int WWorld::Unloader(bChunk* chunk) {
    if (chunk->GetID() != 0x3B800) {
        return 0;
    }
    fCarpDataSize = 0;
    fCarpData = nullptr;
    return 1;
}

int LoaderCarpWGrid(bChunk* chunk) {
    return WWorld::Get().Loader(chunk);
}

int UnloaderCarpWGrid(bChunk* chunk) {
    return WWorld::Get().Unloader(chunk);
}

void WWorld::Close() {
    WCollisionAssets::Shutdown();
}

void WSurface::InitSystem() {
}
