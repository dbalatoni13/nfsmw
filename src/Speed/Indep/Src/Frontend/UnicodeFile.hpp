#ifndef FRONTEND_UNICODEFILE_H
#define FRONTEND_UNICODEFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class UnicodeFile {
  public:
    UnicodeFile();
    UnicodeFile(const char *filename);
    ~UnicodeFile();
    bool Load(const char *filename);
    void Unload();
    void LineWrap(int maxCharacters);
    i16 *First();
    i16 *Next();

  private:
    void FixEndian();
    void FixEOLs();

    i16 *data_; // offset 0x0, size 0x4
    i16 *next_; // offset 0x4, size 0x4
    i16 *end_;  // offset 0x8, size 0x4
};

#endif
