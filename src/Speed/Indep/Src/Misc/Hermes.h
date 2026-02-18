#ifndef MISC_HERMES_H
#define MISC_HERMES_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/VecHashMap64.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

namespace Hermes {

DECLARE_CONTAINER_TYPE(ID_HermesHandlerVector);

// total size: 0x10
class Message {
  public:
    Message() {}

    Message(UCrc32 kind, std::size_t size, unsigned int id) : mKind(kind), mSize(size), mID(id) {}

    ~Message() {}

    void Post(UCrc32 port);
    void Deliver();

    void Send(UCrc32 port) {
        mPort = port;
        Deliver();
    }

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
    template <typename MessageT, typename Class, typename V> struct MemberHandler {
        void (Class::*Handler)(const MessageT &); // offset 0x0, size 0x8
        Class *that;                              // offset 0x8, size 0x4

        static void Call(const Message *msg, Hermes::Handler *handler) {
            MemberHandler<MessageT, Class, V> *pmemberhandler = reinterpret_cast<MemberHandler<MessageT, Class, V> *>(handler);
            (pmemberhandler->that->*(pmemberhandler->Handler))(*static_cast<const MessageT *>(msg));
        }
    };

    template <typename MessageT> struct StaticHandler {
        static void Call(const Message *msg, Hermes::Handler *handler) {
            StaticHandler<MessageT> *pstatichandler;
        }
    };

    template <typename MessageT, typename Class, typename V>
    static HHANDLER Create(Class *that, void (Class::*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        MemberHandler<MessageT, Class, V> *pmemberhandler = reinterpret_cast<MemberHandler<MessageT, Class, V> *>(&h);
        pmemberhandler->Handler = handler;
        pmemberhandler->that = that;

        h.CallFn = pmemberhandler->Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(port);
    }

    template <typename MessageT> static HHANDLER Create(void (*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        StaticHandler<MessageT> *pstatichandler = reinterpret_cast<StaticHandler<MessageT> *>(&h);

        h.CallFn = pstatichandler->Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(port);
    }

    static void Destroy(HHANDLER key);
    static void SetIDFilter(HHANDLER key, bool enabled);

    Handler() {
        bMemSet(this, 0, sizeof(*this));
    }

    Handler(const Handler &src) {
        bMemCpy(this, &src, sizeof(*this));
    }

    // TODO
    friend class PortMessage;

  private:
    HHANDLER _AddToPort(UCrc32 port);

    void Call(const Message *msg) {
        if (msg->GetKind() == mKind) {
            if (msg->GetID() == mID || mNoFilter) {
                CallFn(msg, this);
            }
        }
    }

    static unsigned int mKeyNext;

    uintptr_t Buffer[4];                        // offset 0x0, size 0x10
    void (*CallFn)(const Message *, Handler *); // offset 0x10, size 0x4
    UCrc32 mKind;                               // offset 0x14, size 0x4
    HHANDLER mKey;                              // offset 0x18, size 0x4
    unsigned int mID;                           // offset 0x1C, size 0x4
    bool mNoFilter;                             // offset 0x20, size 0x1
};

class PortMessage {
  public:
    typedef UTL::Std::vector<Hermes::Handler, _type_vector> Handlers;

    // static void *operator new(unsigned int size, void *ptr) {}

    // static void operator delete(void *mem, void *ptr) {}

    // static void *operator new(unsigned int size) {}

    static void operator delete(void *mem, size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr); // TODO
        }
    }

    static void *operator new(size_t size, const char *name) {
        return gFastMem.Alloc(size, nullptr); // TODO
    }

    // static void operator delete(void *mem, const char *name) {}

    // static void operator delete(void *mem, unsigned int size, const char *name) {}

    void RegisterHandler(Handler &handler);
    void UnregisterHandler(HHANDLER key);
    void SetIDFilter(HHANDLER key, bool enabled);
    void HandleMessage(Message *msg);

    PortMessage() {
        mHandlers.reserve(8);
    }

    ~PortMessage() {}

    bool IsEmpty() {
        return mHandlers.size() == 0;
    }

  private:
    Handlers mHandlers; // offset 0x0, size 0x10
};

// total size: 0x10
struct PortKey {
    uint64_t key;    // offset 0x0, size 0x8
    PortMessage *pm; // offset 0x8, size 0x4
};

inline void *DefaultTableAllocFunc(size_t bytes) {
    return AttribAlloc::Allocate(bytes, "TODO");
}

inline void DefaultTableFreeFunc(void *ptr, size_t bytes) {
    AttribAlloc::Free(ptr, bytes, "TODO");
}

// total size: 0x20
class System {
  public:
    typedef VecHashMap64<PortMessage, TablePolicy_Fixed<DefaultTableAllocFunc, DefaultTableFreeFunc>, false, 16> PortMessageMap;
    typedef UTL::Std::map<HHANDLER, PortKey, _type_map> PortKeyMap;

    static void Init();

    static System &Get() {
        return *mObj;
    }

    uint64_t CreateKey(UCrc32 port, UCrc32 messageID);
    void AddPortMessage(uint64_t key, PortMessage *pm);
    void RemovePortMessage(uint64_t key);
    PortMessage *FindPortMessage(uint64_t key);
    System::PortKeyMap &GetPortKeyMap();

  private:
    System() : mPortMessageMap(384) {}

    ~System() {}

    static System *mObj; // size: 0x4

    PortMessageMap mPortMessageMap; // offset 0x0, size 0x10
    PortKeyMap mPortKeyMap;         // offset 0x10, size 0x10
};

}; // namespace Hermes

#endif
