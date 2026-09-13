#ifndef FEMESSAGERESPONSE_H_
#define FEMESSAGERESPONSE_H_

#include "FEList.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

// total size: 0xC
// Decl: 91
class FEResponse {
  public:
    u32 ResponseID;     // offset 0x0, size 0x4, Decl: 93
    u32 ResponseParam;  // offset 0x4, size 0x4, Decl: 94
    u32 ResponseTarget; // offset 0x8, size 0x4, Decl: 95

    FEResponse() : ResponseID(0), ResponseParam(0), ResponseTarget(0) {} // Decl: 97
    ~FEResponse();

    void ReleaseParam(); // Decl: 100

    FEResponse &operator=(FEResponse &rhs); // Decl: 102

    static bool HasString(u32 ResponseID) {
        return (ResponseID - 0x200u < 5) && (ResponseID != 0x203);
    }
    static bool SpawnsPackage(u32 ResponseID) {}
    static bool PostsMessage(u32 ResponseID) {}
    static bool IsConditional(u32 ResponseID) {}

    bool HasString() const {
        return HasString(ResponseID);
    }
    bool SpawnsPackage() const {}
    void SetID(u32 ID) {
        ResponseID = ID;
    }
    void SetParam(u32 Value) {
        ResponseParam = Value;
    }
    void SetParam(const char *pString);
};

enum FEMessageResponseCommands {
    MR_SetScript = 0,
    MR_PostMessageToFEng = 1,
    MR_PostMessageToGame = 2,
    MR_PostMessageToSound = 3,
    MR_SetActiveButton = 256,
    MR_SetInputProcessing = 257,
    MR_RecordCurrentButton = 258,
    MR_RecallRecordedButton = 259,
    MR_DontNavigate = 260,
    MR_PassControlToChildCurrent = 261,
    MR_PassControlToChildGlobal = 262,
    MR_PassControlToParentCurrent = 263,
    MR_PassControlToParentGlobal = 264,
    MR_SwitchToPackage = 512,
    MR_PushPackageGlobal = 513,
    MR_PushPackageCurrent = 514,
    MR_PopPackage = 515,
    MR_PushPackageNone = 516,
    MR_RecordPackageMarker = 704,
    MR_SwitchToPackageMarker = 705,
    MR_ClearPackageMarkers = 706,
    MR_IfScriptEquals = 768,
    MR_IfScriptNotEquals = 769,
    MR_Else = 1280,
    MR_EndIf = 1281,
};

// total size: 0x18
// Decl: 131
class FEMessageResponse : public FEMinNode {
  public:
    u32 MsgID;                 // offset 0xC, size 0x4, Decl: 133
    u32 Count;                 // offset 0x10, size 0x4, Decl: 134
    FEResponse *pResponseList; // offset 0x14, size 0x4, Decl: 135

  private:
    static ObjectPool<FEMessageResponse, 64> NodePool; // size: 0x10, address: 0x80473D48, Decl: 59

    void Init() { // Decl: 139
        MsgID = 0;
        Count = 0;
        pResponseList = nullptr;
    }

  public:
    FEMessageResponse() : FEMinNode() { // Decl: 142
        Init();
    }
    ~FEMessageResponse() override;

    static void *operator new(size_t size, int) {}

    static void *operator new(size_t);

    static void operator delete(void *pNode, int) {}

    static void operator delete(void *pNode);

    FEMessageResponse &operator=(FEMessageResponse &Src);

    void PurgeResponses();

    u32 GetMsgID() const {
        return MsgID;
    }

    void SetMsgID(u32 NewID) {
        MsgID = NewID;
    }

    u32 GetCount() const {
        return Count;
    }

    FEResponse *GetResponse(int Index) const {
        return &pResponseList[Index];
    }

    bool ResponseHasString(int Index) const {}

    void SetCount(u32 NewCount);

    u32 AddResponse(u32 ResponseID, u32 Param);

    u32 AddResponse(u32 ResponseID, const char *pPackagePath);

    void DeleteResponse(u32 Index);

    u32 FindResponse(u32 CommandID) const;

    u32 FindConditionBranchTarget(u32 Index) const;

    FEMessageResponse *GetNext() { // Decl: 183
        return static_cast<FEMessageResponse *>(FEMinNode::GetNext());
    }

    FEMessageResponse *GetPrev() { // Decl: 184
        return static_cast<FEMessageResponse *>(FEMinNode::GetPrev());
    }
};

#endif
