#ifndef _attrib_gen_music_h
#define _attrib_gen_music_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct music : Instance {
    struct _LayoutStruct {
        Attrib::StringKey Artist;   // offset 0x0, size 0x10
        Attrib::StringKey SongName; // offset 0x10, size 0x10
        Attrib::StringKey Album;    // offset 0x20, size 0x10
        PathEventEnum PathEvent;    // offset 0x30, size 0x4
    };

    typedef Attrib::StringKey TypeOf_Album;
    typedef Attrib::StringKey TypeOf_Artist;
    typedef Attrib::StringKey TypeOf_DefPlay;
    typedef PathEventEnum TypeOf_PathEvent;
    typedef Attrib::StringKey TypeOf_SongName;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("music");
    music(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    music(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    music(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    music(const music &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    music(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~music() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x565465f8;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x565465f8, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const music &operator=(const music &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const music &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool Album(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x5ce33422);
    }
    bool Album(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Album, result);
    }
    const Attrib::StringKey &Album() const {
        ATTRIB_CODEGEN_GETLAYOUT(Album);
    }
    bool SET_Album(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Album, input);
    }
    bool Artist(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x3cc1b385);
    }
    bool Artist(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Artist, result);
    }
    const Attrib::StringKey &Artist() const {
        ATTRIB_CODEGEN_GETLAYOUT(Artist);
    }
    bool SET_Artist(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Artist, input);
    }
    bool DefPlay(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x58f80e5e);
    }
    bool DefPlay(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x58f80e5e, result);
    }
    const Attrib::StringKey &DefPlay() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x58f80e5e);
    }
    bool SET_DefPlay(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x58f80e5e, input);
    }
    bool PathEvent(TAttrib<PathEventEnum> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(PathEventEnum, 0x6d154d92);
    }
    bool PathEvent(PathEventEnum &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PathEvent, result);
    }
    const PathEventEnum &PathEvent() const {
        ATTRIB_CODEGEN_GETLAYOUT(PathEvent);
    }
    bool SET_PathEvent(const PathEventEnum &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PathEvent, input);
    }
    bool SongName(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xc352718e);
    }
    bool SongName(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SongName, result);
    }
    const Attrib::StringKey &SongName() const {
        ATTRIB_CODEGEN_GETLAYOUT(SongName);
    }
    bool SET_SongName(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SongName, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    music &ConvertFromInstance(Instance &src) {}
    const music &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key music = 0x565465f8;

}; // namespace ClassName

namespace Hash {
namespace music {

static const Key Album = 0x5ce33422;
static const Key Artist = 0x3cc1b385;
static const Key DefPlay = 0x58f80e5e;
static const Key PathEvent = 0x6d154d92;
static const Key SongName = 0xc352718e;

}; // namespace music
}; // namespace Hash

inline Key Gen::music::ClassKey() {
    return ClassName::music;
}

}; // namespace Attrib

#endif
