#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int GlobalStichSizes = 0;

struct cStitchLoop {
    unsigned int m_StitchAttribKey;
    cStichWrapper *m_Stitch[2];
    SND_Stich *m_StichData;
    short m_tTimeBeforeRepeat;
    short m_tOverlap;

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

    Attrib::Instance loopdata(Attrib::FindCollection(0x3473edcd, attrib), 0, nullptr);
    cSTICH_PlayBack *playback = nullptr;
    if (g_pEAXSound != nullptr) {
        playback = *reinterpret_cast< cSTICH_PlayBack ** >(reinterpret_cast< char * >(g_pEAXSound) + 0x9C);
    }

    const int *stitchIndex = reinterpret_cast< const int * >(loopdata.GetAttributePointer(0x1553cd23, 0));
    if (!stitchIndex) {
        stitchIndex = reinterpret_cast< const int * >(Attrib::DefaultDataArea(sizeof(int)));
    }

    if (playback != nullptr) {
        m_StichData = &playback->GetStich(STICH_TYPE_COLLISION, *stitchIndex);
    }

    const int *overlap = reinterpret_cast< const int * >(loopdata.GetAttributePointer(0xad8c27f5, 0));
    if (!overlap) {
        overlap = reinterpret_cast< const int * >(Attrib::DefaultDataArea(sizeof(int)));
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

void cStitchLoop::Update(const SND_Params *params, float dt) {
    for (int i = 0; i < 2; i++) {
        if (m_Stitch[i] != nullptr) {
            m_Stitch[i]->Update(params);
            if (!m_Stitch[i]->bIsPlaying) {
                delete m_Stitch[i];
                m_Stitch[i] = nullptr;
            }
        }
    }

    m_tTimeBeforeRepeat = static_cast< short >(static_cast< int >(m_tTimeBeforeRepeat) - static_cast< int >(dt * 1000.0f));
    if (m_tTimeBeforeRepeat < 0) {
        m_tTimeBeforeRepeat = m_tOverlap;
        int freeSlot = -1;
        for (int i = 0; i < 2; i++) {
            if (m_Stitch[i] == nullptr) {
                freeSlot = i;
            }
        }
        if (freeSlot != -1 && m_StichData != nullptr) {
            m_Stitch[freeSlot] = new cStichWrapper(*m_StichData);
            m_Stitch[freeSlot]->Play(0, 0, 0);
        }
    }
}

int LoaderSoundStichs(bChunk *chunk) {
    if (chunk->ID != 0x8003b500) {
        return 0;
    }

    bChunk *cur = chunk + 1;
    bChunk *end = reinterpret_cast< bChunk * >(reinterpret_cast< char * >(&chunk[1].ID) + chunk->Size);
    while (cur < end && cur->ID == 0x3b502) {
        SND_Stich *stitch = reinterpret_cast< SND_Stich * >(cur + 1);
        bEndianSwap32(stitch);
        bEndianSwap16(&cur[1].Size);
        bEndianSwap16(reinterpret_cast< char * >(&cur[1].Size) + 2);
        bEndianSwap16(reinterpret_cast< char * >(&cur[2].ID) + 2);
        bEndianSwap16(&cur[2].Size);

        if (g_pEAXSound != nullptr) {
            cSTICH_PlayBack *playback = *reinterpret_cast< cSTICH_PlayBack ** >(reinterpret_cast< char * >(g_pEAXSound) + 0x9C);
            if (playback != nullptr) {
                playback->AddStich(static_cast< STICH_TYPE >(stitch->eStichType), *stitch);
            }
        }

        stitch->pSampleRefList = reinterpret_cast< SND_SampleRef * >(reinterpret_cast< char * >(&cur[2].ID) + cur->Size);
        for (int i = 0; i < static_cast< int >(stitch->Num_SampleRefs); i++) {
            char *sample = reinterpret_cast< char * >(stitch->pSampleRefList + i);
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

    if (IsSoundEnabled && g_pEAXSound != nullptr) {
        cSTICH_PlayBack *playback = *reinterpret_cast< cSTICH_PlayBack ** >(reinterpret_cast< char * >(g_pEAXSound) + 0x9C);
        if (playback != nullptr) {
            playback->DestroyAllStichs();
        }
    }

    return 1;
}
