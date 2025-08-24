#ifndef MISC_HERMES_H
#define MISC_HERMES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace Hermes {

class Message {
    // total size: 0x10
    UCrc32 mKind;       // offset 0x0, size 0x4
    UCrc32 mPort;       // offset 0x4, size 0x4
    unsigned int mSize; // offset 0x8, size 0x4
    unsigned int mID;   // offset 0xC, size 0x4

  public:
    Message() {}

    Message(UCrc32 kind, unsigned int size, unsigned int id) : mKind(kind), mSize(size), mID(id) {}

    ~Message() {}

    void Post(UCrc32 port);
    void Deliver();

    void Send(UCrc32 port) {}

    unsigned int GetSize() const {
        return mSize;
    }

    UCrc32 GetKind() const {
        return mKind;
    }

    unsigned int GetID() const {
        return mID;
    }

    Message &SetID(unsigned int id) {
        mID = id;
        return *this;
    }
};

struct _h_HHANDLER__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef _h_HHANDLER__ *HHANDLER;

struct Handler {
    // total size: 0x24
    unsigned int Buffer[4];                     // offset 0x0, size 0x10
    void (*CallFn)(const Message *, Handler *); // offset 0x10, size 0x4
    UCrc32 mKind;                               // offset 0x14, size 0x4
    HHANDLER mKey;                              // offset 0x18, size 0x4
    unsigned int mID;                           // offset 0x1C, size 0x4
    bool mNoFilter;                             // offset 0x20, size 0x1

    Handler(const Handler &src) {}

    void Call(const Message *msg) {}
};

}; // namespace Hermes

#endif
