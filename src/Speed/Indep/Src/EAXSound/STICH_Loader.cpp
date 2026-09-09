#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"

int GlobalStichSizes = 0; // Decl: 6

int LoaderSoundStichs(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SND_STICHBUNDLE) {
        bChunk *first_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();

        for (bChunk *chunk = first_chunk; chunk < last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() != BCHUNK_SND_STICHDATA) {
                break;
            } else {
                SND_Stich *NewStich = reinterpret_cast<SND_Stich *>(chunk->GetData());
                bPlatEndianSwap(&NewStich->NameHash);
                bPlatEndianSwap(&NewStich->Volume);
                bPlatEndianSwap(&NewStich->StichIndex);
                bPlatEndianSwap(&NewStich->eStichType);
                bPlatEndianSwap(&NewStich->Num_SampleRefs);
                bPlatEndianSwap(&NewStich->RND_Pitch);
                bPlatEndianSwap(&NewStich->RND_Vol);

                if (g_pEAXSound != nullptr && g_pEAXSound->GetStichPlayer() != nullptr) {
                    g_pEAXSound->GetStichPlayer()->AddStich(static_cast<STICH_TYPE>(NewStich->eStichType), *NewStich);
                }

                chunk = chunk->GetNext();
                NewStich->pSampleRefList = reinterpret_cast<SND_SampleRef *>(chunk->GetData());
                for (int i = 0; i < NewStich->Num_SampleRefs; i++) {
                    GlobalStichSizes += sizeof(SND_SampleRef);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].SampleIndex);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].eStichType);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].Volume);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].Pitch);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].Offset);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].Az);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].RND_Vol);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].RND_Pitch);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].Priority);
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].eRollOffType);
                }
            }
        }
        return 1;
    }
    return 0;
}

int UnloaderSoundStichs(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SND_STICHBUNDLE) {
        if (IsSoundEnabled != 0) {
            g_pEAXSound->GetStichPlayer()->DestroyAllStichs();
        }
        return 1;
    }
    return 0;
}
