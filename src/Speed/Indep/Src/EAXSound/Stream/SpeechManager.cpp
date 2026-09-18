#include "Speed/Indep/Src/EAXSound/CSISAllocator.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"

#define MAX_EVLOG_FILES 99999     // Decl: 84
#define MAX_LEN_EVLOG_FILENAME 20 // Decl: 85

namespace Speech {

ScheduledSpeechEvent *Manager::ScheduleSpeechPartII(unsigned int size, void *data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh,
                                                    EAXCharacter *actor) {
    if ((IsSpeechEnabled == 0) || (m_speechMode == SPEECH_SPLITSCREEN_MODE) || (g_pEAXSound == nullptr) ||
        (g_pEAXSound->GetCurAudioSettings() == nullptr) || (g_pEAXSound->GetCurAudioSettings()->SpeechVol == 0.0f)) {
        unsigned int eventkey = Attrib::StringToLowerCaseKey(iid.pString);
        SPCHType_1_EventID eventID = mHashMap.GetID(eventkey);
        if (eventID == kSPCH1_EventID_CellCall) {
            MNotifyCellCallStarted().Send(UCrc32(UCRC32_Gameplay));
            MNotifyCellCallComplete().Send(UCrc32(UCRC32_Gameplay));
        }
        return nullptr;
    }

    if (gSpeechCache.GetEventPool()->IsFull()) {
        return nullptr;
    }

    unsigned int eventkey = Attrib::StringToLowerCaseKey(iid.pString);
    SPCHType_1_EventID eventID = mHashMap.GetID(eventkey);
    Attrib::Gen::speech event_atr(eventkey, 0, nullptr);
    ScheduledSpeechEvent *event;

    for (int i = 0; i < 4; ++i) {
        for (SchedSpchEvents::iterator iter = mEvents[i].begin(); iter != mEvents[i].end(); ++iter) {
            ScheduledSpeechEvent *this_event = *iter;
            if (this_event->ID == eventID) {
                bool obj_updated = ((actor == nullptr) && (this_event->actor == nullptr)) ||
                                   ((actor != nullptr) && (this_event->actor != nullptr) && (this_event->actor != actor));

                if (obj_updated) {
                    this_event->entry_time = WorldTimer;
                    this_event->actor = actor;
                    if (size != 0) {
                        bMemCpy(this_event->GetData(nullptr), data, size);
                    }
                }
                return nullptr;
            }
        }
    }

    if (!event_atr.DoNotDropout()) {
        bool playback = CanPlayback(event_atr);
        if (!playback) {
            return nullptr;
        }
    }

    if (event_atr.Num_RecallList() != 0) {
        for (unsigned int i = 0; i < event_atr.Num_RecallList(); ++i) {
            Attrib::Gen::speech recall(event_atr.RecallList(i).GetCollectionKey(), 0, nullptr);
            SPCHType_1_EventID recall_id = recall.SpeechID();
            if (IsQueued(recall_id, 4)) {
                RecallSpeechEvent(recall_id);
            }
        }
    }

    event = new (size) ScheduledSpeechEvent();
    if (event == nullptr) {
        return nullptr;
    }

    event->iid = &iid;
    event->fh = &fh;
    event->actor = actor;
    event->ID = eventID;
    event->priority = static_cast<unsigned char>(event_atr.interrupt() ? event_atr.priority() + 100 : event_atr.priority());
    if (event_atr.InitDelay() > 0.0f) {
        event->flags |= 2;
    }
    mEvents[0].push_back(event);
    if (size != 0) {
        bMemCpy(event->GetData(nullptr), data, size);
    }
    return event;
}

}; // namespace Speech
