#ifndef EAGL4ANIM_CSIS_H
#define EAGL4ANIM_CSIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x8
struct CsisEvent {
    short unsigned int mGlobalIndex; // offset 0x0, size 0x2
    short unsigned int mLocalIndex;  // offset 0x2, size 0x2
    float triggerTime;               // offset 0x4, size 0x4
};

struct CsisMetaData {
    int mEventId;                     // offset 0x0, size 0x4
    short unsigned int mNumParams;    // offset 0x4, size 0x2
    short unsigned int mInterfaceCRC; // offset 0x6, size 0x2
    int mParamIdx;                    // offset 0x8, size 0x4
};

// total size: 0x18
struct CsisDictionary {
    // int GetNumCsisMetaData() const {}

    // void SetNumCsisMetaData(int n) {}

    // int GetEventId(short unsigned int index) const {}

    // void SetEventIdByIndex(short unsigned int index, int id) {}

    // int GetEventNumParams(short unsigned int index) const {}

    // short unsigned int GetEventInterfaceCRC(short unsigned int index) const {}

    // int GetEventParamStartValue(short unsigned int index, short unsigned int paramIndex) const {}

    // int GetEventParamNumBits(short unsigned int index, short unsigned int paramIndex) const {}

    // static int ComputeSize(int numChannels, int totalParams) {}

    // Static members
    static const char *Name; // size: 0x4, address: 0xFFFFFFFF

    int mNumCsis;                 // offset 0x0, size 0x4
    int *mParamStartValue;        // offset 0x4, size 0x4
    unsigned char *mParamNumBits; // offset 0x8, size 0x4
    CsisMetaData mMetaData[1];    // offset 0xC, size 0xC
};

#endif
