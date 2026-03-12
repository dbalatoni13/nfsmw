#ifndef MISC_STOMPER_H
#define MISC_STOMPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
struct StompEntry {
    int type;     // offset 0x0, size 0x4
    int address;  // offset 0x4, size 0x4
    int reserved; // offset 0x8, size 0x4

    void *Clear(void *pStack);
    void *ClearAll(void *pStack);
};

// total size: 0xC
struct Stomper {
    int StomperType;  // offset 0x0, size 0x4
    void *pData;      // offset 0x4, size 0x4
    int CurrentTable; // offset 0x8, size 0x4

    Stomper();
    void Clear();
};

void InitStomper();
void ResetRenderEggs();
void ActivateAnyRenderEggs();

#endif
