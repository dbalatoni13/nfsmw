#ifndef MISC_REPLAY_H
#define MISC_REPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x10
struct ReplaySnapshotSectionInfo {
    const char *Name;           // offset 0x0, size 0x4
    void *Object;               // offset 0x4, size 0x4
    short Number;               // offset 0x8, size 0x2
    char NumMoveData;           // offset 0xA, size 0x1
    char ChecksumErrorDetected; // offset 0xB, size 0x1
    int Size;                   // offset 0xC, size 0x4
};

// total size: 0x964
class ReplaySnapshot : public bTNode<ReplaySnapshot> {
  private:
    char Name[32];                             // offset 0x8, size 0x20
    Timer BeginTime;                           // offset 0x28, size 0x4
    Timer EndTime;                             // offset 0x2C, size 0x4
    int BeginLoopTime;                         // offset 0x30, size 0x4
    int EndLoopTime;                           // offset 0x34, size 0x4
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
    int PrintLoopCounter;                      // offset 0x50, size 0x4
    ReplaySnapshotSectionInfo SectionInfo[16]; // offset 0x54, size 0x100
    float TotalInterestingValue;               // offset 0x154, size 0x4
    int DataPosition;                          // offset 0x158, size 0x4
    int SizeofData;                            // offset 0x15C, size 0x4
    int MaxDataSize;                           // offset 0x160, size 0x4
    unsigned char Data[2048];                  // offset 0x164, size 0x800
};

#endif
