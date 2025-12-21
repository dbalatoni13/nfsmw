#ifndef ONLINE_ONLINECFG_H
#define ONLINE_ONLINECFG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Gameplay/GRace.h"

// TODO right place for these?
enum eOnlineGameDetails { eHOSTING = 0, eJOINING = 1, eVIEW_PLAYER = 2, eVIEW_GAME = 3 };

enum eOnlineDisconnectPerc {
    OLS_DISCONNECT_PERC_5 = 0,
    OLS_DISCONNECT_PERC_10 = 1,
    OLS_DISCONNECT_PERC_15 = 2,
    OLS_DISCONNECT_PERC_20 = 3,
    OLS_DISCONNECT_PERC_25 = 4,
    OLS_DISCONNECT_PERC_50 = 5,
    OLS_DISCONNECT_PERC_ANY = 6
};

enum eOnlinePrevPage { ePrevPage_None = 0, ePrevPage_LobbyRoom = 1 };

struct OnlineRaceParameters { // 0xc
    void Default();

    /* 0x0 */ uint32 EventHash;
    /* 0x4 */ uint8 NumLaps;
    /* 0x5 */ uint8 TrackDirection;
    /* 0x6 */ uint8 CopDensity;
    /* 0x8 */ bool IsLapKO;
};

enum eOnlineState {
    OLS_NETWORK_UNINITIALIZED = 0,
    OLS_CONNECTED_TO_ISP = 1,
    OLS_LOBBY_LOGGED_IN = 2,
    OLS_LOBBY_IDLE = 2,
    OLS_LOBBY_REQUEST_PENDING = 3,
    OLS_LOBBY_SEND_RACE_RESULTS_WAIT = 4,
    OLS_LOBBY_SENT_RACE_RESULTS = 5
};

// total size: 0x114
struct cOnlineSettings {
    /* 0x000 */ int iNumPlayers;
    static uint8 MaxOnlinePlayers;
    /* 0x004 */ uint8 MinOnlinePlayers;
    /* 0x008 */ eOnlineGameDetails GameDetails;
    /* 0x00c */ bool AnyCarClassAllowed;
    /* 0x010 */ bool RankedGame;
    /* 0x014 */ bool UseNOS;
    /* 0x018 */ bool CollisionDetection;
    /* 0x01c */ bool Passworded;
    /* 0x020 */ char Password[13];
    /* 0x02d */ char VerifyPassword[13];
    /* 0x03c */ bool PerformanceMatching;
    /* 0x040 */ eOnlineDisconnectPerc DisconnectPerc;
    /* 0x044 */ eOnlineDisconnectPerc CurrPlayerDisconnectPerc;
    /* 0x048 */ GRace::Type RaceMode;
    /* 0x04c */ bool QuickJoinRace;
    /* 0x050 */ bool EmailInformation;
    /* 0x054 */ bool ShareEmailAddress;
    /* 0x058 */ char CountryCode[3];
    /* 0x05c */ bool IsUserHostingLANServer;
    /* 0x060 */ bool IsPrivateRoom;
    /* 0x064 */ bool ShowPlayerNames;
    /* 0x068 */ bool ShowBadConnectionWarning;
    /* 0x06c */ bool AppearOffline;
    /* 0x070 */ bool ShowForeignReg;
    /* 0x074 */ int SelectedGameRoomListOption;
    /* 0x078 */ int SelectedConnectingScreen;
    /* 0x07c */ eOnlinePrevPage ePrevPage;

  private:
    /* 0x080 */ OnlineRaceParameters TheOnlineRaceSettings;
    /* 0x08c */ eOnlineState onlineState;
    /* 0x090 */ eOnlineState lastState;
    /* 0x094 */ bool inErrorState;
    /* 0x098 */ char NetworkUserName[20];
    /* 0x0ac */ char NetworkPersona[20];
    /* 0x0c0 */ char NetworkPasswd[20];
    /* 0x0d4 */ char LANUserName[20];
    /* 0x0e8 */ char LANPersona[20];
    /* 0x0fc */ char LANPasswd[20];
    /* 0x110 */ bool bLoggedInToBuddyServer;
    static char LobbyServerAddr[32];
    static uint16 LobbyServerPort;

  public:
    cOnlineSettings();
    void Default();
    void RestoreDefaultSettings();
    eOnlineState SetState();
    eOnlineState GetState();
    eOnlineState GetLastState();
    void SetErrorState();
    bool IsInErrorState();
    OnlineRaceParameters *GetRaceSettings();
    char *GetLobbyAccountName();
    char *GetLobbyPersona();
    char *GetLobbyPassword();
    void SetLobbyAccountName(const char *name);
    void SetLobbyPersona(const char *persona);
    void SetLobbyPassword(const char *password);
    char *GetLobbyServerAddr();
    uint16 GetLobbyServerPort();
    char *SaveToBuffer(char *buf);
    char *LoadFromBuffer(const char *buf);
    int32 GetSaveBufferSize();
    char *GetGamePassword();
    void SetGamePassword(const char *pwd);
    void SetGameVerifyPassword(const char *pwd);
    void SetBuddyServerLoggedIn();
    bool IsLoggedInToBuddyServer();
};

enum OnlineGender { eMale = 0, eFemale = 1, eNum_Gender = 2 };

struct OnlineCreateUserSettings { // 0xe8
    /* 0x00 */ char UserName[17];
    /* 0x11 */ char Password[17];
    /* 0x22 */ char ConfirmPassword[17];
    /* 0x33 */ char EmailAddress[51];
    /* 0x66 */ char ParentalEmail[51];
    /* 0x99 */ char TruncatedLongEmail[51];
    /* 0xcc */ bool EmailInformProducts;
    /* 0xd0 */ bool EmailInformPartners;
    /* 0xd4 */ int BirthMonth;
    /* 0xd8 */ int BirthDay;
    /* 0xdc */ int BirthYear;
    /* 0xe0 */ bool TermsOfService;
    /* 0xe4 */ OnlineGender Gender;

    void Initialize();
};

#endif
