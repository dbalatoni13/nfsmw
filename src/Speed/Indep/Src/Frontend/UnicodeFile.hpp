#ifndef FRONTEND_UNICODEFILE_H
#define FRONTEND_UNICODEFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

// total size: 0xC
struct UnicodeFile {
    short* data_;           // offset 0x0
    unsigned int numLines_; // offset 0x4
    unsigned int numChars_; // offset 0x8

    UnicodeFile();
    ~UnicodeFile();
    bool Load(const char* filename);
    void Unload();
    unsigned int GetNumLines() const;
    const short* GetLine(unsigned int index) const;
};

#endif
