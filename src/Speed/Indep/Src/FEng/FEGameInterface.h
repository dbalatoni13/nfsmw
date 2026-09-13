#ifndef FEGAMEINTERFACE_H_
#define FEGAMEINTERFACE_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEMouse.h"
// #include "Speed/Indep/Src/FEng/FEPackage.h"

class FEPackage;

// Decl: unknown, but probably here
enum FEResourceRequestFlags {
    FR_UnnecessaryLoad = -0x80000000,
    FR_ShapeResource = 0x40000000,
};

// total size: 0x18
// Decl: 34
typedef struct {
    u32 ID;                // offset 0x0, size 0x4, Decl: 35
    const char *pFilename; // offset 0x4, size 0x4, Decl: 36
    u32 Type;              // offset 0x8, size 0x4, Decl: 37
    u32 Flags;             // offset 0xC, size 0x4, Decl: 38

    u32 Handle;    // offset 0x10, size 0x4, Decl: 41
    u32 UserParam; // offset 0x14, size 0x4, Decl: 42
} FEResourceRequest;

// total size: 0x8
// Decl: 50
typedef struct {
    FEObject *pObj; // offset 0x0, size 0x4, Decl: 51
    u32 uSortKey;   // offset 0x4, size 0x4, Decl: 52
} FEObjectListEntry;

// Decl: 56
typedef enum FEng_WarningLevel {
    FEng_NonWarning = 0,
    FEng_SoftWarning = 1,
    FEng_HardWarning = 2,
} FEng_WarningLevel;

// total size: 0x4
// Decl: 66
class FEGameInterface {
  public:
    virtual u8 *GetPackageData(const char *pPackageName, u8 **pBlockStart, bool &bDeleteBlock) = 0; // [1]
    virtual bool LoadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) = 0;       // [2]
    virtual bool UnloadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) = 0;     // [3]
    virtual void PackageWasLoaded(FEPackage *pPackage) = 0;                                         // [4]
    virtual bool PackageWillUnload(FEPackage *pPackage) = 0;                                        // [5]
    virtual bool UnloadUnreferencedLibrary() {                                                      // Decl: 96
        return false;
    }; // [6]
    virtual void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) = 0;          // [7]
    virtual void NotifySoundMessage(u32 Message, FEObject *pObject, u32 ControlMask, u32 pPackagePtr) = 0; // [8]
    virtual void BeginPackageRendering(FEPackage *pPackage) = 0;                                           // [9]
    virtual void EndPackageRendering(FEPackage *pPackage) = 0;                                             // [10]
    virtual void GenerateRenderContext(u16 uContext, FEObject *pObject) = 0;                               // [11]
    virtual bool GetContextTransform(u16 uContext, FEMatrix4 &Matrix) = 0;                                 // [12]
    virtual void RenderObjectList(FEObjectListEntry *pList, u32 Count) {                                   // Decl: 126
        while (Count) {
            Count--;
            RenderObject(pList[Count].pObj);
        }
    }; // [13]
    virtual void RenderObject(FEObject *pObject) = 0;                                 // [14]
    virtual void DrawMousePointer(FEMouse &) {}                                       // [15] // Decl: 142
    virtual void GetViewTransformation(FEMatrix4 *pView) = 0;                         // [16]
    virtual u32 GetJoyPadMask(u8 feng_pad_index) = 0;                                 // [17]
    virtual void GetMouseInfo(FEMouseInfo &Info) = 0;                                 // [18]
    virtual bool DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) = 0; // [19] // Decl: 160
    virtual bool SetCellData(FECodeListBox *, u32, u32) {                             // Decl: 168
        return false;
    }; // [20]
    virtual void OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) {}   // [21] // Decl: 174
    virtual void DebugMessageQueued(u32, FEObject *, FEPackage *, FEObject *, u32) {}    // [22] // Decl: 180
    virtual void DebugMessageProcessed(u32, FEObject *, FEObject *, FEPackage *, u32) {} // [23] // Decl: 183
    virtual void DebugMessageBeginUpdate() {}                                            // [24] // Decl: 185
    virtual void DebugMessageEndUpdate() {}                                              // [25] // Decl: 186
};

void HackClearCache(FEPackage *pkg);

#endif
