#ifndef EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H
#define EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

#include "path/path.h"
#include "path/IPathTrack.h"

struct stPFParms {
    int projnum;                     // offset 0x0, size 0x4
    bool bAttached;                  // offset 0x4, size 0x1
    bool bdataloaded;                // offset 0x8, size 0x1
    unsigned int procflags;          // offset 0xC, size 0x4
    int queue_next;                  // offset 0x10, size 0x4
    char *mapfile;                   // offset 0x14, size 0x4
    char *musfile;                   // offset 0x18, size 0x4
    char *ramfile;                   // offset 0x1C, size 0x4
    char *pmapfile;                  // offset 0x20, size 0x4
    int PATH_PROJECT;                // offset 0x24, size 0x4
    int PATH_TRACK;                  // offset 0x28, size 0x4
    int PATH_VOICE;                  // offset 0x2C, size 0x4
    int PATH_TRACKID;                // offset 0x30, size 0x4
    int PATH_TRACK_BYTESPERSEC;      // offset 0x34, size 0x4
    int curnodeparm;                 // offset 0x38, size 0x4
    int track_status;                // offset 0x3C, size 0x4
    PATHSTATUS pathstatus;           // offset 0x40, size 0x38
};

struct SFXCTL_Pathfinder : public SFXCTL {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    static stPFParms *m_pPFParms[4];
    static void SetCurInteractive(int nproj) { m_curinteractive = nproj; }

  private:
    static int m_curinteractive;

  public:
    Path::IPathTrack *m_PFStrmImp[4];    // offset 0x28, size 0x10
    int m_numprojects;             // offset 0x38, size 0x4
    int m_projrefcnt;              // offset 0x3C, size 0x4

    SFXCTL_Pathfinder();
    ~SFXCTL_Pathfinder() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
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
