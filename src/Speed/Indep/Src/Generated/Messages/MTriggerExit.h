#ifndef GENERATED_MESSAGES_MTRIGGEREXIT_H
#define GENERATED_MESSAGES_MTRIGGEREXIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MTriggerExit : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MTriggerExit);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MTriggerExit");

        return k;
    }

    MTriggerExit(GCollectionKey _Sender, HSIMABLE _Element) : Hermes::Message(_GetKind(), _GetSize(), 0), fSender(_Sender), fElement(_Element) {}

    ~MTriggerExit() {}

    GCollectionKey GetSender() const {
        return fSender;
    }

    void SetSender(GCollectionKey _Sender) {
        fSender = _Sender;
    }

    HSIMABLE GetElement() const {
        return fElement;
    }

    void SetElement(HSIMABLE _Element) {
        fElement = _Element;
    }

  private:
    GCollectionKey fSender; // offset 0x10, size 0x4
    HSIMABLE fElement;      // offset 0x14, size 0x4
};

#endif
