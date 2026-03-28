#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

int PATH_stop(int tracks);
int PATH_shutdown();
int PATH_destroy(int trackhandle);
void PATH_setallocator(EA::Allocator::IAllocator *allocator, const EA::TagValuePair &flags);
void PATH_vectortoreal6();
void PATH_vectortosnd();
void PATH_callbacks(void (*songprogress)(int, int), void (*eventrelease)(void *, PATHEVENTRESULT), void (*eventaction)(int, int, int));
int PATH_createstreamtrack(int track, char *musname, int bytespersec);
IPathTrack *PATH_createstreamimp(int track, int channels, float rate);
IPathTrack *PATH_gettrackimp(int track);
void PATH_clearallevents(int mask);
int PATH_status(int track, PATHSTATUS *status);
int PATH_trackstatus(unsigned int track);
int PATH_addmapfile(char *mapfile);

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
    g_pSFXCTL_Pathfinder = nullptr;
    for (int i = 0; i <= 3; i++) {
        if (m_PFStrmImp[i] != 0) {
            m_PFStrmImp[i] = 0;
        }
    }
}

SFXCTL_Pathfinder::~SFXCTL_Pathfinder() {
    for (int n = 0; n <= 3; ++n) {
        if (m_pPFParms[n]) {
            DestroyTrack(m_pPFParms[n]);
            m_pPFParms[n] = 0;
            for (int i = 0; i <= 3; ++i) {
                if (m_PFStrmImp[i]) {
                    m_PFStrmImp[i] = 0;
                }
            }
        }
    }
}

SndBase *SFXCTL_Pathfinder::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Pathfinder::GetStaticTypeInfo()->typeName, false) SFXCTL_Pathfinder();
    }
    return new (SFXCTL_Pathfinder::GetStaticTypeInfo()->typeName, true) SFXCTL_Pathfinder();
}

SndBase::TypeInfo *SFXCTL_Pathfinder::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Pathfinder::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Pathfinder::InitSFX() {
    EA::TagValuePair tvp;
    tvp.mNext = nullptr;
    tvp.mValue.mInt = 0;
    tvp.mTag = 0;
    PATH_setallocator(&gPF_MemoryAllocator, tvp);
    PATH_vectortoreal6();
    PATH_vectortosnd();
    PATH_callbacks(SongProgressCallback, EventReleaseCallback, EventActionCallback);
    SFXCTL::InitSFX();
}

int SFXCTL_Pathfinder::GetController(int Index) { return -1; }

void SFXCTL_Pathfinder::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Pathfinder::UpdateMixerOutputs() {}

void SFXCTL_Pathfinder::SetupSFX(CSTATE_Base *_StateBase) {
    (void)_StateBase;
    g_pSFXCTL_Pathfinder = this;
}

void SFXCTL_Pathfinder::UpdateParams(float t) {
    (void)t;
    for (int n = 0; n <= 3; ++n) {
        stPFParms *pf = m_pPFParms[n];
        if (!pf) {
            continue;
        }
        if ((pf->procflags & 1) != 0) {
            PATH_status(pf->PATH_TRACK, &pf->pathstatus);
        }
        pf = m_pPFParms[n];
        if ((pf->procflags & 2) != 0) {
            pf->track_status = PATH_trackstatus(pf->PATH_TRACK);
        }
    }
}

int SFXCTL_Pathfinder::CrossMapNodeParam(int pid, int np) {
    (void)pid;

    int nproj = 0;
    int IDHACK;
    switch (m_curinteractive) {
    case 0:
    default:
        break;
    case 1:
        nproj = 1;
        break;
    case 2:
        nproj = 2;
        break;
    case 3:
        nproj = 3;
        break;
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
        if (m_pPFParms[n] != nullptr && projID == static_cast< int >(m_pPFParms[n]->PATH_VOICE) &&
            (m_pPFParms[n]->procflags & 4) != 0) {
            m_pPFParms[n]->curnodeparm = nodeparm;
        }
    }
}

void SFXCTL_Pathfinder::EventActionCallback(const int trackhandle, const int cbID, const int parm) {
    (void)trackhandle;

    switch (cbID) {
        case 1:
        default:
            break;
        case 2:
            MControlPathfinder(false, 0, 0, 0).Send(UCrc32("InteractiveDone"));
            MNotifyMusicFlow(0).Send(UCrc32("InteractiveDone"));
            break;
        case 3:
            if (parm == 0) {
                MControlPathfinder(false, 0x13, 0, 0).Send(UCrc32("Event"));
            } else if (parm == 1) {
                MControlPathfinder(false, 0x14, 0, 0).Send(UCrc32("Event"));
            }
            break;
    }
}

int SFXCTL_Pathfinder::InitPFParms(stPFParms *pstparms, int pathid, int trackid) {
    if (!IsAudioStreamingEnabled) {
        return 0;
    }
    int pathproj = 0x1000000 << pathid;
    int inst;
    int error;
    unsigned int voice = pathproj | 0x10000000u;
    unsigned int track = voice | (1u << trackid);

    m_pPFParms[m_projrefcnt] = pstparms;
    m_pPFParms[m_projrefcnt]->projnum = m_projrefcnt;
    m_pPFParms[m_projrefcnt]->PATH_PROJECT = pathproj;
    m_pPFParms[m_projrefcnt]->PATH_VOICE = voice;
    m_pPFParms[m_projrefcnt]->PATH_TRACK = track;

    error = PATH_addmapfile(m_pPFParms[m_projrefcnt]->pmapfile);
    if (error >= 0) {
        m_pPFParms[m_projrefcnt]->bdataloaded = true;
        g_pSFXCTL_Pathfinder->CreateTrack(m_projrefcnt);
    }
    inst = m_projrefcnt;
    m_projrefcnt = inst + 1;
    return inst;
}

void SFXCTL_Pathfinder::CreateTrack(int index) {
    if (!m_pPFParms[index]->bdataloaded) {
        return;
    }
    char musname[64];
    bStrCat(musname, g_DataPaths[0], m_pPFParms[index]->musfile);
    int result = PATH_createstreamtrack(m_pPFParms[index]->PATH_TRACK, musname, 2500);
    if (result < 0) {
        return;
    }
    m_PFStrmImp[index] = PATH_createstreamimp(m_pPFParms[index]->PATH_TRACK, 3, 2500.0f);
    if (index == 0) {
        AttachStreamInstance(m_pPFParms[0]);
    }
}

int SFXCTL_Pathfinder::GetHandle(int index) {
    if (m_PFStrmImp[index]) {
        return m_PFStrmImp[index]->GetHandle();
    }
    return -1;
}

void SFXCTL_Pathfinder::DetachStreamInstance(stPFParms *pstPFParms) {
    IPathTrack *trackimp = PATH_gettrackimp(m_pPFParms[pstPFParms->projnum]->PATH_TRACK);
    PathTrackSndStream *streamimp;
    char *pbuff = gpEAXS_StrmMgr->GetStreamChannel(1)->GetBuffer();
    int nresult;
    nresult = trackimp->DetachSndStream(&pbuff);
    pstPFParms->bAttached = false;
}

void SFXCTL_Pathfinder::AttachStreamInstance(stPFParms *pstPFParms) {
    IPathTrack *trackimp = PATH_gettrackimp(m_pPFParms[pstPFParms->projnum]->PATH_TRACK);
    PathTrackSndStream *streamimp;
    int mystreamhandle = gpEAXS_StrmMgr->GetStreamChannel(1)->GetStrmHandle();
    char *pbuff = gpEAXS_StrmMgr->GetStreamChannel(1)->GetBuffer();
    if (trackimp != nullptr) {
        int nresult;
        nresult = trackimp->AttachSndStream(mystreamhandle, pbuff);
    }
    pstPFParms->bAttached = true;
    PATH_clearallevents(0x0f000000);
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
