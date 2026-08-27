#include <string.h>

#include <dolphin/card.h>
#include <dolphin/dvd.h>

#include "../../../include/common/realmemcard/gc_driver.h"

namespace Realmc {

extern IThread *gInterfaceThread;
extern const int MAX_COMMENT_LENGTH;

unsigned int GCDriver::MEMCARD_SECTOR_SIZE = 0x2000;
bool GCDriver::mMounted = false;
bool GCDriver::mIsCardPresent = false;
bool GCDriver::mWasCardPresent = false;

GCDriver::GCDriver(const SystemInterface *iSystem) {
    this->mAllocator = iSystem->mAllocator;
    this->mSectorSize = GCDriver::MEMCARD_SECTOR_SIZE;
    GCDriver::mMounted = false;
    GCDriver::mIsCardPresent = false;
    GCDriver::mWasCardPresent = false;

    this->mpWorkArea = this->mAllocator->Alloc(
        0xa000,
        EA::TagValuePair(EA::Allocator::ATT_NAME, "Memcard Work Area") +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, 0));
    memset(this->mpWorkArea, 0, 0xa000);

    this->mpIOBuffer = static_cast<char *>(this->mAllocator->Alloc(
        this->mSectorSize,
            EA::TagValuePair(EA::Allocator::ATT_NAME, "Memcard I/O Buffer") +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, 0x20)));
    memset(this->mpIOBuffer, 0, this->mSectorSize);
    CARDInit();

    DVDDiskID *dvdDiskID = DVDGetCurrentDiskID();
    strncpy(this->mDiskIDGameName, dvdDiskID->gameName, 4);
    strncpy(this->mDiskIDCompany, dvdDiskID->company, 2);
}

GCDriver::~GCDriver() {
    this->mAllocator->Free(this->mpWorkArea, 0);
    this->mAllocator->Free(this->mpIOBuffer, 0);
}

ICardResult GCDriver::ConvertCardResult(int gcCardResult) {
    switch (gcCardResult) {
    case CARD_RESULT_READY:
        return CR_SUCCESS;
    case CARD_RESULT_BUSY:
        return CR_BUSY;
    case CARD_RESULT_WRONGDEVICE:
        return CR_WRONGDEVICE;
    case CARD_RESULT_NOCARD:
        return CR_NOCARD;
    case CARD_RESULT_NOFILE:
        return CR_NOFILE;
    case CARD_RESULT_IOERROR:
        return CR_DAMAGED;
    case CARD_RESULT_BROKEN:
        return CR_CORRUPT;
    case CARD_RESULT_EXIST:
        return CR_FILE_EXIST;
    case CARD_RESULT_NOENT:
        return CR_ENTRY_FULL;
    case CARD_RESULT_INSSPACE:
        return CR_INSUFFICIENT_SPACE;
    case CARD_RESULT_NOPERM:
        return CR_NOPERM;
    case CARD_RESULT_LIMIT:
        return CR_RANGE_ERROR;
    case CARD_RESULT_NAMETOOLONG:
        return CR_NAMETOOLONG;
    case CARD_RESULT_ENCODING:
        return CR_ENCODINGERROR;
    case CARD_RESULT_CANCELED:
        return CR_CANCELLED;
    case CARD_RESULT_FATAL_ERROR:
        return CR_ERROR;
    default:
        return CR_OTHERERROR;
    }
}

GcFileDescriptor *GCDriver::FindFreeFileDescriptor() {
    GcFileDescriptor *fd = this->mFileDescriptors;
    for (int i = 0; i < 3; i++, fd++) {
        if (!fd->mOpen) {
            return fd;
        }
    }
    return 0;
}

GcFileDescriptor *GCDriver::ConvertFileHandleToDescriptor(OpenFileDescriptor *h) {
    GcFileDescriptor *fd = static_cast<GcFileDescriptor *>(h);
    return fd;
}

int GCDriver::GetBannerSize(const FileInfo *pFileInfo) {
    return pFileInfo->gcBannerDataInfo->imageFormat != GCIF_RGB5A3 ? 0xe00 : 0x1800;
}

int GCDriver::GetIconSize(const FileInfo *pFileInfo) {
    if (pFileInfo->gcIconDataInfo->imageFormat == GCIF_RGB5A3) {
        return pFileInfo->gcIconDataInfo->numIconFrames << 11;
    }

    return pFileInfo->gcIconDataInfo->numIconFrames * 3 << 9;
}

ICardResult GCDriver::CardExists(const CardID &cardID) {
    s32 memSize = 0;
    s32 sectorSize = 0;
    int rval;
    ICardResult res;

    do {
        rval = CARDProbeEx(cardID.slot, &memSize, &sectorSize);
        gInterfaceThread->Sleep(1);
    } while (rval == CARD_RESULT_BUSY);

    res = this->ConvertCardResult(rval);
    if (res == CR_SUCCESS && sectorSize != GCDriver::MEMCARD_SECTOR_SIZE) {
        res = CR_WRONGDEVICE;
    }

    return res;
}

unsigned int GCDriver::GetFileBlocks(const CardID &, const FileInfo *pFileInfo, unsigned int *iplDataSize) {
    unsigned int fileSize = MAX_COMMENT_LENGTH + MAX_COMMENT_LENGTH;

    fileSize += this->GetIconSize(pFileInfo);
    if (pFileInfo->gcBannerDataInfo != nullptr) {
        fileSize += this->GetBannerSize(pFileInfo);
    }

    if (iplDataSize != nullptr) {
        *iplDataSize = fileSize;
    }

    fileSize += 0x10;
    fileSize += pFileInfo->fileByteSize;
    return (fileSize + this->mSectorSize - 1) / this->mSectorSize;
}

unsigned int GCDriver::GetSectorSize(const CardID &cardID) {
    bool restorePrevMount;
    bool unmountAfterCheck;
    CardID prevMountedCardID = this->mMountedCardID;
    int mresult;
    u32 sectorSize;
    ICardResult res;

    restorePrevMount = false;
    unmountAfterCheck = false;

    if (this->mMounted) {
        if (cardID.slot != this->mMountedCardID.slot) {
            this->Unmount(this->mMountedCardID);
            restorePrevMount = true;
            this->Mount(cardID);
        }
    } else {
        unmountAfterCheck = true;
        this->Mount(cardID);
    }

    sectorSize = 0;
    do {
        mresult = CARDGetSectorSize(cardID.slot, &sectorSize);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    res = this->ConvertCardResult(mresult);
    if (res != CR_SUCCESS) {
        if (res == CR_NOCARD) {
            sectorSize = this->MEMCARD_SECTOR_SIZE;
        }
    }

    if (restorePrevMount) {
        this->Unmount(cardID);
        this->Mount(prevMountedCardID);
    } else if (unmountAfterCheck) {
        this->Unmount(cardID);
    }

    return sectorSize;
}

bool GCDriver::IsCardPresent() {
    return GCDriver::mIsCardPresent;
}

bool GCDriver::WasCardPresent() {
    return GCDriver::mWasCardPresent;
}

void GCDriver::ResetWasCardPresent() {
    GCDriver::mWasCardPresent = false;
}

void GCDriver::CardRemovalCallback(long, long) {
    GCDriver::mIsCardPresent = false;
}

ICardResult GCDriver::Mount(const CardID &cardID) {
    s32 size;
    s32 sectorSize;
    int cardOpResult;
    ICardResult res;

    GCDriver::mMounted = false;
    size = 0;
    sectorSize = 0;
    do {
        cardOpResult = CARDProbeEx(cardID.slot, &size, &sectorSize);
        gInterfaceThread->Sleep(1);
    } while (cardOpResult == CARD_RESULT_BUSY);

    res = this->ConvertCardResult(cardOpResult);
    if (res == CR_SUCCESS) {
        do {
            cardOpResult = CARDMount(cardID.slot, this->mpWorkArea, GCDriver::CardRemovalCallback);
            gInterfaceThread->Sleep(1);
        } while (cardOpResult == CARD_RESULT_BUSY);

        res = this->ConvertCardResult(cardOpResult);
        if (res == CR_SUCCESS || res == CR_ENCODINGERROR || res == CR_CORRUPT) {
            do {
                cardOpResult = CARDCheck(cardID.slot);
                gInterfaceThread->Sleep(1);
            } while (cardOpResult == CARD_RESULT_BUSY);

            res = this->ConvertCardResult(cardOpResult);

            switch (res) {
            case CR_SUCCESS:
            case CR_CORRUPT:
            case CR_ENCODINGERROR:
                GCDriver::mMounted = true;
                break;
            default:
                break;
            }

            if (res != CR_NOCARD) {
                GCDriver::mIsCardPresent = true;
            }

            if (GCDriver::mMounted) {
                this->mMountedCardID = cardID;
            }
        }
    }

    return res;
}

ICardResult GCDriver::Unmount(const CardID &cardID) {
    if (GCDriver::mMounted) {
        GCDriver::mMounted = false;
        GCDriver::mIsCardPresent = false;
        GCDriver::mWasCardPresent = true;

        int mresult;
        do {
            mresult = CARDUnmount(cardID.slot);
            gInterfaceThread->Sleep(1);
        } while (mresult == CARD_RESULT_BUSY);

        return this->ConvertCardResult(mresult);
    }

    return CR_SUCCESS;
}

ICardResult GCDriver::OpenFile(const CardID &cardID, const FileInfo *pFileInfo, FileOpenMode fmode, OpenFileDescriptor **handle) {
    GcFileDescriptor *fd;
    fd = this->FindFreeFileDescriptor();
    if (fd == nullptr) {
        return CR_ERROR;
    }

    *handle = nullptr;
    ICardResult result;
    int coResult;
    bool convertNGCerror;
    result = CR_ERROR;
    coResult = 0;
    convertNGCerror = true;
    fd->Init(cardID, *pFileInfo, fmode);

    if ((fmode & FOM_CREATE) != 0) {
        if (cardID.systemData == 1) {
            coResult = this->NGCSportsBioCreate(fd);
        } else {
            unsigned int iplDataSize;
            unsigned int fileBlockSize;
            iplDataSize = 0;
            fileBlockSize = this->GetFileBlocks(cardID, &fd->mFileInfo, &iplDataSize);
            fd->mFileHeader.Init(fd->mFileInfo.fileByteSize, iplDataSize);
            result = this->DeleteFile(cardID, pFileInfo);
            if (result != CR_DAMAGED) {
                unsigned long actualFileSize = fileBlockSize * this->mSectorSize;
                do {
                    coResult = CARDCreate(cardID.slot, pFileInfo->fileName, actualFileSize, &fd->mGcFileInfo);
                    gInterfaceThread->Sleep(1);
                } while (coResult == CARD_RESULT_BUSY);
            } else {
                coResult = CARD_RESULT_IOERROR;
            }
        }
    } else {
        int fileNum;
        fileNum = -1;
        do {
            if (cardID.systemData == 1) {
                result = this->FindFileNumber(const_cast<char *>(pFileInfo->fileName), &fileNum);
                if (result == CR_SUCCESS) {
                    coResult = CARDFastOpen(cardID.slot, fileNum, &fd->mGcFileInfo);
                } else {
                    convertNGCerror = false;
                }
            } else {
                coResult = CARDOpen(cardID.slot, pFileInfo->fileName, &fd->mGcFileInfo);
            }

            gInterfaceThread->Sleep(1);
        } while (coResult == CARD_RESULT_BUSY);
    }

    result = convertNGCerror ? this->ConvertCardResult(coResult) : result;

    if (result != CR_SUCCESS) {
        return result;
    }

    fd->mOpen = true;
    CARDGetStatus(cardID.slot, fd->mGcFileInfo.fileNo, &fd->mGcCardStat);
    *handle = fd;

    if (cardID.systemData == 1) {
        fd->mWriteSystemData = result;
        return CR_SUCCESS;
    }

    fd->mWriteSystemData = true;
    if ((fmode & FOM_CREATE) != 0) {
        if (this->WriteHeaderData(fd) != CR_SUCCESS) {
            result = CR_ERROR;
        }
    } else {
        result = this->ReadFile(fd, &fd->mFileHeader, 0x10, nullptr);
        if (result == CR_SUCCESS) {
            result = this->VerifyIplDataChecksum(fd);
        }
    }

    if (result != CR_SUCCESS) {
        CARDClose(&fd->mGcFileInfo);
        fd->mOpen = false;
        *handle = nullptr;
    }

    return result;
}

ICardResult GCDriver::WriteHeaderData(GcFileDescriptor *fd) {
    const FileInfo *pFileInfo = &fd->mFileInfo;
    ICardResult res;

    this->RecordIplDataChecksum(fd);
    res = CR_SUCCESS;
    if (this->WriteFile(fd, &fd->mFileHeader, 0x10, nullptr) == CR_ERROR) {
        res = CR_ERROR;
    }

    char comments[MAX_COMMENT_LENGTH];
    int writesize;

    memset(comments, 0, MAX_COMMENT_LENGTH);
    memcpy(comments, pFileInfo->comment1, pFileInfo->sizeofcomment1);
    if (this->WriteFile(fd, comments, MAX_COMMENT_LENGTH, nullptr) == CR_ERROR) {
        res = CR_ERROR;
    }

    memset(comments, 0, MAX_COMMENT_LENGTH);
    memcpy(comments, pFileInfo->comment2, pFileInfo->sizeofcomment2);
    if (this->WriteFile(fd, comments, MAX_COMMENT_LENGTH, nullptr) == CR_ERROR) {
        res = CR_ERROR;
    }

    if (pFileInfo->gcBannerDataInfo != nullptr) {
        writesize = this->GetBannerSize(pFileInfo);
        if (this->WriteFile(fd, pFileInfo->gcBannerDataInfo->imageData, writesize, nullptr) == CR_ERROR) {
            res = CR_ERROR;
        }
    }

    if (pFileInfo->gcIconDataInfo != nullptr) {
        writesize = this->GetIconSize(pFileInfo);
        if (this->WriteFile(fd, pFileInfo->gcIconDataInfo->imageData, writesize, nullptr) == CR_ERROR) {
            res = CR_ERROR;
        }
    }

    return res;
}

ICardResult GCDriver::WriteFile(OpenFileDescriptor *handle, void *pBuffer, int bufferSize, int *nBytesWritten) {
    ICardResult result;
    GcFileDescriptor *fd;
    char *pWriteBuffer;

    fd = this->ConvertFileHandleToDescriptor(handle);

    if (!fd->mWriteSystemData) {
        return this->NGCSportsBioWrite(fd, pBuffer, bufferSize, nBytesWritten);
    }

    pWriteBuffer = static_cast<char *>(pBuffer);

    if (nBytesWritten != nullptr) {
        *nBytesWritten = bufferSize;
    }

    while (bufferSize > 0) {
        while (fd->mBufferOffset < this->mSectorSize && bufferSize > 0) {
            this->mpIOBuffer[fd->mBufferOffset++] = *pWriteBuffer++;
            bufferSize--;
            fd->mNeedFlush = true;
        }

        if (fd->mBufferOffset == this->mSectorSize && fd->mFileSectorOffset >= 0) {
            int mresult;
            unsigned int fileSize;

            do {
                mresult = CARDWrite(&fd->mGcFileInfo, this->mpIOBuffer, this->mSectorSize, fd->mFileSectorOffset);
                gInterfaceThread->Sleep(1);
            } while (mresult == CARD_RESULT_BUSY);

            if ((result = this->ConvertCardResult(mresult)) != CR_SUCCESS) {
                return result;
            }

            fd->mNeedFlush = result;
            fd->mFileSectorOffset += this->mSectorSize;
            fd->mBufferOffset = result;

            fileSize = this->GetFileBlocks(fd->mCardID, &fd->mFileInfo, nullptr) * this->mSectorSize;
            if (fileSize < static_cast<unsigned int>(fd->mFileSectorOffset)) {
                do {
                    mresult = CARDRead(&fd->mGcFileInfo, this->mpIOBuffer, this->mSectorSize, fd->mFileSectorOffset);
                    gInterfaceThread->Sleep(1);
                } while (mresult == CARD_RESULT_BUSY);

                if ((result = this->ConvertCardResult(mresult)) == CR_SUCCESS) {
                    fd->mFileSectorOffset += this->mSectorSize;
                } else {
                    return result;
                }
            } else {
                memset(this->mpIOBuffer, 0, this->mSectorSize);
            }
        }
    }

    return CR_SUCCESS;
}

ICardResult GCDriver::FlushWriteBuffer(OpenFileDescriptor *handle) {
    ICardResult result = CR_SUCCESS;
    GcFileDescriptor *fd = this->ConvertFileHandleToDescriptor(handle);

    if (fd->mBufferOffset > 0) {
        if (static_cast<long>(fd->mFileHeader.mFileSize) <= fd->mFileSectorOffset - this->mSectorSize + fd->mBufferOffset) {
            fd->mBufferOffset = fd->mFileHeader.mFileSize % this->mSectorSize;
            while (fd->mBufferOffset < this->mSectorSize) {
                this->mpIOBuffer[fd->mBufferOffset] = 0;
                fd->mBufferOffset++;
            }
        }

        int mresult;
        do {
            mresult = CARDWrite(&fd->mGcFileInfo, this->mpIOBuffer, this->mSectorSize, fd->mFileSectorOffset);
            gInterfaceThread->Sleep(1);
        } while (mresult == CARD_RESULT_BUSY);

        result = this->ConvertCardResult(mresult);
    }

    fd->mNeedFlush = false;
    return result;
}

int GCDriver::SetHeaderInfo(GcFileDescriptor *fd) {
    int numicon;
    int mresult;
    ICardResult result;

    memset(&fd->mGcCardStat, 0, sizeof(fd->mGcCardStat));
    fd->mGcCardStat.commentAddr = 0x10;
    fd->mGcCardStat.iconAddr = MAX_COMMENT_LENGTH + MAX_COMMENT_LENGTH + 0x10;

    if (fd->mFileInfo.gcBannerDataInfo != nullptr) {
        if (fd->mFileInfo.gcBannerDataInfo->imageFormat == GCIF_RGB5A3) {
            fd->mGcCardStat.bannerFormat = (fd->mGcCardStat.bannerFormat & 0xfc) | 2;
        } else {
            fd->mGcCardStat.bannerFormat = (fd->mGcCardStat.bannerFormat & 0xfc) | 1;
        }
    } else {
        fd->mGcCardStat.bannerFormat = (fd->mGcCardStat.bannerFormat >> 2) << 2;
    }

    numicon = fd->mFileInfo.gcIconDataInfo->numIconFrames;
    if (numicon <= 7) {
        fd->mGcCardStat.iconFormat &= ~(3 << (numicon * 2));
        fd->mGcCardStat.iconSpeed &= ~(3 << (numicon * 2));
    }

    if (fd->mFileInfo.gcIconDataInfo->animationLoop == GCIL_BACK_AND_FORTH) {
        fd->mGcCardStat.bannerFormat |= 4;
    } else if (fd->mFileInfo.gcIconDataInfo->animationLoop == GCIL_REPEAT) {
        fd->mGcCardStat.bannerFormat &= ~4;
    }

    for (int i = 0; i < numicon; i++) {
        if (fd->mFileInfo.gcIconDataInfo->imageFormat == GCIF_RGB5A3) {
            fd->mGcCardStat.iconFormat = (fd->mGcCardStat.iconFormat & ~(3 << (i * 2))) | (2 << (i * 2));
        } else {
            fd->mGcCardStat.iconFormat = (fd->mGcCardStat.iconFormat & ~(3 << (i * 2))) | (1 << (i * 2));
        }

        switch (fd->mFileInfo.gcIconDataInfo->animationPause[i]) {
        case GCIS_12FRAMES:
            fd->mGcCardStat.iconSpeed |= 3 << (i * 2);
            break;
        case GCIS_8FRAMES:
            fd->mGcCardStat.iconSpeed = (fd->mGcCardStat.iconSpeed & ~(3 << (i * 2))) | (2 << (i * 2));
            break;
        case GCIS_4FRAMES:
            fd->mGcCardStat.iconSpeed = (fd->mGcCardStat.iconSpeed & ~(3 << (i * 2))) | (1 << (i * 2));
            break;
        default:
            fd->mGcCardStat.iconSpeed = (fd->mGcCardStat.iconSpeed & ~(3 << (i * 2))) | (2 << (i * 2));
            break;
        }
    }

    do {
        mresult = CARDSetStatus(fd->mCardID.slot, fd->fileNumber, &fd->mGcCardStat);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    result = this->ConvertCardResult(mresult);
    if (result != CR_SUCCESS) {
        return -1;
    }
    return 0;
}

ICardResult GCDriver::ReadFile(OpenFileDescriptor *handle, void *pBuffer, int bytesToRead, int *nBytesRead) {
    GcFileDescriptor *fd = this->ConvertFileHandleToDescriptor(handle);
    ICardResult result;
    int mresult;
    char *pReadBuffer;
    int bytesLeft;
    long fileSize;
    long currentSectorHead;

    if (!fd->mWriteSystemData) {
        return this->NGCSportsBioRead(fd, pBuffer, bytesToRead, nBytesRead);
    }

    pReadBuffer = static_cast<char *>(pBuffer);
    result = CR_SUCCESS;
    bytesLeft = bytesToRead;

    if (fd->mNeedFlush) {
        this->FlushWriteBuffer(handle);
    }

    fileSize = fd->mGcCardStat.length;
    if (fd->mFileSectorOffset != 0) {
        currentSectorHead = fd->mFileSectorOffset - this->mSectorSize;
    } else {
        currentSectorHead = 0;
    }

    do {
        if (bytesLeft == 0) {
            break;
        }

        long readHead;

        if (fd->readNextSector) {
            do {
                mresult = CARDRead(&fd->mGcFileInfo, this->mpIOBuffer, this->mSectorSize, fd->mFileSectorOffset);
                gInterfaceThread->Sleep(1);
            } while (mresult == CARD_RESULT_BUSY);

            result = this->ConvertCardResult(mresult);
            if (result != CR_SUCCESS) {
                break;
            }

            fd->readNextSector = result;
            fd->mFileSectorOffset += this->mSectorSize;
        }

        while (fd->mBufferOffset < this->mSectorSize && bytesLeft != 0) {
            if (pReadBuffer != nullptr) {
                *pReadBuffer = this->mpIOBuffer[fd->mBufferOffset];
                pReadBuffer++;
            }
            bytesLeft--;
            fd->mBufferOffset++;
        }

        if (fd->mBufferOffset == this->mSectorSize) {
            fd->readNextSector = true;
            fd->mBufferOffset = 0;
        }

        if (bytesLeft != 0) {
            readHead = currentSectorHead + fd->mBufferOffset;
            if (readHead < 0 || readHead >= fileSize) {
                result = CR_RANGE_ERROR;
                break;
            }
        }
    } while (bytesLeft != 0);

    if (nBytesRead != nullptr) {
        *nBytesRead = bytesToRead - bytesLeft;
    }

    return result;
}

ICardResult GCDriver::CloseFile(OpenFileDescriptor *handle) {
    GcFileDescriptor *fd = this->ConvertFileHandleToDescriptor(handle);
    ICardResult res;

    if (!fd->mWriteSystemData) {
        res = this->NGCSportsBioClose(fd);
    } else {
        int mresult;
        if ((fd->mMode & FOM_CREATE) != 0) {
            this->FlushWriteBuffer(handle);
            do {
                mresult = CARDClose(&fd->mGcFileInfo);
                gInterfaceThread->Sleep(1);
            } while (mresult == CARD_RESULT_BUSY);

            res = this->ConvertCardResult(mresult);
            if (res == CR_SUCCESS) {
                if (this->SetHeaderInfo(fd) == -1) {
                    res = CR_ERROR;
                }
            }
        } else {
            do {
                mresult = CARDClose(&fd->mGcFileInfo);
                gInterfaceThread->Sleep(1);
            } while (mresult == CARD_RESULT_BUSY);

            res = this->ConvertCardResult(mresult);
        }
    }

    fd->mOpen = false;
    return res;
}

void GCDriver::RecordIplDataChecksum(GcFileDescriptor *fd) {
    const FileInfo *pFileInfo = &fd->mFileInfo;
    int checksum = 0;
    int iByte;
    char *curByte;
    int dataSize;

    dataSize = pFileInfo->sizeofcomment1;
    curByte = pFileInfo->comment1;
    iByte = 0;
    for (; iByte < dataSize; iByte++) {
        checksum += *curByte++;
    }

    dataSize = pFileInfo->sizeofcomment2;
    curByte = pFileInfo->comment2;
    iByte = 0;
    for (; iByte < dataSize; iByte++) {
        checksum += *curByte++;
    }

    if (pFileInfo->gcBannerDataInfo != nullptr) {
        dataSize = this->GetBannerSize(pFileInfo);
        curByte = pFileInfo->gcBannerDataInfo->imageData;
        iByte = 0;
        for (; iByte < dataSize; iByte++) {
            checksum += *curByte++;
        }
    }

    if (pFileInfo->gcIconDataInfo != nullptr) {
        dataSize = this->GetIconSize(pFileInfo);
        curByte = pFileInfo->gcIconDataInfo->imageData;
        iByte = 0;
        for (; iByte < dataSize; iByte++) {
            checksum += *curByte++;
        }
    }

    fd->mFileHeader.mIplDataChecksum = checksum;
}

ICardResult GCDriver::VerifyIplDataChecksum(GcFileDescriptor *fd) {
    int bytesLeftToRead = fd->mFileHeader.mIplDataSize;
    int readBlockSize = GCDriver::MEMCARD_SECTOR_SIZE - fd->mBufferOffset;
    int readStartPosition = fd->mBufferOffset;
    int checksum = 0;
    ICardResult result = CR_CORRUPT;

    do {
        if (bytesLeftToRead <= 0) {
            break;
        }

        if (bytesLeftToRead < GCDriver::MEMCARD_SECTOR_SIZE) {
            readBlockSize = bytesLeftToRead;
        }

        result = this->ReadFile(fd, nullptr, readBlockSize, nullptr);
        if (result == CR_SUCCESS) {
            int readEndPosition = readStartPosition + readBlockSize;
            bytesLeftToRead -= readBlockSize;
            if (readStartPosition < readEndPosition) {
                for (int iByte = readStartPosition; iByte < readEndPosition; iByte++) {
                    checksum += this->mpIOBuffer[iByte];
                }
            }

            readBlockSize = GCDriver::MEMCARD_SECTOR_SIZE;
            readStartPosition = 0;
        } else {
            break;
        }
    } while (bytesLeftToRead > 0);

    if (result == CR_SUCCESS) {
        result = CR_CORRUPT;
        if (checksum == fd->mFileHeader.mIplDataChecksum) {
            result = CR_SUCCESS;
        }
    }

    return result;
}

int GCDriver::GetOpenFileSize(OpenFileDescriptor *handle) {
    GcFileDescriptor *fd = this->ConvertFileHandleToDescriptor(handle);
    return fd->mFileInfo.fileByteSize;
}

ICardResult GCDriver::GetFreeCardSpace(const CardID &cardID, int *spacefree, int *filesfree) {
    ICardResult res = CR_ERROR;
    s32 getbytesFree = 0;
    s32 getfilesFree = 0;

    if (CARDFreeBlocks(cardID.slot, &getbytesFree, &getfilesFree) == CARD_RESULT_READY) {
        res = CR_SUCCESS;
    }

    *spacefree = getbytesFree / this->mSectorSize;
    *filesfree = getfilesFree;
    return res;
}

ICardResult GCDriver::FormatCard(const CardID &cardID) {
    int mresult;

    do {
        mresult = CARDFormat(cardID.slot);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    return this->ConvertCardResult(mresult);
}

ICardResult GCDriver::FindFirst(const CardID &cardID, FindInfoStruct *pFindInfo) {
    this->mpFindInfo = pFindInfo;
    pFindInfo->Clear();
    this->mpFindInfo->cardID = cardID;
    return this->FindNext();
}

ICardResult GCDriver::FindNext() {
    while (this->mpFindInfo->fileID <= 0x7e) {
        int mresult;
        static CARDStat statInfo;

        do {
            mresult = CARDGetStatus(this->mpFindInfo->cardID.slot, this->mpFindInfo->fileID, &statInfo);
            gInterfaceThread->Sleep(1);
        } while (mresult == CARD_RESULT_BUSY);

        this->mpFindInfo->fileID++;
        if (this->ConvertCardResult(mresult) != CR_SUCCESS) {
            if (this->mpFindInfo->fileID <= 0x7e) {
                continue;
            }
        } else {
            this->mpFindInfo->fileName = statInfo.fileName;
            this->mpFindInfo->fileSize = statInfo.length;
            this->mpFindInfo->userDataOffset = statInfo.offsetData;
            this->mpFindInfo->time = statInfo.time;
            memcpy(this->mpFindInfo->gameName, statInfo.gameName, 4);
            memcpy(this->mpFindInfo->company, statInfo.company, 2);
            return CR_SUCCESS;
        }
    }

    return CR_NOFILE;
}

ICardResult GCDriver::Seek(OpenFileDescriptor *fileHandle, int offset, SeekFrom seekFrom) {
    GcFileDescriptor *fd = this->ConvertFileHandleToDescriptor(fileHandle);

    if (!fd->mWriteSystemData) {
        return this->NGCSportsBioSeek(fd, offset, seekFrom);
    }

    if (fd->mNeedFlush) {
        this->FlushWriteBuffer(fileHandle);
    }

    switch (seekFrom) {
    case SF_SET:
        offset += GetUserDataOffset(fd);
        break;
    case SF_CUR:
        offset += fd->mFileSectorOffset - this->mSectorSize + fd->mBufferOffset;
        break;
    case SF_END:
        offset = fd->mFileHeader.mFileSize - offset;
        break;
    default:
        break;
    }

    if (offset < 0 || offset >= static_cast<int>(fd->mGcCardStat.length)) {
        return CR_RANGE_ERROR;
    }

    fd->mFileSectorOffset = offset / this->mSectorSize * this->mSectorSize;
    fd->mBufferOffset = offset % this->mSectorSize;
    fd->readNextSector = true;
    return CR_SUCCESS;
}

ICardResult GCDriver::SetAttributes(CardID cardID, char *fileName, FileAttribute attr) {
    int fileNum = -1;
    ICardResult result = this->FindFileNumber(fileName, &fileNum);

    if (result == CR_SUCCESS) {
        int mresult;
        do {
            mresult = CARDSetAttributes(cardID.slot, fileNum, attr);
        } while (mresult == CARD_RESULT_BUSY);

        result = this->ConvertCardResult(mresult);
    }

    return result;
}

ICardResult GCDriver::FindFileNumber(char *fileName, int *fileNum) {
    *fileNum = -1;

    for (int i = 0; i <= 0x7e; i++) {
        CARDStat status;
        int res;
        ICardResult result;

        do {
            res = CARDGetStatus(0, i, &status);
            gInterfaceThread->Sleep(1);
        } while (res == CARD_RESULT_BUSY);

        result = this->ConvertCardResult(res);
        switch (result) {
        case CR_NOFILE:
            break;
        case CR_SUCCESS:
            if (strcmp(status.fileName, fileName) == 0) {
                *fileNum = i;
                return CR_SUCCESS;
            }
            break;
        default:
            return result;
        case CR_NOPERM:
            break;
        }
    }

    return CR_NOFILE;
}

ICardResult GCDriver::DeleteFile(const CardID &cardID, const FileInfo *pFileInfo) {
    ICardResult result;

    do {
        if (cardID.systemData == 1) {
            int fileNum = -1;
            result = this->FindFileNumber(const_cast<char *>(pFileInfo->fileName), &fileNum);
            if (result == CR_SUCCESS) {
                result = this->ConvertCardResult(CARDFastDelete(cardID.slot, fileNum));
            }
        } else {
            result = this->ConvertCardResult(CARDDelete(cardID.slot, pFileInfo->fileName));
        }

        gInterfaceThread->Sleep(1);
    } while (result == CR_BUSY);

    return result;
}

ICardResult GCDriver::NGCSportsBioSeek(GcFileDescriptor *fd, int offset, SeekFrom seekFrom) {
    switch (seekFrom) {
    case SF_SET:
        fd->mFileSectorOffset = offset;
        break;
    case SF_CUR:
        fd->mFileSectorOffset += offset;
        break;
    default:
        break;
    }

    if (fd->mFileSectorOffset < 0 || fd->mFileSectorOffset >= static_cast<int>(fd->mGcCardStat.length)) {
        return CR_RANGE_ERROR;
    }
    return CR_SUCCESS;
}

ICardResult GCDriver::NGCSportsBioRead(GcFileDescriptor *fd, void *pBuffer, int bufferSize, int *nBytesRead) {
    int mresult;
    ICardResult result;

    do {
        mresult = CARDRead(&fd->mGcFileInfo, pBuffer, bufferSize, fd->mFileSectorOffset);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    switch (result = this->ConvertCardResult(mresult)) {
    case CR_SUCCESS:
        *nBytesRead = bufferSize;
        fd->mFileSectorOffset += bufferSize;
        return result;
    case CR_RANGE_ERROR:
        *nBytesRead = 0;
        result = CR_SUCCESS;
        break;
    default:
        *nBytesRead = bufferSize;
        result = CR_ERROR;
        break;
    }

    return result;
}

ICardResult GCDriver::NGCSportsBioWrite(GcFileDescriptor *fd, void *pBuffer, int bufferSize, int *nBytesWritten) {
    int mresult;
    ICardResult result;

    do {
        mresult = CARDWrite(&fd->mGcFileInfo, pBuffer, bufferSize, fd->mFileSectorOffset);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    result = this->ConvertCardResult(mresult);
    switch (result) {
    case CR_SUCCESS:
        *nBytesWritten = bufferSize;
        fd->mFileSectorOffset += bufferSize;
        break;
    case CR_RANGE_ERROR:
        *nBytesWritten = 0;
        result = CR_SUCCESS;
        break;
    case CR_NOPERM:
        *nBytesWritten = -1;
        result = CR_SUCCESS;
        break;
    default:
        *nBytesWritten = 0;
        break;
    }

    return result;
}

ICardResult GCDriver::NGCSportsBioClose(GcFileDescriptor *fd) {
    int mresult;
    ICardResult result;

    do {
        mresult = CARDClose(&fd->mGcFileInfo);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    result = this->ConvertCardResult(mresult);
    if (result == CR_SUCCESS) {
        fd->mOpen = result;
    }

    return result;
}

int GCDriver::NGCSportsBioCreate(GcFileDescriptor *fd) {
    int mresult;

    do {
        mresult = CARDCreate(fd->mCardID.slot, fd->mFileInfo.fileName, fd->mFileInfo.fileByteSize,
                             &fd->mGcFileInfo);
        gInterfaceThread->Sleep(1);
    } while (mresult == CARD_RESULT_BUSY);

    switch (mresult) {
    case CARD_RESULT_READY:
        fd->mWriteSystemData = false;
        fd->mErrorCode = 0;
        break;
    case CARD_RESULT_FATAL_ERROR:
        fd->mErrorCode = -1;
        break;
    case CARD_RESULT_NOCARD:
        fd->mErrorCode = -2;
        break;
    case CARD_RESULT_BUSY:
        fd->mErrorCode = -3;
        break;
    case CARD_RESULT_EXIST:
        fd->mErrorCode = -4;
        break;
    case CARD_RESULT_NOENT:
        fd->mErrorCode = -5;
        break;
    case CARD_RESULT_INSSPACE:
        fd->mErrorCode = -6;
        break;
    case CARD_RESULT_NAMETOOLONG:
        fd->mErrorCode = -7;
        break;
    case CARD_RESULT_IOERROR:
        fd->mErrorCode = -8;
        break;
    default:
        fd->mErrorCode = -10;
        break;
    }

    return mresult;
}

bool GCDriver::IsOurFile(const char *fileCompanyName, const char *fileGameName) {
    return strncmp(fileGameName, this->mDiskIDGameName, 4) == 0 && strncmp(fileCompanyName, this->mDiskIDCompany, 2) == 0;
}

} // namespace Realmc
