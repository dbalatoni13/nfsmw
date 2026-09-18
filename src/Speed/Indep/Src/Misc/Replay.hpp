#ifndef MISC_REPLAY_H
#define MISC_REPLAY_H

#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x10
struct ReplaySnapshotSectionInfo {
    const char *Name;           // offset 0x0, size 0x4
    void *Object;               // offset 0x4, size 0x4
    int16 Number;               // offset 0x8, size 0x2
    int8 NumMoveData;           // offset 0xA, size 0x1
    int8 ChecksumErrorDetected; // offset 0xB, size 0x1
    int Size;                   // offset 0xC, size 0x4
};

// total size: 0x964
class ReplaySnapshot : public bTNode<ReplaySnapshot> {
  public:
    ReplaySnapshot() {}

    ~ReplaySnapshot() {}

    char *GetName();

    Timer &GetBeginTime();

    Timer &GetEndTime();

    int32 GetBeginLoopTime();

    int32 GetEndLoopTime();

    void SetEndTime(Timer t, int time) {}

    void SetBeginTime(Timer t, int time) {}

    void SetMemoryImageLoaded() {}

    bool IsMemoryImageLoaded();

    int GetTotalSize();

    int GetDataSize();

    int GetMaxDataSize();

    void BeginTakingSnapshot() {}

    void EndTakingSnapshot() {}

    void BeginRestoringSnapshot() {}

    void EndRestoringSnapshot() {}

    void BeginTakingChecksum() {}

    void EndTakingChecksum() {}

    void BeginVerifyingChecksum() {}

    void EndVerifyingChecksum() {}

    int IsTakingSnapshot();

    int IsRestoringSnapshot();

    int IsTakingChecksum();

    int IsVerifyingChecksum();

    int DoWeWantToClearCalculatedVariables();

    int IsChecksumSnapshot();

#ifdef EA_BUILD_124
    uint32 GetChecksum();
#endif

    void BeginSection(const char *name) {}

    void BeginSection(void *object, const char *name) {}

    void EndSection() {}

    void BeginNamedSection(const char *name) {}

    void EndNamedSection() {}

    void EnablePrinting(int world_loop_counter) {}

#ifdef EA_BUILD_A124
    void VerifyMoveDataInSync();
#endif

    void MoveDataBuffer(void *pdata, int data_size, int memory_size, int sign_extend) {}

    void VerifyDataBuffer(void *pdata, int data_size, int memory_size, int sign_extend) {}

    void MoveData(unsigned char *pdata, int data_size) {}

    void MoveData(signed char *pdata, int data_size) {}

    void MoveData(bool *pdata, int data_size) {}

    void MoveData(char *pdata, int data_size) {}

    void MoveData(unsigned short *pdata, int data_size) {}

    void MoveData(short *pdata, int data_size) {}

    void MoveData(unsigned int *pdata, int data_size) {}

    void MoveData(int *pdata, int data_size) {}

    void MoveData(float *pdata) {}

    void MoveData(Timer *pdata) {}

    void MoveData(bVector2 *pdata) {}

    void MoveData(bVector3 *pdata) {}

    void MoveData(bVector4 *pdata) {}

    void MoveData(bMatrix4 *pdata) {}

    void MoveDataEnum(void *pdata, int data_size) {}

    void VerifyData(unsigned char *pdata, int data_size) {}

    void VerifyData(signed char *pdata, int data_size) {}

    void VerifyData(bool *pdata, int data_size) {}

    void VerifyData(char *pdata, int data_size) {}

    void VerifyData(unsigned short *pdata, int data_size) {}

    void VerifyData(short *pdata, int data_size) {}

    void VerifyData(unsigned int *pdata, int data_size) {}

    void VerifyData(int *pdata, int data_size) {}

    void VerifyData(float *pdata) {}

    void VerifyData(Timer *pdata) {}

    void VerifyData(bVector2 *pdata) {}

    void VerifyData(bVector3 *pdata) {}

    void VerifyData(bVector4 *pdata) {}

    void VerifyDataEnum(void *pdata, int data_size) {}

    int HasOverflowed();

    void RegisterInterestingValue(float interesting_value) {}

    float GetInterestingValue();

  private:
#ifdef EA_BUILD_A124
    // uint32 GetChecksum();
    void WriteData();
    void ReadData();
    int PrintSectionName();
#endif

    char Name[32];                             // offset 0x8, size 0x20
    Timer BeginTime;                           // offset 0x28, size 0x4
    Timer EndTime;                             // offset 0x2C, size 0x4
    int32 BeginLoopTime;                       // offset 0x30, size 0x4
    int32 EndLoopTime;                         // offset 0x34, size 0x4
    int MemoryImageLoadedFlag;                 // offset 0x38, size 0x4
    char TakingSnapshotFlag;                   // offset 0x3C, size 0x1
    char RestoringSnapshotFlag;                // offset 0x3D, size 0x1
    char TakingChecksumFlag;                   // offset 0x3E, size 0x1
    char VerifyingChecksumFlag;                // offset 0x3F, size 0x1
    char CurrentSectionDepth;                  // offset 0x40, size 0x1
    char OverflowedFlag;                       // offset 0x41, size 0x1
    short CurrentSectionNumber;                // offset 0x42, size 0x2
    int MaxSectionSize;                        // offset 0x44, size 0x4
    char *pMaxSectionName;                     // offset 0x48, size 0x4
    int PrintDepth;                            // offset 0x4C, size 0x4
    int32 PrintLoopCounter;                    // offset 0x50, size 0x4
    ReplaySnapshotSectionInfo SectionInfo[16]; // offset 0x54, size 0x100
    float TotalInterestingValue;               // offset 0x154, size 0x4
    int DataPosition;                          // offset 0x158, size 0x4

  protected:
    int SizeofData;           // offset 0x15C, size 0x4
    int MaxDataSize;          // offset 0x160, size 0x4
    unsigned char Data[2048]; // offset 0x164, size 0x800
};

#endif
