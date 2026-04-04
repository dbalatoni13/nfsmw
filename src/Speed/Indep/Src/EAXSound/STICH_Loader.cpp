#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_stitch_loop.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int GlobalStichSizes = 0;

struct cStitchLoop {
    unsigned int m_StitchAttribKey;  // offset 0x0, size 0x4
    const SND_Stich *m_StichData;    // offset 0x4, size 0x4
    cStichWrapper *m_Stitch[2];      // offset 0x8, size 0x8
    short m_tOverlap;                // offset 0x10, size 0x2
    short m_tTimeBeforeRepeat;       // offset 0x12, size 0x2

    cStitchLoop(unsigned int attrib);
    ~cStitchLoop();
    void Update(const SND_Params *params, float dt);
};

cStitchLoop::cStitchLoop(unsigned int attrib)
    : m_StitchAttribKey(attrib) {
    m_Stitch[1] = nullptr;
    m_Stitch[0] = nullptr;

    Attrib::Instance loopdata(Attrib::FindCollection(Attrib::Gen::aud_stitch_loop::ClassKey(), attrib), 0, nullptr);
    cSTICH_PlayBack *playback = g_pEAXSound->GetSTICHPlayback();

    const int *stitchIndex = static_cast<const int *>(loopdata.GetAttributePointer(0x1553cd23, 0));
    if (!stitchIndex) {
        stitchIndex = static_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    m_StichData = &playback->GetStich(STICH_TYPE_COLLISION, *stitchIndex);

    const int *overlap = static_cast<const int *>(loopdata.GetAttributePointer(0xad8c27f5, 0));
    if (!overlap) {
        overlap = static_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    m_tOverlap = static_cast<short>(*overlap);
    m_tTimeBeforeRepeat = m_tOverlap;

    m_Stitch[0] = new cStichWrapper(*m_StichData);
    m_Stitch[0]->Play(0, 0, 0);
}

cStitchLoop::~cStitchLoop() {
    if (m_Stitch[0] != nullptr) {
        delete m_Stitch[0];
    }
    if (m_Stitch[1] != nullptr) {
        delete m_Stitch[1];
    }
}

void cStitchLoop::Update(const SND_Params *Params, float dt) {
    {
        for (int n = 0; n < 2; n++) {
            if (m_Stitch[n] != nullptr) {
                m_Stitch[n]->Update(Params);
                if (!m_Stitch[n]->IsPlaying()) {
                    delete m_Stitch[n];
                    m_Stitch[n] = nullptr;
                }
            }
        }
    }

    m_tTimeBeforeRepeat = static_cast<short>(static_cast<int>(m_tTimeBeforeRepeat) - static_cast<int>(dt * 1000.0f));
    if (m_tTimeBeforeRepeat < 0) {
        m_tTimeBeforeRepeat = m_tOverlap;
        int index = -1;
        {
            for (int n = 0; n < 2; n++) {
                if (m_Stitch[n] == nullptr) {
                    index = n;
                }
            }
        }
        if (index != -1) {
            m_Stitch[index] = new cStichWrapper(*m_StichData);
            m_Stitch[index]->Play(0, 0, 0);
        }
    }
}

int LoaderSoundStichs(bChunk *chunk) {
    if (chunk->ID != 0x8003b500) {
        return 0;
    }

    bChunk *last_chunk = chunk->GetLastChunk();
    chunk = chunk + 1;

    while (chunk < last_chunk) {
        if (chunk->GetID() != 0x3b502) {
            break;
        }
        SND_Stich *NewStich = reinterpret_cast<SND_Stich *>(chunk->GetData());
        bEndianSwap32(NewStich);
        bEndianSwap16(chunk->GetData() + 4);
        bEndianSwap16(chunk->GetData() + 6);
        bEndianSwap16(chunk->GetData() + 0xA);
        bEndianSwap16(chunk->GetData() + 0xC);

        if (g_pEAXSound != nullptr) {
            cSTICH_PlayBack *playback = g_pEAXSound->GetSTICHPlayback();
            if (playback != nullptr) {
                playback->AddStich(static_cast<STICH_TYPE>(NewStich->eStichType), *NewStich);
            }
        }

        bChunk *sampleRefChunk = chunk->GetNext();
        NewStich->pSampleRefList = reinterpret_cast<SND_SampleRef *>(sampleRefChunk->GetData());
        for (int i = 0; i < static_cast<int>(NewStich->Num_SampleRefs); i++) {
            GlobalStichSizes += 0x10;
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 2);
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 4);
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 6);
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 8);
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 0xA);
            bEndianSwap16(i * 16 + reinterpret_cast<char *>(NewStich->pSampleRefList) + 0xC);
        }

        chunk = sampleRefChunk->GetNext();
    }

    return 1;
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

Attrib::Key Attrib::Gen::aud_stitch_loop::ClassKey() {
    return 0x3473edcd;
}
