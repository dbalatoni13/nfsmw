#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"

namespace RealmcIface {

unsigned int TaskManager::FilterGuidelinesMessage(const Realmc::Message *message) {
    unsigned int messageMask = 0;

    switch (this->mMainTask) {
    case TASK_BOOTUPCHECK:
        messageMask = 0x1000;
        if (message->mCardStatus == Realmc::STATUS_FILE_NOT_FOUND) {
            messageMask = 0x9000;
        }
        if (this->mWarningMsgShown) {
            messageMask |= 0x2000;
        } else if (message->info.trc.mMsgId == Realmc::LMSG_FORMAT_DONE) {
            this->mWarningMsgShown = true;
        }
        break;
    case TASK_NONE:
    case TASK_SAVECHECK:
    case TASK_SAVE:
    case TASK_LOAD:
    case TASK_DELETE:
    case TASK_FINDENTRIES:
    case TASK_SETAUTOSAVE:
    case TASK_MONITOR:
        if (message->info.trc.mMsgId == Realmc::LMSG_FIND_FILE_DONE && this->mLastMessageId == Realmc::LMSG_FIND_FILE_DONE) {
            messageMask |= 2;
        } else if (message->info.trc.mMsgId == Realmc::LMSG_CLOSE_FILE_DONE &&
                   (this->mLastMessageId == Realmc::LMSG_CLOSE_FILE_DONE ||
                    this->mLastMessageId == 0x22)) {
            messageMask |= 4;
        } else if (message->info.trc.mMsgId == Realmc::LMSG_OPEN_FILE_DONE && this->mLastMessageId == Realmc::LMSG_OPEN_FILE_DONE) {
            messageMask |= 4;
        } else if (message->info.trc.mMsgId == Realmc::LMSG_DELETE_FILE_DONE && this->mLastMessageId == Realmc::LMSG_DELETE_FILE_DONE) {
            messageMask |= 4;
        }
        break;
    default:
        break;
    }

    this->mLastMessageId = message->info.trc.mMsgId;
    return messageMask;
}

} // namespace RealmcIface
