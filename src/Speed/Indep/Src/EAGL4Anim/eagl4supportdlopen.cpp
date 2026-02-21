#include "eagl4supportdlopen.h"
#include "eagl4supportconspool.h"
#include "eagl4supportdef.h"

#include <cstring>
#include <types.h>

namespace EAGL4 {

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

DynamicLoader::~DynamicLoader() {
    Release();
    RunDestructors();
    if (mpPatchAddresses32) {
        EAGL4Internal::EAGL4Free(mpPatchAddresses32, mMaxPatchAddresses);
    }
    mpPatchAddresses32 = nullptr;
}

// TODO where does this go?
static HashPointer *hashhead;

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
        // r.type = nullptr;
        // r.data = nullptr;
        // r.isInternalRef = false;
        return r;
    }
    ELF32_Sym *s = h->symtab;
    if (i < 0 || i >= h->symbols_num) {
        r.name = nullptr;
        // r.type = nullptr;
        // r.data = nullptr;
        // r.isInternalRef = false;
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
    } else if (iIndex != 0 && iIndex < h->e->e_shnum) {
        r.data = reinterpret_cast<void *>(h->sections[iIndex].sh_offset + s[i].st_value);
    }

    return r;
}

}; // namespace EAGL4
