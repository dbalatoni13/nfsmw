
/*
    Compile unit: ./test.cpp
    Producer: GNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube
    Compile directory: C:\Users\kabiskac\Documents\git_contributions\nfsmw
    Language: C++
    Code range: 0x00000000 -> 0x00000010
    GCC Source File Name Offset: 0x00000000
    GCC Source Info Offset: 0x00000000
*/
struct Outer {
    // total size: 0x4
    struct Inner a; // offset 0x0, size 0x4
};
inline void Outer::asd2(struct Outer * const this, struct Outer & test) {}

// Range: 0x0 -> 0xC
void asd(struct Outer & test /* r3 */) {}

// Range: 0xC -> 0x10
void Outer::asd3(struct Outer * const this /* r3 */) {}

struct __vtbl_ptr_type {
    // total size: 0x8
    short __delta; // offset 0x0, size 0x2
    short __index; // offset 0x2, size 0x2
    union { // inferred
        void * __pfn; // offset 0x4, size 0x4
        short __delta2; // offset 0x4, size 0x2
    };
};
typedef const struct __vtbl_ptr_type __vtbl_ptr_type;
struct bad_alloc {
};
