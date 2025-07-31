#ifndef BWARE_SPEEDSCRIPT_H
#define BWARE_SPEEDSCRIPT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "bMath.hpp"

struct SpeedScriptFile {
    // total size: 0x54
    char Filename[80]; // offset 0x0, size 0x50
    char *ArgBuf;      // offset 0x50, size 0x4
};
struct SpeedScriptEntry {
    // total size: 0xC
    unsigned char FileNumber;         // offset 0x0, size 0x1
    unsigned char IsCommand;          // offset 0x1, size 0x1
    unsigned short NextCommandOffset; // offset 0x2, size 0x2
    unsigned int ArgBufPos;           // offset 0x4, size 0x4
    unsigned short LineNumber;        // offset 0x8, size 0x2
    unsigned short Pad;               // offset 0xA, size 0x2
};

struct SpeedScript {
    // total size: 0x268
    int NumFiles;                        // offset 0x0, size 0x4
    SpeedScriptFile FileTable[4];        // offset 0x4, size 0x150
    int NumEntries;                      // offset 0x154, size 0x4
    int MaxEntries;                      // offset 0x158, size 0x4
    SpeedScriptEntry *EntryTable;        // offset 0x15C, size 0x4
    int NextEntryNum;                    // offset 0x160, size 0x4
    char ErrorText[256];                 // offset 0x164, size 0x100
    void (*ErrorFunction)(const char *); // offset 0x264, size 0x4

    static void DefaultErrorFunction(const char *msg);

    SpeedScript(const char *filename, int enable_fatal_error);
    ~SpeedScript();
    void InitFromFile(const char *filename);
    void Error(const char *format, ...);
    char *GetPositionName();
    void ResizeEntryTable(int new_size);
    SpeedScriptEntry *AddEntry();
    bool ParseNextWord(char *word, const char *buffer, int buffer_size, int *pbuffer_pos, int *pline_number);
    void Init(const char *name, const char *buffer, int buffer_size);
    void HandleIncludeScript(const char *filename);
    char *GetNextCommand();
    char *GetNextCommand(const char *command);
    bool IsAnotherArgument();
    char *GetNextArgument();
    char *GetNextArgumentString();
    int GetNextArgumentInt();
    short GetNextArgumentShort();
    float GetNextArgumentFloat();
    bVector3 GetNextArgumentVector3();

    SpeedScriptEntry *GetCurrentEntry() {
        if ((this->NextEntryNum > 0) && (this->NextEntryNum <= this->NumEntries)) {
            return &this->EntryTable[this->NextEntryNum - 1];
        } else {
            return nullptr;
        }
    }

    SpeedScriptEntry *GetNextEntry() {
        if (this->NextEntryNum < this->NumEntries) {
            return &this->EntryTable[this->NextEntryNum];
        } else {
            return nullptr;
        }
    }

    inline char *GetName(struct SpeedScriptEntry *entry) {
        return &this->FileTable[entry->FileNumber].ArgBuf[entry->ArgBufPos];
    }
};

#endif
