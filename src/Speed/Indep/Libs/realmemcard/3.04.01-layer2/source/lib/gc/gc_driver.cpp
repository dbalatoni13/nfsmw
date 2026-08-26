#include <string.h>

#include <dolphin/card.h>
#include <dolphin/dvd.h>

#include "../../../include/common/realmemcard/gc_driver.h"

namespace Realmc {

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

bool GCDriver::IsOurFile(const char *fileCompanyName, const char *fileGameName) {
    return strncmp(fileGameName, this->mDiskIDGameName, 4) == 0 && strncmp(fileCompanyName, this->mDiskIDCompany, 2) == 0;
}

} // namespace Realmc
