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
    int playingnode;             // offset 0x0, size 0x4
    int playingbar;              // offset 0x4, size 0x4
    int playingbeat;             // offset 0x8, size 0x4
    int currentnode;             // offset 0xC, size 0x4
    int currentrequest;          // offset 0x10, size 0x4
    int readyforrequest;         // offset 0x14, size 0x4
    unsigned int nodeduration;   // offset 0x18, size 0x4
    unsigned int barduration;    // offset 0x1C, size 0x4
    unsigned int beatduration;   // offset 0x20, size 0x4
    unsigned int timeleftinnode; // offset 0x24, size 0x4
    unsigned int timeleftinbar;  // offset 0x28, size 0x4
    unsigned int timeleftinbeat; // offset 0x2C, size 0x4
    int timebuffered;            // offset 0x30, size 0x4
    int timeremaining;           // offset 0x34, size 0x4
};

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

struct PATHTRACKINFO;
struct PATHSUBBANKSTATUS;
struct PathTrackSndStream;

struct IPathTrack {
    int mHandle;                     // offset 0x0, size 0x4
    int mStatusCode;                 // offset 0x4, size 0x4
    int mFXSendLevel;                // offset 0x8, size 0x4
    int mDryLevel;                   // offset 0xC, size 0x4
    int mVolume;                     // offset 0x10, size 0x4
    int mMaxRequests;                // offset 0x14, size 0x4
    int mActiveRequests;             // offset 0x18, size 0x4
    int mUpdateStatus;               // offset 0x1C, size 0x4
    void *mPlayOpts;                 // offset 0x20, size 0x4
    PATHSTATUS mStatus;              // offset 0x24, size 0x38
    PATHTRACKINFO *mTrackInfo;       // offset 0x5C, size 0x4
    int mMaxSubBanks;                // offset 0x60, size 0x4
    PATHSUBBANKSTATUS *mSubBanks;    // offset 0x64, size 0x4
    // vtable at offset 0x68

    // 8 base virtual functions before destructor
    virtual void __base_vfunc0();
    virtual void __base_vfunc1();
    virtual void __base_vfunc2();
    virtual void __base_vfunc3();
    virtual void __base_vfunc4();
    virtual void __base_vfunc5();
    virtual void __base_vfunc6();
    virtual void __base_vfunc7();

    IPathTrack();
    virtual ~IPathTrack();
    virtual int GetNumSubBanks();
    virtual int GetMaxSubBanks();
    virtual PATHSUBBANKSTATUS *GetSubBankPtr(int);
    virtual PATHSUBBANKSTATUS *GetAvailSubBankPtr();
    virtual int AddSubBank(int);
    virtual int AddSubBankDone(int);
    virtual int DetachSubBankHeader(int, int);
    virtual int RemoveSubBank(int);
    virtual void SetTrackInfo(PATHTRACKINFO *info);
    virtual int GetVolume();
    virtual int GetDryLevel();
    virtual int GetFXSendLevel(int bus);
    virtual int GetHandle();
    virtual void *GetPlayOpts();
    virtual void SetFilePath();
    virtual void __unk_vfunc16();
    virtual void __unk_vfunc17();
    virtual void __unk_vfunc18();
    virtual void __unk_vfunc19();
    virtual void __unk_vfunc20();
    virtual void __unk_vfunc21();
    virtual void __unk_vfunc22();
    virtual void __unk_vfunc23();
    virtual void __unk_vfunc24();
    virtual void __unk_vfunc25();
    virtual void __unk_vfunc26();
    virtual int AttachSndStream(int handle, char *buffer);
    virtual int DetachSndStream(char **pbuffer);
};

struct SFXCTL_Pathfinder : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }
    static stPFParms *m_pPFParms[4];
    static int m_curinteractive;

  public:
    IPathTrack *m_PFStrmImp[4];    // offset 0x28, size 0x10
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
