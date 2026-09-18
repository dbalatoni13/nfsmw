#include "StringRegistry.h"

// Decl: StringRegistry.cpp GC
StringRegistry::StringRegistry() {
}

// Decl: StringRegistry.cpp GC
StringPool::StringPool() {
    mStrings = 0;
    mStorage = 0;
}

std::vector<StringStoreBlockPtr> StringStoreBlockPtr::sStringStoreBlockPtrCache;

StringRegistry StringRegistry::fgThis;
