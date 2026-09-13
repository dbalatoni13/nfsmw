#ifndef FEPACKAGEREADER_H_
#define FEPACKAGEREADER_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEChunk.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FETypes.h"

struct FETypeSize;

// total size: 0x4
// Decl: 31
class FETag {
  private:
    u16 ID;   // offset 0x0, size 0x2, Decl: 33
    u16 Size; // offset 0x2, size 0x2, Decl: 34

  public:
    u16 GetID() { // Decl: 37
        return FEngGetu16(ID);
    }
    u16 GetSize() { // Decl: 38
        return FEngGetu16(Size);
    }

    u8 *Data() { // Decl: 40
        return reinterpret_cast<u8 *>(this) + sizeof(this);
    }
    u32 Getu32(u32 Index) {
        return FEngGetu32(reinterpret_cast<u32 *>(Data())[Index]);
    }
    i32 Geti32(u32 Index) {
        return FEngGeti32(reinterpret_cast<i32 *>(Data())[Index]);
    }
    u16 Getu16(u32 Index) {
        return FEngGetu16(reinterpret_cast<u16 *>(Data())[Index]);
    }
    i16 Geti16(u32 Index) {
        return FEngGeti16(reinterpret_cast<i16 *>(Data())[Index]);
    }
    f32 Getf32(u32 Index) {
        return FEngGetf32(reinterpret_cast<f32 *>(Data())[Index]);
    }

    FETag *Next() { // Decl: 48
        return reinterpret_cast<FETag *>(reinterpret_cast<u8 *>(this) + sizeof(this) + GetSize());
    }
};

// total size: 0x58
// Decl: 56
class FEPackageReader {
  private:
    FEPackage *pPack;                                    // offset 0x0, size 0x4, Decl: 58
    FEChunk *pChunk;                                     // offset 0x4, size 0x4, Decl: 59
    bool bIsLibrary, bLoadObjectNames, bLoadScriptNames; // offset 0x8, size 0x1, Decl: 60
    FEObject *pObj;                                      // offset 0x14, size 0x4, Decl: 62
    FEGroup *pLastParent, *pParent;                      // offset 0x18, size 0x4, Decl: 63
    bool bIsReference;                                   // offset 0x20, size 0x1, Decl: 65
    FEObject *pRefObj;                                   // offset 0x24, size 0x4, Decl: 66
    FEPackage *pRefPack;                                 // offset 0x28, size 0x4, Decl: 67
    u32 CurListCol, CurListRow, CurListCell;             // offset 0x2C, size 0x4, Decl: 70
    u32 TypeSizeCount;                                   // offset 0x38, size 0x4, Decl: 72
    FETypeSize *TypeSizeList;                            // offset 0x3C, size 0x4, Decl: 73
    FEGameInterface *pInterface;                         // offset 0x40, size 0x4, Decl: 75
    FEngine *pEngine;                                    // offset 0x44, size 0x4, Decl: 76
    u32 ResourceCount;                                   // offset 0x48, size 0x4, Decl: 78
    u32 ObjectCount;                                     // offset 0x4C, size 0x4, Decl: 79
    u32 ButtonCount;                                     // offset 0x50, size 0x4, Decl: 80
    u32 CurButton;                                       // offset 0x54, size 0x4, Decl: 81

    FEChunk *FindChild(FEChunk *pChunk, u32 ID); // Decl: 83
    u32 GetTypeSize(u32 TypeID);                 // Decl: 84

    bool ReadTypeSizes();               // Decl: 86
    bool ReadHeaderChunk();             // Decl: 87
    bool ReadReferencedPackagesChunk(); // Decl: 88
    bool ReadLibraryRefsChunk();        // Decl: 89
    bool ReadResourceChunk();           // Decl: 90
    bool ReadPackageResponseChunk();    // Decl: 91
    bool ReadObjectChunk();             // Decl: 92

    FEObject *CreateObject(u32 Type); // Decl: 94

    bool ReadObjectTags(FETag *pTag, u32 Length); // Decl: 96
    void ProcessStringTag(FETag *pTag);           // Decl: 97
    void ProcessImageTag(FETag *pTag);            // Decl: 98
    void ProcessMultiImageTag(FETag *pTag);       // Decl: 99
    void ProcessListBoxTag(FETag *pTag);          // Decl: 100
    void ProcessCodeListBoxTag(FETag *pTag);      // Decl: 101

    bool ReadScriptTags(FETag *pTag, u32 Length);                         // Decl: 103
    bool ReadMessageResponseTags(FETag *pTag, u32 Length, bool bPackage); // Decl: 104
    bool ReadMessageTargetListChunk();                                    // Decl: 105

    bool FindReferencedObject(u32 ObjGUID, FEObject **pRefObj,
                              FEPackage **pRefPack); // Decl: 112

  public:
    FEPackageReader();  // Decl: 116
    ~FEPackageReader(); // Decl: 117

    void Reset(); // Decl: 120

    FEPackage *Load(const void *pDataPtr, FEGameInterface *pInt, FEngine *pEng, bool bLoadObjNames, bool bLoadScrNames,
                    bool bLibrary); // Decl: 131
};

#endif
