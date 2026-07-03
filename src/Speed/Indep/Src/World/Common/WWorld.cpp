#include "Speed/Indep/Src/World/WWorld.h"

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"

WWorld::WWorld()
    : fAttributes(0xeec2271a, 0, nullptr), //
      fRootWorldGroup(nullptr),            //
      fCarpData(nullptr),                  //
      fCarpDataSize(0)                     //
{}

void WWorld::Init() {
    if (fgWorld == nullptr) {
        fgWorld = new WWorld();
        SimSurface::InitSystem();
        WSurface::InitSystem();
    }
}

int WWorld::Loader(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_UPPLE_UWORLD) {
        return 0;
    }
    fCarpData = chunk->GetAlignedData(16);
    fCarpDataSize = chunk->GetAlignedSize(16);
    Open();
    return 1;
}

int WWorld::Unloader(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_UPPLE_UWORLD) {
        return 0;
    }
    fCarpData = nullptr;
    fCarpDataSize = 0;
    return 1;
}

int LoaderCarpWGrid(bChunk *chunk) {
    return WWorld::Get().Loader(chunk);
}

int UnloaderCarpWGrid(bChunk *chunk) {
    return WWorld::Get().Unloader(chunk);
}

bChunkLoader bChunkLoaderWGrid(BCHUNK_UPPLE_UWORLD, LoaderCarpWGrid, UnloaderCarpWGrid);

WWorld *WWorld::fgWorld = nullptr;     // size: 0x4, address: 0x80439024
UMath::Vector4 WWorld::mStartPosition; // size: 0x10, address: 0xFFFFFFFF

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

    {
        unsigned int artCount = persistentGroup->GroupCountType('Arti');
        const struct UData *foundSbUData;
        const UGroup *article = fRootWorldGroup->GroupLocateFirst('Arti', 0xFFFFFFFF, 0xFFFFFFFF);

        for (unsigned int artInd = 0; artInd < artCount; artInd++) {
            const UData *sbUData = article->DataLocate(MAKE_UDATA_TYPE('SB'), 'ar');
            article->DataEnd();
            article++;
        }
    }

    return fRootWorldGroup != nullptr;
}

void WWorld::Close() {
    WCollisionAssets::Shutdown();
}
