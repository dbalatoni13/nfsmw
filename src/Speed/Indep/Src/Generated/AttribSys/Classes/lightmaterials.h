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
    bool Ambient(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x483b6e25);
    }
    bool Ambient(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x483b6e25, result);
    }
    const EA::Reflection::Float &Ambient() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x483b6e25);
    }
    bool SET_Ambient(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x483b6e25, input);
    }
    bool Brightness(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x42ff3188);
    }
    bool Brightness(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x42ff3188, result);
    }
    const EA::Reflection::Float &Brightness() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x42ff3188);
    }
    bool SET_Brightness(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x42ff3188, input);
    }
    bool DiffuseMipMapBias(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xca470c4e);
    }
    bool DiffuseMipMapBias(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DiffuseMipMapBias, result);
    }
    const EA::Reflection::Float &DiffuseMipMapBias() const {
        ATTRIB_CODEGEN_GETLAYOUT(DiffuseMipMapBias);
    }
    bool SET_DiffuseMipMapBias(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DiffuseMipMapBias, input);
    }
    bool DiffuseSpace(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe3270a55);
    }
    bool DiffuseSpace(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DiffuseSpace, result);
    }
    const EA::Reflection::Float &DiffuseSpace() const {
        ATTRIB_CODEGEN_GETLAYOUT(DiffuseSpace);
    }
    bool SET_DiffuseSpace(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DiffuseSpace, input);
    }
    bool GrassColour(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xfec6cea1);
    }
    bool GrassColour(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GrassColour, result);
    }
    const UMath::Vector4 &GrassColour() const {
        ATTRIB_CODEGEN_GETLAYOUT(GrassColour);
    }
    bool SET_GrassColour(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GrassColour, input);
    }
    bool GrassGamma(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6ba9c139);
    }
    bool GrassGamma(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GrassGamma, result);
    }
    const EA::Reflection::Float &GrassGamma() const {
        ATTRIB_CODEGEN_GETLAYOUT(GrassGamma);
    }
    bool SET_GrassGamma(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GrassGamma, input);
    }
    bool GrassHeight(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf4d1867e);
    }
    bool GrassHeight(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GrassHeight, result);
    }
    const EA::Reflection::Float &GrassHeight() const {
        ATTRIB_CODEGEN_GETLAYOUT(GrassHeight);
    }
    bool SET_GrassHeight(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GrassHeight, input);
    }
    bool LODRamp(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4113025e);
    }
    bool LODRamp(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LODRamp, result);
    }
    const EA::Reflection::Float &LODRamp() const {
        ATTRIB_CODEGEN_GETLAYOUT(LODRamp);
    }
    bool SET_LODRamp(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LODRamp, input);
    }
    bool LODStart(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe05c9755);
    }
    bool LODStart(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LODStart, result);
    }
    const EA::Reflection::Float &LODStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(LODStart);
    }
    bool SET_LODStart(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LODStart, input);
    }
    bool LightingCone(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe0985f58);
    }
    bool LightingCone(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe0985f58, result);
    }
    const EA::Reflection::Float &LightingCone() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe0985f58);
    }
    bool SET_LightingCone(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe0985f58, input);
    }
    bool LowNoiseIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7d5db51c);
    }
    bool LowNoiseIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LowNoiseIntensity, result);
    }
    const EA::Reflection::Float &LowNoiseIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(LowNoiseIntensity);
    }
    bool SET_LowNoiseIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LowNoiseIntensity, input);
    }
    bool LowNoiseSpace(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x84226176);
    }
    bool LowNoiseSpace(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LowNoiseSpace, result);
    }
    const EA::Reflection::Float &LowNoiseSpace() const {
        ATTRIB_CODEGEN_GETLAYOUT(LowNoiseSpace);
    }
    bool SET_LowNoiseSpace(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LowNoiseSpace, input);
    }
    bool MaxShells(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1ffc70a9);
    }
    bool MaxShells(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MaxShells, result);
    }
    const EA::Reflection::Float &MaxShells() const {
        ATTRIB_CODEGEN_GETLAYOUT(MaxShells);
    }
    bool SET_MaxShells(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MaxShells, input);
    }
    bool NoiseMipMapBias(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9b4be787);
    }
    bool NoiseMipMapBias(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NoiseMipMapBias, result);
    }
    const EA::Reflection::Float &NoiseMipMapBias() const {
        ATTRIB_CODEGEN_GETLAYOUT(NoiseMipMapBias);
    }
    bool SET_NoiseMipMapBias(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NoiseMipMapBias, input);
    }
    bool NoiseSpace(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3a417eae);
    }
    bool NoiseSpace(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3a417eae, result);
    }
    const EA::Reflection::Float &NoiseSpace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3a417eae);
    }
    bool SET_NoiseSpace(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3a417eae, input);
    }
    bool PunchThroughAlphaRef(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xcf2d5699);
    }
    bool PunchThroughAlphaRef(EA::Reflection::UInt8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PunchThroughAlphaRef, result);
    }
    const EA::Reflection::UInt8 &PunchThroughAlphaRef() const {
        ATTRIB_CODEGEN_GETLAYOUT(PunchThroughAlphaRef);
    }
    bool SET_PunchThroughAlphaRef(const EA::Reflection::UInt8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PunchThroughAlphaRef, input);
    }
    bool Scruff(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3eb4022e);
    }
    bool Scruff(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Scruff, result);
    }
    const EA::Reflection::Float &Scruff() const {
        ATTRIB_CODEGEN_GETLAYOUT(Scruff);
    }
    bool SET_Scruff(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Scruff, input);
    }
    bool Smoothness(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb94011a2);
    }
    bool Smoothness(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb94011a2, result);
    }
    const EA::Reflection::Float &Smoothness() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb94011a2);
    }
    bool SET_Smoothness(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb94011a2, input);
    }
    bool diffuseColour(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x2df3899f);
    }
    bool diffuseColour(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(diffuseColour, result);
    }
    const UMath::Vector4 &diffuseColour() const {
        ATTRIB_CODEGEN_GETLAYOUT(diffuseColour);
    }
    bool SET_diffuseColour(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(diffuseColour, input);
    }
    bool diffuseFacing(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5118325a);
    }
    bool diffuseFacing(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(diffuseFacing, result);
    }
    const EA::Reflection::Float &diffuseFacing() const {
        ATTRIB_CODEGEN_GETLAYOUT(diffuseFacing);
    }
    bool SET_diffuseFacing(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(diffuseFacing, input);
    }
    bool diffuseGrazing(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x02ac77f1);
    }
    bool diffuseGrazing(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(diffuseGrazing, result);
    }
    const EA::Reflection::Float &diffuseGrazing() const {
        ATTRIB_CODEGEN_GETLAYOUT(diffuseGrazing);
    }
    bool SET_diffuseGrazing(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(diffuseGrazing, input);
    }
    bool diffusePower(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9ec4ea1b);
    }
    bool diffusePower(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(diffusePower, result);
    }
    const EA::Reflection::Float &diffusePower() const {
        ATTRIB_CODEGEN_GETLAYOUT(diffusePower);
    }
    bool SET_diffusePower(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(diffusePower, input);
    }
    bool filterColourMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xfd165dc3);
    }
    bool filterColourMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xfd165dc3, result);
    }
    const Attrib::StringKey &filterColourMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xfd165dc3);
    }
    bool SET_filterColourMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xfd165dc3, input);
    }
    bool glossMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5439f9c7);
    }
    bool glossMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x5439f9c7, result);
    }
    const Attrib::StringKey &glossMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x5439f9c7);
    }
    bool SET_glossMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x5439f9c7, input);
    }
    bool illuminationMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xf3d46f50);
    }
    bool illuminationMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xf3d46f50, result);
    }
    const Attrib::StringKey &illuminationMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xf3d46f50);
    }
    bool SET_illuminationMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xf3d46f50, input);
    }
    bool opacityMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x15fa3308);
    }
    bool opacityMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x15fa3308, result);
    }
    const Attrib::StringKey &opacityMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x15fa3308);
    }
    bool SET_opacityMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x15fa3308, input);
    }
    bool parallaxHeight(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x85aae9cd);
    }
    bool parallaxHeight(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(parallaxHeight, result);
    }
    const EA::Reflection::Float &parallaxHeight() const {
        ATTRIB_CODEGEN_GETLAYOUT(parallaxHeight);
    }
    bool SET_parallaxHeight(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(parallaxHeight, input);
    }
    bool reflectionMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x95c10731);
    }
    bool reflectionMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x95c10731, result);
    }
    const Attrib::StringKey &reflectionMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x95c10731);
    }
    bool SET_reflectionMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x95c10731, input);
    }
    bool refractionMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x1dafcfc0);
    }
    bool refractionMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x1dafcfc0, result);
    }
    const Attrib::StringKey &refractionMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x1dafcfc0);
    }
    bool SET_refractionMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x1dafcfc0, input);
    }
    bool shader(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5178481d);
    }
    bool shader(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(shader, result);
    }
    const Attrib::StringKey &shader() const {
        ATTRIB_CODEGEN_GETLAYOUT(shader);
    }
    bool SET_shader(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(shader, input);
    }
    bool shaderspec(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xcb31c94d);
    }
    bool shaderspec(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xcb31c94d, result);
    }
    const RefSpec &shaderspec() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xcb31c94d);
    }
    bool SET_shaderspec(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xcb31c94d, input);
    }
    bool specularColour(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x336d7f13);
    }
    bool specularColour(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(specularColour, result);
    }
    const UMath::Vector4 &specularColour() const {
        ATTRIB_CODEGEN_GETLAYOUT(specularColour);
    }
    bool SET_specularColour(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(specularColour, input);
    }
    bool specularColourMap(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x2569fd5b);
    }
    bool specularColourMap(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x2569fd5b, result);
    }
    const Attrib::StringKey &specularColourMap() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x2569fd5b);
    }
    bool SET_specularColourMap(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x2569fd5b, input);
    }
    bool specularFacing(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2636d2e8);
    }
    bool specularFacing(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(specularFacing, result);
    }
    const EA::Reflection::Float &specularFacing() const {
        ATTRIB_CODEGEN_GETLAYOUT(specularFacing);
    }
    bool SET_specularFacing(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(specularFacing, input);
    }
    bool specularGrazing(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xefda4e3d);
    }
    bool specularGrazing(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(specularGrazing, result);
    }
    const EA::Reflection::Float &specularGrazing() const {
        ATTRIB_CODEGEN_GETLAYOUT(specularGrazing);
    }
    bool SET_specularGrazing(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(specularGrazing, input);
    }
    bool specularPower(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc0a9864d);
    }
    bool specularPower(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(specularPower, result);
    }
    const EA::Reflection::Float &specularPower() const {
        ATTRIB_CODEGEN_GETLAYOUT(specularPower);
    }
    bool SET_specularPower(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(specularPower, input);
    }
    bool useVertexColour(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2de05384);
    }
    bool useVertexColour(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2de05384, result);
    }
    const EA::Reflection::Bool &useVertexColour() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2de05384);
    }
    bool SET_useVertexColour(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2de05384, input);
    }

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

}; // namespace Attrib

#endif
