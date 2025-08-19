#ifndef INPUT_ACTION_REF_H
#define INPUT_ACTION_REF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ActionData.h"

class ActionRef {
    // total size: 0x4
    ActionData *actiondata; // offset 0x0, size 0x4

  public:
    // void Print() const {}

    int ID() const {
        return actiondata->ID();
    }

    float Data() const {
        return actiondata->Data();
    }

    int Slot() const {
        return actiondata->Slot();
    }

    // bool IsNull() const {
    //     return actiondata->IsNull();
    // }
};

#endif
