#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_stitch_loop.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int GlobalStichSizes = 0;

Attrib::Key Attrib::Gen::aud_stitch_loop::ClassKey() {
    return 0x3473edcd;
}

struct cStitchLoop {
    unsigned int m_StitchAttribKey;
    SND_Stich *m_StichData;
    cStichWrapper *m_Stitch[2];
    short m_tOverlap;
    short m_tTimeBeforeRepeat;

    cStitchLoop(unsigned int attrib);
    ~cStitchLoop();
    void Update(const SND_Params *params, float dt);
};

cStitchLoop::cStitchLoop(unsigned int attrib)
    : m_StitchAttribKey(attrib) //
    , m_StichData(nullptr) //
    , m_tTimeBeforeRepeat(0) //
    , m_tOverlap(0) {
    m_Stitch[0] = nullptr;
    m_Stitch[1] = nullptr;

    Attrib::Instance loopdata(Attrib::FindCollection(Attrib::Gen::aud_stitch_loop::ClassKey(), attrib), 0, nullptr);
    cSTICH_PlayBack *playback = nullptr;
    if (g_pEAXSound != nullptr) {
        playback = g_pEAXSound->GetSTICHPlayback();
    }

    const int *stitchIndex = static_cast< const int * >(loopdata.GetAttributePointer(0x1553cd23, 0));
    if (!stitchIndex) {
        stitchIndex = static_cast< const int * >(Attrib::DefaultDataArea(sizeof(int)));
    }

    if (playback != nullptr) {
        m_StichData = &playback->GetStich(STICH_TYPE_COLLISION, *stitchIndex);
    }

    const int *overlap = static_cast< const int * >(loopdata.GetAttributePointer(0xad8c27f5, 0));
    if (!overlap) {
        overlap = static_cast< const int * >(Attrib::DefaultDataArea(sizeof(int)));
    }

    m_tTimeBeforeRepeat = static_cast< short >(*overlap);
    m_tOverlap = m_tTimeBeforeRepeat;

    if (m_StichData != nullptr) {
        m_Stitch[0] = new cStichWrapper(*m_StichData);
        m_Stitch[0]->Play(0, 0, 0);
    }
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

    m_tTimeBeforeRepeat = static_cast< short >(static_cast< int >(m_tTimeBeforeRepeat) - static_cast< int >(dt * 1000.0f));
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

    bChunk *cur = chunk + 1;
    bChunk *end = static_cast<bChunk *>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(&chunk[1].ID)) + chunk->Size));
    while (cur < end && cur->ID == 0x3b502) {
        SND_Stich *stitch = static_cast<SND_Stich *>(static_cast<void *>(cur + 1));
        bEndianSwap32(stitch);
        bEndianSwap16(&cur[1].Size);
        bEndianSwap16(static_cast<char *>(static_cast<void *>(&cur[1].Size)) + 2);
        bEndianSwap16(static_cast<char *>(static_cast<void *>(&cur[2].ID)) + 2);
        bEndianSwap16(&cur[2].Size);

        if (g_pEAXSound != nullptr) {
            cSTICH_PlayBack *playback = g_pEAXSound->GetSTICHPlayback();
            if (playback != nullptr) {
                playback->AddStich(static_cast< STICH_TYPE >(stitch->eStichType), *stitch);
            }
        }

        stitch->pSampleRefList = static_cast<SND_SampleRef *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(&cur[2].ID)) + cur->Size));
        for (int i = 0; i < static_cast< int >(stitch->Num_SampleRefs); i++) {
            char *sample = static_cast<char *>(static_cast<void *>(stitch->pSampleRefList + i));
            GlobalStichSizes += 0x10;
            bEndianSwap16(sample + 2);
            bEndianSwap16(sample + 4);
            bEndianSwap16(sample + 6);
            bEndianSwap16(sample + 8);
            bEndianSwap16(sample + 0xA);
            bEndianSwap16(sample + 0xC);
        }

        cur = cur->GetNext();
    }

    return 1;
}

int UnloaderSoundStichs(bChunk *chunk) {
    if (chunk->ID != 0x8003b500) {
        return 0;
    }

    if (IsSoundEnabled) {
        g_pEAXSound->GetSTICHPlayback()->DestroyAllStichs();
    }

    return 1;
}
