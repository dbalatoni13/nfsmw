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
    bool AEMS_EnvBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_EnvBanks(Attrib::StringKey &result) const;
    const Attrib::StringKey &AEMS_EnvBanks() const;
    bool SET_AEMS_EnvBanks(const Attrib::StringKey &input);

    bool AEMS_FEBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_FEBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_FEBanks(unsigned int index) const;
    unsigned int Num_AEMS_FEBanks() const;
    bool SET_AEMS_FEBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_MiscBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_MiscBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_MiscBanks(unsigned int index) const;
    unsigned int Num_AEMS_MiscBanks() const;
    bool SET_AEMS_MiscBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_NOSBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_NOSBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_NOSBanks(unsigned int index) const;
    unsigned int Num_AEMS_NOSBanks() const;
    bool SET_AEMS_NOSBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_RNBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_RNBanks(Attrib::StringKey &result) const;
    const Attrib::StringKey &AEMS_RNBanks() const;
    bool SET_AEMS_RNBanks(const Attrib::StringKey &input);

    bool AEMS_SkidBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_SkidBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_SkidBanks(unsigned int index) const;
    unsigned int Num_AEMS_SkidBanks() const;
    bool SET_AEMS_SkidBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_SparkBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_SparkBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_SparkBanks(unsigned int index) const;
    unsigned int Num_AEMS_SparkBanks() const;
    bool SET_AEMS_SparkBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_StitchBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_StitchBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_StitchBanks(unsigned int index) const;
    unsigned int Num_AEMS_StitchBanks() const;
    bool SET_AEMS_StitchBanks(const Attrib::StringKey &input, unsigned int index);

    bool AEMS_WNBanks(TAttrib<Attrib::StringKey> &result) const;
    bool AEMS_WNBanks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &AEMS_WNBanks(unsigned int index) const;
    unsigned int Num_AEMS_WNBanks() const;
    bool SET_AEMS_WNBanks(const Attrib::StringKey &input, unsigned int index);

    bool BIGPath(TAttrib<Attrib::StringKey> &result) const;
    bool BIGPath(Attrib::StringKey &result) const;
    const Attrib::StringKey &BIGPath() const;
    bool SET_BIGPath(const Attrib::StringKey &input);

    bool CSIPath(TAttrib<Attrib::StringKey> &result) const;
    bool CSIPath(Attrib::StringKey &result) const;
    const Attrib::StringKey &CSIPath() const;
    bool SET_CSIPath(const Attrib::StringKey &input);

    bool EVTPath(TAttrib<Attrib::StringKey> &result) const;
    bool EVTPath(Attrib::StringKey &result) const;
    const Attrib::StringKey &EVTPath() const;
    bool SET_EVTPath(const Attrib::StringKey &input);

    bool EvtSys(TAttrib<Attrib::StringKey> &result) const;
    bool EvtSys(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &EvtSys(unsigned int index) const;
    unsigned int Num_EvtSys() const;
    bool SET_EvtSys(const Attrib::StringKey &input, unsigned int index);

    bool FESpeech(TAttrib<RefSpec> &result) const;
    bool FESpeech(RefSpec &result) const;
    const RefSpec &FESpeech() const;
    bool SET_FESpeech(const RefSpec &input);

    bool IDXPath(TAttrib<Attrib::StringKey> &result) const;
    bool IDXPath(Attrib::StringKey &result) const;
    const Attrib::StringKey &IDXPath() const;
    bool SET_IDXPath(const Attrib::StringKey &input);

    bool InGameSpeech(TAttrib<RefSpec> &result) const;
    bool InGameSpeech(RefSpec &result) const;
    const RefSpec &InGameSpeech() const;
    bool SET_InGameSpeech(const RefSpec &input);

    bool LicensedMusic(TAttrib<RefSpec> &result) const;
    bool LicensedMusic(RefSpec &result) const;
    const RefSpec &LicensedMusic() const;
    bool SET_LicensedMusic(const RefSpec &input);

    bool Locales(TAttrib<RefSpec> &result) const;
    bool Locales(RefSpec &result, unsigned int index) const;
    const RefSpec &Locales(unsigned int index) const;
    unsigned int Num_Locales() const;
    bool SET_Locales(const RefSpec &input, unsigned int index);

    bool PFMapping(TAttrib<RefSpec> &result) const;
    bool PFMapping(RefSpec &result, unsigned int index) const;
    const RefSpec &PFMapping(unsigned int index) const;
    unsigned int Num_PFMapping() const;
    bool SET_PFMapping(const RefSpec &input, unsigned int index);

    bool nissfxstreams(TAttrib<RefSpec> &result) const;
    bool nissfxstreams(RefSpec &result) const;
    const RefSpec &nissfxstreams() const;
    bool SET_nissfxstreams(const RefSpec &input);

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

inline bool Gen::audiosystem::AEMS_EnvBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x7e6ba5c8);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_EnvBanks() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x7e6ba5c8);
}
inline bool Gen::audiosystem::AEMS_EnvBanks(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x7e6ba5c8, result);
}
inline bool Gen::audiosystem::SET_AEMS_EnvBanks(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x7e6ba5c8, input);
}

inline bool Gen::audiosystem::AEMS_FEBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9849f8df);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_FEBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, index);
}
inline bool Gen::audiosystem::AEMS_FEBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_FEBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9849f8df);
}
inline bool Gen::audiosystem::SET_AEMS_FEBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x9849f8df, input, index);
}

inline bool Gen::audiosystem::AEMS_MiscBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xe48df448);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_MiscBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xe48df448, index);
}
inline bool Gen::audiosystem::AEMS_MiscBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xe48df448, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_MiscBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe48df448);
}
inline bool Gen::audiosystem::SET_AEMS_MiscBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xe48df448, input, index);
}

inline bool Gen::audiosystem::AEMS_NOSBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x8aca07f9);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_NOSBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, index);
}
inline bool Gen::audiosystem::AEMS_NOSBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_NOSBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8aca07f9);
}
inline bool Gen::audiosystem::SET_AEMS_NOSBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x8aca07f9, input, index);
}

inline bool Gen::audiosystem::AEMS_RNBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x0663aaf3);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_RNBanks() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x0663aaf3);
}
inline bool Gen::audiosystem::AEMS_RNBanks(Attrib::StringKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x0663aaf3, result);
}
inline bool Gen::audiosystem::SET_AEMS_RNBanks(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x0663aaf3, input);
}

inline bool Gen::audiosystem::AEMS_SkidBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xbd6f7135);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_SkidBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, index);
}
inline bool Gen::audiosystem::AEMS_SkidBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_SkidBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbd6f7135);
}
inline bool Gen::audiosystem::SET_AEMS_SkidBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xbd6f7135, input, index);
}

inline bool Gen::audiosystem::AEMS_SparkBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xf44ae8ed);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_SparkBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, index);
}
inline bool Gen::audiosystem::AEMS_SparkBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_SparkBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf44ae8ed);
}
inline bool Gen::audiosystem::SET_AEMS_SparkBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xf44ae8ed, input, index);
}

inline bool Gen::audiosystem::AEMS_StitchBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xeb1fd1bf);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_StitchBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, index);
}
inline bool Gen::audiosystem::AEMS_StitchBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_StitchBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xeb1fd1bf);
}
inline bool Gen::audiosystem::SET_AEMS_StitchBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xeb1fd1bf, input, index);
}

inline bool Gen::audiosystem::AEMS_WNBanks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x916aa05d);
}
inline const Attrib::StringKey &Gen::audiosystem::AEMS_WNBanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, index);
}
inline bool Gen::audiosystem::AEMS_WNBanks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, result, index);
}
inline unsigned int Gen::audiosystem::Num_AEMS_WNBanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0x916aa05d);
}
inline bool Gen::audiosystem::SET_AEMS_WNBanks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x916aa05d, input, index);
}

inline bool Gen::audiosystem::BIGPath(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xa2f24773);
}
inline const Attrib::StringKey &Gen::audiosystem::BIGPath() const {
    ATTRIB_CODEGEN_GETLAYOUT(BIGPath);
}
inline bool Gen::audiosystem::BIGPath(Attrib::StringKey &result) const {
    result = BIGPath();
    return true;
}
inline bool Gen::audiosystem::SET_BIGPath(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BIGPath, input);
}

inline bool Gen::audiosystem::CSIPath(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xfb1533a6);
}
inline const Attrib::StringKey &Gen::audiosystem::CSIPath() const {
    ATTRIB_CODEGEN_GETLAYOUT(CSIPath);
}
inline bool Gen::audiosystem::CSIPath(Attrib::StringKey &result) const {
    result = CSIPath();
    return true;
}
inline bool Gen::audiosystem::SET_CSIPath(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CSIPath, input);
}

inline bool Gen::audiosystem::EVTPath(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x292001fb);
}
inline const Attrib::StringKey &Gen::audiosystem::EVTPath() const {
    ATTRIB_CODEGEN_GETLAYOUT(EVTPath);
}
inline bool Gen::audiosystem::EVTPath(Attrib::StringKey &result) const {
    result = EVTPath();
    return true;
}
inline bool Gen::audiosystem::SET_EVTPath(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EVTPath, input);
}

inline bool Gen::audiosystem::EvtSys(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x4166fa80);
}
inline const Attrib::StringKey &Gen::audiosystem::EvtSys(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, index);
}
inline bool Gen::audiosystem::EvtSys(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, result, index);
}
inline unsigned int Gen::audiosystem::Num_EvtSys() const {
    ATTRIB_CODEGEN_GETLENGTH(0x4166fa80);
}
inline bool Gen::audiosystem::SET_EvtSys(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x4166fa80, input, index);
}

inline bool Gen::audiosystem::FESpeech(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x174f218e);
}
inline const RefSpec &Gen::audiosystem::FESpeech() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x174f218e);
}
inline bool Gen::audiosystem::FESpeech(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x174f218e, result);
}
inline bool Gen::audiosystem::SET_FESpeech(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x174f218e, input);
}

inline bool Gen::audiosystem::IDXPath(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x9e8fd078);
}
inline const Attrib::StringKey &Gen::audiosystem::IDXPath() const {
    ATTRIB_CODEGEN_GETLAYOUT(IDXPath);
}
inline bool Gen::audiosystem::IDXPath(Attrib::StringKey &result) const {
    result = IDXPath();
    return true;
}
inline bool Gen::audiosystem::SET_IDXPath(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(IDXPath, input);
}

inline bool Gen::audiosystem::InGameSpeech(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4f085f82);
}
inline const RefSpec &Gen::audiosystem::InGameSpeech() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x4f085f82);
}
inline bool Gen::audiosystem::InGameSpeech(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x4f085f82, result);
}
inline bool Gen::audiosystem::SET_InGameSpeech(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x4f085f82, input);
}

inline bool Gen::audiosystem::LicensedMusic(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x5611db83);
}
inline const RefSpec &Gen::audiosystem::LicensedMusic() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x5611db83);
}
inline bool Gen::audiosystem::LicensedMusic(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x5611db83, result);
}
inline bool Gen::audiosystem::SET_LicensedMusic(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x5611db83, input);
}

inline bool Gen::audiosystem::Locales(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xc243117c);
}
inline const RefSpec &Gen::audiosystem::Locales(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xc243117c, index);
}
inline bool Gen::audiosystem::Locales(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xc243117c, result, index);
}
inline unsigned int Gen::audiosystem::Num_Locales() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc243117c);
}
inline bool Gen::audiosystem::SET_Locales(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xc243117c, input, index);
}

inline bool Gen::audiosystem::PFMapping(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x737714de);
}
inline const RefSpec &Gen::audiosystem::PFMapping(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0x737714de, index);
}
inline bool Gen::audiosystem::PFMapping(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0x737714de, result, index);
}
inline unsigned int Gen::audiosystem::Num_PFMapping() const {
    ATTRIB_CODEGEN_GETLENGTH(0x737714de);
}
inline bool Gen::audiosystem::SET_PFMapping(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0x737714de, input, index);
}

inline bool Gen::audiosystem::nissfxstreams(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe6e5c2a3);
}
inline const RefSpec &Gen::audiosystem::nissfxstreams() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xe6e5c2a3);
}
inline bool Gen::audiosystem::nissfxstreams(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xe6e5c2a3, result);
}
inline bool Gen::audiosystem::SET_nissfxstreams(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xe6e5c2a3, input);
}

}; // namespace Attrib

#endif
