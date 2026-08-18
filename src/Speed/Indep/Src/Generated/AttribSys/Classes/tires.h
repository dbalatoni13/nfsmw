#ifndef _attrib_gen_tires_h
#define _attrib_gen_tires_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct tires : Instance {
    struct _LayoutStruct {
        Private _Array_YAW_CONTROL;           // offset 0x0, size 0x8
        EA::Reflection::Float YAW_CONTROL[4]; // offset 0x8, size 0x10
        AxlePair GRIP_SCALE;                  // offset 0x18, size 0x8
        AxlePair DYNAMIC_GRIP;                // offset 0x20, size 0x8
        AxlePair ASPECT_RATIO;                // offset 0x28, size 0x8
        AxlePair RIM_SIZE;                    // offset 0x30, size 0x8
        AxlePair STATIC_GRIP;                 // offset 0x38, size 0x8
        AxlePair SECTION_WIDTH;               // offset 0x40, size 0x8
        EA::Reflection::Float STEERING;       // offset 0x48, size 0x4
        EA::Reflection::Float YAW_SPEED;      // offset 0x4c, size 0x4
    };

    typedef AxlePair TypeOf_ASPECT_RATIO;
    typedef AxlePair TypeOf_DYNAMIC_GRIP;
    typedef AxlePair TypeOf_GRIP_SCALE;
    typedef AxlePair TypeOf_RIM_SIZE;
    typedef AxlePair TypeOf_SECTION_WIDTH;
    typedef AxlePair TypeOf_STATIC_GRIP;
    typedef EA::Reflection::Float TypeOf_STEERING;
    typedef EA::Reflection::Float TypeOf_YAW_CONTROL;
    typedef EA::Reflection::Float TypeOf_YAW_SPEED;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("tires");
    tires(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    tires(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    tires(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    tires(const tires &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    tires(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~tires() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xbd38d1ca;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xbd38d1ca, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const tires &operator=(const tires &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const tires &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool ASPECT_RATIO(TAttrib<AxlePair> &result) const;
    bool ASPECT_RATIO(AxlePair &result) const;
    const AxlePair &ASPECT_RATIO() const;
    bool SET_ASPECT_RATIO(const AxlePair &input);

    bool DYNAMIC_GRIP(TAttrib<AxlePair> &result) const;
    bool DYNAMIC_GRIP(AxlePair &result) const;
    const AxlePair &DYNAMIC_GRIP() const;
    bool SET_DYNAMIC_GRIP(const AxlePair &input);

    bool GRIP_SCALE(TAttrib<AxlePair> &result) const;
    bool GRIP_SCALE(AxlePair &result) const;
    const AxlePair &GRIP_SCALE() const;
    bool SET_GRIP_SCALE(const AxlePair &input);

    bool RIM_SIZE(TAttrib<AxlePair> &result) const;
    bool RIM_SIZE(AxlePair &result) const;
    const AxlePair &RIM_SIZE() const;
    bool SET_RIM_SIZE(const AxlePair &input);

    bool SECTION_WIDTH(TAttrib<AxlePair> &result) const;
    bool SECTION_WIDTH(AxlePair &result) const;
    const AxlePair &SECTION_WIDTH() const;
    bool SET_SECTION_WIDTH(const AxlePair &input);

    bool STATIC_GRIP(TAttrib<AxlePair> &result) const;
    bool STATIC_GRIP(AxlePair &result) const;
    const AxlePair &STATIC_GRIP() const;
    bool SET_STATIC_GRIP(const AxlePair &input);

    bool STEERING(TAttrib<EA::Reflection::Float> &result) const;
    bool STEERING(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &STEERING() const;
    bool SET_STEERING(const EA::Reflection::Float &input);

    bool YAW_CONTROL(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_CONTROL(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &YAW_CONTROL(unsigned int index) const;
    unsigned int Num_YAW_CONTROL() const;
    bool SET_YAW_CONTROL(const EA::Reflection::Float &input, unsigned int index);

    bool YAW_SPEED(TAttrib<EA::Reflection::Float> &result) const;
    bool YAW_SPEED(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &YAW_SPEED() const;
    bool SET_YAW_SPEED(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    tires &ConvertFromInstance(Instance &src) {}
    const tires &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key tires = 0xbd38d1ca;

}; // namespace ClassName

namespace Hash {
namespace tires {

static const Key ASPECT_RATIO = 0xdefc0b82;
static const Key DYNAMIC_GRIP = 0xaafc01b3;
static const Key GRIP_SCALE = 0xf177be1b;
static const Key RIM_SIZE = 0xb698be9f;
static const Key SECTION_WIDTH = 0x5161d2ee;
static const Key STATIC_GRIP = 0xec096166;
static const Key STEERING = 0xfef5cc35;
static const Key YAW_CONTROL = 0x64c43c4b;
static const Key YAW_SPEED = 0xc2094707;

}; // namespace tires
}; // namespace Hash

inline Key Gen::tires::ClassKey() {
    return ClassName::tires;
}

inline bool Gen::tires::ASPECT_RATIO(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xdefc0b82);
}
inline const AxlePair &Gen::tires::ASPECT_RATIO() const {
    ATTRIB_CODEGEN_GETLAYOUT(ASPECT_RATIO);
}
inline bool Gen::tires::ASPECT_RATIO(AxlePair &result) const {
    result = ASPECT_RATIO();
    return true;
}
inline bool Gen::tires::SET_ASPECT_RATIO(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ASPECT_RATIO, input);
}

inline bool Gen::tires::DYNAMIC_GRIP(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xaafc01b3);
}
inline const AxlePair &Gen::tires::DYNAMIC_GRIP() const {
    ATTRIB_CODEGEN_GETLAYOUT(DYNAMIC_GRIP);
}
inline bool Gen::tires::DYNAMIC_GRIP(AxlePair &result) const {
    result = DYNAMIC_GRIP();
    return true;
}
inline bool Gen::tires::SET_DYNAMIC_GRIP(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DYNAMIC_GRIP, input);
}

inline bool Gen::tires::GRIP_SCALE(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xf177be1b);
}
inline const AxlePair &Gen::tires::GRIP_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(GRIP_SCALE);
}
inline bool Gen::tires::GRIP_SCALE(AxlePair &result) const {
    result = GRIP_SCALE();
    return true;
}
inline bool Gen::tires::SET_GRIP_SCALE(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GRIP_SCALE, input);
}

inline bool Gen::tires::RIM_SIZE(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xb698be9f);
}
inline const AxlePair &Gen::tires::RIM_SIZE() const {
    ATTRIB_CODEGEN_GETLAYOUT(RIM_SIZE);
}
inline bool Gen::tires::RIM_SIZE(AxlePair &result) const {
    result = RIM_SIZE();
    return true;
}
inline bool Gen::tires::SET_RIM_SIZE(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RIM_SIZE, input);
}

inline bool Gen::tires::SECTION_WIDTH(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0x5161d2ee);
}
inline const AxlePair &Gen::tires::SECTION_WIDTH() const {
    ATTRIB_CODEGEN_GETLAYOUT(SECTION_WIDTH);
}
inline bool Gen::tires::SECTION_WIDTH(AxlePair &result) const {
    result = SECTION_WIDTH();
    return true;
}
inline bool Gen::tires::SET_SECTION_WIDTH(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SECTION_WIDTH, input);
}

inline bool Gen::tires::STATIC_GRIP(TAttrib<AxlePair> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AxlePair, 0xec096166);
}
inline const AxlePair &Gen::tires::STATIC_GRIP() const {
    ATTRIB_CODEGEN_GETLAYOUT(STATIC_GRIP);
}
inline bool Gen::tires::STATIC_GRIP(AxlePair &result) const {
    result = STATIC_GRIP();
    return true;
}
inline bool Gen::tires::SET_STATIC_GRIP(const AxlePair &input) {
    ATTRIB_CODEGEN_SETLAYOUT(STATIC_GRIP, input);
}

inline bool Gen::tires::STEERING(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfef5cc35);
}
inline const EA::Reflection::Float &Gen::tires::STEERING() const {
    ATTRIB_CODEGEN_GETLAYOUT(STEERING);
}
inline bool Gen::tires::STEERING(EA::Reflection::Float &result) const {
    result = STEERING();
    return true;
}
inline bool Gen::tires::SET_STEERING(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(STEERING, input);
}

inline bool Gen::tires::YAW_CONTROL(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x64c43c4b);
}
inline const EA::Reflection::Float &Gen::tires::YAW_CONTROL(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, YAW_CONTROL, index);
}
inline bool Gen::tires::YAW_CONTROL(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(YAW_CONTROL, result, index);
}
inline unsigned int Gen::tires::Num_YAW_CONTROL() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(YAW_CONTROL);
}
inline bool Gen::tires::SET_YAW_CONTROL(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(YAW_CONTROL, input, index);
}

inline bool Gen::tires::YAW_SPEED(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc2094707);
}
inline const EA::Reflection::Float &Gen::tires::YAW_SPEED() const {
    ATTRIB_CODEGEN_GETLAYOUT(YAW_SPEED);
}
inline bool Gen::tires::YAW_SPEED(EA::Reflection::Float &result) const {
    result = YAW_SPEED();
    return true;
}
inline bool Gen::tires::SET_YAW_SPEED(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(YAW_SPEED, input);
}

}; // namespace Attrib

#endif
