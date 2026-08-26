#ifndef REALMEMCARD_GC_DRIVER_H
#define REALMEMCARD_GC_DRIVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <dolphin/card.h>

#include "driver.h"

namespace Realmc {

enum ICardResult {
    CR_SUCCESS = 0,
    CR_BUSY = 1,
    CR_WRONGDEVICE = 2,
    CR_NOCARD = 3,
    CR_NOFILE = 4,
    CR_DAMAGED = 5,
    CR_CORRUPT = 6,
    CR_FILE_EXIST = 7,
    CR_ENTRY_FULL = 8,
    CR_INSUFFICIENT_SPACE = 9,
    CR_NOPERM = 10,
    CR_RANGE_ERROR = 11,
    CR_NAMETOOLONG = 12,
    CR_ENCODINGERROR = 13,
    CR_CANCELLED = 14,
    CR_ERROR = 15,
    CR_OTHERERROR = 16,
};

enum FileAttribute {
    FA_NONE = 0,
    FA_PUBLIC = 4,
    FA_COPY_PROTECTED = 8,
    FA_NO_MOVE = 16,
    FA_COMPANY = 64,
};

struct GcFileDescriptor : public CmnFileDescriptor {
    GcFileDescriptor() {
        this->mFileHeader.Clear();
        this->Clear();
    }

    inline void Clear() override;
    inline void Init(const CardID &cardID, const FileInfo &fileInfo, FileOpenMode mode) override;

    CARDFileInfo mGcFileInfo;
    CARDStat mGcCardStat;
    long mBufferOffset;
    long mFileSectorOffset;
    GcFileHeader mFileHeader;
    int mNeedFlush;
    int mErrorCode;
    bool readNextSector;
    bool mWriteSystemData;
};

inline void GcFileHeader::Clear() {
    this->mFileSize = 0;
    this->mUserDataSize = 0;
    this->mIplDataSize = 0;
    this->mIplDataChecksum = 0;
}

inline int GetUserDataOffset(GcFileDescriptor *fd) {
    return fd->mFileHeader.mIplDataSize + 0x10;
}

struct GCDriver : public DeviceDriver {
    static inline void *operator new(unsigned int size);
    static inline void operator delete(void *ptr, unsigned int size);
    static inline void *operator new[](unsigned int size);
    static inline void operator delete[](void *ptr, unsigned int size);
    static inline void *operator new(unsigned int, void *ptr);
    static inline void operator delete(void *, void *);
    static inline void *operator new[](unsigned int, void *ptr);
    static inline void operator delete[](void *, void *);

    GCDriver(const SystemInterface *iSystem);
    virtual ~GCDriver() override;

    ICardResult ConvertCardResult(int gcCardResult);
    GcFileDescriptor *FindFreeFileDescriptor();
    GcFileDescriptor *ConvertFileHandleToDescriptor(OpenFileDescriptor *h);
    ICardResult CardExists(const CardID &cardID);
    ICardResult GetAdditionalSpaceNeeded(const CardID &cardID, const FileInfo *pFileInfo, int *spaceneeded, int *filesneeded);
    unsigned int GetFileBlocks(const CardID &cardID, const FileInfo *pFileInfo, unsigned int *iplDataSize);
    unsigned int GetSectorSize(const CardID &cardID);
    unsigned int GetSectors(const CardID &cardID, unsigned int byteSize);
    static void CardRemovalCallback(long, long);
    ICardResult Mount(const CardID &cardID);
    ICardResult Unmount(const CardID &cardID);
    ICardResult OpenFile(const CardID &cardID, const FileInfo *pFileInfo, FileOpenMode fmode, OpenFileDescriptor **handle);
    ICardResult WriteHeaderData(GcFileDescriptor *fd);
    int GetBannerSize(const FileInfo *pFileInfo);
    int GetIconSize(const FileInfo *pFileInfo);
    ICardResult WriteFile(OpenFileDescriptor *handle, void *pBuffer, int bufferSize, int *nBytesWritten);
    ICardResult FlushWriteBuffer(OpenFileDescriptor *handle);
    int SetHeaderInfo(GcFileDescriptor *fd);
    ICardResult ReadFile(OpenFileDescriptor *handle, void *pBuffer, int bytesToRead, int *nBytesRead);
    ICardResult CloseFile(OpenFileDescriptor *handle);
    int GetOpenFileSize(OpenFileDescriptor *handle);
    ICardResult GetFreeCardSpace(const CardID &cardID, int *spacefree, int *filesfree);
    ICardResult DeleteFile(const CardID &cardID, const FileInfo *pFileInfo);
    ICardResult RenameFile(const CardID &cardID, const FileInfo *pFileInfo, const char *pNewFileName);
    ICardResult FormatCard(const CardID &cardID);
    bool IsCardPresent();
    bool WasCardPresent();
    void ResetWasCardPresent();
    bool IsOurFile(const char *fileCompanyName, const char *fileGameName);
    ICardResult FindFirst(const CardID &cardID, FindInfoStruct *pFindInfo);
    ICardResult FindNext();
    ICardResult Seek(OpenFileDescriptor *fileHandle, int offset, SeekFrom seekFrom);
    ICardResult SetAttributes(CardID cardID, char *fileName, FileAttribute attr);
    ICardResult FindFileNumber(char *fileName, int *fileNum);
    ICardResult NGCSportsBioSeek(GcFileDescriptor *fd, int offset, SeekFrom seekFrom);
    ICardResult NGCSportsBioRead(GcFileDescriptor *fd, void *pBuffer, int bufferSize, int *nBytesRead);
    ICardResult NGCSportsBioWrite(GcFileDescriptor *fd, void *pBuffer, int bufferSize, int *nBytesWritten);
    ICardResult NGCSportsBioClose(GcFileDescriptor *fd);
    int NGCSportsBioCreate(GcFileDescriptor *fd);
    void RecordIplDataChecksum(GcFileDescriptor *fd);
    ICardResult VerifyIplDataChecksum(GcFileDescriptor *fd);

    static unsigned int MEMCARD_SECTOR_SIZE;
    static bool mIsCardPresent;
    static bool mWasCardPresent;
    static bool mMounted;

    FindInfoStruct *mpFindInfo;
    GcFileDescriptor mFileDescriptors[3];
    int mSectorSize;
    EA::Allocator::IAllocator *mAllocator;
    void *mpWorkArea;
    char *mpIOBuffer;
    CardID mMountedCardID;
    char mDiskIDGameName[4];
    char mDiskIDCompany[2];
};

inline void *GCDriver::operator new(unsigned int size) {
    return AllocateMemSize(0, size, 0, 0, 0);
}

inline void GCDriver::operator delete(void *ptr, unsigned int size) {
    FreeMemSize(ptr, static_cast<int>(size));
}

inline void *GCDriver::operator new[](unsigned int size) {
    return AllocateMemSize(0, size, 0, 0, 0);
}

inline void GCDriver::operator delete[](void *ptr, unsigned int size) {
    FreeMemSize(ptr, static_cast<int>(size));
}

inline void *GCDriver::operator new(unsigned int, void *ptr) {
    return ptr;
}

inline void GCDriver::operator delete(void *, void *) {}

inline void *GCDriver::operator new[](unsigned int, void *ptr) {
    return ptr;
}

inline void GCDriver::operator delete[](void *, void *) {}

inline void GcFileDescriptor::Clear() {
    this->CmnFileDescriptor::Clear();
    this->mFileHeader.Clear();
    this->fileNumber = 0;
    memset(&this->mGcFileInfo, 0, sizeof(this->mGcFileInfo));
    this->mBufferOffset = 0;
    this->mFileSectorOffset = 0;
    this->mNeedFlush = 0;
    this->mWriteSystemData = false;
    this->mErrorCode = 0;
    this->readNextSector = true;
}

inline void GcFileDescriptor::Init(const CardID &cardID, const FileInfo &fileInfo, FileOpenMode mode) {
    this->Clear();
    this->CmnFileDescriptor::Init(cardID, fileInfo, mode);
}


} // namespace Realmc

#endif
