#include "eagl4supportsympool.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#include <cstring>

namespace EAGL4 {

unsigned int INIT_TABLE_SIZE = 256;

SymbolPool::SymbolPool() {
    mTableLength = INIT_TABLE_SIZE;
    mTableSize = 0;
    mpSymbolTable = nullptr;
    mpFunctions = nullptr;
}

void SymbolPool::FreeMemory() {}

SymbolPool::~SymbolPool() {}

unsigned int SymbolPool::HashFunction(const char *Name) {
    int i = 0;
    int val1 = 0;
    while (Name[i]) {
        val1 ^= (val1 << 5) ^ Name[i];
        i++;
    }
    return val1 % mTableLength;
}

void SymbolPool::Insert(const char *Name, void *value) {
    unsigned int i;

    if (!mpSymbolTable) {
        mTableLength = EAGL4::INIT_TABLE_SIZE;
        mpSymbolTable = reinterpret_cast<EAGL4Internal::SymbolEntry **>(
            EAGL4Internal::EAGL4Malloc(EAGL4::INIT_TABLE_SIZE * sizeof(*mpSymbolTable), "EAGL4::SymbolPool::mpSymbolTable"));
        for (i = 0; i < mTableLength; i++) {
            mpSymbolTable[i] = nullptr;
        };
    }

    unsigned int index = HashFunction(Name);

    while (mpSymbolTable[index]) {
        index++;
        if (index >= mTableLength) {
            index = 0;
        }
    }
    mpSymbolTable[index] = reinterpret_cast<EAGL4Internal::SymbolEntry *>(value);
    mTableSize++;
    if (mTableSize >= mTableLength) {
        const int oldTableLength = mTableLength;
        EAGL4Internal::SymbolEntry **oldTable = mpSymbolTable;
        mTableLength = oldTableLength * 2;

        mpSymbolTable = reinterpret_cast<EAGL4Internal::SymbolEntry **>(
            EAGL4Internal::EAGL4Malloc(mTableLength * sizeof(*mpSymbolTable), "EAGL4::SymbolPool::mpSymbolTable"));

        for (i = 0; i < mTableLength; i++) {
            mpSymbolTable[i] = nullptr;
        }

        mTableSize = 0;
        for (int i = 0; i < oldTableLength; i++) {
            Insert(reinterpret_cast<const char *>(oldTable[i] + 1), oldTable[i]);
        }

        if (oldTable) {
            EAGL4Internal::EAGL4Free(oldTable, oldTableLength);
        }
    }
}

const char *SymbolPool::AddSymbol(const char *Name, void *Address) {
    // ???
    void *p = (void *)Name;
    EAGL4Internal::SymbolEntry *n = new (p) EAGL4Internal::SymbolEntry();
    p = n;
    n->Address = Address;
    strcpy(reinterpret_cast<char *>(n + 1), Name);
    Insert(Name, p);

    return reinterpret_cast<char *>(n + 1);
}

// Fake match
void *SymbolPool::Search(const char *Name, bool &Valid) {
    if (!mpSymbolTable) {
        Valid = false;
        return nullptr;
    }

    unsigned int index = HashFunction(Name);
    unsigned int count = 0;

    EAGL4Internal::FunctionEntry *pFunction; //= reinterpret_cast<EAGL4Internal::FunctionEntry *>(mpSymbolTable[index]);
    EAGL4Internal::SymbolEntry *pSymbol;
    while (!(pSymbol = mpSymbolTable[index]) || strcmp(Name, (char *)(pSymbol + 1)) != 0 ||
           !(pFunction = reinterpret_cast<EAGL4Internal::FunctionEntry *>(mpSymbolTable[index]))) {
        if (count > mTableLength) {
            goto end;
        }
        index++;
        if (index >= mTableLength) {
            index = 0;
        }
        count++;

        /*pSymbol = mpSymbolTable[index];
        if (pSymbol && strcmp(Name, (char *)(pSymbol + 1)) == 0) {
            EAGL4Internal::FunctionEntry *pFunction = reinterpret_cast<EAGL4Internal::FunctionEntry *>(mpSymbolTable[index]);
            void *pSymbolAddress;
            if (pFunction) {
                 Valid = 1; // causes lots of regswaps
                return pFunction;
            }
        }*/
    }
    Valid = 1;
    return *(void **)pFunction;

end:
    Valid = false;
    return nullptr;
}

}; // namespace EAGL4
