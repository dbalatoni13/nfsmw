#include "EDeliverMessage.hpp"

#include "Speed/Indep/Src/Misc/Hermes.h"

EDeliverMessage::EDeliverMessage(Hermes::Message *pMessage, UCrc32 pPort) : Event(0x10), fMessage(EventManager::EmbedField(this, pMessage)), fPort(pPort) {
}

EDeliverMessage::~EDeliverMessage() {
    fMessage->Send(fPort);
}

const char *EDeliverMessage::GetEventName() const {
    return "EDeliverMessage";
}

void EDeliverMessage_MakeEvent_Callback(const void *staticData) {
    new EDeliverMessage(((EDeliverMessage::StaticData *) staticData)->fMessage, ((EDeliverMessage::StaticData *) staticData)->fPort);
}
