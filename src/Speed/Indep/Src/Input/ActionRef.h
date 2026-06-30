#ifndef INPUT_ACTION_REF_H
#define INPUT_ACTION_REF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ActionData.h"
#include "types.h"

// total size: 0x4
class ActionRef {
public:
    ActionRef(ActionData *p);

    int ID() const {
        return actiondata != nullptr ? actiondata->ID() : 0;
    }

    float Data() const {
        return actiondata != nullptr ? actiondata->Data() : 0.0f;
    }

    int Slot() const {
        return actiondata != nullptr ? actiondata->Slot() : 0;
    }

    bool IsNull() const {
        return actiondata != nullptr;
    }

    // void Print() const {}

  private:
    ActionData *actiondata; // offset 0x0, size 0x4
};

#endif
