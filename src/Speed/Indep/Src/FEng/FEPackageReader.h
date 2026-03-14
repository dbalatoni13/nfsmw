#ifndef FENG_FEPACKAGEREADER_H
#define FENG_FEPACKAGEREADER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FEPackage;
struct FEChunk;
struct FETag;
struct FEObject;
struct FEGroup;
struct FEGameInterface;
struct FEngine;
struct FEScript;

// total size: 0x8
struct FETypeSize {
    unsigned long ID;   // offset 0x0, size 0x4
    unsigned long Size; // offset 0x4, size 0x4
};

// total size: 0x58
struct FEPackageReader {
    FEPackage* pPack;               // offset 0x0, size 0x4
    FEChunk* pChunk;                // offset 0x4, size 0x4
    bool bIsLibrary;                // offset 0x8, size 0x1
    bool bLoadObjectNames;          // offset 0xC, size 0x1
    bool bLoadScriptNames;          // offset 0x10, size 0x1
    FEObject* pObj;                 // offset 0x14, size 0x4
    FEGroup* pLastParent;           // offset 0x18, size 0x4
    FEGroup* pParent;               // offset 0x1C, size 0x4
    bool bIsReference;              // offset 0x20, size 0x1
    FEObject* pRefObj;              // offset 0x24, size 0x4
    FEPackage* pRefPack;            // offset 0x28, size 0x4
    unsigned long CurListCol;       // offset 0x2C, size 0x4
    unsigned long CurListRow;       // offset 0x30, size 0x4
    unsigned long CurListCell;      // offset 0x34, size 0x4
    unsigned long TypeSizeCount;    // offset 0x38, size 0x4
    FETypeSize* TypeSizeList;       // offset 0x3C, size 0x4
    FEGameInterface* pInterface;    // offset 0x40, size 0x4
    FEngine* pEngine;               // offset 0x44, size 0x4
    unsigned long ResourceCount;    // offset 0x48, size 0x4
    unsigned long ObjectCount;      // offset 0x4C, size 0x4
    unsigned long ButtonCount;      // offset 0x50, size 0x4
    unsigned long CurButton;        // offset 0x54, size 0x4

    FEPackageReader();
    ~FEPackageReader();
    void Reset();
    FEPackage* Load(const void* pDataPtr, FEGameInterface* pInt, FEngine* pEng, bool bLoadObjNames, bool bLoadScrNames, bool bLibrary);
    void Read(FEPackage* pPackage, FEChunk* pData, FEngine* pEng);
    void ReadChunk(FEChunk* pChunk);
    void ReadObject(FEChunk* pChunk);
    void ReadScriptTags(FETag* pTag, unsigned long TagSize);
    void ReadObjectTags(FETag* pTag, unsigned long TagSize);
    void ProcessObjectTag(FETag* pTag);
    void ProcessScriptTag(FETag* pTag, FEScript* pScript);
    void ProcessListBoxTag(FETag* pTag);
    void ProcessCodeListBoxTag(FETag* pTag);
    void ProcessCommentTag(FETag* pTag);
    FEObject* CreateObject(unsigned long ObjectType, unsigned long TypeID);
    void ReadReferenceObject(FEChunk* pChunk);
    void ReadReferenceObjectTags(FETag* pTag, unsigned long TagSize);
    void ReadLibraryChunk(FEChunk* pChunk);
    void ReadResourceChunk(FEChunk* pChunk);
    void ReadMessageTargets(FEChunk* pChunk);
    void ReadButtonMapChunk(FEChunk* pChunk);
    void CountChunkResources(FEChunk* pChunk);
    unsigned long GetTypeSizeFromID(unsigned long TypeID);
    unsigned long CountChunkObjects(FEChunk* pChunk);
    void SetupListBoxResource(FETag* pTag, unsigned long ulResHandle, unsigned long ulResParam, unsigned long ulResIndex);
};

#endif
