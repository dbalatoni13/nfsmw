#include "uiRapSheetTEP.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
void FEngSetInvisible(FEObject* obj);
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetLastButton(const char* pkg_name, unsigned char button);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
void FEngSetButtonState(const char* pkg_name, unsigned int button_hash, bool enabled);
unsigned int GetFECarNameHashFromFEKey(unsigned int fekey);
uiRapSheetTEP::uiRapSheetTEP(ScreenConstructorData* sd) : UIWidgetMenu(sd) , button_pressed(0) , num_pursuits(0) { Setup(); }
uiRapSheetTEP::~uiRapSheetTEP() {}
void uiRapSheetTEP::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x0C407210:
        button_pressed = pobj->NameHash;
        break;
    case 0x406415E3:
        if (num_pursuits == 0) { return; }
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
        break;
    case 0x72619778:
        if (pobj == nullptr) { return; }
        if (pobj->NameHash != 0xCDA0A66B) { return; }
        FEngSetCurrentButton(GetPackageName(), FEngHashString("BL_%d", num_pursuits));
        break;
    case 0x911C0A4B:
        if (pobj == nullptr) { return; }
        if (pobj->NameHash != static_cast<unsigned long>(FEngHashString("BL_%d", num_pursuits))) { return; }
        FEngSetCurrentButton(GetPackageName(), 0xCDA0A66B);
        break;
    case 0x35F8620B:
        if (num_pursuits == 0) { return; }
        {
            unsigned char button = FEngGetLastButton(GetPackageName());
            if (button == 0) { button = 1; }
            FEngSetCurrentButton(GetPackageName(), FEngHashString("BL_%d", button));
        }
        break;
    case 0xE1FDE1D1: {
        int index;
        switch (button_pressed) {
        case 0xCDA0A66B: index = 0; break;
        case 0xCDA0A66C: index = 1; break;
        case 0xCDA0A66D: index = 2; break;
        case 0xCDA0A66E: index = 3; break;
        case 0xCDA0A66F: index = 4; break;
        default: index = -1; break;
        }
        if (index != -1) { cFEng::Get()->QueuePackageSwitch("RapSheetPD.fng", index, 0, false); FEngSetLastButton(GetPackageName(), static_cast<unsigned char>(index + 1)); }
        else { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); FEngSetLastButton(GetPackageName(), 1); }
        break;
    }
    }
}
void uiRapSheetTEP::Setup() {
    UserProfile& prof = *FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    HighScoresDatabase* scores = prof.GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof.GetProfileName());
    FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    for (int i = 0; i < 5; i++) {
        const TopEvadedPursuitDetail& pursuit = scores->GetTopEvadedPursuitScores(static_cast<unsigned short>(i));
        if (pursuit.Length != 0) {
            char time_str[16];
            Timer t(pursuit.Length);
            t.PrintToString(time_str, 0);
            int index = i + 1;
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_CAR_%d", index), GetLocalizedString(0x69EAB50F), GetLocalizedString(GetFECarNameHashFromFEKey(pursuit.CarFEKey)));
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", index), GetLocalizedString(0x060C058A), pursuit.Bounty);
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", index), GetLocalizedString(0x41474FB1), pursuit.PursuitName);
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE3_%d", index), GetLocalizedString(0x36175146), time_str);
            num_pursuits++;
        } else {
            int index = i + 1;
            FEngSetButtonState(GetPackageName(), FEngHashString("BL_%d", index), false);
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_CAR_%d", index), GetLocalizedString(0xE3274304));
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", index), "");
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", index), "");
            FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE3_%d", index), "");
        }
    }
    if (num_pursuits == 0) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xEB5E7757));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x73DCB662));
    }
}
