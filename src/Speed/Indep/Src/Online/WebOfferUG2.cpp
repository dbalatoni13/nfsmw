#include "WebOfferUG2.hpp"

extern "C" WebOfferT *WebOfferCreate(uint32 maxScriptSize);
extern "C" void WebOfferDestroy(WebOfferT *webOffer);

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
