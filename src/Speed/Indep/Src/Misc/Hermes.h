//
//
#ifndef HERMES_H
#define HERMES_H

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTL.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/VecHashMap64.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

namespace Hermes {

// total size: 0x4
// TODO why is this macro expansion wrong?
struct _h_HHANDLER__ {
    int unused; // offset 0x0, size 0x4
};

typedef _h_HHANDLER__ *HHANDLER;

DECLARE_CONTAINER_TYPE(ID_HermesHandlerVector);

// total size: 0x10
// Decl: 107
class Message {
  public:
    Message() {}

    // Decl: 113
    void Send(UCrc32 port) {
        this->mPort = port;
        this->Deliver();
    }

    void Post(UCrc32 port);

    // Decl: 123
    size_t GetSize() const {
        return this->mSize;
    }

    // Decl: 126
    UCrc32 GetKind() const {
        return this->mKind;
    }

    // Decl: 129
    uint32 GetID() const {
        return this->mID;
    }

    // Decl: 132
    Message &SetID(uint32 id) {
        this->mID = id;
        return *this;
    }

  protected:
    // Decl: 135
    Message(UCrc32 kind, size_t size, uint32 id) : mKind(kind), mSize(size), mID(id) {}
    // Decl: 136
    ~Message() {}

  private:
    void Deliver();

    UCrc32 mKind; // offset 0x0, size 0x4, Decl: 143
    UCrc32 mPort; // offset 0x4, size 0x4, Decl: 144
    size_t mSize; // offset 0x8, size 0x4, Decl: 145
    uint32 mID;   // offset 0xC, size 0x4, Decl: 146
};

// Decl: 153
#define DECLARE_MESSAGE(_MSG_)                                                                                                                       \
  public:                                                                                                                                            \
    static uint32_t _GetSize() {                                                                                                                     \
        return sizeof(_MSG_);                                                                                                                        \
    }                                                                                                                                                \
    static UCrc32 _GetKind() {                                                                                                                       \
        static UCrc32 k(#_MSG_);                                                                                                                     \
        return k;                                                                                                                                    \
    }                                                                                                                                                \
    _MSG_() : Message(_GetKind(), _GetSize(), 0) {}

// Decl: 161
#define DECLARE_MESSAGE_SUBCLASS(_MSG_, _BASE_)                                                                                                      \
  public:                                                                                                                                            \
    static uint32_t _GetSize() {                                                                                                                     \
        return sizeof(_MSG_);                                                                                                                        \
    }                                                                                                                                                \
    static UCrc32 _GetKind() {                                                                                                                       \
        static UCrc32 k(#_MSG_);                                                                                                                     \
        return k                                                                                                                                     \
    }                                                                                                                                                \
    _MSG_() : _BASE_(_GetKind(), _GetSize(), 0) {}

// total size: 0x24
// Decl: 175
class Handler {
  public:
    static void Destroy(HHANDLER key);                   // Decl: 178
    static void SetIDFilter(HHANDLER key, bool enabled); // Decl: 179

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
        static void Call(const Message *msg, Handler *handler) {
            StaticHandler<MessageT> *pstatichandler;
        }
    };

    template <typename MessageT> static HHANDLER Create(void (*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        StaticHandler<MessageT> *pstatichandler = reinterpret_cast<StaticHandler<MessageT> *>(&h);

        h.CallFn = pstatichandler->Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(port);
    }

    // Decl: 189
    Handler() {
        bMemSet(this, 0, sizeof(*this));
    }

    // Decl: 197
    template <typename MessageT, typename Class, typename V>
    static HHANDLER Create(Class *that, void (Class::*handler)(const MessageT &), UCrc32 port, unsigned int id) {
        Handler h;
        MemberHandler<MessageT, Class, V> *pmemberhandler = reinterpret_cast<MemberHandler<MessageT, Class, V> *>(&h);
        pmemberhandler->that = that;
        pmemberhandler->Handler = handler;

        h.CallFn = &MemberHandler<MessageT, Class, V>::Call;
        h.mKind = MessageT::_GetKind();
        h.mKey = reinterpret_cast<HHANDLER>(mKeyNext++);
        h.mID = id;

        return h._AddToPort(port);
    }

    // Decl: 203
    Handler(const Handler &src) {
        bMemCpy(this, &src, sizeof(*this));
    }

    // TODO needed?
    friend class PortMessage;

  private:
    void Call(const Message *msg) {
        if (msg->GetKind() == mKind) {
            if (msg->GetID() == this->mID || this->mNoFilter) {
                this->CallFn(msg, this);
            }
        }
    }

    HHANDLER _AddToPort(UCrc32 port);

    uintptr_t Buffer[4];                        // offset 0x0, size 0x10, Decl: 254
    void (*CallFn)(const Message *, Handler *); // offset 0x10, size 0x4, Decl: 255
    UCrc32 mKind;                               // offset 0x14, size 0x4, Decl: 256
    HHANDLER mKey;                              // offset 0x18, size 0x4, Decl: 257
    uint32 mID;                                 // offset 0x1C, size 0x4, Decl: 258
    bool mNoFilter;                             // offset 0x20, size 0x1, Decl: 259

    static uint32 mKeyNext; // Decl: 261
};

// total size: 0x10
class PortMessage {
  public:
    typedef UTL::Std::vector<Handler, _type_vector> Handlers;

    USE_FASTALLOC(PortMessage);

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

// These were restructured moved to the cpp file in Carbon

// total size: 0x10
struct PortKey {
    uint64_t key;    // offset 0x0, size 0x8
    PortMessage *pm; // offset 0x8, size 0x4
};

// total size: 0x20
class System {
  public:
    typedef VecHashMap64<PortMessage, TablePolicy_Fixed<DefaultTableAllocFunc, DefaultTableFreeFunc>, false, 16> PortMessageMap;
    typedef UTL::Std::map<HHANDLER, PortKey, _type_map> PortKeyMap;

    static void Init();
    static void Shutdown();

    static System &Get() {
        return *mObj;
    }

    friend class Message;
    friend class Handler;

  private:
    System() : mPortMessageMap(384) {}
    ~System() {}

    uint64_t CreateKey(UCrc32 port, UCrc32 messageID);
    void AddPortMessage(uint64_t key, PortMessage *pm);
    void RemovePortMessage(uint64_t key);
    PortMessage *FindPortMessage(uint64_t key);
    System::PortKeyMap &GetPortKeyMap();

    static System *mObj; // size: 0x4

    PortMessageMap mPortMessageMap; // offset 0x0, size 0x10
    PortKeyMap mPortKeyMap;         // offset 0x10, size 0x10
};

}; // namespace Hermes

#endif
