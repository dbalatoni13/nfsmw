/************************************************************************
*                                                                       *
*   Xam.h -- This module defines the system APIs                        *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#ifndef __XAM_H__
#define __XAM_H__



#ifdef __cplusplus
extern "C" {
#endif


//
// Application Identifiers
//

#define APPSZ_XAM                               ("xam.dll")
#define APPSZ_UVBASE                            ("uvbase.xex")
#define APPSZ_XLIVEBASE                         ("xlivebase.xex")
#define APPSZ_XGI                               ("xgi.xex")
#define APPSZ_SIGNINUI                          ("signin.xex")
#define APPSZ_FRIENDSUI                         ("friends.xex")
#define APPSZ_FEEDBACKUI                        ("feedback.xex")
#define APPSZ_XMP                               ("musicplayer.xex")
#define APPSZ_GAMERPROFILE                      ("gamerprofile.xex")
#define APPSZ_MARKETPLACEUI                     ("marketplace.xex")
#define APPSZ_VIRTUALKEYBOARD                   ("vk.xex")
#define APPSZ_QUICKCHATUI                       ("quickchat.xex")
#define APPSZ_VOICEMAILUI                       ("voicemail.xex")
#define APPSZ_CREATEPROFILE                     ("createprofile.xex")
#define APPSZ_DEVICESELECTORUI                  ("deviceselector.xex")
#define APPSZ_UPDATERUI                         ("updater.xex")
#define APPSZ_MINIMEDIAPLAYER                   ("minimediaplayer.xex")


typedef PVOID                                   HXAMAPP, *PHXAMAPP;

// The hardcoded handles need to remain in this order:
//  (1) UI app
//  (2) XAM
//  (3) Always running apps (order doesn't really matters)
//  (4) [In the future] other apps
#define HXAMAPP_UI                              ((HXAMAPP)0x000000FF)
#define HXAMAPP_XAM                             ((HXAMAPP)0x000000FE)
#define HXAMAPP_UVBASE                          ((HXAMAPP)0x000000FD)
#define HXAMAPP_XLIVEBASE                       ((HXAMAPP)0x000000FC)
#define HXAMAPP_XGI                             ((HXAMAPP)0x000000FB)
#define HXAMAPP_XMP                             ((HXAMAPP)0x000000FA)

// Title HXAMAPP.  Cannot be used as the other ones, can just be used as an
// identifier.  Keep it outside of the range of the other ones and also do
// not account fir it in the _HXAMAPP_Xxx_ define's below.
#define HXAMAPP_TITLE                           ((HXAMAPP)0x000000EE)

#define _HXAMAPP_MASK_                          0x0000FFFF

//
// Message Identifiers
// The high word of a message identifier is reserved
// The low word of a message identifier is a unique message number
//

#define MSGID(Area, Number)                     (DWORD)((WORD)(Area) << 16 | (WORD)(Number))
#define MSG_AREA(msgid)                         (((msgid) >> 16) & 0xFFFF)
#define MSG_NUMBER(msgid)                       ((msgid) & 0xFFFF)

//
// Message Areas
//

#define MSGAREA_NONE                            (0x0000)
#define MSGAREA_XAM                             (0x0002)
#define MSGAREA_XBASE                           MSGAREA_XAM
#define MSGAREA_SYSTEM                          (0x8000)
#define MSGAREA_XLIVEBASE                       (0x0005)
#define MSGAREA_UVBASE                          (0x0010)
#define MSGAREA_XGI                             (0x000B)
#define MSGAREA_XMP                             (0x0007)

//
// System Messages
//

#define XM_SYS_BASE                             MSGID(MSGAREA_SYSTEM, 0x0001)
#define XM_SYS_TITLESTARTUP                     MSGID(MSGAREA_SYSTEM, 0x0002)
#define XM_SYS_SHUTDOWN                         MSGID(MSGAREA_SYSTEM, 0x0003)
#define XM_SYS_LOAD                             MSGID(MSGAREA_SYSTEM, 0x0004)
#define XM_SYS_UNLOAD                           MSGID(MSGAREA_SYSTEM, 0x0005)
#define XM_SYS_TITLESHUTDOWN                    MSGID(MSGAREA_SYSTEM, 0x0006)
#define XM_SYS_ASYNCMESSAGE                     MSGID(MSGAREA_SYSTEM, 0x0007)
#define XM_SYS_UIINIT                           MSGID(MSGAREA_SYSTEM, 0x0008)
#define XM_SYS_PERSIST                          MSGID(MSGAREA_SYSTEM, 0x0009)
#define XM_SYS_UPDATEFRAME                      MSGID(MSGAREA_SYSTEM, 0x000A)
#define XM_SYS_THREADATTACH                     MSGID(MSGAREA_SYSTEM, 0x000B)
#define XM_SYS_THREADDETACH                     MSGID(MSGAREA_SYSTEM, 0x000C)
#define XM_SYS_LOCALECHANGE                     MSGID(MSGAREA_SYSTEM, 0x000D)
#define XM_SYS_XNOTIFYPREVIEW                   MSGID(MSGAREA_SYSTEM, 0x000E)

//
// XLiveBase
//
#define XM_LIVEBASE_LOGONDISCONNECT             MSGID(MSGAREA_XLIVEBASE, 0x8002)
#define XM_LIVEBASE_LOGONGETHR                  MSGID(MSGAREA_XLIVEBASE, 0x8003)
#define XM_LIVEBASE_LOGONGETID                  MSGID(MSGAREA_XLIVEBASE, 0x8004)
#define XM_LIVEBASE_LOGONGETUSERS               MSGID(MSGAREA_XLIVEBASE, 0x8005)
#define XM_LIVEBASE_LOGONGETNATTYPE             MSGID(MSGAREA_XLIVEBASE, 0x8006)
#define XM_LIVEBASE_LOGONCHECKUSERPRIVILEGE     MSGID(MSGAREA_XLIVEBASE, 0x8008)
#define XM_LIVEBASE_CONTENTAVAILABLE            MSGID(MSGAREA_XLIVEBASE, 0x8009)
#define XM_LIVEBASE_CONTENTUPDATEACCESSTIMES    MSGID(MSGAREA_XLIVEBASE, 0x800A)
#define XM_LIVEBASE_AREUSERSFRIENDS             MSGID(MSGAREA_XLIVEBASE, 0x800B)
#define XM_LIVEBASE_MUTELISTADD                 MSGID(MSGAREA_XLIVEBASE, 0x800C)
#define XM_LIVEBASE_MUTELISTREMOVE              MSGID(MSGAREA_XLIVEBASE, 0x800D)
#define XM_LIVEBASE_MUTELISTQUERY               MSGID(MSGAREA_XLIVEBASE, 0x800E)
#define XM_LIVEBASE_PLAYERLISTADD               MSGID(MSGAREA_XLIVEBASE, 0x800F)
#define XM_LIVEBASE_PLAYERLISTUPDATE            MSGID(MSGAREA_XLIVEBASE, 0x8010)
#define XM_LIVEBASE_PLAYERCREATEENUM            MSGID(MSGAREA_XLIVEBASE, 0x8011)
#define XM_LIVEBASE_PLAYERENUMERATE             MSGID(MSGAREA_XLIVEBASE, 0x8012)
#define XM_LIVEBASE_PLAYERCLOSEENUM             MSGID(MSGAREA_XLIVEBASE, 0x8013)
#define XM_LIVEBASE_PRESENCEUPDATESTATEFLAGS    MSGID(MSGAREA_XLIVEBASE, 0x8014)
#define XM_LIVEBASE_PRESENCESETGAMESESSION      MSGID(MSGAREA_XLIVEBASE, 0x8015)
#define XM_LIVEBASE_PRESENCEGETSESSION          MSGID(MSGAREA_XLIVEBASE, 0x8016)

//
// XBase
//

#define XM_BASE_SUBAREA                         MSGID(MSGAREA_XBASE, 0xF000)

#define XM_BASE_SUBAREA_CONTENT                 MSGID(MSGAREA_XBASE, 0x0000)
#define XM_BASE_CONTENTCREATE                   MSGID(MSGAREA_XBASE, 0x0001)
#define XM_BASE_CONTENTOPEN                     MSGID(MSGAREA_XBASE, 0x0002)
#define XM_BASE_CONTENTCLOSE                    MSGID(MSGAREA_XBASE, 0x0003)
#define XM_BASE_CONTENTDELETE                   MSGID(MSGAREA_XBASE, 0x0004)
#define XM_BASE_CONTENTENUMERATE                MSGID(MSGAREA_XBASE, 0x0005)
#define XM_BASE_CONTENTCREATEENUM               MSGID(MSGAREA_XBASE, 0x0006)
#define XM_BASE_CONTENTCLOSEENUM                MSGID(MSGAREA_XBASE, 0x0007)
#define XM_BASE_DEVICECREATEENUM                MSGID(MSGAREA_XBASE, 0x0008)
#define XM_BASE_DEVICECLOSEENUM                 MSGID(MSGAREA_XBASE, 0x0009)
#define XM_BASE_DEVICEENUMERATE                 MSGID(MSGAREA_XBASE, 0x000A)

#define XM_BASE_SUBAREA_UI                      MSGID(MSGAREA_XBASE, 0x1000)
#define XM_BASE_UISHOWSIGNIN                    MSGID(MSGAREA_XBASE, 0x1001)
#define XM_BASE_UISHOWFRIENDS                   MSGID(MSGAREA_XBASE, 0x1002)
#define XM_BASE_UISHOWPLAYERS                   MSGID(MSGAREA_XBASE, 0x1003)
#define XM_BASE_UISHOWMESSAGES                  MSGID(MSGAREA_XBASE, 0x1004)
#define XM_BASE_UISHOWKEYBOARD                  MSGID(MSGAREA_XBASE, 0x1005)
#define XM_BASE_UISHOWQUICKCHAT                 MSGID(MSGAREA_XBASE, 0x1006)
#define XM_BASE_UISHOWVOICEMAIL                 MSGID(MSGAREA_XBASE, 0x1007)
#define XM_BASE_UISHOWGAMERPROFILE              MSGID(MSGAREA_XBASE, 0x1008)
#define XM_BASE_UISHOWFEEDBACK                  MSGID(MSGAREA_XBASE, 0x1009)
#define XM_BASE_UISHOWMARKETPLACE               MSGID(MSGAREA_XBASE, 0x100A)
#define XM_BASE_UISHOWDEVICESELECTOR            MSGID(MSGAREA_XBASE, 0x100B)
#define XM_BASE_UISHOWUPDATER                   MSGID(MSGAREA_XBASE, 0x100C)
#define XM_BASE_UISHOWMESSAGEBOX                MSGID(MSGAREA_XBASE, 0x100D)
#define XM_BASE_UISHOWCREATEPROFILE             MSGID(MSGAREA_XBASE, 0x100E)
#define XM_BASE_UISHOWLIVEUPSELL                MSGID(MSGAREA_XBASE, 0x100F)

#define XM_BASE_SUBAREA_PROFILE                 MSGID(MSGAREA_XBASE, 0x3000)

//
// XMP Messages
//


//
// Virtual key codes for XInput APIs
//

#define VK_CANCEL                       0x03
#define VK_BACK                         0x08
#define VK_TAB                          0x09
#define VK_CLEAR                        0x0C
#define VK_RETURN                       0x0D
#define VK_SHIFT                        0x10
#define VK_CONTROL                      0x11
#define VK_MENU                         0x12
#define VK_PAUSE                        0x13
#define VK_CAPITAL                      0x14
#define VK_KANA                         0x15
#define VK_HANGEUL                      0x15
#define VK_HANGUL                       0x15
#define VK_JUNJA                        0x17
#define VK_FINAL                        0x18
#define VK_HANJA                        0x19
#define VK_KANJI                        0x19
#define VK_ESCAPE                       0x1B
#define VK_CONVERT                      0x1C
#define VK_NONCONVERT                   0x1D
#define VK_ACCEPT                       0x1E
#define VK_MODECHANGE                   0x1F
#define VK_SPACE                        0x20
#define VK_PRIOR                        0x21
#define VK_NEXT                         0x22
#define VK_END                          0x23
#define VK_HOME                         0x24
#define VK_LEFT                         0x25
#define VK_UP                           0x26
#define VK_RIGHT                        0x27
#define VK_DOWN                         0x28
#define VK_SELECT                       0x29
#define VK_PRINT                        0x2A
#define VK_EXECUTE                      0x2B
#define VK_SNAPSHOT                     0x2C
#define VK_INSERT                       0x2D
#define VK_DELETE                       0x2E
#define VK_HELP                         0x2F
#define VK_0                            0x30
#define VK_1                            0x31
#define VK_2                            0x32
#define VK_3                            0x33
#define VK_4                            0x34
#define VK_5                            0x35
#define VK_6                            0x36
#define VK_7                            0x37
#define VK_8                            0x38
#define VK_9                            0x39

//
// 0x40 : unassigned
// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
//

#define VK_LWIN                         0x5B
#define VK_RWIN                         0x5C
#define VK_APPS                         0x5D
#define VK_SLEEP                        0x5F
#define VK_NUMPAD0                      0x60
#define VK_NUMPAD1                      0x61
#define VK_NUMPAD2                      0x62
#define VK_NUMPAD3                      0x63
#define VK_NUMPAD4                      0x64
#define VK_NUMPAD5                      0x65
#define VK_NUMPAD6                      0x66
#define VK_NUMPAD7                      0x67
#define VK_NUMPAD8                      0x68
#define VK_NUMPAD9                      0x69
#define VK_MULTIPLY                     0x6A
#define VK_ADD                          0x6B
#define VK_SEPARATOR                    0x6C
#define VK_SUBTRACT                     0x6D
#define VK_DECIMAL                      0x6E
#define VK_DIVIDE                       0x6F
#define VK_F1                           0x70
#define VK_F2                           0x71
#define VK_F3                           0x72
#define VK_F4                           0x73
#define VK_F5                           0x74
#define VK_F6                           0x75
#define VK_F7                           0x76
#define VK_F8                           0x77
#define VK_F9                           0x78
#define VK_F10                          0x79
#define VK_F11                          0x7A
#define VK_F12                          0x7B
#define VK_F13                          0x7C
#define VK_F14                          0x7D
#define VK_F15                          0x7E
#define VK_F16                          0x7F
#define VK_F17                          0x80
#define VK_F18                          0x81
#define VK_F19                          0x82
#define VK_F20                          0x83
#define VK_F21                          0x84
#define VK_F22                          0x85
#define VK_F23                          0x86
#define VK_F24                          0x87
#define VK_NUMLOCK                      0x90
#define VK_SCROLL                       0x91
#define VK_OEM_NEC_EQUAL                0x92
#define VK_OEM_FJ_JISHO                 0x92
#define VK_OEM_FJ_MASSHOU               0x93
#define VK_OEM_FJ_TOUROKU               0x94
#define VK_OEM_FJ_LOYA                  0x95
#define VK_OEM_FJ_ROYA                  0x96
#define VK_LSHIFT                       0xA0
#define VK_RSHIFT                       0xA1
#define VK_LCONTROL                     0xA2
#define VK_RCONTROL                     0xA3
#define VK_LMENU                        0xA4
#define VK_RMENU                        0xA5
#define VK_BROWSER_BACK                 0xA6
#define VK_BROWSER_FORWARD              0xA7
#define VK_BROWSER_REFRESH              0xA8
#define VK_BROWSER_STOP                 0xA9
#define VK_BROWSER_SEARCH               0xAA
#define VK_BROWSER_FAVORITES            0xAB
#define VK_BROWSER_HOME                 0xAC
#define VK_VOLUME_MUTE                  0xAD
#define VK_VOLUME_DOWN                  0xAE
#define VK_VOLUME_UP                    0xAF
#define VK_MEDIA_NEXT_TRACK             0xB0
#define VK_MEDIA_PREV_TRACK             0xB1
#define VK_MEDIA_STOP                   0xB2
#define VK_MEDIA_PLAY_PAUSE             0xB3
#define VK_LAUNCH_MAIL                  0xB4
#define VK_LAUNCH_MEDIA_SELECT          0xB5
#define VK_LAUNCH_APP1                  0xB6
#define VK_LAUNCH_APP2                  0xB7
#define VK_OEM_1                        0xBA
#define VK_OEM_PLUS                     0xBB
#define VK_OEM_COMMA                    0xBC
#define VK_OEM_MINUS                    0xBD
#define VK_OEM_PERIOD                   0xBE
#define VK_OEM_2                        0xBF
#define VK_OEM_3                        0xC0
#define VK_OEM_4                        0xDB
#define VK_OEM_5                        0xDC
#define VK_OEM_6                        0xDD
#define VK_OEM_7                        0xDE
#define VK_OEM_8                        0xDF
#define VK_OEM_AX                       0xE1
#define VK_OEM_102                      0xE2
#define VK_ICO_HELP                     0xE3
#define VK_ICO_00                       0xE4
#define VK_PROCESSKEY                   0xE5
#define VK_ICO_CLEAR                    0xE6
#define VK_PACKET                       0xE7
#define VK_PLAY                         0xFA

#define VK_DVDMENU                      0x5870
#define VK_DVDANGLE                     0x5871
#define VK_DVDAUDIO                     0x5872
#define VK_DVDSUBTITLE                  0x5873
#define VK_RADIO                        0x5874
#define VK_DISPLAY                      0x5875
#define VK_STOP                         0x5876
#define VK_RECORD                       0x5877
#define VK_FASTFWD                      0x5878
#define VK_REWIND                       0x5879
#define VK_SKIP                         0x587A
#define VK_REPLAY                       0x587B
#define VK_INFO                         0x587C
#define VK_GUIDE                        0x5803  // same as VK_PAD_Y
#define VK_TV                           0x5801  // same as VK_PAD_B
#define VK_MYTV                         0x587F
#define VK_MYMUSIC                      0x5880
#define VK_RECORDTV                     0x5881
#define VK_MYPICTURES                   0x5882
#define VK_MYVIDEOS                     0x5883
#define VK_ASPECT                       0x5884
#define VK_STAR                         0x5885
#define VK_POUND                        0x5886
#define VK_TITLE                        0x5887
#define VK_MCE                          0x5888
#define VK_KIOSK                        0x5889
#define VK_TTONOFF                      0x588A
#define VK_TTRED                        0x588B
#define VK_TTGREEN                      0x588C
#define VK_TTYELLOW                     0x588D
#define VK_TTBLUE                       0x588E
#define VK_TTINDEX                      0x588F
#define VK_TTORANGE                     0x5890

//
// Codes returned for the gamepad input
//

#define VK_PAD_A                        0x5800
#define VK_PAD_B                        0x5801
#define VK_PAD_X                        0x5802
#define VK_PAD_Y                        0x5803
#define VK_PAD_RSHOULDER                0x5804
#define VK_PAD_LSHOULDER                0x5805
#define VK_PAD_LTRIGGER                 0x5806
#define VK_PAD_RTRIGGER                 0x5807

#define VK_PAD_DPAD_UP                  0x5810
#define VK_PAD_DPAD_DOWN                0x5811
#define VK_PAD_DPAD_LEFT                0x5812
#define VK_PAD_DPAD_RIGHT               0x5813
#define VK_PAD_START                    0x5814
#define VK_PAD_BACK                     0x5815
#define VK_PAD_LTHUMB_PRESS             0x5816
#define VK_PAD_RTHUMB_PRESS             0x5817

#define VK_PAD_LTHUMB_UP                0x5820
#define VK_PAD_LTHUMB_DOWN              0x5821
#define VK_PAD_LTHUMB_RIGHT             0x5822
#define VK_PAD_LTHUMB_LEFT              0x5823
#define VK_PAD_LTHUMB_UPLEFT            0x5824
#define VK_PAD_LTHUMB_UPRIGHT           0x5825
#define VK_PAD_LTHUMB_DOWNRIGHT         0x5826
#define VK_PAD_LTHUMB_DOWNLEFT          0x5827

#define VK_PAD_RTHUMB_UP                0x5830
#define VK_PAD_RTHUMB_DOWN              0x5831
#define VK_PAD_RTHUMB_RIGHT             0x5832
#define VK_PAD_RTHUMB_LEFT              0x5833
#define VK_PAD_RTHUMB_UPLEFT            0x5834
#define VK_PAD_RTHUMB_UPRIGHT           0x5835
#define VK_PAD_RTHUMB_DOWNRIGHT         0x5836
#define VK_PAD_RTHUMB_DOWNLEFT          0x5837


#define VKBD_DEFAULT                    0x00000000
#define VKBD_LATIN_FULL                 0x00000001
#define VKBD_LATIN_EMAIL                0x00000002
#define VKBD_LATIN_GAMERTAG             0x00000004
#define VKBD_LATIN_PHONE                0x00000008
#define VKBD_LATIN_IP_ADDRESS           0x00000010
#define VKBD_LATIN_NUMERIC              0x00000020
#define VKBD_LATIN_ALPHABET             0x00000040
#define VKBD_LATIN_PASSWORD             0x00000080
#define VKBD_LATIN_SUBSCRIPTION         0x00000100
#define VKBD_JAPANESE_FULL              0x00001000
#define VKBD_KOREAN_FULL                0x00002000
#define VKBD_TCH_FULL                   0x00004000
#define VKBD_SELECT_OK                  0x10000000
#define VKBD_HIGHLIGHT_TEXT             0x20000000


//------------------------------------------------------------------------------
// Debug instrumentation
//

XBOXAPI
VOID
WINAPI
XDebugSetSystemOutputLevel(
    IN      HXAMAPP                     hxamapp             OPTIONAL,
    IN      ULONG                       ulLevel
    );

XBOXAPI
VOID
WINAPI
XDebugSetSystemBreakLevel(
    IN      HXAMAPP                     hxamapp             OPTIONAL,
    IN      ULONG                       ulLevel
    );


//------------------------------------------------------------------------------
// Message processing
//

typedef
HRESULT
(WINAPI
*PMESSAGEPROC)(
    IN     DWORD                        dwCommand,
    IN OUT UINT_PTR                     pParam1,
    IN OUT UINT_PTR                     pParam2
    );

HRESULT
WINAPI
XMsgInProcessCall(
    IN      HXAMAPP                     hxamapp,
    IN      DWORD                       dwMessage,
    IN OUT  UINT_PTR                    param1,
    IN OUT  UINT_PTR                    param2
    );

HRESULT
WINAPI
XMsgStartIORequest(
    IN      HXAMAPP                     hxamapp,
    IN      DWORD                       dwMessage,
    IN      PXOVERLAPPED                pOverlapped,
    IN OUT  PVOID                       pUserBuffer,
    IN      DWORD                       cbUserBuffer
    );

HRESULT
WINAPI
XMsgStartIORequestEx(
    IN      HXAMAPP                     hxamapp,
    IN      DWORD                       dwMessage,
    IN      PXOVERLAPPED                pOverlapped,
    IN OUT  PVOID                       pUserBuffer,
    IN      DWORD                       cbUserBuffer,
    IN      PVOID                       pTaskAttribs
    );

HRESULT
WINAPI
XMsgCompleteIORequest(
    IN      PXOVERLAPPED                pOverlapped,
    IN      DWORD                       dwStatus,
    IN      DWORD                       dwExtendedError,
    IN      ULONG                       ulBytesTransferred
    );

HRESULT
WINAPI
XMsgSystemProcessCall(
    IN      HXAMAPP                     hxamapp,
    IN      DWORD                       dwMessage,
    IN      PVOID                       pUserBuffer,
    IN      DWORD                       cbUserBuffer
    );

HRESULT
WINAPI
XMsgCancelIORequest(
    IN      PXOVERLAPPED                pOverlapped,
    IN      BOOL                        fWait
    );



#define XAMINLINE                       FORCEINLINE


//------------------------------------------------------------------------------
// Notification apis
//------------------------------------------------------------------------------

//  Notification ids are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-----------+-----------------+-------------------------------+
//  |R|    Area   |    Version      |            Index              |
//  +-+-----+-----+-----------------+-------------------------------+
//
//  where
//
//      R - is a reserved bit (internal)
//
//      Area - is the area ranging from 0 - 63 (6 bits)
//
//      Version - is used to ensure that new notifications are not sent to
//          titles built to previous XDKs
//
//      Index - is the id of the notification within the area.  Each area can
//           have up to 65535 notifications starting at 1 (zero being invalid).
//

#define XNID(Version, Area, Index)      (DWORD)( (WORD)(Area) << 25 | (WORD)(Version) << 16 | (WORD)(Index))
#define XNID_VERSION(msgid)             (((msgid) >> 16) & 0x1FF)
#define XNID_AREA(msgid)                (((msgid) >> 25) & 0x3F)
#define XNID_INDEX(msgid)               ((msgid) & 0xFFFF)


//
// Notification Areas
//

#define XNOTIFY_SYSTEM                  (0x00000001)
#define XNOTIFY_LIVE                    (0x00000002)
#define XNOTIFY_FRIENDS                 (0x00000004)
#define XNOTIFY_CUSTOM                  (0x00000008)
#define XNOTIFY_XMP                     (0x00000020)
#define XNOTIFY_ALL                     (XNOTIFY_SYSTEM | XNOTIFY_LIVE | XNOTIFY_FRIENDS | XNOTIFY_CUSTOM | XNOTIFY_XMP)

//
// Bit numbers of each area (bit 0 is the least significant bit)
//

#define _XNAREA_SYSTEM                  (0)
#define _XNAREA_LIVE                    (1)
#define _XNAREA_FRIENDS                 (2)
#define _XNAREA_CUSTOM                  (3)
#define _XNAREA_XMP                     (5)

//
// System notifications
//

#define XN_SYS_FIRST                    XNID(0, _XNAREA_SYSTEM, 0x0001)
#define XN_SYS_UI                       XNID(0, _XNAREA_SYSTEM, 0x0009)
#define XN_SYS_SIGNINCHANGED            XNID(0, _XNAREA_SYSTEM, 0x000a)
#define XN_SYS_STORAGEDEVICESCHANGED    XNID(0, _XNAREA_SYSTEM, 0x000b)
#define XN_SYS_PROFILESETTINGCHANGED    XNID(0, _XNAREA_SYSTEM, 0x000e)
#define XN_SYS_MUTELISTCHANGED          XNID(0, _XNAREA_SYSTEM, 0x0011)
#define XN_SYS_INPUTDEVICESCHANGED      XNID(0, _XNAREA_SYSTEM, 0x0012)
#define XN_SYS_LAST                     XNID(0, _XNAREA_SYSTEM, 0x0013)


//
// Live notifications
//

#define XN_LIVE_FIRST                   XNID(0, _XNAREA_LIVE, 0x0001)
#define XN_LIVE_CONNECTIONCHANGED       XNID(0, _XNAREA_LIVE, 0x0001)
#define XN_LIVE_INVITE_ACCEPTED         XNID(0, _XNAREA_LIVE, 0x0002)
#define XN_LIVE_LINK_STATE_CHANGED      XNID(0, _XNAREA_LIVE, 0x0003)
#define XN_LIVE_CONTENT_INSTALLED       XNID(0, _XNAREA_LIVE, 0x0007)
#define XN_LIVE_MEMBERSHIP_PURCHASED    XNID(0, _XNAREA_LIVE, 0x0008)
#define XN_LIVE_VOICECHAT_AWAY          XNID(0, _XNAREA_LIVE, 0x0009)
#define XN_LIVE_PRESENCE_CHANGED        XNID(0, _XNAREA_LIVE, 0x000A)
#define XN_LIVE_LAST                    XNID(0, _XNAREA_LIVE, 0x000E)

//
// Friends notifications
//

#define XN_FRIENDS_FIRST                XNID(0, _XNAREA_FRIENDS, 0x0001)
#define XN_FRIENDS_PRESENCE_CHANGED     XNID(0, _XNAREA_FRIENDS, 0x0001)
#define XN_FRIENDS_FRIEND_ADDED         XNID(0, _XNAREA_FRIENDS, 0x0002)
#define XN_FRIENDS_FRIEND_REMOVED       XNID(0, _XNAREA_FRIENDS, 0x0003)
#define XN_FRIENDS_LAST                 XNID(0, _XNAREA_FRIENDS, 0x0007)

//
// Custom notifications
//

#define XN_CUSTOM_FIRST                 XNID(0, _XNAREA_CUSTOM, 0x0001)
#define XN_CUSTOM_GAMEBANNERPRESSED     XNID(0, _XNAREA_CUSTOM, 0x0002)
#define XN_CUSTOM_ACTIONPRESSED         XNID(0, _XNAREA_CUSTOM, 0x0003)
#define XN_CUSTOM_LAST                  XNID(0, _XNAREA_CUSTOM, 0x0004)


//
// XMP notifications
//

#define XN_XMP_FIRST                           XNID(0, _XNAREA_XMP, 0x0001)
#define XN_XMP_STATECHANGED                    XNID(0, _XNAREA_XMP, 0x0001)
#define XN_XMP_PLAYBACKBEHAVIORCHANGED         XNID(0, _XNAREA_XMP, 0x0002)
#define XN_XMP_PLAYBACKCONTROLLERCHANGED       XNID(0, _XNAREA_XMP, 0x0003)
#define XN_XMP_LAST                            XNID(0, _XNAREA_XMP, 0x0008)


HANDLE
WINAPI
XNotifyCreateListener(
    IN      ULONGLONG                   qwAreas
    );


BOOL
WINAPI
XNotifyGetNext(
    IN      HANDLE                      hNotification,
    IN      DWORD                       dwMsgFilter,
    IN      PDWORD                      pdwId,
    OUT     PULONG_PTR                  pParam
    );

//
// Popup Notifications
//
#define XNOTIFYUI_POS_TOP               1
#define XNOTIFYUI_POS_BOTTOM            2
#define XNOTIFYUI_POS_LEFT              4
#define XNOTIFYUI_POS_RIGHT             8
#define XNOTIFYUI_POS_VCENTER           0
#define XNOTIFYUI_POS_HCENTER           0

#define XNOTIFYUI_POS_TOPLEFT           (XNOTIFYUI_POS_TOP     | XNOTIFYUI_POS_LEFT)
#define XNOTIFYUI_POS_TOPCENTER         (XNOTIFYUI_POS_TOP     | XNOTIFYUI_POS_HCENTER)
#define XNOTIFYUI_POS_TOPRIGHT          (XNOTIFYUI_POS_TOP     | XNOTIFYUI_POS_RIGHT)
#define XNOTIFYUI_POS_CENTERLEFT        (XNOTIFYUI_POS_VCENTER | XNOTIFYUI_POS_LEFT)
#define XNOTIFYUI_POS_CENTER            (XNOTIFYUI_POS_VCENTER | XNOTIFYUI_POS_HCENTER)
#define XNOTIFYUI_POS_CENTERRIGHT       (XNOTIFYUI_POS_VCENTER | XNOTIFYUI_POS_RIGHT)
#define XNOTIFYUI_POS_BOTTOMLEFT        (XNOTIFYUI_POS_BOTTOM  | XNOTIFYUI_POS_LEFT)
#define XNOTIFYUI_POS_BOTTOMCENTER      (XNOTIFYUI_POS_BOTTOM  | XNOTIFYUI_POS_HCENTER)
#define XNOTIFYUI_POS_BOTTOMRIGHT       (XNOTIFYUI_POS_BOTTOM  | XNOTIFYUI_POS_RIGHT)

#define XNOTIFYUI_POS_VALIDBITS         (XNOTIFYUI_POS_TOP | XNOTIFYUI_POS_BOTTOM | XNOTIFYUI_POS_LEFT | XNOTIFYUI_POS_RIGHT)

VOID
WINAPI
XNotifyPositionUI(
    IN      DWORD                       dwPosition
    );

DWORD
WINAPI
XNotifyDelayUI(
    IN      ULONG                       ulMilliSeconds
    );


//------------------------------------------------------------------------------
// XAM Memory
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// XEnumerator helpers
//------------------------------------------------------------------------------
DWORD
WINAPI
XamCreateEnumeratorHandle(
    IN      DWORD                       dwUserIndex,
    IN      HXAMAPP                     hxamapp,
    IN      DWORD                       dwMsgIDEnum,
    IN      DWORD                       dwMsgIDCloseEnum,
    IN      DWORD                       cbSizeOfPrivateEnumStructure,
    IN      DWORD                       cItemsRequested,
    IN      DWORD                       dwEnumFlags,
    OUT     PHANDLE                     phEnum
    );

// Need to call ObDereferenceObject on pvObj on success
DWORD
WINAPI
XamGetPrivateEnumStructureFromHandle(
    IN      HANDLE                      hEnum,
    OUT     PVOID*                      ppvObj
    );


DWORD
WINAPI
XamUserGetName(
    IN      DWORD                       dwUserIndex,
    IN      LPSTR                       pUserName,
    IN      DWORD                       cchUserName
    );

typedef enum _XAMUSER_SIGNIN_STATE
{
    eXamUserSigninState_NotSignedIn,
    eXamUserSigninState_SignedInLocally,
    eXamUserSigninState_SignedInToLive
} XAMUSER_SIGNIN_STATE;

XAMUSER_SIGNIN_STATE
WINAPI
XamUserGetSigninState(
    IN      DWORD                       dwUserIndex
    );


//------------------------------------------------------------------------------
// XamProfile functions and flags
//------------------------------------------------------------------------------


#ifdef __cplusplus
};
#endif



#endif // __XAM_H__

