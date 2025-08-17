#ifndef INPUT_ACTION_REF_H
#define INPUT_ACTION_REF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class ActionData {
    // total size: 0xC
    int id;     // offset 0x0, size 0x4
    int slot;   // offset 0x4, size 0x4
    float data; // offset 0x8, size 0x4

  public:
    // bool IsNull() const {}

    // int Slot() const {}

    // void SetID(int id) {}

    // void SetData(float d) {}

    // void SetSlot(int slot) {}

    // int ID() const {}

    // float Data() const {}
};

class ActionRef {
    // total size: 0x4
    ActionData *actiondata; // offset 0x0, size 0x4

    // void Print() const {}

    // int ID() const {}

    // float Data() const {}

    // int Slot() const {}

    // bool IsNull() const {}
};

#endif
