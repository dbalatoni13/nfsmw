#ifndef _attrib_gen_lightmaterials_h
#define _attrib_gen_lightmaterials_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct lightmaterials : Instance {
    struct _LayoutStruct {
        UMath::Vector4 GrassColour;                 // offset 0x0, size 0x10
        UMath::Vector4 diffuseColour;               // offset 0x10, size 0x10
        UMath::Vector4 specularColour;              // offset 0x20, size 0x10
        Attrib::StringKey shader;                   // offset 0x30, size 0x10
        EA::Reflection::Float GrassGamma;           // offset 0x40, size 0x4
        EA::Reflection::Float specularFacing;       // offset 0x44, size 0x4
        EA::Reflection::Float LODRamp;              // offset 0x48, size 0x4
        EA::Reflection::Float Scruff;               // offset 0x4c, size 0x4
        EA::Reflection::Float LowNoiseSpace;        // offset 0x50, size 0x4
        EA::Reflection::Float NoiseMipMapBias;      // offset 0x54, size 0x4
        EA::Reflection::Float LODStart;             // offset 0x58, size 0x4
        EA::Reflection::Float diffuseFacing;        // offset 0x5c, size 0x4
        EA::Reflection::Float LowNoiseIntensity;    // offset 0x60, size 0x4
        EA::Reflection::Float MaxShells;            // offset 0x64, size 0x4
        EA::Reflection::Float specularPower;        // offset 0x68, size 0x4
        EA::Reflection::Float DiffuseMipMapBias;    // offset 0x6c, size 0x4
        EA::Reflection::Float GrassHeight;          // offset 0x70, size 0x4
        EA::Reflection::Float diffusePower;         // offset 0x74, size 0x4
        EA::Reflection::Float DiffuseSpace;         // offset 0x78, size 0x4
        EA::Reflection::Float specularGrazing;      // offset 0x7c, size 0x4
        EA::Reflection::Float diffuseGrazing;       // offset 0x80, size 0x4
        EA::Reflection::Float parallaxHeight;       // offset 0x84, size 0x4
        EA::Reflection::UInt8 PunchThroughAlphaRef; // offset 0x88, size 0x1
    };

    typedef EA::Reflection::Float TypeOf_Ambient;
    typedef EA::Reflection::Float TypeOf_Brightness;
    typedef EA::Reflection::Float TypeOf_DiffuseMipMapBias;
    typedef EA::Reflection::Float TypeOf_DiffuseSpace;
    typedef UMath::Vector4 TypeOf_GrassColour;
    typedef EA::Reflection::Float TypeOf_GrassGamma;
    typedef EA::Reflection::Float TypeOf_GrassHeight;
    typedef EA::Reflection::Float TypeOf_LODRamp;
    typedef EA::Reflection::Float TypeOf_LODStart;
    typedef EA::Reflection::Float TypeOf_LightingCone;
    typedef EA::Reflection::Float TypeOf_LowNoiseIntensity;
    typedef EA::Reflection::Float TypeOf_LowNoiseSpace;
    typedef EA::Reflection::Float TypeOf_MaxShells;
    typedef EA::Reflection::Float TypeOf_NoiseMipMapBias;
    typedef EA::Reflection::Float TypeOf_NoiseSpace;
    typedef EA::Reflection::UInt8 TypeOf_PunchThroughAlphaRef;
    typedef EA::Reflection::Float TypeOf_Scruff;
    typedef EA::Reflection::Float TypeOf_Smoothness;
    typedef UMath::Vector4 TypeOf_diffuseColour;
    typedef EA::Reflection::Float TypeOf_diffuseFacing;
    typedef EA::Reflection::Float TypeOf_diffuseGrazing;
    typedef EA::Reflection::Float TypeOf_diffusePower;
    typedef Attrib::StringKey TypeOf_filterColourMap;
    typedef Attrib::StringKey TypeOf_glossMap;
    typedef Attrib::StringKey TypeOf_illuminationMap;
    typedef Attrib::StringKey TypeOf_opacityMap;
    typedef EA::Reflection::Float TypeOf_parallaxHeight;
    typedef Attrib::StringKey TypeOf_reflectionMap;
    typedef Attrib::StringKey TypeOf_refractionMap;
    typedef Attrib::StringKey TypeOf_shader;
    typedef RefSpec TypeOf_shaderspec;
    typedef UMath::Vector4 TypeOf_specularColour;
    typedef Attrib::StringKey TypeOf_specularColourMap;
    typedef EA::Reflection::Float TypeOf_specularFacing;
    typedef EA::Reflection::Float TypeOf_specularGrazing;
    typedef EA::Reflection::Float TypeOf_specularPower;
    typedef EA::Reflection::Bool TypeOf_useVertexColour;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("lightmaterials");
    lightmaterials(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    lightmaterials(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    lightmaterials(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    lightmaterials(const lightmaterials &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    lightmaterials(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~lightmaterials() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xd32a743f;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xd32a743f, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const lightmaterials &operator=(const lightmaterials &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const lightmaterials &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool Ambient(TAttrib<EA::Reflection::Float> &result) const;
    bool Ambient(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Ambient() const;
    bool SET_Ambient(const EA::Reflection::Float &input);

    bool Brightness(TAttrib<EA::Reflection::Float> &result) const;
    bool Brightness(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Brightness() const;
    bool SET_Brightness(const EA::Reflection::Float &input);

    bool DiffuseMipMapBias(TAttrib<EA::Reflection::Float> &result) const;
    bool DiffuseMipMapBias(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DiffuseMipMapBias() const;
    bool SET_DiffuseMipMapBias(const EA::Reflection::Float &input);

    bool DiffuseSpace(TAttrib<EA::Reflection::Float> &result) const;
    bool DiffuseSpace(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DiffuseSpace() const;
    bool SET_DiffuseSpace(const EA::Reflection::Float &input);

    bool GrassColour(TAttrib<UMath::Vector4> &result) const;
    bool GrassColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &GrassColour() const;
    bool SET_GrassColour(const UMath::Vector4 &input);

    bool GrassGamma(TAttrib<EA::Reflection::Float> &result) const;
    bool GrassGamma(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GrassGamma() const;
    bool SET_GrassGamma(const EA::Reflection::Float &input);

    bool GrassHeight(TAttrib<EA::Reflection::Float> &result) const;
    bool GrassHeight(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GrassHeight() const;
    bool SET_GrassHeight(const EA::Reflection::Float &input);

    bool LODRamp(TAttrib<EA::Reflection::Float> &result) const;
    bool LODRamp(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LODRamp() const;
    bool SET_LODRamp(const EA::Reflection::Float &input);

    bool LODStart(TAttrib<EA::Reflection::Float> &result) const;
    bool LODStart(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LODStart() const;
    bool SET_LODStart(const EA::Reflection::Float &input);

    bool LightingCone(TAttrib<EA::Reflection::Float> &result) const;
    bool LightingCone(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LightingCone() const;
    bool SET_LightingCone(const EA::Reflection::Float &input);

    bool LowNoiseIntensity(TAttrib<EA::Reflection::Float> &result) const;
    bool LowNoiseIntensity(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LowNoiseIntensity() const;
    bool SET_LowNoiseIntensity(const EA::Reflection::Float &input);

    bool LowNoiseSpace(TAttrib<EA::Reflection::Float> &result) const;
    bool LowNoiseSpace(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LowNoiseSpace() const;
    bool SET_LowNoiseSpace(const EA::Reflection::Float &input);

    bool MaxShells(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxShells(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxShells() const;
    bool SET_MaxShells(const EA::Reflection::Float &input);

    bool NoiseMipMapBias(TAttrib<EA::Reflection::Float> &result) const;
    bool NoiseMipMapBias(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NoiseMipMapBias() const;
    bool SET_NoiseMipMapBias(const EA::Reflection::Float &input);

    bool NoiseSpace(TAttrib<EA::Reflection::Float> &result) const;
    bool NoiseSpace(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NoiseSpace() const;
    bool SET_NoiseSpace(const EA::Reflection::Float &input);

    bool PunchThroughAlphaRef(TAttrib<EA::Reflection::UInt8> &result) const;
    bool PunchThroughAlphaRef(EA::Reflection::UInt8 &result) const;
    const EA::Reflection::UInt8 &PunchThroughAlphaRef() const;
    bool SET_PunchThroughAlphaRef(const EA::Reflection::UInt8 &input);

    bool Scruff(TAttrib<EA::Reflection::Float> &result) const;
    bool Scruff(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Scruff() const;
    bool SET_Scruff(const EA::Reflection::Float &input);

    bool Smoothness(TAttrib<EA::Reflection::Float> &result) const;
    bool Smoothness(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Smoothness() const;
    bool SET_Smoothness(const EA::Reflection::Float &input);

    bool diffuseColour(TAttrib<UMath::Vector4> &result) const;
    bool diffuseColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &diffuseColour() const;
    bool SET_diffuseColour(const UMath::Vector4 &input);

    bool diffuseFacing(TAttrib<EA::Reflection::Float> &result) const;
    bool diffuseFacing(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &diffuseFacing() const;
    bool SET_diffuseFacing(const EA::Reflection::Float &input);

    bool diffuseGrazing(TAttrib<EA::Reflection::Float> &result) const;
    bool diffuseGrazing(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &diffuseGrazing() const;
    bool SET_diffuseGrazing(const EA::Reflection::Float &input);

    bool diffusePower(TAttrib<EA::Reflection::Float> &result) const;
    bool diffusePower(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &diffusePower() const;
    bool SET_diffusePower(const EA::Reflection::Float &input);

    bool filterColourMap(TAttrib<Attrib::StringKey> &result) const;
    bool filterColourMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &filterColourMap() const;
    bool SET_filterColourMap(const Attrib::StringKey &input);

    bool glossMap(TAttrib<Attrib::StringKey> &result) const;
    bool glossMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &glossMap() const;
    bool SET_glossMap(const Attrib::StringKey &input);

    bool illuminationMap(TAttrib<Attrib::StringKey> &result) const;
    bool illuminationMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &illuminationMap() const;
    bool SET_illuminationMap(const Attrib::StringKey &input);

    bool opacityMap(TAttrib<Attrib::StringKey> &result) const;
    bool opacityMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &opacityMap() const;
    bool SET_opacityMap(const Attrib::StringKey &input);

    bool parallaxHeight(TAttrib<EA::Reflection::Float> &result) const;
    bool parallaxHeight(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &parallaxHeight() const;
    bool SET_parallaxHeight(const EA::Reflection::Float &input);

    bool reflectionMap(TAttrib<Attrib::StringKey> &result) const;
    bool reflectionMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &reflectionMap() const;
    bool SET_reflectionMap(const Attrib::StringKey &input);

    bool refractionMap(TAttrib<Attrib::StringKey> &result) const;
    bool refractionMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &refractionMap() const;
    bool SET_refractionMap(const Attrib::StringKey &input);

    bool shader(TAttrib<Attrib::StringKey> &result) const;
    bool shader(Attrib::StringKey &result) const;
    const Attrib::StringKey &shader() const;
    bool SET_shader(const Attrib::StringKey &input);

    bool shaderspec(TAttrib<RefSpec> &result) const;
    bool shaderspec(RefSpec &result) const;
    const RefSpec &shaderspec() const;
    bool SET_shaderspec(const RefSpec &input);

    bool specularColour(TAttrib<UMath::Vector4> &result) const;
    bool specularColour(UMath::Vector4 &result) const;
    const UMath::Vector4 &specularColour() const;
    bool SET_specularColour(const UMath::Vector4 &input);

    bool specularColourMap(TAttrib<Attrib::StringKey> &result) const;
    bool specularColourMap(Attrib::StringKey &result) const;
    const Attrib::StringKey &specularColourMap() const;
    bool SET_specularColourMap(const Attrib::StringKey &input);

    bool specularFacing(TAttrib<EA::Reflection::Float> &result) const;
    bool specularFacing(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &specularFacing() const;
    bool SET_specularFacing(const EA::Reflection::Float &input);

    bool specularGrazing(TAttrib<EA::Reflection::Float> &result) const;
    bool specularGrazing(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &specularGrazing() const;
    bool SET_specularGrazing(const EA::Reflection::Float &input);

    bool specularPower(TAttrib<EA::Reflection::Float> &result) const;
    bool specularPower(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &specularPower() const;
    bool SET_specularPower(const EA::Reflection::Float &input);

    bool useVertexColour(TAttrib<EA::Reflection::Bool> &result) const;
    bool useVertexColour(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &useVertexColour() const;
    bool SET_useVertexColour(const EA::Reflection::Bool &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    lightmaterials &ConvertFromInstance(Instance &src) {}
    const lightmaterials &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key lightmaterials = 0xd32a743f;

}; // namespace ClassName

namespace Hash {
namespace lightmaterials {

static const Key Ambient = 0x483b6e25;
static const Key Brightness = 0x42ff3188;
static const Key DiffuseMipMapBias = 0xca470c4e;
static const Key DiffuseSpace = 0xe3270a55;
static const Key GrassColour = 0xfec6cea1;
static const Key GrassGamma = 0x6ba9c139;
static const Key GrassHeight = 0xf4d1867e;
static const Key LODRamp = 0x4113025e;
static const Key LODStart = 0xe05c9755;
static const Key LightingCone = 0xe0985f58;
static const Key LowNoiseIntensity = 0x7d5db51c;
static const Key LowNoiseSpace = 0x84226176;
static const Key MaxShells = 0x1ffc70a9;
static const Key NoiseMipMapBias = 0x9b4be787;
static const Key NoiseSpace = 0x3a417eae;
static const Key PunchThroughAlphaRef = 0xcf2d5699;
static const Key Scruff = 0x3eb4022e;
static const Key Smoothness = 0xb94011a2;
static const Key diffuseColour = 0x2df3899f;
static const Key diffuseFacing = 0x5118325a;
static const Key diffuseGrazing = 0x02ac77f1;
static const Key diffusePower = 0x9ec4ea1b;
static const Key filterColourMap = 0xfd165dc3;
static const Key glossMap = 0x5439f9c7;
static const Key illuminationMap = 0xf3d46f50;
static const Key opacityMap = 0x15fa3308;
static const Key parallaxHeight = 0x85aae9cd;
static const Key reflectionMap = 0x95c10731;
static const Key refractionMap = 0x1dafcfc0;
static const Key shader = 0x5178481d;
static const Key shaderspec = 0xcb31c94d;
static const Key specularColour = 0x336d7f13;
static const Key specularColourMap = 0x2569fd5b;
static const Key specularFacing = 0x2636d2e8;
static const Key specularGrazing = 0xefda4e3d;
static const Key specularPower = 0xc0a9864d;
static const Key useVertexColour = 0x2de05384;

}; // namespace lightmaterials
}; // namespace Hash

inline Key Gen::lightmaterials::ClassKey() {
    return ClassName::lightmaterials;
}

inline bool Gen::lightmaterials::Ambient(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x483b6e25);
}
inline const EA::Reflection::Float &Gen::lightmaterials::Ambient() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x483b6e25);
}
inline bool Gen::lightmaterials::Ambient(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x483b6e25, result);
}
inline bool Gen::lightmaterials::SET_Ambient(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x483b6e25, input);
}

inline bool Gen::lightmaterials::Brightness(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x42ff3188);
}
inline const EA::Reflection::Float &Gen::lightmaterials::Brightness() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x42ff3188);
}
inline bool Gen::lightmaterials::Brightness(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x42ff3188, result);
}
inline bool Gen::lightmaterials::SET_Brightness(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x42ff3188, input);
}

inline bool Gen::lightmaterials::DiffuseMipMapBias(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xca470c4e);
}
inline const EA::Reflection::Float &Gen::lightmaterials::DiffuseMipMapBias() const {
    ATTRIB_CODEGEN_GETLAYOUT(DiffuseMipMapBias);
}
inline bool Gen::lightmaterials::DiffuseMipMapBias(EA::Reflection::Float &result) const {
    result = DiffuseMipMapBias();
    return true;
}
inline bool Gen::lightmaterials::SET_DiffuseMipMapBias(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DiffuseMipMapBias, input);
}

inline bool Gen::lightmaterials::DiffuseSpace(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe3270a55);
}
inline const EA::Reflection::Float &Gen::lightmaterials::DiffuseSpace() const {
    ATTRIB_CODEGEN_GETLAYOUT(DiffuseSpace);
}
inline bool Gen::lightmaterials::DiffuseSpace(EA::Reflection::Float &result) const {
    result = DiffuseSpace();
    return true;
}
inline bool Gen::lightmaterials::SET_DiffuseSpace(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DiffuseSpace, input);
}

inline bool Gen::lightmaterials::GrassColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xfec6cea1);
}
inline const UMath::Vector4 &Gen::lightmaterials::GrassColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(GrassColour);
}
inline bool Gen::lightmaterials::GrassColour(UMath::Vector4 &result) const {
    result = GrassColour();
    return true;
}
inline bool Gen::lightmaterials::SET_GrassColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GrassColour, input);
}

inline bool Gen::lightmaterials::GrassGamma(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6ba9c139);
}
inline const EA::Reflection::Float &Gen::lightmaterials::GrassGamma() const {
    ATTRIB_CODEGEN_GETLAYOUT(GrassGamma);
}
inline bool Gen::lightmaterials::GrassGamma(EA::Reflection::Float &result) const {
    result = GrassGamma();
    return true;
}
inline bool Gen::lightmaterials::SET_GrassGamma(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GrassGamma, input);
}

inline bool Gen::lightmaterials::GrassHeight(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf4d1867e);
}
inline const EA::Reflection::Float &Gen::lightmaterials::GrassHeight() const {
    ATTRIB_CODEGEN_GETLAYOUT(GrassHeight);
}
inline bool Gen::lightmaterials::GrassHeight(EA::Reflection::Float &result) const {
    result = GrassHeight();
    return true;
}
inline bool Gen::lightmaterials::SET_GrassHeight(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GrassHeight, input);
}

inline bool Gen::lightmaterials::LODRamp(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4113025e);
}
inline const EA::Reflection::Float &Gen::lightmaterials::LODRamp() const {
    ATTRIB_CODEGEN_GETLAYOUT(LODRamp);
}
inline bool Gen::lightmaterials::LODRamp(EA::Reflection::Float &result) const {
    result = LODRamp();
    return true;
}
inline bool Gen::lightmaterials::SET_LODRamp(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LODRamp, input);
}

inline bool Gen::lightmaterials::LODStart(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe05c9755);
}
inline const EA::Reflection::Float &Gen::lightmaterials::LODStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(LODStart);
}
inline bool Gen::lightmaterials::LODStart(EA::Reflection::Float &result) const {
    result = LODStart();
    return true;
}
inline bool Gen::lightmaterials::SET_LODStart(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LODStart, input);
}

inline bool Gen::lightmaterials::LightingCone(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe0985f58);
}
inline const EA::Reflection::Float &Gen::lightmaterials::LightingCone() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe0985f58);
}
inline bool Gen::lightmaterials::LightingCone(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe0985f58, result);
}
inline bool Gen::lightmaterials::SET_LightingCone(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe0985f58, input);
}

inline bool Gen::lightmaterials::LowNoiseIntensity(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7d5db51c);
}
inline const EA::Reflection::Float &Gen::lightmaterials::LowNoiseIntensity() const {
    ATTRIB_CODEGEN_GETLAYOUT(LowNoiseIntensity);
}
inline bool Gen::lightmaterials::LowNoiseIntensity(EA::Reflection::Float &result) const {
    result = LowNoiseIntensity();
    return true;
}
inline bool Gen::lightmaterials::SET_LowNoiseIntensity(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LowNoiseIntensity, input);
}

inline bool Gen::lightmaterials::LowNoiseSpace(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x84226176);
}
inline const EA::Reflection::Float &Gen::lightmaterials::LowNoiseSpace() const {
    ATTRIB_CODEGEN_GETLAYOUT(LowNoiseSpace);
}
inline bool Gen::lightmaterials::LowNoiseSpace(EA::Reflection::Float &result) const {
    result = LowNoiseSpace();
    return true;
}
inline bool Gen::lightmaterials::SET_LowNoiseSpace(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LowNoiseSpace, input);
}

inline bool Gen::lightmaterials::MaxShells(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1ffc70a9);
}
inline const EA::Reflection::Float &Gen::lightmaterials::MaxShells() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaxShells);
}
inline bool Gen::lightmaterials::MaxShells(EA::Reflection::Float &result) const {
    result = MaxShells();
    return true;
}
inline bool Gen::lightmaterials::SET_MaxShells(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaxShells, input);
}

inline bool Gen::lightmaterials::NoiseMipMapBias(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9b4be787);
}
inline const EA::Reflection::Float &Gen::lightmaterials::NoiseMipMapBias() const {
    ATTRIB_CODEGEN_GETLAYOUT(NoiseMipMapBias);
}
inline bool Gen::lightmaterials::NoiseMipMapBias(EA::Reflection::Float &result) const {
    result = NoiseMipMapBias();
    return true;
}
inline bool Gen::lightmaterials::SET_NoiseMipMapBias(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NoiseMipMapBias, input);
}

inline bool Gen::lightmaterials::NoiseSpace(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3a417eae);
}
inline const EA::Reflection::Float &Gen::lightmaterials::NoiseSpace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3a417eae);
}
inline bool Gen::lightmaterials::NoiseSpace(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3a417eae, result);
}
inline bool Gen::lightmaterials::SET_NoiseSpace(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3a417eae, input);
}

inline bool Gen::lightmaterials::PunchThroughAlphaRef(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xcf2d5699);
}
inline const EA::Reflection::UInt8 &Gen::lightmaterials::PunchThroughAlphaRef() const {
    ATTRIB_CODEGEN_GETLAYOUT(PunchThroughAlphaRef);
}
inline bool Gen::lightmaterials::PunchThroughAlphaRef(EA::Reflection::UInt8 &result) const {
    result = PunchThroughAlphaRef();
    return true;
}
inline bool Gen::lightmaterials::SET_PunchThroughAlphaRef(const EA::Reflection::UInt8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PunchThroughAlphaRef, input);
}

inline bool Gen::lightmaterials::Scruff(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3eb4022e);
}
inline const EA::Reflection::Float &Gen::lightmaterials::Scruff() const {
    ATTRIB_CODEGEN_GETLAYOUT(Scruff);
}
inline bool Gen::lightmaterials::Scruff(EA::Reflection::Float &result) const {
    result = Scruff();
    return true;
}
inline bool Gen::lightmaterials::SET_Scruff(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Scruff, input);
}

inline bool Gen::lightmaterials::Smoothness(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb94011a2);
}
inline const EA::Reflection::Float &Gen::lightmaterials::Smoothness() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb94011a2);
}
inline bool Gen::lightmaterials::Smoothness(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb94011a2, result);
}
inline bool Gen::lightmaterials::SET_Smoothness(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb94011a2, input);
}

inline bool Gen::lightmaterials::diffuseColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x2df3899f);
}
inline const UMath::Vector4 &Gen::lightmaterials::diffuseColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(diffuseColour);
}
inline bool Gen::lightmaterials::diffuseColour(UMath::Vector4 &result) const {
    result = diffuseColour();
    return true;
}
inline bool Gen::lightmaterials::SET_diffuseColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(diffuseColour, input);
}

inline bool Gen::lightmaterials::diffuseFacing(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5118325a);
}
inline const EA::Reflection::Float &Gen::lightmaterials::diffuseFacing() const {
    ATTRIB_CODEGEN_GETLAYOUT(diffuseFacing);
}
inline bool Gen::lightmaterials::diffuseFacing(EA::Reflection::Float &result) const {
    result = diffuseFacing();
    return true;
}
inline bool Gen::lightmaterials::SET_diffuseFacing(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(diffuseFacing, input);
}

inline bool Gen::lightmaterials::diffuseGrazing(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x02ac77f1);
}
inline const EA::Reflection::Float &Gen::lightmaterials::diffuseGrazing() const {
    ATTRIB_CODEGEN_GETLAYOUT(diffuseGrazing);
}
inline bool Gen::lightmaterials::diffuseGrazing(EA::Reflection::Float &result) const {
    result = diffuseGrazing();
    return true;
}
inline bool Gen::lightmaterials::SET_diffuseGrazing(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(diffuseGrazing, input);
}

inline bool Gen::lightmaterials::diffusePower(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9ec4ea1b);
}
inline const EA::Reflection::Float &Gen::lightmaterials::diffusePower() const {
    ATTRIB_CODEGEN_GETLAYOUT(diffusePower);
}
inline bool Gen::lightmaterials::diffusePower(EA::Reflection::Float &result) const {
    result = diffusePower();
    return true;
}
inline bool Gen::lightmaterials::SET_diffusePower(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(diffusePower, input);
}

inline bool Gen::lightmaterials::filterColourMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xfd165dc3);
}
inline const Attrib::StringKey &Gen::lightmaterials::filterColourMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xfd165dc3);
}
inline bool Gen::lightmaterials::filterColourMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xfd165dc3, result);
}
inline bool Gen::lightmaterials::SET_filterColourMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xfd165dc3, input);
}

inline bool Gen::lightmaterials::glossMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5439f9c7);
}
inline const Attrib::StringKey &Gen::lightmaterials::glossMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x5439f9c7);
}
inline bool Gen::lightmaterials::glossMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x5439f9c7, result);
}
inline bool Gen::lightmaterials::SET_glossMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x5439f9c7, input);
}

inline bool Gen::lightmaterials::illuminationMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xf3d46f50);
}
inline const Attrib::StringKey &Gen::lightmaterials::illuminationMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xf3d46f50);
}
inline bool Gen::lightmaterials::illuminationMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xf3d46f50, result);
}
inline bool Gen::lightmaterials::SET_illuminationMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xf3d46f50, input);
}

inline bool Gen::lightmaterials::opacityMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x15fa3308);
}
inline const Attrib::StringKey &Gen::lightmaterials::opacityMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x15fa3308);
}
inline bool Gen::lightmaterials::opacityMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x15fa3308, result);
}
inline bool Gen::lightmaterials::SET_opacityMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x15fa3308, input);
}

inline bool Gen::lightmaterials::parallaxHeight(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x85aae9cd);
}
inline const EA::Reflection::Float &Gen::lightmaterials::parallaxHeight() const {
    ATTRIB_CODEGEN_GETLAYOUT(parallaxHeight);
}
inline bool Gen::lightmaterials::parallaxHeight(EA::Reflection::Float &result) const {
    result = parallaxHeight();
    return true;
}
inline bool Gen::lightmaterials::SET_parallaxHeight(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(parallaxHeight, input);
}

inline bool Gen::lightmaterials::reflectionMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x95c10731);
}
inline const Attrib::StringKey &Gen::lightmaterials::reflectionMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x95c10731);
}
inline bool Gen::lightmaterials::reflectionMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x95c10731, result);
}
inline bool Gen::lightmaterials::SET_reflectionMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x95c10731, input);
}

inline bool Gen::lightmaterials::refractionMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x1dafcfc0);
}
inline const Attrib::StringKey &Gen::lightmaterials::refractionMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x1dafcfc0);
}
inline bool Gen::lightmaterials::refractionMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x1dafcfc0, result);
}
inline bool Gen::lightmaterials::SET_refractionMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x1dafcfc0, input);
}

inline bool Gen::lightmaterials::shader(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5178481d);
}
inline const Attrib::StringKey &Gen::lightmaterials::shader() const {
    ATTRIB_CODEGEN_GETLAYOUT(shader);
}
inline bool Gen::lightmaterials::shader(Attrib::StringKey &result) const {
    result = shader();
    return true;
}
inline bool Gen::lightmaterials::SET_shader(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(shader, input);
}

inline bool Gen::lightmaterials::shaderspec(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xcb31c94d);
}
inline const RefSpec &Gen::lightmaterials::shaderspec() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xcb31c94d);
}
inline bool Gen::lightmaterials::shaderspec(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xcb31c94d, result);
}
inline bool Gen::lightmaterials::SET_shaderspec(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xcb31c94d, input);
}

inline bool Gen::lightmaterials::specularColour(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x336d7f13);
}
inline const UMath::Vector4 &Gen::lightmaterials::specularColour() const {
    ATTRIB_CODEGEN_GETLAYOUT(specularColour);
}
inline bool Gen::lightmaterials::specularColour(UMath::Vector4 &result) const {
    result = specularColour();
    return true;
}
inline bool Gen::lightmaterials::SET_specularColour(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(specularColour, input);
}

inline bool Gen::lightmaterials::specularColourMap(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x2569fd5b);
}
inline const Attrib::StringKey &Gen::lightmaterials::specularColourMap() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x2569fd5b);
}
inline bool Gen::lightmaterials::specularColourMap(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x2569fd5b, result);
}
inline bool Gen::lightmaterials::SET_specularColourMap(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x2569fd5b, input);
}

inline bool Gen::lightmaterials::specularFacing(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2636d2e8);
}
inline const EA::Reflection::Float &Gen::lightmaterials::specularFacing() const {
    ATTRIB_CODEGEN_GETLAYOUT(specularFacing);
}
inline bool Gen::lightmaterials::specularFacing(EA::Reflection::Float &result) const {
    result = specularFacing();
    return true;
}
inline bool Gen::lightmaterials::SET_specularFacing(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(specularFacing, input);
}

inline bool Gen::lightmaterials::specularGrazing(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xefda4e3d);
}
inline const EA::Reflection::Float &Gen::lightmaterials::specularGrazing() const {
    ATTRIB_CODEGEN_GETLAYOUT(specularGrazing);
}
inline bool Gen::lightmaterials::specularGrazing(EA::Reflection::Float &result) const {
    result = specularGrazing();
    return true;
}
inline bool Gen::lightmaterials::SET_specularGrazing(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(specularGrazing, input);
}

inline bool Gen::lightmaterials::specularPower(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc0a9864d);
}
inline const EA::Reflection::Float &Gen::lightmaterials::specularPower() const {
    ATTRIB_CODEGEN_GETLAYOUT(specularPower);
}
inline bool Gen::lightmaterials::specularPower(EA::Reflection::Float &result) const {
    result = specularPower();
    return true;
}
inline bool Gen::lightmaterials::SET_specularPower(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(specularPower, input);
}

inline bool Gen::lightmaterials::useVertexColour(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2de05384);
}
inline const EA::Reflection::Bool &Gen::lightmaterials::useVertexColour() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2de05384);
}
inline bool Gen::lightmaterials::useVertexColour(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2de05384, result);
}
inline bool Gen::lightmaterials::SET_useVertexColour(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2de05384, input);
}

}; // namespace Attrib

#endif
