#ifndef GAMEPLAY_GSPEEDTRAP_H
#define GAMEPLAY_GSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x14
class GSpeedTrap {
  private:
    unsigned short mFlags;         // offset 0x0, size 0x2
    unsigned short mBinNumber;     // offset 0x2, size 0x2
    unsigned int mSpeedTrapKey;    // offset 0x4, size 0x4
    unsigned int mCameraMarkerKey; // offset 0x8, size 0x4
    float mRequiredValue;          // offset 0xC, size 0x4
    float mRecordedValue;          // offset 0x10, size 0x4
};

#endif
