#define REALMC_GC_MESSAGE_INLINE
#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#undef REALMC_GC_MESSAGE_INLINE
#include "../../../include/common/realmemcard/gc_driver.h"
#include "../../../include/common/realmemcard/memcard_utilities.h"

namespace Realmc {

void GCInterface::UpdateTaskCardExists() {
    GCInterface::mTaskCardExists.mTaskResult = RESULT_SUCCESS;
    switch (GCInterface::mpDriver->CardExists(GCInterface::mTaskCardExists.mCardID)) {
    case CR_SUCCESS:
        GCInterface::mTaskCardExists.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskCardExists.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_WRONGDEVICE:
        GCInterface::mTaskCardExists.mCardStatus = STATUS_WRONG_DEVICE;
        break;
    default:
        GCInterface::mTaskCardExists.mTaskResult = RESULT_FAILED;
        GCInterface::mTaskCardExists.mCardStatus = STATUS_FAILED;
        break;
    }

    if (GCInterface::mTaskCardExists.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_CARD_EXISTS_DONE, GCInterface::mTaskCardExists.mTaskResult, GCInterface::mTaskCardExists.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.cardID = GCInterface::mTaskCardExists.mCardID;
    }
    GCInterface::mTaskCardExists.End();
}

void GCInterface::UpdateTaskGetCardInfo() {
    int freeSpace;
    int freeFiles;

    freeSpace = 0;
    freeFiles = 0;
    GCInterface::mTaskGetCardInfo.mTaskResult = RESULT_SUCCESS;
    GCInterface::mTaskGetCardInfo.mCardStatus = STATUS_FAILED;
    if (GCInterface::mpDriver->GetFreeCardSpace(GCInterface::mTaskGetCardInfo.mCardID, &freeSpace, &freeFiles) == CR_SUCCESS) {
        GCInterface::mTaskGetCardInfo.mCardStatus = STATUS_OK;
    }
    if (GCInterface::mTaskGetCardInfo.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_CARD_INFO_DONE, GCInterface::mTaskGetCardInfo.mTaskResult, GCInterface::mTaskGetCardInfo.mCardStatus);
        GCInterface::mTaskMsg.info.cardInfo.freeSpace = freeSpace;
        GCInterface::mTaskMsg.info.cardInfo.freeFiles = freeFiles;
    }
    GCInterface::mTaskGetCardInfo.End();
}

void GCInterface::UpdateTaskMount() {
    GCInterface::mTaskMount.mTaskResult = RESULT_FAILED;
    switch (GCInterface::mpDriver->Mount(GCInterface::mTaskMount.mCardID)) {
    case CR_SUCCESS:
        GCInterface::mTaskMount.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskMount.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskMount.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_CORRUPT:
    case CR_ENCODINGERROR:
        GCInterface::mTaskMount.mCardStatus = STATUS_CARD_UNFORMATTED;
        break;
    case CR_OTHERERROR:
        GCInterface::mTaskMount.mCardStatus = STATUS_CARD_DAMAGED;
        break;
    case CR_WRONGDEVICE:
        GCInterface::mTaskMount.mCardStatus = STATUS_WRONG_DEVICE;
        break;
    default:
        GCInterface::mTaskMount.mCardStatus = STATUS_FAILED;
        break;
    }
    GCInterface::mTaskMount.End();
    if (GCInterface::mTaskMount.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_MOUNT_DONE, GCInterface::mTaskMount.mTaskResult, GCInterface::mTaskMount.mCardStatus);
    }
}

void GCInterface::UpdateTaskUnmount() {
    GCInterface::mTaskUnmount.mTaskResult = RESULT_FAILED;
    switch (GCInterface::mpDriver->Unmount(GCInterface::mTaskUnmount.mCardID)) {
    case CR_SUCCESS:
        GCInterface::mTaskUnmount.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskUnmount.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskUnmount.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_ERROR:
        GCInterface::mTaskUnmount.mCardStatus = STATUS_FAILED;
        break;
    default:
        break;
    }
    GCInterface::mTaskUnmount.End();
    if (GCInterface::mTaskUnmount.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_UNMOUNT_DONE, GCInterface::mTaskUnmount.mTaskResult, GCInterface::mTaskUnmount.mCardStatus);
    }
}

void GCInterface::UpdateTaskOpen() {
    OpenFileDescriptor *handle;

    GCInterface::mTaskOpen.mTaskResult = RESULT_FAILED;
    GCInterface::mTaskOpen.mCardStatus = STATUS_UNKNOWN;
    handle = nullptr;
    switch (GCInterface::mpDriver->OpenFile(GCInterface::mTaskOpen.mCardID, GCInterface::mTaskOpen.mFileInfo, GCInterface::mTaskOpen.mMode, &handle)) {
    case CR_SUCCESS:
        GCInterface::mTaskOpen.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskOpen.mCardStatus = STATUS_OK;
        break;
    case CR_NOFILE:
        GCInterface::mTaskOpen.mCardStatus = STATUS_ENTRY_NOT_FOUND;
        break;
    case CR_CORRUPT:
        GCInterface::mTaskOpen.mCardStatus = STATUS_FILE_CORRUPTED;
        break;
    case CR_DAMAGED:
        GCInterface::mTaskOpen.mCardStatus = STATUS_CARD_DAMAGED;
        break;
    case CR_NOCARD:
        GCInterface::mTaskOpen.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_ERROR:
        GCInterface::mTaskOpen.mCardStatus = STATUS_FAILED;
        break;
    default:
        break;
    }
    GCInterface::mTaskOpen.End();
    if (GCInterface::mTaskOpen.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_OPEN_FILE_DONE, GCInterface::mTaskOpen.mTaskResult, GCInterface::mTaskOpen.mCardStatus);
        GCInterface::mTaskMsg.info.openResult.fileHandle = handle;
        GCInterface::mTaskMsg.info.openResult.fileName = GCInterface::mTaskOpen.mFileInfo->fileName;
    }
}

void GCInterface::UpdateTaskClose() {
    GCInterface::mTaskOpen.mCardStatus = STATUS_UNKNOWN;
    GCInterface::mTaskOpen.mTaskResult = RESULT_FAILED;
    switch (GCInterface::mpDriver->CloseFile(GCInterface::mTaskClose.mFileHandle)) {
    case CR_SUCCESS:
        GCInterface::mTaskClose.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskClose.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskClose.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_ERROR:
        GCInterface::mTaskClose.mCardStatus = STATUS_FAILED;
        break;
    default:
        break;
    }
    GCInterface::mTaskClose.End();
    if (GCInterface::mTaskClose.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_CLOSE_FILE_DONE, GCInterface::mTaskClose.mTaskResult, GCInterface::mTaskClose.mCardStatus);
    }
}

void GCInterface::UpdateTaskRead() {
    int bytesRead;
    ICardResult result;

    GCInterface::mTaskRead.mTaskResult = RESULT_FAILED;
    GCInterface::mTaskRead.mCardStatus = STATUS_UNKNOWN;
    bytesRead = 0;
    result = GCInterface::mpDriver->ReadFile(GCInterface::mTaskRead.mFileHandle, GCInterface::mTaskRead.mBuffer, GCInterface::mTaskRead.mBufferSize, &bytesRead);
    switch (result) {
    case CR_SUCCESS:
        GCInterface::mTaskRead.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskRead.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskRead.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_NOFILE:
        GCInterface::mTaskRead.mCardStatus = STATUS_FILE_NOT_FOUND;
        break;
    case CR_ENTRY_FULL:
        GCInterface::mTaskRead.mCardStatus = STATUS_CARD_FULL;
        break;
    case CR_INSUFFICIENT_SPACE:
        GCInterface::mTaskRead.mCardStatus = STATUS_INSUFFICIENT_SPACE;
        break;
    case CR_NOPERM:
        GCInterface::mTaskRead.mCardStatus = STATUS_ACCESS_DENIED;
        break;
    case CR_RANGE_ERROR:
        GCInterface::mTaskRead.mCardStatus = STATUS_RANGE_ERROR;
        break;
    default:
        GCInterface::mTaskRead.mCardStatus = STATUS_FAILED;
        break;
    }
    if (GCInterface::mTaskRead.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_READ_DONE, GCInterface::mTaskRead.mTaskResult, GCInterface::mTaskRead.mCardStatus);
        GCInterface::mTaskMsg.info.readResult.bytesRead = bytesRead;
    }
    GCInterface::mTaskRead.End();
}

void GCInterface::UpdateTaskWrite() {
    int bytesWritten;
    ICardResult result;

    GCInterface::mTaskWrite.mTaskResult = RESULT_FAILED;
    GCInterface::mTaskWrite.mCardStatus = STATUS_UNKNOWN;
    bytesWritten = 0;
    result = GCInterface::mpDriver->WriteFile(GCInterface::mTaskWrite.mFileHandle, GCInterface::mTaskWrite.mBuffer, GCInterface::mTaskWrite.mBufferSize, &bytesWritten);
    switch (result) {
    case CR_SUCCESS:
        GCInterface::mTaskWrite.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskWrite.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskWrite.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_NOFILE:
        GCInterface::mTaskWrite.mCardStatus = STATUS_FILE_NOT_FOUND;
        break;
    case CR_ENTRY_FULL:
        GCInterface::mTaskWrite.mCardStatus = STATUS_CARD_FULL;
        break;
    case CR_NOPERM:
        GCInterface::mTaskWrite.mCardStatus = STATUS_ACCESS_DENIED;
        break;
    case CR_RANGE_ERROR:
        GCInterface::mTaskWrite.mCardStatus = STATUS_RANGE_ERROR;
        break;
    default:
        GCInterface::mTaskWrite.mCardStatus = STATUS_FAILED;
        break;
    }
    if (GCInterface::mTaskWrite.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_WRITE_DONE, GCInterface::mTaskWrite.mTaskResult, GCInterface::mTaskWrite.mCardStatus);
        GCInterface::mTaskMsg.info.writeResult.bytesWritten = bytesWritten;
    }
    GCInterface::mTaskWrite.End();
}

void GCInterface::UpdateTaskSeek() {
    GCInterface::mTaskSeek.mTaskResult = RESULT_FAILED;
    GCInterface::mTaskSeek.mCardStatus = STATUS_UNKNOWN;
    switch (GCInterface::mpDriver->Seek(GCInterface::mTaskSeek.mFileHandle, GCInterface::mTaskSeek.mOffset, GCInterface::mTaskSeek.mSeekFrom)) {
    case CR_SUCCESS:
        GCInterface::mTaskSeek.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskSeek.mCardStatus = STATUS_OK;
        break;
    case CR_RANGE_ERROR:
        GCInterface::mTaskSeek.mCardStatus = STATUS_RANGE_ERROR;
        break;
    default:
        break;
    }
    if (GCInterface::mTaskSeek.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_SEEK_DONE, GCInterface::mTaskSeek.mTaskResult, GCInterface::mTaskSeek.mCardStatus);
    }
    GCInterface::mTaskSeek.End();
}

void GCInterface::UpdateTaskFlush() {
    if (GCInterface::mpDriver->FlushWriteBuffer(GCInterface::mTaskFlush.mFileHandle) == CR_SUCCESS) {
        GCInterface::mTaskFlush.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskFlush.mCardStatus = STATUS_OK;
    } else {
        GCInterface::mTaskFlush.mTaskResult = RESULT_FAILED;
        GCInterface::mTaskFlush.mCardStatus = STATUS_FAILED;
    }
    GCInterface::mTaskFlush.End();
    if (GCInterface::mTaskFlush.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_FLUSH_DONE, GCInterface::mTaskFlush.mTaskResult, GCInterface::mTaskFlush.mCardStatus);
    }
}

void GCInterface::UpdateTaskDelete() {
    FileInfo fileInfo;

    memset(&fileInfo, 0, sizeof(fileInfo));
    fileInfo.fileName = GCInterface::mTaskDelete.mFileName;
    GCInterface::mTaskDelete.mCardStatus = STATUS_UNKNOWN;
    GCInterface::mTaskDelete.mTaskResult = RESULT_FAILED;
    switch (GCInterface::mpDriver->DeleteFile(GCInterface::mTaskDelete.mCardID, &fileInfo)) {
    case CR_SUCCESS:
        GCInterface::mTaskDelete.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskDelete.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskDelete.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_NOPERM:
        GCInterface::mTaskDelete.mCardStatus = STATUS_ACCESS_DENIED;
        break;
    case CR_DAMAGED:
        GCInterface::mTaskDelete.mCardStatus = STATUS_CARD_DAMAGED;
        break;
    case CR_NOFILE:
        GCInterface::mTaskDelete.mCardStatus = STATUS_ENTRY_NOT_FOUND;
        break;
    default:
        GCInterface::mTaskDelete.mCardStatus = STATUS_FAILED;
        break;
    }
    GCInterface::mTaskDelete.End();
    if (GCInterface::mTaskDelete.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_DELETE_FILE_DONE, GCInterface::mTaskDelete.mTaskResult, GCInterface::mTaskDelete.mCardStatus);
    }
}

void GCInterface::UpdateTaskSetAttribute() {
    GCInterface::mTaskSetAttribute.mTaskResult = RESULT_FAILED;
    GCInterface::mTaskSetAttribute.mCardStatus = STATUS_UNKNOWN;
    switch (GCInterface::mpDriver->SetAttributes(GCInterface::mTaskSetAttribute.mCardID, GCInterface::mTaskSetAttribute.mFileName, GCInterface::mTaskSetAttribute.mAttribute)) {
    case CR_SUCCESS:
        GCInterface::mTaskSetAttribute.mTaskResult = RESULT_SUCCESS;
        GCInterface::mTaskSetAttribute.mCardStatus = STATUS_OK;
        break;
    case CR_NOCARD:
        GCInterface::mTaskSetAttribute.mCardStatus = STATUS_NO_CARD;
        break;
    case CR_NOFILE:
        GCInterface::mTaskSetAttribute.mCardStatus = STATUS_ENTRY_NOT_FOUND;
        break;
    case CR_NOPERM:
        GCInterface::mTaskSetAttribute.mCardStatus = STATUS_ACCESS_DENIED;
        break;
    default:
        GCInterface::mTaskSetAttribute.mCardStatus = STATUS_FAILED;
        break;
    }
    if (GCInterface::mTaskSetAttribute.mNotifyUser) {
        GCInterface::mTaskMsg.Set(LMSG_SET_ATTRIBUTE_DONE, GCInterface::mTaskSetAttribute.mTaskResult, GCInterface::mTaskSetAttribute.mCardStatus);
    }
    GCInterface::mTaskSetAttribute.End();
}

void GCInterface::UpdateTaskFindFile() {
    switch (GCInterface::mTaskFind.GetState()) {
    case TS_START:
        GCInterface::mTaskFind.SetState(TS_FIND_FIRST, TS_START);
    case TS_FIND_FIRST: {
        FindResult *result;

        result = GCInterface::Find(GCInterface::mTaskFind.mCardID, GCInterface::mTaskFind.mFileName, true);
        if (result->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
            GCInterface::mTaskFind.SetState(TS_DONE, TS_START);
        } else {
            ++GCInterface::mTaskFind.mNumFilesFound;
            if (GCInterface::mTaskFind.mNotifyUser) {
                GCInterface::mTaskMsg = result->msg;
            }
            GCInterface::mTaskFind.SetState(TS_FIND_NEXT, TS_START);
        }
        break;
    }
    case TS_FIND_NEXT: {
        FindResult *result;

        result = GCInterface::Find(GCInterface::mTaskFind.mCardID, GCInterface::mTaskFind.mFileName, false);
        if (result->msg.mCardStatus == STATUS_ENTRY_NOT_FOUND) {
            GCInterface::mTaskFind.SetState(TS_DONE, TS_START);
        } else {
            ++GCInterface::mTaskFind.mNumFilesFound;
            if (GCInterface::mTaskFind.mNotifyUser) {
                GCInterface::mTaskMsg = result->msg;
            }
        }
        break;
    }
    case TS_DONE:
        GCInterface::mTaskFind.mTaskResult = RESULT_SUCCESS;
        if (GCInterface::mTaskFind.mNumFilesFound > 0) {
            GCInterface::mTaskFind.mCardStatus = STATUS_OK;
        } else {
            GCInterface::mTaskFind.mCardStatus = STATUS_ENTRY_NOT_FOUND;
        }
        GCInterface::mTaskFind.End();
        if (GCInterface::mTaskFind.mNotifyUser) {
            GCInterface::mTaskMsg.Set(LMSG_FIND_FILE_DONE, GCInterface::mTaskFind.mTaskResult, GCInterface::mTaskFind.mCardStatus);
        }
        break;
    default:
        break;
    }
}

FindResult *GCInterface::Find(const CardID &cardID, const char *fileName, bool findFirst) {
    ICardResult result;

    GCInterface::mFindResult.msg.Clear();
    memset(GCInterface::mFindResult.fileName, 0, sizeof(GCInterface::mFindResult.fileName));
    GCInterface::mFindResult.msg.Set(LMSG_FILE_INFO, RESULT_SUCCESS, STATUS_ENTRY_NOT_FOUND);

    static FindInfoStruct findFileInfo;
    bool found;
    if (findFirst) {
        findFileInfo.Clear();
        result = GCInterface::mpDriver->FindFirst(cardID, &findFileInfo);
    } else {
        result = GCInterface::mpDriver->FindNext();
    }
    found = false;
    if (result != CR_SUCCESS) {
        return &GCInterface::mFindResult;
    }
    do {
        if (RealmcUtils::Wildcard(findFileInfo.fileName, const_cast<char *>(fileName))) {
            found = true;
            GCInterface::mFindResult.msg.info.fileInfo.fileSize = findFileInfo.fileSize;
            GCInterface::mFindResult.msg.info.fileInfo.userDataOffset = findFileInfo.userDataOffset;
            GCInterface::mFindResult.msg.info.fileInfo.fileTime = findFileInfo.time;
            memcpy(GCInterface::mFindResult.msg.info.fileInfo.companyCode, findFileInfo.company, 2);
            memcpy(GCInterface::mFindResult.msg.info.fileInfo.gameCode, findFileInfo.gameName, 4);
            GCInterface::mFindResult.msg.mCardStatus = STATUS_OK;
            GCInterface::mFindResult.msg.info.fileInfo.fileName = findFileInfo.fileName;
            strcpy(GCInterface::mFindResult.fileName, findFileInfo.fileName);
        } else {
            result = GCInterface::mpDriver->FindNext();
        }
    } while (result == CR_SUCCESS && !found);
    return &GCInterface::mFindResult;
}

} // namespace Realmc
