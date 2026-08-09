//
#ifndef SFXCTL_PATHFINDER_H
#define SFXCTL_PATHFINDER_H
#define MAX_PF_PROJECTS 4 // Decl: 4

#define PF_STATUS 0x01      // Decl: 6
#define PF_TRACKSTATUS 0x02 // Decl: 7
#define PF_SONGPROG_CB 0x04 // Decl: 8

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

#include "path/path.h"
#include "path/IPathTrack.h"

// total size: 0x78
// Decl: 51
struct stPFParms {
    int projnum;                // offset 0x0, size 0x4, Decl: 52
    bool bAttached;             // offset 0x4, size 0x1, Decl: 53
    bool bdataloaded;           // offset 0x8, size 0x1, Decl: 54
    unsigned int procflags;     // offset 0xC, size 0x4, Decl: 55
    int queue_next;             // offset 0x10, size 0x4, Decl: 57
    char *mapfile;              // offset 0x14, size 0x4, Decl: 60
    char *musfile;              // offset 0x18, size 0x4, Decl: 61
    char *ramfile;              // offset 0x1C, size 0x4, Decl: 62
    char *pmapfile;             // offset 0x20, size 0x4, Decl: 65
    int PATH_PROJECT;           // offset 0x24, size 0x4, Decl: 68
    int PATH_TRACK;             // offset 0x28, size 0x4, Decl: 69
    int PATH_VOICE;             // offset 0x2C, size 0x4, Decl: 70
    int PATH_TRACKID;           // offset 0x30, size 0x4, Decl: 71
    int PATH_TRACK_BYTESPERSEC; // offset 0x34, size 0x4, Decl: 72
    int curnodeparm;            // offset 0x38, size 0x4, Decl: 74
    int track_status;           // offset 0x3C, size 0x4, Decl: 75
    PATHSTATUS pathstatus;      // offset 0x40, size 0x38, Decl: 76
};

// total size: 0x40
// Decl: 86
class SFXCTL_Pathfinder : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_Pathfinder();
    ~SFXCTL_Pathfinder() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateParams(float t) override;
    void UpdateMixerOutputs() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;

    static void SetCurInteractive(int nproj) {
        m_curinteractive = nproj;
    } // Decl: 98

    static int CrossMapNodeParam(int pid, int np);

    void AttachStreamInstance(stPFParms *pstPFParms);
    void DetachStreamInstance(stPFParms *pstPFParms);
    int InitPFParms(stPFParms *pstparms, int pathid, int trackid);
    void DestroyTrack(stPFParms *pstPFParms);
    int GetHandle(int index);

    static stPFParms *m_pPFParms[4]; // size: 0x10, address: 0x80417AC4

    Path::IPathTrack *m_PFStrmImp[4]; // offset 0x28, size 0x10, Decl: 116

  private:
    void UpdatePursuitActivity();
    static void SongProgressCallback(int projID, int nodeparm);
    static void EventReleaseCallback(void *eventID, PATHEVENTRESULT result);
    static void EventActionCallback(const int trackhandle, const int cbID, const int parm);

    void CreateTrack(int index); // TODO order and visibility unknown

  private:
    static int m_curinteractive; // size: 0x4, address: 0x80417AE4
    int m_numprojects;           // offset 0x38, size 0x4, Decl: 122
    int m_projrefcnt;            // offset 0x3C, size 0x4, Decl: 123
};

#endif
