#ifndef FEEVENT_H_
#define FEEVENT_H_

#include "types.h"
#include "Speed/Indep/Src/FEng/FETypes.h"

// total size: 0xC
// Decl: 23
typedef struct {
    u32 EventID; // offset 0x0, size 0x4, Decl: 24
    u32 Target;  // offset 0x4, size 0x4, Decl: 25
    u32 tTime;   // offset 0x8, size 0x4, Decl: 26
} FEEvent;

// total size: 0x8
// Decl: 45
class FEEventList {
  private:
    i32 Count;       // offset 0x0, size 0x4, Decl: 47
    FEEvent *pEvent; // offset 0x4, size 0x4, Decl: 48

    void Insert(i32 Index);

  public:
    FEEventList() { // Decl: 53
        Count = 0;
        pEvent = nullptr;
    }
    FEEventList(FEEventList &Src) {} // Decl: 54
    ~FEEventList() {
        delete[] pEvent;
        pEvent = nullptr;
    }

    FEEvent &operator[](int Index) {
        return pEvent[Index];
    }
    void operator=(FEEventList &Src); // Decl: 62

    void SetCount(i32 NewCount);
    i32 AddEvent(u32 EventID, u32 Target, u32 tTime);
    i32 FindEvent(u32 EventID);
    void Delete(i32 Index);
    u32 GetCount() { // Decl: 68
        return Count;
    }

    void SortEvents(); // Decl: 70
};

#endif
