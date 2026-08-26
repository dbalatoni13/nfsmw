#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#include "../../../include/common/realmemcard/impl/interfaceimp.h"

namespace RealmcIface {

void MemcardInterfaceImpl::MessageDone(MessageChoices choice) {
    Realmc::UserMessage msg;

    this->mUserChoice = choice;
    msg = Realmc::UMSG_NONE;
    switch (choice) {
    case CHOICE_NONE:
        break;
    case CHOICE_OPTION1:
        msg = Realmc::UMSG_OPTION1;
        break;
    case CHOICE_OPTION2:
        msg = Realmc::UMSG_OPTION2;
        break;
    case CHOICE_OPTION3:
        msg = Realmc::UMSG_OPTION3;
        break;
    case CHOICE_OPTION4:
        msg = Realmc::UMSG_OPTION4;
        break;
    default:
        break;
    }

    this->mIMemcard->SendMessage(msg, 0);
}

MemcardTask MemcardInterfaceImpl::Update(unsigned int elapsedTime) {
    if (this->mAutosaveEnabled) {
        this->_CheckForCardRemoval();
    }

    if (this->mActiveTask == TASK_NONE) {
        return TASK_NONE;
    }

    const Realmc::Message *message = this->mIMemcard->GetMessage(elapsedTime);

    if (message != nullptr) {
        switch (this->mActiveTask) {
        case TASK_CHECKCARD:
            this->_ProcessCheckCard(message);
            break;
        case TASK_SAVECHECK:
        case TASK_SAVE:
            this->_ProcessSave(message);
            break;
        case TASK_LOAD:
            this->_ProcessLoad(message);
            break;
        case TASK_DELETE:
            this->_ProcessDelete(message);
            break;
        case TASK_FINDENTRIES:
            this->_ProcessFindEntries(message);
            break;
        case TASK_BOOTUPCHECK:
            this->_ProcessBootupCheck(message);
            break;
        case TASK_SETAUTOSAVE:
            this->_ProcessSetAutosave(message);
            break;
        default:
            break;
        }
    }

    if (this->mMonitorState == MONITOR_ON && this->mActiveTask == TASK_CHECKCARD) {
        return TASK_MONITOR;
    }

    return this->mActiveTask;
}

void MemcardInterfaceImpl::SetMessage(MessageState state, unsigned int message) {
    if (state == MESSAGE_HIDE) {
        this->mHiddenMessages |= message;
        this->mForceMessages &= ~message;
    } else if (state == MESSAGE_FORCE) {
        this->mForceMessages |= message;
        this->mHiddenMessages &= ~message;
    } else {
        this->mHiddenMessages &= ~message;
        this->mForceMessages &= ~message;
    }
}

void MemcardInterfaceImpl::_ShowGuidelinesMessage(const Realmc::Message::DetailInfo::Trc *message) {
    const unsigned short *options[4];

    this->mUserChoice = CHOICE_NONE;
    {
        unsigned int iOption;

        for (iOption = 0; iOption < message->mNumOptions; iOption++) {
            options[iOption] = message->mOptions[iOption].mMsg;
        }
    }

    this->_ClearMessage();
    this->mIGame->ShowMessage(message->mMsg, message->mNumOptions, options);
    this->mMessageShowing = true;
}

void MemcardInterfaceImpl::_ClearMessage() {
    if (this->mMessageShowing) {
        this->mIGame->ClearMessage();
        this->mMessageShowing = false;
    }

    this->mIMemcard->SendMessage(Realmc::UMSG_NONE, 0);
}

RealmcIface::TaskResult MemcardInterfaceImpl::_TranslateTaskResult(Realmc::TaskResult result) {
    RealmcIface::TaskResult taskResult = RESULT_UNKNOWN;

    switch (result) {
    case Realmc::RESULT_SUCCESS:
        taskResult = RESULT_SUCCESS;
        break;
    case Realmc::RESULT_FAILED:
        taskResult = RESULT_FAILED;
        break;
    case Realmc::RESULT_CANCELLED:
        taskResult = RESULT_CANCELLED;
        break;
    case Realmc::RESULT_RETRY:
        taskResult = RESULT_RETRY;
        break;
    default:
        break;
    }

    return taskResult;
}

RealmcIface::CardStatus MemcardInterfaceImpl::_TranslateCardStatus(Realmc::CardStatus status) {
    RealmcIface::CardStatus cardStatus = STATUS_UNKNOWN;

    switch (static_cast<unsigned int>(status)) {
    case Realmc::STATUS_OK:
        cardStatus = STATUS_OK;
        break;
    case Realmc::STATUS_FILE_DELETED:
        cardStatus = STATUS_ENTRY_DELETED;
        break;
    case Realmc::STATUS_NO_CARD:
        cardStatus = STATUS_NO_CARD;
        break;
    case Realmc::STATUS_CARD_CHANGED:
        cardStatus = STATUS_CARD_CHANGED;
        break;
    case Realmc::STATUS_CARD_REMOVED:
        cardStatus = STATUS_CARD_REMOVED;
        break;
    case Realmc::STATUS_CARD_UNFORMATTED:
        cardStatus = STATUS_CARD_UNFORMATTED;
        break;
    case Realmc::STATUS_CARD_DAMAGED:
        cardStatus = STATUS_CARD_DAMAGED;
        break;
    case Realmc::STATUS_WRONG_DEVICE:
        cardStatus = STATUS_WRONG_DEVICE;
        break;
    case Realmc::STATUS_CARD_FULL:
    case Realmc::STATUS_INSUFFICIENT_SPACE:
        cardStatus = STATUS_INSUFFICIENT_SPACE;
        break;
    case Realmc::STATUS_FILE_NOT_FOUND:
    case Realmc::STATUS_ENTRY_NOT_FOUND:
    case Realmc::STATUS_DIRECTORY_NOT_FOUND:
        cardStatus = STATUS_ENTRY_NOT_FOUND;
        break;
    case Realmc::STATUS_FILE_CORRUPTED:
    case Realmc::STATUS_RANGE_ERROR:
        cardStatus = STATUS_ENTRY_CORRUPTED;
        break;
    case Realmc::STATUS_EXIT_TO_CARD_MANAGER:
        cardStatus = STATUS_EXIT_TO_CARD_MANAGER;
        break;
    case Realmc::STATUS_CANNOTMOUNT:
        cardStatus = STATUS_CANNOTMOUNT;
        break;
    case Realmc::STATUS_ACCESS_DENIED:
        cardStatus = STATUS_WRONG_DEVICE;
        break;
    case Realmc::STATUS_ENTRY_ALREADY_EXISTS:
    case Realmc::STATUS_FILE_NOT_OPENED:
    case Realmc::STATUS_DIRECTORY_NOT_EMPTY:
    case Realmc::STATUS_TOO_MANY_OPENED_FILES:
    case Realmc::STATUS_INACCESSIBLE_CARD:
    case Realmc::STATUS_FAILED:
        cardStatus = STATUS_CARD_ERROR;
        break;
    }

    return cardStatus;
}

void MemcardInterfaceImpl::ClearTask() {
    static_cast<Realmc::InterfaceImp *>(this->mIMemcard)->ClearTask();
    this->mActiveTask = TASK_NONE;
    this->mActiveSubtask = SUBTASK_NONE;
}

void MemcardInterfaceImpl::TaskManagerBootupCheck(const BootupCheckParams *params, unsigned int nEntries, const char **entryNames, wchar_t *content) {
    this->mTaskManager->BootupCheck(params, nEntries, entryNames, content);
}

void MemcardInterfaceImpl::TaskManagerFindEntries(const char *entryNamePattern, const TitleInfo *titleInfo) {
    this->mTaskManager->FindEntries(entryNamePattern, titleInfo);
}

void MemcardInterfaceImpl::TaskManagerLoad(const char *entryName, char *header, char *body, const wchar_t *contentName, const wchar_t *typeName, const TitleInfo *titleInfo) {
    this->mTaskManager->Load(entryName, header, body, contentName, typeName, titleInfo);
}

void MemcardInterfaceImpl::TaskManagerSave(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo, const TitleInfo *titleInfo) {
    this->mTaskManager->Save(entryName, header, body, saveInfo, titleInfo);
}

void MemcardInterfaceImpl::TaskManagerDelete(const char *entryName, const wchar_t *contentName) {
    this->mEntryList = &entryName;
    this->mTaskManager->Delete(1, &entryName, contentName);
}

void MemcardInterfaceImpl::TaskManagerCheckCard(CardId cardId) {
    this->mTaskManager->CheckCard(cardId);
}

void MemcardInterfaceImpl::TaskManagerSetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId) {
    this->mTaskManager->SetAutosave(state, nSaveReqs, saveReqs, entryName, cardId);
}

void MemcardInterfaceImpl::TaskManagerSetMonitor(MonitorState state) {
    this->mMonitorState = state;
    this->mTaskManager->SetMonitor(state);
}

} // namespace RealmcIface
