#ifndef FRONTEND_UNICODEFILE_H
#define FRONTEND_UNICODEFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
class UnicodeFile {
  public:
    UnicodeFile();
    UnicodeFile(const char *filename) { Load(filename); }
    ~UnicodeFile();

    bool Load(const char *filename);
    void Unload();
    short *First();
    short *Next();
    void FixEndian();
    void FixEOLs();
    void LineWrap(int maxCharacters);

    short *data_; // offset 0x0, size 0x4
    short *next_; // offset 0x4, size 0x4
    short *end_;  // offset 0x8, size 0x4
};

#endif
