#ifndef REALMEMCARD_DRIVER_H
#define REALMEMCARD_DRIVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <string.h>

#include "impl/memcard_interface_impl.h"

namespace Realmc {

enum FileOpenMode {
    FOM_UNKNOWN = 0,
    FOM_READ = 1,
    FOM_WRITE = 2,
    FOM_READ_WRITE = 3,
    FOM_CREATE = 512,
};

struct DeviceDriver {
    static inline void *operator new(unsigned int size) {
        return AllocateMemSize(0, size, 0, 0, 0);
    }

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    static inline void *operator new[](unsigned int size) {
        return AllocateMemSize(0, size, 0, 0, 0);
    }

    static inline void operator delete[](void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    static inline void *operator new(unsigned int, void *ptr) {
        return ptr;
    }

    static inline void operator delete(void *, void *) {}

    static inline void *operator new[](unsigned int, void *ptr) {
        return ptr;
    }

    static inline void operator delete[](void *, void *) {}

    inline DeviceDriver();
    inline virtual ~DeviceDriver() {}
};

struct CmnFileDescriptor : public OpenFileDescriptor {
    CmnFileDescriptor() {
        this->mSignature = 0x524d4306;
        this->Clear();
    }

    inline virtual void Clear();
    inline virtual void Init(const CardID &cardID, const FileInfo &fileInfo, FileOpenMode mode);
    bool CheckSignature();

    unsigned int mSignature;
    CardID mCardID;
    FileInfo mFileInfo;
    FileOpenMode mMode;
    bool mOpen;
};

struct FindInfoStruct {
    FindInfoStruct() {}
    void Clear() {
        this->fileName = nullptr;
        this->fileSize = 0;
        this->userDataOffset = 0;
        this->time = 0;
        this->fileID = 0;
        memset(this->gameName, 0, 4);
        memset(this->company, 0, 2);
    }

    CardID cardID;
    char *fileName;
    int fileSize;
    unsigned int userDataOffset;
    int time;
    int fileID;
    unsigned char gameName[4];
    unsigned char company[2];
};

struct GcFileHeader {
    GcFileHeader() {}
    inline void Clear();
    inline void Init(unsigned int userDataSize, unsigned int iplDataSize);

    unsigned int mFileSize;
    unsigned int mUserDataSize;
    unsigned int mIplDataSize;
    int mIplDataChecksum;
};

} // namespace Realmc

inline Realmc::DeviceDriver::DeviceDriver() {}

inline void Realmc::CmnFileDescriptor::Clear() {
    memset(&this->mFileInfo, 0, sizeof(this->mFileInfo));
    this->mMode = FOM_READ;
    this->mOpen = false;
}

inline void Realmc::CmnFileDescriptor::Init(const CardID &cardID, const FileInfo &fileInfo, FileOpenMode mode) {
    this->mCardID = cardID;
    this->mFileInfo = fileInfo;
    this->mMode = mode;
    this->mOpen = false;
}

inline void Realmc::GcFileHeader::Init(unsigned int userDataSize, unsigned int iplDataSize) {
    this->mFileSize = userDataSize + 0x10 + iplDataSize;
    this->mUserDataSize = userDataSize;
    this->mIplDataSize = iplDataSize;
    this->mIplDataChecksum = 0;
}

#endif
