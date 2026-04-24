#include "Speed/Indep/Src/FEng/FETypeLib.h"
#include "Speed/Indep/Src/FEng/FETypeNode.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

extern int bSPrintf(char* buf, const char* fmt, ...);

static const char* FEColor1Name = "Top Left";
static const char* FEColor2Name = "Top Right";
static const char* FEColor3Name = "Bottom Right";
static const char* FEColor4Name = "Bottom Left";
static const char* FEFrameNumName = "Frame Number";

FETypeNode* FETypeLib::CreateBaseObjectType(const char* pName) {
    FEVector3 ZeroVect;
    FEVector3 SizeVect;
    FEQuaternion ZeroQuat;
    FEColor White;

    FETypeNode* pType = FENG_NEW FETypeNode();
    pType->SetName(pName);

    pType->AddField("Color", 6);
    pType->AddField("Pivot", 4);
    pType->AddField("Position", 4);
    pType->AddField("Rotation", 5);
    pType->AddField("Size", 4);

    ZeroVect.x = 0.0f;
    ZeroVect.y = 0.0f;
    ZeroVect.z = 0.0f;

    SizeVect.x = 1.0f;
    SizeVect.y = 1.0f;
    SizeVect.z = 1.0f;

    ZeroQuat.x = 0.0f;
    ZeroQuat.y = 0.0f;
    ZeroQuat.z = 0.0f;
    ZeroQuat.w = 1.0f;

    White = FEColor(0xFFFFFFFF);
    FEFieldNode* pField = pType->GetFirstField();
    pField->SetDefault(&White);
    pField = pField->GetNext();
    pField->SetDefault(&ZeroVect);
    pField = pField->GetNext();
    pField->SetDefault(&ZeroVect);
    pField = pField->GetNext();
    pField->SetDefault(&ZeroQuat);
    pField = pField->GetNext();
    pField->SetDefault(&SizeVect);

    return pType;
}

FETypeNode* FETypeLib::CreateImageObjectType(const char* pName) {
    FEVector2 ZeroVect;
    FEVector2 OneVect;

    FETypeNode* pType = CreateBaseObjectType(pName);

    pType->AddField("Upper Left", 3);
    pType->AddField("Lower Right", 3);

    ZeroVect = FEVector2(0.0f, 0.0f);
    FEFieldNode* pField = pType->GetField("Upper Left");
    pField->SetDefault(&ZeroVect);
    OneVect = FEVector2(1.0f, 1.0f);
    pField->GetNext()->SetDefault(&OneVect);

    return pType;
}

FETypeNode* FETypeLib::CreateMultiImageObjectType(const char* pName) {
    FETypeNode* pType = CreateImageObjectType(pName);
    char sztemp[32];
    FEVector3 pivot_rot(0.0f, 0.0f, 0.0f);
    FEVector2 top_left(0.0f, 0.0f);
    FEVector2 bottom_right(1.0f, 1.0f);

    for (int i = 1; i < 4; i++) {
        bSPrintf(sztemp, "Tex %d: Top Left", i);
        pType->AddField(sztemp, 3);
        FEFieldNode* pField = pType->GetField(sztemp);
        pField->SetDefault(&top_left);
    }
    for (int i = 1; i < 4; i++) {
        bSPrintf(sztemp, "Tex %d: Bottom Right", i);
        pType->AddField(sztemp, 3);
        FEFieldNode* pField = pType->GetField(sztemp);
        pField->SetDefault(&bottom_right);
    }

    bSPrintf(sztemp, "Pivot Rot (Z)");
    pType->AddField(sztemp, 4);
    FEFieldNode* pField = pType->GetField(sztemp);
    pField->SetDefault(&pivot_rot);

    return pType;
}

bool FETypeLib::Startup() {
    FETypeNode* pType;
    FEColor White;
    int DefaultFrame;

    pType = CreateImageObjectType("Image");
    pType->SetID(1);
    AddType(pType);

    pType = CreateImageObjectType("CBV Image");
    pType->AddField(FEColor1Name, 6);
    pType->AddField(FEColor2Name, 6);
    pType->AddField(FEColor3Name, 6);
    pType->AddField(FEColor4Name, 6);
    White = FEColor(0xFFFFFFFF);
    FEFieldNode* pField;
    pField = pType->GetField(FEColor1Name);
    pField->SetDefault(&White);
    pField = pType->GetField(FEColor2Name);
    pField->SetDefault(&White);
    pField = pType->GetField(FEColor3Name);
    pField->SetDefault(&White);
    pField = pType->GetField(FEColor4Name);
    pField->SetDefault(&White);
    pType->SetID(9);
    AddType(pType);

    pType = CreateImageObjectType("Anim Image");
    pType->AddField(FEFrameNumName, 1);
    DefaultFrame = 0;
    pField = pType->GetField(FEFrameNumName);
    pField->SetDefault(&DefaultFrame);
    pType->SetID(10);
    AddType(pType);

    pType = CreateBaseObjectType("Simple Image");
    pType->SetID(11);
    AddType(pType);

    pType = CreateMultiImageObjectType("MultiImage");
    pType->SetID(12);
    AddType(pType);

    pType = CreateBaseObjectType("String");
    pType->SetID(2);
    AddType(pType);

    pType = CreateBaseObjectType("Model");
    pType->SetID(3);
    AddType(pType);

    pType = CreateBaseObjectType("Movie");
    pType->SetID(7);
    AddType(pType);

    pType = CreateBaseObjectType("Effect");
    pType->SetID(8);
    AddType(pType);

    pType = CreateBaseObjectType("List");
    pType->SetID(4);
    AddType(pType);

    pType = CreateBaseObjectType("Group");
    pType->SetID(5);
    AddType(pType);

    pType = CreateBaseObjectType("Code List");
    pType->SetID(6);
    AddType(pType);

    return true;
}

FETypeNode* FETypeLib::FindType(unsigned long TypeID) {
    FETypeNode* pNode = GetFirstType();
    for (;;) {
        if (!pNode) {
            break;
        }
        if (pNode->GetID() == TypeID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}
