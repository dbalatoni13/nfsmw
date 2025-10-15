/************************************************************************
*                                                                       *
*   Xbox.h -- This module defines the Xbox APIs                         *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#pragma once

#ifndef _XBOX_H_
#define _XBOX_H_


//
// Define API decoration for direct importing of DLL references.
//

#define XBOXAPI

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfig.h>

#define XUSER_NAME_SIZE                 16
#define XUSER_MAX_NAME_LENGTH           (XUSER_NAME_SIZE - 1)

typedef ULONGLONG                       XUID;
typedef XUID                            *PXUID;

#define INVALID_XUID                    ((XUID) 0)

XBOXAPI
FORCEINLINE
BOOL
WINAPI
IsEqualXUID(
    IN      XUID                        xuid1,
    IN      XUID                        xuid2
    )
{
    return (xuid1 == xuid2);
}

XBOXAPI
FORCEINLINE
BOOL
WINAPI
IsTeamXUID(
    XUID xuid
    )
{
    return (xuid & 0xFF00000000000000) == 0xFE00000000000000;
}


XBOXAPI
DWORD
WINAPI
XGetLanguage(
    VOID
    );


typedef struct _XVIDEO_MODE {
    DWORD                               dwDisplayWidth;
    DWORD                               dwDisplayHeight;
    BOOL                                fIsInterlaced;
    BOOL                                fIsWideScreen;
    BOOL                                fIsHiDef;
    float                               RefreshRate;
    DWORD                               VideoStandard;
    DWORD                               Reserved[5];
} XVIDEO_MODE, *PXVIDEO_MODE;

#define XC_VIDEO_STANDARD_NTSC_M        1
#define XC_VIDEO_STANDARD_NTSC_J        2
#define XC_VIDEO_STANDARD_PAL_I         3

XBOXAPI
VOID
WINAPI
XGetVideoMode(
    OUT     PXVIDEO_MODE                pVideoMode
    );

#define XC_VIDEO_FLAGS_WIDESCREEN       0x00000001
#define XC_VIDEO_FLAGS_HDTV_720p        0x00000002
#define XC_VIDEO_FLAGS_HDTV_480p        0x00000008
#define XC_VIDEO_FLAGS_HDTV_1080i       0x00000004

#define XC_CONSOLE_REGION_NA            0x00
#define XC_CONSOLE_REGION_ASIA          0x01
#define XC_CONSOLE_REGION_EUROPE        0x02
#define XC_CONSOLE_REGION_RESTOFWORLD   0x03
#define XC_CONSOLE_REGION_DEVKIT        0x7F

#define XC_GAME_REGION(region, subregion) \
    ((XC_CONSOLE_REGION_##region << 8) | (subregion))

#define XC_GAME_REGION_NA_ALL           XC_GAME_REGION(NA, 0xFF)
#define XC_GAME_REGION_ASIA_ALL         XC_GAME_REGION(ASIA, 0xFF)
#define XC_GAME_REGION_ASIA_JAPAN       XC_GAME_REGION(ASIA, 0x01)
#define XC_GAME_REGION_ASIA_CHINA       XC_GAME_REGION(ASIA, 0x02)
#define XC_GAME_REGION_ASIA_REST        XC_GAME_REGION(ASIA, 0xFC)
#define XC_GAME_REGION_EUROPE_ALL       XC_GAME_REGION(EUROPE, 0xFF)
#define XC_GAME_REGION_EUROPE_AUNZ      XC_GAME_REGION(EUROPE, 0x01)
#define XC_GAME_REGION_EUROPE_REST      XC_GAME_REGION(EUROPE, 0xFE)
#define XC_GAME_REGION_RESTOFWORLD_ALL  XC_GAME_REGION(RESTOFWORLD, 0xFF)
#define XC_GAME_REGION_DEVKIT_ALL       XC_GAME_REGION(DEVKIT, 0xFF)
#define XC_GAME_REGION_MANUFACTURING    0x8000

#define XC_GAME_REGION_REGION(region) \
    (((region) & 0xFF00) >> 8)

XBOXAPI
DWORD
WINAPI
XGetGameRegion(
    VOID
    );


// Device types available in XINPUT_CAPABILITIES
#define XINPUT_DEVTYPE_GAMEPAD          0x01
#define XINPUT_DEVTYPE_USB_KEYBOARD     0x02

// Device subtypes available in XINPUT_CAPABILITIES
#define XINPUT_DEVSUBTYPE_UNKNOWN       0x00
#define XINPUT_DEVSUBTYPE_GAMEPAD       0x01
#define XINPUT_DEVSUBTYPE_WHEEL         0x10
#define XINPUT_DEVSUBTYPE_ARCADE_STICK  0x20
#define XINPUT_DEVSUBTYPE_FLIGHT_STICK  0x30

// Flags for XINPUT_CAPABILITIES
#define XINPUT_CAPS_FFB_SUPPORTED       0x0001
#define XINPUT_CAPS_WIRELESS            0x0002
#define XINPUT_CAPS_VOICE_SUPPORTED     0x0004
#define XINPUT_CAPS_PMD_SUPPORTED       0x0008

// Constants for gamepad buttons
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

//
// Gamepad thresholds
//

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

//
// Flags used in XINPUT_KEYSTROKE
//

#define XINPUT_KEYSTROKE_KEYDOWN        0x0001
#define XINPUT_KEYSTROKE_KEYUP          0x0002
#define XINPUT_KEYSTROKE_REPEAT         0x0004
// Modifiers
#define XINPUT_KEYSTROKE_SHIFT          0x0008
#define XINPUT_KEYSTROKE_CTRL           0x0010
#define XINPUT_KEYSTROKE_ALT            0x0020
#define XINPUT_KEYSTROKE_ALTGR          0x0040
// State Lock
#define XINPUT_KEYSTROKE_CAPSLOCK       0x0080
#define XINPUT_KEYSTROKE_NUMLOCK        0x0100
// Japanese - State Lock
#define XINPUT_KEYSTROKE_ROMAJI         0x0200
#define XINPUT_KEYSTROKE_HIRAGANA       0x0400
// Korean - State Lock
#define XINPUT_KEYSTROKE_HANGUL         0x0800
// Valid
#define XINPUT_KEYSTROKE_VALIDUNICODE   0x1000
// Key is from IR remote
#define XINPUT_KEYSTROKE_REMOTE         0x8000

//
// Flags to pass to XInputGetKeystroke
//

#define XINPUT_FLAG_GAMEPAD             0x00000001
#define XINPUT_FLAG_KEYBOARD            0x00000002
#define XINPUT_FLAG_REMOTE              0x00000004
#define XINPUT_FLAG_ANYDEVICE           0x000000FF

#define XINPUT_FLAG_ANYUSER             0x40000000


//
// Flags used XINPUT_KEYBOARDSTATE modifiers
//

#define XINPUT_MODIFIER_LCONTROL        0xE0
#define XINPUT_MODIFIER_LSHIFT          0xE1
#define XINPUT_MODIFIER_LALT            0xE2
#define XINPUT_MODIFIER_LGUI            0xE3
#define XINPUT_MODIFIER_RCONTROL        0xE4
#define XINPUT_MODIFIER_RSHIFT          0xE5
#define XINPUT_MODIFIER_RALT            0xE6
#define XINPUT_MODIFIER_RGUI            0xE7

//
// Number of key codes in XINPUT_KEYBOARDSTATE
//

#define XINPUT_MAX_KEYBOARD_STATE       6

//
// Structures used by XInput APIs
//

typedef struct _XINPUT_GAMEPAD
{
    WORD                                wButtons;
    BYTE                                bLeftTrigger;
    BYTE                                bRightTrigger;
    SHORT                               sThumbLX;
    SHORT                               sThumbLY;
    SHORT                               sThumbRX;
    SHORT                               sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
    DWORD                               dwPacketNumber;
    XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

typedef struct _XINPUT_VIBRATION
{
    WORD                                wLeftMotorSpeed;
    WORD                                wRightMotorSpeed;
} XINPUT_VIBRATION, *PXINPUT_VIBRATION;

typedef struct _XINPUT_CAPABILITIES
{
    BYTE                                Type;
    BYTE                                SubType;
    WORD                                Flags;
    XINPUT_GAMEPAD                      Gamepad;
    XINPUT_VIBRATION                    Vibration;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

typedef struct _XINPUT_KEYSTROKE
{
    WORD                                VirtualKey;
    WCHAR                               Unicode;
    WORD                                Flags;
    BYTE                                UserIndex;
    BYTE                                HidCode;
} XINPUT_KEYSTROKE, *PXINPUT_KEYSTROKE;

typedef struct _XINPUT_KEYBOARDSTATE {
    BYTE bModifiers;
    BYTE bReserved;
    BYTE HidCodes[XINPUT_MAX_KEYBOARD_STATE];
    WORD VKCodes[XINPUT_MAX_KEYBOARD_STATE];
} XINPUT_KEYBOARDSTATE, *PXINPUT_KEYBOARDSTATE;



XBOXAPI
DWORD
WINAPI
XInputGetState(
    IN      DWORD                       dwUserIndex,
    OUT     PXINPUT_STATE               pState
    );

XBOXAPI
DWORD
WINAPI
XInputGetCapabilities(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwFlags,
    OUT     PXINPUT_CAPABILITIES        pCapabilities
    );

XBOXAPI
DWORD
WINAPI
XInputSetState(
    IN      DWORD                       dwUserIndex,
    IN      PXINPUT_VIBRATION           pVibration
    );


XBOXAPI
DWORD
WINAPI
XInputGetKeyboardState(
    IN      DWORD                       dwUserIndex,
    OUT     PXINPUT_KEYBOARDSTATE       pKeyboardState
    );


#define XINPUT_LANGUAGE_ENGLISH         1
#define XINPUT_LANGUAGE_ENGLISH_UK      2
#define XINPUT_LANGUAGE_JAPANESE        3
#define XINPUT_LANGUAGE_GERMAN          4
#define XINPUT_LANGUAGE_FRENCH          5
#define XINPUT_LANGUAGE_FRENCH_CANADA   6
#define XINPUT_LANGUAGE_ITALIAN         7
#define XINPUT_LANGUAGE_SPANISH         8
//#define XINPUT_LANGUAGE_CHINESE       9  - there will be two chinese layouts
//#define XINPUT_LANGUAGE_CHINESE       10 - don't know what to call them yet
#define XINPUT_LANGUAGE_KOREAN          11
#define XINPUT_LANGUAGE_SWEDISH         12
#define XINPUT_LANGUAGE_PORTUGUESE_PT   13
#define XINPUT_LANGUAGE_PORTUGUESE_BR   14



XBOXAPI
DWORD
WINAPI
XInputGetKeyboardLanguage(
    IN      DWORD                       dwUserIndex
    );

XBOXAPI
DWORD
WINAPI
XInputGetKeystroke(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwFlags,
    OUT     PXINPUT_KEYSTROKE           pKeystroke
    );

XBOXAPI
DWORD
WINAPI
XInputGetKeystrokeEx(
    IN OUT  PDWORD                      pdwUserIndex,
    IN      DWORD                       dwFlags,
    OUT     PXINPUT_KEYSTROKE           pKeystroke
    );

#if _DEBUG
XBOXAPI
VOID
WINAPI
XInputEnableAutobind(
    IN      BOOL                        fEnable
    );
#else   // _DEBUG
#define XInputEnableAutobind(f)
#endif  // !_DEBUG


#define XINIT_LIMIT_DEVKIT_MEMORY               0x00000004
#define XINIT_DONT_MODIFY_HARD_DISK             0x00000010

XBOXAPI
DWORD
WINAPI
XMountUtilityDrive(
    IN      BOOL                        fFormatClean,
    IN      DWORD                       dwBytesPerCluster,
    IN      SIZE_T                      dwFileCacheSize
    );

XBOXAPI
DWORD
WINAPI
XFlushUtilityDrive(
    VOID
    );


#define MAX_LAUNCH_DATA_SIZE            1020
#define XLDEMO_RUNMODE_KIOSKMODE        0x01
#define XLDEMO_RUNMODE_USERSELECTED     0x02

typedef struct _LD_DEMO  // Required for launching into demos
{
    DWORD dwID;
    DWORD dwRunmode;
    DWORD dwTimeout;
    CHAR  szLauncherXEX[128];
    CHAR  szLaunchedXEX[128];
} LD_DEMO, *PLD_DEMO;

// value of LD_DEMO.dwID
#define LAUNCH_DATA_DEMO_ID 'CDX1'




XBOXAPI
DWORD
WINAPI
XSetLaunchData(
    IN      PVOID                       pLaunchData,
    IN      DWORD                       dwLaunchDataSize
    );

XBOXAPI
DWORD
WINAPI
XGetLaunchDataSize(
    OUT     LPDWORD                     pdwLaunchDataSize
    );

XBOXAPI
DWORD
WINAPI
XGetLaunchData(
    OUT     PVOID                       pBuffer,
    IN      DWORD                       dwBufferSize
    );

DECLSPEC_NORETURN
XBOXAPI
VOID
WINAPI
XLaunchNewImage(
    IN      LPCSTR                      szImagePath,
    IN      DWORD                       dwFlags
    );


#define XLAUNCH_KEYWORD_DASH            NULL
#define XLAUNCH_KEYWORD_DEFAULT_APP     ""
#define XLAUNCH_KEYWORD_DASH_ARCADE     "XLNI_DASH_ARCADE"

typedef struct _XOVERLAPPED             XOVERLAPPED, *PXOVERLAPPED;


XBOXAPI
BOOL
WINAPI
XGetModuleSection(
    IN      HANDLE                      ModuleHandle,
    IN      LPCSTR                      szSectionName,
    OUT     PVOID                       *pSectionData,
    OUT     ULONG                       *pSectionSize
    );


XBOXAPI
DWORD
WINAPI
XSetThreadProcessor(
    IN      HANDLE                      hThread,
    IN      DWORD                       dwProcessor
    );

typedef VOID (WINAPI *XTHREAD_NOTIFY_PROC)(BOOL fCreate);
typedef struct _XTHREAD_NOTIFICATION {
    LIST_ENTRY                          ListEntry;
    XTHREAD_NOTIFY_PROC                 pfnNotifyRoutine;
} XTHREAD_NOTIFICATION, *PXTHREAD_NOTIFICATION;

XBOXAPI
VOID
WINAPI
XRegisterThreadNotifyRoutine(
    IN OUT  PXTHREAD_NOTIFICATION       pThreadNotification,
    IN      BOOL                        fRegister
    );

XBOXAPI
VOID
WINAPI
XSetProcessQuantumLength(
    IN      DWORD                       dwMilliseconds
    );

XBOXAPI
DWORD
WINAPI
XGetProcessQuantumLength(
    VOID
    );

XBOXAPI
BOOL
WINAPI
XSetFileCacheSize(
    IN      SIZE_T                      dwCacheSize
    );

XBOXAPI
SIZE_T
WINAPI
XGetFileCacheSize(
    VOID
    );


XBOXAPI
VOID
WINAPI
XSaveFloatingPointStateForDpc(
    VOID
    );

XBOXAPI
VOID
WINAPI
XRestoreFloatingPointStateForDpc(
    VOID
    );

#define XLOCKL2_INDEX_XPS                       0
#define XLOCKL2_INDEX_TITLE                     1

#define XLOCKL2_LOCK_SIZE_1_WAY                 (128*1024)
#define XLOCKL2_LOCK_SIZE_2_WAYS                (256*1024)

#define XLOCKL2_FLAG_SUSPEND_REPLACEMENT        0x00000001
#define XLOCKL2_FLAG_NONEXCLUSIVE_REPLACEMENT   0X00000002

XBOXAPI
BOOL
WINAPI
XLockL2(
    IN      DWORD                       dwIndex,
    IN      CONST PVOID                 pRangeStart,
    IN      DWORD                       dwRangeSize,
    IN      DWORD                       dwLockSize,
    IN      DWORD                       dwFlags
    );

VOID
WINAPI
XUnlockL2(
    IN      DWORD                       dwIndex
    );

XBOXAPI
LPVOID
WINAPI
XPhysicalAlloc(
    IN      SIZE_T                      dwSize,
    IN      ULONG_PTR                   ulPhysicalAddress,
    IN      ULONG_PTR                   ulAlignment,
    IN      DWORD                       flProtect
    );

XBOXAPI
LPVOID
WINAPI
XPhysicalAllocEx(
    IN      SIZE_T                      dwSize,
    IN      ULONG_PTR                   ulLowestAcceptableAddress,
    IN      ULONG_PTR                   ulHighestAcceptableAddress,
    IN      ULONG_PTR                   ulAlignment,
    IN      DWORD                       flProtect
    );

XBOXAPI
SIZE_T
WINAPI
XPhysicalSize(
    IN      LPVOID                      lpAddress
    );

XBOXAPI
VOID
WINAPI
XPhysicalProtect(
    IN      LPVOID                      lpAddress,
    IN      SIZE_T                      dwSize,
    IN      DWORD                       flNewProtect
    );

XBOXAPI
VOID
WINAPI
XPhysicalFree(
    IN      LPVOID                      lpAddress
    );

XBOXAPI
DWORD
WINAPI
XQueryMemoryProtect(
    IN      LPVOID                      lpAddress
    );

XBOXAPI
VOID
WINAPI
XEnableSmallPagesOverride(
    IN      BOOL                        fEnable
    );

XBOXAPI
BOOL
WINAPI
XGetSmallPagesOverride(
    );


// allocation attribute definitions for XMemAlloc and XMemFree APIs

#define XALLOC_MEMTYPE_HEAP                         0
#define XALLOC_MEMTYPE_PHYSICAL                     1

#define XALLOC_MEMPROTECT_WRITECOMBINE_LARGE_PAGES  0
#define XALLOC_MEMPROTECT_NOCACHE                   1
#define XALLOC_MEMPROTECT_READWRITE                 2
#define XALLOC_MEMPROTECT_WRITECOMBINE              3

#define XALLOC_ALIGNMENT_DEFAULT                    0x0
#define XALLOC_ALIGNMENT_4                          0x1
#define XALLOC_ALIGNMENT_8                          0x2
#define XALLOC_ALIGNMENT_16                         0x4

#define XALLOC_PHYSICAL_ALIGNMENT_DEFAULT           0x0 // Default is 4K alignment
#define XALLOC_PHYSICAL_ALIGNMENT_4                 0x2
#define XALLOC_PHYSICAL_ALIGNMENT_8                 0x3
#define XALLOC_PHYSICAL_ALIGNMENT_16                0x4
#define XALLOC_PHYSICAL_ALIGNMENT_32                0x5
#define XALLOC_PHYSICAL_ALIGNMENT_64                0x6
#define XALLOC_PHYSICAL_ALIGNMENT_128               0x7
#define XALLOC_PHYSICAL_ALIGNMENT_256               0x8
#define XALLOC_PHYSICAL_ALIGNMENT_512               0x9
#define XALLOC_PHYSICAL_ALIGNMENT_1K                0xA
#define XALLOC_PHYSICAL_ALIGNMENT_2K                0xB
#define XALLOC_PHYSICAL_ALIGNMENT_4K                0xC
#define XALLOC_PHYSICAL_ALIGNMENT_8K                0xD
#define XALLOC_PHYSICAL_ALIGNMENT_16K               0xE
#define XALLOC_PHYSICAL_ALIGNMENT_32K               0xF

typedef enum _XALLOC_ALLOCATOR_IDS {
    eXALLOCAllocatorId_GameMin                  = 0,
    eXALLOCAllocatorId_GameMax                  = 127,
    eXALLOCAllocatorId_MsReservedMin            = 128,
    eXALLOCAllocatorId_D3D                      = 128,
    eXALLOCAllocatorId_D3DX,
    eXALLOCAllocatorId_XAUDIO,
    eXALLOCAllocatorId_XAPI,
    eXALLOCAllocatorId_XACT,
    eXALLOCAllocatorId_XBOXKERNEL,
    eXALLOCAllocatorId_XBDM,
    eXALLOCAllocatorId_XGRAPHICS,
    eXALLOCAllocatorId_XONLINE,
    eXALLOCAllocatorId_XVOICE,
    eXALLOCAllocatorId_XHV,
    eXALLOCAllocatorId_USB,
    eXALLOCAllocatorId_XMV,
    eXALLOCAllocatorId_SHADERCOMPILER,
    eXALLOCAllocatorId_XUI,
    eXALLOCAllocatorId_XASYNC,
    eXALLOCAllocatorId_XCAM,
    eXALLOCAllocatorId_MsReservedMax            = 191,
    eXALLOCAllocatorId_MiddlewareReservedMin    = 192,
    eXALLOCAllocatorId_MiddlewareReservedMax    = 255
} XALLOC_ALLOCATOR_IDS;

// The PPC compiler defines bitfields to be ordered from the MSB to the LSB,
// which is opposite the convention on the X86 platform.  Use the 'bitfield_order'
// pragma to switch the ordering.  This does not affect endianness in any way.

#if defined(_M_PPCBE)
#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
#endif

typedef struct _XALLOC_ATTRIBUTES {
    DWORD                               dwObjectType:13;
    DWORD                               dwHeapTracksAttributes:1;
    DWORD                               dwMustSucceed:1;
    DWORD                               dwFixedSize:1;
    DWORD                               dwAllocatorId:8;
    DWORD                               dwAlignment:4;
    DWORD                               dwMemoryProtect:2;
    DWORD                               dwZeroInitialize:1;
    DWORD                               dwMemoryType:1;
} XALLOC_ATTRIBUTES, *PXALLOC_ATTRIBUTES;

#if defined(_M_PPCBE)
#pragma bitfield_order(pop)
#endif

#define MAKE_XALLOC_ATTRIBUTES(ObjectType,\
                               HeapTracksAttributes,\
                               MustSucceed,\
                               FixedSize,\
                               AllocatorId,\
                               Alignment,\
                               MemoryProtect,\
                               ZeroInitialize,\
                               MemoryType)\
    ((DWORD)( ObjectType | \
             (HeapTracksAttributes << 13) | \
             (MustSucceed << 14) | \
             (FixedSize << 15) | \
             (AllocatorId << 16) | \
             (Alignment << 24) | \
             (MemoryProtect << 28) | \
             (ZeroInitialize << 30) | \
             (MemoryType << 31)))

#define XALLOC_IS_PHYSICAL(Attributes)  ((BOOL)(Attributes & 0x80000000)!=0)

XBOXAPI
LPVOID
WINAPI
XMemAlloc(
    IN      SIZE_T                      dwSize,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
LPVOID
WINAPI
XMemAllocDefault(
    IN      SIZE_T                      dwSize,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
VOID
WINAPI
XMemFree(
    IN OUT  PVOID                       pAddress,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
VOID
WINAPI
XMemFreeDefault(
    IN OUT  PVOID                       pAddress,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
SIZE_T
WINAPI
XMemSize(
    IN      PVOID                       pAddress,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
SIZE_T
WINAPI
XMemSizeDefault(
    IN      PVOID                       pAddress,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
VOID
WINAPI
XSetAttributesOnHeapAlloc(
    IN      PVOID                       pBaseAddress,
    IN      DWORD                       dwAllocAttributes
    );

XBOXAPI
DWORD
WINAPI
XGetAttributesOnHeapAlloc(
    IN      PVOID                       pBaseAddress
    );

XBOXAPI
PVOID
WINAPI
XMemSet128(
    OUT     PVOID                       pDest,
    IN      INT                         c,
    IN      SIZE_T                      count
    );

XBOXAPI
PVOID
WINAPI
XMemSet(
    OUT     PVOID                       pDest,
    IN      INT                         c,
    IN      SIZE_T                      count
    );

XBOXAPI
PVOID
WINAPI
XMemCpy128(
    OUT     PVOID                       pDest,
    IN      CONST VOID*                 pSrc,
    IN      SIZE_T                      count
    );

XBOXAPI
PVOID
WINAPI
XMemCpy(
    OUT     PVOID                       pDest,
    IN      CONST VOID*                 pSrc,
    IN      SIZE_T                      count
    );

XBOXAPI
PVOID
WINAPI
XMemCpyExtended128(
    OUT     PVOID                       pDest,
    IN      CONST VOID*                 pSrc,
    IN      SIZE_T                      count
    );

XBOXAPI
PVOID
WINAPI
XMemCpyExtended(
    OUT     PVOID                       pDest,
    IN      CONST VOID*                 pSrc,
    IN      SIZE_T                      count
    );

XBOXAPI
DWORD
WINAPI
XMemGetPageSize(
    const VOID* address
    );

#ifdef _DEBUG

#define XVER_DEVKIT                     1
#define XVER_RETAIL                     2

XBOXAPI
DWORD
WINAPI
XDebugGetSystemVersionA(
    OUT     LPSTR                       szVersionString,
    IN      UINT                        cchVersionString
    );
#define XDebugGetSystemVersion          XDebugGetSystemVersionA

XBOXAPI
DWORD
WINAPI
XDebugGetXTLVersionA(
    OUT     LPSTR                       szVersionString,
    IN      UINT                        cchVersionString
    );
#define XDebugGetXTLVersion             XDebugGetXTLVersionA

#endif  // _DEBUG

#define XCALCSIG_SIGNATURE_SIZE         20


// Xbox-specific Overlapped

typedef
VOID
(WINAPI *PXOVERLAPPED_COMPLETION_ROUTINE)(
    IN      DWORD                       dwErrorCode,
    IN      DWORD                       dwNumberOfBytesTransfered,
    IN OUT  PXOVERLAPPED                pOverlapped
    );


typedef struct _XOVERLAPPED {
    ULONG_PTR                           InternalLow;
    ULONG_PTR                           InternalHigh;
    ULONG_PTR                           InternalContext;
    HANDLE                              hEvent;
    PXOVERLAPPED_COMPLETION_ROUTINE     pCompletionRoutine;
    DWORD_PTR                           dwCompletionContext;
    DWORD                               dwExtendedError;
} XOVERLAPPED, *PXOVERLAPPED;

#define XHasOverlappedIoCompleted(lpOverlapped) \
    (*((volatile ULONG_PTR*)(&(lpOverlapped)->InternalLow)) != ERROR_IO_PENDING)

XBOXAPI
DWORD
WINAPI
XGetOverlappedResult(
    IN      PXOVERLAPPED                pOverlapped,
    OUT     LPDWORD                     pdwResult           OPTIONAL,
    IN      BOOL                        bWait
    );

XBOXAPI
DWORD
WINAPI
XGetOverlappedExtendedError(
    IN      PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XCancelOverlapped(
    IN      PXOVERLAPPED                pOverlapped
    );

// XContent

// Content types
#define XCONTENTTYPE_SAVEDGAME          0x00000001
#define XCONTENTTYPE_MARKETPLACE        0x00000002
#define XCONTENTTYPE_PUBLISHER          0x00000003

//  Content creation/open flags
#define XCONTENTFLAG_NONE               0x00000000
#define XCONTENTFLAG_CREATENEW          CREATE_NEW
#define XCONTENTFLAG_CREATEALWAYS       CREATE_ALWAYS
#define XCONTENTFLAG_OPENEXISTING       OPEN_EXISTING
#define XCONTENTFLAG_OPENALWAYS         OPEN_ALWAYS
#define XCONTENTFLAG_TRUNCATEEXISTING   TRUNCATE_EXISTING

//  Content attributes
#define XCONTENTFLAG_NOPROFILE_TRANSFER 0x00000010
#define XCONTENTFLAG_NODEVICE_TRANSFER  0x00000020
#define XCONTENTFLAG_STRONG_SIGNED      0x00000040

//  Device selector flags
#define XCONTENTFLAG_MANAGESTORAGE      0x00000100
#define XCONTENTFLAG_FORCE_SHOW_UI      0x00000200

//  Enumeration scoping
#define XCONTENTFLAG_ENUM_EXCLUDECOMMON 0x00001000


#define XCONTENT_MAX_DISPLAYNAME_LENGTH 128
#define XCONTENT_MAX_FILENAME_LENGTH    42
#define XCONTENTDEVICE_MAX_NAME_LENGTH  27

typedef DWORD                           XCONTENTDEVICEID, *PXCONTENTDEVICEID;

#define XCONTENTDEVICE_ANY              ((XCONTENTDEVICEID)0)

typedef struct _XCONTENT_DATA
{
    XCONTENTDEVICEID                    DeviceID;
    DWORD                               dwContentType;
    WCHAR                               szDisplayName[XCONTENT_MAX_DISPLAYNAME_LENGTH];
    CHAR                                szFileName[XCONTENT_MAX_FILENAME_LENGTH];
} XCONTENT_DATA, *PXCONTENT_DATA;


typedef DWORD                           XCONTENTDEVICETYPE, *PXCONTENTDEVICETYPE;

// Device types
#define XCONTENTDEVICETYPE_HDD          ((XCONTENTDEVICETYPE)1)
#define XCONTENTDEVICETYPE_MU           ((XCONTENTDEVICETYPE)2)

typedef struct _XDEVICE_DATA
{
    XCONTENTDEVICEID                    DeviceID;
    XCONTENTDEVICETYPE                  DeviceType;
    ULONGLONG                           ulDeviceBytes;
    ULONGLONG                           ulDeviceFreeBytes;
    WCHAR                               wszFriendlyName[XCONTENTDEVICE_MAX_NAME_LENGTH];
} XDEVICE_DATA, *PXDEVICE_DATA;


#define XCONTENT_NONE                   0x00000000
#define XCONTENT_CREATED_NEW            0x00000001
#define XCONTENT_OPENED_EXISTING        0x00000002

XBOXAPI
DWORD
WINAPI
XContentCreate(
    IN      DWORD                       dwUserIndex,
    IN      LPCSTR                      szRootName,
    IN      CONST XCONTENT_DATA*        pContentData,
    IN      DWORD                       dwContentFlags,
    OUT     PDWORD                      pdwDisposition      OPTIONAL,
    OUT     PDWORD                      pdwLicenseMask      OPTIONAL,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentCreateEx(
    IN      DWORD                       dwUserIndex,
    IN      LPCSTR                      szRootName,
    IN      CONST XCONTENT_DATA*        pContentData,
    IN      DWORD                       dwContentFlags,
    OUT     PDWORD                      pdwDisposition      OPTIONAL,
    OUT     PDWORD                      pdwLicenseMask      OPTIONAL,
    IN      SIZE_T                      dwFileCacheSize,
    IN      ULARGE_INTEGER              uliContentSize,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentDelete(
    IN      DWORD                       dwUserIndex         OPTIONAL,
    IN      CONST XCONTENT_DATA*        pContentData,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentInstall(
    IN      DWORD                       dwUserIndex,
    IN      XCONTENTDEVICEID            DeviceID,
    IN      LPCSTR                      pszFileName,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentClose(
    IN      LPCSTR                      szRootName,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentFlush(
    IN      LPCSTR                      szRootName,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentSetThumbnail(
    IN      DWORD                       dwUserIndex,
    IN      CONST XCONTENT_DATA*        pContentData,
    IN      CONST BYTE*                 pbThumbnail,
    IN      DWORD                       cbThumbnail,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentGetThumbnail(
    IN      DWORD                       dwUserIndex,
    IN      CONST XCONTENT_DATA*        pContentData,
    OUT     PBYTE                       pbThumbnail,        OPTIONAL
    IN OUT  PDWORD                      pcbThumbnail,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentGetCreator(
    IN      DWORD                       dwUserIndex,
    IN      CONST XCONTENT_DATA*        pContentData,
    OUT     PBOOL                       pfUserIsCreator,
    OUT     PXUID                       pxuid,              OPTIONAL
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentGetLicenseMask(
    OUT     PDWORD                      pdwLicenseMask,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XContentLaunchImage(
    IN      DWORD                       dwUserIndex,
    IN      CONST XCONTENT_DATA*        pContentData,
    IN      LPCSTR                      szImagePath
    );

XBOXAPI
DWORD
WINAPI
XContentGetDeviceState(
    IN      XCONTENTDEVICEID            DeviceID,
    IN OUT  PXOVERLAPPED                pOverlapped
    );

XBOXAPI
DWORD
WINAPI
XContentGetDeviceStateByHandle(
    IN      CONST XCONTENT_DATA*        pContentData,
    IN OUT  PXOVERLAPPED                pOverlapped
    );

XBOXAPI
DWORD
WINAPI
XContentGetDeviceData(
    IN      XCONTENTDEVICEID            DeviceID,
    OUT     PXDEVICE_DATA               pDeviceData
    );

XBOXAPI
DWORD
WINAPI
XContentGetDeviceName(
    IN      XCONTENTDEVICEID            DeviceID,
    OUT     LPWSTR                      wszName,
    IN      DWORD                       cchName
    );


XBOXAPI
DWORD
WINAPI
XContentCreateEnumerator(
    IN      DWORD                       dwUserIndex,
    IN      XCONTENTDEVICEID            DeviceID,
    IN      DWORD                       dwContentType,
    IN      DWORD                       dwContentFlags,
    IN      DWORD                       cItem,
    OUT     PDWORD                      pcbBuffer           OPTIONAL,
    OUT     PHANDLE                     phEnum
    );

typedef struct _XOFFERING_CONTENTAVAILABLE_RESULT
{
    DWORD                               dwNewOffers;
    DWORD                               dwTotalOffers;
} XOFFERING_CONTENTAVAILABLE_RESULT;

XBOXAPI
DWORD
WINAPI
XContentGetMarketplaceCounts(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwContentCategories,
    IN      DWORD                       cbResults,
    IN OUT  XOFFERING_CONTENTAVAILABLE_RESULT   *pResults,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );


XBOXAPI
DWORD
WINAPI
XEnumerate(
    IN      HANDLE                      hEnum,
    IN OUT  PVOID                       pvBuffer,
    IN      DWORD                       cbBuffer,
    OUT     PDWORD                      pcItemsReturned     OPTIONAL,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XEnumerateBack(
    IN      HANDLE                      hEnum,
    IN OUT  PVOID                       pvBuffer,
    IN      DWORD                       cbBuffer,
    OUT     PDWORD                      pcItemsReturned     OPTIONAL,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );


//-----------------------------------------------------------------------------
// Game instrumentation errors              = 0x8056F0XX
//-----------------------------------------------------------------------------

#define SPA_E_CORRUPT_FILE          _HRESULT_TYPEDEF_(0x8056F001)
#define SPA_E_NOT_LOADED            _HRESULT_TYPEDEF_(0x8056F002)
#define SPA_E_BAD_TITLEID           _HRESULT_TYPEDEF_(0x8056F003)


// User/Profile/Account APIs

#define XUSER_MAX_COUNT                 4

#define XUSER_INDEX_FOCUS               0x000000FD
#define XUSER_INDEX_NONE                0x000000FE
#define XUSER_INDEX_ANY                 0x000000FF

typedef enum _XUSER_SIGNIN_STATE
{
    eXUserSigninState_NotSignedIn,
    eXUserSigninState_SignedInLocally,
    eXUserSigninState_SignedInToLive
} XUSER_SIGNIN_STATE;

XBOXAPI
DWORD
WINAPI
XUserGetXUID(
    IN      DWORD                       dwUserIndex,
    OUT     PXUID                       pxuid
    );

XBOXAPI
DWORD
WINAPI
XUserGetName(
    IN      DWORD                       dwUserIndex,
    OUT     LPSTR                       szUserName,
    IN      DWORD                       cchUserName
    );

XBOXAPI
XUSER_SIGNIN_STATE
WINAPI
XUserGetSigninState(
    IN      DWORD                       dwUserIndex
    );

typedef enum _XPRIVILEGE_TYPE
{
    XPRIVILEGE_MULTIPLAYER_SESSIONS              = 254, // on|off

    XPRIVILEGE_COMMUNICATIONS                    = 252, // on (communicate w/everyone) | off (check _FO)
    XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY       = 251, // on (communicate w/friends only) | off (blocked)

    XPRIVILEGE_PROFILE_VIEWING                   = 249, // on (viewing allowed) | off (check _FO)
    XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY      = 248, // on (view friend’s only) | off (no details)

    XPRIVILEGE_USER_CREATED_CONTENT              = 247, // on (allow viewing of UCC) | off (check _FO)
    XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY = 246, // on (view UCC from friends only) | off (blocked)

    XPRIVILEGE_PURCHASE_CONTENT                  = 245, // on (allow purchase) | off (blocked)

    XPRIVILEGE_PRESENCE                          = 244, // on (share presence info) | off (check _FO)
    XPRIVILEGE_PRESENCE_FRIENDS_ONLY             = 243, // on (share w/friends only | off (don’t share)

} XPRIVILEGE_TYPE;


XBOXAPI
DWORD
WINAPI
XUserAreUsersFriends(
    IN      DWORD                       dwUserIndex,
    IN      PXUID                       pXuids,
    IN      DWORD                       dwXuidCount,
    OUT     PBOOL                       pfResult            OPTIONAL,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XUserCheckPrivilege(
    IN      DWORD                       dwUserIndex,
    IN      XPRIVILEGE_TYPE             PrivilegeType,
    OUT     PBOOL                       pfResult
    );

// Context and Property definitions

typedef struct _XUSER_DATA
{
    BYTE                                type;

    union
    {
        LONG                            nData;     // XUSER_DATA_TYPE_INT32
        LONGLONG                        i64Data;   // XUSER_DATA_TYPE_INT64
        double                          dblData;   // XUSER_DATA_TYPE_DOUBLE
        struct                                     // XUSER_DATA_TYPE_UNICODE
        {
            DWORD                       cbData;    // Includes null-terminator
            LPCWSTR                     pwszData;
        } string;
        FLOAT                           fData;     // XUSER_DATA_TYPE_FLOAT
        struct                                     // XUSER_DATA_TYPE_BINARY
        {
            DWORD                       cbData;
            PBYTE                       pbData;
        } binary;
        FILETIME                        ftData;    // XUSER_DATA_TYPE_DATETIME
    };
} XUSER_DATA, *PXUSER_DATA;

typedef struct _XUSER_PROPERTY
{
    DWORD                               dwPropertyId;
    XUSER_DATA                          value;
} XUSER_PROPERTY, *PXUSER_PROPERTY;

typedef struct _XUSER_CONTEXT
{
    DWORD                               dwContextId;
    DWORD                               dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;

// Context and Property APIs

XBOXAPI
VOID
WINAPI
XUserSetContext(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwContextId,
    IN      DWORD                       dwContextValue
    );

XBOXAPI
VOID
WINAPI
XUserSetProperty(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwPropertyId,
    IN      DWORD                       cbValue,
    IN      CONST VOID*                 pvValue
    );

// System-defined contexts and properties

#define X_PROPERTY_TYPE_MASK            0xF0000000
#define X_PROPERTY_SCOPE_MASK           0x00008000
#define X_PROPERTY_ID_MASK              0x00007FFF


#define XPROPERTYID(global, type, id)   ((global ? X_PROPERTY_SCOPE_MASK : 0) | ((type << 28) & X_PROPERTY_TYPE_MASK) | (id & X_PROPERTY_ID_MASK))
#define XCONTEXTID(global, id)          XPROPERTYID(global, XUSER_DATA_TYPE_CONTEXT, id)
#define XPROPERTYTYPEFROMID(id)         ((id >> 28) & 0xf)
#define XISSYSTEMPROPERTY(id)           (id & X_PROPERTY_SCOPE_MASK)

// Predefined contexts
#define X_CONTEXT_PRESENCE              XCONTEXTID(1, 0x1)
#define X_CONTEXT_GAME_TYPE             XCONTEXTID(1, 0xA)
#define X_CONTEXT_GAME_MODE             XCONTEXTID(1, 0xB)
#define X_CONTEXT_SESSION_JOINABLE      XCONTEXTID(1, 0xC)

#define X_PROPERTY_RANK                 XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0x1)
#define X_PROPERTY_GAMERNAME            XPROPERTYID(1, XUSER_DATA_TYPE_UNICODE, 0x2)
#define X_PROPERTY_SESSION_ID           XPROPERTYID(1, XUSER_DATA_TYPE_INT64,   0x3)

// System attributes used in matchmaking queries (These are not really properties)
#define X_PROPERTY_GAMER_ZONE           XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0x101)
#define X_PROPERTY_GAMER_COUNTRY        XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0x102)
#define X_PROPERTY_GAMER_LANGUAGE       XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0x103)
#define X_PROPERTY_GAMER_RATING         XPROPERTYID(1, XUSER_DATA_TYPE_FLOAT,   0x104)
#define X_PROPERTY_GAMER_MU             XPROPERTYID(1, XUSER_DATA_TYPE_DOUBLE,  0x105)
#define X_PROPERTY_GAMER_SIGMA          XPROPERTYID(1, XUSER_DATA_TYPE_DOUBLE,  0x106)
#define X_PROPERTY_GAMER_PUID           XPROPERTYID(1, XUSER_DATA_TYPE_INT64,   0x107)
#define X_PROPERTY_AFFILIATE_SCORE      XPROPERTYID(1, XUSER_DATA_TYPE_INT64,   0x108)
#define X_PROPERTY_GAMER_HOSTNAME       XPROPERTYID(1, XUSER_DATA_TYPE_UNICODE, 0x109)


// Properties used to write to skill leaderboards
#define X_PROPERTY_RELATIVE_SCORE       XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0xA)
#define X_PROPERTY_SESSION_TEAM         XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   0xB)

// Values for X_CONTEXT_GAME_TYPE
#define X_CONTEXT_GAME_TYPE_RANKED      0
#define X_CONTEXT_GAME_TYPE_STANDARD    1

// Achievement APIs

typedef struct
{
    DWORD                               dwUserIndex;
    DWORD                               dwAchievementId;
} XUSER_ACHIEVEMENT, *PXUSER_ACHIEVEMENT;

XBOXAPI
DWORD
WINAPI
XUserWriteAchievements(
    IN      DWORD                       dwNumAchievements,
    IN      CONST XUSER_ACHIEVEMENT*    pAchievements,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

typedef struct
{
    DWORD                               dwId;
    LPWSTR                              pwszLabel;
    LPWSTR                              pwszDescription;
    LPWSTR                              pwszUnachieved;
    DWORD                               dwImageId;
    DWORD                               dwCred;
    FILETIME                            ftAchieved;
    DWORD                               dwFlags;
} XACHIEVEMENT_DETAILS, *PXACHIEVEMENT_DETAILS;

// These lengths include the NULL-terminator
#define XACHIEVEMENT_MAX_LABEL_LENGTH   32
#define XACHIEVEMENT_MAX_DESC_LENGTH    100
#define XACHIEVEMENT_MAX_UNACH_LENGTH   100

#define XACHIEVEMENT_SIZE_BASE          (sizeof(XACHIEVEMENT_DETAILS))
#define XACHIEVEMENT_SIZE_STRINGS       (sizeof(WCHAR) * (XACHIEVEMENT_MAX_LABEL_LENGTH  + XACHIEVEMENT_MAX_DESC_LENGTH + XACHIEVEMENT_MAX_UNACH_LENGTH))
#define XACHIEVEMENT_SIZE_FULL          (XACHIEVEMENT_SIZE_BASE + XACHIEVEMENT_SIZE_STRINGS)

#define XACHIEVEMENT_INVALID_ID         ((DWORD)0xFFFFFFFF)

// XACHIEVEMENT_DETAILS::dwFlags can be manipulated with these defines and macros
#define XACHIEVEMENT_DETAILS_MASK_TYPE          0x00000007
#define XACHIEVEMENT_DETAILS_SHOWUNACHIEVED     0x00000008
#define XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE    0x00010000
#define XACHIEVEMENT_DETAILS_ACHIEVED           0x00020000

#define AchievementType(dwFlags)           (dwFlags & XACHIEVEMENT_DETAILS_MASK_TYPE)
#define AchievementShowUnachieved(dwFlags) (dwFlags & XACHIEVEMENT_DETAILS_SHOWUNACHIEVED ? TRUE : FALSE)
#define AchievementEarnedOnline(dwFlags)   (dwFlags & XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE ? TRUE : FALSE)
#define AchievementEarned(dwFlags)         (dwFlags & XACHIEVEMENT_DETAILS_ACHIEVED ? TRUE : FALSE)

// Types returned from AchievementType macro

#define XACHIEVEMENT_TYPE_COMPLETION            1
#define XACHIEVEMENT_TYPE_LEVELING              2
#define XACHIEVEMENT_TYPE_UNLOCK                3
#define XACHIEVEMENT_TYPE_EVENT                 4
#define XACHIEVEMENT_TYPE_TOURNAMENT            5
#define XACHIEVEMENT_TYPE_CHECKPOINT            6
#define XACHIEVEMENT_TYPE_OTHER                 7

#define XACHIEVEMENT_DETAILS_ALL                0xFFFFFFFF
#define XACHIEVEMENT_DETAILS_LABEL              0x00000001
#define XACHIEVEMENT_DETAILS_DESCRIPTION        0x00000002
#define XACHIEVEMENT_DETAILS_UNACHIEVED         0x00000004
#define XACHIEVEMENT_DETAILS_TFC                0x00000020

XBOXAPI
DWORD
WINAPI
XUserCreateAchievementEnumerator(
    IN      DWORD                       dwTitleId,
    IN      DWORD                       dwUserIndex,
    IN      XUID                        xuid,
    IN      DWORD                       dwDetailFlags,
    IN      DWORD                       dwStartingIndex,
    IN      DWORD                       cItem,
    OUT     PDWORD                      pcbBuffer,
    OUT     PHANDLE                     ph
    );

//
// Pictures
//

XBOXAPI
DWORD
WINAPI
XUserReadAchievementPicture(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwTitleId,
    IN      DWORD                       dwImageId,
    IN OUT  PBYTE                       pbTextureBuffer,
    IN      DWORD                       dwPitch,
    IN      DWORD                       dwHeight,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );



XBOXAPI
DWORD
WINAPI
XUserReadGamerPicture(
    IN      DWORD                       dwUserIndex,
    IN      BOOL                        fSmall,
    IN OUT  PBYTE                       pbTextureBuffer,
    IN      DWORD                       dwPitch,
    IN      DWORD                       dwHeight,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );


XBOXAPI
DWORD
WINAPI
XUserReadGamerPictureByKey(
    IN      CONST PXUSER_DATA           pGamercardPictureKey,
    IN      BOOL                        fSmall,
    IN OUT  PBYTE                       pbTextureBuffer,
    IN      DWORD                       dwPitch,
    IN      DWORD                       dwHeight,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );


XBOXAPI
DWORD
WINAPI
XUserAwardGamerPicture(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwPictureId,
    IN      DWORD                       dwReserved,
    IN OUT  PXOVERLAPPED                pXOverlapped        OPTIONAL
    );


// Stats APIs

#define XUSER_STATS_ATTRS_IN_SPEC       64

typedef struct _XUSER_STATS_COLUMN
{
    WORD                                wColumnId;
    XUSER_DATA                          Value;
} XUSER_STATS_COLUMN, *PXUSER_STATS_COLUMN;

typedef struct _XUSER_STATS_ROW
{
    XUID                                xuid;
    DWORD                               dwRank;
    LONGLONG                            i64Rating;
    CHAR                                szGamertag[XUSER_NAME_SIZE];
    DWORD                               dwNumColumns;
    PXUSER_STATS_COLUMN                 pColumns;
} XUSER_STATS_ROW, *PXUSER_STATS_ROW;

typedef struct _XUSER_STATS_VIEW
{
    DWORD                               dwViewId;
    DWORD                               dwTotalViewRows;
    DWORD                               dwNumRows;
    PXUSER_STATS_ROW                    pRows;
} XUSER_STATS_VIEW, *PXUSER_STATS_VIEW;

typedef struct _XUSER_STATS_READ_RESULTS
{
    DWORD                               dwNumViews;
    PXUSER_STATS_VIEW                   pViews;
} XUSER_STATS_READ_RESULTS, *PXUSER_STATS_READ_RESULTS;

typedef struct _XUSER_STATS_SPEC
{
    DWORD                               dwViewId;
    DWORD                               dwNumColumnIds;
    WORD                                rgwColumnIds[XUSER_STATS_ATTRS_IN_SPEC];
} XUSER_STATS_SPEC, *PXUSER_STATS_SPEC;

XBOXAPI
DWORD
WINAPI
XUserResetStatsView(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwViewId,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XUserResetStatsViewAllUsers(
    IN      DWORD                       dwViewId,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XUserReadStats(
    IN      DWORD                       dwTitleId,
    IN      DWORD                       dwNumXuids,
    IN      CONST XUID*                 pXuids,
    IN      DWORD                       dwNumStatsSpecs,
    IN      CONST XUSER_STATS_SPEC*     pSpecs,
    IN OUT  PDWORD                      pcbResults,
    OUT     PXUSER_STATS_READ_RESULTS   pResults,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );


XBOXAPI
DWORD
WINAPI
XUserCreateStatsEnumeratorByRank(
    IN      DWORD                       dwTitleId,
    IN      DWORD                       dwRankStart,
    IN      DWORD                       dwNumRows,
    IN      DWORD                       dwNumStatsSpecs,
    IN      CONST XUSER_STATS_SPEC*     pSpecs,
    OUT     PDWORD                      pcbBuffer,
    OUT     PHANDLE                     ph
    );

XBOXAPI
DWORD
WINAPI
XUserCreateStatsEnumeratorByXuid(
    IN      DWORD                       dwTitleId,
    IN      XUID                        XuidPivot,
    IN      DWORD                       dwNumRows,
    IN      DWORD                       dwNumStatsSpecs,
    IN      CONST XUSER_STATS_SPEC*     pSpecs,
    OUT     PDWORD                      pcbBuffer,
    OUT     PHANDLE                     ph
    );


XBOXAPI
DWORD
WINAPI
XUserCreateStatsEnumeratorByRating(
    IN      DWORD                       dwTitleId,
    IN      LONGLONG                    i64Rating,
    IN      DWORD                       dwNumRows,
    IN      DWORD                       dwNumStatsSpecs,
    IN      CONST XUSER_STATS_SPEC*     pSpecs,
    OUT     PDWORD                      pcbBuffer,
    OUT     PHANDLE                     ph
    );


// Column ids for skill leaderboards (STATS_VIEW_SKILL_* views)
#define X_STATS_COLUMN_SKILL_SKILL              61
#define X_STATS_COLUMN_SKILL_GAMESPLAYED        62
#define X_STATS_COLUMN_SKILL_MU                 63
#define X_STATS_COLUMN_SKILL_SIGMA              64


// Signin UI API

// XShowSiginUI flags.
#define XSSUI_FLAGS_LOCALSIGNINONLY                 0x00000001
#define XSSUI_FLAGS_SHOWONLYONLINEENABLED           0x00000002


XBOXAPI
DWORD
WINAPI
XShowSigninUI(
    IN      DWORD                       cPanes,
    IN      DWORD                       dwFlags
    );


XBOXAPI
DWORD
WINAPI
XShowFriendsUI(
    IN      DWORD                       dwUserIndex
    );

XBOXAPI
DWORD
WINAPI
XShowPlayersUI(
    IN      DWORD                       dwUserIndex
    );

XBOXAPI
DWORD
WINAPI
XShowMessagesUI(
    IN      DWORD                       dwUserIndex
    );

XBOXAPI
DWORD
WINAPI
XShowMessageComposeUI(
    IN      DWORD                       dwUserIndex,
    IN      CONST XUID*                 pXuidRecipients     OPTIONAL,
    IN      DWORD                       cRecipients,
    IN      LPCWSTR                     wszText             OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XShowGameInviteUI(
    IN      DWORD                       dwUserIndex,
    IN      CONST XUID*                 pXuidRecipients     OPTIONAL,
    IN      DWORD                       cRecipients,
    IN      LPCWSTR                     wszText             OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XShowFriendRequestUI(
    IN      DWORD                       dwUserIndex,
    IN      XUID                        xuidUser
    );

XBOXAPI
DWORD
WINAPI
XShowKeyboardUI(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwFlags,
    IN      LPCWSTR                     wseDefaultText,
    IN      LPCWSTR                     wszTitleText,
    IN      LPCWSTR                     wszDescriptionText,
    OUT     LPWSTR                      wszResultText,
    IN      DWORD                       cchResultText,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XShowGamerCardUI(
    IN      DWORD                       dwUserIndex,
    IN      XUID                        XuidPlayer
    );

XBOXAPI
DWORD
WINAPI
XShowAchievementsUI(
    IN      DWORD                       dwUserIndex
    );

typedef enum _XSHOWMARKETPLACEUI_ENTRYPOINTS {
    XSHOWMARKETPLACEUI_ENTRYPOINT_CONTENTLIST,
    XSHOWMARKETPLACEUI_ENTRYPOINT_CONTENTITEM,
    XSHOWMARKETPLACEUI_ENTRYPOINT_MEMBERSHIPLIST,
    XSHOWMARKETPLACEUI_ENTRYPOINT_MEMBERSHIPITEM,
    XSHOWMARKETPLACEUI_ENTRYPOINT_MAX
} XSHOWMARKETPLACEUI_ENTRYPOINTS;


XBOXAPI
DWORD
WINAPI
XShowMarketplaceUI(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwEntryPoint,
    IN      ULONGLONG                   qwOfferID               OPTIONAL,
    IN      DWORD                       dwContentCategories
    );

XBOXAPI
DWORD
WINAPI
XShowDeviceSelectorUI(
    IN      DWORD                       dwUserIndex,
    IN      DWORD                       dwContentType,
    IN      DWORD                       dwContentFlags,
    IN      ULARGE_INTEGER              uliBytesRequested,
    OUT     PXCONTENTDEVICEID           pDeviceID,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

DECLSPEC_NORETURN
XBOXAPI
VOID
WINAPI
XShowDirtyDiscErrorUI(
    IN      DWORD                       dwUserIndex
    );

// API to show error and informational messages
#define XMB_NOICON                      0x00000000
#define XMB_ERRORICON                   0x00000001
#define XMB_WARNINGICON                 0x00000002
#define XMB_ALERTICON                   0x00000003
#define XMB_PASSCODEMODE                0x00010000
#define XMB_VERIFYPASSCODEMODE          0x00020000

#define XMB_MAXBUTTONS                  3
#define XMB_CANCELID                    -1

typedef struct _MESSAGEBOX_RESULT
{
    union
    {
        DWORD                           dwButtonPressed;
        WORD                            rgwPasscode[4];
    };
} MESSAGEBOX_RESULT, *PMESSAGEBOX_RESULT;

XBOXAPI
DWORD
WINAPI
XShowMessageBoxUI(
    IN      DWORD                       dwUserIndex,
    IN      LPCWSTR                     wszTitle,
    IN      LPCWSTR                     wszText,
    IN      DWORD                       cButtons,
    IN      LPCWSTR*                    pwszButtons,
    IN      DWORD                       dwFocusButton,
    IN      DWORD                       dwFlags,
    OUT     PMESSAGEBOX_RESULT          pResult,
    IN OUT  PXOVERLAPPED                pOverlapped         OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XShowPlayerReviewUI(
    IN      DWORD                       dwUserIndex,
    IN      XUID                        XuidFeedbackTarget
    );


//  UI Extensibility API
#define GAMEBANNER_WIDTH                384
#define GAMEBANNER_HEIGHT               128

XBOXAPI
DWORD
WINAPI
XCustomSetBannerImage(
    IN      PVOID                       pvTexture,
    IN      DWORD                       dwFlags
    );


#define CUSTOMACTION_FLAG_CLOSESUI      1

XBOXAPI
VOID
WINAPI
XCustomSetAction(
    IN      DWORD                       dwActionIndex,
    IN      LPCWSTR                     szActionText,
    IN      DWORD                       dwFlags
    );

XBOXAPI
BOOL
WINAPI
XCustomGetLastActionPress(
    OUT     DWORD*                      pdwUserIndex,
    OUT     DWORD*                      pdwActionIndex,
    OUT     XUID*                       pXuid
    );



XBOXAPI
DWORD
WINAPI
XEnableScreenSaver(
    IN      BOOL                        fEnable
    );

#ifdef __cplusplus
}
#endif


#endif // _XBOX_H_

