#ifndef _attrib_gen_chassis_h
#define _attrib_gen_chassis_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct chassis : Instance {
    struct _LayoutStruct {
        AxlePair SHOCK_DIGRESSION;               // offset 0x0, size 0x8
        AxlePair SPRING_PROGRESSION;             // offset 0x8, size 0x8
        AxlePair TRAVEL;                         // offset 0x10, size 0x8
        AxlePair RIDE_HEIGHT;                    // offset 0x18, size 0x8
        AxlePair TRACK_WIDTH;                    // offset 0x20, size 0x8
        AxlePair SHOCK_EXT_STIFFNESS;            // offset 0x28, size 0x8
        AxlePair SHOCK_STIFFNESS;                // offset 0x30, size 0x8
        AxlePair SPRING_STIFFNESS;               // offset 0x38, size 0x8
        AxlePair SHOCK_VALVING;                  // offset 0x40, size 0x8
        AxlePair SWAYBAR_STIFFNESS;              // offset 0x48, size 0x8
        EA::Reflection::Float ROLL_CENTER;       // offset 0x50, size 0x4
        EA::Reflection::Float WHEEL_BASE;        // offset 0x54, size 0x4
        EA::Reflection::Float SHOCK_BLOWOUT;     // offset 0x58, size 0x4
        EA::Reflection::Float AERO_CG;           // offset 0x5c, size 0x4
        EA::Reflection::Float RENDER_MOTION;     // offset 0x60, size 0x4
        EA::Reflection::Float FRONT_AXLE;        // offset 0x64, size 0x4
        EA::Reflection::Float AERO_COEFFICIENT;  // offset 0x68, size 0x4
        EA::Reflection::Float FRONT_WEIGHT_BIAS; // offset 0x6c, size 0x4
        EA::Reflection::Float DRAG_COEFFICIENT;  // offset 0x70, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_AERO_CG;
    typedef EA::Reflection::Float TypeOf_AERO_COEFFICIENT;
    typedef EA::Reflection::Float TypeOf_DRAG_COEFFICIENT;
    typedef EA::Reflection::Float TypeOf_FRONT_AXLE;
    typedef EA::Reflection::Float TypeOf_FRONT_WEIGHT_BIAS;
    typedef EA::Reflection::Float TypeOf_RENDER_MOTION;
    typedef AxlePair TypeOf_RIDE_HEIGHT;
    typedef EA::Reflection::Float TypeOf_ROLL_CENTER;
    typedef EA::Reflection::Float TypeOf_SHOCK_BLOWOUT;
    typedef AxlePair TypeOf_SHOCK_DIGRESSION;
    typedef AxlePair TypeOf_SHOCK_EXT_STIFFNESS;
    typedef AxlePair TypeOf_SHOCK_STIFFNESS;
    typedef AxlePair TypeOf_SHOCK_VALVING;
    typedef AxlePair TypeOf_SPRING_PROGRESSION;
    typedef AxlePair TypeOf_SPRING_STIFFNESS;
    typedef AxlePair TypeOf_SWAYBAR_STIFFNESS;
    typedef AxlePair TypeOf_TRACK_WIDTH;
    typedef AxlePair TypeOf_TRAVEL;
    typedef EA::Reflection::Float TypeOf_WHEEL_BASE;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("chassis");
    chassis(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chassis(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chassis(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chassis(const chassis &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    chassis(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~chassis() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xafa210f0;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xafa210f0, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const chassis &operator=(const chassis &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const chassis &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AERO_CG(TAttrib<EA::Reflection::Float> &result) const;
    bool AERO_CG(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AERO_CG() const;
    bool SET_AERO_CG(const EA::Reflection::Float &input);

    bool AERO_COEFFICIENT(TAttrib<EA::Reflection::Float> &result) const;
    bool AERO_COEFFICIENT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AERO_COEFFICIENT() const;
    bool SET_AERO_COEFFICIENT(const EA::Reflection::Float &input);

    bool DRAG_COEFFICIENT(TAttrib<EA::Reflection::Float> &result) const;
    bool DRAG_COEFFICIENT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DRAG_COEFFICIENT() const;
    bool SET_DRAG_COEFFICIENT(const EA::Reflection::Float &input);

    bool FRONT_AXLE(TAttrib<EA::Reflection::Float> &result) const;
    bool FRONT_AXLE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FRONT_AXLE() const;
    bool SET_FRONT_AXLE(const EA::Reflection::Float &input);

    bool FRONT_WEIGHT_BIAS(TAttrib<EA::Reflection::Float> &result) const;
    bool FRONT_WEIGHT_BIAS(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FRONT_WEIGHT_BIAS() const;
    bool SET_FRONT_WEIGHT_BIAS(const EA::Reflection::Float &input);

    bool RENDER_MOTION(TAttrib<EA::Reflection::Float> &result) const;
    bool RENDER_MOTION(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RENDER_MOTION() const;
    bool SET_RENDER_MOTION(const EA::Reflection::Float &input);

    bool RIDE_HEIGHT(TAttrib<AxlePair> &result) const;
    bool RIDE_HEIGHT(AxlePair &result) const;
    const AxlePair &RIDE_HEIGHT() const;
    bool SET_RIDE_HEIGHT(const AxlePair &input);

    bool ROLL_CENTER(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLL_CENTER(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLL_CENTER() const;
    bool SET_ROLL_CENTER(const EA::Reflection::Float &input);

    bool SHOCK_BLOWOUT(TAttrib<EA::Reflection::Float> &result) const;
    bool SHOCK_BLOWOUT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SHOCK_BLOWOUT() const;
    bool SET_SHOCK_BLOWOUT(const EA::Reflection::Float &input);

    bool SHOCK_DIGRESSION(TAttrib<AxlePair> &result) const;
    bool SHOCK_DIGRESSION(AxlePair &result) const;
    const AxlePair &SHOCK_DIGRESSION() const;
    bool SET_SHOCK_DIGRESSION(const AxlePair &input);

    bool SHOCK_EXT_STIFFNESS(TAttrib<AxlePair> &result) const;
    bool SHOCK_EXT_STIFFNESS(AxlePair &result) const;
    const AxlePair &SHOCK_EXT_STIFFNESS() const;
    bool SET_SHOCK_EXT_STIFFNESS(const AxlePair &input);

    bool SHOCK_STIFFNESS(TAttrib<AxlePair> &result) const;
    bool SHOCK_STIFFNESS(AxlePair &result) const;
    const AxlePair &SHOCK_STIFFNESS() const;
    bool SET_SHOCK_STIFFNESS(const AxlePair &input);

    bool SHOCK_VALVING(TAttrib<AxlePair> &result) const;
    bool SHOCK_VALVING(AxlePair &result) const;
    const AxlePair &SHOCK_VALVING() const;
    bool SET_SHOCK_VALVING(const AxlePair &input);

    bool SPRING_PROGRESSION(TAttrib<AxlePair> &result) const;
    bool SPRING_PROGRESSION(AxlePair &result) const;
    const AxlePair &SPRING_PROGRESSION() const;
    bool SET_SPRING_PROGRESSION(const AxlePair &input);

    bool SPRING_STIFFNESS(TAttrib<AxlePair> &result) const;
    bool SPRING_STIFFNESS(AxlePair &result) const;
    const AxlePair &SPRING_STIFFNESS() const;
    bool SET_SPRING_STIFFNESS(const AxlePair &input);

    bool SWAYBAR_STIFFNESS(TAttrib<AxlePair> &result) const;
    bool SWAYBAR_STIFFNESS(AxlePair &result) const;
    const AxlePair &SWAYBAR_STIFFNESS() const;
    bool SET_SWAYBAR_STIFFNESS(const AxlePair &input);

    bool TRACK_WIDTH(TAttrib<AxlePair> &result) const;
    bool TRACK_WIDTH(AxlePair &result) const;
    const AxlePair &TRACK_WIDTH() const;
    bool SET_TRACK_WIDTH(const AxlePair &input);

    bool TRAVEL(TAttrib<AxlePair> &result) const;
    bool TRAVEL(AxlePair &result) const;
    const AxlePair &TRAVEL() const;
    bool SET_TRAVEL(const AxlePair &input);

    bool WHEEL_BASE(TAttrib<EA::Reflection::Float> &result) const;
    bool WHEEL_BASE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &WHEEL_BASE() const;
    bool SET_WHEEL_BASE(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    chassis &ConvertFromInstance(Instance &src) {}
    const chassis &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key chassis = 0xafa210f0;

}; // namespace ClassName

namespace Hash {
namespace chassis {

static const Key AERO_CG = 0xa073de78;
static const Key AERO_COEFFICIENT = 0xc29b0f17;
static const Key DRAG_COEFFICIENT = 0x3c860745;
static const Key FRONT_AXLE = 0xd8cbcc48;
static const Key FRONT_WEIGHT_BIAS = 0xe56dee24;
static const Key RENDER_MOTION = 0xb3d3e7d5;
static const Key RIDE_HEIGHT = 0x46c189b0;
static const Key ROLL_CENTER = 0x6702cff9;
static const Key SHOCK_BLOWOUT = 0x54536d38;
static const Key SHOCK_DIGRESSION = 0xaa13ae7b;
static const Key SHOCK_EXT_STIFFNESS = 0x54680830;
static const Key SHOCK_STIFFNESS = 0x4bc16211;
static const Key SHOCK_VALVING = 0x42c42b3f;
static const Key SPRING_PROGRESSION = 0xa0b49fc2;
static const Key SPRING_STIFFNESS = 0x802e05df;
static const Key SWAYBAR_STIFFNESS = 0x61ca75f2;
static const Key TRACK_WIDTH = 0xa738aedf;
static const Key TRAVEL = 0x7e01b96d;
static const Key WHEEL_BASE = 0x2d1375a1;

}; // namespace chassis
}; // namespace Hash

inline Key Gen::chassis::ClassKey() {
    return ClassName::chassis;
}

inline bool Gen::chassis::AERO_CG(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa073de78);
}
inline const EA::Reflection::Float &Gen::chassis::AERO_CG() const {
    ATTRIB_CODEGEN_GETLAYOUT(AERO_CG);
}
inline bool Gen::chassis::AERO_CG(EA::Reflection::Float &result) const {
    result = AERO_CG();
    return true;
}
inline bool Gen::chassis::SET_AERO_CG(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AERO_CG, input);
}

inline bool Gen::chassis::AERO_COEFFICIENT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc29b0f17);
}
inline const EA::Reflection::Float &Gen::chassis::AERO_COEFFICIENT() const {
    ATTRIB_CODEGEN_GETLAYOUT(AERO_COEFFICIENT);
}
inline bool Gen::chassis::AERO_COEFFICIENT(EA::Reflection::Float &result) const {
    result = AERO_COEFFICIENT();
    return true;
}
inline bool Gen::chassis::SET_AERO_COEFFICIENT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AERO_COEFFICIENT, input);
}

inline bool Gen::chassis::DRAG_COEFFICIENT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c860745);
}
inline const EA::Reflection::Float &Gen::chassis::DRAG_COEFFICIENT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DRAG_COEFFICIENT);
}
inline bool Gen::chassis::DRAG_COEFFICIENT(EA::Reflection::Float &result) const {
    result = DRAG_COEFFICIENT();
    return true;
}
inline bool Gen::chassis::SET_DRAG_COEFFICIENT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DRAG_COEFFICIENT, input);
}

inline bool Gen::chassis::FRONT_AXLE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd8cbcc48);
}
inline const EA::Reflection::Float &Gen::chassis::FRONT_AXLE() const {
    ATTRIB_CODEGEN_GETLAYOUT(FRONT_AXLE);
}
inline bool Gen::chassis::FRONT_AXLE(EA::Reflection::Float &result) const {
    result = FRONT_AXLE();
    return true;
}
inline bool Gen::chassis::SET_FRONT_AXLE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FRONT_AXLE, input);
}

inline bool Gen::chassis::FRONT_WEIGHT_BIAS(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe56dee24);
}
inline const EA::Reflection::Float &Gen::chassis::FRONT_WEIGHT_BIAS() const {
    ATTRIB_CODEGEN_GETLAYOUT(FRONT_WEIGHT_BIAS);
}
inline bool Gen::chassis::FRONT_WEIGHT_BIAS(EA::Reflection::Float &result) const {
    result = FRONT_WEIGHT_BIAS();
    return true;
}
inline bool Gen::chassis::SET_FRONT_WEIGHT_BIAS(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FRONT_WEIGHT_BIAS, input);
}

inline bool Gen::chassis::RENDER_MOTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb3d3e7d5);
}
inline const EA::Reflection::Float &Gen::chassis::RENDER_MOTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(RENDER_MOTION);
}
inline bool Gen::chassis::RENDER_MOTION(EA::Reflection::Float &result) const {
    result = RENDER_MOTION();
    return true;
}
inline bool Gen::chassis::SET_RENDER_MOTION(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RENDER_MOTION, input);
}

inline bool Gen::chassis::RIDE_HEIGHT(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x46c189b0);
}
inline const AxlePair &Gen::chassis::RIDE_HEIGHT() const {
    ATTRIB_CODEGEN_GETLAYOUT(RIDE_HEIGHT);
}
inline bool Gen::chassis::RIDE_HEIGHT(AxlePair &result) const {
    result = RIDE_HEIGHT();
    return true;
}
inline bool Gen::chassis::SET_RIDE_HEIGHT(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RIDE_HEIGHT, input);
}

inline bool Gen::chassis::ROLL_CENTER(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6702cff9);
}
inline const EA::Reflection::Float &Gen::chassis::ROLL_CENTER() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLL_CENTER);
}
inline bool Gen::chassis::ROLL_CENTER(EA::Reflection::Float &result) const {
    result = ROLL_CENTER();
    return true;
}
inline bool Gen::chassis::SET_ROLL_CENTER(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLL_CENTER, input);
}

inline bool Gen::chassis::SHOCK_BLOWOUT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x54536d38);
}
inline const EA::Reflection::Float &Gen::chassis::SHOCK_BLOWOUT() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_BLOWOUT);
}
inline bool Gen::chassis::SHOCK_BLOWOUT(EA::Reflection::Float &result) const {
    result = SHOCK_BLOWOUT();
    return true;
}
inline bool Gen::chassis::SET_SHOCK_BLOWOUT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_BLOWOUT, input);
}

inline bool Gen::chassis::SHOCK_DIGRESSION(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xaa13ae7b);
}
inline const AxlePair &Gen::chassis::SHOCK_DIGRESSION() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_DIGRESSION);
}
inline bool Gen::chassis::SHOCK_DIGRESSION(AxlePair &result) const {
    result = SHOCK_DIGRESSION();
    return true;
}
inline bool Gen::chassis::SET_SHOCK_DIGRESSION(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_DIGRESSION, input);
}

inline bool Gen::chassis::SHOCK_EXT_STIFFNESS(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x54680830);
}
inline const AxlePair &Gen::chassis::SHOCK_EXT_STIFFNESS() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_EXT_STIFFNESS);
}
inline bool Gen::chassis::SHOCK_EXT_STIFFNESS(AxlePair &result) const {
    result = SHOCK_EXT_STIFFNESS();
    return true;
}
inline bool Gen::chassis::SET_SHOCK_EXT_STIFFNESS(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_EXT_STIFFNESS, input);
}

inline bool Gen::chassis::SHOCK_STIFFNESS(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x4bc16211);
}
inline const AxlePair &Gen::chassis::SHOCK_STIFFNESS() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_STIFFNESS);
}
inline bool Gen::chassis::SHOCK_STIFFNESS(AxlePair &result) const {
    result = SHOCK_STIFFNESS();
    return true;
}
inline bool Gen::chassis::SET_SHOCK_STIFFNESS(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_STIFFNESS, input);
}

inline bool Gen::chassis::SHOCK_VALVING(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x42c42b3f);
}
inline const AxlePair &Gen::chassis::SHOCK_VALVING() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_VALVING);
}
inline bool Gen::chassis::SHOCK_VALVING(AxlePair &result) const {
    result = SHOCK_VALVING();
    return true;
}
inline bool Gen::chassis::SET_SHOCK_VALVING(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_VALVING, input);
}

inline bool Gen::chassis::SPRING_PROGRESSION(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xa0b49fc2);
}
inline const AxlePair &Gen::chassis::SPRING_PROGRESSION() const {
    ATTRIB_CODEGEN_GETLAYOUT(SPRING_PROGRESSION);
}
inline bool Gen::chassis::SPRING_PROGRESSION(AxlePair &result) const {
    result = SPRING_PROGRESSION();
    return true;
}
inline bool Gen::chassis::SET_SPRING_PROGRESSION(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SPRING_PROGRESSION, input);
}

inline bool Gen::chassis::SPRING_STIFFNESS(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x802e05df);
}
inline const AxlePair &Gen::chassis::SPRING_STIFFNESS() const {
    ATTRIB_CODEGEN_GETLAYOUT(SPRING_STIFFNESS);
}
inline bool Gen::chassis::SPRING_STIFFNESS(AxlePair &result) const {
    result = SPRING_STIFFNESS();
    return true;
}
inline bool Gen::chassis::SET_SPRING_STIFFNESS(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SPRING_STIFFNESS, input);
}

inline bool Gen::chassis::SWAYBAR_STIFFNESS(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x61ca75f2);
}
inline const AxlePair &Gen::chassis::SWAYBAR_STIFFNESS() const {
    ATTRIB_CODEGEN_GETLAYOUT(SWAYBAR_STIFFNESS);
}
inline bool Gen::chassis::SWAYBAR_STIFFNESS(AxlePair &result) const {
    result = SWAYBAR_STIFFNESS();
    return true;
}
inline bool Gen::chassis::SET_SWAYBAR_STIFFNESS(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SWAYBAR_STIFFNESS, input);
}

inline bool Gen::chassis::TRACK_WIDTH(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xa738aedf);
}
inline const AxlePair &Gen::chassis::TRACK_WIDTH() const {
    ATTRIB_CODEGEN_GETLAYOUT(TRACK_WIDTH);
}
inline bool Gen::chassis::TRACK_WIDTH(AxlePair &result) const {
    result = TRACK_WIDTH();
    return true;
}
inline bool Gen::chassis::SET_TRACK_WIDTH(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TRACK_WIDTH, input);
}

inline bool Gen::chassis::TRAVEL(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x7e01b96d);
}
inline const AxlePair &Gen::chassis::TRAVEL() const {
    ATTRIB_CODEGEN_GETLAYOUT(TRAVEL);
}
inline bool Gen::chassis::TRAVEL(AxlePair &result) const {
    result = TRAVEL();
    return true;
}
inline bool Gen::chassis::SET_TRAVEL(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TRAVEL, input);
}

inline bool Gen::chassis::WHEEL_BASE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2d1375a1);
}
inline const EA::Reflection::Float &Gen::chassis::WHEEL_BASE() const {
    ATTRIB_CODEGEN_GETLAYOUT(WHEEL_BASE);
}
inline bool Gen::chassis::WHEEL_BASE(EA::Reflection::Float &result) const {
    result = WHEEL_BASE();
    return true;
}
inline bool Gen::chassis::SET_WHEEL_BASE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WHEEL_BASE, input);
}

}; // namespace Attrib

#endif
