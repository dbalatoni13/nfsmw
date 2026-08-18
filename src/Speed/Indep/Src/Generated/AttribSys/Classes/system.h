#ifndef _attrib_gen_system_h
#define _attrib_gen_system_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct system : Instance {
    struct _LayoutStruct {
        Private _Array_SimTasks;        // offset 0x0, size 0x8
        Attrib::StringKey SimTasks[14]; // offset 0x8, size 0xe0
    };

    typedef Attrib::StringKey TypeOf_SimControls;
    typedef Attrib::StringKey TypeOf_SimSubSystems;
    typedef Attrib::StringKey TypeOf_SimTasks;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("system");
    system(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    system(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    system(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    system(const system &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    system(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~system() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x4e8dce05;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x4e8dce05, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const system &operator=(const system &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const system &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool SimControls(TAttrib<Attrib::StringKey> &result) const;
    bool SimControls(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &SimControls(unsigned int index) const;
    unsigned int Num_SimControls() const;
    bool SET_SimControls(const Attrib::StringKey &input, unsigned int index);

    bool SimSubSystems(TAttrib<Attrib::StringKey> &result) const;
    bool SimSubSystems(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &SimSubSystems(unsigned int index) const;
    unsigned int Num_SimSubSystems() const;
    bool SET_SimSubSystems(const Attrib::StringKey &input, unsigned int index);

    bool SimTasks(TAttrib<Attrib::StringKey> &result) const;
    bool SimTasks(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &SimTasks(unsigned int index) const;
    unsigned int Num_SimTasks() const;
    bool SET_SimTasks(const Attrib::StringKey &input, unsigned int index);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    system &ConvertFromInstance(Instance &src) {}
    const system &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key system = 0x4e8dce05;

}; // namespace ClassName

namespace Hash {
namespace system {

static const Key SimControls = 0xd474e60a;
static const Key SimSubSystems = 0x0dfc2418;
static const Key SimTasks = 0xa1786a84;

}; // namespace system
}; // namespace Hash

inline Key Gen::system::ClassKey() {
    return ClassName::system;
}

inline bool Gen::system::SimControls(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xd474e60a);
}
inline const Attrib::StringKey &Gen::system::SimControls(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xd474e60a, index);
}
inline bool Gen::system::SimControls(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xd474e60a, result, index);
}
inline unsigned int Gen::system::Num_SimControls() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd474e60a);
}
inline bool Gen::system::SET_SimControls(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xd474e60a, input, index);
}

inline bool Gen::system::SimSubSystems(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x0dfc2418);
}
inline const Attrib::StringKey &Gen::system::SimSubSystems(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x0dfc2418, index);
}
inline bool Gen::system::SimSubSystems(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x0dfc2418, result, index);
}
inline unsigned int Gen::system::Num_SimSubSystems() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0dfc2418);
}
inline bool Gen::system::SET_SimSubSystems(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x0dfc2418, input, index);
}

inline bool Gen::system::SimTasks(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xa1786a84);
}
inline const Attrib::StringKey &Gen::system::SimTasks(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(Attrib::StringKey, SimTasks, index);
}
inline bool Gen::system::SimTasks(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SimTasks, result, index);
}
inline unsigned int Gen::system::Num_SimTasks() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(SimTasks);
}
inline bool Gen::system::SET_SimTasks(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(SimTasks, input, index);
}

}; // namespace Attrib

#endif
