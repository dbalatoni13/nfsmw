#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/PF_iallocatorimpl.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#include "path/IPathTrack.h"
#include "path/PathToSnd.h"

int DEBUG_PATHFINDER = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 36
int VALIDATE_MUSFILE;     // size: 0x4, address: 0xFFFFFFFF, Decl: 37

PF_Allocator gPF_MemoryAllocator;                 // size: 0x8, address: 0x8045DD70, Decl: 336
stPFParms *SFXCTL_Pathfinder::m_pPFParms[4] = {}; // Decl: 337
SFXCTL_Pathfinder *g_pSFXCTL_Pathfinder;          // size: 0x4, address: 0x8045DDF0, Decl: 338

DEFINE_CREATABLE(0x1010000, SFXCTL_Pathfinder, SFXCTL);

int SFXCTL_Pathfinder::m_curinteractive = 0; // size: 0x4, Decl: 341

SFXCTL_Pathfinder::SFXCTL_Pathfinder() : m_projrefcnt(0) {
    g_pSFXCTL_Pathfinder = nullptr;
    for (int i = 0; i < NUM_ELEMENTS(this->m_PFStrmImp); i++) {
        if (this->m_PFStrmImp[i] != nullptr) {
            this->m_PFStrmImp[i] = nullptr;
        }
    }
}

SFXCTL_Pathfinder::~SFXCTL_Pathfinder() {
    for (int n = 0; n < NUM_ELEMENTS(m_pPFParms); ++n) {
        if (m_pPFParms[n] != nullptr) {
            this->DestroyTrack(m_pPFParms[n]);
            m_pPFParms[n] = nullptr;
            for (int i = 0; i < NUM_ELEMENTS(this->m_PFStrmImp); ++i) {
                if (this->m_PFStrmImp[i] != nullptr) {
                    this->m_PFStrmImp[i] = nullptr;
                }
            }
        }
    }
}

int SFXCTL_Pathfinder::GetController(int Index) {
    return -1;
}

void SFXCTL_Pathfinder::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Pathfinder::UpdateParams(float t) {
    for (int n = 0; n < NUM_ELEMENTS(m_pPFParms); ++n) {
        if (m_pPFParms[n] == nullptr) {
            continue;
        }
        if ((m_pPFParms[n]->procflags & 1) != 0) {
            PATH_status(m_pPFParms[n]->PATH_TRACK, &m_pPFParms[n]->pathstatus);
        }
        if ((m_pPFParms[n]->procflags & 2) != 0) {
            m_pPFParms[n]->track_status = PATH_trackstatus(m_pPFParms[n]->PATH_TRACK);
        }
    }
}

void SFXCTL_Pathfinder::UpdateMixerOutputs() {}

void SFXCTL_Pathfinder::SetupSFX(CSTATE_Base *_StateBase) {
    g_pSFXCTL_Pathfinder = this;
}

int PFXMAP[4][21][2] = {
    {
        {0x00000199, 0x00000000}, {0x0000006A, 0x00000001}, {0x00000132, 0x00000002}, {0x000000D5, 0x00000003}, {0x00000000, 0x00000004},
        {0x00000189, 0x00000005}, {0x0000017F, 0x00000006}, {0x0000018F, 0x00000007}, {0x0000019C, 0x00000008}, {0x02C3FA91, 0x00000009},
        {0x02C0DC6F, 0x0000000A}, {0x02E9222F, 0x0000000B}, {0x02518C15, 0x0000000C}, {0x02B39639, 0x0000000D}, {0x02E2814E, 0x0000000E},
        {0x024C6876, 0x0000000F}, {0x01C53FC7, 0x00000010}, {0x026E7282, 0x00000011}, {0x0290B6DC, 0x00000012}, {0x01A74802, 0x00000013},
        {0x01A0778D, 0x00000014},
    },
    {
        {0x00000136, 0x00000000}, {0x0000006C, 0x00000001}, {0x000000EC, 0x00000002}, {0x000000BA, 0x00000003}, {0x00000000, 0x00000004},
        {0x00000122, 0x00000005}, {0x00000116, 0x00000006}, {0x0000012C, 0x00000007}, {0x00000139, 0x00000008}, {0x02C3FA91, 0x00000009},
        {0x02C0DC6F, 0x0000000A}, {0x02E9222F, 0x0000000B}, {0x02518C15, 0x0000000C}, {0x02B39639, 0x0000000D}, {0x02E2814E, 0x0000000E},
        {0x024C6876, 0x0000000F}, {0x01C53FC7, 0x00000010}, {0x026E7282, 0x00000011}, {0x0290B6DC, 0x00000012}, {0x01452198, 0x00000013},
        {0x01421105, 0x00000014},
    },
    {
        {0x00000163, 0x00000000}, {0x00000049, 0x00000001}, {0x000000EC, 0x00000002}, {0x00000086, 0x00000003}, {0x00000000, 0x00000004},
        {0x0000014F, 0x00000005}, {0x00000143, 0x00000006}, {0x00000159, 0x00000007}, {0x00000166, 0x00000008}, {0x02C3FA91, 0x00000009},
        {0x02C0DC6F, 0x0000000A}, {0x02E9222F, 0x0000000B}, {0x02518C15, 0x0000000C}, {0x02B39639, 0x0000000D}, {0x02E2814E, 0x0000000E},
        {0x024C6876, 0x0000000F}, {0x01C53FC7, 0x00000010}, {0x026E7282, 0x00000011}, {0x0290B6DC, 0x00000012}, {0x01CBFE19, 0x00000013},
        {0x01CCCF04, 0x00000014},
    },
    {
        {0x00000156, 0x00000000}, {0x00000042, 0x00000001}, {0x00000110, 0x00000002}, {0x000000A6, 0x00000003}, {0x00000000, 0x00000004},
        {0x00000142, 0x00000005}, {0x00000138, 0x00000006}, {0x0000014C, 0x00000007}, {0x00000159, 0x00000008}, {0x02C3FA91, 0x00000009},
        {0x02C0DC6F, 0x0000000A}, {0x02E9222F, 0x0000000B}, {0x02518C15, 0x0000000C}, {0x02B39639, 0x0000000D}, {0x02E2814E, 0x0000000E},
        {0x024C6876, 0x0000000F}, {0x01C53FC7, 0x00000010}, {0x026E7282, 0x00000011}, {0x0290B6DC, 0x00000012}, {0x01F0EBB3, 0x00000013},
        {0x01F7DB5C, 0x00000014},
    },
};

int SFXCTL_Pathfinder::CrossMapNodeParam(int pid, int np) {
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

    // TODO the limit is PF_XMAP_MAX
    for (int n = 0; n < 21; ++n) {
        if (PFXMAP[nproj][n][0] == np) {
            return n;
        }
    }
    return -1;
}

void SFXCTL_Pathfinder::SongProgressCallback(int projID, int nodeparm) {
    if (projID == PATH_VOICE_0) {
        MControlPathfinder(false, 0, 0, static_cast<unsigned int>(nodeparm)).Send(UCrc32("PartUpdate"));
    } else {
        int nevt = CrossMapNodeParam(projID, nodeparm);
        if (nevt != -1) {
            MNotifyMusicFlow(nevt).Send(UCrc32("PartUpdate"));
        }
    }

    for (int n = 0; n < NUM_ELEMENTS(m_pPFParms); ++n) {
        if ((m_pPFParms[n] != nullptr) && projID == m_pPFParms[n]->PATH_VOICE && (m_pPFParms[n]->procflags & 4) != 0) {
            m_pPFParms[n]->curnodeparm = nodeparm;
        }
    }
}

void SFXCTL_Pathfinder::EventReleaseCallback(void *eventID, PATHEVENTRESULT result) {}

void SFXCTL_Pathfinder::EventActionCallback(const int trackhandle, const int cbID, const int parm) {
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

void SFXCTL_Pathfinder::InitSFX() {
    PATH_setallocator(&gPF_MemoryAllocator, NULLALLOCTVP);
    PATH_vectortoreal6();
    PATH_vectortosnd();
    PATH_callbacks(SongProgressCallback, EventReleaseCallback, EventActionCallback);
    SFXCTL::InitSFX();
}

// TODO dwarf
int SFXCTL_Pathfinder::InitPFParms(stPFParms *pstparms, int pathid, int trackid) {
    if (IsAudioStreamingEnabled == 0) {
        return 0;
    }
    int pathproj = 0x1000000 << pathid;
    int inst;
    int error;
    unsigned int voice = pathproj | 0x10000000;
    unsigned int track = voice | (1 << trackid);

    m_pPFParms[this->m_projrefcnt] = pstparms;
    m_pPFParms[this->m_projrefcnt]->projnum = this->m_projrefcnt;
    m_pPFParms[this->m_projrefcnt]->PATH_PROJECT = pathproj;
    m_pPFParms[this->m_projrefcnt]->PATH_VOICE = voice;
    m_pPFParms[this->m_projrefcnt]->PATH_TRACK = track;

    error = PATH_addmapfile(m_pPFParms[this->m_projrefcnt]->pmapfile);
    if (error >= 0) {
        m_pPFParms[this->m_projrefcnt]->bdataloaded = true;
        g_pSFXCTL_Pathfinder->CreateTrack(this->m_projrefcnt);
    }
    inst = this->m_projrefcnt;
    this->m_projrefcnt = inst + 1;
    return inst;
}

void SFXCTL_Pathfinder::CreateTrack(int index) {
    if (!m_pPFParms[index]->bdataloaded) {
        return;
    } else {
        char musname[64];
        bStrCat(musname, g_DataPaths[7], m_pPFParms[index]->musfile);
        int result = PATH_createstreamtrack(m_pPFParms[index]->PATH_TRACK, musname, 2500);
        if (result < 0) {
            return;
        }
        this->m_PFStrmImp[index] = PATH_createstreamimp(m_pPFParms[index]->PATH_TRACK, 3, 0.0f);
        if (index == 0) {
            this->AttachStreamInstance(m_pPFParms[0]);
        }
    }
}

int SFXCTL_Pathfinder::GetHandle(int index) {
    if (this->m_PFStrmImp[index] != nullptr) {
        return this->m_PFStrmImp[index]->GetHandle();
    }
    return -1;
}

void SFXCTL_Pathfinder::DetachStreamInstance(stPFParms *pstPFParms) {
    Path::IPathTrack *trackimp = PATH_gettrackimp(m_pPFParms[pstPFParms->projnum]->PATH_TRACK);
    Path::PathTrackSndStream *streamimp;
    char *pbuff = gpEAXS_StrmMgr->GetStreamChannel(1)->GetBuffer();
    int nresult = static_cast<Path::PathTrackSndStream *>(trackimp)->DetachStreamInstance(pbuff);
    pstPFParms->bAttached = false;
}

void SFXCTL_Pathfinder::AttachStreamInstance(stPFParms *pstPFParms) {
    Path::IPathTrack *trackimp = PATH_gettrackimp(m_pPFParms[pstPFParms->projnum]->PATH_TRACK);
    Path::PathTrackSndStream *streamimp;
    int mystreamhandle = gpEAXS_StrmMgr->GetStreamChannel(1)->GetStrmHandle();
    char *pbuff = gpEAXS_StrmMgr->GetStreamChannel(1)->GetBuffer();
    if (trackimp != nullptr) {
        int nresult = static_cast<Path::PathTrackSndStream *>(trackimp)->AttachStreamInstance(mystreamhandle, pbuff);
    }
    pstPFParms->bAttached = true;
    PATH_clearallevents(0x0f000000);
}

void SFXCTL_Pathfinder::DestroyTrack(stPFParms *pstPFParms) {
    if (pstPFParms->bAttached) {
        if (m_pPFParms[pstPFParms->projnum] != nullptr) {
            PATH_stop(pstPFParms->PATH_TRACK);
            this->DetachStreamInstance(pstPFParms);
            PATH_destroy(pstPFParms->PATH_TRACK);
            m_pPFParms[pstPFParms->projnum] = nullptr;
            this->m_projrefcnt--;
        }
    } else {
        if (m_pPFParms[pstPFParms->projnum] != nullptr) {
            PATH_destroy(pstPFParms->PATH_TRACK);
            m_pPFParms[pstPFParms->projnum] = nullptr;
            this->m_projrefcnt--;
        }
    }

    bool bshutdown = true;
    for (int n = 0; n < NUM_ELEMENTS(m_pPFParms); ++n) {
        if (m_pPFParms[n] != nullptr) {
            bshutdown = false;
        }
    }

    if (bshutdown == true) {
        SNDSYS_entercritical();
        PATH_shutdown();
        SNDSYS_leavecritical();
        for (int i = 0; i < NUM_ELEMENTS(this->m_PFStrmImp); ++i) {
            if (this->m_PFStrmImp[i] != nullptr) {
                this->m_PFStrmImp[i] = nullptr;
            }
        }
    }

    if (this->m_projrefcnt < 0) {
        this->m_projrefcnt = 0;
    }
}
