#define REALMC_GC_MESSAGE_INLINE
#include "Speed/GameCube/bWare/GameCube/SN/include/wchar.h"
#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#undef REALMC_GC_MESSAGE_INLINE
#include "../../../include/common/realmemcard/gc_driver.h"
#include "../../../include/common/realmemcard/memcard_utilities.h"

namespace Realmc {

void GCInterface::UpdateTaskTrcStartGame() {
    if (GCInterface::mMsgTimer.IsExpired() == false) {
        return;
    }
    switch (GCInterface::mTaskTrcStartGame.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcStartGame.SetState(TS_MOUNT, TS_START);
        GCInterface::mTaskTrcStartGame.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcStartGame.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcStartGame.mCardID = GCInterface::mTaskTrcStartGame.mFirstCardChecked;
        GCInterface::mTaskTrcStartGame.mMounted = false;
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcStartGame.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcStartGame.mMounted) {
                GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcStartGame.mCardID);
                GCInterface::mTaskTrcStartGame.mMounted = false;
            }
            GCInterface::mTaskTrcStartGame.SetSubstate(TS_CHECK_MOUNT_RESULT);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcStartGame.mCardID,
                                              GCInterface::mTaskTrcStartGame.GetID(),
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_CHECK_MOUNT_RESULT:
            switch (GCInterface::mTaskTrcMount.mTaskResult) {
        case RESULT_SUCCESS:
            GCInterface::mTaskTrcStartGame.mMounted = true;
            GCInterface::mTaskTrcStartGame.SetState(TS_CHECK_SPACE, TS_START);
            break;
        case RESULT_CANCELLED:
            GCInterface::mTaskTrcStartGame.mTaskResult = GCInterface::mTaskTrcMount.mTaskResult;
            GCInterface::mTaskTrcStartGame.mCardStatus = GCInterface::mTaskTrcMount.mCardStatus;
            GCInterface::mTaskTrcStartGame.SetState(TS_DONE, TS_START);
            break;
        case RESULT_RETRY:
            GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
            break;
        default:
            GCInterface::mTaskTrcStartGame.mMounted = false;
            if (GCInterface::mTaskTrcMount.mCardStatus == STATUS_CARD_CHANGED) {
                GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
            } else {
                GCInterface::mTaskTrcStartGame.mCardStatus = GCInterface::mTaskTrcMount.mCardStatus;
                GCInterface::mTaskTrcStartGame.SetState(TS_CHECK_NEXT_CARD, TS_START);
            }
            break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_SPACE:
        switch (GCInterface::mTaskTrcStartGame.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcCheckSpace.Start(GCInterface::mTaskTrcStartGame.mCardID,
                                                   &GCInterface::mTaskTrcStartGame.mInfo.fileInfo,
                                                   GCInterface::mTaskTrcStartGame.GetID(),
                                                   true);
            GCInterface::mTaskTrcStartGame.SetSubstate(TS_CHECK_SPACE_RESULT);
            GCInterface::mTaskTrcCheckSpace.mParent->StartTask(&GCInterface::mTaskTrcCheckSpace);
            break;
        case TS_CHECK_SPACE_RESULT:
            GCInterface::mTaskTrcStartGame.mCardStatus = GCInterface::mTaskTrcCheckSpace.mCardStatus;
            switch (GCInterface::mTaskTrcCheckSpace.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcStartGame.SetState(TS_CHECK_NEXT_CARD, TS_START);
                break;
            case RESULT_RETRY:
                GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
                break;
            case RESULT_FAILED:
            case RESULT_CANCELLED:
            default:
                GCInterface::mTaskTrcStartGame.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcStartGame.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_NEXT_CARD:
        if (GCInterface::mTaskTrcStartGame.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcStartGame.mCardID);
            GCInterface::mTaskTrcStartGame.mMounted = false;
        }
        GCInterface::mTaskTrcStartGame.mSlotStatus[GCInterface::mTaskTrcStartGame.mCardID.slot] =
            GCInterface::mTaskTrcStartGame.mCardStatus;
        if (GCInterface::mTaskTrcStartGame.mInfo.checkAllSlots &&
            GCInterface::mTaskTrcStartGame.mCardID.slot == 0) {
            GCInterface::mTaskTrcStartGame.mCardID.slot = 1;
            GCInterface::mTaskTrcStartGame.SetState(TS_MOUNT, TS_START);
        } else {
            GCInterface::mTaskTrcStartGame.SetState(TS_DISPLAY_RESULT, TS_START);
        }
        break;
    case TS_DISPLAY_RESULT:
        switch (GCInterface::mTaskTrcStartGame.GetSubstate()) {
        case TS_START: {
            bool foundGoodCard;

            foundGoodCard = false;
            for (int iSlot = GCInterface::mTaskTrcStartGame.mFirstCardChecked.slot;
                 iSlot <= 1 && !foundGoodCard;
                 ++iSlot) {
                if (GCInterface::mTaskTrcStartGame.mSlotStatus[iSlot] == STATUS_OK) {
                    GCInterface::mTaskTrcStartGame.mFirstCardChecked.slot = iSlot;
                    foundGoodCard = true;
                    GCInterface::mTaskTrcStartGame.mCardStatus =
                        GCInterface::mTaskTrcStartGame.mSlotStatus[iSlot];
                }
            }
            if (foundGoodCard) {
                GCInterface::mTaskTrcStartGame.SetState(TS_DONE, TS_START);
                GCInterface::mTaskTrcStartGame.mTaskResult = RESULT_SUCCESS;
                break;
            }
            GCInterface::mTaskTrcStartGame.mCardID = GCInterface::mTaskTrcStartGame.mFirstCardChecked;
            if (GCInterface::mTaskTrcStartGame.mSlotStatus[0] == STATUS_NO_CARD &&
                GCInterface::mTaskTrcStartGame.mSlotStatus[1] != STATUS_NO_CARD &&
                GCInterface::mTaskTrcStartGame.mSlotStatus[1] != STATUS_UNKNOWN) {
                GCInterface::mTaskTrcStartGame.mCardID.slot = 1;
            }
            if (GCInterface::mTaskTrcStartGame.mSlotStatus[GCInterface::mTaskTrcStartGame.mCardID.slot] ==
                STATUS_INSUFFICIENT_SPACE) {
                GCInterface::mTaskTrcStartGame.SetSubstate(TS_INSUFFICIENT_SPACE_ASK_USER);
            } else {
                GCInterface::mTaskShowCardStatusMsg.Start(
                    GCInterface::mTaskTrcStartGame.mCardID,
                    GCInterface::mTaskTrcStartGame.mTaskResult,
                    GCInterface::mTaskTrcStartGame.mCardStatus,
                    GCInterface::mTaskTrcStartGame.GetID(),
                    false);
                GCInterface::mTaskTrcStartGame.SetSubstate(TS_CHECK_RESULT);
                GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                    &GCInterface::mTaskShowCardStatusMsg);
            }
            break;
        }
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_RETRY:
                GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcStartGame.mSlotStatus[GCInterface::mTaskTrcStartGame.mCardID.slot] =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcStartGame.SetSubstate(TS_START);
                break;
            default:
                if (GCInterface::mTaskShowCardStatusMsg.mTaskResult == RESULT_SUCCESS) {
                    GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcStartGame.mCardID);
                    GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
                } else {
                    GCInterface::mTaskTrcStartGame.SetState(TS_DONE, TS_START);
                }
                GCInterface::mTaskTrcStartGame.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcStartGame.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                break;
            }
            break;
        case TS_INSUFFICIENT_SPACE_ASK_USER:
            GCInterface::mUserMsg = UMSG_NONE;
            if (GCInterface::mTaskTrcStartGame.mInfo.fileInfo.usingMultipleSaves) {
                GCInterface::mTaskMsg.LC_msg(
                    0x13,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcStartGame.mCardID.slot,
                    GCInterface::mTaskTrcStartGame.mInfo.fileInfo.gameTitle);
            } else {
                GCInterface::mTaskMsg.LC_msg(
                    0x12,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcStartGame.mCardID.slot,
                    GCInterface::mTaskTrcStartGame.mInfo.fileInfo.gameTitle,
                    GCInterface::mTaskTrcCheckSpace.mBlocksNeeded,
                    GCInterface::mTaskTrcCheckSpace.mFilesNeeded);
            }
            GCInterface::mTaskTrcStartGame.SetSubstate(TS_WAIT_FOR_USER_REPLY);
            break;
        case TS_WAIT_FOR_USER_REPLY:
            if (GCInterface::mUserMsg == UMSG_NONE) {
                if (GCInterface::mpDriver->CardExists(GCInterface::mTaskTrcStartGame.mCardID) == CR_NOCARD) {
                    GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
                }
            } else {
                switch (GCInterface::ConvertUmsgToOption(GCInterface::mUserMsg,
                                                          5,
                                                          GCInterface::mTaskTrcStartGame.GetID())) {
                case IO_RETRY:
                    GCInterface::mTaskTrcStartGame.SetState(TS_START, TS_START);
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcStartGame.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcStartGame.SetState(TS_DONE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        GCInterface::mTaskTrcStartGame.End();
        GCInterface::mTaskMsg.Set(LMSG_TRC_STARTGAME_DONE,
                                   GCInterface::mTaskTrcStartGame.mTaskResult,
                                   GCInterface::mTaskTrcStartGame.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcStartGame.mCardID;
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcCardExists() {
    switch (GCInterface::mTaskTrcCardExists.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcCardExists.mMounted = false;
        GCInterface::mTaskTrcCardExists.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcCardExists.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcCardExists.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        GCInterface::mTaskTrcCardExists.SetState(TS_SHOW_CARD_STATUS_MSG, TS_START);
        GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcCardExists.mCardID, GCInterface::mTaskTrcCardExists.GetID(), false);
        GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
        break;
    case TS_SHOW_CARD_STATUS_MSG:
        GCInterface::mTaskTrcCardExists.SetState(TS_CHECK_RESULT, TS_START);
        GCInterface::mTaskShowCardStatusMsg.Start(GCInterface::mTaskTrcCardExists.mCardID,
                                                  GCInterface::mTaskTrcMount.mTaskResult,
                                                  GCInterface::mTaskTrcMount.mCardStatus,
                                                  GCInterface::mTaskTrcCardExists.GetID(), false);
        GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(&GCInterface::mTaskShowCardStatusMsg);
        break;
    case TS_CHECK_RESULT: {
        GCInterface::mTaskTrcCardExists.mTaskResult = GCInterface::mTaskShowCardStatusMsg.mTaskResult;
        GCInterface::mTaskTrcCardExists.mCardStatus = GCInterface::mTaskShowCardStatusMsg.mCardStatus;
        if (GCInterface::mTaskTrcCardExists.mTaskResult == RESULT_SUCCESS) {
            GCInterface::mTaskTrcCardExists.SetState(TS_DONE, TS_START);
            GCInterface::mTaskTrcCardExists.mMounted = true;
        } else if (GCInterface::mTaskTrcCardExists.mTaskResult == RESULT_FAILED) {
            GCInterface::mTaskTrcMount.mCardStatus = GCInterface::mTaskTrcCardExists.mCardStatus;
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcCardExists.SetState(TS_SHOW_CARD_STATUS_MSG, TS_START);
        } else {
            if (GCInterface::mTaskTrcCardExists.mCardStatus == STATUS_NO_CARD) {
                GCInterface::mTaskTrcCardExists.SetState(TS_CHECK_REMOUNT, TS_START);
            } else {
                GCInterface::mTaskTrcCardExists.SetState(TS_DONE, TS_START);
            }
        }
        break;
    }
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcCardExists.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcCardExists.mMounted) {
                GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcCardExists.mCardID);
                GCInterface::mTaskTrcCardExists.mMounted = false;
            }
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcCardExists.mCardID, GCInterface::mTaskTrcCardExists.GetID(), false);
            GCInterface::mTaskTrcCardExists.SetSubstate(TS_CHECK_CARD);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_CHECK_CARD:
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcCardExists.mTaskResult = RESULT_SUCCESS;
                GCInterface::mTaskTrcCardExists.mMounted = true;
            } else {
                GCInterface::mTaskTrcCardExists.mTaskResult = GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcCardExists.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcCardExists.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcCardExists.mCardID);
            GCInterface::mTaskTrcCardExists.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_CARDEXISTS_DONE,
                                  GCInterface::mTaskTrcCardExists.mTaskResult,
                                  GCInterface::mTaskTrcCardExists.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcCardExists.mCardID;
        GCInterface::mTaskTrcCardExists.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcGetCardInfo() {
    switch (GCInterface::mTaskTrcGetCardInfo.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcGetCardInfo.mMounted = false;
        GCInterface::mTaskTrcGetCardInfo.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcGetCardInfo.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcGetCardInfo.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        GCInterface::mTaskTrcGetCardInfo.SetState(TS_SHOW_CARD_STATUS_MSG, TS_START);
        GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcGetCardInfo.mCardID, GCInterface::mTaskTrcGetCardInfo.GetID(), false);
        GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
        break;
    case TS_SHOW_CARD_STATUS_MSG:
        GCInterface::mTaskTrcGetCardInfo.SetState(TS_CHECK_RESULT, TS_START);
        GCInterface::mTaskShowCardStatusMsg.Start(GCInterface::mTaskTrcGetCardInfo.mCardID,
                                                  GCInterface::mTaskTrcMount.mTaskResult,
                                                  GCInterface::mTaskTrcMount.mCardStatus,
                                                  GCInterface::mTaskTrcGetCardInfo.GetID(), false);
        GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(&GCInterface::mTaskShowCardStatusMsg);
        break;
    case TS_CHECK_RESULT: {
        GCInterface::mTaskTrcGetCardInfo.mTaskResult = GCInterface::mTaskShowCardStatusMsg.mTaskResult;
        GCInterface::mTaskTrcGetCardInfo.mCardStatus = GCInterface::mTaskShowCardStatusMsg.mCardStatus;
        if (GCInterface::mTaskTrcGetCardInfo.mTaskResult == RESULT_SUCCESS) {
            GCInterface::mTaskTrcGetCardInfo.SetState(TS_GET_CARD_INFO, TS_START);
            GCInterface::mTaskTrcGetCardInfo.mMounted = true;
        } else if (GCInterface::mTaskTrcGetCardInfo.mTaskResult == RESULT_FAILED) {
            GCInterface::mTaskTrcMount.mCardStatus = GCInterface::mTaskTrcGetCardInfo.mCardStatus;
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcGetCardInfo.SetState(TS_SHOW_CARD_STATUS_MSG, TS_START);
        } else {
            if (GCInterface::mTaskTrcGetCardInfo.mCardStatus == STATUS_NO_CARD) {
                GCInterface::mTaskTrcGetCardInfo.SetState(TS_CHECK_REMOUNT, TS_START);
            } else {
                GCInterface::mTaskTrcGetCardInfo.SetState(TS_DONE, TS_START);
            }
        }
        break;
    }
    case TS_GET_CARD_INFO:
        GCInterface::mTaskTrcGetCardInfo.mCardStatus = STATUS_FAILED;
        if (GCInterface::mpDriver->GetFreeCardSpace(GCInterface::mTaskTrcGetCardInfo.mCardID,
                                                     &GCInterface::mTaskTrcGetCardInfo.mCardInfo.freeSpace,
                                                     &GCInterface::mTaskTrcGetCardInfo.mCardInfo.freeFiles) == CR_SUCCESS) {
            GCInterface::mTaskTrcGetCardInfo.mCardStatus = STATUS_OK;
        }
        GCInterface::mTaskTrcGetCardInfo.SetState(TS_DONE, TS_START);
        break;
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcGetCardInfo.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcGetCardInfo.mMounted) {
                GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcGetCardInfo.mCardID);
                GCInterface::mTaskTrcGetCardInfo.mMounted = false;
            }
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcGetCardInfo.mCardID, GCInterface::mTaskTrcGetCardInfo.GetID(), false);
            GCInterface::mTaskTrcGetCardInfo.SetSubstate(TS_CHECK_CARD);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_CHECK_CARD:
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcGetCardInfo.mTaskResult = RESULT_SUCCESS;
                GCInterface::mTaskTrcGetCardInfo.mMounted = true;
            } else {
                GCInterface::mTaskTrcGetCardInfo.mTaskResult = GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcGetCardInfo.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcGetCardInfo.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcGetCardInfo.mCardID);
            GCInterface::mTaskTrcGetCardInfo.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_GETCARDINFO_DONE,
                                   GCInterface::mTaskTrcGetCardInfo.mTaskResult,
                                   GCInterface::mTaskTrcGetCardInfo.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcGetCardInfo.mCardInfo.cardID;
        GCInterface::mTaskMsg.info.cardInfo.freeSpace = GCInterface::mTaskTrcGetCardInfo.mCardInfo.freeSpace;
        GCInterface::mTaskMsg.info.cardInfo.freeFiles = GCInterface::mTaskTrcGetCardInfo.mCardInfo.freeFiles;
        GCInterface::mTaskTrcGetCardInfo.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcLoadFile() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcLoadFile.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcLoadFile.SetState(TS_MOUNT, TS_START);
        GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcLoadFile.mMounted = false;
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcLoadFile.mCardID,
                                              TID_NONE,
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_SHOW_CARD_STATUS_MSG:
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcLoadFile.mCardID,
                GCInterface::mTaskTrcMount.mTaskResult,
                GCInterface::mTaskTrcMount.mCardStatus,
                GCInterface::mTaskTrcLoadFile.GetID(),
                false);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcLoadFile.mMounted = true;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_READY, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcMount.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcSaveFile.mMounted = false;
                GCInterface::mTaskTrcLoadFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
                GCInterface::mTaskTrcMount.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                break;
            default:
                GCInterface::mTaskTrcLoadFile.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcLoadFile.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcLoadFile.SetState(TS_DONE, TS_START);
                GCInterface::mTaskTrcLoadFile.mMounted = false;
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_LOAD_READY:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcLoadFile.mCardID) != STATUS_NO_CARD) {
            switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
            case TS_START:
                GCInterface::mUserMsg =
                    static_cast<UserMessage>(static_cast<int>(
                        GCInterface::mTaskTrcLoadFile.GetSubstate()));
                GCInterface::mTaskMsg.LC_msg(0x1e, IO_CONTINUE,
                                             GCInterface::mTaskTrcLoadFile.mCardID.slot);
                GCInterface::mTaskTrcLoadFile.SetSubstate(TS_WAIT_FOR_RESULT);
                break;
            case TS_WAIT_FOR_RESULT:
                if (GCInterface::mUserMsg != UMSG_NONE) {
                    GCInterface::mTaskTrcLoadFile.SetState(TS_OPEN_FILE, TS_START);
                }
                break;
            default:
                break;
            }
        } else {
            GCInterface::mTaskTrcLoadFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
        }
        break;
    case TS_OPEN_FILE:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x1f, IO_NONE,
                                         GCInterface::mTaskTrcLoadFile.mCardID.slot);
            GCInterface::mMsgTimer.Set(1);
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_OPEN_FILE);
            break;
        case TS_OPEN_FILE:
            switch (GCInterface::mpDriver->OpenFile(
                GCInterface::mTaskTrcLoadFile.mCardID,
                &GCInterface::mTaskTrcLoadFile.mFileInfo,
                FOM_READ,
                &GCInterface::mTaskTrcLoadFile.mFileHandle)) {
            case CR_SUCCESS:
                if (GCInterface::mpDriver->GetOpenFileSize(
                        GCInterface::mTaskTrcLoadFile.mFileHandle) == -1) {
                    GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FILE_CORRUPTED;
                    GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                } else {
                    GCInterface::mTaskMsg.Set(LMSG_OPEN_FILE_DONE,
                                               RESULT_SUCCESS,
                                               STATUS_OK);
                    GCInterface::mTaskMsg.info.openResult.fileHandle =
                        GCInterface::mTaskTrcLoadFile.mFileHandle;
                    GCInterface::mTaskMsg.info.openResult.fileName =
                        GCInterface::mTaskTrcLoadFile.mFileInfo.fileName;
                    GCInterface::mTaskTrcLoadFile.SetState(TS_READ_WAIT, TS_START);
                }
                break;
            case CR_NOCARD:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                break;
            case CR_NOFILE:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FILE_NOT_FOUND;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                break;
            default:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FAILED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_READ_WAIT:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.Set(LMSG_CHECK_DATA_INTEGRITY);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_USER_CHECK_DATA_RESULT);
            break;
        case TS_READ_WAIT:
            if (GCInterface::mTaskRead.mTaskResult != RESULT_SUCCESS) {
                GCInterface::mTaskTrcLoadFile.mCardStatus = GCInterface::mTaskRead.mCardStatus;
            } else if (GCInterface::mUserMsg != UMSG_READ_COMPLETE) {
                break;
            }
            switch (GCInterface::mpDriver->CloseFile(
                GCInterface::mTaskTrcLoadFile.mFileHandle)) {
            case CR_SUCCESS:
                GCInterface::mTaskTrcLoadFile.SetState(TS_USER_CHECK_DATA, TS_START);
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_OK;
                break;
            case CR_NOCARD:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                break;
            default:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FAILED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_USER_CHECK_DATA:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.Set(LMSG_CHECK_DATA_INTEGRITY);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_USER_CHECK_DATA_RESULT);
            break;
        case TS_USER_CHECK_DATA_RESULT:
            if (GCInterface::mUserMsg == UMSG_DATA_OK) {
                GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_SUCCESS;
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_OK;
                GCInterface::mTaskTrcLoadFile.SetState(TS_DONE, TS_START);
            } else if (GCInterface::mUserMsg == UMSG_DATA_CORRUPT) {
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FILE_CORRUPTED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_LOAD_FAILED_INFORM_USER, TS_START);
            }
            break;
        default:
            break;
        }
        break;
    case TS_LOAD_FAILED_INFORM_USER:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_FAILED;
            if (GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_FILE_CORRUPTED) {
                GCInterface::mTaskMsg.LC_msg(
                    0x1d,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcLoadFile.mFileInfo.gameTitle,
                    GCInterface::mTaskTrcLoadFile.mFileInfo.fileContentName,
                    GCInterface::mTaskTrcLoadFile.mCardID.slot);
            } else if (GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_FILE_NOT_FOUND) {
                if (GCInterface::mTaskTrcLoadFile.mFileInfo.fileContentName != nullptr) {
                    GCInterface::mTaskMsg.LC_msg(
                        0x21,
                        GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                  IO_CONTINUE | IO_RETRY,
                                                  IO_NONE,
                                                  IO_NONE),
                        GCInterface::mTaskTrcLoadFile.mFileInfo.fileTypeName,
                        GCInterface::mTaskTrcLoadFile.mFileInfo.fileContentName);
                } else {
                    wchar_t unicodeBackSpace[2];

                    RealmcUtils::Ascii2Unicode(unicodeBackSpace, "\b");
                    GCInterface::mTaskMsg.LC_msg(
                        0x21,
                        GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                  IO_CONTINUE | IO_RETRY,
                                                  IO_NONE,
                                                  IO_NONE),
                        unicodeBackSpace,
                        GCInterface::mTaskTrcLoadFile.mFileInfo.fileTypeName);
                }
            } else {
                GCInterface::mTaskMsg.LC_msg(0x20,
                                             GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                       IO_CONTINUE | IO_RETRY,
                                                                       IO_NONE,
                                                                       IO_NONE),
                                             GCInterface::mTaskTrcLoadFile.mCardID.slot);
            }
            GCInterface::mTaskMsg.mTaskResult = GCInterface::mTaskTrcLoadFile.mTaskResult;
            GCInterface::mTaskMsg.mCardStatus = GCInterface::mTaskTrcLoadFile.mCardStatus;
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_LOAD_FAILED_WAIT_FOR_USER_REPLY);
            break;
        case TS_LOAD_FAILED_WAIT_FOR_USER_REPLY:
            if (GCInterface::CheckCard(GCInterface::mTaskTrcLoadFile.mCardID) == STATUS_NO_CARD) {
                if (GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_FILE_CORRUPTED) {
                    GCInterface::mTaskTrcLoadFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
                    break;
                }
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_NO_CARD;
            }
            if (GCInterface::mUserMsg != UMSG_NONE) {
                InputOptions userChoice;

                userChoice = GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_FILE_CORRUPTED ? 0x25 : 5,
                    GCInterface::mTaskTrcLoadFile.GetID());
                switch (userChoice) {
                case IO_RETRY:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_RETRY;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                case IO_DELETE:
                    GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_START);
                    GCInterface::mTaskTrcLoadFile.SetState(TS_DELETE_FILE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FILE_ASK_USER:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcLoadFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcLoadFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(
                0x25,
                GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                          IO_CONTINUE | IO_RETRY,
                                          IO_NONE,
                                          IO_NONE),
                GCInterface::mTaskTrcLoadFile.mCardID.slot);
            GCInterface::mUserMsg =
                static_cast<UserMessage>(static_cast<int>(
                    GCInterface::mTaskTrcLoadFile.GetSubstate()));
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_DELETE_WAIT_FOR_USER_REPLY);
            break;
        case TS_DELETE_WAIT_FOR_USER_REPLY:
            if (GCInterface::CheckCard(GCInterface::mTaskTrcLoadFile.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_NO_CARD;
            }
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    5,
                    GCInterface::mTaskTrcLoadFile.GetID())) {
                case IO_RETRY:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_RETRY;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                case IO_DELETE:
                    GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_START);
                    GCInterface::mTaskTrcLoadFile.SetState(TS_DELETE_FILE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FILE:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x26, IO_NONE,
                                         GCInterface::mTaskTrcLoadFile.mCardID.slot);
            GCInterface::mMsgTimer.Set(1);
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_DELETE_FILE);
            break;
        case TS_DELETE_FILE:
            switch (GCInterface::mpDriver->DeleteFile(
                GCInterface::mTaskTrcLoadFile.mCardID,
                &GCInterface::mTaskTrcLoadFile.mFileInfo)) {
            case CR_SUCCESS:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FILE_DELETED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_DONE, TS_START);
                break;
            case CR_DAMAGED:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_CARD_DAMAGED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_DELETE_FAILED_INFORM_USER, TS_START);
                break;
            default:
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_FAILED;
                GCInterface::mTaskTrcLoadFile.SetState(TS_DELETE_FAILED_INFORM_USER, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FAILED_INFORM_USER:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x11,
                                         GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                   IO_CONTINUE | IO_RETRY,
                                                                   IO_NONE,
                                                                   IO_NONE),
                                         GCInterface::mTaskTrcLoadFile.mCardID.slot);
            GCInterface::mMsgTimer.Set(1);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_DELETE_FAILED_WAIT_FOR_USER_REPLY);
            break;
        case TS_DELETE_FAILED_WAIT_FOR_USER_REPLY:
            if (GCInterface::CheckCard(GCInterface::mTaskTrcLoadFile.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcLoadFile.mCardStatus = STATUS_NO_CARD;
            }
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    5,
                    GCInterface::mTaskTrcLoadFile.GetID())) {
                case IO_RETRY:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_RETRY;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcLoadFile.SetState(
                        GCInterface::mTaskTrcLoadFile.mCardStatus == STATUS_NO_CARD
                            ? TS_CHECK_REMOUNT
                            : TS_DONE,
                        TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_FAILED_CARD_REMOVED_INFORM_USER:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcLoadFile.mCardID,
                GCInterface::mTaskTrcLoadFile.mTaskResult,
                GCInterface::mTaskTrcLoadFile.mCardStatus,
                GCInterface::mTaskTrcLoadFile.GetID(),
                true);
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            GCInterface::mTaskTrcLoadFile.mTaskResult =
                GCInterface::mTaskShowCardStatusMsg.mTaskResult;
            GCInterface::mTaskTrcLoadFile.mCardStatus =
                GCInterface::mTaskShowCardStatusMsg.mCardStatus;
            GCInterface::mTaskTrcLoadFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_OVERWRITE_ASK_USER:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(
                0x18,
                GCMessage::PackMsgOptions(IO_CONTINUE | IO_OVERWRITE,
                                          IO_FORMAT | IO_RETRY | IO_OVERWRITE,
                                          IO_NONE,
                                          IO_NONE));
            GCInterface::mUserMsg = static_cast<UserMessage>(static_cast<int>(
                GCInterface::mTaskTrcSaveFile.GetSubstate()));
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_OVERWRITE_CHECK_USER_REPLY);
            break;
        case TS_OVERWRITE_CHECK_USER_REPLY:
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    IO_NO | IO_YES,
                    GCInterface::mTaskTrcSaveFile.GetID())) {
                case IO_NO:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                    break;
                case IO_YES:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
                    GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_OK;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcLoadFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcLoadFile.mCardID,
                                              GCInterface::mTaskTrcLoadFile.GetID(),
                                              true);
            GCInterface::mTaskTrcLoadFile.SetSubstate(TS_MOUNT_RESULT_WAIT_USER_REPLY);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_MOUNT_RESULT_WAIT_USER_REPLY:
            GCInterface::mTaskTrcLoadFile.mCardStatus =
                GCInterface::mTaskTrcMount.mCardStatus;
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcLoadFile.mTaskResult = RESULT_CANCELLED;
                GCInterface::mTaskTrcLoadFile.mMounted = true;
            } else {
                GCInterface::mTaskTrcLoadFile.mTaskResult =
                    GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcLoadFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcLoadFile.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcLoadFile.mCardID);
            GCInterface::mTaskTrcLoadFile.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_LOADFILE_DONE,
                                   GCInterface::mTaskTrcLoadFile.mTaskResult,
                                   GCInterface::mTaskTrcLoadFile.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcLoadFile.mCardID;
        GCInterface::mTaskTrcLoadFile.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcSaveCheck() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcSaveFile.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcSaveFile.mMounted = false;
        GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcSaveFile.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcSaveFile.mCardID,
                                              GCInterface::mTaskTrcSaveFile.GetID(),
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_SHOW_CARD_STATUS_MSG:
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                GCInterface::mTaskTrcMount.mTaskResult,
                GCInterface::mTaskTrcMount.mCardStatus,
                GCInterface::mTaskTrcSaveFile.GetID(),
                false);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcSaveFile.mMounted = true;
                GCInterface::mTaskTrcSaveFile.SetState(TS_CHECK_SPACE, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcMount.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcMount.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcSaveFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
                GCInterface::mTaskTrcSaveFile.mMounted = false;
                break;
            default:
                GCInterface::mTaskTrcSaveFile.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcSaveFile.mMounted = false;
                if (GCInterface::mTaskShowCardStatusMsg.mTaskResult == RESULT_CANCELLED) {
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                } else {
                    GCInterface::mTaskTrcSaveFile.mTaskResult =
                        GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                }
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_SPACE:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcCheckSpace.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                &GCInterface::mTaskTrcSaveFile.mFileInfo,
                GCInterface::mTaskTrcSaveFile.mBlocksNeeded,
                GCInterface::mTaskTrcSaveFile.mFilesNeeded,
                GCInterface::mTaskTrcSaveFile.GetID(),
                false);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_SPACE_RESULT);
            GCInterface::mTaskTrcCheckSpace.mParent->StartTask(
                &GCInterface::mTaskTrcCheckSpace);
            break;
        case TS_CHECK_SPACE_RESULT:
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskTrcCheckSpace.mCardStatus;
            switch (GCInterface::mTaskTrcCheckSpace.mTaskResult) {
            case RESULT_SUCCESS:
                if (GCInterface::mTaskTrcCheckSpace.mFileExists) {
                    GCInterface::mTaskTrcSaveFile.SetState(TS_OVERWRITE_ASK_USER, TS_START);
                } else if (GCInterface::mTaskTrcSaveFile.mCardStatus == STATUS_OK) {
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                } else {
                    GCInterface::mTaskTrcSaveFile.SetSubstate(TS_INSUFFICIENT_SPACE_ASK_USER);
                }
                break;
            case RESULT_RETRY:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            case RESULT_FAILED:
            case RESULT_CANCELLED:
            default:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        case TS_INSUFFICIENT_SPACE_ASK_USER: {
            if (GCInterface::mTaskTrcSaveFile.mFileInfo.usingMultipleSaves) {
                GCInterface::mTaskMsg.LC_msg(
                    0x13,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                    IO_NONE,
                    IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot,
                    GCInterface::mTaskTrcSaveFile.mFileInfo.gameTitle);
            } else {
                wchar_t displayedName[64];
                wchar_t unicodeSpace[2];

                RealmcUtils::Ascii2Unicode(unicodeSpace, " ");
                if (GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName != nullptr) {
                    wcscpy(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName);
                    wcscat(displayedName, unicodeSpace);
                    wcscat(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName);
                } else {
                    wcscpy(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName);
                }
                GCInterface::mTaskMsg.LC_msg(
                    0x16,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot,
                    displayedName,
                    GCInterface::mTaskTrcSaveFile.mBlocksNeeded,
                    GCInterface::mTaskTrcSaveFile.mFilesNeeded);
            }
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_WAIT_FOR_USER_REPLY);
            break;
        }
        case TS_WAIT_FOR_USER_REPLY:
            if (GCInterface::mUserMsg == UMSG_NONE) {
                if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) != STATUS_NO_CARD) {
                    break;
                }
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            switch (GCInterface::ConvertUmsgToOption(
                GCInterface::mUserMsg,
                5,
                GCInterface::mTaskTrcSaveFile.GetID())) {
            case IO_RETRY:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                break;
            case IO_CONTINUE:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
                break;
            default:
                return;
            }
            GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_OVERWRITE_ASK_USER:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x18, GCMessage::PackMsgOptions(
                                                   IO_CONTINUE | IO_OVERWRITE,
                                                   IO_FORMAT | IO_RETRY | IO_OVERWRITE,
                                                   IO_NONE,
                                                   IO_NONE));
            GCInterface::mUserMsg = static_cast<UserMessage>(static_cast<int>(
                GCInterface::mTaskTrcSaveFile.GetSubstate()));
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_OVERWRITE_CHECK_USER_REPLY);
            break;
        case TS_OVERWRITE_CHECK_USER_REPLY:
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    IO_NO | IO_YES,
                    GCInterface::mTaskTrcSaveFile.GetID())) {
                case IO_NO:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                    break;
                case IO_YES:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
                    GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_OK;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_FAILED_CARD_REMOVED_INFORM_USER:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                GCInterface::mTaskTrcSaveFile.mTaskResult,
                GCInterface::mTaskTrcSaveFile.mCardStatus,
                GCInterface::mTaskTrcSaveFile.GetID(),
                true);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            GCInterface::mTaskTrcSaveFile.mTaskResult =
                GCInterface::mTaskShowCardStatusMsg.mTaskResult;
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskShowCardStatusMsg.mCardStatus;
            GCInterface::mTaskTrcSaveFile.SetState(TS_CHECK_REMOUNT, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcSaveFile.mMounted) {
                GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcSaveFile.mCardID);
                GCInterface::mTaskTrcSaveFile.mMounted = false;
            }
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcSaveFile.mCardID,
                                              GCInterface::mTaskTrcSaveFile.GetID(),
                                              false);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_MOUNT_RESULT_WAIT_USER_REPLY);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_MOUNT_RESULT_WAIT_USER_REPLY:
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskTrcMount.mCardStatus;
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                GCInterface::mTaskTrcSaveFile.mMounted = true;
            } else {
                GCInterface::mTaskTrcSaveFile.mTaskResult =
                    GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcSaveFile.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcSaveFile.mCardID);
            GCInterface::mTaskTrcSaveFile.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_SAVECHECK_DONE,
                                   GCInterface::mTaskTrcSaveFile.mTaskResult,
                                   GCInterface::mTaskTrcSaveFile.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcSaveFile.mCardID;
        GCInterface::mTaskTrcSaveFile.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcSaveFile() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcSaveFile.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcSaveFile.mMounted = false;
        GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcSaveFile.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcSaveFile.mCardID,
                                              GCInterface::mTaskTrcSaveFile.GetID(),
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_SHOW_CARD_STATUS_MSG:
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                GCInterface::mTaskTrcMount.mTaskResult,
                GCInterface::mTaskTrcMount.mCardStatus,
                GCInterface::mTaskTrcSaveFile.GetID(),
                false);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcSaveFile.mMounted = true;
                GCInterface::mTaskTrcSaveFile.SetState(TS_CHECK_SPACE, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcMount.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcMount.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcSaveFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
                GCInterface::mTaskTrcSaveFile.mMounted = false;
                break;
            default:
                GCInterface::mTaskTrcSaveFile.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcSaveFile.mMounted = false;
                GCInterface::mTaskTrcSaveFile.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_SPACE:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_SPACE_RESULT);
            GCInterface::mTaskTrcCheckSpace.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                &GCInterface::mTaskTrcSaveFile.mFileInfo,
                GCInterface::mTaskTrcSaveFile.GetID(),
                false);
            GCInterface::mTaskTrcCheckSpace.mParent->StartTask(
                &GCInterface::mTaskTrcCheckSpace);
            break;
        case TS_CHECK_SPACE_RESULT:
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskTrcCheckSpace.mCardStatus;
            switch (GCInterface::mTaskTrcCheckSpace.mTaskResult) {
            case RESULT_SUCCESS:
                if (GCInterface::mTaskTrcCheckSpace.mFileExists) {
                    GCInterface::mTaskTrcSaveFile.SetState(TS_OVERWRITE_ASK_USER, TS_START);
                } else if (GCInterface::mTaskTrcSaveFile.mCardStatus == STATUS_OK) {
                    GCInterface::mTaskTrcSaveFile.SetState(TS_SAVE_READY, TS_START);
                } else {
                    GCInterface::mTaskTrcSaveFile.SetSubstate(TS_INSUFFICIENT_SPACE_ASK_USER);
                }
                break;
            case RESULT_RETRY:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            case RESULT_FAILED:
            case RESULT_CANCELLED:
            default:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        case TS_INSUFFICIENT_SPACE_ASK_USER: {
            wchar_t displayedName[64];
            wchar_t unicodeSpace[2];

            RealmcUtils::Ascii2Unicode(unicodeSpace, " ");
            if (GCInterface::mTaskTrcSaveFile.mFileInfo.usingMultipleSaves) {
                GCInterface::mTaskMsg.LC_msg(
                    0x13,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot,
                    GCInterface::mTaskTrcSaveFile.mFileInfo.gameTitle);
            } else if (GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName != nullptr) {
                wcscpy(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName);
                wcscat(displayedName, unicodeSpace);
                wcscat(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName);
                GCInterface::mTaskMsg.LC_msg(
                    0x16,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot,
                    displayedName,
                    GCInterface::mTaskTrcSaveFile.mBlocksNeeded,
                    GCInterface::mTaskTrcSaveFile.mFilesNeeded);
            } else {
                wcscpy(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName);
                GCInterface::mTaskMsg.LC_msg(
                    0x16,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot,
                    displayedName,
                    GCInterface::mTaskTrcSaveFile.mBlocksNeeded,
                    GCInterface::mTaskTrcSaveFile.mFilesNeeded);
            }
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_WAIT_FOR_USER_REPLY);
            break;
        }
        case TS_WAIT_FOR_USER_REPLY:
            if (GCInterface::mUserMsg == UMSG_NONE) {
                if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) != STATUS_NO_CARD) {
                    break;
                }
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                break;
            }
            switch (GCInterface::ConvertUmsgToOption(
                GCInterface::mUserMsg, 5, GCInterface::mTaskTrcSaveFile.GetID())) {
            case IO_RETRY:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                break;
            case IO_CONTINUE:
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
                break;
            default:
                return;
            }
            GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_OVERWRITE_ASK_USER:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START: {
            wchar_t displayedName[64];
            wchar_t unicodeSpace[2];

            if (GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName != nullptr) {
                GCInterface::mTaskMsg.LC_msg(
                    0x19,
                    GCMessage::PackMsgOptions(IO_CONTINUE | IO_OVERWRITE,
                                              IO_FORMAT | IO_RETRY | IO_OVERWRITE,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mFileInfo.gameTitle,
                    GCInterface::mTaskTrcSaveFile.mFileInfo.fileTypeName,
                    GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName,
                    GCInterface::mTaskTrcSaveFile.mCardID.slot);
            } else {
                RealmcUtils::Ascii2Unicode(unicodeSpace, " ");
                wcscpy(displayedName, unicodeSpace);
                wcscat(displayedName, GCInterface::mTaskTrcSaveFile.mFileInfo.fileContentName);
                GCInterface::mTaskMsg.LC_msg(
                    0x19,
                    GCMessage::PackMsgOptions(IO_CONTINUE | IO_OVERWRITE,
                                              IO_FORMAT | IO_RETRY | IO_OVERWRITE,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mFileInfo.gameTitle,
                    displayedName,
                    GCInterface::mTaskTrcSaveFile.mCardID.slot);
            }
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_OVERWRITE_CHECK_USER_REPLY);
            break;
        }
        case TS_OVERWRITE_CHECK_USER_REPLY:
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    IO_NO | IO_YES,
                    GCInterface::mTaskTrcSaveFile.GetID())) {
                case IO_NO:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                    break;
                case IO_YES:
                    GCInterface::mTaskTrcSaveFile.SetState(TS_SAVE_READY, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_SAVE_READY:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(
                0x1a,
                GCMessage::PackMsgOptions(IO_CONTINUE, IO_NONE, IO_NONE, IO_NONE),
                GCInterface::mTaskTrcSaveFile.mCardID.slot);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_WAIT_FOR_RESULT);
            break;
        case TS_WAIT_FOR_RESULT:
            if (GCInterface::mUserMsg != UMSG_NONE) {
                GCInterface::mTaskTrcSaveFile.SetState(TS_OPEN_FILE, TS_START);
            }
            break;
        default:
            break;
        }
        break;
    case TS_OPEN_FILE:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x1b, 0, GCInterface::mTaskTrcSaveFile.mCardID.slot);
            GCInterface::mMsgTimer.Set(GCInterface::mMsgTimer.mNumSecondsDefaultDelay);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_OPEN_FILE);
            break;
        case TS_OPEN_FILE: {
            ICardResult result;

            result = GCInterface::mpDriver->OpenFile(
                GCInterface::mTaskTrcSaveFile.mCardID,
                &GCInterface::mTaskTrcSaveFile.mFileInfo,
                FOM_CREATE,
                &GCInterface::mTaskTrcSaveFile.mFileHandle);
            switch (result) {
            case CR_SUCCESS:
                GCInterface::mTaskMsg.Set(LMSG_OPEN_FILE_DONE,
                                            static_cast<TaskResult>(static_cast<int>(result)),
                                            static_cast<CardStatus>(static_cast<int>(result)));
                GCInterface::mTaskMsg.info.openResult.fileHandle =
                    GCInterface::mTaskTrcSaveFile.mFileHandle;
                GCInterface::mTaskMsg.info.openResult.fileName =
                    GCInterface::mTaskTrcSaveFile.mFileInfo.fileName;
                GCInterface::mTaskTrcSaveFile.SetState(
                    TS_WRITE_READY, static_cast<TaskState>(static_cast<int>(result)));
                break;
            case CR_NOCARD:
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED, TS_START);
                break;
            case CR_DAMAGED:
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_CARD_DAMAGED;
                GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED, TS_START);
                break;
            default:
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_FAILED;
                GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED, TS_START);
                break;
            }
            break;
        }
        default:
            break;
        }
        break;
    case TS_WRITE_READY:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.Set(LMSG_WRITE_READY);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_WRITE_WAIT);
            break;
        case TS_WRITE_WAIT:
            if (GCInterface::mTaskWrite.mTaskResult == RESULT_FAILED) {
                GCInterface::mTaskTrcSaveFile.mCardStatus = GCInterface::mTaskWrite.mCardStatus;
            } else if (GCInterface::mUserMsg != UMSG_WRITE_COMPLETE) {
                break;
            } else {
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_SUCCESS;
            }
            switch (GCInterface::mpDriver->CloseFile(GCInterface::mTaskTrcSaveFile.mFileHandle)) {
            case CR_SUCCESS:
                GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_OK;
                break;
            case CR_NOCARD:
                GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED, TS_START);
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_NO_CARD;
                break;
            default:
                GCInterface::mTaskTrcSaveFile.SetState(TS_FAILED, TS_START);
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_FAILED;
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_FAILED:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcSaveFile.mCardStatus == STATUS_CARD_DAMAGED) {
                GCInterface::mTaskMsg.LC_msg(
                    0x11,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot);
            } else {
                GCInterface::mTaskMsg.LC_msg(
                    0x1c,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcSaveFile.mCardID.slot);
            }
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_WAIT_FOR_USER_REPLY);
            break;
        case TS_WAIT_FOR_USER_REPLY:
            if (GCInterface::CheckCard(GCInterface::mTaskTrcSaveFile.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcSaveFile.mCardStatus = STATUS_NO_CARD;
            }
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    5,
                    GCInterface::mTaskTrcSaveFile.GetID())) {
                case IO_RETRY:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_RETRY;
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                    break;
                default:
                    return;
                }
                GCInterface::mTaskTrcSaveFile.SetState(
                    GCInterface::mTaskTrcSaveFile.mCardStatus == STATUS_NO_CARD
                        ? TS_CHECK_REMOUNT
                        : TS_DONE,
                    TS_START);
            }
            break;
        default:
            break;
        }
        break;
    case TS_FAILED_CARD_REMOVED_INFORM_USER:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcSaveFile.mCardID,
                GCInterface::mTaskTrcSaveFile.mTaskResult,
                GCInterface::mTaskTrcSaveFile.mCardStatus,
                GCInterface::mTaskTrcSaveFile.GetID(),
                true);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            GCInterface::mTaskTrcSaveFile.mTaskResult =
                GCInterface::mTaskShowCardStatusMsg.mTaskResult;
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskShowCardStatusMsg.mCardStatus;
            GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcSaveFile.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcSaveFile.mMounted) {
                GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcSaveFile.mCardID);
                GCInterface::mTaskTrcSaveFile.mMounted = false;
            }
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcSaveFile.mCardID,
                                              GCInterface::mTaskTrcSaveFile.GetID(),
                                              false);
            GCInterface::mTaskTrcSaveFile.SetSubstate(TS_MOUNT_RESULT_WAIT_USER_REPLY);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_MOUNT_RESULT_WAIT_USER_REPLY:
            GCInterface::mTaskTrcSaveFile.mCardStatus =
                GCInterface::mTaskTrcMount.mCardStatus;
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcSaveFile.mTaskResult = RESULT_CANCELLED;
                GCInterface::mTaskTrcSaveFile.mMounted = true;
            } else {
                GCInterface::mTaskTrcSaveFile.mTaskResult =
                    GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcSaveFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcSaveFile.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcSaveFile.mCardID);
            GCInterface::mTaskTrcSaveFile.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_SAVEFILE_DONE,
                                   GCInterface::mTaskTrcSaveFile.mTaskResult,
                                   GCInterface::mTaskTrcSaveFile.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcSaveFile.mCardID;
        GCInterface::mTaskTrcSaveFile.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcDeleteFile() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcDeleteFile.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcDeleteFile.mMounted = false;
        GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcDeleteFile.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcDeleteFile.mCardID,
                                              TID_NONE,
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_SHOW_CARD_STATUS_MSG:
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcDeleteFile.mCardID,
                GCInterface::mTaskTrcMount.mTaskResult,
                GCInterface::mTaskTrcMount.mCardStatus,
                GCInterface::mTaskTrcDeleteFile.GetID(),
                false);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcDeleteFile.mMounted = true;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DELETE_FILE_ASK_USER, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcMount.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcMount.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
                GCInterface::mTaskTrcDeleteFile.mMounted = false;
                break;
            default:
                GCInterface::mTaskTrcDeleteFile.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcDeleteFile.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcDeleteFile.mMounted = false;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FILE_ASK_USER:
        if (GCInterface::CheckCard(GCInterface::mTaskTrcDeleteFile.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcDeleteFile.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        }
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(
                0x25,
                GCMessage::PackMsgOptions(IO_CONTINUE | IO_RETRY,
                                          IO_OVERWRITE | IO_RETRY,
                                          IO_NONE,
                                          IO_NONE));
            GCInterface::mUserMsg = static_cast<UserMessage>(static_cast<int>(
                GCInterface::mTaskTrcDeleteFile.GetSubstate()));
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_DELETE_WAIT_FOR_USER_REPLY);
            break;
        case TS_DELETE_WAIT_FOR_USER_REPLY:
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    0x24,
                    GCInterface::mTaskTrcDeleteFile.GetID())) {
                case IO_CONTINUE:
                    GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_CANCELLED;
                    GCInterface::mTaskTrcDeleteFile.SetState(TS_DONE, TS_START);
                    break;
                case IO_DELETE:
                    GCInterface::mTaskTrcDeleteFile.SetState(TS_DELETE_FILE, TS_START);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FILE:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskMsg.LC_msg(0x26, IO_NONE,
                                         GCInterface::mTaskTrcDeleteFile.mCardID.slot);
            GCInterface::mMsgTimer.Set(1);
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_DELETE_FILE);
            break;
        case TS_DELETE_FILE:
            switch (GCInterface::mpDriver->DeleteFile(
                GCInterface::mTaskTrcDeleteFile.mCardID,
                &GCInterface::mTaskTrcDeleteFile.mFileInfo)) {
            case CR_SUCCESS:
                GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_SUCCESS;
                GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_OK;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DONE, TS_START);
                break;
            case CR_NOCARD:
            case CR_NOFILE:
                GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_FILE_NOT_FOUND;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DELETE_FAILED_INFORM_USER, TS_START);
                break;
            case CR_DAMAGED:
                GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_CARD_DAMAGED;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DELETE_FAILED_INFORM_USER, TS_START);
                break;
            default:
                GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_FAILED;
                GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_FAILED;
                GCInterface::mTaskTrcDeleteFile.SetState(TS_DELETE_FAILED_INFORM_USER, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_DELETE_FAILED_INFORM_USER:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            if (GCInterface::mTaskTrcDeleteFile.mCardStatus == STATUS_CARD_DAMAGED) {
                GCInterface::mTaskMsg.LC_msg(
                    0x11,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcDeleteFile.mCardID.slot);
            } else {
                GCInterface::mTaskMsg.LC_msg(
                    0x27,
                    GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                              IO_CONTINUE | IO_RETRY,
                                              IO_NONE,
                                              IO_NONE),
                    GCInterface::mTaskTrcDeleteFile.mCardID.slot);
            }
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcDeleteFile.SetSubstate(
                TS_DELETE_FAILED_WAIT_FOR_USER_REPLY);
            break;
        case TS_DELETE_FAILED_WAIT_FOR_USER_REPLY:
            if (GCInterface::CheckCard(GCInterface::mTaskTrcDeleteFile.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcDeleteFile.mCardStatus = STATUS_NO_CARD;
            }
            if (GCInterface::mUserMsg != UMSG_NONE) {
                switch (GCInterface::ConvertUmsgToOption(
                    GCInterface::mUserMsg,
                    5,
                    GCInterface::mTaskTrcDeleteFile.GetID())) {
                case IO_RETRY:
                    GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_RETRY;
                    break;
                case IO_CONTINUE:
                    GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_CANCELLED;
                    break;
                default:
                    return;
                }
                GCInterface::mTaskTrcDeleteFile.SetState(
                    GCInterface::mTaskTrcDeleteFile.mCardStatus == STATUS_NO_CARD
                        ? TS_CHECK_REMOUNT
                        : TS_DONE,
                    TS_START);
            }
            break;
        default:
            break;
        }
        break;
    case TS_FAILED_CARD_REMOVED_INFORM_USER:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcDeleteFile.mCardID,
                GCInterface::mTaskTrcDeleteFile.mTaskResult,
                GCInterface::mTaskTrcDeleteFile.mCardStatus,
                GCInterface::mTaskTrcDeleteFile.GetID(),
                true);
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            GCInterface::mTaskTrcDeleteFile.mTaskResult =
                GCInterface::mTaskShowCardStatusMsg.mTaskResult;
            GCInterface::mTaskTrcDeleteFile.mCardStatus =
                GCInterface::mTaskShowCardStatusMsg.mCardStatus;
            GCInterface::mTaskTrcDeleteFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_CHECK_REMOUNT:
        switch (GCInterface::mTaskTrcDeleteFile.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcDeleteFile.mCardID,
                                              GCInterface::mTaskTrcDeleteFile.GetID(),
                                              true);
            GCInterface::mTaskTrcDeleteFile.SetSubstate(TS_MOUNT_RESULT_WAIT_USER_REPLY);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_MOUNT_RESULT_WAIT_USER_REPLY:
            GCInterface::mTaskTrcDeleteFile.mCardStatus =
                GCInterface::mTaskTrcMount.mCardStatus;
            if (GCInterface::mTaskTrcMount.mTaskResult == RESULT_SUCCESS) {
                GCInterface::mTaskTrcDeleteFile.mTaskResult = RESULT_CANCELLED;
                GCInterface::mTaskTrcDeleteFile.mMounted = true;
            } else {
                GCInterface::mTaskTrcDeleteFile.mTaskResult =
                    GCInterface::mTaskTrcMount.mTaskResult;
            }
            GCInterface::mTaskTrcDeleteFile.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcDeleteFile.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcDeleteFile.mCardID);
            GCInterface::mTaskTrcDeleteFile.mMounted = false;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_DELETEFILE_DONE,
                                   GCInterface::mTaskTrcDeleteFile.mTaskResult,
                                   GCInterface::mTaskTrcDeleteFile.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskTrcDeleteFile.mCardID;
        GCInterface::mTaskTrcDeleteFile.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcListFiles() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcListFiles.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcListFiles.mMounted = false;
        GCInterface::mTaskTrcListFiles.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcListFiles.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcListFiles.mNumFilesFound = 0;
        GCInterface::mTaskTrcListFiles.SetState(TS_MOUNT, TS_START);
    case TS_MOUNT:
        switch (GCInterface::mTaskTrcListFiles.GetSubstate()) {
        case TS_START:
            GCInterface::mTaskTrcListFiles.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
            GCInterface::mTaskTrcMount.Start(GCInterface::mTaskTrcListFiles.mCardID,
                                              TID_TRC_LISTFILES,
                                              false);
            GCInterface::mTaskTrcMount.mParent->StartTask(&GCInterface::mTaskTrcMount);
            break;
        case TS_SHOW_CARD_STATUS_MSG:
            GCInterface::mTaskTrcListFiles.SetSubstate(TS_CHECK_RESULT);
            GCInterface::mTaskShowCardStatusMsg.Start(
                GCInterface::mTaskTrcListFiles.mCardID,
                GCInterface::mTaskTrcMount.mTaskResult,
                GCInterface::mTaskTrcMount.mCardStatus,
                GCInterface::mTaskTrcListFiles.GetID(),
                false);
            GCInterface::mTaskShowCardStatusMsg.mParent->StartTask(
                &GCInterface::mTaskShowCardStatusMsg);
            break;
        case TS_CHECK_RESULT:
            switch (GCInterface::mTaskShowCardStatusMsg.mTaskResult) {
            case RESULT_SUCCESS:
                GCInterface::mTaskTrcListFiles.mMounted = true;
                GCInterface::mTaskTrcListFiles.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcListFiles.mListingStarted = true;
                GCInterface::mTaskTrcListFiles.SetState(TS_FIND_FILE, TS_START);
                break;
            case RESULT_FAILED:
                GCInterface::mTaskTrcMount.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcMount.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcListFiles.SetSubstate(TS_SHOW_CARD_STATUS_MSG);
                break;
            default:
                GCInterface::mTaskTrcListFiles.mTaskResult =
                    GCInterface::mTaskShowCardStatusMsg.mTaskResult;
                GCInterface::mTaskTrcListFiles.mCardStatus =
                    GCInterface::mTaskShowCardStatusMsg.mCardStatus;
                GCInterface::mTaskTrcListFiles.SetState(TS_DONE, TS_START);
                break;
            }
            break;
        default:
            break;
        }
        break;
    case TS_FIND_FILE:
        switch (GCInterface::mTaskTrcListFiles.GetSubstate()) {
        case TS_START: {
            FindResult *result;

            result = GCInterface::Find(GCInterface::mTaskTrcListFiles.mCardID,
                                        GCInterface::mTaskTrcListFiles.mFileInfo.fileName,
                                        true);
            if (result->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
                GCInterface::mTaskTrcListFiles.mCardStatus = result->msg.mCardStatus;
                GCInterface::mTaskTrcListFiles.SetState(TS_DONE, TS_START);
            } else {
                ++GCInterface::mTaskTrcListFiles.mNumFilesFound;
                GCInterface::mTaskMsg = result->msg;
                GCInterface::mTaskTrcListFiles.SetSubstate(TS_FIND_NEXT);
            }
            break;
        }
        case TS_FIND_NEXT: {
            FindResult *result;

            result = GCInterface::Find(GCInterface::mTaskTrcListFiles.mCardID,
                                        GCInterface::mTaskTrcListFiles.mFileInfo.fileName,
                                        false);
            if (result->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
                GCInterface::mTaskTrcListFiles.SetState(TS_DONE, TS_START);
            } else {
                ++GCInterface::mTaskTrcListFiles.mNumFilesFound;
                GCInterface::mTaskMsg = result->msg;
            }
            break;
        }
        default:
            break;
        }
        break;
    /* Keep the state machine's terminal work separate from the find cases. */
    case TS_DONE:
        if (GCInterface::mTaskTrcListFiles.mListingStarted &&
            GCInterface::CheckCard(GCInterface::mTaskTrcListFiles.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcListFiles.SetState(TS_START, TS_START);
        }
        if (GCInterface::mTaskTrcListFiles.mMounted) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcListFiles.mCardID);
            GCInterface::mTaskTrcListFiles.mMounted = false;
        }
        if (GCInterface::mTaskTrcListFiles.mTaskResult == RESULT_SUCCESS) {
            GCInterface::mTaskTrcListFiles.mCardStatus =
                GCInterface::mTaskTrcListFiles.mNumFilesFound == 0
                    ? STATUS_FILE_NOT_FOUND
                    : STATUS_OK;
        }
        GCInterface::mTaskMsg.Set(LMSG_TRC_LISTFILES_DONE,
                                   GCInterface::mTaskTrcListFiles.mTaskResult,
                                   GCInterface::mTaskTrcListFiles.mCardStatus);
        GCInterface::mTaskTrcListFiles.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcMount() {
    switch (GCInterface::mTaskTrcMount.GetState()) {
    case TS_START: {
        bool cardFound;

        GCInterface::mTaskTrcMount.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcMount.mCardStatus = STATUS_UNKNOWN;
        cardFound = false;
        {
            int i;
            for (i = 0; i < 3 && !cardFound; ++i) {
                switch (GCInterface::mpDriver->CardExists(GCInterface::mTaskTrcMount.mCardID)) {
                case CR_SUCCESS:
                    GCInterface::mTaskTrcMount.SetState(TS_MOUNT, TS_START);
                    cardFound = true;
                    break;
                case CR_WRONGDEVICE:
                    GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
                    GCInterface::mTaskTrcMount.mCardStatus = STATUS_WRONG_DEVICE;
                    GCInterface::mTaskTrcMount.SetState(TS_DONE, TS_START);
                    break;
                case CR_NOCARD:
                    GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
                    GCInterface::mTaskTrcMount.mCardStatus = STATUS_NO_CARD;
                    GCInterface::mTaskTrcMount.SetState(TS_DONE, TS_START);
                    break;
                default:
                    break;
                }
            }
        }
        break;
    }
    case TS_MOUNT:
        GCInterface::mUserMsg = UMSG_NONE;
        GCInterface::mTaskTrcMount.SetState(TS_DONE, TS_START);
        switch (GCInterface::mpDriver->Mount(GCInterface::mTaskTrcMount.mCardID)) {
        case CR_SUCCESS:
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_SUCCESS;
            GCInterface::mTaskTrcMount.mCardStatus = STATUS_OK;
            break;
        case CR_DAMAGED:
        case CR_OTHERERROR:
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcMount.mCardStatus = STATUS_CARD_DAMAGED;
            break;
        case CR_WRONGDEVICE:
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcMount.mCardStatus = STATUS_WRONG_DEVICE;
            break;
        case CR_CORRUPT:
        case CR_ENCODINGERROR:
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcMount.mCardStatus = STATUS_CARD_UNFORMATTED;
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcMount.mCardID);
            GCInterface::mTaskTrcMount.SetState(TS_DONE, TS_START);
            break;
        case CR_NOCARD:
            GCInterface::mTaskTrcMount.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcMount.mCardStatus = STATUS_NO_CARD;
            break;
        default:
            break;
        }
        break;
    case TS_DONE:
        GCInterface::mTaskTrcMount.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcFormat() {
    if (!GCInterface::mMsgTimer.IsExpired()) {
        return;
    }

    switch (GCInterface::mTaskTrcFormat.GetState()) {
    case TS_START:
        GCInterface::mTaskTrcFormat.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcFormat.mCardStatus = STATUS_CARD_UNFORMATTED;
        GCInterface::mpDriver->Mount(GCInterface::mTaskTrcFormat.mCardID);
        GCInterface::mUserMsg = UMSG_NONE;
        GCInterface::mTaskMsg.LC_msg(0x10,
                                     GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                               IO_OVERWRITE,
                                                               IO_CONTINUE | IO_RETRY,
                                                               IO_NONE),
                                     GCInterface::mTaskTrcFormat.mCardID.slot);
        GCInterface::mTaskTrcFormat.SetState(TS_WAIT_FOR_USER_REPLY, TS_START);
        break;
    case TS_WAIT_FOR_USER_REPLY:
        if (GCInterface::mUserMsg != UMSG_NONE) {
            switch (GCInterface::ConvertUmsgToOption(GCInterface::mUserMsg,
                                                      7,
                                                      GCInterface::mTaskTrcFormat.mTrcTaskID)) {
            case IO_FORMAT:
                GCInterface::mTaskMsg.LC_msg(0x22,
                                             GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                       IO_OVERWRITE,
                                                                       IO_CONTINUE | IO_RETRY,
                                                                       IO_NONE),
                                             GCInterface::mTaskTrcFormat.mTrcTaskID);
                GCInterface::mUserMsg = UMSG_NONE;
                GCInterface::mTaskTrcFormat.SetState(TS_FORMAT_WAIT_FOR_USER_CONFIRMATION, TS_START);
                break;
            case IO_RETRY:
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
                break;
            case IO_CONTINUE:
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_CANCELLED;
                GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
                break;
            default:
                break;
            }
        } else {
            if (GCInterface::CheckCard(GCInterface::mTaskTrcFormat.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcFormat.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
            }
        }
        break;
    case TS_FORMAT_WAIT_FOR_USER_CONFIRMATION:
        if (GCInterface::mUserMsg == UMSG_NONE) {
            if (GCInterface::CheckCard(GCInterface::mTaskTrcFormat.mCardID) == STATUS_NO_CARD) {
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskTrcFormat.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
            }
            break;
        }
        switch (GCInterface::ConvertUmsgToOption(GCInterface::mUserMsg,
                                                  7,
                                                  GCInterface::mTaskTrcFormat.mTrcTaskID)) {
        case IO_FORMAT:
            GCInterface::mTaskMsg.LC_msg(0x23, 0, GCInterface::mTaskTrcFormat.mCardID.slot);
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mMsgTimer.Set(GCInterface::mMsgTimer.mNumSecondsDefaultDelay);
            GCInterface::mTaskTrcFormat.SetState(TS_FORMAT, TS_START);
            break;
        case IO_RETRY:
            GCInterface::mTaskTrcFormat.mTaskResult = RESULT_RETRY;
            GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
            break;
        case IO_CONTINUE:
            GCInterface::mTaskTrcFormat.mTaskResult = RESULT_CANCELLED;
            GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
            break;
        default:
            break;
        }
        break;
    case TS_FORMAT:
        GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
        switch (GCInterface::mpDriver->FormatCard(GCInterface::mTaskTrcFormat.mCardID)) {
        case CR_SUCCESS:
            GCInterface::mTaskTrcFormat.mTaskResult = RESULT_SUCCESS;
            GCInterface::mTaskTrcFormat.mCardStatus = STATUS_OK;
            break;
        case CR_NOCARD:
            GCInterface::mTaskTrcFormat.SetState(TS_FAILED_CARD_REMOVED_INFORM_USER, TS_START);
            break;
        default:
            GCInterface::mTaskTrcFormat.mTaskResult = RESULT_FAILED;
            GCInterface::mTaskTrcFormat.mCardStatus = STATUS_CARD_DAMAGED;
            break;
        }
        break;
    case TS_FAILED_CARD_REMOVED_INFORM_USER:
        GCInterface::mTaskTrcFormat.mTaskResult = RESULT_FAILED;
        GCInterface::mTaskTrcFormat.mCardStatus = STATUS_NO_CARD;
        GCInterface::mTaskMsg.LC_msg(0x24,
                                     GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                               IO_CONTINUE | IO_RETRY,
                                                               IO_NONE,
                                                               IO_NONE),
                                     GCInterface::mTaskTrcFormat.mCardID.slot);
        GCInterface::mUserMsg = UMSG_NONE;
        GCInterface::mTaskTrcFormat.SetState(TS_FAILED_CARD_REMOVED_WAIT_FOR_USER_REPLY, TS_START);
        break;
    case TS_FAILED_CARD_REMOVED_WAIT_FOR_USER_REPLY:
        if (GCInterface::mUserMsg != UMSG_NONE) {
            switch (GCInterface::ConvertUmsgToOption(GCInterface::mUserMsg,
                                                      5,
                                                      GCInterface::mTaskTrcFormat.GetID())) {
            case IO_CONTINUE:
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_CANCELLED;
                break;
            case IO_RETRY:
                GCInterface::mTaskTrcFormat.mTaskResult = RESULT_RETRY;
                break;
            default:
                break;
            }
            GCInterface::mTaskTrcFormat.SetState(TS_DONE, TS_START);
        }
        break;
    case TS_DONE:
        if (GCInterface::mTaskTrcFormat.mTaskResult != RESULT_SUCCESS) {
            GCInterface::mpDriver->Unmount(GCInterface::mTaskTrcFormat.mCardID);
        }
        GCInterface::mTaskTrcFormat.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskShowCardStatusMessage() {
    switch (GCInterface::mTaskShowCardStatusMsg.GetState()) {
    case TS_START:
        if (GCInterface::mTaskShowCardStatusMsg.mTaskResult == RESULT_CANCELLED ||
            GCInterface::mTaskShowCardStatusMsg.mTaskResult == RESULT_RETRY) {
            GCInterface::mTaskShowCardStatusMsg.SetState(TS_DONE, TS_START);
            break;
        }
        GCInterface::mTaskShowCardStatusMsg.SetState(TS_WAIT_FOR_USER_REPLY, TS_START);
        GCInterface::mUserMsg = UMSG_NONE;
        switch (static_cast<unsigned int>(GCInterface::mTaskShowCardStatusMsg.mCardStatus)) {
        case STATUS_OK:
        case STATUS_INSUFFICIENT_SPACE:
            GCInterface::mTaskShowCardStatusMsg.mTaskResult = RESULT_SUCCESS;
            GCInterface::mTaskShowCardStatusMsg.SetState(TS_DONE, TS_START);
            break;
        case STATUS_CARD_UNFORMATTED:
            GCInterface::mTaskShowCardStatusMsg.SetState(TS_FORMAT, TS_START);
            break;
        case STATUS_WRONG_DEVICE:
            GCInterface::mTaskMsg.LC_msg(0x0f,
                                         GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                   IO_CONTINUE | IO_RETRY,
                                                                   IO_NONE,
                                                                   IO_NONE),
                                         GCInterface::mTaskShowCardStatusMsg.mCardID.slot);
            break;
        case STATUS_NO_CARD:
            GCInterface::mTaskMsg.LC_msg(0x0e,
                                         GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                   IO_CONTINUE | IO_RETRY,
                                                                   IO_NONE,
                                                                   IO_NONE),
                                         GCInterface::mTaskShowCardStatusMsg.mCardID.slot);
            break;
        case STATUS_CARD_DAMAGED:
            GCInterface::mTaskMsg.LC_msg(0x11,
                                         GCMessage::PackMsgOptions(IO_FORMAT | IO_RETRY,
                                                                   IO_CONTINUE | IO_RETRY,
                                                                   IO_NONE,
                                                                   IO_NONE),
                                         GCInterface::mTaskShowCardStatusMsg.mCardID.slot);
            break;
        default:
            break;
        }
        break;
    case TS_WAIT_FOR_USER_REPLY:
        if (GCInterface::mUserMsg == UMSG_NONE) {
            if (GCInterface::mTaskShowCardStatusMsg.mCardStatus == STATUS_NO_CARD) {
                break;
            }
            if (GCInterface::mpDriver->CardExists(GCInterface::mTaskShowCardStatusMsg.mCardID) == CR_NOCARD) {
                GCInterface::mTaskShowCardStatusMsg.mTaskResult = RESULT_RETRY;
                GCInterface::mTaskShowCardStatusMsg.mCardStatus = STATUS_NO_CARD;
                GCInterface::mTaskShowCardStatusMsg.SetState(TS_DONE, TS_START);
            }
            break;
        }
        switch (GCInterface::ConvertUmsgToOption(GCInterface::mUserMsg,
                                                  5,
                                                  GCInterface::mTaskShowCardStatusMsg.mTrcTaskID)) {
        case IO_RETRY:
            GCInterface::mTaskShowCardStatusMsg.mTaskResult = RESULT_RETRY;
            break;
        case IO_CONTINUE:
            GCInterface::mTaskShowCardStatusMsg.mTaskResult = RESULT_CANCELLED;
            break;
        default:
            break;
        }
        GCInterface::mTaskShowCardStatusMsg.SetState(TS_DONE, TS_START);
        break;
    case TS_FORMAT:
        GCInterface::mTaskTrcFormat.Start(GCInterface::mTaskShowCardStatusMsg.mCardID,
                                           GCInterface::mTaskShowCardStatusMsg.mTrcTaskID,
                                           true);
        GCInterface::mTaskTrcFormat.mParent->StartTask(&GCInterface::mTaskTrcFormat);
        GCInterface::mTaskShowCardStatusMsg.SetState(TS_FORMAT_WAIT_FOR_RESULT, TS_START);
        break;
    case TS_FORMAT_WAIT_FOR_RESULT:
        GCInterface::mTaskShowCardStatusMsg.mTaskResult = GCInterface::mTaskTrcFormat.mTaskResult;
        GCInterface::mTaskShowCardStatusMsg.mCardStatus = GCInterface::mTaskTrcFormat.mCardStatus;
        GCInterface::mTaskShowCardStatusMsg.SetState(TS_DONE, TS_START);
        break;
    case TS_DONE:
        GCInterface::mTaskShowCardStatusMsg.End();
        break;
    default:
        break;
    }
}

void GCInterface::UpdateTaskTrcCheckSpace() {
    switch (GCInterface::mTaskTrcCheckSpace.GetState()) {
    case TS_START:
        if (GCInterface::mTaskTrcCheckSpace.mTaskID == TID_TRC_STARTGAME) {
            GCInterface::mTaskTrcCheckSpace.mBlocksNeeded = GCInterface::mTaskTrcStartGame.mInfo.totalBlocksNeeded;
            GCInterface::mTaskTrcCheckSpace.mFilesNeeded = GCInterface::mTaskTrcStartGame.mInfo.totalFilesNeeded;
        } else if (GCInterface::mTaskTrcCheckSpace.mTaskID == TID_TRC_SAVEFILE) {
            GCInterface::mTaskTrcCheckSpace.mBlocksNeeded = GCInterface::mpDriver->GetFileBlocks(
                GCInterface::mTaskTrcCheckSpace.mCardID,
                &GCInterface::mTaskTrcCheckSpace.mFileInfo,
                nullptr);
            GCInterface::mTaskTrcCheckSpace.mFilesNeeded = 1;
        }
        GCInterface::mTaskTrcCheckSpace.mTaskResult = RESULT_UNKNOWN;
        GCInterface::mTaskTrcCheckSpace.mCardStatus = STATUS_UNKNOWN;
        GCInterface::mTaskTrcCheckSpace.mSectorSize = GCInterface::mpDriver->GetSectorSize(
            GCInterface::mTaskTrcCheckSpace.mCardID);
        GCInterface::mTaskTrcCheckSpace.SetState(TS_CHECK_CARD, TS_START);
        break;
    case TS_CHECK_CARD:
        if (GCInterface::mpDriver->GetFreeCardSpace(GCInterface::mTaskTrcCheckSpace.mCardID,
                                                     reinterpret_cast<int *>(&GCInterface::mTaskTrcCheckSpace.mBlocksFree),
                                                     reinterpret_cast<int *>(&GCInterface::mTaskTrcCheckSpace.mFilesFree)) == CR_SUCCESS) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_FIRST, TS_START);
        } else {
            GCInterface::mTaskTrcCheckSpace.mTaskResult = RESULT_RETRY;
            GCInterface::mTaskTrcCheckSpace.mCardStatus = STATUS_FAILED;
            GCInterface::mTaskTrcCheckSpace.SetState(TS_DONE, TS_START);
            break;
        }
    case TS_FIND_FIRST:
        GCInterface::mTaskTrcCheckSpace.mFindResult = GCInterface::Find(
            GCInterface::mTaskTrcCheckSpace.mCardID,
            GCInterface::mTaskTrcCheckSpace.mFileInfo.fileName,
            true);
        if (GCInterface::mTaskTrcCheckSpace.mFindResult->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_CHECK_SPACE, TS_START);
        } else if (GCInterface::mpDriver->IsOurFile(
                       GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.companyCode,
                       GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.gameCode)) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_FILE_RESULT, TS_START);
        } else {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_NEXT, TS_START);
        }
        break;
    case TS_FIND_FILE_RESULT:
        if (strcmp(GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.fileName,
                   GCInterface::mTaskTrcCheckSpace.mFileInfo.fileName) == 0) {
            GCInterface::mTaskTrcCheckSpace.mFileExists = true;
        }
        if (GCInterface::mTaskTrcCheckSpace.mNotifyUser) {
            GCInterface::mTaskMsg.mMsg = LMSG_IS_IT_YOUR_FILE;
            GCInterface::mTaskMsg.info = GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info;
            GCInterface::mUserMsg = UMSG_NONE;
            GCInterface::mTaskTrcCheckSpace.SetState(TS_WAIT_FOR_USER_REPLY, TS_START);
        } else if (GCInterface::mTaskTrcCheckSpace.mFileExists) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_UPDATE_SPACE_USED, TS_START);
        } else {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_NEXT, TS_START);
        }
        break;
    case TS_WAIT_FOR_USER_REPLY:
        switch (GCInterface::mUserMsg) {
        case UMSG_YES:
            GCInterface::mTaskTrcCheckSpace.SetState(TS_UPDATE_SPACE_USED, TS_START);
            break;
        case UMSG_NO:
            {
                GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_NEXT, TS_START);
            }
            break;
        default:
            break;
        }
        break;
    case TS_UPDATE_SPACE_USED:
        GCInterface::mTaskTrcCheckSpace.mBlocksUsed +=
            (GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.fileSize +
             GCInterface::mTaskTrcCheckSpace.mSectorSize - 1) /
            GCInterface::mTaskTrcCheckSpace.mSectorSize;
        GCInterface::mTaskTrcCheckSpace.mFilesUsed++;
        GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_NEXT, TS_START);
        break;
    case TS_FIND_NEXT:
        GCInterface::mTaskTrcCheckSpace.mFindResult = GCInterface::Find(
            GCInterface::mTaskTrcCheckSpace.mCardID,
            GCInterface::mTaskTrcCheckSpace.mFileInfo.fileName,
            false);
        if (GCInterface::mTaskTrcCheckSpace.mFindResult->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_CHECK_SPACE, TS_START);
        } else if (GCInterface::mpDriver->IsOurFile(
                       GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.companyCode,
                       GCInterface::mTaskTrcCheckSpace.mFindResult->msg.info.fileInfo.gameCode)) {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_FILE_RESULT, TS_START);
        } else {
            GCInterface::mTaskTrcCheckSpace.SetState(TS_FIND_NEXT, TS_START);
        }
        break;
    case TS_CHECK_SPACE:
        if (GCInterface::mTaskTrcCheckSpace.mBlocksUsed >= GCInterface::mTaskTrcCheckSpace.mBlocksNeeded) {
            GCInterface::mTaskTrcCheckSpace.mBlocksNeeded = 0;
        } else {
            GCInterface::mTaskTrcCheckSpace.mBlocksNeeded -= GCInterface::mTaskTrcCheckSpace.mBlocksUsed;
        }
        if (GCInterface::mTaskTrcCheckSpace.mFilesUsed >= GCInterface::mTaskTrcCheckSpace.mFilesNeeded) {
            GCInterface::mTaskTrcCheckSpace.mFilesNeeded = 0;
        } else {
            GCInterface::mTaskTrcCheckSpace.mFilesNeeded -= GCInterface::mTaskTrcCheckSpace.mFilesUsed;
        }
        if (GCInterface::CheckCard(GCInterface::mTaskTrcCheckSpace.mCardID) == STATUS_NO_CARD) {
            GCInterface::mTaskTrcCheckSpace.mTaskResult = RESULT_RETRY;
            GCInterface::mTaskTrcCheckSpace.mCardStatus = STATUS_NO_CARD;
        } else if (GCInterface::mTaskTrcCheckSpace.mBlocksFree < GCInterface::mTaskTrcCheckSpace.mBlocksNeeded ||
                   GCInterface::mTaskTrcCheckSpace.mFilesFree < GCInterface::mTaskTrcCheckSpace.mFilesNeeded) {
            GCInterface::mTaskTrcCheckSpace.mTaskResult = RESULT_SUCCESS;
            GCInterface::mTaskTrcCheckSpace.mCardStatus = STATUS_INSUFFICIENT_SPACE;
        } else {
            GCInterface::mTaskTrcCheckSpace.mTaskResult = RESULT_SUCCESS;
            GCInterface::mTaskTrcCheckSpace.mCardStatus = STATUS_OK;
        }
        GCInterface::mTaskTrcCheckSpace.SetState(TS_DONE, TS_START);
        break;
    case TS_DONE:
        GCInterface::mTaskTrcCheckSpace.End();
        break;
    default:
        break;
    }
}

} // namespace Realmc
