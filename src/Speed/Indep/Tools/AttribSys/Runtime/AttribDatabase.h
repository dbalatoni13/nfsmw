#ifndef ATTRIBSYS_ATTRIB_DATABASE_H
#define ATTRIBSYS_ATTRIB_DATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct DatabasePrivate;

struct Database {
    // total size: 0x8
    DatabasePrivate &mPrivates; // offset 0x0, size 0x4
    // const struct __vtbl_ptr_type *_vptr.Database; // offset 0x4, size 0x4

    bool IsInitialized() {}
};

struct DatabasePrivate : public Database {
    // total size: 0x4C
    // struct ClassTable mClasses;                // offset 0x8, size 0x10
    // unsigned int mNumCompiledTypes;            // offset 0x18, size 0x4
    // struct TypeDescPtrVec mCompiledTypes;      // offset 0x1C, size 0x10
    // struct TypeTable mTypes;                   // offset 0x2C, size 0x10
    // struct CollectionList mGarbageCollections; // offset 0x3C, size 0x8
    // struct ClassList mGarbageClasses;          // offset 0x44, size 0x8
};

#endif
