#ifndef __INPUT_DEF_PARSER_H__
#define __INPUT_DEF_PARSER_H__

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Input/Action.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "stl/_list.h"

enum InputUpdateType {
    kUpdate = 0,
    kPress = 1,
    kRelease = 2,
    kAnalogPress = 3,
    kAnalogRelease = 4,
    kCenterControl = 5,
};

// total size: 0x1C
struct InputMapEntry {
    InputUpdateType UpdateType; // offset 0x0, size 0x4
    float LowerDZ;              // offset 0x4, size 0x4
    float UpperDZ;              // offset 0x8, size 0x4
    ActionID Action;            // offset 0xC, size 0x4
    int DeviceScalarIndex;      // offset 0x10, size 0x4
    float PreviousValue;        // offset 0x14, size 0x4
    float CurrentValue;         // offset 0x18, size 0x4

    bool HasChanged() const {
        return this->CurrentValue < 0; // ?
    }
};

class InputMapping {
public:
    typedef struct _STL::list<InputMapEntry> Entries;

    // static void operator delete(void *mem, const char *name) {};
    InputMapping(InputDevice *device, const Attrib::Collection *spec);
    ~InputMapping();


    Entries &GetEntries() { return this->mEntries; }

    USE_FASTALLOC(InputMapping)



private:
    Entries mEntries;
};

#endif
