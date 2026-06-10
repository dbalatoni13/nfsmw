#ifndef FEPACKAGEREADER_H_
#define FEPACKAGEREADER_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEChunk.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FETypes.h"

struct FETypeSize;

// File: speed/indep/src/feng/FEPackageReader.h
// total size: 0x4
// Decl: speed/indep/src/feng/FEPackageReader.h:31
class FETag {
  private:
    u16 ID;   // offset 0x0, size 0x2, Decl: speed/indep/src/feng/FEPackageReader.h:33
    u16 Size; // offset 0x2, size 0x2, Decl: speed/indep/src/feng/FEPackageReader.h:34

  public:
    u16 GetID() { // Decl: speed/indep/src/feng/FEPackageReader.h:37
        return FEngGetu16(ID);
    }
    u16 GetSize() { // Decl: speed/indep/src/feng/FEPackageReader.h:38
        return FEngGetu16(Size);
    }

    u8 *Data() { // Decl: speed/indep/src/feng/FEPackageReader.h:40
        return reinterpret_cast<unsigned char *>(this) + 4;
    }
    u32 Getu32(u32 Index) {
        return FEngGetu32(reinterpret_cast<u32 *>(Data())[Index]);
    }
    i32 Geti32(u32 Index) {
        return reinterpret_cast<i32 *>(Data())[Index];
    }
    u16 Getu16(u32 Index) {
        return reinterpret_cast<u16 *>(Data())[Index];
    }
    i16 Geti16(u32 Index) {
        return reinterpret_cast<i16 *>(Data())[Index];
    }
    f32 Getf32(u32 Index) {
        return FEngGetf32(reinterpret_cast<f32 *>(Data())[Index]);
    }

    FETag *Next() { // Decl: speed/indep/src/feng/FEPackageReader.h:48
        return reinterpret_cast<FETag *>(Data() + GetSize());
    }
};

// total size: 0x58
// Decl: speed/indep/src/feng/FEPackageReader.h:56
class FEPackageReader {
  private:
    FEPackage *pPack;                                    // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:58
    FEChunk *pChunk;                                     // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:59
    bool bIsLibrary, bLoadObjectNames, bLoadScriptNames; // offset 0x8, size 0x1, Decl: speed/indep/src/feng/FEPackageReader.h:60
    FEObject *pObj;                                      // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:62
    FEGroup *pLastParent, *pParent;                      // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:63
    bool bIsReference;                                   // offset 0x20, size 0x1, Decl: speed/indep/src/feng/FEPackageReader.h:65
    FEObject *pRefObj;                                   // offset 0x24, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:66
    FEPackage *pRefPack;                                 // offset 0x28, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:67
    u32 CurListCol, CurListRow, CurListCell;             // offset 0x2C, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:70
    u32 TypeSizeCount;                                   // offset 0x38, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:72
    FETypeSize *TypeSizeList;                            // offset 0x3C, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:73
    FEGameInterface *pInterface;                         // offset 0x40, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:75
    FEngine *pEngine;                                    // offset 0x44, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:76
    u32 ResourceCount;                                   // offset 0x48, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:78
    u32 ObjectCount;                                     // offset 0x4C, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:79
    u32 ButtonCount;                                     // offset 0x50, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:80
    u32 CurButton;                                       // offset 0x54, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.h:81

    FEChunk *FindChild(FEChunk *pChunk, u32 ID); // Decl: speed/indep/src/feng/FEPackageReader.h:83
    u32 GetTypeSize(u32 TypeID);                 // Decl: speed/indep/src/feng/FEPackageReader.h:84

    bool ReadTypeSizes();               // Decl: speed/indep/src/feng/FEPackageReader.h:86
    bool ReadHeaderChunk();             // Decl: speed/indep/src/feng/FEPackageReader.h:87
    bool ReadReferencedPackagesChunk(); // Decl: speed/indep/src/feng/FEPackageReader.h:88
    bool ReadLibraryRefsChunk();        // Decl: speed/indep/src/feng/FEPackageReader.h:89
    bool ReadResourceChunk();           // Decl: speed/indep/src/feng/FEPackageReader.h:90
    bool ReadPackageResponseChunk();    // Decl: speed/indep/src/feng/FEPackageReader.h:91
    bool ReadObjectChunk();             // Decl: speed/indep/src/feng/FEPackageReader.h:92

    FEObject *CreateObject(u32 Type); // Decl: speed/indep/src/feng/FEPackageReader.h:94

    bool ReadObjectTags(FETag *pTag, u32 Length); // Decl: speed/indep/src/feng/FEPackageReader.h:96
    void ProcessStringTag(FETag *pTag);           // Decl: speed/indep/src/feng/FEPackageReader.h:97
    void ProcessImageTag(FETag *pTag);            // Decl: speed/indep/src/feng/FEPackageReader.h:98
    void ProcessMultiImageTag(FETag *pTag);       // Decl: speed/indep/src/feng/FEPackageReader.h:99
    void ProcessListBoxTag(FETag *pTag);          // Decl: speed/indep/src/feng/FEPackageReader.h:100
    void ProcessCodeListBoxTag(FETag *pTag);      // Decl: speed/indep/src/feng/FEPackageReader.h:101

    bool ReadScriptTags(FETag *pTag, u32 Length);                         // Decl: speed/indep/src/feng/FEPackageReader.h:103
    bool ReadMessageResponseTags(FETag *pTag, u32 Length, bool bPackage); // Decl: speed/indep/src/feng/FEPackageReader.h:104
    bool ReadMessageTargetListChunk();                                    // Decl: speed/indep/src/feng/FEPackageReader.h:105

    bool FindReferencedObject(u32 ObjGUID, FEObject **pRefObj,
                              FEPackage **pRefPack); // Decl: speed/indep/src/feng/FEPackageReader.h:112

  public:
    FEPackageReader();  // Decl: speed/indep/src/feng/FEPackageReader.h:116
    ~FEPackageReader(); // Decl: speed/indep/src/feng/FEPackageReader.h:117

    void Reset(); // Decl: speed/indep/src/feng/FEPackageReader.h:120

    FEPackage *Load(const void *pDataPtr, FEGameInterface *pInt, FEngine *pEng, bool bLoadObjNames, bool bLoadScrNames,
                    bool bLibrary); // Decl: speed/indep/src/feng/FEPackageReader.h:131
};

#endif
