#include "WebOfferUG2.hpp"

CWebOffer::CWebOffer()
    : m_pWebOfferAPI(nullptr) //
    , m_pCurrentCommand(nullptr) //
    , m_bProcessingCommand(false) {}

CWebOffer::~CWebOffer() { Shutdown(); }
