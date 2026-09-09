//
#ifndef SFXCTL_NISREVVING_HPP
#define SFXCTL_NISREVVING_HPP

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

#define MAX_NUM_REV_DATA_POINTS 4096                                             // Decl: 8
#define SIZE_REV_BUFFER (MAX_NUM_REV_DATA_POINTS * (int)sizeof(EngRevDataPoint)) // Decl: 9

// total size: 0xC
// Decl: 12
struct EngRevDataPoint {
    EngRevDataPoint(float _time, int _RPM, int _trq)
        : time(_time), //
          RPM(_RPM),   //
          Trq(_trq) {}

    float time; // offset 0x0, size 0x4, Decl: 29
    int RPM;    // offset 0x4, size 0x4, Decl: 30
    int Trq;    // offset 0x8, size 0x4, Decl: 31
};

// total size: 0x8
// Decl: 35
struct EngRevDataSet {
    // Decl: 36
    EngRevDataSet()
        : NumPoints(0), //
          DataPoints(nullptr) {}

    int NumPoints;               // offset 0x0, size 0x4
    EngRevDataPoint *DataPoints; // offset 0x4, size 0x4
};

// total size: 0x98
// Decl: 48
class NIS_RevManager : public AudioMemBase {
  public:
    NIS_RevManager();
    ~NIS_RevManager() override;

    void OpenNISRevData(uint32 anim_id);
    void StartNISReving();
    void Start321Reving();
    void CloseNIS();
    void Update(float t);
    void StartRecording(uint32 anim_id, float flength);
    void EndRecording(float time);

    int *pRevData;                     // offset 0x4, size 0x4
    EngRevDataSet m_EngineDataSet[16]; // offset 0x8, size 0x80
    bool IsInitialized;                // offset 0x88, size 0x1
    EngRevDataPoint *pBuffer;          // offset 0x8C, size 0x4
    EngRevDataPoint *pCurEntry;        // offset 0x90, size 0x4
    int RecordingCount;                // offset 0x94, size 0x4
};

extern NIS_RevManager *g_pNISRevMgr;

#endif
