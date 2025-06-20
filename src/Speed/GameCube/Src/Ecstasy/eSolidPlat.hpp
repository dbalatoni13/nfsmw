#pragma once

struct eSolidPlatInfo {
    // total size: 0x24
    unsigned short Version;              // offset 0x0, size 0x2
    unsigned short StripFlags;           // offset 0x2, size 0x2
    unsigned short NumStrips;            // offset 0x4, size 0x2
    unsigned short NumIdxClrTable;       // offset 0x6, size 0x2
    unsigned int SizeofStripData;        // offset 0x8, size 0x4
    unsigned int DataOffset0;            // offset 0xC, size 0x4
    unsigned int DataOffset1;            // offset 0x10, size 0x4
    unsigned int DataOffset2;            // offset 0x14, size 0x4
    unsigned int DataOffset3;            // offset 0x18, size 0x4
    struct eStripEntry *StripEntryTable; // offset 0x1C, size 0x4
    unsigned char *StripDataStart;       // offset 0x20, size 0x4
};

struct eSolidPlatInterface {
    // total size: 0x4
    struct eSolidPlatInfo *PlatInfo; // offset 0x0, size 0x4
};
