#ifndef _CLUMP_H_

#define _CLUMP_H_ // Decl: 6

// total size: 0x10
// Decl: 9
typedef struct CLUMP_ITEMtag {
    unsigned int key;    // offset 0x0, size 0x4, Decl: 10
    unsigned int size;   // offset 0x4, size 0x4, Decl: 11
    unsigned int header; // offset 0x8, size 0x4, Decl: 12
    unsigned int sample; // offset 0xC, size 0x4, Decl: 13
} CLUMP_ITEM;

#define CLUMP_MAX_BANK_TYPES 20 // Decl: 16

// total size: 0x68
// Decl: 19
typedef struct CLUMP_IDX_FILEtag {
    unsigned int numtypes;                       // offset 0x0, size 0x4, Decl: 20
    unsigned int numbanks[CLUMP_MAX_BANK_TYPES]; // offset 0x4, size 0x50, Decl: 22
    unsigned int count;                          // offset 0x54, size 0x4, Decl: 23
    CLUMP_ITEM item[1];                          // offset 0x58, size 0x10, Decl: 24
} CLUMP_IDX_FILE;

#define CLUMP_TYPE_SHIFT 24 // Decl: 27

#endif
