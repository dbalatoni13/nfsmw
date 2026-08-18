#ifndef _attrib_gen_ocean_h
#define _attrib_gen_ocean_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct ocean : Instance {
    typedef UMath::Vector4 TypeOf_layer1_specular_colour;
    typedef UMath::Vector2 TypeOf_layer1_tile_xy;
    typedef UMath::Vector4 TypeOf_layer2_specular_colour;
    typedef UMath::Vector2 TypeOf_layer2_tile_xy;
    typedef EA::Reflection::Float TypeOf_reflection_amount;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("ocean");
    ocean(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    ocean(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    ocean(const Instance &src) : Instance(src) {}
    ocean(const ocean &src) : Instance(src) {}
    ocean(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~ocean() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x093d7c56;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x093d7c56, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const ocean &operator=(const ocean &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const ocean &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool layer1_specular_colour(TAttrib<UMath::Vector4> &result) const;
    bool layer1_specular_colour(UMath::Vector4 &result) const;
    const UMath::Vector4 &layer1_specular_colour() const;
    bool SET_layer1_specular_colour(const UMath::Vector4 &input);

    bool layer1_tile_xy(TAttrib<UMath::Vector2> &result) const;
    bool layer1_tile_xy(UMath::Vector2 &result) const;
    const UMath::Vector2 &layer1_tile_xy() const;
    bool SET_layer1_tile_xy(const UMath::Vector2 &input);

    bool layer2_specular_colour(TAttrib<UMath::Vector4> &result) const;
    bool layer2_specular_colour(UMath::Vector4 &result) const;
    const UMath::Vector4 &layer2_specular_colour() const;
    bool SET_layer2_specular_colour(const UMath::Vector4 &input);

    bool layer2_tile_xy(TAttrib<UMath::Vector2> &result) const;
    bool layer2_tile_xy(UMath::Vector2 &result) const;
    const UMath::Vector2 &layer2_tile_xy() const;
    bool SET_layer2_tile_xy(const UMath::Vector2 &input);

    bool reflection_amount(TAttrib<EA::Reflection::Float> &result) const;
    bool reflection_amount(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &reflection_amount() const;
    bool SET_reflection_amount(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    ocean &ConvertFromInstance(Instance &src) {}
    const ocean &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key ocean = 0x093d7c56;

}; // namespace ClassName

namespace Hash {
namespace ocean {

static const Key layer1_specular_colour = 0xd9e53379;
static const Key layer1_tile_xy = 0x270e8256;
static const Key layer2_specular_colour = 0x19be3d84;
static const Key layer2_tile_xy = 0x8813b330;
static const Key reflection_amount = 0xe956e716;

}; // namespace ocean
}; // namespace Hash

inline Key Gen::ocean::ClassKey() {
    return ClassName::ocean;
}

inline bool Gen::ocean::layer1_specular_colour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xd9e53379);
}
inline const UMath::Vector4 &Gen::ocean::layer1_specular_colour() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0xd9e53379);
}
inline bool Gen::ocean::layer1_specular_colour(UMath::Vector4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0xd9e53379, result);
}
inline bool Gen::ocean::SET_layer1_specular_colour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0xd9e53379, input);
}

inline bool Gen::ocean::layer1_tile_xy(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x270e8256);
}
inline const UMath::Vector2 &Gen::ocean::layer1_tile_xy() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x270e8256);
}
inline bool Gen::ocean::layer1_tile_xy(UMath::Vector2 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x270e8256, result);
}
inline bool Gen::ocean::SET_layer1_tile_xy(const UMath::Vector2 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x270e8256, input);
}

inline bool Gen::ocean::layer2_specular_colour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x19be3d84);
}
inline const UMath::Vector4 &Gen::ocean::layer2_specular_colour() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x19be3d84);
}
inline bool Gen::ocean::layer2_specular_colour(UMath::Vector4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x19be3d84, result);
}
inline bool Gen::ocean::SET_layer2_specular_colour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x19be3d84, input);
}

inline bool Gen::ocean::layer2_tile_xy(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x8813b330);
}
inline const UMath::Vector2 &Gen::ocean::layer2_tile_xy() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x8813b330);
}
inline bool Gen::ocean::layer2_tile_xy(UMath::Vector2 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x8813b330, result);
}
inline bool Gen::ocean::SET_layer2_tile_xy(const UMath::Vector2 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x8813b330, input);
}

inline bool Gen::ocean::reflection_amount(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe956e716);
}
inline const EA::Reflection::Float &Gen::ocean::reflection_amount() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe956e716);
}
inline bool Gen::ocean::reflection_amount(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe956e716, result);
}
inline bool Gen::ocean::SET_reflection_amount(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe956e716, input);
}

}; // namespace Attrib

#endif
