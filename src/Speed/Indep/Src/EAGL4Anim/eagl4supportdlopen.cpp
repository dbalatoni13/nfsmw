#include "eagl4supportdlopen.h"
#include "eagl4supportconspool.h"
#include "eagl4supportdef.h"

#include <cstring>
#include <types.h>

static void *dlsym(void *handle, const char *name);

namespace EAGL4 {

namespace {

static const char kSymTabName[] = ".symtab";
static const char kStrTabName[] = ".strtab";
static const char kNamespaceMarker[] = ":::";
static const char kDynamicSymbolsMsg[] = "EAGL4::dynamic symbols";
static const char kDynamicSymbols2Msg[] = "EAGL4::dynamic symbols 2";
static const char gRuntimeAllocType[] = "RUNTIME_ALLOC::";

static inline unsigned short ByteSwap16(unsigned short value) {
    return static_cast<unsigned short>((value << 8) | (value >> 8));
}

static inline unsigned int ByteSwap32(unsigned int value) {
    return ((value >> 16) & 0xFF) << 8 | (value >> 24) | (((value & 0xFF) << 8 | ((value & 0xFFFF) >> 8)) << 16);
}

static inline char *ResolveLoaderAddress(char *data, unsigned int dataLen, char *reloc, unsigned int offset) {
    if (offset < dataLen) {
        return data + offset;
    }
    if (reloc) {
        return reloc + (offset - dataLen);
    }
    if (dataLen < offset) {
        return nullptr;
    }
    return data + offset;
}

static inline const char *GetLoaderSymbolType(const char *name) {
    const char *type = name + strlen(name);
    if (type[1] == 0x7F) {
        type += 2;
    }
    return type + 1;
}

} // namespace

static HashPointer *hashhead;

SymbolPool DynamicLoader::gSymbolPool;
ConstructorPool DynamicLoader::gConsPool;
RuntimeAllocConstructorPool DynamicLoader::gRuntimeAllocConsPool;

// const char DynamicLoader::ModelType[];
// const char DynamicLoader::BBoxType[];
// const char DynamicLoader::TARType[];
// const char DynamicLoader::ShapeType[];
const char DynamicLoader::AnimBankType[] = "AnimationBank";
// const char DynamicLoader::BoneType[];
const char DynamicLoader::SkeletonType[] = "Skeleton";
// const char DynamicLoader::MorphType[];
// const char DynamicLoader::VersionType[];
// const char DynamicLoader::AnimClipSetType[];
// const char DynamicLoader::AnimIdType[];
// const char DynamicLoader::PosePaletteBankType[];

static long unsigned int elfhash(const char *name) {
    long unsigned int h = 0; // r3
    long unsigned int g;     // r9

    while (*name) {
        h = h * 0x10 + *name;
        name++;
        g = h & 0xf0000000;
        if (g != 0) {
            h ^= g >> 0x18;
        }
        h &= ~g;
    }
    return h & 0xFF;
}

DynamicLoader::DynamicLoader(void *d, unsigned int len, DynamicUserCallback pSearchFunction)
    : mpData((char *)d),                //
      mDataLen(len),                    //
      mIsResolved(false),               //
      handle(nullptr),                  //
      nDestructors(0),                  //
      destructors(nullptr),             //
      RuntimeAllocDestructors(nullptr), //
      mpReloc(nullptr),                 //
      mSearchCallback(pSearchFunction), //
      mNumPatchAddresses(0),            //
      mMaxPatchAddresses(0),            //
      mpPatchAddresses32(nullptr) {
    Initialize(pSearchFunction);
    Resolve();
    DoVersionCheck();
}

bool DynamicLoader::DoVersionCheck() {
    return true;
}

void DynamicLoader::Resolve() {
    HashPointer *h = reinterpret_cast<HashPointer *>(handle);
    if (!h || h->resolved) {
        return;
    }

    h->resolved = true;

    ELFSectionHeader *sections = h->sections;
    int unresolvedCount = 0;
    int unresolvedSeen[32];
    int sectionCount = h->e->e_shnum;

    for (int sectionIndex = 0; sectionIndex < sectionCount; sectionIndex++) {
        ELFSectionHeader *section = &sections[sectionIndex];
        if (section->sh_type != SHT_REL) {
            continue;
        }

        ELF32_Rel *rel = reinterpret_cast<ELF32_Rel *>(section->sh_voffset);
        int relCount = static_cast<int>(section->sh_size);
        if (relCount < 0) {
            relCount += 7;
        }
        relCount >>= 3;

        ELF32_Sym *symtab = reinterpret_cast<ELF32_Sym *>(section->sh_vlink);
        char *targetBase = reinterpret_cast<char *>(section->sh_vinfo);

        for (int relIndex = 0; relIndex < relCount; relIndex++) {
            rel[relIndex].r_offset = ByteSwap32(rel[relIndex].r_offset);
            rel[relIndex].r_info = ByteSwap32(rel[relIndex].r_info);

            ELF32_Sym *sym = &symtab[rel[relIndex].r_info >> 8];
            int symbolBase = 0;

            while ((sym->st_info & 0xF) < STT_FILE) {
                if (sym->st_shndx && sym->st_shndx < h->e->e_shnum) {
                    symbolBase = reinterpret_cast<char *>(sections[sym->st_shndx].sh_voffset) - reinterpret_cast<char *>(nullptr);
                    break;
                }

                const char *name = &h->strtab[sym->st_name];
                bool valid = false;
                void *resolvedAddr = nullptr;

                for (HashPointer *other = hashhead; other; other = other->next) {
                    if (other != h) {
                        resolvedAddr = dlsym(other, name);
                        if (resolvedAddr) {
                            sym->st_shndx = 1;
                            sym->st_other = 2;
                            sym->st_value = reinterpret_cast<char *>(resolvedAddr) - reinterpret_cast<char *>(sections[1].sh_voffset);
                            symbolBase = reinterpret_cast<char *>(sections[1].sh_voffset) - reinterpret_cast<char *>(nullptr);
                            break;
                        }
                    }
                }

                if (resolvedAddr) {
                    break;
                }

                if (h->pSearchFunction) {
                    resolvedAddr = h->pSearchFunction(name, valid);
                    if (valid) {
                        sym->st_shndx = 1;
                        sym->st_other = 3;
                        sym->st_value = reinterpret_cast<char *>(resolvedAddr) - reinterpret_cast<char *>(sections[1].sh_voffset);
                        symbolBase = reinterpret_cast<char *>(sections[1].sh_voffset) - reinterpret_cast<char *>(nullptr);
                        break;
                    }
                }

                resolvedAddr = gSymbolPool.Search(name, valid);
                if (valid) {
                    sym->st_shndx = 1;
                    sym->st_other = 4;
                    sym->st_value = reinterpret_cast<char *>(resolvedAddr) - reinterpret_cast<char *>(sections[1].sh_voffset);
                    symbolBase = reinterpret_cast<char *>(sections[1].sh_voffset) - reinterpret_cast<char *>(nullptr);
                    break;
                }

                const char *type = GetLoaderSymbolType(name);
                if (strncmp(gRuntimeAllocType, name, strlen(gRuntimeAllocType)) == 0) {
                    RuntimeAllocConstructor ctor = gRuntimeAllocConsPool.FindConstructor(type);
                    if (ctor) {
                        RuntimeAllocDestructor dtor = gRuntimeAllocConsPool.FindDestructor(type);
                        int auxData = 0;
                        bool createDestructor = false;
                        void *runtimeAlloc =
                            ctor(name + strlen(gRuntimeAllocType), reinterpret_cast<class DynamicLoader *>(this), auxData, createDestructor, name);

                        if (createDestructor && runtimeAlloc) {
                            RuntimeAllocDestructorEntry *entry =
                                reinterpret_cast<RuntimeAllocDestructorEntry *>(EAGL4Internal::EAGL4Malloc(sizeof(RuntimeAllocDestructorEntry), nullptr));
                            entry->d = dtor;
                            entry->data = runtimeAlloc;
                            entry->auxData = auxData;
                            entry->next = RuntimeAllocDestructors;
                            RuntimeAllocDestructors = entry;
                        }

                        sym->st_shndx = 1;
                        sym->st_other = 5;
                        sym->st_value = reinterpret_cast<char *>(runtimeAlloc) - reinterpret_cast<char *>(sections[1].sh_voffset);
                        symbolBase = reinterpret_cast<char *>(sections[1].sh_voffset) - reinterpret_cast<char *>(nullptr);
                        break;
                    }
                }

                bool alreadySeen = false;
                if (unresolvedCount <= 0x1F) {
                    for (int i = 0; i < unresolvedCount; i++) {
                        if (name == reinterpret_cast<const char *>(unresolvedSeen[i])) {
                            alreadySeen = true;
                            break;
                        }
                    }
                    if (!alreadySeen) {
                        unresolvedSeen[unresolvedCount++] = reinterpret_cast<int>(name);
                    }
                }
                break;
            }

            int value = symbolBase + sym->st_value;
            unsigned int *relTarget = reinterpret_cast<unsigned int *>(targetBase + rel[relIndex].r_offset);
            unsigned int relValue = ByteSwap32(*relTarget);

            switch (rel[relIndex].r_info & 0xFF) {
                case R_MIPS_32:
                    relValue += value;
                    break;

                case R_MIPS_26:
                    relValue = (relValue & 0xFC000000) | ((value + ((relValue & 0x03FFFFFF) * 4) & 0x0FFFFFFF) >> 2);
                    break;

                case R_MIPS_HI16:
                    *reinterpret_cast<short *>(relTarget) =
                        static_cast<short>(*reinterpret_cast<short *>(relTarget) + static_cast<short>(static_cast<unsigned int>(value) >> 16));
                    continue;

                case R_MIPS_LO16:
                    *reinterpret_cast<short *>(relTarget) = static_cast<short>(*reinterpret_cast<short *>(relTarget) + static_cast<short>(value));
                    continue;
            }

            *relTarget = ByteSwap32(relValue);
        }
    }

    RunConstructors();
    mIsResolved = true;
}

void DynamicLoader::Initialize(DynamicUserCallback pSearchFunction) {
    HashPointer *h = reinterpret_cast<HashPointer *>(EAGL4Internal::EAGL4Malloc(sizeof(HashPointer), nullptr));

    h->next = nullptr;
    h->prev = nullptr;
    h->strtab = nullptr;
    h->resolved = false;
    h->symbols_num = 0;
    h->symtab = nullptr;
    h->sections = nullptr;
    h->e = reinterpret_cast<ELFHeader *>(mpData);
    h->chain = nullptr;
    h->isOriginal = nullptr;
    h->mpDynamicLoader = this;
    h->pSearchFunction = pSearchFunction;

    ELFHeader *e = h->e;

    e->e_type = ByteSwap16(e->e_type);
    e->e_machine = ByteSwap16(e->e_machine);
    e->e_version = ByteSwap32(e->e_version);
    e->e_entry = ByteSwap32(e->e_entry);
    e->e_phoff = ByteSwap32(e->e_phoff);
    e->e_shoff = ByteSwap32(e->e_shoff);
    e->e_flags = ByteSwap32(e->e_flags);
    e->e_ehsize = ByteSwap16(e->e_ehsize);
    e->e_phentsize = ByteSwap16(e->e_phentsize);
    e->e_phnum = ByteSwap16(e->e_phnum);
    e->e_shentsize = ByteSwap16(e->e_shentsize);
    e->e_shnum = ByteSwap16(e->e_shnum);
    e->e_shstrndx = ByteSwap16(e->e_shstrndx);

    h->sections = reinterpret_cast<ELFSectionHeader *>(ResolveLoaderAddress(mpData, mDataLen, mpReloc, e->e_shoff));

    for (int i = 0; i < e->e_shnum; i++) {
        ELFSectionHeader *section = &h->sections[i];

        section->sh_name = ByteSwap32(section->sh_name);
        section->sh_type = ByteSwap32(section->sh_type);
        section->sh_flags = ByteSwap32(section->sh_flags);
        section->sh_addr = ByteSwap32(section->sh_addr);
        section->sh_offset = ByteSwap32(section->sh_offset);
        section->sh_size = ByteSwap32(section->sh_size);
        section->sh_link = ByteSwap32(section->sh_link);
        section->sh_info = ByteSwap32(section->sh_info);
        section->sh_addralign = ByteSwap32(section->sh_addralign);
        section->sh_entsize = ByteSwap32(section->sh_entsize);

        section->sh_voffset = ResolveLoaderAddress(mpData, mDataLen, mpReloc, section->sh_offset);
    }

    char *sectionNameTable = reinterpret_cast<char *>(h->sections[e->e_shstrndx].sh_voffset);

    for (int i = 0; i < e->e_shnum; i++) {
        ELFSectionHeader *section = &h->sections[i];

        if (section->sh_type == SHT_STRTAB) {
            section->sh_vlink = h->sections[section->sh_link].sh_voffset;
            if (strcmp(kStrTabName, sectionNameTable + section->sh_name) == 0) {
                char *stringEntry = reinterpret_cast<char *>(section->sh_voffset);
                int remaining = section->sh_size;

                h->strtab = stringEntry;
                while (remaining > 0) {
                    int len = strlen(stringEntry);
                    if (stringEntry[0] == '_' && stringEntry[1] == '_') {
                        char *scope = strstr(stringEntry + 2, kNamespaceMarker);
                        if (scope) {
                            char temp[128];
                            *scope = 0;
                            strcpy(temp, stringEntry + 2);
                            int markerLen = strlen(kNamespaceMarker);
                            int tailLen = strlen(scope + markerLen);
                            memmove(stringEntry, scope + markerLen, tailLen + 1);
                            stringEntry[tailLen + 1] = '\x7F';
                            strcpy(stringEntry + tailLen + 2, temp);
                            len = strlen(stringEntry);
                        }
                    }
                    stringEntry += len + 1;
                    remaining -= len + 1;
                }
            }
        } else if (section->sh_type == SHT_SYMTAB || section->sh_type == SHT_STRTAB) {
            section->sh_vlink = h->sections[section->sh_link].sh_voffset;
            if (strcmp(kSymTabName, sectionNameTable + section->sh_name) == 0) {
                h->symtab = reinterpret_cast<ELF32_Sym *>(section->sh_voffset);
                h->symbols_num = static_cast<int>(section->sh_size >> 4);
            }
        } else if (section->sh_type == SHT_REL) {
            section->sh_vinfo = h->sections[section->sh_info].sh_voffset;
            section->sh_vlink = h->sections[section->sh_link].sh_voffset;
        }
    }

    for (int i = 0; i < 0x100; i++) {
        h->hash[i] = static_cast<unsigned long>(-1);
    }

    h->chain = reinterpret_cast<unsigned long *>(EAGL4Internal::EAGL4Malloc(h->symbols_num * sizeof(unsigned long), kDynamicSymbolsMsg));
    h->isOriginal = reinterpret_cast<bool *>(EAGL4Internal::EAGL4Malloc(h->symbols_num * sizeof(unsigned long), kDynamicSymbols2Msg));

    for (int i = 0; i < h->symbols_num; i++) {
        ELF32_Sym *sym = &h->symtab[i];

        sym->st_name = ByteSwap32(sym->st_name);
        sym->st_value = ByteSwap32(sym->st_value);
        sym->st_size = ByteSwap32(sym->st_size);
        sym->st_shndx = ByteSwap16(sym->st_shndx);

        unsigned long hash = elfhash(h->strtab + sym->st_name);
        h->chain[i] = h->hash[hash];
        h->hash[hash] = i;

        if (sym->st_shndx == 0 || h->e->e_shnum <= sym->st_shndx) {
            h->isOriginal[i] = false;
        } else {
            h->isOriginal[i] = true;
        }
    }

    h->next = hashhead;
    if (hashhead) {
        hashhead->prev = h;
    }
    h->prev = nullptr;
    hashhead = h;
    handle = h;
}

DynamicLoader::~DynamicLoader() {
    Release();
    RunDestructors();
    if (mpPatchAddresses32) {
        EAGL4Internal::EAGL4Free(mpPatchAddresses32, mMaxPatchAddresses);
    }
    mpPatchAddresses32 = nullptr;
}

void DynamicLoader::Release() {
    if (handle) {
        HashPointer *h = reinterpret_cast<HashPointer *>(handle);

        if (h->prev) {
            h->prev->next = h->next;
        } else {
            hashhead = h->next;
        }

        if (h->next) {
            h->next->prev = h->prev;
        }

        if (h->chain) {
            EAGL4Internal::EAGL4Free(h->chain, h->symbols_num * sizeof(uintptr_t));
        }

        if (h->isOriginal) {
            EAGL4Internal::EAGL4Free(h->isOriginal, h->symbols_num * sizeof(uintptr_t));
        }

        // TODO how to avoid the null check?
        delete h;
        handle = nullptr;
    }
    mIsResolved = false;
}

void DynamicLoader::RunConstructors() {
    int n = GetCount();
    int N = 0;

    for (int i = 0; i < n; i++) {
        Symbol s = GetSymbol(i);
        if (s.isInternalRef) {
            Constructor c = gConsPool.FindConstructor(s.type);
            if (c) {
                N++;
            }
        }
    }
    if (N > 0) {
        destructors = reinterpret_cast<DestructorEntry *>(EAGL4Internal::EAGL4Malloc(N * sizeof(*destructors), "EAGL4::dynamic destructor list"));
        N = 0;
        for (int i = 0; i < n; i++) {
            Symbol s = GetSymbol(i);
            if (s.isInternalRef) {
                Constructor c = gConsPool.FindConstructor(s.type);
                if (!c) {
                    continue;
                }
                Destructor d = gConsPool.FindDestructor(s.type);
                c(s.data, this, s.name);
                destructors[N].d = d;
                destructors[N].data = s.data;
                N++;
            }
        }
    }
    nDestructors = N;
}

void DynamicLoader::RunDestructors() {
    if (destructors) {
        for (int i = nDestructors - 1; i >= 0; i--) {
            destructors[i].d(destructors[i].data);
        }

        EAGL4Internal::EAGL4Free(destructors, nDestructors * sizeof(*destructors));
        destructors = nullptr;
    }
    RuntimeAllocDestructorEntry *de = RuntimeAllocDestructors;
    while (de) {
        RuntimeAllocDestructorEntry *tempde = de->next;

        de->d(de->data, de->auxData);
        delete de;

        de = tempde;
    }

    RuntimeAllocDestructors = nullptr;
}

int DynamicLoader::GetCount() const {
    if (!handle) {
        return 0;
    }
    HashPointer *h = reinterpret_cast<HashPointer *>(handle);
    return h->symbols_num;
}

// TODO
DynamicLoader::Symbol DynamicLoader::GetSymbol(int i) const {
    DynamicLoader::Symbol r;
    HashPointer *h = reinterpret_cast<HashPointer *>(handle);
    if (!h) {
        // r.name = nullptr;
        r.type = nullptr;
        // r.data = nullptr;
        r.isInternalRef = false;
        return r;
    }
    ELF32_Sym *s = h->symtab;
    if (i < 0 || i >= h->symbols_num) {
        // r.name = nullptr;
        r.type = nullptr;
        // r.data = nullptr;
        r.isInternalRef = false;
        return r;
    }
    r.name = &h->strtab[s[i].st_name];
    r.type = &r.name[strlen(&h->strtab[s[i].st_name])];
    r.type++;
    if (r.type[1] == 0x7F) {
        r.type++;
    } else {
        r.type--;
    }
    r.isInternalRef = (s[i].st_other - 2) > 3;

    int iIndex = s[i].st_shndx;
    if (s[i].st_other == 1) {
        r.data = reinterpret_cast<void *>(s[i].st_value);
    } else if (iIndex > 0 && iIndex < h->e->e_shnum) {
        r.data = reinterpret_cast<void *>(h->sections[iIndex].sh_offset + s[i].st_value);
    }

    return r;
}

}; // namespace EAGL4

static void *dlsym(void *handle, const char *name) {
    EAGL4::HashPointer *h = reinterpret_cast<EAGL4::HashPointer *>(handle);
    EAGL4::ELF32_Sym *s = h->symtab;
    unsigned long j = h->hash[EAGL4::elfhash(name)];

    while (j != -1) {
        if (h->isOriginal[j] && strcmp(name, &h->strtab[s[j].st_name]) == 0) {
            int iIndex = s[j].st_shndx;
            if (iIndex > 0 && iIndex < h->e->e_shnum) {
                return reinterpret_cast<void *>(h->sections[iIndex].sh_offset + s[j].st_value);
            }
        }
        j = h->chain[j];
    }
    return nullptr;
}

namespace EAGL4 {

bool DynamicLoader::GetNextSymbol(const char *type, int &iIndex, Symbol &result) const {
    for (; iIndex < GetCount(); iIndex++) {
        Symbol s = GetSymbol(iIndex);
        if (strcmp(type, s.type) == 0) {
            result = s;
            iIndex++;
            return true;
        }
    }
    return false;
}

bool DynamicLoader::GetNextAddr(const char *type, int &iIndex, void *&addr) const {
    Symbol s;
    if (GetNextSymbol(type, iIndex, s)) {
        addr = s.data;
        return true;
    } else {
        return false;
    }
}

}; // namespace EAGL4
