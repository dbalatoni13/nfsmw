#ifndef FENG_FEPACKAGE_H
#define FENG_FEPACKAGE_H

#include "FEObject.h"

struct FEObjectCallback;
struct FEGroup;
struct FEngine;
struct FEGameInterface;
struct FEResourceRequest;
struct FEMsgTargetList;
struct FELibraryRef;
struct FEObjectMouseState;
struct FEMessageResponse;
struct FEPackageRenderInfo;

// total size: 0x18
struct FEResourceRequest {
    unsigned long ID;             // offset 0x0, size 0x4
    const char* pFilename;        // offset 0x4, size 0x4
    unsigned long Type;           // offset 0x8, size 0x4
    unsigned long Flags;          // offset 0xC, size 0x4
    unsigned long Handle;         // offset 0x10, size 0x4
    unsigned long UserParam;      // offset 0x14, size 0x4
};

// total size: 0x40
struct FEMatrix4 {
    float m11; // offset 0x0
    float m12; // offset 0x4
    float m13; // offset 0x8
    float m14; // offset 0xC
    float m21; // offset 0x10
    float m22; // offset 0x14
    float m23; // offset 0x18
    float m24; // offset 0x1C
    float m31; // offset 0x20
    float m32; // offset 0x24
    float m33; // offset 0x28
    float m34; // offset 0x2C
    float m41; // offset 0x30
    float m42; // offset 0x34
    float m43; // offset 0x38
    float m44; // offset 0x3C

    inline FEMatrix4& operator=(const FEMatrix4& m) {
        m11 = m.m11; m12 = m.m12; m13 = m.m13; m14 = m.m14;
        m21 = m.m21; m22 = m.m22; m23 = m.m23; m24 = m.m24;
        m31 = m.m31; m32 = m.m32; m33 = m.m33; m34 = m.m34;
        m41 = m.m41; m42 = m.m42; m43 = m.m43; m44 = m.m44;
        return *this;
    }

    void Identify();
};

// total size: 0x14
struct FENode : public FEMinNode {
    char* name;            // offset 0xC, size 0x4
    unsigned int nameHash; // offset 0x10, size 0x4

    FENode();
    ~FENode() override;
    bool SetName(const char* theName);

    inline const char* GetName() const { return name; }
    inline const unsigned int GetNameHash() const { return nameHash; }
    inline FENode* GetNext() const { return static_cast<FENode*>(FEMinNode::GetNext()); }
    inline FENode* GetPrev() const { return static_cast<FENode*>(FEMinNode::GetPrev()); }
};

// total size: 0x10
struct FEList : public FEMinList {
    FENode* FindNode(const char* pName, FENode* node) const;
    FENode* FindNode(const char* pName) const;
};

// total size: 0x8
struct FEButtonMap {
    FEObject** pList;    // offset 0x0, size 0x4
    unsigned long Count; // offset 0x4, size 0x4

    inline FEButtonMap() : pList(nullptr), Count(0) {}
    inline ~FEButtonMap() {}
    inline unsigned long GetCount() { return Count; }
    inline void SetButton(unsigned long Index, FEObject* pButton) { pList[Index] = pButton; }
    inline FEObject* GetButton(unsigned long Index) { return pList[Index]; }

    void SetCount(unsigned long NewCount);
};

// total size: 0xC4
struct FEPackage : public FENode {
    static unsigned long uHoldDirtyFlags;

    bool bExecuting;                      // offset 0x14, size 0x1
    bool bUseIdleList;                    // offset 0x18, size 0x1
    bool bIsLibrary;                      // offset 0x1C, size 0x1
    bool bStartEqualsAccept;              // offset 0x20, size 0x1
    bool bErrorScreen;                    // offset 0x24, size 0x1
    int Priority;                         // offset 0x28, size 0x4
    unsigned long Controllers;            // offset 0x2C, size 0x4
    unsigned long OldControllers;         // offset 0x30, size 0x4
    bool bInputEnabled;                   // offset 0x34, size 0x1
    unsigned long VersionNumber;          // offset 0x38, size 0x4
    FEngine* pEnginePtr;                  // offset 0x3C, size 0x4
    int iTickIncrement;                   // offset 0x40, size 0x4
    char* pFilename;                      // offset 0x44, size 0x4
    FEPackage* pParentPackage;            // offset 0x48, size 0x4
    unsigned long UserParam;              // offset 0x4C, size 0x4
    FEMinList Objects;                    // offset 0x50, size 0x10
    FEMinList Responses;                  // offset 0x60, size 0x10
    unsigned long NumRequests;            // offset 0x70, size 0x4
    FEResourceRequest* pRequests;         // offset 0x74, size 0x4
    unsigned long NumMsgTargets;          // offset 0x78, size 0x4
    FEMsgTargetList* pMsgTargets;         // offset 0x7C, size 0x4
    FEList LibrariesUsed;                 // offset 0x80, size 0x10
    unsigned long NumLibRefs;             // offset 0x90, size 0x4
    FELibraryRef* pLibRefs;              // offset 0x94, size 0x4
    FEObject* pCurrentButton;            // offset 0x98, size 0x4
    FEButtonMap ButtonMap;               // offset 0x9C, size 0x8
    char* pResourceNames;                // offset 0xA4, size 0x4
    FEMinList Comments;                  // offset 0xA8, size 0x10
    FEObjectMouseState* MouseObjectStates; // offset 0xB8, size 0x4
    int NumMouseObjects;                 // offset 0xBC, size 0x4
    int NumMouseObjectsCounter;          // offset 0xC0, size 0x4

    FEPackage();
    ~FEPackage() override;

    bool InitializePackage();
    void Update(FEngine* pEngine, long tDeltaTicks);

    inline FEObject* GetCurrentButton() { return pCurrentButton; }
    inline FEButtonMap* GetButtonMap() { return &ButtonMap; }
    inline FEObject* GetFirstObject() { return static_cast<FEObject*>(Objects.GetHead()); }
    inline FEPackage* GetNext() { return static_cast<FEPackage*>(FENode::GetNext()); }
    inline FEPackage* GetPrev() { return static_cast<FEPackage*>(FENode::GetPrev()); }
    inline unsigned long GetControlMask() const { return Controllers; }
    inline void SetErrorScreen(bool b) { bErrorScreen = b; }

    FEObject* FindObjectByHash(unsigned long NameHash);
    void SetCurrentButton(FEObject* pNewButton, bool bSendMsgs);
    bool ForAllChildren(FEGroup* pGroup, FEObjectCallback& Callback);
    bool ForAllObjects(FEObjectCallback& Callback);
};

#endif
