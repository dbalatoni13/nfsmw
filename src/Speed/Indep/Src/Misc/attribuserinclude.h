#ifndef _attribuserinclude_h_
#define _attribuserinclude_h_

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

namespace Attrib {

// total size: 0x10
class StringKey {
  public:
    StringKey() {
        mHash64 = 0;
        mHash32 = 0;
        mString = "";
    }

    StringKey(const char *str) {
        mHash64 = StringHash64(str);
        mHash32 = StringHash32(str);
        mString = str;
    }

    StringKey(const StringKey &src)
        : mHash64(src.mHash64), //
          mHash32(src.mHash32), //
          mString(src.mString) {}

    const char *GetString() const {
        return this->mString;
    }

    uint64_t GetHash64() const {
        return this->mHash64;
    }

    uint32_t GetHash32() const {
        return this->mHash32;
    }

    bool IsEmpty() const {
        if (this->mString != nullptr) {
            return this->mString[0] == 0;
        }
        return true;
    }

    bool IsValid() const {
        return this->mString != nullptr;
    }

    bool operator==(const StringKey &rhs) const {
        return this->mHash64 == rhs.mHash64;
    }

    operator uint64_t() const {
        return this->mHash64;
    }

    operator uint32_t() const {
        return this->mHash32;
    }

  private:
    uint64_t mHash64;    // offset 0x0, size 0x8
    uint32_t mHash32;    // offset 0x8, size 0x4
    const char *mString; // offset 0xC, size 0x4
};

// total size: 0x4
class StringKeyPtr {
    static const StringKey gDefault;

    const StringKey *mKey; // offset 0x0, size 0x4
};

// const Attrib::StringKey gDefault; // size: 0x0, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/misc/attribuserinclude.h:75

}; // namespace Attrib

//   #undef:82  min
//         #undef:83  max
//         #define:92  REMOVE_EXCESSIVE_ATTRIB_ASSERTS
//         #define:109  USER_ATTRIB_ASSERT_MESSAGE(exp,msg)
//         #define:110  USER_ATTRIB_ASSERT_MESSAGE1(exp,msg,param1)
//         #define:111  USER_ATTRIB_ASSERT_MESSAGE2(exp,msg,param1,param2)
//         #define:112  USER_ATTRIB_ASSERT_MESSAGE3(exp,msg,param1,param2,param3)
//         #define:127  USER_ATTRIB_ALLOC(bytes,name) AttribAlloc::Allocate(bytes, name)
//         #define:128  USER_ATTRIB_FREE(ptr,bytes,name) AttribAlloc::Free(ptr, bytes, name)
//         #define:134  USER_ATTRIB_TABLESIZE(request) (request)
//         #define:135  USER_ATTRIB_ALIGNVEC ALIGNVEC
//         #define:140  USER_ATTRIB_SEND_CHANGE(port,class,collection,attrib)
//         #define:141  USER_ATTRIB_SEND_CREATE(port,instance)
//         #define:142  USER_ATTRIB_SEND_MODIFY(port,instance,attribkey)
//         #define:143  USER_ATTRIB_SEND_REMOVE(port,instance)
//         #define:146  USER_ATTRIB_PREREGISTERSTRINGS(num)
//         #define:147  USER_ATTRIB_REGISTERSTRING(k,s)
//         #define:148  USER_ATTRIB_KEYTOSTRING(k) NULL
//         #define:167  USER_ATTRIB_VECTOR(type) eastl::vector<type,attrib_allocator>
//         #define:168  USER_ATTRIB_LIST(type) eastl::list<type,attrib_allocator>
//         #define:169  USER_ATTRIB_SET(type) eastl::set<type,eastl::less<type>,attrib_allocator>
//         #define:170  USER_ATTRIB_MAX(A,B) eastl::max(A,B)
//         #define:171  USER_ATTRIB_SORT(B,E) eastl::sort(B,E)
//         #define:172  USER_ATTRIB_FIND(B,E,T) eastl::find(B,E,T)
//         #define:173  USER_ATTRIB_LOWER_BOUND(B,E,T) eastl::lower_bound(B,E,T)
//         #define:177  USER_ATTRIB_BASETYPES

typedef UMath::Vector2 Vector2; // 184
typedef UMath::Vector3 Vector3; // 185
typedef UMath::Vector4 Vector4;
// typedef Vector4 Quaternion;
typedef UMath::Matrix4 Matrix;

typedef int PathEventEnum; // Decl: speed/indep/src/misc/attribuserinclude.h:196

//         #define:237  USER_ATTRIB_GROWTABLE(currententries,overrun) NONDEBUG_USER_ATTRIB_GROWTABLE(currententries, overrun)

#endif
