#ifndef __E_DEBUG_WORLD_H
#define __E_DEBUG_WORLD_H

#include <types.h>

// total size: 0x1
class DebugWorld {
  public:
    DebugWorld();
    ~DebugWorld();
    static bool IsEnabled() {}
    static void Init();
    static void DeInit();
    static DebugWorld &Get() {
        return *mThis;
    }
    void Service();
    void DebugDisplay();

  private:
    void HandleSaveHotPosition();
    void HandleJumpToHotPosition();

    static DebugWorld *mThis;
    static bool mOnOff;
};

#endif
