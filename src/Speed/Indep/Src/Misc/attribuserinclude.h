#ifndef _attribuserinclude_h_
#define _attribuserinclude_h_

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

namespace Attrib {

// total size: 0x10
// Decl: 24
class StringKey {
  public:
    StringKey() {
        mHash64 = 0;
        mHash32 = 0;
        mString = "";
    }

    StringKey(const char *str) {
        this->mHash64 = StringHash64(str);
        this->mHash32 = StringHash32(str);
        this->mString = str;
    }
    StringKey(const StringKey &src) : mHash64(src.mHash64), mHash32(src.mHash32), mString(src.mString) {}
    const StringKey &operator=(const StringKey &rhs) {
        this->mString = rhs.mString;
        this->mHash64 = rhs.mHash64;
        this->mHash32 = rhs.mHash32;
        return *this;
    }

    const char *GetString() const {
        return (this->mString != nullptr) ? this->mString : "";
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

    bool IsNotEmpty() const {
        if (this->mString == nullptr) {
            return false;
        }
        if (this->mString[0] == '\0') {
            return false;
        }
        return true;
    }

    bool IsValid() const {
        return this->mString != nullptr;
    }

    bool operator==(const StringKey &rhs) const {
        return this->mHash64 == rhs.mHash64;
    }
    bool operator!=(const StringKey &rhs) const {
        return this->mHash64 != rhs.mHash64;
    }
    bool operator<(const StringKey &rhs) const {
        return this->mHash64 < rhs.mHash64;
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
    static const StringKey gDefault; // Decl: 75

    const StringKey *mKey; // offset 0x0, size 0x4
};

}; // namespace Attrib

#undef min // Decl: 82
#undef max // Decl: 83

// TODO
#define REMOVE_EXCESSIVE_ATTRIB_ASSERTS // Decl: 92

#define USER_ATTRIB_ASSERT_MESSAGE(exp, msg)                          // Decl: 109
#define USER_ATTRIB_ASSERT_MESSAGE1(exp, msg, param1)                 // Decl: 110
#define USER_ATTRIB_ASSERT_MESSAGE2(exp, msg, param1, param2)         // Decl: 111
#define USER_ATTRIB_ASSERT_MESSAGE3(exp, msg, param1, param2, param3) // Decl: 112

#define USER_ATTRIB_ALLOC(bytes, name) AttribAlloc::Allocate(bytes, name)      // Decl: 127
#define USER_ATTRIB_FREE(ptr, bytes, name) AttribAlloc::Free(ptr, bytes, name) // Decl: 128

#define USER_ATTRIB_TABLESIZE(request) (request) // Decl: 134
#define USER_ATTRIB_ALIGNVEC ALIGNVEC            // Decl: 135

#define USER_ATTRIB_SEND_CHANGE(port, class, collection, attrib) // Decl: 140
#define USER_ATTRIB_SEND_CREATE(port, instance)                  // Decl: 141
#define USER_ATTRIB_SEND_MODIFY(port, instance, attribkey)       // Decl: 142
#define USER_ATTRIB_SEND_REMOVE(port, instance)                  // Decl: 143

#define USER_ATTRIB_PREREGISTERSTRINGS(num) // Decl: 146
#define USER_ATTRIB_REGISTERSTRING(k, s)    // Decl: 147
#define USER_ATTRIB_KEYTOSTRING(k) NULL     // Decl: 148

// #define USER_ATTRIB_VECTOR(type) eastl::vector<type, attrib_allocator>              // Decl: 167
// #define USER_ATTRIB_LIST(type) eastl::list<type, attrib_allocator>                  // Decl: 168
// #define USER_ATTRIB_SET(type) eastl::set<type, eastl::less<type>, attrib_allocator> // Decl: 169
// #define USER_ATTRIB_MAX(A, B) eastl::max(A, B)                                      // Decl: 170
// #define USER_ATTRIB_SORT(B, E) eastl::sort(B, E)                                    // Decl: 171
// #define USER_ATTRIB_FIND(B, E, T) eastl::find(B, E, T)                              // Decl: 172
// #define USER_ATTRIB_LOWER_BOUND(B, E, T) eastl::lower_bound(B, E, T)                // Decl: 173

#define USER_ATTRIB_BASETYPES // Decl: 177

typedef UMath::Vector2 Vector2; // 184
typedef UMath::Vector3 Vector3; // 185
typedef UMath::Vector4 Vector4;
// typedef Vector4 Quaternion;
typedef UMath::Matrix4 Matrix;

typedef int PathEventEnum; // Decl: 196

#define USER_ATTRIB_GROWTABLE(currententries, overrun) NONDEBUG_USER_ATTRIB_GROWTABLE(currententries, overrun) // Decl: 237

#endif
