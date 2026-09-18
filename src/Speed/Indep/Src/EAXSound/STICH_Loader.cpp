#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_stitch_loop.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int GlobalStichSizes = 0;

cStitchLoop::cStitchLoop(unsigned int attrib)
    : m_StitchAttribKey(attrib) {
    this->m_Stitch[0] = nullptr;
    this->m_Stitch[1] = nullptr;

    Attrib::Gen::aud_stitch_loop loopdata(attrib, 0, nullptr);

    this->m_StichData = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_COLLISION, loopdata.stitch());

    this->m_tOverlap = static_cast<short>(loopdata.overlap());
    this->m_tTimeBeforeRepeat = this->m_tOverlap;

    this->m_Stitch[0] = new cStichWrapper(*this->m_StichData);
    this->m_Stitch[0]->Play(0, 0, 0);
}

cStitchLoop::~cStitchLoop() {
    if (this->m_Stitch[0]) {
        delete this->m_Stitch[0];
    }
    if (this->m_Stitch[1]) {
        delete this->m_Stitch[1];
    }
}

void cStitchLoop::Update(const SND_Params *Params, float dt) {
    {
        for (int n = 0; n < 2; n++) {
            if (this->m_Stitch[n]) {
                this->m_Stitch[n]->Update(Params);
                if (!this->m_Stitch[n]->IsPlaying()) {
                    delete this->m_Stitch[n];
                    this->m_Stitch[n] = nullptr;
                }
            }
        }
    }

    this->m_tTimeBeforeRepeat = static_cast<short>(static_cast<int>(this->m_tTimeBeforeRepeat) - static_cast<int>(dt * 1000.0f));
    if (this->m_tTimeBeforeRepeat < 0) {
        this->m_tTimeBeforeRepeat = this->m_tOverlap;
        int index = -1;
        {
            for (int n = 0; n < 2; n++) {
                if (!this->m_Stitch[n]) {
                    index = n;
                }
            }
        }
        if (index != -1) {
            this->m_Stitch[index] = new cStichWrapper(*this->m_StichData);
            this->m_Stitch[index]->Play(0, 0, 0);
        }
    }
}

int LoaderSoundStichs(bChunk *chunk) {
    if (chunk->GetID() == 0x8003b500) {
        bChunk *first_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();

        for (bChunk *chunk = first_chunk; chunk < last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() != 0x3b502) {
                break;
            }
            {
                SND_Stich *NewStich = reinterpret_cast<SND_Stich *>(chunk->GetData());
                bPlatEndianSwap(&NewStich->NameHash);
                bPlatEndianSwap(&NewStich->Volume);
                bPlatEndianSwap(&NewStich->StichIndex);
                bPlatEndianSwap(reinterpret_cast<signed char *>(&NewStich->eStichType));
                bPlatEndianSwap(&NewStich->Num_SampleRefs);
                bPlatEndianSwap(&NewStich->RND_Pitch);
                bPlatEndianSwap(&NewStich->RND_Vol);

                if (g_pEAXSound && g_pEAXSound->GetStichPlayer()) {
                    g_pEAXSound->GetStichPlayer()->AddStich(static_cast<STICH_TYPE>(NewStich->eStichType), *NewStich);
                }

                chunk = chunk->GetNext();
                NewStich->pSampleRefList = reinterpret_cast<SND_SampleRef *>(chunk->GetData());
                for (int i = 0; i < static_cast<int>(NewStich->Num_SampleRefs); i++) {
                    GlobalStichSizes += 0x10;
                    bPlatEndianSwap(&NewStich->pSampleRefList[i].SampleIndex);
                    bPlatEndianSwap(reinterpret_cast<signed char *>(&NewStich->pSampleRefList[i].eStichType));
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
    if (chunk->GetID() == 0x8003b500) {
        if (IsSoundEnabled) {
            g_pEAXSound->GetStichPlayer()->DestroyAllStichs();
        }
        return 1;
    }
    return 0;
}
