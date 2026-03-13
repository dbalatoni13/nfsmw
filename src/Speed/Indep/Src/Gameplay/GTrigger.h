#ifndef GAMEPLAY_GTRIGGER_H
#define GAMEPLAY_GTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"

// total size: 0xCC
class GTrigger : public GRuntimeInstance {
  public:
    GTrigger(const unsigned int &triggerKey);

    bool IsEnabled() const {
        return mTriggerEnabled != 0;
    }

    void GetPosition(UMath::Vector3 &pos);

  private:
    char _pad28[0x74 - 0x28];    // pad from GRuntimeInstance to mTriggerEnabled
    unsigned int mTriggerEnabled; // offset 0x74, size 0x4
    char _pad78[0xCC - 0x78];    // pad rest
};

#endif
