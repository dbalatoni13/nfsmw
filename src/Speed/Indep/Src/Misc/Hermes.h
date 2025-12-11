#ifndef MISC_HERMES_H
#define MISC_HERMES_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace Hermes {

// total size: 0x10
class Message {
  public:
    Message() {}

    Message(UCrc32 kind, std::size_t size, unsigned int id) : mKind(kind), mSize(size), mID(id) {}

    ~Message() {}

    void Post(UCrc32 port);
    void Deliver();

    void Send(UCrc32 port) {}

    std::size_t GetSize() const {
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

  private:
    UCrc32 mKind;      // offset 0x0, size 0x4
    UCrc32 mPort;      // offset 0x4, size 0x4
    std::size_t mSize; // offset 0x8, size 0x4
    unsigned int mID;  // offset 0xC, size 0x4
};

// total size: 0x4
struct _h_HHANDLER__ {
    int unused; // offset 0x0, size 0x4
};

typedef _h_HHANDLER__ *HHANDLER;

// total size: 0x24
class Handler {
  public:
    // total size: 0xC
    template <typename MessageT, typename Class, typename Override> struct MemberHandler {
        void (Class::*Handler)(const MessageT &); // offset 0x0, size 0x8
        Class *that;                              // offset 0x8, size 0x4

        static void Call(const Message *msg, Hermes::Handler *handler) {}
    };

    template <typename MessageT> struct StaticHandler {
        static void Call(const Message *msg, Hermes::Handler *handler) {
            StaticHandler<MessageT> *pstatichandler;
        }
    };

    template <typename MessageT, typename Class, typename Override>
    static HHANDLER Create(Class *that, void (Class::*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        MemberHandler<MessageT, Class, Override> *pmemberhandler = reinterpret_cast<MemberHandler<MessageT, Class, Override> *>(h.Buffer);
        pmemberhandler->Handler = handler;
        pmemberhandler->that = that;

        h.CallFn = pmemberhandler->Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(UCrc32(port));
    }

    template <typename MessageT> static HHANDLER Create(void (*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        StaticHandler<MessageT> *pstatichandler = reinterpret_cast<StaticHandler<MessageT> *>(h.Buffer);

        h.CallFn = pstatichandler->Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(UCrc32(port));
    }

    static void Destroy(HHANDLER key);

    Handler() {
        bMemSet(this, 0, sizeof(*this));
    }

  private:
    HHANDLER _AddToPort(UCrc32 port);

    void Call(const Message *msg) {}

    static unsigned int mKeyNext;

    uintptr_t Buffer[4];                        // offset 0x0, size 0x10
    void (*CallFn)(const Message *, Handler *); // offset 0x10, size 0x4
    UCrc32 mKind;                               // offset 0x14, size 0x4
    HHANDLER mKey;                              // offset 0x18, size 0x4
    unsigned int mID;                           // offset 0x1C, size 0x4
    bool mNoFilter;                             // offset 0x20, size 0x1
};

}; // namespace Hermes

#endif
