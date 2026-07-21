#include "WebOfferUG2.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"

void *operator new(size_t size, const char *file, int line, int allocationParams);
void *operator new[](size_t size, const char *file, int line, int allocationParams);

FEString *FEngFindString(const char *pkg_name, int name_hash);
void FEngSetVisible(FEObject *obj);
void FEngSetButtonState(const char *pkg_name, uint32 button_hash, bool enabled);
void FEngSetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash,
                   bool start_at_beginning);
void FEngSetCurrentButton(const char *pkg_name, uint32 hash);
FEngFont *FindFont(uint32 font_hash);
bool ConvertUTF8ToUCS2(uint16 *ucs2_data, int ucs2_data_len, uint8 *utf8_data);
char *OLGetProductName();
char *OLGetPlatform();
extern char LobbyLKEY[32];

namespace BuildRegion {
char *GetSlusCode();
}

namespace ISOCodes {
char *GetLanguageISOCode();
char *GetCountryISOCode();
}

extern "C" WebOfferT *WebOfferCreate(uint32 maxScriptSize);
extern "C" void WebOfferDestroy(WebOfferT *webOffer);
extern "C" void WebOfferSetup(WebOfferT *webOffer, WebOfferSetupT *setup);
extern "C" void WebOfferExecute(WebOfferT *webOffer, const char *offerURL);
extern "C" WebOfferCommandT *WebOfferCommand(WebOfferT *webOffer);
extern "C" void WebOfferGetAlert(WebOfferT *webOffer, WebOfferAlertT *alertData);
extern "C" void WebOfferGetPromo(WebOfferT *webOffer, WebOfferPromoT *promoData);
extern "C" void WebOfferGetCredit(WebOfferT *webOffer, WebOfferCreditT *creditData);
extern "C" void WebOfferGetBusy(WebOfferT *webOffer, WebOfferBusyT *busyData);
extern "C" void WebOfferHttp(WebOfferT *webOffer);
extern "C" char *WebOfferGetNews(WebOfferT *webOffer, WebOfferNewsT *newsData);
extern "C" void WebOfferAction(WebOfferT *webOffer, int action);
extern "C" void WebOfferSetPromo(WebOfferT *webOffer, WebOfferPromoT *promoData);
extern "C" void WebOfferSetCredit(WebOfferT *webOffer, WebOfferCreditT *creditData);
extern "C" int WebOfferHttpComplete(WebOfferT *webOffer);
extern "C" int WebOfferResultData(WebOfferT *webOffer, void *resultData, void *resultSize);
extern "C" char *TagFieldFind(const char *record, const char *name);
extern "C" int TagFieldGetString(const char *field, char *buffer, int bufferSize, const char *defaultValue);

CWebOffer::CWebOffer()
    : m_pWebOfferAPI(nullptr) //
    , m_pCurrentCommand(nullptr) //
    , m_bProcessingCommand(false) {}

CWebOffer::~CWebOffer() { Shutdown(); }

bool CWebOffer::Initialise(uint32 MaxScriptSize) {
    m_pWebOfferAPI = WebOfferCreate(MaxScriptSize);
    return m_pWebOfferAPI != nullptr;
}

void CWebOffer::Shutdown() {
    if (m_pWebOfferAPI) {
        WebOfferDestroy(m_pWebOfferAPI);
        m_pWebOfferAPI = nullptr;
    }
}

bool CWebOffer::Start(SOfferData &OfferData) {
    bool bStarted = false;
    if (m_pWebOfferAPI) {
        bStrNCpy(m_WebOfferSetup.strPersona, OfferData.pPersonaName, 19);
        bStrNCpy(m_WebOfferSetup.strProduct, OfferData.pProductName, 31);
        bStrNCpy(m_WebOfferSetup.strPlatform, OfferData.pPlatformName, 31);
        bStrNCpy(m_WebOfferSetup.strLanguage, OfferData.pLanguage, 3);
        bStrNCpy(m_WebOfferSetup.strCountry, OfferData.pCountry, 3);
        bStrNCpy(m_WebOfferSetup.strSlusCode, OfferData.pSLUSCode, 31);
        bStrNCpy(m_WebOfferSetup.strLKey, OfferData.pLobbyKey, 39);
        m_WebOfferSetup.strFavTeam[0] = '\0';
        WebOfferSetup(m_pWebOfferAPI, &m_WebOfferSetup);
        WebOfferExecute(m_pWebOfferAPI, OfferData.pOfferURL);
        m_bProcessingCommand = false;
        bStarted = true;
    }
    return bStarted;
}

void CWebOffer::Tick() {
    if (m_pWebOfferAPI) {
        if (m_bProcessingCommand) {
            ProcessCommand();
        } else {
            StartNextCommand();
        }
    }
}

void CWebOffer::StartNextCommand() {
    m_pCurrentCommand = WebOfferCommand(m_pWebOfferAPI);
    if (m_pCurrentCommand) {
        switch (m_pCurrentCommand->iCommand) {
        case 'alrt':
            _StartAlert();
            break;
        case 'prom':
            _StartPromo();
            break;
        case 'card':
            _StartCredit();
            break;
        case 'http':
            _StartHTTP();
            break;
        case 'news':
            _StartNews();
            break;
        }
    } else {
        _Finished();
    }
}

void CWebOffer::_StartAlert() {
    WebOfferAlertT AlertData;
    WebOfferGetAlert(m_pWebOfferAPI, &AlertData);
    StartAlert(AlertData);
    m_bProcessingCommand = true;
}

void CWebOffer::_StartPromo() {
    WebOfferPromoT PromoData;
    WebOfferGetPromo(m_pWebOfferAPI, &PromoData);
    StartPromo(PromoData);
    m_bProcessingCommand = true;
}

void CWebOffer::_StartCredit() {
    WebOfferCreditT CreditData;
    WebOfferGetCredit(m_pWebOfferAPI, &CreditData);
    StartCredit(CreditData);
    m_bProcessingCommand = true;
}

void CWebOffer::_StartHTTP() {
    WebOfferBusyT BusyData;
    WebOfferGetBusy(m_pWebOfferAPI, &BusyData);
    StartHTTP(BusyData);
    WebOfferHttp(m_pWebOfferAPI);
    m_bProcessingCommand = true;
}

void CWebOffer::_StartNews() {
    WebOfferNewsT NewsData;
    char *pNewsText = WebOfferGetNews(m_pWebOfferAPI, &NewsData);
    StartNews(pNewsText, NewsData);
    m_bProcessingCommand = true;
}

void CWebOffer::ProcessCommand() {
    switch (m_pCurrentCommand->iCommand) {
    case 'alrt':
        _ProcessAlert();
        break;
    case 'prom':
        _ProcessPromo();
        break;
    case 'card':
        _ProcessCredit();
        break;
    case 'http':
        _ProcessHTTP();
        break;
    case 'news':
        _ProcessNews();
        break;
    }
}

void CWebOffer::_ProcessAlert() {
    int Action = ProcessAlert();
    if (Action < eProcessAction_Nothing) {
        WebOfferAction(m_pWebOfferAPI, Action);
        m_bProcessingCommand = false;
        EndAlert();
    }
}

void CWebOffer::_ProcessPromo() {
    int Action = ProcessPromo();
    if (Action < eProcessAction_Nothing) {
        bool bSubmit = false;
        WebOfferPromoT PromoData;
        WebOfferGetPromo(m_pWebOfferAPI, &PromoData);
        if (Action == eProcessAction_Button1 && PromoData.Button[0].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button2 && PromoData.Button[1].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button3 && PromoData.Button[2].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button4 && PromoData.Button[3].strType[0] == '^') {
            bSubmit = true;
        }
        if (bSubmit && FillInPromoSubmitData(PromoData.strPromo, sizeof(PromoData.strPromo))) {
            WebOfferSetPromo(m_pWebOfferAPI, &PromoData);
        }
        WebOfferAction(m_pWebOfferAPI, Action);
        m_bProcessingCommand = false;
        EndPromo();
    }
}

void CWebOffer::_ProcessCredit() {
    int Action = ProcessCredit();
    if (Action < eProcessAction_Nothing) {
        bool bSubmit = false;
        WebOfferCreditT CreditData;
        WebOfferGetCredit(m_pWebOfferAPI, &CreditData);
        if (Action == eProcessAction_Button1 && CreditData.Button[0].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button2 && CreditData.Button[1].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button3 && CreditData.Button[2].strType[0] == '^') {
            bSubmit = true;
        }
        if (Action == eProcessAction_Button4 && CreditData.Button[3].strType[0] == '^') {
            bSubmit = true;
        }
        if (bSubmit && FillInCreditCardSubmitData(CreditData)) {
            WebOfferSetCredit(m_pWebOfferAPI, &CreditData);
        }
        WebOfferAction(m_pWebOfferAPI, Action);
        m_bProcessingCommand = false;
        EndCredit();
    }
}

void CWebOffer::_ProcessHTTP() {
    int Action = ProcessHTTP();
    bool bComplete = WebOfferHttpComplete(m_pWebOfferAPI);
    bool bAction = Action < eProcessAction_Nothing;
    if (bAction) {
        WebOfferAction(m_pWebOfferAPI, Action);
    }
    if (bComplete || bAction) {
        m_bProcessingCommand = false;
        EndHTTP();
    }
}

void CWebOffer::_ProcessNews() {
    int Action = ProcessNews();
    if (Action < eProcessAction_Nothing) {
        WebOfferAction(m_pWebOfferAPI, Action);
        m_bProcessingCommand = false;
        EndNews();
    }
}

void CWebOffer::_Finished() {
    int ResultCode = WebOfferResultData(m_pWebOfferAPI, nullptr, nullptr);
    Finished(ResultCode);
}

char CUIWebOfferStart::m_WebOfferScript[512];

void ConfigureWebOfferForTOS() {
    char *config = LobbyCore::Instance().GetServerConfig();
    char TOSURLFromServerConfig[256];
    TOSURLFromServerConfig[0] = '\0';
    if (config) {
        TagFieldGetString(TagFieldFind(config, "TOSAC_URL"), TOSURLFromServerConfig, 255, "");
    }

    char *pURL = TOSURLFromServerConfig;
    char *pURLScriptFormat =
        "%%{ CMD=http\nTIME=%d\nMESG=0x%x\nURL-GET=%s\nSUCCESS-GOTO=$http\nBTN1-GOTO=$exit=-1\n%%}\n"
        "%%{FAILURE CMD=alrt\nTITLE=0x%x\nMESG=0x%x\nBTN1=0x%x\nBTN1-GOTO=$exit=%d\n%%}";
    if (bStrLen(pURL) == 0) {
        char FudgedURL[32];
        memcpy(FudgedURL, "http://1mdi71", 14);
        pURL = FudgedURL;
    }

    int TotalStrLen = bStrLen(pURLScriptFormat) + 36;
    if (TotalStrLen < 0x200) {
        bSPrintf(CUIWebOfferStart::m_WebOfferScript, pURLScriptFormat, 30, 0x780207ca, pURL, 0,
                 0x9016a670, 0x417b2601, -1);
    }
}

MenuScreen *CreateOnlineNews(ScreenConstructorData *sd) {
    CUIWebOfferNews *pScreen = new ("UIOnlineNews", 0) CUIWebOfferNews(sd);
    SFEngNewsScreenData *pNewsData = reinterpret_cast<SFEngNewsScreenData *>(sd->Arg);
    int TextSize = 0;

    for (int n = 0; pNewsData->WebOfferNews.strTitle[n]; ++n) {
        char c = pNewsData->WebOfferNews.strTitle[n];
        if (static_cast<unsigned int>(c - 'A') < 26) {
            c |= 0x20;
        }
        pNewsData->WebOfferNews.strTitle[n] = c;
    }

    pScreen->m_pOwner = pNewsData->pParentScreen;
    pNewsData->pParentScreen->NotificationMessage(0xb19b00b5, nullptr,
                                                   reinterpret_cast<uint32>(pScreen), 0);

    SButtonInit Buttons[2];
    SButtonInit *pButtonInit = Buttons;
    int NumButtons = 0;
    for (int n = 0; n < 2; ++n) {
        if (pNewsData->WebOfferNews.Button[n].strText[0]) {
            pButtonInit->pButtonText = pNewsData->WebOfferNews.Button[n].strText;
            ++NumButtons;
            ++pButtonInit;
        }
    }
    pScreen->m_NumButtons = NumButtons;

    char FEngName[32];
    for (int FEngButtonCount = 1; FEngButtonCount <= 3; ++FEngButtonCount) {
        bSPrintf(FEngName, "Button%d", FEngButtonCount);
        FEObject *pFEObject = FEngFindObject(pScreen->GetPackageName(), FEHashUpper(FEngName));
        FEngSetInvisible(pFEObject);
        FEngSetButtonState(pScreen->GetPackageName(), pFEObject->GetNameHash(), false);
        bSPrintf(FEngName, "Button%d_Text", FEngButtonCount);
        pFEObject = FEngFindObject(pScreen->GetPackageName(), FEHashUpper(FEngName));
        FEngSetInvisible(pFEObject);
    }

    if (NumButtons == 1) {
        pScreen->m_Buttons[0].Action = eProcessAction_Button1;
        bSPrintf(FEngName, "Button%d", 1);
        pScreen->m_Buttons[0].NameHash = FEHashUpper(FEngName);
        FEObject *pFEObject =
            FEngFindObject(pScreen->GetPackageName(), pScreen->m_Buttons[0].NameHash);
        FEngSetVisible(pFEObject);
        FEngSetButtonState(pScreen->GetPackageName(), pScreen->m_Buttons[0].NameHash, true);
        FEngSetScript(pScreen->GetPackageName(), pScreen->m_Buttons[0].NameHash, 0x249db7b7,
                      true);
        bSPrintf(FEngName, "Button%d_Text", 1);
        FEString *pFEString =
            FEngFindString(pScreen->GetPackageName(), FEHashUpper(FEngName));
        pFEObject = FEngFindObject(pScreen->GetPackageName(), FEHashUpper(FEngName));
        pFEString->SetStringFromUTF8(Buttons[0].pButtonText);
        FEngSetVisible(pFEObject);
        FEngSetScript(pScreen->GetPackageName(), FEHashUpper(FEngName), 0x249db7b7, true);
        FEngSetCurrentButton(pScreen->GetPackageName(), pScreen->m_Buttons[0].NameHash);
        FEngSetInvisible(pScreen->GetPackageName(), 0x7c64b811);
    } else if (NumButtons == 2) {
        for (int i = 0; i < 2; ++i) {
            pScreen->m_Buttons[i].Action = static_cast<EProcessAction>(i + 1);
            bSPrintf(FEngName, "Button%d", i + 1);
            pScreen->m_Buttons[i].NameHash = FEHashUpper(FEngName);
            FEObject *pFEObject =
                FEngFindObject(pScreen->GetPackageName(), pScreen->m_Buttons[i].NameHash);
            FEngSetVisible(pFEObject);
            FEngSetButtonState(pScreen->GetPackageName(), pScreen->m_Buttons[i].NameHash, true);
            FEngSetScript(pScreen->GetPackageName(), pScreen->m_Buttons[i].NameHash,
                          i == 0 ? 0x249db7b7 : 0x7ab5521a, true);
            bSPrintf(FEngName, "Button%d_Text", i + 1);
            pFEObject = FEngFindObject(pScreen->GetPackageName(), FEHashUpper(FEngName));
            FEString *pFEString =
                FEngFindString(pScreen->GetPackageName(), FEHashUpper(FEngName));
            pFEString->SetStringFromUTF8(Buttons[i].pButtonText);
            FEngSetVisible(pFEObject);
            FEngSetScript(pScreen->GetPackageName(), FEHashUpper(FEngName),
                          i == 0 ? 0x249db7b7 : 0x7ab5521a, true);
        }
        FEngSetCurrentButton(pScreen->GetPackageName(), pScreen->m_Buttons[0].NameHash);
    }

    FEString *pString =
        FEngFindString(pScreen->GetPackageName(), FEHashUpper("Header_text_shadow"));
    pString->SetStringFromUTF8(pNewsData->WebOfferNews.strTitle);
    pString = FEngFindString(pScreen->GetPackageName(), FEHashUpper("Header_text_inner"));
    pString->SetStringFromUTF8(pNewsData->WebOfferNews.strTitle);
    FEngSetScript(pScreen->GetPackageName(), FEHashUpper("Help"), 0x16a259, true);
    FEngSetScript(pScreen->GetPackageName(), 0x3df39a82, 0x16a259, true);
    FEngSetScript(pScreen->GetPackageName(), 0xfcd5b255, 0x16a259, true);

    FEString *pFERankModeString =
        FEngFindString(pScreen->GetPackageName(), FEHashUpper("RankMode_Type"));
    pFERankModeString->string = const_cast<char *>("");
    pFERankModeString->SetFlags(pFERankModeString->GetFlags() | 0x400000);

    pString = FEngFindString(pScreen->GetPackageName(), FEHashUpper("Scroll_Text_1"));
    FEngFont *pFont = FindFont(pString->GetHandle());
    pScreen->m_TextScroller.Initialise(pScreen, pString->MaxWidth, 11,
                                       const_cast<char *>("Scroll_Text_%d"), pFont);
    pScreen->m_TextScroller.UseScrollBar(&pScreen->m_ScrollBar);

    if (pNewsData->pNewsText) {
        TextSize = bStrLen(pNewsData->pNewsText) + 1;
    }
    if (TextSize < 1) {
        pScreen->m_TextScroller.SetText(nullptr);
    } else {
        int16 *pText = new ("UIWebOfferNews(UTF8toUCS)", 0, 0x40) int16[TextSize];
        ConvertUTF8ToUCS2(reinterpret_cast<uint16 *>(pText), TextSize,
                          reinterpret_cast<uint8 *>(pNewsData->pNewsText));
        pScreen->m_TextScroller.SetText(pText);
        if (pText) {
            delete[] pText;
        }
    }

    return pScreen;
}

inline MenuScreen *CreateOnlineWebOfferScreen(ScreenConstructorData *sd) {
    CUIWebOfferStart *pScreen = new ("UIOnlineWebOffer", 0) CUIWebOfferStart(sd);
    CWebOffer::SOfferData OfferData;
    OfferData.pPersonaName = FEDatabase->OnlineSettings.GetLobbyPersona();
    OfferData.pProductName = OLGetProductName();
    OfferData.pPlatformName = OLGetPlatform();
    OfferData.pLanguage = ISOCodes::GetLanguageISOCode();
    OfferData.pCountry = ISOCodes::GetCountryISOCode();
    OfferData.pSLUSCode = BuildRegion::GetSlusCode();
    OfferData.pLobbyKey = LobbyLKEY;
    OfferData.pOfferURL = CUIWebOfferStart::m_WebOfferScript;
    pScreen->m_WebOffer.Initialise(0x4000);
    pScreen->m_WebOffer.Start(OfferData);
    return pScreen;
}

void CWebOfferUG2::DismissDialog() {
    if (m_Dialog) {
        DialogInterface::DismissDialog(m_Dialog);
        m_Dialog = 0;
    }
}

void CWebOfferUG2::StartAlert(const WebOfferAlertT &AlertData) {
    m_bAlertDialogPopulated = false;
    m_AlertData = AlertData;

    int NumButtons = 0;
    int ButtonAction = 1;
    int ButtonActionHashes[4];
    if (m_Dialog) {
        DialogInterface::DismissDialog(m_Dialog);
    }
    for (int ButtonNumber = 0; ButtonNumber < 4; ++ButtonNumber) {
        if (AlertData.Button[ButtonNumber].strText[0]) {
            ++NumButtons;
            ButtonActionHashes[NumButtons - 1] = 0x0eb0ffe0 | ButtonAction;
            ++ButtonAction;
        }
    }

    bool bMsgTextWasLabel;
    char *pMsgText = DecodeString(AlertData.strMessage, &bMsgTextWasLabel);
    if (NumButtons == 1) {
        m_Dialog = DialogInterface::ShowOneButton(m_pOwner->GetPackageName(), "", dialog_alert, 0,
                                                   ButtonActionHashes[0], pMsgText);
    } else if (NumButtons == 0) {
        m_Dialog =
            DialogInterface::ShowMessage(m_pOwner->GetPackageName(), "", dialog_alert, pMsgText);
    } else if (NumButtons == 2) {
        m_Dialog = DialogInterface::ShowTwoButtons(
            m_pOwner->GetPackageName(), "", dialog_alert, 0, 0, ButtonActionHashes[0],
            ButtonActionHashes[1], ButtonActionHashes[1], first_dialog_button1, pMsgText);
    } else if (NumButtons == 3) {
        m_Dialog = DialogInterface::ShowThreeButtons(
            m_pOwner->GetPackageName(), "", dialog_alert, 0, 0, 0, ButtonActionHashes[0],
            ButtonActionHashes[1], ButtonActionHashes[2], 0, first_dialog_button1, pMsgText);
    }
    if (m_Dialog && !bMsgTextWasLabel) {
        DialogInterface::SetBlurbIsUTF8();
    }
    PopulateAlertDialog();
}

void CWebOfferUG2::StartHTTP(const WebOfferBusyT &BusyData) {
    if (m_Dialog) {
        DialogInterface::DismissDialog(m_Dialog);
    }
    bool bTextWasLabel;
    char *pMsgText = DecodeString(BusyData.strMessage, &bTextWasLabel);
    m_Dialog = DialogInterface::ShowOneButton(m_pOwner->GetPackageName(), "", dialog_alert,
                                               0x1a294dad, 0x0eb0ffe1, pMsgText);
    if (!bTextWasLabel) {
        DialogInterface::SetBlurbIsUTF8();
    }
}
