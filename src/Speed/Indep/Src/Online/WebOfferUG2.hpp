#ifndef ONLINE_WEBOFFERUG2_HPP
#define ONLINE_WEBOFFERUG2_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct WebOfferT;

char *GetLocalizedString(uint32 hash);
int bStrToLong(const char *string);

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

struct WebOfferColorT {
    unsigned char uRed;
    unsigned char uGreen;
    unsigned char uBlue;
    unsigned char uAlpha;
};

struct WebOfferButtonT {
    char strType[1];
    char strText[40];
    WebOfferColorT Color;
};

struct WebOfferBusyT {
    char strTitle[64];
    char strMessage[1024];
    WebOfferColorT MsgColor;
    WebOfferButtonT Button[1];
};

struct WebOfferAlertT {
    char strTitle[64];
    char strImage[32];
    char strMessage[1024];
    WebOfferColorT MsgColor;
    WebOfferButtonT Button[4];
};

struct WebOfferCreditT {
    char strTitle[64];
    char strMessage[1024];
    WebOfferColorT MsgColor;
    WebOfferButtonT Button[4];
    char strFirstName[100];
    char strLastName[100];
    char strNameOnCard[200];
    char strAddress[2][100];
    char strCity[100];
    char strState[40];
    char strPostCode[40];
    char *pCountryList;
    char strCountry[50];
    char strEmail[100];
    char *pCardList;
    char strCardType[80];
    char strCardNumber[40];
    int iExpiryMonth;
    int iExpiryYear;
};

struct WebOfferPromoT {
    char strTitle[64];
    char strMessage[1024];
    WebOfferColorT MsgColor;
    WebOfferButtonT Button[4];
    char strPromo[200];
    char strPromoKPopTitle[64];
};

struct WebOfferNewsT {
    char strTitle[64];
    char strImage[32];
    WebOfferButtonT Button[4];
    WebOfferColorT NewsColor;
    char *pNews;
};

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
    virtual void StartAlert(const WebOfferAlertT &) {}
    virtual void StartPromo(const WebOfferPromoT &) {}
    virtual void StartCredit(const WebOfferCreditT &) {}
    virtual void StartHTTP(const WebOfferBusyT &) {}
    virtual void StartNews(const char *, const WebOfferNewsT &) {}
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

typedef int dialog_handle;

struct SFEngNewsScreenData {
    MenuScreen *pParentScreen;
    char *pNewsText;
    WebOfferNewsT WebOfferNews;
};

struct SButtonInit {
    char *pButtonText;
};

struct SButtons {
    uint32 NameHash;
    uint32 HighlightNameHash;
    EProcessAction Action;
};

struct FEngFont;

struct CTextScroller {
  private:
    MenuScreen *m_pOwner;
    FEngFont *m_pFont;
    FEScrollBar *m_pScrollBar;
    char m_TextBoxNameTemplate[32];
    int m_ViewWidth;
    int m_ViewVisibleLines;
    int m_NumAddedLines;
    int16 **m_pLines;
    char *m_pRawDataBlock;
    uint32 m_DataBlockSize;
    uint32 m_DataBlockCurPos;
    int m_TopLine;
    uint32 m_ScrollDownMsg;
    uint32 m_ScrollUpMsg;

  public:
    CTextScroller();
    ~CTextScroller();
    void Initialise(MenuScreen *pOwner, int ViewWidth, int ViewLines, char *pTextDisplayNameTempl,
                    FEngFont *pFont);
    void UseScrollBar(FEScrollBar *pScrollBar) { m_pScrollBar = pScrollBar; }
    void SetText(int16 *pText);
};

struct CUIWebOfferScreen : MenuScreen {
  protected:
    SButtons m_Buttons[2];
    uint8 m_NumButtons;

  public:
    CUIWebOfferScreen(ScreenConstructorData *sd)
        : MenuScreen(sd) //
        , m_NumButtons(0) {}
    virtual ~CUIWebOfferScreen() {}
    void InitialiseButtons();
    EProcessAction TestButtonMessage(uint32 msg);
};

struct CUIWebOfferNews : CUIWebOfferScreen {
  private:
    MenuScreen *m_pOwner;
    FEScrollBar m_ScrollBar;
    CTextScroller m_TextScroller;
    int m_iLastTopLine;

  public:
    CUIWebOfferNews(ScreenConstructorData *sd)
        : CUIWebOfferScreen(sd) //
        , m_ScrollBar(sd->PackageFilename, "Scrollbar", true, false, false) {}
    ~CUIWebOfferNews() override;
    void NotificationMessage(uint32 msg, FEObject *obj, uint32 param1, uint32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(uint32 msg, eMenuSoundTriggers maybe) override;

    friend MenuScreen *CreateOnlineNews(ScreenConstructorData *sd);
};

struct CUIWebOfferForeignReg : CUIWebOfferScreen {
  private:
    FEScrollBar m_ScrollBar;
    CTextScroller m_TextScroller;

  public:
    CUIWebOfferForeignReg(ScreenConstructorData *sd)
        : CUIWebOfferScreen(sd) //
        , m_ScrollBar(sd->PackageFilename, "Scrollbar", true, false, false) {}
    ~CUIWebOfferForeignReg() override {}
    void NotificationMessage(uint32 msg, FEObject *obj, uint32 param1, uint32 param2) override;

    friend MenuScreen *CreateForeignReg(ScreenConstructorData *sd);
};

class CWebOfferUG2 : public CWebOffer {
    SFEngNewsScreenData m_NewsData;
    WebOfferAlertT m_AlertData;

  private:
    EProcessAction m_PendingAction;
    char *m_pCurrentFEngPackage;
    MenuScreen *m_pOwner;
    dialog_handle m_Dialog;
    bool m_bAlertDialogPopulated;

  public:
    CWebOfferUG2()
        : m_PendingAction(eProcessAction_Nothing) //
        , m_pCurrentFEngPackage(nullptr) //
        , m_pOwner(nullptr) //
        , m_Dialog(0) //
        , m_bAlertDialogPopulated(false) {}
    ~CWebOfferUG2() override;
    void SetOwner(MenuScreen *owner) { m_pOwner = owner; }
    void SetPendingAction(EProcessAction action) { m_PendingAction = action; }
    void DismissDialog();
    char *DecodeString(const char *pString, bool *pbWasLabel) {
        if (pString && pString[0] == '0' && pString[1] == 'x') {
            int StringLabel = bStrToLong(pString);
            *pbWasLabel = true;
            return GetLocalizedString(StringLabel);
        }
        *pbWasLabel = false;
        return const_cast<char *>(pString);
    }

  protected:
    void StartAlert(const WebOfferAlertT &AlertData) override;
    void StartHTTP(const WebOfferBusyT &BusyData) override;
    void StartNews(const char *pNewsText, const WebOfferNewsT &NewsData) override;
    EProcessAction ProcessAlert() override;
    EProcessAction ProcessHTTP() override;
    EProcessAction ProcessNews() override;
    void EndAlert() override;
    void EndHTTP() override;
    void EndNews() override;
    void Finished(int ResultCode) override;
    void PopulateAlertDialog();
};

class CUIWebOfferStart : public MenuScreen {
  private:
    CWebOfferUG2 m_WebOffer;
    MenuScreen *m_pCurrentWebOfferScreen;
    static char m_NextPackageSuccess[32];
    static char m_NextPackageFail[32];
    static char m_WebOfferScript[512];

  public:
    CUIWebOfferStart(ScreenConstructorData *screenConstructorData)
        : MenuScreen(screenConstructorData) //
        , m_pCurrentWebOfferScreen(nullptr) {
        m_WebOffer.SetOwner(this);
    }
    ~CUIWebOfferStart() override;
    void Shutdown();
    static void SetNextPackages(const char *successPackage, const char *failPackage);
    static void MakeURLScript2(const char *url, uint32 titleHash, uint32 messageHash);
    static void MakeURLScript(const char *url);
    static int MakeDialogButtonHash(int buttonIndex);
    void NotificationMessage(uint32 msg, FEObject *obj, uint32 param1, uint32 param2) override;

    friend void ConfigureWebOfferForTOS();
    friend MenuScreen *CreateOnlineWebOfferScreen(ScreenConstructorData *sd);
};

void ConfigureWebOfferForTOS();
MenuScreen *CreateOnlineNews(ScreenConstructorData *sd);
MenuScreen *CreateOnlineWebOfferScreen(ScreenConstructorData *sd);
MenuScreen *CreateForeignReg(ScreenConstructorData *sd);

#endif
