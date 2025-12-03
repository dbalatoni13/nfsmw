#ifndef ATTRIBSYS_CLASSES_CONTROLLER_H
#define ATTRIBSYS_CLASSES_CONTROLLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO move
enum InputUpdateType {
    kUpdate = 0,
    kPress = 1,
    kRelease = 2,
    kAnalogPress = 3,
    kAnalogRelease = 4,
    kCenterControl = 5,
};

// TODO move?
// total size: 0x20
struct ControllerDataRecord {
    Attrib::StringKey mDeviceID; // offset 0x0, size 0x10
    InputUpdateType mUpdateType; // offset 0x10, size 0x4
    float mLowerDZ;              // offset 0x14, size 0x4
    float mUpperDZ;              // offset 0x18, size 0x4
};

namespace Attrib {
namespace Gen {

struct controller : Instance {
    struct _LayoutStruct {
        char CollectionName[4]; // offset 0x0, size 0x4
        bool Pauseable;         // offset 0x4, size 0x1
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "controller");
    }

    controller(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    controller(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~controller() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x2dee1998;
    }

    const ControllerDataRecord &ICEACTION_DUTCH_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x005f9d1d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_DUTCH_RIGHT() const {
        return this->Get(0x005f9d1d).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_CANCEL(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x01079014, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_CANCEL() const {
        return this->Get(0x01079014).GetLength();
    }

    const ControllerDataRecord &ICEACTION_FAST_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x021ecbba, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_FAST_LEFT() const {
        return this->Get(0x021ecbba).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON2(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x02c68d30, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON2() const {
        return this->Get(0x02c68d30).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_GAS(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x06525329, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_GAS() const {
        return this->Get(0x06525329).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_RDOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x06645bd6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RDOWN() const {
        return this->Get(0x06645bd6).GetLength();
    }

    const ControllerDataRecord &REACTION_APPLY_TO_NEXT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x06bb1fc4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &GAMEACTION_SHIFTUP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x06dbd3c3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_SHIFTUP() const {
        return this->Get(0x06dbd3c3).GetLength();
    }

    const ControllerDataRecord &ICEACTION_INSERT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x0bec5a35, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_INSERT() const {
        return this->Get(0x0bec5a35).GetLength();
    }

    const ControllerDataRecord &ICEACTION_GRAB_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x0d6d7fb4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_GRAB_LEFT() const {
        return this->Get(0x0d6d7fb4).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x0fd8110f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_UP() const {
        return this->Get(0x0fd8110f).GetLength();
    }

    const ControllerDataRecord &HUDACTION_PAD_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x112b5d2d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_PAD_DOWN() const {
        return this->Get(0x112b5d2d).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x136435ac, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_DOWN() const {
        return this->Get(0x136435ac).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN3(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x15024c88, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_DEBUGHUMAN3() const {
        return this->Get(0x15024c88).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x16dab024, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &GAMEACTION_HANDBRAKE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x1aaf2c13, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_HANDBRAKE() const {
        return this->Get(0x1aaf2c13).GetLength();
    }

    const ControllerDataRecord &ICEACTION_FAST_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x1cf73674, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_FAST_RIGHT() const {
        return this->Get(0x1cf73674).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_SIMSTEP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x1f903fec, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_SIMSTEP() const {
        return this->Get(0x1f903fec).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_TOGGLEAI(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x1ff07aef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_TOGGLEAI() const {
        return this->Get(0x1ff07aef).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_TOGGLECARCOLOUR(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x25e38fdf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_TOGGLECARCOLOUR() const {
        return this->Get(0x25e38fdf).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SIMSPEED_INC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x25ee8e66, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SIMSPEED_INC() const {
        return this->Get(0x25ee8e66).GetLength();
    }

    const ControllerDataRecord &REACTION_SELECTNEXT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x291bda5d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &FRONTENDACTION_RUP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x2b8b4b6f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RUP() const {
        return this->Get(0x2b8b4b6f).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_TURBO(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x2cf3d7d7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_TURBO() const {
        return this->Get(0x2cf3d7d7).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_TILT_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x2cfdf83b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_TILT_UP() const {
        return this->Get(0x2cfdf83b).GetLength();
    }

    const ControllerDataRecord &REACTION_LOOKRIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x2e183c63, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_LOOKRIGHT() const {
        return this->Get(0x2e183c63).GetLength();
    }

    const ControllerDataRecord &CAMERAACTION_CHANGE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x2f478795, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERAACTION_CHANGE() const {
        return this->Get(0x2f478795).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_FORWARD(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x3391934b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_FORWARD() const {
        return this->Get(0x3391934b).GetLength();
    }

    const ControllerDataRecord &ICEACTION_DUTCH_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x34e03ed7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_DUTCH_LEFT() const {
        return this->Get(0x34e03ed7).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_RLEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x376803fa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RLEFT() const {
        return this->Get(0x376803fa).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_STEERLEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x37b1b6a8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_STEERLEFT() const {
        return this->Get(0x37b1b6a8).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN2(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x37eb8ea4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_DEBUGHUMAN2() const {
        return this->Get(0x37eb8ea4).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_FORWARD(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x395f1ceb, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_MOVE_FORWARD() const {
        return this->Get(0x395f1ceb).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_D_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x3ae35a4c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_D_LEFT() const {
        return this->Get(0x3ae35a4c).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON3(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x4178fc35, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON3() const {
        return this->Get(0x4178fc35).GetLength();
    }

    const ControllerDataRecord &ICEACTION_COPY(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x469a29a2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_COPY() const {
        return this->Get(0x469a29a2).GetLength();
    }

    const ControllerDataRecord &ICEACTION_ZOOM_IN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x46b49ed9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_ZOOM_IN() const {
        return this->Get(0x46b49ed9).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_D_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x49eade84, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_D_RIGHT() const {
        return this->Get(0x49eade84).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_SHIFTDOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x50638735, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_SHIFTDOWN() const {
        return this->Get(0x50638735).GetLength();
    }

    const ControllerDataRecord &ICEACTION_LETTERBOX_IN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x509346cd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_LETTERBOX_IN() const {
        return this->Get(0x509346cd).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_BACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x521d0a7c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_MOVE_BACK() const {
        return this->Get(0x521d0a7c).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_OUT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x535f828a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_OUT() const {
        return this->Get(0x535f828a).GetLength();
    }

    const ControllerDataRecord &ICEACTION_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5383adea, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_UP() const {
        return this->Get(0x5383adea).GetLength();
    }

    const ControllerDataRecord &ICEACTION_PLAY(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5404dd6d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_PLAY() const {
        return this->Get(0x5404dd6d).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_TILT_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x58482016, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_TILT_DOWN() const {
        return this->Get(0x58482016).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x591cf93a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_RIGHT() const {
        return this->Get(0x591cf93a).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON1(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5954d9c3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON1() const {
        return this->Get(0x5954d9c3).GetLength();
    }

    const ControllerDataRecord &HUDACTION_NEXTSONG(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5a1e0eb7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_NEXTSONG() const {
        return this->Get(0x5a1e0eb7).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_SPIN_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5bcd578b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_SPIN_LEFT() const {
        return this->Get(0x5bcd578b).GetLength();
    }

    const ControllerDataRecord &ICEACTION_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5bd28819, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_RIGHT() const {
        return this->Get(0x5bd28819).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5c36897f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_RIGHT() const {
        return this->Get(0x5c36897f).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x5eb6f87f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_DOWN() const {
        return this->Get(0x5eb6f87f).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x64a558de, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_RIGHT() const {
        return this->Get(0x64a558de).GetLength();
    }

    const ControllerDataRecord &REACTION_PREVIOUSLANE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x64bf8411, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x65f33c23, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_UP() const {
        return this->Get(0x65f33c23).GetLength();
    }

    const ControllerDataRecord &REACTION_PICK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x67490952, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &FRONTENDACTION_START(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x67eebe3f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_START() const {
        return this->Get(0x67eebe3f).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHAKE_FRQ_DEC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x68306931, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHAKE_FRQ_DEC() const {
        return this->Get(0x68306931).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SCREENSHOT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x68af00cb, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SCREENSHOT() const {
        return this->Get(0x68af00cb).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN4(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x6b76f3be, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_DEBUGHUMAN4() const {
        return this->Get(0x6b76f3be).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON0(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x6eb8dedc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON0() const {
        return this->Get(0x6eb8dedc).GetLength();
    }

    const ControllerDataRecord &HUDACTION_PAD_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x713c8f5c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_PAD_RIGHT() const {
        return this->Get(0x713c8f5c).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_IN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x71e24c87, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_IN() const {
        return this->Get(0x71e24c87).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_D_FORWARD(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x76354cb7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_D_FORWARD() const {
        return this->Get(0x76354cb7).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_TOGGLESIMSTEP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x770e0d16, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_TOGGLESIMSTEP() const {
        return this->Get(0x770e0d16).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_TURNLEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x77570455, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_TURNLEFT() const {
        return this->Get(0x77570455).GetLength();
    }

    const ControllerDataRecord &ICEACTION_CUT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x7be61732, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_CUT() const {
        return this->Get(0x7be61732).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_SUPER_TURBO(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x7d1b500d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_SUPER_TURBO() const {
        return this->Get(0x7d1b500d).GetLength();
    }

    const ControllerDataRecord &CAMERAACTION_PULLBACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x7f189eec, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERAACTION_PULLBACK() const {
        return this->Get(0x7f189eec).GetLength();
    }

    const ControllerDataRecord &HUDACTION_SKIPNIS(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8053361a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_SKIPNIS() const {
        return this->Get(0x8053361a).GetLength();
    }

    const ControllerDataRecord &ICEACTION_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x80ff983c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_DOWN() const {
        return this->Get(0x80ff983c).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_BRAKE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x82a9b7d8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_BRAKE() const {
        return this->Get(0x82a9b7d8).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_SPEED_INC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x82fd8328, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_SPEED_INC() const {
        return this->Get(0x82fd8328).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x83d67a7b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_LEFT() const {
        return this->Get(0x83d67a7b).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8573245a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_DOWN() const {
        return this->Get(0x8573245a).GetLength();
    }

    const ControllerDataRecord &ICEACTION_CLIP_IN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x85c7f9ff, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_CLIP_IN() const {
        return this->Get(0x85c7f9ff).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_SPIN_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8784bb00, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_SPIN_RIGHT() const {
        return this->Get(0x8784bb00).GetLength();
    }

    const ControllerDataRecord &REACTION_EDIT_NODE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x87c357b2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_D_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x88e9f301, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_D_DOWN() const {
        return this->Get(0x88e9f301).GetLength();
    }

    const ControllerDataRecord &REACTION_LOOKDOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8a90bda5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_LOOKDOWN() const {
        return this->Get(0x8a90bda5).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHAKE_MAG_DEC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8a9ee6ef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHAKE_MAG_DEC() const {
        return this->Get(0x8a9ee6ef).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8afced7a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_LEFT() const {
        return this->Get(0x8afced7a).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_D_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8bb25bd4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_D_UP() const {
        return this->Get(0x8bb25bd4).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_RTRIGGER(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8d005b54, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RTRIGGER() const {
        return this->Get(0x8d005b54).GetLength();
    }

    const ControllerDataRecord &CAMERAACTION_LOOKBACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x8dc18faf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERAACTION_LOOKBACK() const {
        return this->Get(0x8dc18faf).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_BACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x90b590f9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_BACK() const {
        return this->Get(0x90b590f9).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SCREENSHOT_STREAM(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x915b5e15, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SCREENSHOT_STREAM() const {
        return this->Get(0x915b5e15).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_D_BACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x94227bad, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_D_BACK() const {
        return this->Get(0x94227bad).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x97011fc1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_LEFT() const {
        return this->Get(0x97011fc1).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x98a510c3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_LEFT() const {
        return this->Get(0x98a510c3).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_GAMEBREAKER(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x98d605b8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_GAMEBREAKER() const {
        return this->Get(0x98d605b8).GetLength();
    }

    const ControllerDataRecord &ICEACTION_HIDE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x9b1e3f1b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_HIDE() const {
        return this->Get(0x9b1e3f1b).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x9b870f45, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_LEFT() const {
        return this->Get(0x9b870f45).GetLength();
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const ControllerDataRecord &ICEACTION_LETTERBOX_OUT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0x9d6dea3c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_LETTERBOX_OUT() const {
        return this->Get(0x9d6dea3c).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON5(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa28033b2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON5() const {
        return this->Get(0xa28033b2).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SELECT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa3b4828b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SELECT() const {
        return this->Get(0xa3b4828b).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_D_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa449a027, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_D_RIGHT() const {
        return this->Get(0xa449a027).GetLength();
    }

    const ControllerDataRecord &ICEACTION_CLIP_OUT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa4f85f2d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_CLIP_OUT() const {
        return this->Get(0xa4f85f2d).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa914965d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_MOVE_LEFT() const {
        return this->Get(0xa914965d).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xa968b63a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RIGHT() const {
        return this->Get(0xa968b63a).GetLength();
    }

    const ControllerDataRecord &REACTION_ACTIVATE_EDIT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xaa96159c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &GAMEACTION_NOS(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xabc46a0f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_NOS() const {
        return this->Get(0xabc46a0f).GetLength();
    }

    const ControllerDataRecord &CAMERAACTION_DEBUG(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xacc223c6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERAACTION_DEBUG() const {
        return this->Get(0xacc223c6).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_STEERRIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xb3a37c41, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_STEERRIGHT() const {
        return this->Get(0xb3a37c41).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_RESET(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xb488445a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_RESET() const {
        return this->Get(0xb488445a).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SIMSPEED_DEC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xb5625552, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SIMSPEED_DEC() const {
        return this->Get(0xb5625552).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_MOVE_BACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xb9db7418, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_MOVE_BACK() const {
        return this->Get(0xb9db7418).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_JUMP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xba48a502, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_JUMP() const {
        return this->Get(0xba48a502).GetLength();
    }

    const ControllerDataRecord &CAMERAACTION_ENABLE_ICE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xbac6c7d7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERAACTION_ENABLE_ICE() const {
        return this->Get(0xbac6c7d7).GetLength();
    }

    const ControllerDataRecord &HUDACTION_ENGAGE_EVENT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xbbab56ef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_ENGAGE_EVENT() const {
        return this->Get(0xbbab56ef).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xbdcd877e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &DEBUGACTION_DROPCAR(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xbdf62d8e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_DROPCAR() const {
        return this->Get(0xbdf62d8e).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc0c95361, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_UP() const {
        return this->Get(0xc0c95361).GetLength();
    }

    const ControllerDataRecord &REACTION_LOOKLEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc1097c28, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_LOOKLEFT() const {
        return this->Get(0xc1097c28).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_ACCEPT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc18193fd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_ACCEPT() const {
        return this->Get(0xc18193fd).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc455d61d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_UP() const {
        return this->Get(0xc455d61d).GetLength();
    }

    const ControllerDataRecord &REACTION_AUTOFIT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc604e4c8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &ICEACTION_ALT_1(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc6658aef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_ALT_1() const {
        return this->Get(0xc6658aef).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_FORTH(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc667a7c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_FORTH() const {
        return this->Get(0xc667a7c0).GetLength();
    }

    const ControllerDataRecord &ICEACTION_UNDO(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xc9aca929, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_UNDO() const {
        return this->Get(0xc9aca929).GetLength();
    }

    const ControllerDataRecord &ICEACTION_HELP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xcbb4ccbf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_HELP() const {
        return this->Get(0xcbb4ccbf).GetLength();
    }

    const ControllerDataRecord &REACTION_LOOKUP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xccc3ad00, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_LOOKUP() const {
        return this->Get(0xccc3ad00).GetLength();
    }

    const ControllerDataRecord &ICEACTION_PASTE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xcf996f30, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_PASTE() const {
        return this->Get(0xcf996f30).GetLength();
    }

    const ControllerDataRecord &REACTION_MOVE_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xd19432c0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_REACTION_MOVE_RIGHT() const {
        return this->Get(0xd19432c0).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHAKE_FRQ_INC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xd3f858ca, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHAKE_FRQ_INC() const {
        return this->Get(0xd3f858ca).GetLength();
    }

    const ControllerDataRecord &ICEACTION_CANCEL(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xd9e3873b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_CANCEL() const {
        return this->Get(0xd9e3873b).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHAKE_MAG_INC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xda3fa888, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHAKE_MAG_INC() const {
        return this->Get(0xda3fa888).GetLength();
    }

    const ControllerDataRecord &REACTION_TURBO(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xda7d1177, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &HUDACTION_PAUSEREQUEST(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdaaf60cd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_PAUSEREQUEST() const {
        return this->Get(0xdaaf60cd).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_BACK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdc0e7b73, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_BACK() const {
        return this->Get(0xdc0e7b73).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_FORWARD(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdc1ab9ea, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_FORWARD() const {
        return this->Get(0xdc1ab9ea).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdc4ccb3c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_DOWN() const {
        return this->Get(0xdc4ccb3c).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_UP(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdcfe9685, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_UP() const {
        return this->Get(0xdcfe9685).GetLength();
    }

    const ControllerDataRecord &ICEACTION_BUBBLE_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xdd43339a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_BUBBLE_RIGHT() const {
        return this->Get(0xdd43339a).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_RRIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xde24df25, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_RRIGHT() const {
        return this->Get(0xde24df25).GetLength();
    }

    const ControllerDataRecord &HUDACTION_PAD_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xe2d51bdc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_HUDACTION_PAD_LEFT() const {
        return this->Get(0xe2d51bdc).GetLength();
    }

    const ControllerDataRecord &VOIPACTION_PUSHTOTALK(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xe5c50a17, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_VOIPACTION_PUSHTOTALK() const {
        return this->Get(0xe5c50a17).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN1(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xe82d180f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_DEBUGHUMAN1() const {
        return this->Get(0xe82d180f).GetLength();
    }

    const ControllerDataRecord &ICEACTION_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xeb1ce242, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_LEFT() const {
        return this->Get(0xeb1ce242).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_LTRIGGER(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xeda51002, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_LTRIGGER() const {
        return this->Get(0xeda51002).GetLength();
    }

    const ControllerDataRecord &REACTION_NEXTLANE(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xedd1f872, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    const ControllerDataRecord &DEBUGACTION_TOGGLEDEMOCAMERAS(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xedd2a2f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_TOGGLEDEMOCAMERAS() const {
        return this->Get(0xedd2a2f3).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_DOWN(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xef3fac67, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_DOWN() const {
        return this->Get(0xef3fac67).GetLength();
    }

    const ControllerDataRecord &ICEACTION_ZOOM_OUT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xf036026a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_ZOOM_OUT() const {
        return this->Get(0xf036026a).GetLength();
    }

    const ControllerDataRecord &ICEACTION_SHUTTLE_SPEED_DEC(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xf08628de, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_SHUTTLE_SPEED_DEC() const {
        return this->Get(0xf08628de).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_BUTTON4(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xf31f71c7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_BUTTON4() const {
        return this->Get(0xf31f71c7).GetLength();
    }

    const ControllerDataRecord &DEBUGACTION_LOOK_D_LEFT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xf3c5ed4e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_DEBUGACTION_LOOK_D_LEFT() const {
        return this->Get(0xf3c5ed4e).GetLength();
    }

    const ControllerDataRecord &FRONTENDACTION_CANCEL_ALT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xf6e6bad9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FRONTENDACTION_CANCEL_ALT() const {
        return this->Get(0xf6e6bad9).GetLength();
    }

    const ControllerDataRecord &GAMEACTION_TURNRIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xfa647f8f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_GAMEACTION_TURNRIGHT() const {
        return this->Get(0xfa647f8f).GetLength();
    }

    const ControllerDataRecord &ICEACTION_GRAB_RIGHT(unsigned int index) const {
        const ControllerDataRecord *resultptr = reinterpret_cast<const ControllerDataRecord *>(this->GetAttributePointer(0xfcbd5969, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const ControllerDataRecord *>(DefaultDataArea(sizeof(ControllerDataRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_ICEACTION_GRAB_RIGHT() const {
        return this->Get(0xfcbd5969).GetLength();
    }

    const bool &Pauseable() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Pauseable;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
