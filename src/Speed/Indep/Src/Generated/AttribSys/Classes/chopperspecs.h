#ifndef _attrib_gen_chopperspecs_h
#define _attrib_gen_chopperspecs_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct chopperspecs : Instance {
    struct _LayoutStruct {
        UMath::Vector4 AIR_RESISTANCE_SCALE;        // offset 0x0, size 0x10
        EA::Reflection::Float YAW_STRENGTH_FRONT;   // offset 0x10, size 0x4
        EA::Reflection::Float YAW_DAMP;             // offset 0x14, size 0x4
        EA::Reflection::Float YAW_LIMIT_FRONT;      // offset 0x18, size 0x4
        EA::Reflection::Float PITCH_ALIGN_SCALE;    // offset 0x1c, size 0x4
        EA::Reflection::Float AIR_RESISTANCE;       // offset 0x20, size 0x4
        EA::Reflection::Float PITCH_STOP_SPEED;     // offset 0x24, size 0x4
        EA::Reflection::Float YAW_BOOST_LIMIT;      // offset 0x28, size 0x4
        EA::Reflection::Float ROLL_SPEED_MIN;       // offset 0x2c, size 0x4
        EA::Reflection::Float ROLL_SLOW_DOWN_RATE;  // offset 0x30, size 0x4
        EA::Reflection::Float ROLL_START_SPEED;     // offset 0x34, size 0x4
        EA::Reflection::Float PITCH_ANG;            // offset 0x38, size 0x4
        EA::Reflection::Float YAW_STRENGTH_REAR;    // offset 0x3c, size 0x4
        EA::Reflection::Float ROLL_ANG;             // offset 0x40, size 0x4
        EA::Reflection::Float MAX_SPEED_MPS;        // offset 0x44, size 0x4
        EA::Reflection::Float STRAFE_SCALEY;        // offset 0x48, size 0x4
        EA::Reflection::Float STRAFE_SCALEX;        // offset 0x4c, size 0x4
        EA::Reflection::Float YAW_LIMIT_REAR;       // offset 0x50, size 0x4
        EA::Reflection::Float TURN_BOOST_SPEED;     // offset 0x54, size 0x4
        EA::Reflection::Float PITCH_SLOW_DOWN_RATE; // offset 0x58, size 0x4
        EA::Reflection::Float DRIVE_SPEED;          // offset 0x5c, size 0x4
        EA::Reflection::Float ROLL_ALIGN_SCALE;     // offset 0x60, size 0x4
        EA::Reflection::Float SAME_ALIGN_SCALE;     // offset 0x64, size 0x4
        EA::Reflection::Bool SCALE_STEERING;        // offset 0x68, size 0x1
    };

    typedef EA::Reflection::Float TypeOf_AIR_RESISTANCE;
    typedef UMath::Vector4 TypeOf_AIR_RESISTANCE_SCALE;
    typedef EA::Reflection::Float TypeOf_DRIVE_SPEED;
    typedef EA::Reflection::Float TypeOf_MAX_SPEED_MPS;
    typedef EA::Reflection::Float TypeOf_PITCH_ALIGN_SCALE;
    typedef EA::Reflection::Float TypeOf_PITCH_ANG;
    typedef EA::Reflection::Float TypeOf_PITCH_SLOW_DOWN_RATE;
    typedef EA::Reflection::Float TypeOf_PITCH_STOP_SPEED;
    typedef EA::Reflection::Float TypeOf_ROLL_ALIGN_SCALE;
    typedef EA::Reflection::Float TypeOf_ROLL_ANG;
    typedef EA::Reflection::Float TypeOf_ROLL_SLOW_DOWN_RATE;
    typedef EA::Reflection::Float TypeOf_ROLL_SPEED_MIN;
    typedef EA::Reflection::Float TypeOf_ROLL_START_SPEED;
    typedef EA::Reflection::Float TypeOf_SAME_ALIGN_SCALE;
    typedef EA::Reflection::Bool TypeOf_SCALE_STEERING;
    typedef EA::Reflection::Float TypeOf_STRAFE_SCALEX;
    typedef EA::Reflection::Float TypeOf_STRAFE_SCALEY;
    typedef EA::Reflection::Float TypeOf_TURN_BOOST_SPEED;
    typedef EA::Reflection::Float TypeOf_YAW_BOOST_LIMIT;
    typedef EA::Reflection::Float TypeOf_YAW_DAMP;
    typedef EA::Reflection::Float TypeOf_YAW_LIMIT_FRONT;
    typedef EA::Reflection::Float TypeOf_YAW_LIMIT_REAR;
    typedef EA::Reflection::Float TypeOf_YAW_STRENGTH_FRONT;
    typedef EA::Reflection::Float TypeOf_YAW_STRENGTH_REAR;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("chopperspecs");
    chopperspecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chopperspecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chopperspecs(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chopperspecs(const chopperspecs &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chopperspecs(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~chopperspecs() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x5d898ee7;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x5d898ee7, dynamicCollectionKey, spaceForAdditionalAttributes);
    }
    Key GenerateUniqueKey(const char *name, bool registerName) const {
        return GenerateUniqueKey(name, registerName);
    }
    void Change(const Collection *c) {
        Instance::Change(c);
    }
    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }
    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }
    void ChangeWithDefault(const RefSpec &refspec) {
        Instance::ChangeWithDefault(refspec);
    }
    void ChangeWithDefault(Key collectionkey) {
        Change(FindCollectionWithDefault(ClassKey(), collectionkey));
    }
    const chopperspecs &operator=(const chopperspecs &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const chopperspecs &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AIR_RESISTANCE(TAttrib<EA::Reflection::Float> &result) const;
    bool AIR_RESISTANCE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AIR_RESISTANCE() const;
    bool SET_AIR_RESISTANCE(const EA::Reflection::Float &input);

    bool AIR_RESISTANCE_SCALE(TAttrib<UMath::Vector4> &result) const;
    bool AIR_RESISTANCE_SCALE(UMath::Vector4 &result) const;
    const UMath::Vector4 &AIR_RESISTANCE_SCALE() const;
    bool SET_AIR_RESISTANCE_SCALE(const UMath::Vector4 &input);

    bool DRIVE_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool DRIVE_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DRIVE_SPEED() const;
    bool SET_DRIVE_SPEED(const EA::Reflection::Float &input);

    bool MAX_SPEED_MPS(TAttrib<EA::Reflection::Float> &result) const;
    bool MAX_SPEED_MPS(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MAX_SPEED_MPS() const;
    bool SET_MAX_SPEED_MPS(const EA::Reflection::Float &input);

    bool PITCH_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const;
    bool PITCH_ALIGN_SCALE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PITCH_ALIGN_SCALE() const;
    bool SET_PITCH_ALIGN_SCALE(const EA::Reflection::Float &input);

    bool PITCH_ANG(TAttrib<EA::Reflection::Float> &result) const;
    bool PITCH_ANG(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PITCH_ANG() const;
    bool SET_PITCH_ANG(const EA::Reflection::Float &input);

    bool PITCH_SLOW_DOWN_RATE(TAttrib<EA::Reflection::Float> &result) const;
    bool PITCH_SLOW_DOWN_RATE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PITCH_SLOW_DOWN_RATE() const;
    bool SET_PITCH_SLOW_DOWN_RATE(const EA::Reflection::Float &input);

    bool PITCH_STOP_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool PITCH_STOP_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PITCH_STOP_SPEED() const;
    bool SET_PITCH_STOP_SPEED(const EA::Reflection::Float &input);

    bool ROLL_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_ALIGN_SCALE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_ALIGN_SCALE() const;
    bool SET_ROLL_ALIGN_SCALE(const EA::Reflection::Float &input);

    bool ROLL_ANG(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_ANG(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_ANG() const;
    bool SET_ROLL_ANG(const EA::Reflection::Float &input);

    bool ROLL_SLOW_DOWN_RATE(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_SLOW_DOWN_RATE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_SLOW_DOWN_RATE() const;
    bool SET_ROLL_SLOW_DOWN_RATE(const EA::Reflection::Float &input);

    bool ROLL_SPEED_MIN(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_SPEED_MIN(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_SPEED_MIN() const;
    bool SET_ROLL_SPEED_MIN(const EA::Reflection::Float &input);

    bool ROLL_START_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_START_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_START_SPEED() const;
    bool SET_ROLL_START_SPEED(const EA::Reflection::Float &input);

    bool SAME_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const;
    bool SAME_ALIGN_SCALE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SAME_ALIGN_SCALE() const;
    bool SET_SAME_ALIGN_SCALE(const EA::Reflection::Float &input);

    bool SCALE_STEERING(TAttrib<EA::Reflection::Bool> &result) const;
    bool SCALE_STEERING(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &SCALE_STEERING() const;
    bool SET_SCALE_STEERING(const EA::Reflection::Bool &input);

    bool STRAFE_SCALEX(TAttrib<EA::Reflection::Float> &result) const;
    bool STRAFE_SCALEX(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &STRAFE_SCALEX() const;
    bool SET_STRAFE_SCALEX(const EA::Reflection::Float &input);

    bool STRAFE_SCALEY(TAttrib<EA::Reflection::Float> &result) const;
    bool STRAFE_SCALEY(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &STRAFE_SCALEY() const;
    bool SET_STRAFE_SCALEY(const EA::Reflection::Float &input);

    bool TURN_BOOST_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool TURN_BOOST_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TURN_BOOST_SPEED() const;
    bool SET_TURN_BOOST_SPEED(const EA::Reflection::Float &input);

    bool YAW_BOOST_LIMIT(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_BOOST_LIMIT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_BOOST_LIMIT() const;
    bool SET_YAW_BOOST_LIMIT(const EA::Reflection::Float &input);

    bool YAW_DAMP(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_DAMP(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_DAMP() const;
    bool SET_YAW_DAMP(const EA::Reflection::Float &input);

    bool YAW_LIMIT_FRONT(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_LIMIT_FRONT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_LIMIT_FRONT() const;
    bool SET_YAW_LIMIT_FRONT(const EA::Reflection::Float &input);

    bool YAW_LIMIT_REAR(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_LIMIT_REAR(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_LIMIT_REAR() const;
    bool SET_YAW_LIMIT_REAR(const EA::Reflection::Float &input);

    bool YAW_STRENGTH_FRONT(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_STRENGTH_FRONT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_STRENGTH_FRONT() const;
    bool SET_YAW_STRENGTH_FRONT(const EA::Reflection::Float &input);

    bool YAW_STRENGTH_REAR(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_STRENGTH_REAR(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_STRENGTH_REAR() const;
    bool SET_YAW_STRENGTH_REAR(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    chopperspecs &ConvertFromInstance(Instance &src) {}
    const chopperspecs &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key chopperspecs = 0x5d898ee7;

}; // namespace ClassName

namespace Hash {
namespace chopperspecs {

static const Key AIR_RESISTANCE = 0xd28fd39c;
static const Key AIR_RESISTANCE_SCALE = 0x1807dfdf;
static const Key DRIVE_SPEED = 0x3244ec00;
static const Key MAX_SPEED_MPS = 0x9bbdb702;
static const Key PITCH_ALIGN_SCALE = 0x699b2b3c;
static const Key PITCH_ANG = 0x180485aa;
static const Key PITCH_SLOW_DOWN_RATE = 0x37894b0f;
static const Key PITCH_STOP_SPEED = 0x0e1da662;
static const Key ROLL_ALIGN_SCALE = 0x39950018;
static const Key ROLL_ANG = 0x287e9321;
static const Key ROLL_SLOW_DOWN_RATE = 0x62a08af6;
static const Key ROLL_SPEED_MIN = 0xd6c490c9;
static const Key ROLL_START_SPEED = 0xb2d7c5b8;
static const Key SAME_ALIGN_SCALE = 0x8b54c4ae;
static const Key SCALE_STEERING = 0x5c470cc8;
static const Key STRAFE_SCALEX = 0x90316cbc;
static const Key STRAFE_SCALEY = 0x1ebefd4a;
static const Key TURN_BOOST_SPEED = 0x21997946;
static const Key YAW_BOOST_LIMIT = 0x8509f111;
static const Key YAW_DAMP = 0xc68cae8f;
static const Key YAW_LIMIT_FRONT = 0x3e984672;
static const Key YAW_LIMIT_REAR = 0x09030d44;
static const Key YAW_STRENGTH_FRONT = 0x9656d8f8;
static const Key YAW_STRENGTH_REAR = 0x2da26212;

}; // namespace chopperspecs
}; // namespace Hash

inline Key Gen::chopperspecs::ClassKey() {
    return ClassName::chopperspecs;
}

inline bool Gen::chopperspecs::AIR_RESISTANCE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd28fd39c);
}
inline const EA::Reflection::Float &Gen::chopperspecs::AIR_RESISTANCE() const {
    ATTRIB_CODEGEN_GETLAYOUT(AIR_RESISTANCE);
}
inline bool Gen::chopperspecs::AIR_RESISTANCE(EA::Reflection::Float &result) const {
    result = AIR_RESISTANCE();
    return true;
}
inline bool Gen::chopperspecs::SET_AIR_RESISTANCE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AIR_RESISTANCE, input);
}

inline bool Gen::chopperspecs::AIR_RESISTANCE_SCALE(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1807dfdf);
}
inline const UMath::Vector4 &Gen::chopperspecs::AIR_RESISTANCE_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(AIR_RESISTANCE_SCALE);
}
inline bool Gen::chopperspecs::AIR_RESISTANCE_SCALE(UMath::Vector4 &result) const {
    result = AIR_RESISTANCE_SCALE();
    return true;
}
inline bool Gen::chopperspecs::SET_AIR_RESISTANCE_SCALE(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AIR_RESISTANCE_SCALE, input);
}

inline bool Gen::chopperspecs::DRIVE_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3244ec00);
}
inline const EA::Reflection::Float &Gen::chopperspecs::DRIVE_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(DRIVE_SPEED);
}
inline bool Gen::chopperspecs::DRIVE_SPEED(EA::Reflection::Float &result) const {
    result = DRIVE_SPEED();
    return true;
}
inline bool Gen::chopperspecs::SET_DRIVE_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DRIVE_SPEED, input);
}

inline bool Gen::chopperspecs::MAX_SPEED_MPS(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9bbdb702);
}
inline const EA::Reflection::Float &Gen::chopperspecs::MAX_SPEED_MPS() const {
    ATTRIB_CODEGEN_GETLAYOUT(MAX_SPEED_MPS);
}
inline bool Gen::chopperspecs::MAX_SPEED_MPS(EA::Reflection::Float &result) const {
    result = MAX_SPEED_MPS();
    return true;
}
inline bool Gen::chopperspecs::SET_MAX_SPEED_MPS(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MAX_SPEED_MPS, input);
}

inline bool Gen::chopperspecs::PITCH_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x699b2b3c);
}
inline const EA::Reflection::Float &Gen::chopperspecs::PITCH_ALIGN_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(PITCH_ALIGN_SCALE);
}
inline bool Gen::chopperspecs::PITCH_ALIGN_SCALE(EA::Reflection::Float &result) const {
    result = PITCH_ALIGN_SCALE();
    return true;
}
inline bool Gen::chopperspecs::SET_PITCH_ALIGN_SCALE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PITCH_ALIGN_SCALE, input);
}

inline bool Gen::chopperspecs::PITCH_ANG(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x180485aa);
}
inline const EA::Reflection::Float &Gen::chopperspecs::PITCH_ANG() const {
    ATTRIB_CODEGEN_GETLAYOUT(PITCH_ANG);
}
inline bool Gen::chopperspecs::PITCH_ANG(EA::Reflection::Float &result) const {
    result = PITCH_ANG();
    return true;
}
inline bool Gen::chopperspecs::SET_PITCH_ANG(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PITCH_ANG, input);
}

inline bool Gen::chopperspecs::PITCH_SLOW_DOWN_RATE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x37894b0f);
}
inline const EA::Reflection::Float &Gen::chopperspecs::PITCH_SLOW_DOWN_RATE() const {
    ATTRIB_CODEGEN_GETLAYOUT(PITCH_SLOW_DOWN_RATE);
}
inline bool Gen::chopperspecs::PITCH_SLOW_DOWN_RATE(EA::Reflection::Float &result) const {
    result = PITCH_SLOW_DOWN_RATE();
    return true;
}
inline bool Gen::chopperspecs::SET_PITCH_SLOW_DOWN_RATE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PITCH_SLOW_DOWN_RATE, input);
}

inline bool Gen::chopperspecs::PITCH_STOP_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0e1da662);
}
inline const EA::Reflection::Float &Gen::chopperspecs::PITCH_STOP_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(PITCH_STOP_SPEED);
}
inline bool Gen::chopperspecs::PITCH_STOP_SPEED(EA::Reflection::Float &result) const {
    result = PITCH_STOP_SPEED();
    return true;
}
inline bool Gen::chopperspecs::SET_PITCH_STOP_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PITCH_STOP_SPEED, input);
}

inline bool Gen::chopperspecs::ROLL_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x39950018);
}
inline const EA::Reflection::Float &Gen::chopperspecs::ROLL_ALIGN_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_ALIGN_SCALE);
}
inline bool Gen::chopperspecs::ROLL_ALIGN_SCALE(EA::Reflection::Float &result) const {
    result = ROLL_ALIGN_SCALE();
    return true;
}
inline bool Gen::chopperspecs::SET_ROLL_ALIGN_SCALE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_ALIGN_SCALE, input);
}

inline bool Gen::chopperspecs::ROLL_ANG(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x287e9321);
}
inline const EA::Reflection::Float &Gen::chopperspecs::ROLL_ANG() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_ANG);
}
inline bool Gen::chopperspecs::ROLL_ANG(EA::Reflection::Float &result) const {
    result = ROLL_ANG();
    return true;
}
inline bool Gen::chopperspecs::SET_ROLL_ANG(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_ANG, input);
}

inline bool Gen::chopperspecs::ROLL_SLOW_DOWN_RATE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x62a08af6);
}
inline const EA::Reflection::Float &Gen::chopperspecs::ROLL_SLOW_DOWN_RATE() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_SLOW_DOWN_RATE);
}
inline bool Gen::chopperspecs::ROLL_SLOW_DOWN_RATE(EA::Reflection::Float &result) const {
    result = ROLL_SLOW_DOWN_RATE();
    return true;
}
inline bool Gen::chopperspecs::SET_ROLL_SLOW_DOWN_RATE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_SLOW_DOWN_RATE, input);
}

inline bool Gen::chopperspecs::ROLL_SPEED_MIN(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd6c490c9);
}
inline const EA::Reflection::Float &Gen::chopperspecs::ROLL_SPEED_MIN() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_SPEED_MIN);
}
inline bool Gen::chopperspecs::ROLL_SPEED_MIN(EA::Reflection::Float &result) const {
    result = ROLL_SPEED_MIN();
    return true;
}
inline bool Gen::chopperspecs::SET_ROLL_SPEED_MIN(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_SPEED_MIN, input);
}

inline bool Gen::chopperspecs::ROLL_START_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb2d7c5b8);
}
inline const EA::Reflection::Float &Gen::chopperspecs::ROLL_START_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_START_SPEED);
}
inline bool Gen::chopperspecs::ROLL_START_SPEED(EA::Reflection::Float &result) const {
    result = ROLL_START_SPEED();
    return true;
}
inline bool Gen::chopperspecs::SET_ROLL_START_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_START_SPEED, input);
}

inline bool Gen::chopperspecs::SAME_ALIGN_SCALE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8b54c4ae);
}
inline const EA::Reflection::Float &Gen::chopperspecs::SAME_ALIGN_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(SAME_ALIGN_SCALE);
}
inline bool Gen::chopperspecs::SAME_ALIGN_SCALE(EA::Reflection::Float &result) const {
    result = SAME_ALIGN_SCALE();
    return true;
}
inline bool Gen::chopperspecs::SET_SAME_ALIGN_SCALE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SAME_ALIGN_SCALE, input);
}

inline bool Gen::chopperspecs::SCALE_STEERING(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x5c470cc8);
}
inline const EA::Reflection::Bool &Gen::chopperspecs::SCALE_STEERING() const {
    ATTRIB_CODEGEN_GETLAYOUT(SCALE_STEERING);
}
inline bool Gen::chopperspecs::SCALE_STEERING(EA::Reflection::Bool &result) const {
    result = SCALE_STEERING();
    return true;
}
inline bool Gen::chopperspecs::SET_SCALE_STEERING(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SCALE_STEERING, input);
}

inline bool Gen::chopperspecs::STRAFE_SCALEX(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x90316cbc);
}
inline const EA::Reflection::Float &Gen::chopperspecs::STRAFE_SCALEX() const {
    ATTRIB_CODEGEN_GETLAYOUT(STRAFE_SCALEX);
}
inline bool Gen::chopperspecs::STRAFE_SCALEX(EA::Reflection::Float &result) const {
    result = STRAFE_SCALEX();
    return true;
}
inline bool Gen::chopperspecs::SET_STRAFE_SCALEX(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(STRAFE_SCALEX, input);
}

inline bool Gen::chopperspecs::STRAFE_SCALEY(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1ebefd4a);
}
inline const EA::Reflection::Float &Gen::chopperspecs::STRAFE_SCALEY() const {
    ATTRIB_CODEGEN_GETLAYOUT(STRAFE_SCALEY);
}
inline bool Gen::chopperspecs::STRAFE_SCALEY(EA::Reflection::Float &result) const {
    result = STRAFE_SCALEY();
    return true;
}
inline bool Gen::chopperspecs::SET_STRAFE_SCALEY(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(STRAFE_SCALEY, input);
}

inline bool Gen::chopperspecs::TURN_BOOST_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x21997946);
}
inline const EA::Reflection::Float &Gen::chopperspecs::TURN_BOOST_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(TURN_BOOST_SPEED);
}
inline bool Gen::chopperspecs::TURN_BOOST_SPEED(EA::Reflection::Float &result) const {
    result = TURN_BOOST_SPEED();
    return true;
}
inline bool Gen::chopperspecs::SET_TURN_BOOST_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TURN_BOOST_SPEED, input);
}

inline bool Gen::chopperspecs::YAW_BOOST_LIMIT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8509f111);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_BOOST_LIMIT() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_BOOST_LIMIT);
}
inline bool Gen::chopperspecs::YAW_BOOST_LIMIT(EA::Reflection::Float &result) const {
    result = YAW_BOOST_LIMIT();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_BOOST_LIMIT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_BOOST_LIMIT, input);
}

inline bool Gen::chopperspecs::YAW_DAMP(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc68cae8f);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_DAMP() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_DAMP);
}
inline bool Gen::chopperspecs::YAW_DAMP(EA::Reflection::Float &result) const {
    result = YAW_DAMP();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_DAMP(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_DAMP, input);
}

inline bool Gen::chopperspecs::YAW_LIMIT_FRONT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3e984672);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_LIMIT_FRONT() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_LIMIT_FRONT);
}
inline bool Gen::chopperspecs::YAW_LIMIT_FRONT(EA::Reflection::Float &result) const {
    result = YAW_LIMIT_FRONT();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_LIMIT_FRONT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_LIMIT_FRONT, input);
}

inline bool Gen::chopperspecs::YAW_LIMIT_REAR(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x09030d44);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_LIMIT_REAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_LIMIT_REAR);
}
inline bool Gen::chopperspecs::YAW_LIMIT_REAR(EA::Reflection::Float &result) const {
    result = YAW_LIMIT_REAR();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_LIMIT_REAR(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_LIMIT_REAR, input);
}

inline bool Gen::chopperspecs::YAW_STRENGTH_FRONT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9656d8f8);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_STRENGTH_FRONT() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_STRENGTH_FRONT);
}
inline bool Gen::chopperspecs::YAW_STRENGTH_FRONT(EA::Reflection::Float &result) const {
    result = YAW_STRENGTH_FRONT();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_STRENGTH_FRONT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_STRENGTH_FRONT, input);
}

inline bool Gen::chopperspecs::YAW_STRENGTH_REAR(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2da26212);
}
inline const EA::Reflection::Float &Gen::chopperspecs::YAW_STRENGTH_REAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_STRENGTH_REAR);
}
inline bool Gen::chopperspecs::YAW_STRENGTH_REAR(EA::Reflection::Float &result) const {
    result = YAW_STRENGTH_REAR();
    return true;
}
inline bool Gen::chopperspecs::SET_YAW_STRENGTH_REAR(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_STRENGTH_REAR, input);
}

}; // namespace Attrib

#endif
