#include "SpeechManager.hpp"

namespace Speech {

template <typename T>
void Manager::ScheduleSpeech(T &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) {
    ScheduledSpeechEvent *event = ScheduleSpeechPartII(sizeof(T), &data, iid, fh, actor);
}

} // namespace Speech
