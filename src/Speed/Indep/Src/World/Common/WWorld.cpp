#include "Speed/Indep/Src/World/WWorld.h"

#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

int WWorld::Unloader(bChunk* chunk) {
    if (chunk->GetID() == 0x3B800) {
        fCarpDataSize = 0;
        fCarpData = nullptr;
        return 1;
    }
    return 0;
}

void WWorld::Close() {
    WCollisionAssets::Shutdown();
}

void WSurface::InitSystem() {
}
