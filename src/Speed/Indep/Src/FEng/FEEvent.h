#ifndef FEEVENT_H_
#define FEEVENT_H_

#include "types.h"

// total size: 0xC
// Decl: speed/indep/src/feng/FEEvent.h:23
typedef struct {
    u32 EventID; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEEvent.h:24
    u32 Target;  // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEEvent.h:25
    u32 tTime;   // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEEvent.h:26
} FEEvent;

// total size: 0x8
// Decl: speed/indep/src/feng/FEEvent.h:45
class FEEventList {
  private:
    i32 Count;       // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEEvent.h:47
    FEEvent *pEvent; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEEvent.h:48

    void Insert(int Index);

  public:
    FEEventList() {}                        // Decl: speed/indep/src/feng/FEEvent.h:53
    FEEventList(struct FEEventList &Src) {} // Decl: speed/indep/src/feng/FEEvent.h:54
    ~FEEventList() {}

    FEEvent &operator[](int Index) {
        return pEvent[Index];
    }
    void operator=(struct FEEventList &Src); // Decl: speed/indep/src/feng/FEEvent.h:62

    void SetCount(i32 NewCount);
    i32 AddEvent(u32 EventID, u32 Target, u32 tTime);
    i32 FindEvent(u32 EventID);
    void Delete(i32 Index);
    u32 GetCount() { // Decl: speed/indep/src/feng/FEEvent.h:68
        return Count;
    }

    void SortEvents(); // Decl: speed/indep/src/feng/FEEvent.h:70
};

#endif
