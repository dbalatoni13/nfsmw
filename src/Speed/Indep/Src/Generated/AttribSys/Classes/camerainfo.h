#ifndef _attrib_gen_camerainfo_h
#define _attrib_gen_camerainfo_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct camerainfo : Instance {
    struct _LayoutStruct {
        Private _Array_STIFFNESS;            // offset 0x0, size 0x8
        EA::Reflection::Float STIFFNESS[2];  // offset 0x8, size 0x8
        Private _Array_LATOFFSET;            // offset 0x10, size 0x8
        EA::Reflection::Float LATOFFSET[2];  // offset 0x18, size 0x8
        Private _Array_LAG;                  // offset 0x20, size 0x8
        EA::Reflection::Float LAG[2];        // offset 0x28, size 0x8
        Private _Array_FOV;                  // offset 0x30, size 0x8
        EA::Reflection::Float FOV[2];        // offset 0x38, size 0x8
        Private _Array_HEIGHT;               // offset 0x40, size 0x8
        EA::Reflection::Float HEIGHT[2];     // offset 0x48, size 0x8
        Private _Array_ANGLE;                // offset 0x50, size 0x8
        EA::Reflection::Float ANGLE[2];      // offset 0x58, size 0x8
        EA::Reflection::Text CollectionName; // offset 0x60, size 0x4
        Private _Array_TILTING;              // offset 0x64, size 0x8
        EA::Reflection::Bool TILTING[2];     // offset 0x6c, size 0x2
        Private _Array_SELECTABLE;           // offset 0x6e, size 0x8
        EA::Reflection::Bool SELECTABLE[2];  // offset 0x76, size 0x2
    };

    typedef EA::Reflection::Float TypeOf_ANGLE;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Float TypeOf_FOV;
    typedef EA::Reflection::Float TypeOf_HEIGHT;
    typedef EA::Reflection::Float TypeOf_LAG;
    typedef EA::Reflection::Float TypeOf_LATOFFSET;
    typedef EA::Reflection::Bool TypeOf_SELECTABLE;
    typedef EA::Reflection::Float TypeOf_STIFFNESS;
    typedef EA::Reflection::Bool TypeOf_TILTING;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("camerainfo");
    camerainfo(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    camerainfo(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    camerainfo(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    camerainfo(const camerainfo &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    camerainfo(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~camerainfo() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x93c171e4;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x93c171e4, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const camerainfo &operator=(const camerainfo &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const camerainfo &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool ANGLE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7d1e620e);
    }
    bool ANGLE(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(ANGLE, result, index);
    }
    const EA::Reflection::Float &ANGLE(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, ANGLE, index);
    }
    unsigned int Num_ANGLE() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(ANGLE);
    }
    bool SET_ANGLE(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(ANGLE, input, index);
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
    }
    bool CollectionName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CollectionName, result);
    }
    const EA::Reflection::Text &CollectionName() const {
        ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
    }
    bool SET_CollectionName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
    }
    bool FOV(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x263e9452);
    }
    bool FOV(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(FOV, result, index);
    }
    const EA::Reflection::Float &FOV(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, FOV, index);
    }
    unsigned int Num_FOV() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(FOV);
    }
    bool SET_FOV(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(FOV, input, index);
    }
    bool HEIGHT(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x762b7718);
    }
    bool HEIGHT(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(HEIGHT, result, index);
    }
    const EA::Reflection::Float &HEIGHT(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, HEIGHT, index);
    }
    unsigned int Num_HEIGHT() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(HEIGHT);
    }
    bool SET_HEIGHT(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(HEIGHT, input, index);
    }
    bool LAG(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xde0857e3);
    }
    bool LAG(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(LAG, result, index);
    }
    const EA::Reflection::Float &LAG(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, LAG, index);
    }
    unsigned int Num_LAG() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(LAG);
    }
    bool SET_LAG(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(LAG, input, index);
    }
    bool LATOFFSET(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2fc5f041);
    }
    bool LATOFFSET(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(LATOFFSET, result, index);
    }
    const EA::Reflection::Float &LATOFFSET(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, LATOFFSET, index);
    }
    unsigned int Num_LATOFFSET() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(LATOFFSET);
    }
    bool SET_LATOFFSET(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(LATOFFSET, input, index);
    }
    bool SELECTABLE(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x40e94f86);
    }
    bool SELECTABLE(EA::Reflection::Bool &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SELECTABLE, result, index);
    }
    const EA::Reflection::Bool &SELECTABLE(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Bool, SELECTABLE, index);
    }
    unsigned int Num_SELECTABLE() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(SELECTABLE);
    }
    bool SET_SELECTABLE(const EA::Reflection::Bool &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(SELECTABLE, input, index);
    }
    bool STIFFNESS(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7f8eea1a);
    }
    bool STIFFNESS(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(STIFFNESS, result, index);
    }
    const EA::Reflection::Float &STIFFNESS(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, STIFFNESS, index);
    }
    unsigned int Num_STIFFNESS() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(STIFFNESS);
    }
    bool SET_STIFFNESS(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(STIFFNESS, input, index);
    }
    bool TILTING(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x665f4d74);
    }
    bool TILTING(EA::Reflection::Bool &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TILTING, result, index);
    }
    const EA::Reflection::Bool &TILTING(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Bool, TILTING, index);
    }
    unsigned int Num_TILTING() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TILTING);
    }
    bool SET_TILTING(const EA::Reflection::Bool &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TILTING, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    camerainfo &ConvertFromInstance(Instance &src) {}
    const camerainfo &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key camerainfo = 0x93c171e4;

}; // namespace ClassName

namespace Hash {
namespace camerainfo {

static const Key ANGLE = 0x7d1e620e;
static const Key CollectionName = 0x9ca1c8f9;
static const Key FOV = 0x263e9452;
static const Key HEIGHT = 0x762b7718;
static const Key LAG = 0xde0857e3;
static const Key LATOFFSET = 0x2fc5f041;
static const Key SELECTABLE = 0x40e94f86;
static const Key STIFFNESS = 0x7f8eea1a;
static const Key TILTING = 0x665f4d74;

}; // namespace camerainfo
}; // namespace Hash

inline Key Gen::camerainfo::ClassKey() {
    return ClassName::camerainfo;
}

}; // namespace Attrib

#endif
