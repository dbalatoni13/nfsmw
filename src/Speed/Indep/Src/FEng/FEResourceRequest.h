#ifndef FENG_FERESOURCEREQUEST_H
#define FENG_FERESOURCEREQUEST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x18
struct FEResourceRequest {
    unsigned long ID;      // offset 0x0, size 0x4
    const char* pFilename; // offset 0x4, size 0x4
    unsigned long Type;    // offset 0x8, size 0x4
    unsigned long Flags;   // offset 0xC, size 0x4
    unsigned long Handle;  // offset 0x10, size 0x4
    unsigned long UserParam; // offset 0x14, size 0x4
};

#endif
