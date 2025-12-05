#ifndef EVENTS_EREQUESTEVENTINFODIALOG_H
#define EVENTS_EREQUESTEVENTINFODIALOG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ERequestEventInfoDialog : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fJoyPort; // offset: 0x4, size 0x4
        GRuntimeInstance * fRaceActivity; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ERequestEventInfoDialog(int pJoyPort, GRuntimeInstance * pRaceActivity);

    override virtual ~ERequestEventInfoDialog();

    override virtual const char *GetEventName() {
        return "ERequestEventInfoDialog";
    }

  private:
    int fJoyPort; // offset: 0x8, size 0x4
    GRuntimeInstance * fRaceActivity; // offset: 0xc, size 0x4
};

void ERequestEventInfoDialog_MakeEvent_Callback(const void *staticData);

#endif
