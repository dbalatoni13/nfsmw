#ifndef ATTRIBSYS_CLASSES_CHOPPERSPECS_H
#define ATTRIBSYS_CLASSES_CHOPPERSPECS_H


#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct chopperspecs : Instance {
struct _LayoutStruct {
UMath::Vector4 AIR_RESISTANCE_SCALE; // offset 0x0, size 0x10
float YAW_STRENGTH_FRONT; // offset 0x10, size 0x4
float YAW_DAMP; // offset 0x14, size 0x4
float YAW_LIMIT_FRONT; // offset 0x18, size 0x4
float PITCH_ALIGN_SCALE; // offset 0x1c, size 0x4
float AIR_RESISTANCE; // offset 0x20, size 0x4
float PITCH_STOP_SPEED; // offset 0x24, size 0x4
float YAW_BOOST_LIMIT; // offset 0x28, size 0x4
float ROLL_SPEED_MIN; // offset 0x2c, size 0x4
float ROLL_SLOW_DOWN_RATE; // offset 0x30, size 0x4
float ROLL_START_SPEED; // offset 0x34, size 0x4
float PITCH_ANG; // offset 0x38, size 0x4
float YAW_STRENGTH_REAR; // offset 0x3c, size 0x4
float ROLL_ANG; // offset 0x40, size 0x4
float MAX_SPEED_MPS; // offset 0x44, size 0x4
float STRAFE_SCALEY; // offset 0x48, size 0x4
float STRAFE_SCALEX; // offset 0x4c, size 0x4
float YAW_LIMIT_REAR; // offset 0x50, size 0x4
float TURN_BOOST_SPEED; // offset 0x54, size 0x4
float PITCH_SLOW_DOWN_RATE; // offset 0x58, size 0x4
float DRIVE_SPEED; // offset 0x5c, size 0x4
float ROLL_ALIGN_SCALE; // offset 0x60, size 0x4
float SAME_ALIGN_SCALE; // offset 0x64, size 0x4
bool SCALE_STEERING; // offset 0x68, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "chopperspecs");
}

chopperspecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

chopperspecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~chopperspecs() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0x5d898ee7;
}

const UMath::Vector4 &AIR_RESISTANCE_SCALE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AIR_RESISTANCE_SCALE;
}

const float &YAW_STRENGTH_FRONT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_STRENGTH_FRONT;
}

const float &YAW_DAMP() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_DAMP;
}

const float &YAW_LIMIT_FRONT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_LIMIT_FRONT;
}

const float &PITCH_ALIGN_SCALE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PITCH_ALIGN_SCALE;
}

const float &AIR_RESISTANCE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AIR_RESISTANCE;
}

const float &PITCH_STOP_SPEED() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PITCH_STOP_SPEED;
}

const float &YAW_BOOST_LIMIT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_BOOST_LIMIT;
}

const float &ROLL_SPEED_MIN() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_SPEED_MIN;
}

const float &ROLL_SLOW_DOWN_RATE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_SLOW_DOWN_RATE;
}

const float &ROLL_START_SPEED() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_START_SPEED;
}

const float &PITCH_ANG() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PITCH_ANG;
}

const float &YAW_STRENGTH_REAR() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_STRENGTH_REAR;
}

const float &ROLL_ANG() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_ANG;
}

const float &MAX_SPEED_MPS() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MAX_SPEED_MPS;
}

const float &STRAFE_SCALEY() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->STRAFE_SCALEY;
}

const float &STRAFE_SCALEX() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->STRAFE_SCALEX;
}

const float &YAW_LIMIT_REAR() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_LIMIT_REAR;
}

const float &TURN_BOOST_SPEED() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TURN_BOOST_SPEED;
}

const float &PITCH_SLOW_DOWN_RATE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PITCH_SLOW_DOWN_RATE;
}

const float &DRIVE_SPEED() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DRIVE_SPEED;
}

const float &ROLL_ALIGN_SCALE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_ALIGN_SCALE;
}

const float &SAME_ALIGN_SCALE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SAME_ALIGN_SCALE;
}

const bool &SCALE_STEERING() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SCALE_STEERING;
}

};

} // namespace Gen
} // namespace Attrib

#endif
