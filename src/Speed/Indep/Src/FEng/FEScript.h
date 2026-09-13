#ifndef FESCRIPT_H_
#define FESCRIPT_H_

#include "Speed/Indep/Src/FEng/FEEvent.h"
#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

// Decl: 30
enum FEKeyTrack_Indices {
    FETrack_Color = 0,
    FETrack_Pivot = 1,
    FETrack_Position = 2,
    FETrack_Rotation = 3,
    FETrack_Size = 4,
    FETrack_UpperLeft = 5,
    FETrack_LowerRight = 6,
    FETrack_FrameNumber = 7,
    FETrack_Color1 = 7,
    FETrack_Color2 = 8,
    FETrack_Color3 = 9,
    FETrack_Color4 = 10,
    Num_BaseFETracks = 11
};

static const u32 FEHash_Init = FEHASH_INIT; // size: 0x4, Decl: 50

static const u32 SF_ActionMask = 3; // size: 0x4, Decl: 53
static const u32 SF_PlayOnce = 0;   // size: 0x4, Decl: 54
static const u32 SF_PlayLoop = 1;   // size: 0x4, Decl: 55
static const u32 SF_PingPong = 2;   // size: 0x4, Decl: 56

// total size: 0x34
// Decl: 68
class FEScript : public FEMinNode {
  private:
    static ObjectPool<FEScript, 32> NodePool; // size: 0x10, address: 0x80473DFC, Decl: 27

  public:
    i32 Length;          // offset 0xC, size 0x4, Decl: 73
    i32 CurTime;         // offset 0x10, size 0x4, Decl: 74
    u32 Flags;           // offset 0x14, size 0x4, Decl: 75
    FEScript *pChainTo;  // offset 0x18, size 0x4, Decl: 76
    u32 TrackCount;      // offset 0x1C, size 0x4, Decl: 78
    FEKeyTrack *pTracks; // offset 0x20, size 0x4, Decl: 79
    FEEventList Events;  // offset 0x24, size 0x8, Decl: 81
    char *pName;         // offset 0x2C, size 0x4, Decl: 83
    u32 ID;              // offset 0x30, size 0x4, Decl: 84

    FEScript() { // Decl: 87
        Init();
    }
    FEScript(FEScript &Src, bool bReference); // Decl: 87
    ~FEScript() override;

    void Init();

    void operator=(FEScript &Src);

    static void *operator new(size_t size, int) {} // Decl: 95

    static void *operator new(size_t); // Decl: 95

    static void operator delete(void *pNode, int) {} // Decl: 97

    static void operator delete(void *pNode); // Decl: 97

    u32 ComputeSize();

    const char *GetName() const {
        return pName;
    }

    void SetName(const char *pNewName);

    void SetTrackCount(i32 Count);

    void SetPlayAction(u8 ActionType);

    void SetLength(i32 NewLength);

    void GenerateBaseKeyFrom(FEScript *pSrcScript, i32 iSrcTime);

    FEKeyTrack *FindTrack(FEKeyTrack_Indices TrackIndex) const;

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, FEVector2 &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, FEVector3 &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, FEQuaternion &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, FEColor &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, i32 &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, float &Val);

    bool SetKeyTrackValueAt(i32 tTime, u32 TrackIndex, bool &Val);

    void SetBaseTrackValueAt(i32 tTime, u32 TrackIndex, FEVector3 &Val);

    void ScaleScript(i32 NewLength);

    FEScript *GetNext() const {
        return static_cast<FEScript *>(FEMinNode::GetNext());
    }

    FEScript *GetPrev() const { // Decl: 128
        return static_cast<FEScript *>(FEMinNode::GetPrev());
    }
};

#endif
