#ifndef _attrib_gen_timeofdaylighting_h
#define _attrib_gen_timeofdaylighting_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct timeofdaylighting : Instance {
    struct _LayoutStruct {
        UMath::Vector4 SpecularColour;            // offset 0x0, size 0x10
        UMath::Vector4 DiffuseColour;             // offset 0x10, size 0x10
        UMath::Vector4 AmbientColour;             // offset 0x20, size 0x10
        UMath::Vector4 FogHazeColour;             // offset 0x30, size 0x10
        UMath::Vector4 FixedFunctionSkyColor;     // offset 0x40, size 0x10
        EA::Reflection::Float FogDistanceScale;   // offset 0x50, size 0x4
        EA::Reflection::Float FogHazeColourScale; // offset 0x54, size 0x4
        EA::Reflection::Float FogSkyColourScale;  // offset 0x58, size 0x4
        EA::Reflection::Float EnvSkyBrightness;   // offset 0x5c, size 0x4
        EA::Reflection::Float CarSpecScale;       // offset 0x60, size 0x4
    };

    typedef UMath::Vector4 TypeOf_AmbientColour;
    typedef EA::Reflection::Float TypeOf_CarSpecScale;
    typedef UMath::Vector4 TypeOf_DiffuseColour;
    typedef EA::Reflection::Float TypeOf_EnvSkyBrightness;
    typedef UMath::Vector4 TypeOf_FixedFunctionSkyColor;
    typedef EA::Reflection::Float TypeOf_FogDistanceScale;
    typedef UMath::Vector4 TypeOf_FogHazeColour;
    typedef EA::Reflection::Float TypeOf_FogHazeColourScale;
    typedef EA::Reflection::Float TypeOf_FogInLightScatter;
    typedef UMath::Vector4 TypeOf_FogSkyColour;
    typedef EA::Reflection::Float TypeOf_FogSkyColourScale;
    typedef EA::Reflection::Float TypeOf_FogSunFalloff;
    typedef UMath::Vector4 TypeOf_SpecularColour;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("timeofdaylighting");
    timeofdaylighting(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    timeofdaylighting(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    timeofdaylighting(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    timeofdaylighting(const timeofdaylighting &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    timeofdaylighting(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~timeofdaylighting() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x399ed882;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x399ed882, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const timeofdaylighting &operator=(const timeofdaylighting &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const timeofdaylighting &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AmbientColour(TAttrib<UMath::Vector4> &result) const;
    bool AmbientColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &AmbientColour() const;
    bool SET_AmbientColour(const UMath::Vector4 &input);

    bool CarSpecScale(TAttrib<EA::Reflection::Float> &result) const;
    bool CarSpecScale(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CarSpecScale() const;
    bool SET_CarSpecScale(const EA::Reflection::Float &input);

    bool DiffuseColour(TAttrib<UMath::Vector4> &result) const;
    bool DiffuseColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &DiffuseColour() const;
    bool SET_DiffuseColour(const UMath::Vector4 &input);

    bool EnvSkyBrightness(TAttrib<EA::Reflection::Float> &result) const;
    bool EnvSkyBrightness(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EnvSkyBrightness() const;
    bool SET_EnvSkyBrightness(const EA::Reflection::Float &input);

    bool FixedFunctionSkyColor(TAttrib<UMath::Vector4> &result) const;
    bool FixedFunctionSkyColor(UMath::Vector4 &result) const;
    const UMath::Vector4 &FixedFunctionSkyColor() const;
    bool SET_FixedFunctionSkyColor(const UMath::Vector4 &input);

    bool FogDistanceScale(TAttrib<EA::Reflection::Float> &result) const;
    bool FogDistanceScale(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FogDistanceScale() const;
    bool SET_FogDistanceScale(const EA::Reflection::Float &input);

    bool FogHazeColour(TAttrib<UMath::Vector4> &result) const;
    bool FogHazeColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &FogHazeColour() const;
    bool SET_FogHazeColour(const UMath::Vector4 &input);

    bool FogHazeColourScale(TAttrib<EA::Reflection::Float> &result) const;
    bool FogHazeColourScale(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FogHazeColourScale() const;
    bool SET_FogHazeColourScale(const EA::Reflection::Float &input);

    bool FogInLightScatter(TAttrib<EA::Reflection::Float> &result) const;
    bool FogInLightScatter(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FogInLightScatter() const;
    bool SET_FogInLightScatter(const EA::Reflection::Float &input);

    bool FogSkyColour(TAttrib<UMath::Vector4> &result) const;
    bool FogSkyColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &FogSkyColour() const;
    bool SET_FogSkyColour(const UMath::Vector4 &input);

    bool FogSkyColourScale(TAttrib<EA::Reflection::Float> &result) const;
    bool FogSkyColourScale(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FogSkyColourScale() const;
    bool SET_FogSkyColourScale(const EA::Reflection::Float &input);

    bool FogSunFalloff(TAttrib<EA::Reflection::Float> &result) const;
    bool FogSunFalloff(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FogSunFalloff() const;
    bool SET_FogSunFalloff(const EA::Reflection::Float &input);

    bool SpecularColour(TAttrib<UMath::Vector4> &result) const;
    bool SpecularColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &SpecularColour() const;
    bool SET_SpecularColour(const UMath::Vector4 &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    timeofdaylighting &ConvertFromInstance(Instance &src) {}
    const timeofdaylighting &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key timeofdaylighting = 0x399ed882;

}; // namespace ClassName

namespace Hash {
namespace timeofdaylighting {

static const Key AmbientColour = 0x52194d72;
static const Key CarSpecScale = 0xb656e7c6;
static const Key DiffuseColour = 0x7623feb2;
static const Key EnvSkyBrightness = 0x747f53a9;
static const Key FixedFunctionSkyColor = 0xbbe84a95;
static const Key FogDistanceScale = 0x72606030;
static const Key FogHazeColour = 0xc91c60c7;
static const Key FogHazeColourScale = 0x02c1f6ca;
static const Key FogInLightScatter = 0x00756239;
static const Key FogSkyColour = 0x1d76ea96;
static const Key FogSkyColourScale = 0x9095e8b8;
static const Key FogSunFalloff = 0x2ef8a8bf;
static const Key SpecularColour = 0x5855b1e7;

}; // namespace timeofdaylighting
}; // namespace Hash

inline Key Gen::timeofdaylighting::ClassKey() {
    return ClassName::timeofdaylighting;
}

inline bool Gen::timeofdaylighting::AmbientColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x52194d72);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::AmbientColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(AmbientColour);
}
inline bool Gen::timeofdaylighting::AmbientColour(UMath::Vector4 &result) const {
    result = AmbientColour();
    return true;
}
inline bool Gen::timeofdaylighting::SET_AmbientColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AmbientColour, input);
}

inline bool Gen::timeofdaylighting::CarSpecScale(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb656e7c6);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::CarSpecScale() const {
    ATTRIB_CODEGEN_GETLAYOUT(CarSpecScale);
}
inline bool Gen::timeofdaylighting::CarSpecScale(EA::Reflection::Float &result) const {
    result = CarSpecScale();
    return true;
}
inline bool Gen::timeofdaylighting::SET_CarSpecScale(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CarSpecScale, input);
}

inline bool Gen::timeofdaylighting::DiffuseColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x7623feb2);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::DiffuseColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(DiffuseColour);
}
inline bool Gen::timeofdaylighting::DiffuseColour(UMath::Vector4 &result) const {
    result = DiffuseColour();
    return true;
}
inline bool Gen::timeofdaylighting::SET_DiffuseColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DiffuseColour, input);
}

inline bool Gen::timeofdaylighting::EnvSkyBrightness(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x747f53a9);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::EnvSkyBrightness() const {
    ATTRIB_CODEGEN_GETLAYOUT(EnvSkyBrightness);
}
inline bool Gen::timeofdaylighting::EnvSkyBrightness(EA::Reflection::Float &result) const {
    result = EnvSkyBrightness();
    return true;
}
inline bool Gen::timeofdaylighting::SET_EnvSkyBrightness(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EnvSkyBrightness, input);
}

inline bool Gen::timeofdaylighting::FixedFunctionSkyColor(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xbbe84a95);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::FixedFunctionSkyColor() const {
    ATTRIB_CODEGEN_GETLAYOUT(FixedFunctionSkyColor);
}
inline bool Gen::timeofdaylighting::FixedFunctionSkyColor(UMath::Vector4 &result) const {
    result = FixedFunctionSkyColor();
    return true;
}
inline bool Gen::timeofdaylighting::SET_FixedFunctionSkyColor(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FixedFunctionSkyColor, input);
}

inline bool Gen::timeofdaylighting::FogDistanceScale(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x72606030);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::FogDistanceScale() const {
    ATTRIB_CODEGEN_GETLAYOUT(FogDistanceScale);
}
inline bool Gen::timeofdaylighting::FogDistanceScale(EA::Reflection::Float &result) const {
    result = FogDistanceScale();
    return true;
}
inline bool Gen::timeofdaylighting::SET_FogDistanceScale(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FogDistanceScale, input);
}

inline bool Gen::timeofdaylighting::FogHazeColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xc91c60c7);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::FogHazeColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(FogHazeColour);
}
inline bool Gen::timeofdaylighting::FogHazeColour(UMath::Vector4 &result) const {
    result = FogHazeColour();
    return true;
}
inline bool Gen::timeofdaylighting::SET_FogHazeColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FogHazeColour, input);
}

inline bool Gen::timeofdaylighting::FogHazeColourScale(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x02c1f6ca);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::FogHazeColourScale() const {
    ATTRIB_CODEGEN_GETLAYOUT(FogHazeColourScale);
}
inline bool Gen::timeofdaylighting::FogHazeColourScale(EA::Reflection::Float &result) const {
    result = FogHazeColourScale();
    return true;
}
inline bool Gen::timeofdaylighting::SET_FogHazeColourScale(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FogHazeColourScale, input);
}

inline bool Gen::timeofdaylighting::FogInLightScatter(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x00756239);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::FogInLightScatter() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x00756239);
}
inline bool Gen::timeofdaylighting::FogInLightScatter(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x00756239, result);
}
inline bool Gen::timeofdaylighting::SET_FogInLightScatter(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x00756239, input);
}

inline bool Gen::timeofdaylighting::FogSkyColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1d76ea96);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::FogSkyColour() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x1d76ea96);
}
inline bool Gen::timeofdaylighting::FogSkyColour(UMath::Vector4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x1d76ea96, result);
}
inline bool Gen::timeofdaylighting::SET_FogSkyColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x1d76ea96, input);
}

inline bool Gen::timeofdaylighting::FogSkyColourScale(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9095e8b8);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::FogSkyColourScale() const {
    ATTRIB_CODEGEN_GETLAYOUT(FogSkyColourScale);
}
inline bool Gen::timeofdaylighting::FogSkyColourScale(EA::Reflection::Float &result) const {
    result = FogSkyColourScale();
    return true;
}
inline bool Gen::timeofdaylighting::SET_FogSkyColourScale(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FogSkyColourScale, input);
}

inline bool Gen::timeofdaylighting::FogSunFalloff(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2ef8a8bf);
}
inline const EA::Reflection::Float &Gen::timeofdaylighting::FogSunFalloff() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x2ef8a8bf);
}
inline bool Gen::timeofdaylighting::FogSunFalloff(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x2ef8a8bf, result);
}
inline bool Gen::timeofdaylighting::SET_FogSunFalloff(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x2ef8a8bf, input);
}

inline bool Gen::timeofdaylighting::SpecularColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x5855b1e7);
}
inline const UMath::Vector4 &Gen::timeofdaylighting::SpecularColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpecularColour);
}
inline bool Gen::timeofdaylighting::SpecularColour(UMath::Vector4 &result) const {
    result = SpecularColour();
    return true;
}
inline bool Gen::timeofdaylighting::SET_SpecularColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpecularColour, input);
}

}; // namespace Attrib

#endif
