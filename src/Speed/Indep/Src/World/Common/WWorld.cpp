#include "Speed/Indep/Src/World/WWorld.h"

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

int LoaderCarpWGrid(bChunk *chunk);
int UnloaderCarpWGrid(bChunk *chunk);

bChunkLoader bChunkLoaderWGrid(0x3B800, LoaderCarpWGrid, UnloaderCarpWGrid);

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
    fCarpData = nullptr;
    fCarpDataSize = 0;
    return 1;
}

bool WWorld::Open() {
    const void *sources[3];
    int sizes[3];

    sources[2] = nullptr;
    sources[1] = nullptr;
    sizes[2] = 0;
    sizes[1] = 0;
    sources[0] = fCarpData;
    sizes[0] = fCarpDataSize;
    const UGroup *persistentGroup = UGroup::Deserialize(1, reinterpret_cast<const unsigned int *>(sizes), sources, 0);
    CARP::ResolveTagReferences(persistentGroup, 0);
    WCollisionAssets::Init(persistentGroup, nullptr);
    fRootWorldGroup = persistentGroup;

    unsigned int artCount = persistentGroup->GroupCountType(0x41727469);
    const UGroup *article = fRootWorldGroup->GroupLocateFirst(0x41727469, 0xFFFFFFFF, 0xFFFFFFFF);

    for (unsigned int artInd = 0; artInd < artCount; artInd++) {
        article->DataLocateTag(0x53426172);
        article->GetArray();
        article++;
    }

    return fRootWorldGroup != nullptr;
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

WSurface WSurface::kNull;

void WSurface::InitSystem() {
}
