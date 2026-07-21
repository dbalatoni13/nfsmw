#include "WebOfferUG2.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"

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
