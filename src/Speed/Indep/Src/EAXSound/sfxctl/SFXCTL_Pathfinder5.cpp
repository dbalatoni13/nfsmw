#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

int PATH_stop(int tracks);
int PATH_shutdown();
int PATH_destroy(int trackhandle);

SFXCTL_Pathfinder *g_pSFXCTL_Pathfinder;
stPFParms *SFXCTL_Pathfinder::m_pPFParms[4] = {nullptr, nullptr, nullptr, nullptr};
int SFXCTL_Pathfinder::m_curinteractive = 0;

int PFXMAP[4][21][2] = {
    {{409, 0}, {106, 1}, {306, 2}, {213, 3}, {0, 4},   {393, 5},      {383, 6},
     {399, 7}, {412, 8}, {46398097, 9}, {46193775, 10}, {48833071, 11}, {38898709, 12}, {45323833, 13},
     {48398670, 14}, {38561910, 15}, {29704135, 16}, {40792706, 17}, {43038428, 18}, {27740162, 19}, {27293581, 20}},
    {{310, 0}, {108, 1}, {236, 2}, {186, 3}, {0, 4},   {290, 5},      {278, 6},
     {300, 7}, {313, 8}, {46398097, 9}, {46193775, 10}, {48833071, 11}, {38898709, 12}, {45323833, 13},
     {48398670, 14}, {38561910, 15}, {29704135, 16}, {40792706, 17}, {43038428, 18}, {21307800, 19}, {21106949, 20}},
    {{355, 0}, {73, 1},  {236, 2}, {134, 3}, {0, 4},   {335, 5},      {323, 6},
     {345, 7}, {358, 8}, {46398097, 9}, {46193775, 10}, {48833071, 11}, {38898709, 12}, {45323833, 13},
     {48398670, 14}, {38561910, 15}, {29704135, 16}, {40792706, 17}, {43038428, 18}, {30146073, 19}, {30199556, 20}},
    {{342, 0}, {66, 1},  {272, 2}, {166, 3}, {0, 4},   {322, 5},      {312, 6},
     {332, 7}, {345, 8}, {46398097, 9}, {46193775, 10}, {48833071, 11}, {38898709, 12}, {45323833, 13},
     {48398670, 14}, {38561910, 15}, {29704135, 16}, {40792706, 17}, {43038428, 18}, {32566195, 19}, {33020764, 20}},
};

SFXCTL_Pathfinder::SFXCTL_Pathfinder()
    : m_projrefcnt(0) {
    m_PFStrmImp[0] = nullptr;
    m_PFStrmImp[1] = nullptr;
    m_PFStrmImp[2] = nullptr;
    m_PFStrmImp[3] = nullptr;
}

SFXCTL_Pathfinder::~SFXCTL_Pathfinder() {}

SndBase *SFXCTL_Pathfinder::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Pathfinder();
}

SndBase::TypeInfo *SFXCTL_Pathfinder::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Pathfinder::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Pathfinder::InitSFX() { SFXCTL::InitSFX(); }

int SFXCTL_Pathfinder::GetController(int Index) { return -1; }

void SFXCTL_Pathfinder::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Pathfinder::UpdateMixerOutputs() {}

void SFXCTL_Pathfinder::SetupSFX(CSTATE_Base *_StateBase) {
    (void)_StateBase;
    g_pSFXCTL_Pathfinder = this;
}

void SFXCTL_Pathfinder::UpdateParams(float t) {
    (void)t;
    for (int n = 0; n < 4; ++n) {
        stPFParms *pf = m_pPFParms[n];
        if (pf == nullptr) {
            continue;
        }
        if ((pf->procflags & 1) != 0) {
            pf->pathstatus.playingnode = pf->PATH_TRACK;
        }
        if ((pf->procflags & 2) != 0) {
            pf->track_status = kPATH_TRACK_PLAYSTATUS_NONE;
        }
    }
}

int SFXCTL_Pathfinder::CrossMapNodeParam(int pid, int np) {
    (void)pid;

    int nproj = 0;
    if (m_curinteractive == 1) {
        nproj = 1;
    } else if (m_curinteractive <= 1) {
    } else {
        if (m_curinteractive == 2) {
            nproj = 2;
        } else if (m_curinteractive == 3) {
            nproj = 3;
        }
    }

    for (int n = 0; n < 21; ++n) {
        if (PFXMAP[nproj][n][0] == np) {
            return n;
        }
    }
    return -1;
}

void SFXCTL_Pathfinder::SongProgressCallback(int projID, int nodeparm) {
    if (projID == 0x11000000) {
        MControlPathfinder(false, 0, 0, static_cast< unsigned int >(nodeparm)).Send(UCrc32("PartUpdate"));
    } else {
        int nevt = CrossMapNodeParam(projID, nodeparm);
        if (nevt != -1) {
            MNotifyMusicFlow(nevt).Send(UCrc32("PartUpdate"));
        }
    }

    for (int n = 0; n < 4; ++n) {
        stPFParms *pf = m_pPFParms[n];
        if (pf != nullptr && static_cast< int >(pf->PATH_VOICE) == projID && (pf->procflags & 4) != 0) {
            pf->curnodeparm = nodeparm;
        }
    }
}

void SFXCTL_Pathfinder::EventActionCallback(int trackhandle, int cbID, int parm) {
    (void)trackhandle;

    if (cbID == 2) {
        MControlPathfinder(false, 0, 0, 0).Send(UCrc32("InteractiveDone"));
        MNotifyMusicFlow(0).Send(UCrc32("InteractiveDone"));
        return;
    }

    if (cbID == 3) {
        if (parm == 0 || parm == 1 || parm == 2) {
            MControlPathfinder(false, 0x13, static_cast< unsigned int >(parm), 0).Send(UCrc32("Event"));
        }
    }
}

int SFXCTL_Pathfinder::InitPFParms(stPFParms *pstparms, int pathid, int trackid) {
    if (pstparms == nullptr) {
        return -1;
    }
    if (m_projrefcnt >= 4) {
        return -1;
    }

    int slot = m_projrefcnt;
    unsigned int project = 0x1000000u << (pathid & 0x1f);
    unsigned int voice = project | 0x10000000u;
    unsigned int track = voice | (1u << (trackid & 0x1f));

    m_pPFParms[slot] = pstparms;
    pstparms->projnum = slot;
    pstparms->PATH_PROJECT = project;
    pstparms->PATH_VOICE = voice;
    pstparms->PATH_TRACK = track;
    pstparms->bdataloaded = true;
    pstparms->bAttached = false;
    pstparms->track_status = kPATH_TRACK_PLAYSTATUS_NONE;
    ++m_projrefcnt;

    CreateTrack(slot);
    return slot;
}

void SFXCTL_Pathfinder::CreateTrack(int index) {
    if (index < 0 || index >= 4) {
        return;
    }
    stPFParms *pf = m_pPFParms[index];
    if (pf == nullptr || !pf->bdataloaded) {
        return;
    }
    if (index == 0) {
        AttachStreamInstance(pf);
    }
}

int SFXCTL_Pathfinder::GetHandle(int index) {
    if (index < 0 || index >= 4) {
        return -1;
    }
    if (m_PFStrmImp[index] == nullptr) {
        return -1;
    }
    return -1;
}

void SFXCTL_Pathfinder::DetachStreamInstance(stPFParms *pstPFParms) {
    if (pstPFParms == nullptr) {
        return;
    }
    pstPFParms->bAttached = false;
}

void SFXCTL_Pathfinder::AttachStreamInstance(stPFParms *pstPFParms) {
    if (pstPFParms == nullptr) {
        return;
    }
    pstPFParms->bAttached = true;
}

void SFXCTL_Pathfinder::DestroyTrack(stPFParms *pstPFParms) {
    bool bshutdown = true;
    int n;
    int i;

    if (pstPFParms->bAttached) {
        if (m_pPFParms[pstPFParms->projnum]) {
            PATH_stop(pstPFParms->PATH_TRACK);
            DetachStreamInstance(pstPFParms);
            PATH_destroy(pstPFParms->PATH_TRACK);
            m_pPFParms[pstPFParms->projnum] = 0;
            m_projrefcnt -= 1;
        }
    } else {
        if (m_pPFParms[pstPFParms->projnum]) {
            PATH_destroy(pstPFParms->PATH_TRACK);
            m_pPFParms[pstPFParms->projnum] = 0;
            m_projrefcnt -= 1;
        }
    }

    for (n = 0; n < 4; ++n) {
        if (m_pPFParms[n]) {
            bshutdown = false;
        }
    }

    if (bshutdown == true) {
        SNDSYS_entercritical();
        PATH_shutdown();
        SNDSYS_leavecritical();
        for (i = 0; i < 4; ++i) {
            if (m_PFStrmImp[i]) {
                m_PFStrmImp[i] = 0;
            }
        }
    }

    if (m_projrefcnt < 0) {
        m_projrefcnt = 0;
    }
}

void SFXCTL_Pathfinder::EventReleaseCallback(void *eventID, PATHEVENTRESULT result) {}
