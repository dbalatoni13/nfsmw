#ifndef EAGL4ANIM_EAGL4SUPPORTDLOPEN_H
#define EAGL4ANIM_EAGL4SUPPORTDLOPEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportconspool.h"
#include "eagl4supportdef.h"
#include "eagl4supportsympool.h"

#include <types.h>

namespace EAGL4 {

// total size: 0x30
class DynamicLoader {
  public:
    // total size: 0x10
    struct Symbol {
        Symbol()
            : name(nullptr), //
              type(nullptr), //
              data(nullptr), //
              isInternalRef(false) {}

        const char *name;   // offset 0x0, size 0x4
        const char *type;   // offset 0x4, size 0x4
        void *data;         // offset 0x8, size 0x4
        bool isInternalRef; // offset 0xC, size 0x1
    };

    static void SetVersionCheck(bool check) {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    // bool IsResolved() {}

    // void *ELFAddr(unsigned int offset) {}

    DynamicLoader(void *d, unsigned int len, DynamicUserCallback pSearchFunction);

    DynamicLoader(void *d, unsigned int len, void *r, bool delayResolve, DynamicUserCallback pSearchFunction);

    bool DoVersionCheck();

    ~DynamicLoader();

    void Release();

    void RunConstructors();

    void RunDestructors();

    void Resolve();

    void ReResolve(void *pNewAddr);

    void Initialize(DynamicUserCallback pSearchFunction);

    bool GetAddr(const char *type, const char *name, void *&result) const;

    int GetIndex(const char *name) const;

    int GetCount() const;

    Symbol GetSymbol(int i) const;

    void *GetElfData() const;

    bool GetNextSymbol(const char *type, int &iIndex, Symbol &result) const;

    bool GetNextAddr(const char *type, int &iIndex, void *&addr) const;

    static const char *RegisterVar(const char *name, void *addr);

    static void UnRegisterVar(const char *name);

    static void *GetRegisteredVar(const char *name, bool &valid);

    void AddPatchAddress(void *pPatchAddress);

    static SymbolPool gSymbolPool;                            // size: 0x14, address: 0x8045B130
    static ConstructorPool gConsPool;                         // size: 0x28, address: 0x8045B144
    static RuntimeAllocConstructorPool gRuntimeAllocConsPool; // size: 0x28, address: 0x8045B16C

    static const char ModelType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char BBoxType[];            // size: 0x0, address: 0xFFFFFFFF
    static const char TARType[];             // size: 0x0, address: 0xFFFFFFFF
    static const char ShapeType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char AnimBankType[];        // size: 0x0, address: 0x803D428C
    static const char BoneType[];            // size: 0x0, address: 0xFFFFFFFF
    static const char SkeletonType[];        // size: 0x0, address: 0x803D42A4
    static const char MorphType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char VersionType[];         // size: 0x0, address: 0xFFFFFFFF
    static const char AnimClipSetType[];     // size: 0x0, address: 0xFFFFFFFF
    static const char AnimIdType[];          // size: 0x0, address: 0xFFFFFFFF
    static const char PosePaletteBankType[]; // size: 0x0, address: 0xFFFFFFFF

  private:
    void *handle;                                         // offset 0x0, size 0x4
    int nDestructors;                                     // offset 0x4, size 0x4
    DestructorEntry *destructors;                         // offset 0x8, size 0x4
    RuntimeAllocDestructorEntry *RuntimeAllocDestructors; // offset 0xC, size 0x4
    char *mpData;                                         // offset 0x10, size 0x4
    unsigned int mDataLen;                                // offset 0x14, size 0x4
    char *mpReloc;                                        // offset 0x18, size 0x4
    DynamicUserCallback mSearchCallback;                  // offset 0x1C, size 0x4
    int mNumPatchAddresses;                               // offset 0x20, size 0x4
    int mMaxPatchAddresses;                               // offset 0x24, size 0x4
    unsigned int **mpPatchAddresses32;                    // offset 0x28, size 0x4

    static const int kDefaultMaxPatchAddresses; // size: 0x4, address: 0xFFFFFFFF
    static bool VersionCheck;                   // size: 0x1, address: 0xFFFFFFFF

    bool mIsResolved; // offset 0x2C, size 0x1
};

typedef unsigned int Elf32_Addr;
typedef short unsigned int Elf32_Half;
typedef unsigned int Elf32_Off;
typedef int Elf32_Sword;
typedef unsigned int Elf32_Word;

struct ELFHeader { // 0x34
    /* 0x00 */ unsigned char e_ident[16];
    /* 0x10 */ Elf32_Half e_type;
    /* 0x12 */ Elf32_Half e_machine;
    /* 0x14 */ Elf32_Word e_version;
    /* 0x18 */ Elf32_Addr e_entry;
    /* 0x1c */ Elf32_Off e_phoff;
    /* 0x20 */ Elf32_Off e_shoff;
    /* 0x24 */ Elf32_Word e_flags;
    /* 0x28 */ Elf32_Half e_ehsize;
    /* 0x2a */ Elf32_Half e_phentsize;
    /* 0x2c */ Elf32_Half e_phnum;
    /* 0x2e */ Elf32_Half e_shentsize;
    /* 0x30 */ Elf32_Half e_shnum;
    /* 0x32 */ Elf32_Half e_shstrndx;
};

struct ELFSectionHeader { // 0x28
    /* 0x00 */ Elf32_Word sh_name;
    /* 0x04 */ Elf32_Word sh_type;
    /* 0x08 */ Elf32_Word sh_flags;
    /* 0x0c */ Elf32_Addr sh_addr;
    /* 0x10 */ union { // 0x4
        /* 0x10 */ Elf32_Off sh_offset;
        /* 0x10 */ void *sh_voffset;
    };
    /* 0x14 */ Elf32_Word sh_size;
    /* 0x18 */ union { // 0x4
        /* 0x18 */ Elf32_Word sh_link;
        /* 0x18 */ void *sh_vlink;
    };
    /* 0x1c */ union { // 0x4
        /* 0x1c */ Elf32_Word sh_info;
        /* 0x1c */ void *sh_vinfo;
    };
    /* 0x20 */ Elf32_Word sh_addralign;
    /* 0x24 */ Elf32_Word sh_entsize;
};

struct ELF32_Sym { // 0x10
    /* 0x0 */ Elf32_Word st_name;
    /* 0x4 */ Elf32_Addr st_value;
    /* 0x8 */ Elf32_Word st_size;
    /* 0xc */ unsigned char st_info;
    /* 0xd */ unsigned char st_other;
    /* 0xe */ Elf32_Half st_shndx;
};

struct ELF32_Rel { // 0x8
    /* 0x0 */ Elf32_Addr r_offset;
    /* 0x4 */ Elf32_Word r_info;
};

struct ELF32_Rela { // 0xc
    /* 0x0 */ Elf32_Addr r_offset;
    /* 0x4 */ Elf32_Word r_info;
    /* 0x8 */ Elf32_Sword r_addend;
};

enum ELF32_STB_TYPE { STB_LOCAL = 0, STB_GLOBAL = 1, STB_WEAK = 2, STB_LOPROC = 13, STB_HIPROC = 15 };

enum ELF32_STT_TYPE { STT_NOTYPE = 0, STT_OBJECT = 1, STT_FUNC = 2, STT_SECTION = 3, STT_FILE = 4, STT_LOPROC = 13, STT_HIPROC = 15 };

enum ELF32_SH_TYPE {
    SHT_NULL = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_HASH = 5,
    SHT_DYNAMIC = 6,
    SHT_NOTE = 7,
    SHT_NOBITS = 8,
    SHT_REL = 9,
    SHT_SHLIB = 10,
    SHT_DYNSYM = 11,
    SHT_LOPROC = 0x70000000,
    SHT_HIPROC = 0x7FFFFFFF,
    SHT_LOUSER = 0x80000000,
    SHT_HIUSER = 0xFFFFFFFF,
};

enum ELF32_R_TYPES {
    R_MIPS_NONE = 0,
    R_MIPS_16 = 1,
    R_MIPS_32 = 2,
    R_MIPS_REL32 = 3,
    R_MIPS_26 = 4,
    R_MIPS_HI16 = 5,
    R_MIPS_LO16 = 6,
    R_MIPS_GPREL16 = 7,
    R_MIPS_LITERAL = 8,
    R_MIPS_GOT16 = 9,
    R_MIPS_PC16 = 10,
    R_MIPS_CALL16 = 11,
    R_MIPS_GPREL32 = 12,
    R_MIPS_UNUSED1 = 13,
    R_MIPS_UNUSED2 = 14,
    R_MIPS_UNUSED3 = 15,
    R_MIPS_SHIFT5 = 16,
    R_MIPS_SHIFT6 = 17,
    R_MIPS_64 = 18,
    R_MIPS_GOT_DISP = 19,
    R_MIPS_GOT_PAGE = 20,
    R_MIPS_GOT_OFST = 21
};

enum ELF32_SHF_TYPES { SHF_WRITE = 1, SHF_ALLOC = 2, SHF_EXECINSTR = 4, SHF_MASKPROC = -268435456, SHF_MIPS_GPREL = 268435456 };

enum ELF32_SHN_TYPES { SHN_UNDEF = 0, SHN_MIPS_ACCOMON = 65280, SHN_ABS = 65521, SHN_COMMON = 65522, SHN_HIRESERVE = 65535 };

// total size: 0x430
struct HashPointer {
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    bool IsResolved() {
        return resolved;
    }

    DynamicLoader &GetDynamicLoader() {
        return *mpDynamicLoader;
    }

    HashPointer(DynamicLoader *pDL) {
        mpDynamicLoader = pDL;
    }

    ~HashPointer() {}

    HashPointer *next;                   // offset 0x0, size 0x4
    HashPointer *prev;                   // offset 0x4, size 0x4
    char *strtab;                        // offset 0x8, size 0x4
    bool resolved;                       // offset 0xC, size 0x1
    int symbols_num;                     // offset 0x10, size 0x4
    ELF32_Sym *symtab;                   // offset 0x14, size 0x4
    ELFSectionHeader *sections;          // offset 0x18, size 0x4
    ELFHeader *e;                        // offset 0x1C, size 0x4
    long unsigned int hash[256];         // offset 0x20, size 0x400
    long unsigned int *chain;            // offset 0x420, size 0x4
    bool *isOriginal;                    // offset 0x424, size 0x4
    DynamicLoader *mpDynamicLoader;      // offset 0x428, size 0x4
    DynamicUserCallback pSearchFunction; // offset 0x42C, size 0x4
};

}; // namespace EAGL4

#endif
