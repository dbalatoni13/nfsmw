#ifndef MISC_STOMPER_H
#define MISC_STOMPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
struct StompEntry {
    void *Set(void *p);
    void *Clear(void *p);
    void *SetAll(void *pStack);
    void *ClearAll(void *pStack);

    int nType; // offset 0x0, size 0x4
    union {
        void *pData;                      // offset 0x0, size 0x4
        int (*pFuncInt)(bool, int);       // offset 0x0, size 0x4
        float (*pFuncFloat)(bool, float); // offset 0x0, size 0x4
    }; // offset 0x4, size 0x4
    union {
        int nData;   // offset 0x0, size 0x4
        float fData; // offset 0x0, size 0x4
    }; // offset 0x8, size 0x4
};

enum eStomper {
    eSTOMPER_NONE = 0,
    eSTOMPER_CARTOON = 1,
    eSTOMPER_WIRE_ORANGE = 2,
    eSTOMPER_NEGATIVE_BLUE_FOG = 3,
    eSTOMPER_NEGATIVE_CREAM_FOG = 4,
    eSTOMPER_FLAT_ORANGE = 5,
    eSTOMPER_BLUE_NIGHT = 6,
    eSTOMPER_HOT_MAGENTA = 7,
    eSTOMPER_MOTION_BLUR = 8,
    eSTOMPER_BLACK_AND_WHITE = 9,
    eSTOMPER_COOL_FOG = 10,
    eSTOMPER_CUTIE_PETOOTIE = 11,
    eSTOMPER_CHROME_CARS = 12,
};

// total size: 0xC
class Stomper {
  public:
    Stomper();
    void Clear();

  private:
    bool bEnabled;     // offset 0x0, size 0x1
    char *pStack;      // offset 0x4, size 0x4
    eStomper nCurrent; // offset 0x8, size 0x4
};

void InitStomper();
void ResetRenderEggs();
void ActivateAnyRenderEggs();

#endif
