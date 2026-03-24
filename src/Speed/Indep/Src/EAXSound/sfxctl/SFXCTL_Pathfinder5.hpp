#ifndef EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H
#define EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

enum PATHEVENTRESULT {};
enum PATHTRACKPLAYSTATUS {
    kPATH_TRACK_PLAYSTATUS_NONE = 0,
};

struct PATHSTATUS {
    int playingnode;
    int playingbar;
    int playingbeat;
    int currentnode;
    int currentrequest;
    int readyforrequest;
    unsigned int nodeduration;
    unsigned int barduration;
    unsigned int beatduration;
    unsigned int timeleftinnode;
    unsigned int timeleftinbar;
    unsigned int timeleftinbeat;
    int timebuffered;
    int timeremaining;
};

struct stPFParms {
    int projnum;
    bool bAttached;
    bool bdataloaded;
    unsigned int procflags;
    int queue_next;
    char *mapfile;
    char *musfile;
    char *ramfile;
    char *pmapfile;
    int PATH_PROJECT;
    int PATH_TRACK;
    int PATH_VOICE;
    int PATH_TRACKID;
    int PATH_TRACK_BYTESPERSEC;
    int curnodeparm;
    int track_status;
    PATHSTATUS pathstatus;
};

struct IPathTrack;

struct SFXCTL_Pathfinder : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static stPFParms *m_pPFParms[4];
    static int m_curinteractive;

  public:
    /* 0x28 */ IPathTrack *m_PFStrmImp[4];
    /* 0x38 */ int m_numprojects;
    /* 0x3c */ int m_projrefcnt;

    SFXCTL_Pathfinder();
    ~SFXCTL_Pathfinder() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    int InitPFParms(stPFParms *pstparms, int pathid, int trackid);
    void CreateTrack(int index);
    int GetHandle(int index);
    void DetachStreamInstance(stPFParms *pstPFParms);
    void AttachStreamInstance(stPFParms *pstPFParms);
    void DestroyTrack(stPFParms *pstPFParms);

    static void EventReleaseCallback(void *eventID, PATHEVENTRESULT result);
    static int CrossMapNodeParam(int pid, int np);
    static void SongProgressCallback(int projID, int nodeparm);
    static void EventActionCallback(int trackhandle, int cbID, int parm);
};

extern SFXCTL_Pathfinder *g_pSFXCTL_Pathfinder;

#endif
