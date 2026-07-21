#include "WebOfferUG2.hpp"

extern "C" WebOfferT *WebOfferCreate(uint32 maxScriptSize);
extern "C" void WebOfferDestroy(WebOfferT *webOffer);
extern "C" void WebOfferSetup(WebOfferT *webOffer, WebOfferSetupT *setup);
extern "C" void WebOfferExecute(WebOfferT *webOffer, const char *offerURL);
extern "C" WebOfferCommandT *WebOfferCommand(WebOfferT *webOffer);

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
