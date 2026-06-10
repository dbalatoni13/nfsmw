#ifndef FENGINE_H_
#define FENGINE_H_

#include <types.h>

#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "FEPackage.h"
#include "FEMouse.h"
#include "Speed/Indep/Src/FEng/FEObjectSorter.h"
#include "Speed/Indep/Src/FEng/FEPackageList.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEButtonMap.h"
#include "Speed/Indep/Src/FEng/FETypeLib.h"

#define MsgTarget_All ((FEObject *)0)                 // :26
#define MsgTarget_Game ((FEObject *)-1)               // :27
#define MsgTarget_Global ((FEObject *)-2)             // :28
#define MsgTarget_Package ((FEObject *)-3)            // :29
#define MsgTarget_PackageAndContents ((FEObject *)-4) // :30
#define MsgTarget_Sound ((FEObject *)-5)              // :31
#define MsgTarget_Input ((FEObject *)-6)              // :32
#define MsgTargetValue_All 0                          // :34
#define MsgTargetValue_Game -1                        // :35
#define MsgTargetValue_Global -2                      // :36
#define MsgTargetValue_Package -3                     // :37
#define MsgTargetValue_PackageAndContents -4          // :38
#define MsgTargetValue_Sound -5                       // :39
#define MsgTargetValue_Input -6                       // :40

// Decl: speed/indep/src/feng/fengine.h:52
typedef enum {
    FE_PACKAGE_PRIORITY_FIFTH_CLOSEST = 100,
    FE_PACKAGE_PRIORITY_FOURTH_CLOSEST = 101,
    FE_PACKAGE_PRIORITY_THIRD_CLOSEST = 102,
    FE_PACKAGE_PRIORITY_SECOND_CLOSEST = 103,
    FE_PACKAGE_PRIORITY_CLOSEST = 104,
    FE_PACKAGE_PRIORITY_ERROR = 105
} FE_PACKAGE_PRIORITY;

// total size: 0x8
// Decl: speed/indep/src/feng/fengine.h:68
struct FEPackageButtonRec {
    u32 PackageHash; // offset 0x0, size 0x4
    u32 ButtonGUID;  // offset 0x4, size 0x4
};

// File: speed/indep/src/feng/FEngine.h
// total size: 0x20
// Decl: speed/indep/src/feng/FEngine.h:84
class FEPackageCommand : public FENode {
  public:
    FEPackageCommand() {} // Decl: speed/indep/src/feng/FEngine.h:86
    ~FEPackageCommand() override {}

    i32 iCommand;        // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FEngine.h:87
    u32 uControlMask;    // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FEngine.h:88
    FEPackage *pPackage; // offset 0x1C, size 0x4, Decl: speed/indep/src/feng/FEngine.h:89
};

// total size: 0x5268
// Decl: speed/indep/src/feng/FEngine.h:97
class FEngine {
  private:
    bool bExecuting;       // offset 0x0, size 0x1, Decl: speed/indep/src/feng/FEngine.h:106
    bool bMouseActive;     // offset 0x4, size 0x1, Decl: speed/indep/src/feng/FEngine.h:107
    bool bLoadObjectNames; // offset 0x8, size 0x1, Decl: speed/indep/src/feng/FEngine.h:108
    bool bLoadScriptNames; // offset 0xC, size 0x1, Decl: speed/indep/src/feng/FEngine.h:109

    FEJoyPad *pJoyPad; // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FEngine.h:111
    FEMouse Mouse;     // offset 0x14, size 0x24, Decl: speed/indep/src/feng/FEngine.h:112

    u32 FastRep, FastRepCache;      // offset 0x38, size 0x4, Decl: speed/indep/src/feng/FEngine.h:114
    u32 PadHoldRegistered;          // offset 0x40, size 0x4, Decl: speed/indep/src/feng/FEngine.h:115
    unsigned int HoldDecrement[19]; // offset 0x44, size 0x4C, Decl: speed/indep/src/feng/FEngine.h:116
    FEObject *HeldButtons[19];      // offset 0x90, size 0x4C, Decl: speed/indep/src/feng/FEngine.h:117
    FEButtonWrapMode WrapMode;      // offset 0xDC, size 0x4, Decl: speed/indep/src/feng/FEngine.h:118

    u32 NumJoyPads;    // offset 0xE0, size 0x4, Decl: speed/indep/src/feng/FEngine.h:120
    u16 uGroupContext; // offset 0xE4, size 0x2, Decl: speed/indep/src/feng/FEngine.h:121

    FEPackageList PackList; // offset 0xE8, size 0x10, Decl: speed/indep/src/feng/FEngine.h:123
    FEList IdleList;        // offset 0xF8, size 0x10, Decl: speed/indep/src/feng/FEngine.h:124

    FEList LibraryList;          // offset 0x108, size 0x10, Decl: speed/indep/src/feng/FEngine.h:126
    FEGameInterface *pInterface; // offset 0x118, size 0x4, Decl: speed/indep/src/feng/FEngine.h:127

    FEObjectSorter<1024> Sorter; // offset 0x11C, size 0x4004, Decl: speed/indep/src/feng/FEngine.h:129
    FEMinList MsgQ;              // offset 0x4120, size 0x10, Decl: speed/indep/src/feng/FEngine.h:130
    FEList PackageCommands;      // offset 0x4130, size 0x10, Decl: speed/indep/src/feng/FEngine.h:131

    FETypeLib TypeLib; // offset 0x4140, size 0x14, Decl: speed/indep/src/feng/FEngine.h:133

    i32 CurrentPackageRecordIndex; // offset 0x4154, size 0x4, Decl: speed/indep/src/feng/FEngine.h:135
    char RecordedPackageNames[16]; // offset 0x4158, size 0x1000, Decl: speed/indep/src/feng/FEngine.h:136

    i32 NextButtonRecordIndex;                     // offset 0x5158, size 0x4, Decl: speed/indep/src/feng/FEngine.h:138
    FEPackageButtonRec RecordedPackageButtons[32]; // offset 0x515C, size 0x100, Decl: speed/indep/src/feng/FEngine.h:139

    bool bErrorScreenMode; // offset 0x525C, size 0x1, Decl: speed/indep/src/feng/FEngine.h:141

    bool bRenderedRecently; // offset 0x5260, size 0x1, Decl: speed/indep/src/feng/FEngine.h:145

    static u32 SysGUID; // size: 0x4, address: 0x8041D07C, Decl: speed/indep/src/feng/FEngine.h:147

    bool bDebugMessages; // offset 0x5264, size 0x1, Decl: speed/indep/src/feng/FEngine.h:157
  public:
    void RenderGroup(FEGroup *pGroup, FEMatrix4 &stParent, FEMatrix4 &stView, unsigned short uContext);
    void RenderObject(FEObject *pObj, FEMatrix4 &stView, unsigned short uContext);
    void ProcessPadsForPackage(FEPackage *pPackage);                                      // Decl: speed/indep/src/feng/FEngine.h:161
    void ProcessMouseForPackage(FEPackage *pPackage);                                     // Decl: speed/indep/src/feng/FEngine.h:162
    void UpdateMouseState(FEPackage *pkg, FEObjectMouseState *state, float mx, float my); // Decl: speed/indep/src/feng/FEngine.h:163
  private:
    void ProcessMessageQueue(); // Decl: speed/indep/src/feng/FEngine.h:165
  public:
    bool ProcessListBoxResponses(FEObject *pObj, u32 MsgID);

    bool ProcessCodeListBoxResponses(FEObject *pObj, u32 MsgID);

    void ProcessObjectMessage(FEObject *pObj, FEPackage *pPack, u32 MsgID, u32 ControlMask);

    void ProcessGlobalMessage(FEPackage *pPack, u32 MsgID, u32 ControlMask);

    void ProcessResponses(FEMessageResponse *pRespList, FEObject *pObj, FEPackage *pPack, u32 ControlMask);

  private:
    FEPackage *FindPackageWithControl(); // Decl: speed/indep/src/feng/FEngine.h:171

    FEPackageCommand *FindQueuedNodeWithControl(); // Decl: speed/indep/src/feng/FEngine.h:172

    void ProcessPackageCommands(); // Decl: speed/indep/src/feng/FEngine.h:173

  public:
    void ToggleErrorScreenMode(bool b) {
        bErrorScreenMode = b;
    }

    bool IsErrorScreenMode() {} // Decl: speed/indep/src/feng/FEngine.h:179

    FEObjectSorter<1024> &GetSorter() { // Decl: speed/indep/src/feng/FEngine.h:182
        return Sorter;
    };

    FEngine(); // Decl: speed/indep/src/feng/FEngine.h:184
    ~FEngine();
    void ReleaseAll(); // Decl: speed/indep/src/feng/FEngine.h:186

    FEPackage *LoadPackage(const void *pPackageData, bool bLoadAsLibrary);
    bool UnloadPackage(FEPackage *pPackage);        // Decl: speed/indep/src/feng/FEngine.h:189
    void UnloadLibraryPackage(FEPackage *pLibPack); // Decl: speed/indep/src/feng/FEngine.h:190

    void SetExecution(bool bProcessEverything);          // Decl: speed/indep/src/feng/FEngine.h:192
    void SetProcessInput(FEPackage *pkg, bool bProcess); // Decl: speed/indep/src/feng/FEngine.h:193

    void SetInitialState(); // Decl: speed/indep/src/feng/FEngine.h:195

    void SetNumJoyPads(unsigned char Count);
    FEJoyPad *GetJoyPad(unsigned char Index) {}
    void SetUseMouse(bool bUseMouse) {} // Decl: speed/indep/src/feng/FEngine.h:199
    FEMouse *GetMouse() {}              // Decl: speed/indep/src/feng/FEngine.h:200

    FETypeLib &GetTypeLib() {} // Decl: speed/indep/src/feng/FEngine.h:202

    void SetLoadObjectNames(bool bLoadNames) {} // Decl: speed/indep/src/feng/FEngine.h:204
    bool GetLoadObjectNames() const {}

    void SetLoadScriptNames(bool bLoadNames) {} // Decl: speed/indep/src/feng/FEngine.h:207
    bool GetLoadScriptNames() const {}

    void SetWrapMode(FEButtonWrapMode NewMode) {} // Decl: speed/indep/src/feng/FEngine.h:210
    FEButtonWrapMode GetWrapMode() {}             // Decl: speed/indep/src/feng/FEngine.h:211

    static u32 GetNextGUID() { // Decl: speed/indep/src/feng/FEngine.h:213
        return SysGUID++;
    }
    static u32 GetSysGUID() { // Decl: speed/indep/src/feng/FEngine.h:214
        return SysGUID;
    }

    static void SetSysGUID(u32 NewGUID) {}

    void SetInterface(FEGameInterface *pNewInterface) { // Decl: speed/indep/src/feng/FEngine.h:218
        pInterface = pNewInterface;
    }

    void FlushRenderCache(); // Decl: speed/indep/src/feng/FEngine.h:220

    void ResetState(); // Decl: speed/indep/src/feng/FEngine.h:223

    void Update(const long tDeltaTicks, unsigned int lock);

    void Render(); // Decl: speed/indep/src/feng/FEngine.h:230

    FEPackage *GetFirstIdle() const;

    void AddToIdleList(FEPackage *pPack);      // Decl: speed/indep/src/feng/FEngine.h:234
    void RemoveFromIdleList(FEPackage *pPack); // Decl: speed/indep/src/feng/FEngine.h:235

    FEPackage *FindIdlePackage(const char *pName) const;

    FEPackage *GetFirstLibrary() const;

    void AddToLibraryList(FEPackage *pPack);      // Decl: speed/indep/src/feng/FEngine.h:241
    void RemoveFromLibraryList(FEPackage *pPack); // Decl: speed/indep/src/feng/FEngine.h:242

    FEPackage *FindLibraryPackage(const char *pFilename) const;

    FEPackage *FindLibraryPackage(u32 NameHash) const;

    FEPackage *FindLibraryPackage(FEPackage *pPack) const;

    FEPackage *PushPackage(const char *pPackageName, const u8 Level, const u32 ControlMask);

    void QueuePackageCommand(int command, u32 ControlMask, const char *pPackageName);

    void QueuePackageSwitch(const char *pPackageName, u32 ControlMask);

    void QueuePackagePush(const char *pPackageName, u32 ControlMask);

    void QueuePackagePop(); // Decl: speed/indep/src/feng/FEngine.h:264

    void QueuePackageUserTransfer(FEPackage *pFrom, bool bToChild, u32 ControlMask);

    void QueueMessage(u32 MsgID, FEObject *pFrom, FEPackage *pFromPackage, FEObject *pTo, u32 ControlMask);

    void SendMessageToGame(u32 MsgID, FEObject *pFrom, FEPackage *pFromPackage, u32 uControlMask);

    FEObject *FindObject(u32 Hash, const char *pPackageName) const;

    FEObject *FindObject(const char *pObjectName, const char *pPackageName) const;

    FEPackageList *GetPackageList() { // Decl: speed/indep/src/feng/FEngine.h:272
        FEPackageList *p = &PackList;
        return p;
    }

    bool ForAllObjects(FEObjectCallback &Callback); // Decl: speed/indep/src/feng/FEngine.h:275

    bool IsPackageBeingPopped(const char *pPackageName);

    int GetNumPackagesBelowPriority(u8 priority);

    bool PackageHasCommands(const char *pPackageName);

    void RecordLastPackageButton(u32 PackageHash, u32 ButtonGUID);

    u32 RecallLastPackageButton(u32 PackageHash);

    void ClearRememberedPackageButtons(); // Decl: speed/indep/src/feng/FEngine.h:284

    bool RecordPackageMarker(const char *pPackageName);

    int GetNumPackageMarkers() const {}

    const char *GetPackageMarker(u32 Index) const {}

    char *const RecallPackageMarker(); // Decl: speed/indep/src/feng/FEngine.h:289

    void ClearPackageMarkers(); // Decl: speed/indep/src/feng/FEngine.h:290

    void DebugMessages(bool bDebug) {} // Decl: speed/indep/src/feng/FEngine.h:295
};

#endif
