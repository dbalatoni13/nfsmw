#include "AnimDirectory.hpp"
#include "AnimCandidates.hpp"
#include "AnimPlayer.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

struct CAnimResourceFileProxy : public bTNode<CAnimResourceFileProxy> {
    enum MemoryPool {
        Main = 0,
        TrackStream = 1,
        CarPool = 2,
    };

    ResourceFile *mResourceFile; // offset 0x8, size 0x4
    void *mMemPointer;           // offset 0xC, size 0x4
    MemoryPool mMemoryPool;      // offset 0x10, size 0x4
};

AnimDirectory *TheAnimDirectory;
AnimSceneLoadInfo gAnimLoader_Info;
bTList<CAnimResourceFileProxy> gAnimLoader_ResourceFileList;

void LoadSceneCandidateData(bChunk *chunk) {
    CAnimSettings::IsDebugPrintEnabled();
    TheAnimCandidateData = reinterpret_cast<CAnimCandidateData *>(chunk->GetData());
    TheAnimCandidateData->EndianSwap();
}

void UnloadSceneCandidateData(bChunk *chunk) {
    CAnimSettings::IsDebugPrintEnabled();
    TheAnimCandidateData = nullptr;
}

void LoadSceneLoadData(bChunk *chunk) {
    CAnimSettings::IsDebugPrintEnabled();
    TheAnimDirectory = reinterpret_cast<AnimDirectory *>(chunk->GetData());
    TheAnimDirectory->EndianSwap();
}

void UnloadSceneLoadData(bChunk *chunk) {
    CAnimSettings::IsDebugPrintEnabled();
    TheAnimDirectory = nullptr;
}

int LoaderAnimDirectoryData(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_ANIM_DIRECTORY) {
        for (bChunk *sub = chunk->GetFirstChunk(), *last = chunk->GetLastChunk(); sub != last; sub = sub->GetNext()) {
            switch (sub->GetID()) {
                case BCHUNK_SCENE_LOAD_DATA:
                    LoadSceneLoadData(sub);
                    break;
                case BCHUNK_SCENE_CANDIDATE_DATA:
                    LoadSceneCandidateData(sub);
                    break;
            }
        }
        return 1;
    } else {
        return 0;
    }
}

int UnloaderAnimDirectoryData(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_ANIM_DIRECTORY) {
        for (bChunk *sub = chunk->GetFirstChunk(), *last = chunk->GetLastChunk(); sub != last; sub = sub->GetNext()) {
            switch (sub->GetID()) {
                case BCHUNK_SCENE_LOAD_DATA:
                    UnloadSceneLoadData(sub);
                    break;
                case BCHUNK_SCENE_CANDIDATE_DATA:
                    UnloadSceneCandidateData(sub);
                    break;
            }
        }
        return 1;
    } else {
        return 0;
    }
}
