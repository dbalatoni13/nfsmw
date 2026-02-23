#ifndef EAGL4ANIM_CSISEVENTCHANNEL_H
#define EAGL4ANIM_CSISEVENTCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "Csis.h"
#include "EventHandler.h"

namespace EAGL4Anim {

// total size: 0x14
class CsisEventChannel : public AnimMemoryMap {
  public:
    int GetNumEvents() const {
        return mNumEvents;
    }

    void SetNumEvents(int n) {
        mNumEvents = n;
    }

    CsisEvent *GetEvents() {
        return reinterpret_cast<CsisEvent *>(&this[1]);
    }

    CsisDictionary *GetCsisDictionary() {
        return mCsisDict;
    }

    void SetCsisDictionary(CsisDictionary *dict) {
        mCsisDict = dict;
    }

    unsigned short *GetCsisDataIndex() {
        return mCsisDataIndex;
    }

    void SetCsisDataIndex(unsigned short *data) {
        mCsisDataIndex = data;
    }

    unsigned char *GetCsisData() {
        return mCsisData;
    }

    void SetCsisData(unsigned char *data) {
        mCsisData = data;
    }

    int GetSize() const {}

    static int ComputeSize(int numEvents) {}

    void HandleEvents(float currentTime, EventHandler **eventHandlers, void *extraData, CsisEvent &event);

    void Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData);

  private:
    int mNumEvents;                 // offset 0x4, size 0x4
    CsisDictionary *mCsisDict;      // offset 0x8, size 0x4
    unsigned short *mCsisDataIndex; // offset 0xC, size 0x4
    unsigned char *mCsisData;       // offset 0x10, size 0x4
};

}; // namespace EAGL4Anim

#endif
