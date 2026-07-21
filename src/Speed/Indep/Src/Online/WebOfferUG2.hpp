#ifndef ONLINE_WEBOFFERUG2_HPP
#define ONLINE_WEBOFFERUG2_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct WebOfferT;

struct WebOfferSetupT {
    char strPersona[20];
    char strFavTeam[32];
    char strProduct[32];
    char strPlatform[32];
    char strLanguage[4];
    char strCountry[4];
    char strSlusCode[32];
    char strLKey[40];
    char strSkuCode[32];
};

struct WebOfferCommandT {
    int iCommand;
    char strParms[4096];
};

struct WebOfferAlertT;
struct WebOfferPromoT;
struct WebOfferCreditT;
struct WebOfferBusyT;
struct WebOfferNewsT;

enum EProcessAction {
    eProcessAction_Nothing = 10,
    eProcessAction_AbortScript = 0,
    eProcessAction_Button1 = 1,
    eProcessAction_Button2 = 2,
    eProcessAction_Button3 = 3,
    eProcessAction_Button4 = 4
};

class CWebOffer {
  protected:
    WebOfferT *m_pWebOfferAPI;
    WebOfferCommandT *m_pCurrentCommand;
    WebOfferSetupT m_WebOfferSetup;

  private:
    bool m_bProcessingCommand;

  public:
    struct SOfferData {
        char *pPersonaName;
        char *pProductName;
        char *pPlatformName;
        char *pLanguage;
        char *pCountry;
        char *pSLUSCode;
        char *pLobbyKey;
        char *pOfferURL;
    };

    CWebOffer();
    virtual ~CWebOffer();
    bool Initialise(uint32 MaxScriptSize);
    void Shutdown();
    bool Start(SOfferData &OfferData);
    void Tick();

  protected:
    virtual void StartAlert(WebOfferAlertT &) {}
    virtual void StartPromo(WebOfferPromoT &) {}
    virtual void StartCredit(WebOfferCreditT &) {}
    virtual void StartHTTP(WebOfferBusyT &) {}
    virtual void StartNews(char *, WebOfferNewsT &) {}
    virtual EProcessAction ProcessAlert() { return eProcessAction_Nothing; }
    virtual EProcessAction ProcessPromo() { return eProcessAction_Nothing; }
    virtual EProcessAction ProcessCredit() { return eProcessAction_Nothing; }
    virtual EProcessAction ProcessHTTP() { return eProcessAction_Nothing; }
    virtual EProcessAction ProcessNews() { return eProcessAction_Nothing; }
    virtual void EndAlert() {}
    virtual void EndPromo() {}
    virtual void EndCredit() {}
    virtual void EndHTTP() {}
    virtual void EndNews() {}
    virtual bool FillInPromoSubmitData(char *, int) { return false; }
    virtual bool FillInCreditCardSubmitData(WebOfferCreditT &) { return false; }
    virtual void Finished(int) {}

  private:
    void StartNextCommand();
    void ProcessCommand();
    void _StartAlert();
    void _StartPromo();
    void _StartCredit();
    void _StartHTTP();
    void _StartNews();
    void _ProcessAlert();
    void _ProcessPromo();
    void _ProcessCredit();
    void _ProcessHTTP();
    void _ProcessNews();
    void _Finished();
};

#endif
