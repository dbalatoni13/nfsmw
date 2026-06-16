#ifndef _attrib_gen_audiosystem_h
#define _attrib_gen_audiosystem_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct audiosystem : Instance {
    struct _LayoutStruct {
        Attrib::StringKey CSIPath; // offset 0x0, size 0x10
        Attrib::StringKey BIGPath; // offset 0x10, size 0x10
        Attrib::StringKey IDXPath; // offset 0x20, size 0x10
        Attrib::StringKey EVTPath; // offset 0x30, size 0x10
    };

    typedef Attrib::StringKey TypeOf_AEMS_EnvBanks;
    typedef Attrib::StringKey TypeOf_AEMS_FEBanks;
    typedef Attrib::StringKey TypeOf_AEMS_MiscBanks;
    typedef Attrib::StringKey TypeOf_AEMS_NOSBanks;
    typedef Attrib::StringKey TypeOf_AEMS_RNBanks;
    typedef Attrib::StringKey TypeOf_AEMS_SkidBanks;
    typedef Attrib::StringKey TypeOf_AEMS_SparkBanks;
    typedef Attrib::StringKey TypeOf_AEMS_StitchBanks;
    typedef Attrib::StringKey TypeOf_AEMS_WNBanks;
    typedef Attrib::StringKey TypeOf_BIGPath;
    typedef Attrib::StringKey TypeOf_CSIPath;
    typedef Attrib::StringKey TypeOf_EVTPath;
    typedef Attrib::StringKey TypeOf_EvtSys;
    typedef RefSpec TypeOf_FESpeech;
    typedef Attrib::StringKey TypeOf_IDXPath;
    typedef RefSpec TypeOf_InGameSpeech;
    typedef RefSpec TypeOf_LicensedMusic;
    typedef RefSpec TypeOf_Locales;
    typedef RefSpec TypeOf_PFMapping;
    typedef RefSpec TypeOf_nissfxstreams;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("audiosystem");
    audiosystem(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    audiosystem(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    audiosystem(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    audiosystem(const audiosystem &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    audiosystem(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~audiosystem() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xd3c18f03;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xd3c18f03, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const audiosystem &operator=(const audiosystem &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const audiosystem &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AEMS_EnvBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x7e6ba5c8);
    }
    bool AEMS_EnvBanks(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x7e6ba5c8, result);
    }
    const Attrib::StringKey &AEMS_EnvBanks() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x7e6ba5c8);
    }
    bool SET_AEMS_EnvBanks(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x7e6ba5c8, input);
    }
    bool AEMS_FEBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9849f8df);
    }
    bool AEMS_FEBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, result, index);
    }
    const Attrib::StringKey &AEMS_FEBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, index);
    }
    unsigned int Num_AEMS_FEBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9849f8df);
    }
    bool SET_AEMS_FEBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, input, index);
    }
    bool AEMS_MiscBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xe48df448);
    }
    bool AEMS_MiscBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xe48df448, result, index);
    }
    const Attrib::StringKey &AEMS_MiscBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xe48df448, index);
    }
    unsigned int Num_AEMS_MiscBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe48df448);
    }
    bool SET_AEMS_MiscBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xe48df448, input, index);
    }
    bool AEMS_NOSBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x8aca07f9);
    }
    bool AEMS_NOSBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, result, index);
    }
    const Attrib::StringKey &AEMS_NOSBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, index);
    }
    unsigned int Num_AEMS_NOSBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8aca07f9);
    }
    bool SET_AEMS_NOSBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, input, index);
    }
    bool AEMS_RNBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x0663aaf3);
    }
    bool AEMS_RNBanks(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x0663aaf3, result);
    }
    const Attrib::StringKey &AEMS_RNBanks() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x0663aaf3);
    }
    bool SET_AEMS_RNBanks(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x0663aaf3, input);
    }
    bool AEMS_SkidBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xbd6f7135);
    }
    bool AEMS_SkidBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, result, index);
    }
    const Attrib::StringKey &AEMS_SkidBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, index);
    }
    unsigned int Num_AEMS_SkidBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xbd6f7135);
    }
    bool SET_AEMS_SkidBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, input, index);
    }
    bool AEMS_SparkBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xf44ae8ed);
    }
    bool AEMS_SparkBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, result, index);
    }
    const Attrib::StringKey &AEMS_SparkBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, index);
    }
    unsigned int Num_AEMS_SparkBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf44ae8ed);
    }
    bool SET_AEMS_SparkBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, input, index);
    }
    bool AEMS_StitchBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xeb1fd1bf);
    }
    bool AEMS_StitchBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, result, index);
    }
    const Attrib::StringKey &AEMS_StitchBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, index);
    }
    unsigned int Num_AEMS_StitchBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xeb1fd1bf);
    }
    bool SET_AEMS_StitchBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, input, index);
    }
    bool AEMS_WNBanks(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x916aa05d);
    }
    bool AEMS_WNBanks(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, result, index);
    }
    const Attrib::StringKey &AEMS_WNBanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, index);
    }
    unsigned int Num_AEMS_WNBanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0x916aa05d);
    }
    bool SET_AEMS_WNBanks(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, input, index);
    }
    bool BIGPath(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xa2f24773);
    }
    bool BIGPath(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BIGPath, result);
    }
    const Attrib::StringKey &BIGPath() const {
        ATTRIB_CODEGEN_GETLAYOUT(BIGPath);
    }
    bool SET_BIGPath(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BIGPath, input);
    }
    bool CSIPath(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xfb1533a6);
    }
    bool CSIPath(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CSIPath, result);
    }
    const Attrib::StringKey &CSIPath() const {
        ATTRIB_CODEGEN_GETLAYOUT(CSIPath);
    }
    bool SET_CSIPath(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CSIPath, input);
    }
    bool EVTPath(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x292001fb);
    }
    bool EVTPath(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EVTPath, result);
    }
    const Attrib::StringKey &EVTPath() const {
        ATTRIB_CODEGEN_GETLAYOUT(EVTPath);
    }
    bool SET_EVTPath(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EVTPath, input);
    }
    bool EvtSys(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x4166fa80);
    }
    bool EvtSys(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, result, index);
    }
    const Attrib::StringKey &EvtSys(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, index);
    }
    unsigned int Num_EvtSys() const {
        ATTRIB_CODEGEN_GETLENGTH(0x4166fa80);
    }
    bool SET_EvtSys(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, input, index);
    }
    bool FESpeech(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x174f218e);
    }
    bool FESpeech(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x174f218e, result);
    }
    const RefSpec &FESpeech() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x174f218e);
    }
    bool SET_FESpeech(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x174f218e, input);
    }
    bool IDXPath(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9e8fd078);
    }
    bool IDXPath(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IDXPath, result);
    }
    const Attrib::StringKey &IDXPath() const {
        ATTRIB_CODEGEN_GETLAYOUT(IDXPath);
    }
    bool SET_IDXPath(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IDXPath, input);
    }
    bool InGameSpeech(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4f085f82);
    }
    bool InGameSpeech(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x4f085f82, result);
    }
    const RefSpec &InGameSpeech() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x4f085f82);
    }
    bool SET_InGameSpeech(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x4f085f82, input);
    }
    bool LicensedMusic(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x5611db83);
    }
    bool LicensedMusic(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x5611db83, result);
    }
    const RefSpec &LicensedMusic() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x5611db83);
    }
    bool SET_LicensedMusic(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x5611db83, input);
    }
    bool Locales(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xc243117c);
    }
    bool Locales(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xc243117c, result, index);
    }
    const RefSpec &Locales(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xc243117c, index);
    }
    unsigned int Num_Locales() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc243117c);
    }
    bool SET_Locales(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xc243117c, input, index);
    }
    bool PFMapping(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x737714de);
    }
    bool PFMapping(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x737714de, result, index);
    }
    const RefSpec &PFMapping(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x737714de, index);
    }
    unsigned int Num_PFMapping() const {
        ATTRIB_CODEGEN_GETLENGTH(0x737714de);
    }
    bool SET_PFMapping(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x737714de, input, index);
    }
    bool nissfxstreams(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe6e5c2a3);
    }
    bool nissfxstreams(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xe6e5c2a3, result);
    }
    const RefSpec &nissfxstreams() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xe6e5c2a3);
    }
    bool SET_nissfxstreams(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xe6e5c2a3, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    audiosystem &ConvertFromInstance(Instance &src) {}
    const audiosystem &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key audiosystem = 0xd3c18f03;

}; // namespace ClassName

namespace Hash {
namespace audiosystem {

static const Key AEMS_EnvBanks = 0x7e6ba5c8;
static const Key AEMS_FEBanks = 0x9849f8df;
static const Key AEMS_MiscBanks = 0xe48df448;
static const Key AEMS_NOSBanks = 0x8aca07f9;
static const Key AEMS_RNBanks = 0x0663aaf3;
static const Key AEMS_SkidBanks = 0xbd6f7135;
static const Key AEMS_SparkBanks = 0xf44ae8ed;
static const Key AEMS_StitchBanks = 0xeb1fd1bf;
static const Key AEMS_WNBanks = 0x916aa05d;
static const Key BIGPath = 0xa2f24773;
static const Key CSIPath = 0xfb1533a6;
static const Key EVTPath = 0x292001fb;
static const Key EvtSys = 0x4166fa80;
static const Key FESpeech = 0x174f218e;
static const Key IDXPath = 0x9e8fd078;
static const Key InGameSpeech = 0x4f085f82;
static const Key LicensedMusic = 0x5611db83;
static const Key Locales = 0xc243117c;
static const Key PFMapping = 0x737714de;
static const Key nissfxstreams = 0xe6e5c2a3;

}; // namespace audiosystem
}; // namespace Hash

inline Key Gen::audiosystem::ClassKey() {
    return ClassName::audiosystem;
}

}; // namespace Attrib

#endif
