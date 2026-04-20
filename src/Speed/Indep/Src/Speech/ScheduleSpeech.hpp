#ifndef SPEECH_SCHEDULESPEECH_H
#define SPEECH_SCHEDULESPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"

template <typename T>
void ScheduleSpeech(T &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) {
    Speech::Manager::ScheduleSpeechPartII(sizeof(T), &data, iid, fh, actor);
}

#endif
