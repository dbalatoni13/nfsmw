#include "WebOfferUG2.hpp"

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
