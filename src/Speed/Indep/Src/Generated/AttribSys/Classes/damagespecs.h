#ifndef _attrib_gen_damagespecs_h
#define _attrib_gen_damagespecs_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct damagespecs : Instance {
    struct _LayoutStruct {
        DamageScaleRecord DZ_LFRONT;         // offset 0x0, size 0x8
        DamageScaleRecord DZ_FRONT;          // offset 0x8, size 0x8
        DamageScaleRecord DZ_BOTTOM;         // offset 0x10, size 0x8
        DamageScaleRecord DZ_LEFT;           // offset 0x18, size 0x8
        DamageScaleRecord DZ_RREAR;          // offset 0x20, size 0x8
        DamageScaleRecord DZ_LREAR;          // offset 0x28, size 0x8
        DamageScaleRecord DZ_RIGHT;          // offset 0x30, size 0x8
        DamageScaleRecord DZ_REAR;           // offset 0x38, size 0x8
        DamageScaleRecord DZ_TOP;            // offset 0x40, size 0x8
        DamageScaleRecord DZ_RFRONT;         // offset 0x48, size 0x8
        EA::Reflection::Float SHOCK_TIME;    // offset 0x50, size 0x4
        EA::Reflection::Float HP_THRESHOLD;  // offset 0x54, size 0x4
        EA::Reflection::Float SUPPRESS_DIST; // offset 0x58, size 0x4
        EA::Reflection::Float SHOCK_FORCE;   // offset 0x5c, size 0x4
        EA::Reflection::Float FORCE;         // offset 0x60, size 0x4
        EA::Reflection::Float HIT_POINTS;    // offset 0x64, size 0x4
    };

    typedef DamageScaleRecord TypeOf_DZ_BOTTOM;
    typedef DamageScaleRecord TypeOf_DZ_FRONT;
    typedef DamageScaleRecord TypeOf_DZ_LEFT;
    typedef DamageScaleRecord TypeOf_DZ_LFRONT;
    typedef DamageScaleRecord TypeOf_DZ_LREAR;
    typedef DamageScaleRecord TypeOf_DZ_REAR;
    typedef DamageScaleRecord TypeOf_DZ_RFRONT;
    typedef DamageScaleRecord TypeOf_DZ_RIGHT;
    typedef DamageScaleRecord TypeOf_DZ_RREAR;
    typedef DamageScaleRecord TypeOf_DZ_TOP;
    typedef EA::Reflection::Float TypeOf_FORCE;
    typedef EA::Reflection::Float TypeOf_HIT_POINTS;
    typedef EA::Reflection::Float TypeOf_HP_THRESHOLD;
    typedef EA::Reflection::Float TypeOf_SHOCK_FORCE;
    typedef EA::Reflection::Float TypeOf_SHOCK_TIME;
    typedef EA::Reflection::Float TypeOf_SUPPRESS_DIST;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("damagespecs");
    damagespecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    damagespecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    damagespecs(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    damagespecs(const damagespecs &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    damagespecs(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~damagespecs() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xc1f0b434;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xc1f0b434, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const damagespecs &operator=(const damagespecs &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const damagespecs &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool DZ_BOTTOM(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_BOTTOM(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_BOTTOM() const;
    bool SET_DZ_BOTTOM(const DamageScaleRecord &input);

    bool DZ_FRONT(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_FRONT(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_FRONT() const;
    bool SET_DZ_FRONT(const DamageScaleRecord &input);

    bool DZ_LEFT(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_LEFT(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_LEFT() const;
    bool SET_DZ_LEFT(const DamageScaleRecord &input);

    bool DZ_LFRONT(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_LFRONT(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_LFRONT() const;
    bool SET_DZ_LFRONT(const DamageScaleRecord &input);

    bool DZ_LREAR(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_LREAR(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_LREAR() const;
    bool SET_DZ_LREAR(const DamageScaleRecord &input);

    bool DZ_REAR(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_REAR(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_REAR() const;
    bool SET_DZ_REAR(const DamageScaleRecord &input);

    bool DZ_RFRONT(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_RFRONT(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_RFRONT() const;
    bool SET_DZ_RFRONT(const DamageScaleRecord &input);

    bool DZ_RIGHT(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_RIGHT(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_RIGHT() const;
    bool SET_DZ_RIGHT(const DamageScaleRecord &input);

    bool DZ_RREAR(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_RREAR(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_RREAR() const;
    bool SET_DZ_RREAR(const DamageScaleRecord &input);

    bool DZ_TOP(TAttrib<DamageScaleRecord> &result) const;
    bool DZ_TOP(DamageScaleRecord &result) const;
    const DamageScaleRecord &DZ_TOP() const;
    bool SET_DZ_TOP(const DamageScaleRecord &input);

    bool FORCE(TAttrib<EA::Reflection::Float> &result) const;
    bool FORCE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FORCE() const;
    bool SET_FORCE(const EA::Reflection::Float &input);

    bool HIT_POINTS(TAttrib<EA::Reflection::Float> &result) const;
    bool HIT_POINTS(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HIT_POINTS() const;
    bool SET_HIT_POINTS(const EA::Reflection::Float &input);

    bool HP_THRESHOLD(TAttrib<EA::Reflection::Float> &result) const;
    bool HP_THRESHOLD(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HP_THRESHOLD() const;
    bool SET_HP_THRESHOLD(const EA::Reflection::Float &input);

    bool SHOCK_FORCE(TAttrib<EA::Reflection::Float> &result) const;
    bool SHOCK_FORCE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SHOCK_FORCE() const;
    bool SET_SHOCK_FORCE(const EA::Reflection::Float &input);

    bool SHOCK_TIME(TAttrib<EA::Reflection::Float> &result) const;
    bool SHOCK_TIME(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SHOCK_TIME() const;
    bool SET_SHOCK_TIME(const EA::Reflection::Float &input);

    bool SUPPRESS_DIST(TAttrib<EA::Reflection::Float> &result) const;
    bool SUPPRESS_DIST(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SUPPRESS_DIST() const;
    bool SET_SUPPRESS_DIST(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    damagespecs &ConvertFromInstance(Instance &src) {}
    const damagespecs &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key damagespecs = 0xc1f0b434;

}; // namespace ClassName

namespace Hash {
namespace damagespecs {

static const Key DZ_BOTTOM = 0xadd2023d;
static const Key DZ_FRONT = 0x1e21c772;
static const Key DZ_LEFT = 0x698e7c31;
static const Key DZ_LFRONT = 0xa6f71a1c;
static const Key DZ_LREAR = 0xb631bf4e;
static const Key DZ_REAR = 0xe1707b8a;
static const Key DZ_RFRONT = 0x0380056f;
static const Key DZ_RIGHT = 0xf700fe17;
static const Key DZ_RREAR = 0xcc3f0ac3;
static const Key DZ_TOP = 0xc4ce6d74;
static const Key FORCE = 0x3698b384;
static const Key HIT_POINTS = 0x97f059b7;
static const Key HP_THRESHOLD = 0x499cb7bd;
static const Key SHOCK_FORCE = 0x907930df;
static const Key SHOCK_TIME = 0x78059ff8;
static const Key SUPPRESS_DIST = 0xc802a18d;

}; // namespace damagespecs
}; // namespace Hash

inline Key Gen::damagespecs::ClassKey() {
    return ClassName::damagespecs;
}

inline bool Gen::damagespecs::DZ_BOTTOM(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xadd2023d);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_BOTTOM() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_BOTTOM);
}
inline bool Gen::damagespecs::DZ_BOTTOM(DamageScaleRecord &result) const {
    result = DZ_BOTTOM();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_BOTTOM(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_BOTTOM, input);
}

inline bool Gen::damagespecs::DZ_FRONT(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0x1e21c772);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_FRONT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_FRONT);
}
inline bool Gen::damagespecs::DZ_FRONT(DamageScaleRecord &result) const {
    result = DZ_FRONT();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_FRONT(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_FRONT, input);
}

inline bool Gen::damagespecs::DZ_LEFT(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0x698e7c31);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_LEFT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_LEFT);
}
inline bool Gen::damagespecs::DZ_LEFT(DamageScaleRecord &result) const {
    result = DZ_LEFT();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_LEFT(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_LEFT, input);
}

inline bool Gen::damagespecs::DZ_LFRONT(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xa6f71a1c);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_LFRONT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_LFRONT);
}
inline bool Gen::damagespecs::DZ_LFRONT(DamageScaleRecord &result) const {
    result = DZ_LFRONT();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_LFRONT(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_LFRONT, input);
}

inline bool Gen::damagespecs::DZ_LREAR(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xb631bf4e);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_LREAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_LREAR);
}
inline bool Gen::damagespecs::DZ_LREAR(DamageScaleRecord &result) const {
    result = DZ_LREAR();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_LREAR(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_LREAR, input);
}

inline bool Gen::damagespecs::DZ_REAR(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xe1707b8a);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_REAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_REAR);
}
inline bool Gen::damagespecs::DZ_REAR(DamageScaleRecord &result) const {
    result = DZ_REAR();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_REAR(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_REAR, input);
}

inline bool Gen::damagespecs::DZ_RFRONT(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0x0380056f);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_RFRONT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_RFRONT);
}
inline bool Gen::damagespecs::DZ_RFRONT(DamageScaleRecord &result) const {
    result = DZ_RFRONT();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_RFRONT(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_RFRONT, input);
}

inline bool Gen::damagespecs::DZ_RIGHT(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xf700fe17);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_RIGHT() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_RIGHT);
}
inline bool Gen::damagespecs::DZ_RIGHT(DamageScaleRecord &result) const {
    result = DZ_RIGHT();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_RIGHT(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_RIGHT, input);
}

inline bool Gen::damagespecs::DZ_RREAR(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xcc3f0ac3);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_RREAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_RREAR);
}
inline bool Gen::damagespecs::DZ_RREAR(DamageScaleRecord &result) const {
    result = DZ_RREAR();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_RREAR(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_RREAR, input);
}

inline bool Gen::damagespecs::DZ_TOP(TAttrib<DamageScaleRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(DamageScaleRecord, 0xc4ce6d74);
}
inline const DamageScaleRecord &Gen::damagespecs::DZ_TOP() const {
    ATTRIB_CODEGEN_GETLAYOUT(DZ_TOP);
}
inline bool Gen::damagespecs::DZ_TOP(DamageScaleRecord &result) const {
    result = DZ_TOP();
    return true;
}
inline bool Gen::damagespecs::SET_DZ_TOP(const DamageScaleRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DZ_TOP, input);
}

inline bool Gen::damagespecs::FORCE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3698b384);
}
inline const EA::Reflection::Float &Gen::damagespecs::FORCE() const {
    ATTRIB_CODEGEN_GETLAYOUT(FORCE);
}
inline bool Gen::damagespecs::FORCE(EA::Reflection::Float &result) const {
    result = FORCE();
    return true;
}
inline bool Gen::damagespecs::SET_FORCE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FORCE, input);
}

inline bool Gen::damagespecs::HIT_POINTS(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x97f059b7);
}
inline const EA::Reflection::Float &Gen::damagespecs::HIT_POINTS() const {
    ATTRIB_CODEGEN_GETLAYOUT(HIT_POINTS);
}
inline bool Gen::damagespecs::HIT_POINTS(EA::Reflection::Float &result) const {
    result = HIT_POINTS();
    return true;
}
inline bool Gen::damagespecs::SET_HIT_POINTS(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HIT_POINTS, input);
}

inline bool Gen::damagespecs::HP_THRESHOLD(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x499cb7bd);
}
inline const EA::Reflection::Float &Gen::damagespecs::HP_THRESHOLD() const {
    ATTRIB_CODEGEN_GETLAYOUT(HP_THRESHOLD);
}
inline bool Gen::damagespecs::HP_THRESHOLD(EA::Reflection::Float &result) const {
    result = HP_THRESHOLD();
    return true;
}
inline bool Gen::damagespecs::SET_HP_THRESHOLD(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HP_THRESHOLD, input);
}

inline bool Gen::damagespecs::SHOCK_FORCE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x907930df);
}
inline const EA::Reflection::Float &Gen::damagespecs::SHOCK_FORCE() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_FORCE);
}
inline bool Gen::damagespecs::SHOCK_FORCE(EA::Reflection::Float &result) const {
    result = SHOCK_FORCE();
    return true;
}
inline bool Gen::damagespecs::SET_SHOCK_FORCE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_FORCE, input);
}

inline bool Gen::damagespecs::SHOCK_TIME(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x78059ff8);
}
inline const EA::Reflection::Float &Gen::damagespecs::SHOCK_TIME() const {
    ATTRIB_CODEGEN_GETLAYOUT(SHOCK_TIME);
}
inline bool Gen::damagespecs::SHOCK_TIME(EA::Reflection::Float &result) const {
    result = SHOCK_TIME();
    return true;
}
inline bool Gen::damagespecs::SET_SHOCK_TIME(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SHOCK_TIME, input);
}

inline bool Gen::damagespecs::SUPPRESS_DIST(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc802a18d);
}
inline const EA::Reflection::Float &Gen::damagespecs::SUPPRESS_DIST() const {
    ATTRIB_CODEGEN_GETLAYOUT(SUPPRESS_DIST);
}
inline bool Gen::damagespecs::SUPPRESS_DIST(EA::Reflection::Float &result) const {
    result = SUPPRESS_DIST();
    return true;
}
inline bool Gen::damagespecs::SET_SUPPRESS_DIST(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SUPPRESS_DIST, input);
}

}; // namespace Attrib

#endif
