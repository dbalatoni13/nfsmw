#ifndef SFILTER_H
#define SFILTER_H // Decl: 323

#define SFILTERCALL // Decl: 336

typedef int FIXED24P8; // Decl: 343

// total size: 0x1C
// Decl: 356
typedef struct SFILTERNODE {
    int (*filterfn)(void *, int, void *, void *, int); // offset 0x0, size 0x4, Decl: 357
    void (*restorefn)(void *);                         // offset 0x4, size 0x4, Decl: 358
    SFILTERNODE *pfnnext;                              // offset 0x8, size 0x4, Decl: 359
    SFILTERNODE *pfnnext2;                             // offset 0xC, size 0x4, Decl: 360
    SFILTERNODE *pfnprev1;                             // offset 0x10, size 0x4, Decl: 361
    SFILTERNODE *pfnprev2;                             // offset 0x14, size 0x4, Decl: 362
    unsigned short priority;                           // offset 0x18, size 0x2, Decl: 363
    unsigned char requester;                           // offset 0x1A, size 0x1, Decl: 364
    unsigned char clean;                               // offset 0x1B, size 0x1, Decl: 365
} SFILTERNODE;

#endif
