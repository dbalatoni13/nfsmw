#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#include "../../../include/common/realmemcard/impl/interfaceimp.h"

namespace Realmc {

extern "C" char lbl_80414790[];

extern wchar_t sCardName[48] asm("_6Realmc.sCardName");

void GCInterface::TrcStartGame(const StartGameInfo &info) {
    this->CheckMessageCompatibility();
    this->mMutex->Lock();
    mTaskTrcStartGame.Start(&info);
    mTaskTrcStartGame.mParent->StartTask(&mTaskTrcStartGame);
    this->mMutex->Unlock();
}

void GCInterface::TrcCardExists(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskTrcCardExists.Start(cardID);
    GCInterface::mTaskTrcCardExists.mParent->StartTask(&GCInterface::mTaskTrcCardExists);
    this->mMutex->Unlock();
}

void GCInterface::TrcGetCardInfo(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskTrcGetCardInfo.Start(cardID);
    GCInterface::mTaskTrcGetCardInfo.mParent->StartTask(&GCInterface::mTaskTrcGetCardInfo);
    this->mMutex->Unlock();
}

void GCInterface::TrcLoadFile(const CardID &cardID, const FileInfo &fileInfo) {
    this->mMutex->Lock();
    mTaskTrcLoadFile.Start(cardID, &fileInfo);
    mTaskTrcLoadFile.mParent->StartTask(&mTaskTrcLoadFile);
    this->mMutex->Unlock();
}

void GCInterface::TrcSaveFile(const CardID &cardID, const FileInfo &fileInfo, SaveTaskType saveTaskType,
                              unsigned int nBlocksNeeded, unsigned int nFilesNeeded) {
    this->mMutex->Lock();
    mTaskTrcSaveFile.Start(cardID, &fileInfo, saveTaskType, nBlocksNeeded, nFilesNeeded);
    mTaskTrcSaveFile.mParent->StartTask(&mTaskTrcSaveFile);
    this->mMutex->Unlock();
}

void GCInterface::TrcListFiles(const CardID &cardID, const FileInfo &fileInfo, ListTaskType) {
    this->mMutex->Lock();
    GCInterface::mTaskTrcListFiles.Start(cardID, &fileInfo);
    GCInterface::mTaskTrcListFiles.mParent->StartTask(&GCInterface::mTaskTrcListFiles);
    this->mMutex->Unlock();
}

void GCInterface::TrcDeleteFile(const CardID &cardID, const FileInfo &fileInfo) {
    this->mMutex->Lock();
    GCInterface::mTaskTrcDeleteFile.Start(cardID, &fileInfo);
    GCInterface::mTaskTrcDeleteFile.mParent->StartTask(&GCInterface::mTaskTrcDeleteFile);
    this->mMutex->Unlock();
}

const unsigned short *GCInterface::GetCardName(const CardID &cardID) {
    return GCInterface::GetCardName(cardID, sCardName, 0x60);
}

const unsigned short *GCInterface::GetCardName(const CardID &cardID, wchar_t *namebuf, int buflen) {
    unsigned short slot[2];
    const unsigned short *p;
    int len;

    static_cast<GCInterface *>(gInterface)->LockInterfaceMutex();
    memset(namebuf, 0, buflen * 2);
    *reinterpret_cast<unsigned int *>(slot) = 0;
    slot[0] = 'A';
    slot[0] = cardID.slot + slot[0];
    p = reinterpret_cast<const unsigned short *>(Locale::GetString(0x28, lbl_80414790, slot));
    len = Locale::GetWstrLength(reinterpret_cast<const wchar_t *>(p)) * 2;
    if (len > buflen) {
        len = buflen;
    }
    memcpy(namebuf, p, len);
    static_cast<GCInterface *>(gInterface)->UnlockInterfaceMutex();
    return reinterpret_cast<const unsigned short *>(namebuf);
}

void GCInterface::CardExists(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskCardExists.Start(cardID, true);
    GCInterface::mTaskCardExists.mParent->StartTask(&GCInterface::mTaskCardExists);
    this->mMutex->Unlock();
}

void GCInterface::GetCardInfo(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskGetCardInfo.Start(cardID, true);
    GCInterface::mTaskGetCardInfo.mParent->StartTask(&GCInterface::mTaskGetCardInfo);
    this->mMutex->Unlock();
}

void GCInterface::Mount(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskMount.Start(cardID, true);
    GCInterface::mTaskMount.mParent->StartTask(&GCInterface::mTaskMount);
    this->mMutex->Unlock();
}

void GCInterface::Unmount(const CardID &cardID) {
    this->mMutex->Lock();
    GCInterface::mTaskUnmount.Start(cardID, true);
    GCInterface::mTaskUnmount.mParent->StartTask(&GCInterface::mTaskUnmount);
    this->mMutex->Unlock();
}

void GCInterface::OpenFile(const CardID &cardID, const FileInfo &fileInfo, FileOpenMode mode) {
    this->mMutex->Lock();
    GCInterface::mTaskOpen.Start(cardID, &fileInfo, mode, true);
    GCInterface::mTaskOpen.mParent->StartTask(&GCInterface::mTaskOpen);
    this->mMutex->Unlock();
}

void GCInterface::CloseFile(OpenFileDescriptor *fileHandle) {
    this->mMutex->Lock();
    GCInterface::mTaskClose.Start(fileHandle, true);
    GCInterface::mTaskClose.mParent->StartTask(&GCInterface::mTaskClose);
    this->mMutex->Unlock();
}

void GCInterface::Read(OpenFileDescriptor *fileHandle, void *buffer, int bufferSize) {
    this->mMutex->Lock();
    GCInterface::mTaskRead.Start(fileHandle, buffer, bufferSize, true);
    GCInterface::mTaskRead.mParent->StartTask(&GCInterface::mTaskRead);
    this->mMutex->Unlock();
}

void GCInterface::Write(OpenFileDescriptor *fileHandle, void *buffer, int bufferSize) {
    this->mMutex->Lock();
    GCInterface::mTaskWrite.Start(fileHandle, buffer, bufferSize, true);
    GCInterface::mTaskWrite.mParent->StartTask(&GCInterface::mTaskWrite);
    this->mMutex->Unlock();
}

void GCInterface::Seek(OpenFileDescriptor *fileHandle, int offset, SeekFrom seekFrom) {
    this->mMutex->Lock();
    GCInterface::mTaskSeek.Start(fileHandle, offset, seekFrom, true);
    GCInterface::mTaskSeek.mParent->StartTask(&GCInterface::mTaskSeek);
    this->mMutex->Unlock();
}

void GCInterface::Flush(OpenFileDescriptor *fileHandle) {
    this->mMutex->Lock();
    GCInterface::mTaskFlush.Start(fileHandle, true);
    GCInterface::mTaskFlush.mParent->StartTask(&GCInterface::mTaskFlush);
    this->mMutex->Unlock();
}

void GCInterface::DeleteFile(const CardID &cardID, const char *, const char *fileName) {
    this->mMutex->Lock();
    GCInterface::mTaskDelete.Start(cardID, fileName, true);
    GCInterface::mTaskDelete.mParent->StartTask(&GCInterface::mTaskDelete);
    this->mMutex->Unlock();
}

void GCInterface::SetFileAttribute(const CardID &cardID, const char *, const char *fileName, FileAttribute attribute) {
    this->mMutex->Lock();
    GCInterface::mTaskSetAttribute.Start(cardID, fileName, attribute, true);
    GCInterface::mTaskSetAttribute.mParent->StartTask(&GCInterface::mTaskSetAttribute);
    this->mMutex->Unlock();
}

void GCInterface::FindFile(const CardID &cardID, const char *, const char *fileName) {
    this->mMutex->Lock();
    GCInterface::mTaskFind.Start(cardID, fileName, true);
    GCInterface::mTaskFind.mParent->StartTask(&GCInterface::mTaskFind);
    this->mMutex->Unlock();
}

} // namespace Realmc

asm(".section .bss, \"wa\", @nobits\n"
    ".balign 8\n"
    ".globl _6Realmc.sCardName\n"
    "_6Realmc.sCardName:\n"
    ".skip 0x60\n"
    ".globl findFileInfo.635_804D9648\n"
    "findFileInfo.635_804D9648:\n"
    ".skip 0x20\n"
    ".previous");
