#include "eagl4supportdlopen.h"
#include "eagl4supportconspool.h"
#include "eagl4supportdef.h"

#include <cstring>
#include <types.h>

static void *dlsym(void *handle, const char *name);

inline void *RuntimeAllocDestructorEntry::operator new(size_t size) {
    return EAGL4Internal::EAGL4Malloc(size, nullptr);
}

inline RuntimeAllocDestructorEntry::RuntimeAllocDestructorEntry(RuntimeAllocDestructor d, void *data, int auxData)
    : d(d),       //
      data(data), //
      auxData(auxData) {}

namespace EAGL4 {

namespace {

static const char kSymTabName[] = ".symtab";
static const char kStrTabName[] = ".strtab";
static const char kNamespaceMarker[] = ":::";
static const char kDynamicSymbolsMsg[] = "EAGL4::dynamic symbols";
static const char kDynamicSymbols2Msg[] = "EAGL4::dynamic symbols 2";
static const char gRuntimeAllocType[] = "RUNTIME_ALLOC::";

static inline unsigned short htotus(unsigned short s) {
    return static_cast<unsigned short>((s << 8) | (s >> 8));
}

static inline unsigned int htotul(unsigned int l) {
    unsigned int ul;

    ul = htotus(static_cast<unsigned short>(l));
    ul <<= 16;
    ul |= htotus(static_cast<unsigned short>(l >> 16));
    return ul;
}

static inline unsigned int ByteSwap32(unsigned int value) {
    return ((value >> 16) & 0xFF) << 8 | (value >> 24) | (((value & 0xFF) << 8 | ((value & 0xFFFF) >> 8)) << 16);
}

} // namespace

inline void *HashPointer::operator new(size_t size) {
    return EAGL4Internal::EAGL4Malloc(size, nullptr);
}

inline HashPointer::HashPointer(DynamicLoader *pDL) {
    e = nullptr;
    mpDynamicLoader = pDL;
    pSearchFunction = nullptr;
    next = nullptr;
    prev = nullptr;
    strtab = nullptr;
    resolved = false;
    symbols_num = 0;
    symtab = nullptr;
    sections = nullptr;
    chain = nullptr;
    isOriginal = nullptr;
}

inline void *DynamicLoader::ELFAddr(unsigned int offset) {
    if (offset < mDataLen) {
        return mpData + offset;
    }
    if (mpReloc) {
        return mpReloc + (offset - mDataLen);
    }
    if (mDataLen < offset) {
        return nullptr;
    }
    return mpData + offset;
}

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
    int i;
    int j;
    HashPointer *h;
    ELFSectionHeader *sheader;
    ELFHeader *e;
    const int MAX_UNRESOLVED_ERRORS = 32;
    bool unresolvedSymbolError;
    char *unresolvedList[32];
    int numUnresolved;

    h = reinterpret_cast<HashPointer *>(handle);
    if (!h || h->resolved) {
        return;
    }

    e = h->e;
    h->resolved = true;
    sheader = h->sections;
    unresolvedSymbolError = false;
    numUnresolved = 0;

retry:
    ;

    for (i = 0; i < e->e_shnum; i++) {
        int relocations;
        char *patchbase;
        ELF32_Rel *r;
        ELF32_Sym *symtab;

        if (sheader[i].sh_type != SHT_REL) {
            continue;
        }

        relocations = static_cast<int>(sheader[i].sh_size);
        if (relocations < 0) {
            relocations += 7;
        }
        patchbase = reinterpret_cast<char *>(sheader[i].sh_vinfo);
        r = reinterpret_cast<ELF32_Rel *>(sheader[i].sh_voffset);
        symtab = reinterpret_cast<ELF32_Sym *>(sheader[i].sh_vlink);
        relocations >>= 3;

        for (j = 0; j < relocations; j++) {
            unsigned int baseaddr = 0;
            int iIndex = j;
            ELF32_Sym *sym;
            unsigned int *patchaddr;
            unsigned short *patchaddr16;

            r[j].r_offset = htotul(r[j].r_offset);
            r[j].r_info = htotul(r[j].r_info);
            sym = &symtab[r[j].r_info >> 8];

            while ((sym->st_info & 0xF) < STT_FILE) {
                if (sym->st_shndx && sym->st_shndx < e->e_shnum) {
                    baseaddr = reinterpret_cast<unsigned int>(sheader[sym->st_shndx].sh_voffset);
                    break;
                }

                HashPointer *hp;

                for (hp = hashhead; hp; hp = hp->next) {
                    if (hp != h) {
                        void *addr = dlsym(hp, &h->strtab[sym->st_name]);

                        if (addr) {
                            sym->st_shndx = 1;
                            sym->st_other = 2;
                            baseaddr = reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                            sym->st_value = reinterpret_cast<unsigned int>(addr) - baseaddr;
                            break;
                        }
                    }
                }

                if (hp) {
                    break;
                }

                if (h->pSearchFunction) {
                    bool valid;
                    void *addr = h->pSearchFunction(&h->strtab[sym->st_name], valid);

                    if (valid) {
                        sym->st_shndx = 1;
                        sym->st_other = 3;
                        baseaddr = reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                        sym->st_value = reinterpret_cast<unsigned int>(addr) - baseaddr;
                        break;
                    }
                }

                {
                    bool valid;
                    void *addr = gSymbolPool.Search(&h->strtab[sym->st_name], valid);

                    if (valid) {
                        sym->st_shndx = 1;
                        sym->st_other = 4;
                        baseaddr = reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                        sym->st_value = reinterpret_cast<unsigned int>(addr) - baseaddr;
                        break;
                    }
                }

                {
                    Symbol s;

                    s.name = &h->strtab[sym->st_name];
                    s.type = s.name + strlen(s.name) + 1;
                    if (s.type[0] == 0x7F) {
                        s.type++;
                    } else {
                        s.type--;
                    }
                    if (strncmp(gRuntimeAllocType, s.name, strlen(gRuntimeAllocType)) == 0) {
                        const char *stripped_name;
                        RuntimeAllocConstructor c;

                        stripped_name = s.name + strlen(gRuntimeAllocType);
                        c = gRuntimeAllocConsPool.FindConstructor(s.type);

                        if (c) {
                            RuntimeAllocDestructor d = gRuntimeAllocConsPool.FindDestructor(s.type);
                            int auxData;
                            bool bCallDestructor = false;
                            s.data = c(stripped_name, reinterpret_cast<class DynamicLoader *>(this), auxData, bCallDestructor, s.name);

                            if (bCallDestructor && s.data) {
                                RuntimeAllocDestructorEntry *de = new RuntimeAllocDestructorEntry(d, s.data, auxData);
                                de->next = RuntimeAllocDestructors;
                                RuntimeAllocDestructors = de;
                            }

                            sym->st_shndx = 1;
                            sym->st_other = 5;
                            baseaddr = reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                            sym->st_value = reinterpret_cast<unsigned int>(s.data) - baseaddr;
                            break;
                        }
                    }

                    if (!unresolvedSymbolError) {
                        unresolvedSymbolError = true;
                    }
                    if (numUnresolved < MAX_UNRESOLVED_ERRORS) {
                        bool found = false;

                        for (j = 0; j < numUnresolved; j++) {
                            if (&h->strtab[sym->st_name] == unresolvedList[j]) {
                                found = true;
                                break;
                            }
                        }

                        if (!found) {
                            unresolvedList[numUnresolved++] = &h->strtab[sym->st_name];
                        }
                        j = iIndex;
                    }
                }
                break;
            }

            baseaddr += sym->st_value;
            patchaddr = reinterpret_cast<unsigned int *>(patchbase + r[j].r_offset);
            *patchaddr = htotul(*patchaddr);
            patchaddr16 = reinterpret_cast<unsigned short *>(patchaddr);

            switch (r[j].r_info & 0xFF) {
                case R_MIPS_32:
                    *patchaddr += baseaddr;
                    break;

                case R_MIPS_26:
                    *patchaddr = (*patchaddr & 0xFC000000) | ((baseaddr + ((*patchaddr & 0x03FFFFFF) * 4) & 0x0FFFFFFF) >> 2);
                    break;

                case R_MIPS_HI16:
                    *patchaddr16 = static_cast<unsigned short>(*patchaddr16 + static_cast<unsigned short>(baseaddr >> 16));
                    continue;

                case R_MIPS_LO16:
                    *patchaddr16 = static_cast<unsigned short>(*patchaddr16 + static_cast<unsigned short>(baseaddr));
                    continue;
            }
        }
    }

    RunConstructors();
    mIsResolved = true;
}

void DynamicLoader::Initialize(DynamicUserCallback pSearchFunction) {
    int i;
    HashPointer *pHP = new HashPointer(this);
    HashPointer &h = *pHP;
    ELFSectionHeader *sheader;
    ELFHeader *e;
    char *shstrtab;
    void *p;

    e = reinterpret_cast<ELFHeader *>(mpData);
    h.e = e;

    e->e_type = htotus(e->e_type);
    e->e_machine = htotus(e->e_machine);
    e->e_version = htotul(e->e_version);
    e->e_entry = htotul(e->e_entry);
    e->e_phoff = htotul(e->e_phoff);
    e->e_shoff = htotul(e->e_shoff);
    e->e_flags = htotul(e->e_flags);
    e->e_ehsize = htotus(e->e_ehsize);
    e->e_phentsize = htotus(e->e_phentsize);
    e->e_phnum = htotus(e->e_phnum);
    e->e_shentsize = htotus(e->e_shentsize);
    e->e_shnum = htotus(e->e_shnum);
    e->e_shstrndx = htotus(e->e_shstrndx);

    p = ELFAddr(e->e_shoff);
    h.sections = reinterpret_cast<ELFSectionHeader *>(p);

    for (i = 0; i < e->e_shnum; i++) {
        sheader = &h.sections[i];

        sheader->sh_name = htotul(sheader->sh_name);
        sheader->sh_type = htotul(sheader->sh_type);
        sheader->sh_flags = htotul(sheader->sh_flags);
        sheader->sh_addr = htotul(sheader->sh_addr);
        sheader->sh_offset = htotul(sheader->sh_offset);
        sheader->sh_size = htotul(sheader->sh_size);
        sheader->sh_link = htotul(sheader->sh_link);
        sheader->sh_info = htotul(sheader->sh_info);
        sheader->sh_addralign = htotul(sheader->sh_addralign);
        sheader->sh_entsize = htotul(sheader->sh_entsize);

        p = ELFAddr(sheader->sh_offset);
        sheader->sh_voffset = p;
    }

    shstrtab = reinterpret_cast<char *>(h.sections[e->e_shstrndx].sh_voffset);

    for (i = 0; i < e->e_shnum; i++) {
        sheader = &h.sections[i];

        if (sheader->sh_type == SHT_STRTAB) {
            sheader->sh_vlink = h.sections[sheader->sh_link].sh_voffset;
            if (strcmp(kStrTabName, shstrtab + sheader->sh_name) == 0) {
                char *s = reinterpret_cast<char *>(sheader->sh_voffset);
                int len = sheader->sh_size;
                const char *TYPE_SEPARATOR = kNamespaceMarker;
                int slen;

                h.strtab = s;
                while (len > 0) {
                    slen = strlen(s);
                    if (s[0] == '_' && s[1] == '_') {
                        char *t = strstr(s + 2, TYPE_SEPARATOR);
                        if (t) {
                            char typebuf[128];
                            char *type_separator = t + strlen(TYPE_SEPARATOR);
                            unsigned int nameLength;

                            *t = 0;
                            strcpy(typebuf, s + 2);
                            nameLength = strlen(type_separator);
                            memmove(s, type_separator, nameLength + 1);
                            s[nameLength + 1] = '\x7F';
                            strcpy(s + nameLength + 2, typebuf);
                        }
                    }
                    s += slen + 1;
                    len -= slen + 1;
                }
            }
        } else if (sheader->sh_type == SHT_SYMTAB || sheader->sh_type == SHT_STRTAB) {
            sheader->sh_vlink = h.sections[sheader->sh_link].sh_voffset;
            if (strcmp(kSymTabName, shstrtab + sheader->sh_name) == 0) {
                h.symtab = reinterpret_cast<ELF32_Sym *>(sheader->sh_voffset);
                h.symbols_num = static_cast<int>(sheader->sh_size >> 4);
            }
        } else if (sheader->sh_type == SHT_REL) {
            sheader->sh_vinfo = h.sections[sheader->sh_info].sh_voffset;
            sheader->sh_vlink = h.sections[sheader->sh_link].sh_voffset;
        }
    }

    h.pSearchFunction = pSearchFunction;

    for (i = 0; i < 0x100; i++) {
        h.hash[i] = static_cast<unsigned long>(-1);
    }

    h.chain = reinterpret_cast<unsigned long *>(EAGL4Internal::EAGL4Malloc(h.symbols_num * sizeof(unsigned long), kDynamicSymbolsMsg));
    h.isOriginal = reinterpret_cast<bool *>(EAGL4Internal::EAGL4Malloc(h.symbols_num * sizeof(unsigned long), kDynamicSymbols2Msg));

    for (i = 0; i < h.symbols_num; i++) {
        ELF32_Sym *sym = &h.symtab[i];

        sym->st_name = htotul(sym->st_name);
        sym->st_value = htotul(sym->st_value);
        sym->st_size = htotul(sym->st_size);
        sym->st_shndx = htotus(sym->st_shndx);

        unsigned long hash = elfhash(h.strtab + sym->st_name);
        h.chain[i] = h.hash[hash];
        h.hash[hash] = i;

        if (sym->st_shndx == 0 || h.e->e_shnum <= sym->st_shndx) {
            h.isOriginal[i] = false;
        } else {
            h.isOriginal[i] = true;
        }
    }

    h.next = hashhead;
    if (hashhead) {
        hashhead->prev = &h;
    }
    h.prev = nullptr;
    hashhead = &h;
    handle = &h;
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

        EAGL4Internal::EAGL4Free(h, sizeof(HashPointer));
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
    if (r.type[1] == 0x7F) {
        r.type += 2;
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
