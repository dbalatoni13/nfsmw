//
//
//
#ifndef STITCH_AEMS_H
#define STITCH_AEMS_H

#include "csis/csis.h"

// total size: 0x28
// Decl: 19
struct AEMS_StichCollisionStruct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
};

// total size: 0x28
// Decl: 47
struct AEMS_StichWooshStruct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
};

// total size: 0x28
// Decl: 75
struct AEMS_StichStaticStruct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
};

// total size: 0x2C
// Decl: 116
class AEMS_StichCollision {
  public:
    void SetType(int x) {}

    int GetType() {}

    void SetID(int x) {}

    int GetID() {}

    void SetVol(int x) {}

    int GetVol() {}

    void SetPitch(int x) {}

    int GetPitch() {}

    void SetAz(int x) {}

    int GetAz() {}

    void SetOffset(int x) {}

    int GetOffset() {}

    void SetFilter_DryFX(int x) {}

    int GetFilter_DryFX() {}

    void SetFilter_WetFX(int x) {}

    int GetFilter_WetFX() {}

    void SetFilter_LoPass(int x) {}

    int GetFilter_LoPass() {}

    void SetFilter_HiPass(int x) {}

    int GetFilter_HiPass() {}

    int GetRefCount() {}

    static void *operator new(size_t size) {}

    static void operator delete(void *ptr) {}

    AEMS_StichCollision(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                        int filter_HiPass) {}

    ~AEMS_StichCollision() {}

    void CommitMemberData() {}

  private:
    AEMS_StichCollision();
    AEMS_StichCollision &operator=(const AEMS_StichCollision &);

    Csis::Class *mpClass;            // offset 0x0, size 0x4, Decl: 338
    AEMS_StichCollisionStruct mData; // offset 0x4, size 0x28, Decl: 339
};

// total size: 0x2C
// Decl: 347
class AEMS_StichWoosh {
  public:
    void SetType(int x) {}

    int GetType() {}

    void SetID(int x) {}

    int GetID() {}

    void SetVol(int x) {}

    int GetVol() {}

    void SetPitch(int x) {}

    int GetPitch() {}

    void SetAz(int x) {}

    int GetAz() {}

    void SetOffset(int x) {}

    int GetOffset() {}

    void SetFilter_DryFX(int x) {}

    int GetFilter_DryFX() {}

    void SetFilter_WetFX(int x) {}

    int GetFilter_WetFX() {}

    void SetFilter_LoPass(int x) {}

    int GetFilter_LoPass() {}

    void SetFilter_HiPass(int x) {}

    int GetFilter_HiPass() {}

    int GetRefCount() {}

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {}

    AEMS_StichWoosh(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                    int filter_HiPass) {}

    ~AEMS_StichWoosh() {}

    void CommitMemberData() {}

  private:
    AEMS_StichWoosh();
    AEMS_StichWoosh &operator=(const AEMS_StichWoosh &);

    Csis::Class *mpClass;        // offset 0x0, size 0x4
    AEMS_StichWooshStruct mData; // offset 0x4, size 0x28
};

// total size: 0x2C
// Decl: 578
class AEMS_StichStatic {
  public:
    void SetType(int x) {}

    int GetType() {}

    void SetID(int x) {}

    int GetID() {}

    void SetVol(int x) {}

    int GetVol() {}

    void SetPitch(int x) {}

    int GetPitch() {}

    void SetAz(int x) {}

    int GetAz() {}

    void SetOffset(int x) {}

    int GetOffset() {}

    void SetFilter_DryFX(int x) {}

    int GetFilter_DryFX() {}

    void SetFilter_WetFX(int x) {}

    int GetFilter_WetFX() {}

    void SetFilter_LoPass(int x) {}

    int GetFilter_LoPass() {}

    void SetFilter_HiPass(int x) {}

    int GetFilter_HiPass() {}

    int GetRefCount() {}

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {}

    AEMS_StichStatic(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                     int filter_HiPass) {}

    ~AEMS_StichStatic() {}

    void CommitMemberData() {}

  private:
    AEMS_StichStatic();
    AEMS_StichStatic &operator=(const AEMS_StichStatic &);

    Csis::Class *mpClass;         // offset 0x0, size 0x4
    AEMS_StichStaticStruct mData; // offset 0x4, size 0x28
};

#endif
