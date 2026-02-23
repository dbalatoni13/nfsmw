#ifndef EAGL4ANIM_CSIS_H
#define EAGL4ANIM_CSIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace EAGL4Anim {

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
    int GetNumCsisMetaData() const {
        return mNumCsis;
    }

    void SetNumCsisMetaData(int n) {
        mNumCsis = n;
    }

    int GetEventId(short unsigned int index) const {
        return mMetaData[index].mEventId;
    }

    void SetEventIdByIndex(short unsigned int index, int id) {
        mMetaData[index].mEventId = id;
    }

    int GetEventNumParams(short unsigned int index) const {
        return mMetaData[index].mNumParams;
    }

    short unsigned int GetEventInterfaceCRC(short unsigned int index) const {
        return mMetaData[index].mInterfaceCRC;
    }

    int GetEventParamStartValue(short unsigned int index, short unsigned int paramIndex) const {
        return mParamStartValue[mMetaData[index].mParamIdx + paramIndex];
    }

    int GetEventParamNumBits(short unsigned int index, short unsigned int paramIndex) const {
        return mParamNumBits[mMetaData[index].mParamIdx + paramIndex];
    }

    // static int ComputeSize(int numChannels, int totalParams) {}

    static const char *Name; // size: 0x4, address: 0xFFFFFFFF

    int mNumCsis;                 // offset 0x0, size 0x4
    int *mParamStartValue;        // offset 0x4, size 0x4
    unsigned char *mParamNumBits; // offset 0x8, size 0x4
    CsisMetaData mMetaData[1];    // offset 0xC, size 0xC
};

// total size: 0x10
class CsisData {
  public:
    unsigned short GetInterfaceCRC() {
        return mInterfaceCRC;
    }

    void SetInterfaceCRC(unsigned short crc) {
        mInterfaceCRC = crc;
    }

    int GetNumParameters() {
        return mNumParams;
    }

    void SetNumParameters(int n) {
        mNumParams = n;
    }

    int GetParameter(int index) {
        return mParams[index];
    }

    void SetParams(int *params) {
        mParams = params;
    }

    void *mUserData; // offset 0x0, size 0x4
  private:
    unsigned short mInterfaceCRC; // offset 0x4, size 0x2
    int mNumParams;               // offset 0x8, size 0x4
    int *mParams;                 // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
