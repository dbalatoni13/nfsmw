#ifndef _attrib_gen_audioimpact_h
#define _attrib_gen_audioimpact_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct audioimpact : Instance {
    typedef Attrib::StringKey TypeOf_DESCRIPTION;
    typedef STICH_COLLISION_TYPE TypeOf_STITCH_LEVEL_0;
    typedef STICH_COLLISION_TYPE TypeOf_STITCH_LEVEL_1;
    typedef STICH_COLLISION_TYPE TypeOf_STITCH_LEVEL_2;
    typedef STICH_COLLISION_TYPE TypeOf_STITCH_LEVEL_3;
    typedef CollisionStream TypeOf_StreamSweetner;
    typedef StitchCollisionVol TypeOf_Volumes;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("audioimpact");
    audioimpact(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    audioimpact(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    audioimpact(const Instance &src) : Instance(src) {}
    audioimpact(const audioimpact &src) : Instance(src) {}
    audioimpact(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~audioimpact() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xfbffb107;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xfbffb107, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const audioimpact &operator=(const audioimpact &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const audioimpact &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool DESCRIPTION(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x09925106);
    }
    bool DESCRIPTION(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x09925106, result, index);
    }
    const Attrib::StringKey &DESCRIPTION(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x09925106, index);
    }
    unsigned int Num_DESCRIPTION() const {
        ATTRIB_CODEGEN_GETLENGTH(0x09925106);
    }
    bool SET_DESCRIPTION(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x09925106, input, index);
    }
    bool STITCH_LEVEL_0(TAttrib<STICH_COLLISION_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(STICH_COLLISION_TYPE, 0xc15856df);
    }
    bool STITCH_LEVEL_0(STICH_COLLISION_TYPE &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc15856df, result, index);
    }
    const STICH_COLLISION_TYPE &STITCH_LEVEL_0(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc15856df, index);
    }
    unsigned int Num_STITCH_LEVEL_0() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc15856df);
    }
    bool SET_STITCH_LEVEL_0(const STICH_COLLISION_TYPE &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc15856df, input, index);
    }
    bool STITCH_LEVEL_1(TAttrib<STICH_COLLISION_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(STICH_COLLISION_TYPE, 0xdadb5580);
    }
    bool STITCH_LEVEL_1(STICH_COLLISION_TYPE &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(STICH_COLLISION_TYPE, 0xdadb5580, result, index);
    }
    const STICH_COLLISION_TYPE &STITCH_LEVEL_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(STICH_COLLISION_TYPE, 0xdadb5580, index);
    }
    unsigned int Num_STITCH_LEVEL_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdadb5580);
    }
    bool SET_STITCH_LEVEL_1(const STICH_COLLISION_TYPE &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(STICH_COLLISION_TYPE, 0xdadb5580, input, index);
    }
    bool STITCH_LEVEL_2(TAttrib<STICH_COLLISION_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(STICH_COLLISION_TYPE, 0xc9218f8c);
    }
    bool STITCH_LEVEL_2(STICH_COLLISION_TYPE &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc9218f8c, result, index);
    }
    const STICH_COLLISION_TYPE &STITCH_LEVEL_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc9218f8c, index);
    }
    unsigned int Num_STITCH_LEVEL_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc9218f8c);
    }
    bool SET_STITCH_LEVEL_2(const STICH_COLLISION_TYPE &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(STICH_COLLISION_TYPE, 0xc9218f8c, input, index);
    }
    bool STITCH_LEVEL_3(TAttrib<STICH_COLLISION_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(STICH_COLLISION_TYPE, 0xefbca3c9);
    }
    bool STITCH_LEVEL_3(STICH_COLLISION_TYPE &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(STICH_COLLISION_TYPE, 0xefbca3c9, result, index);
    }
    const STICH_COLLISION_TYPE &STITCH_LEVEL_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(STICH_COLLISION_TYPE, 0xefbca3c9, index);
    }
    unsigned int Num_STITCH_LEVEL_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xefbca3c9);
    }
    bool SET_STITCH_LEVEL_3(const STICH_COLLISION_TYPE &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(STICH_COLLISION_TYPE, 0xefbca3c9, input, index);
    }
    bool StreamSweetner(TAttrib<CollisionStream> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CollisionStream, 0xa311c644);
    }
    bool StreamSweetner(CollisionStream &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(CollisionStream, 0xa311c644, result, index);
    }
    const CollisionStream &StreamSweetner(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(CollisionStream, 0xa311c644, index);
    }
    unsigned int Num_StreamSweetner() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa311c644);
    }
    bool SET_StreamSweetner(const CollisionStream &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(CollisionStream, 0xa311c644, input, index);
    }
    bool Volumes(TAttrib<StitchCollisionVol> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(StitchCollisionVol, 0xfcc8e754);
    }
    bool Volumes(StitchCollisionVol &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(StitchCollisionVol, 0xfcc8e754, result);
    }
    const StitchCollisionVol &Volumes() const {
        ATTRIB_CODEGEN_GETVALUE(StitchCollisionVol, 0xfcc8e754);
    }
    bool SET_Volumes(const StitchCollisionVol &input) {
        ATTRIB_CODEGEN_SETVALUE(StitchCollisionVol, 0xfcc8e754, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    audioimpact &ConvertFromInstance(Instance &src) {}
    const audioimpact &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key audioimpact = 0xfbffb107;

}; // namespace ClassName

namespace Hash {
namespace audioimpact {

static const Key DESCRIPTION = 0x09925106;
static const Key STITCH_LEVEL_0 = 0xc15856df;
static const Key STITCH_LEVEL_1 = 0xdadb5580;
static const Key STITCH_LEVEL_2 = 0xc9218f8c;
static const Key STITCH_LEVEL_3 = 0xefbca3c9;
static const Key StreamSweetner = 0xa311c644;
static const Key Volumes = 0xfcc8e754;

}; // namespace audioimpact
}; // namespace Hash

inline Key Gen::audioimpact::ClassKey() {
    return ClassName::audioimpact;
}

}; // namespace Attrib

#endif
