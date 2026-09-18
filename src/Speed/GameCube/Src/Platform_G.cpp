#include "Speed/Indep/Src/Misc/Platform.h"

#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

#include "dolphin.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"

// Definidas fuera de esta unidad: GetVideoMode en 0x80209A74, SetVideoMode en
// 0x80209A80 y GetBuildRegionVideoMode en 0x80209A94. El nombre del enum sale
// del manglado, SetVideoMode__F10VIDEO_MODE.
extern Bool bEURGB60;

void SetVideoMode(VIDEO_MODE mode);
VIDEO_MODE GetBuildRegionVideoMode();
void eSetDisplaySystem(int mode);

// realcore todavia no decompilado; prototypes sueltos de la API de sistema.
struct FILESYSOPTS {
    int nType;         // +0x00
    void *pAllocator;  // +0x04
    int nMinAlign;     // +0x08
    int nAsyncThreads; // +0x0C
    int pad[8];        // +0x10
    int nFlags;        // +0x30
};

void bWareInit();
void THREAD_init();
void TIMER_init(int);
int FILE_getopts(FILESYSOPTS *opts);
int FILE_setopts(FILESYSOPTS *opts);
int FILE_init(void *mem, int size);
void ASYNCFILE_init(int numAsync, int arg);
int SYNCTASK_add(void (*task)(void *, int), int priority, int period, void *arg);
extern "C" void snProfInit(int id, void *data, int size);

namespace EA {
namespace Allocator {
class IAllocator;
}
}

extern EA::Allocator::IAllocator &gMemoryAllocator;

int snProfilerEnable = 1;

int egAlphaA = 0;
int egAlphaB = 2;
int egAlphaC = 1;
int egAlphaD = 1;
int egAlphaF = 16;
unsigned long g_GC_Disk_GameName;
void *arenaLo;

void FlushCaches() {
    PPCSync();
}

void EnableInterrupts() {
    OSEnableInterrupts();
}

static inline void OSInitFastCastGC() {
    asm volatile(
        "li 3, 4
	"
        "oris 3, 3, 4
	"
        "mtspr 914, 3
	"
        "li 3, 5
	"
        "oris 3, 3, 5
	"
        "mtspr 915, 3
	"
        "li 3, 6
	"
        "oris 3, 3, 6
	"
        "mtspr 916, 3
	"
        "li 3, 7
	"
        "oris 3, 3, 7
	"
        "mtspr 917, 3");
}

static inline void OSSetGQR5GC(unsigned int type, unsigned int scale) {
    unsigned int gqr = (type << 8) | scale;
    gqr = (gqr << 16) | gqr;
    asm volatile("mtspr 917, %0" : : "b"(gqr));
}

static inline void OSSetGQR6GC(unsigned int type, unsigned int scale) {
    unsigned int gqr = (type << 8) | scale;
    gqr = (gqr << 16) | gqr;
    asm volatile("mtspr 918, %0" : : "b"(gqr));
}

static inline void OSSetGQR7GC(unsigned int type, unsigned int scale) {
    unsigned int gqr = (type << 8) | scale;
    gqr = (gqr << 16) | gqr;
    asm volatile("mtspr 919, %0" : : "b"(gqr));
}

void InitPlatform() {
    static char profdata[0x2000];

    FILESYSOPTS opts;

    bWareInit();
    OSInit();
    DVDInit();
    VIInit();
    PADInit();
    arenaLo = OSGetArenaLo();
    bMemoryInit();
    THREAD_init();
    TIMER_init(100);
    unsigned long diskGameCode = *(unsigned long *)DVDGetCurrentDiskID();
    g_GC_Disk_GameName = diskGameCode;
    opts.nType = 0x38;
    FILE_getopts(&opts);
    opts.nFlags = 1;
    opts.pAllocator = static_cast<void *>(&gMemoryAllocator);
    opts.nMinAlign = 0x20;
    opts.nAsyncThreads = 0x40;
    FILE_setopts(&opts);
    FILE_init(0, 0);
    ASYNCFILE_init(0x10, 0);
    SYNCTASK_add(DVDErrorTask, 2, 0, 0);

    // GQRs del motor de paired singles (U8/U16/S8/S16 en carga y descarga).
    OSInitFastCastGC();
    OSSetGQR5GC(0xb, 0x7);
    OSSetGQR6GC(0x7, 0x4);
    OSSetGQR7GC(0x6, 0x6);

    if (snProfilerEnable != 0) {
        snProfInit(0x278D, profdata, 0x2000);
    }
}

// Las tres llamadas van duplicadas en las dos ramas a proposito: el original no
// las comparte.
void InitDisplaySystem() {
    if (bEURGB60) {
        SetVideoMode(MODE_PAL60);
        eSetDisplaySystem(GetVideoMode());
    } else {
        SetVideoMode(GetBuildRegionVideoMode());
        eSetDisplaySystem(GetVideoMode());
    }
}

int bDoWithStack(void *function, void *stack_pointer, int arg1, int arg2) {
    return 0;
}

// STRIPPED
void FinishedRenderingFEngLayer() {}

int GC_GetOSLanguage() {
    if (BuildRegion::IsEuropeFr()) {
        return 1;
    }

    if (BuildRegion::IsEuropeGer()) {
        return 2;
    }

    if (BuildRegion::IsJapan()) {
        return 10;
    }

    return 0;
}

// Dos pasadas de negro y espera de retrazado antes de reiniciar, para que no se
// vea basura en pantalla. No se reinicia si la tarjeta esta ocupada.
void CheckReset(int reset) {
    if (!MemoryCard::IsCardBusy()) {
        VISetBlack(TRUE);
        VIFlush();
        VIWaitForRetrace();
        VISetBlack(TRUE);
        VIFlush();
        VIWaitForRetrace();
        OSResetSystem(reset, 1, FALSE);
    }
}

// Deja pasar solo los estados de error que la pantalla de aviso sabe mostrar;
// cualquier otro se normaliza a DVD_STATE_END.
int DVDValidErrorState(int state) {
    switch (state) {
    case DVD_STATE_COVER_OPEN:
        return DVD_STATE_COVER_OPEN;
    case DVD_STATE_NO_DISK:
        return DVD_STATE_NO_DISK;
    case DVD_STATE_WRONG_DISK:
        return DVD_STATE_WRONG_DISK;
    case DVD_STATE_RETRY:
        return DVD_STATE_RETRY;
    case DVD_STATE_FATAL_ERROR:
        return DVD_STATE_FATAL_ERROR;
    }

    return DVD_STATE_END;
}

inline void VMStatsManager::Init(const char *name) {
    this->mFrameStats.Init();
    this->mFrameCounter = 0;
    this->mElapsedTime = 0.0f;
    this->mAccumService_us = 0;
    this->mAccumNumFaults = 0;
    this->mMinServicePercentPerFrame = 9999999.0f;
    this->mMaxServicePercentPerFrame = -9999999.0f;
    this->mMinNumServicesPerFrame = 9999999;
    this->mMaxNumServicesPerFrame = 0;
    this->mMinFrameTime = 9999999.0f;
    this->mMaxFrameTime = -9999999.0f;
    this->DebugName = name;
}

// ----------------------------------------------------------------DVDErrorTask

#include "Speed/GameCube/Src/Logitech/LGWheels.h"

#include "Speed/Indep/Src/World/TrackStreamer.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

// Contadores de memoria de video por subsistema (frontend, streaming, juego).
VMStatsManager gVMStatsManager_FE("Frontend");
VMStatsManager gVMStatsManager_LS("Loading");
VMStatsManager gVMStatsManager_IG("InGame");

extern int g_discErrorNumber;
extern int g_discErrorOccured;
extern int FinishedLoadingGlobalSuccesful;
extern LGWheels *plat_lgwheels;
extern PADStatus HardwarePadStatus[4];

// Enlace C++ a proposito: el original la referencia manglada
// (FEngTickSinglePackage__FPCcUi, definida en zFe2), no como simbolo C.
void eBeginScene();
void eEndScene();
int ActualReadJoystickData();

static const char FEngDiscErrorPackage[] = "DiscError.fng";

// Tabla de textos de error de disco [idioma][tipo]. El sexto texto de cada
// idioma es el "Loading..." que se muestra con el efecto de tecleo.
const char *s_OpenCover_ErrorText[7][6] = {
    "The Disc Cover is open.  ^If you want to continue the game, ^please close the Disc Cover.",
    "Please insert the ^Need for Speed\231 Most Wanted Game Disc.",
    "Please insert the ^Need for Speed\231 Most Wanted Game Disc.",
    "The Game Disc could not be read. ^Please read the Nintendo GameCube ^Instruction Booklet for more information.",
    "An error has occurred. Turn the power off ^and refer to the Nintendo GameCube Instruction ^Booklet for further instructions.",
    "Loading...",
    "Le couvercle est ouvert. ^Pour continuer \340 jouer, ^veuillez fermer le couvercle.",
    "Veuillez ins\351rer le disque ^Need for Speed\231 Most Wanted.",
    "Veuillez ins\351rer le disque ^Need for Speed\231 Most Wanted.",
    "La lecture du disque a \351chou\351. ^Veuillez vous r\351f\351rer au manuel d'instructions ^Nintendo GameCube pour de plus amples informations.",
    "Une erreur est survenue. Eteignez la console ^et r\351f\351rez-vous au manuel d'instructions Nintendo ^GameCube pour de plus amples informations.",
    "Chargement...",
    "Der Disc-Deckel ist ge\366ffnet.^Bitte den Disc-Deckel schlie\337en,^um mit dem Spiel fortzufahren.",
    "Bitte die^Need for Speed\231 Most Wanted-Game Disc einlegen.",
    "Bitte die^Need for Speed\231 Most Wanted-Game Disc einlegen.",
    "Diese Game Disc kann nicht gelesen werden.^ Bitte Bedienungsanleitung^des Nintendo GameCube lesen,^ um weitere Informationen zu erhalten. ",
    "Ein Fehler ist aufgetreten. Bitte den^Nintendo GameCube ausschalten und die^Bedienungsanleitung lesen, um weitere ^Informationen zu erhalten.",
    "L\344dt...",
    "Il coperchio del disco \350 aperto.^Se vuoi proseguire nel gioco, chiudi il coperchio del disco.",
    "Inserisci il disco di gioco di Need for Speed\231 Most Wanted.",
    "Inserisci il disco di gioco di Need for Speed\231 Most Wanted.",
    "Impossibile leggere il disco di gioco.^Consulta il manuale d'istruzioni del^Nintendo GameCube per ulteriori indicazioni.",
    "Si \350 verificato un errore. Spegni (OFF) ^e controlla il manuale d'istruzioni del Nintendo GameCube ^per ulteriori indicazioni.",
    "Caricamento...",
    "La cubierta del disco est\341 abierta. ^Si quieres continuar la partida, cierra^la cubierta del disco.",
    "Introduce el disco^Need for Speed\231 Most Wanted.",
    "Introduce el disco^Need for Speed\231 Most Wanted.",
    "No se ha podido leer el disco del juego. ^Consulta el folleto de instrucciones de^Nintendo GameCube para^obtener m\341s informaci\363n.",
    "Se ha producido un error. Apaga la consola y^consulta el folleto de instrucciones de Nintendo GameCube^para obtener m\341s informaci\363n.",
    "Cargando...",
    "De disc-deksel is open. ^Sluit de disc-deksel als je verder ^wilt spelen.",
    "Plaats de spel-disc met Need for Speed\231 Most Wanted.",
    "Plaats de spel-disc met Need for Speed\231 Most Wanted.",
    "De spel-disc kon niet worden gelezen. ^Lees de handleiding van de Nintendo GameCube ^voor meer informatie.",
    "Er is een fout opgetreden. Zet de ^Nintendo GameCube uit en raadpleeg de handleiding ^van de Nintendo GameCube voor nadere instructies.",
    "Laden...",
    "Diskluckan \344r \366ppen.  Om du vill forts\344tta spelet ^m\345ste du st\344nga luckan.",
    "S\344tt in spelskivan^Need for Speed\231 Most Wanted.",
    "S\344tt in spelskivan^Need for Speed\231 Most Wanted.",
    "Det gick inte att l\344sa fr\345n skivan. ^Mer information finns i anv\344ndarhandboken^ till din Nintendo GameCube.",
    "Det har uppst\345tt ett fel. St\344ng av str\366mmen^ och f\366lj instruktionerna i anv\344ndarhandboken ^till din Nintendo GameCube.",
    "Laddar...",
};

void DVDErrorTask(void *arg, int reason) {
    static int resetButtonPressed = 0;
    static int queuedSavingResetButtonPressed = 0;
    static int resetMode = -1;
    static int softwareResetCheckStarted = 0;
    static unsigned int softwareResetStartTick;
    static int num_queued_resets = 0;

    int dvderrorhappened = 0;
    int dvderrormessagehash = 0;
    int softwareResetTriggered = 0;
    bool cleanUp;
    const int resetButtonCombo = 0x1600;
    int current_language = 0;
    int dvderrorticks = 0;
    int last_anim_tick = 0;
    int current_anim_tick = 0;
    char strip_chars = 0;

    do {
        {
            IOModule::GetIOModule().Update();
            if (cFEngJoyInput::mInstance != 0) {
                cFEngJoyInput::mInstance->HandleJoy();
            }
            if (FinishedLoadingGlobalSuccesful == 0) {
                {
                    ActualReadJoystickData();
                }
            }
            if ((HardwarePadStatus[0].button & 0x1600) == 0x1600 ||
                (HardwarePadStatus[1].button & 0x1600) == 0x1600) {
                if (softwareResetCheckStarted == 0) {
                    softwareResetStartTick = OSGetTick();
                    softwareResetCheckStarted = 1;
                } else {
                    {
                        unsigned int current_tick = OSGetTick();
                        unsigned int diff_tick = current_tick - softwareResetStartTick;
                        if (OSTicksToMilliseconds(diff_tick) > 500) {
                            resetMode = 0;
                            softwareResetTriggered = 1;
                        }
                    }
                }
            } else {
                softwareResetCheckStarted = 0;
            }
        }

        if (MemoryCard::IsCardBusy()) {
            if (OSGetResetSwitchState() != 0 || softwareResetTriggered != 0) {
                queuedSavingResetButtonPressed = 1;
            } else if (queuedSavingResetButtonPressed != 0) {
                resetButtonPressed = 1;
                num_queued_resets = num_queued_resets + 1;
            }
            if (MemoryCard::s_pThis != 0) {
                MemoryCard::s_pThis->Tick(16);
            }
            continue;
        }

        if (dvderrorhappened != 0) {
            {
                unsigned long MotorRumble[4];
                bSyncTaskRun();
                if (MemoryCard::s_pThis != 0) {
                    MemoryCard::s_pThis->Tick(16);
                }
                {
                    int port;
                    DVDCheckDisk();
                    bMemSet(MotorRumble, 0, 16);
                    MotorRumble[0] = PAD_MOTOR_STOP_HARD;
                    MotorRumble[1] = PAD_MOTOR_STOP_HARD;
                    MotorRumble[2] = PAD_MOTOR_STOP_HARD;
                    MotorRumble[3] = PAD_MOTOR_STOP_HARD;
                    PADControlAllMotors(MotorRumble);
                    plat_lgwheels->ReadAll();
                    for (port = 0; port <= 3; port++) {
                        if (plat_lgwheels->IsConnected(port)) {
                            plat_lgwheels->StopConstantForce(port);
                            plat_lgwheels->StopSurfaceEffect(port);
                            plat_lgwheels->StopDamperForce(port);
                            plat_lgwheels->StopCarAirborne(port);
                            plat_lgwheels->StopSlipperyRoadEffect(port);
                            plat_lgwheels->PlaySpringForce(port, plat_lgwheels->Position[port].wheel, 180, 180);
                        }
                    }
                }
            }
        }

        if (num_queued_resets == 0 && resetMode == -1 && OSGetResetSwitchState() != 0) {
            resetButtonPressed = 1;
        } else if (num_queued_resets > 0 || resetButtonPressed != 0) {
            resetMode = 0;
        }

        int dvdstatus = DVDGetDriveStatus();
        if (dvdstatus != DVD_STATE_FATAL_ERROR && resetMode != -1) {
            {
                int reset_mode = resetMode;
                resetMode = -1;
                CheckReset(reset_mode);
            }
        }

        switch (dvdstatus) {
        case DVD_STATE_COVER_OPEN:
            dvderrormessagehash = 0;
            break;
        case DVD_STATE_NO_DISK:
            dvderrormessagehash = 1;
            break;
        case DVD_STATE_WRONG_DISK:
            dvderrormessagehash = 2;
            break;
        case DVD_STATE_RETRY:
            dvderrormessagehash = 3;
            break;
        case DVD_STATE_FATAL_ERROR:
            dvderrormessagehash = 4;
            break;
        }

        if (MemoryCard::IsCardBusy()) {
            return;
        }

        dvderrorhappened = DVDValidErrorState(dvdstatus);
        if (dvderrorhappened != 0) {
            {
                struct cFEng *feng;
                current_language = GC_GetOSLanguage();
                g_discErrorNumber = dvderrorhappened;
                g_discErrorOccured = 1;
                if (gMoviePlayer != 0) {
                    gMoviePlayer->Stop();
                }
                SoundPause(true, ePAUSE_ERROR);
                SetSoundControlState(true, SNDSTATE_ERROR, "GC Error");
                if (g_pEAXSound != 0) {
                    g_pEAXSound->Update(0.1f);
                }
                feng = cFEng::Get();
                if (!feng->IsPackagePushed("DiscError.fng")) {
                    feng->PushErrorPackage("DiscError.fng", 0, 0xff);
                }
            }
            FEPrintf("DiscError.fng", 0xeeffd04f, s_OpenCover_ErrorText[current_language][dvderrormessagehash]);
        } else if (g_discErrorOccured != 0) {
            TrackStreamer *streamer = &TheTrackStreamer;
            if (!streamer->HasUserMemoryAllocations() && streamer->IsLoadingInProgressNonRepeatable()) {
                ServiceResourceLoading();
                streamer->ServiceNonGameState();
                streamer->ServiceGameState();
                dvdstatus = DVD_STATE_BUSY;
            }
            if (dvdstatus != DVD_STATE_END) {
                {
                    char the_loading_text[16];
                    int to_copy;
                    char copy_length;
                    int anim_frames;
                    copy_length = bStrLen(s_OpenCover_ErrorText[current_language][5]);
                    bMemSet(the_loading_text, 0, 16);
                    anim_frames = 0x10;
                    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
                        anim_frames = 0x40;
                    }
                    if ((dvderrorticks & anim_frames) != (last_anim_tick & anim_frames)) {
                        strip_chars = 3 - (current_anim_tick % 4);
                        last_anim_tick = dvderrorticks;
                        current_anim_tick = current_anim_tick + 1;
                    }
                    to_copy = copy_length - strip_chars;
                    bStrNCpy(the_loading_text, s_OpenCover_ErrorText[current_language][5], to_copy);
                    to_copy = bStrLen(the_loading_text);
                    while (to_copy <= copy_length) {
                        bStrCat(the_loading_text, the_loading_text, " ");
                        to_copy = to_copy + 1;
                    }
                    FEPrintf("DiscError.fng", 0xeeffd04f, the_loading_text);
                    if (MemoryCard::s_pThis != 0) {
                        MemoryCard::s_pThis->Tick(16);
                    }
                }
            } else {
                {
                    bool wasMovieActive;
                    g_discErrorOccured = 0;
                    g_discErrorNumber = 0;
                    dvderrorhappened = 0;
                    SoundPause(false, ePAUSE_ERROR);
                    SetSoundControlState(false, SNDSTATE_ERROR, "GC Error");
                    if (g_pEAXSound != 0) {
                        g_pEAXSound->Update(0.1f);
                    }
                    wasMovieActive = false;
                    if (gMoviePlayer != 0) {
                        wasMovieActive = true;
                        gMoviePlayer->Stop();
                    }
                    cFEng::Get()->MakeLoadedPackagesDirty();
                    if (cFEng::Get()->IsPackagePushed("DiscError.fng")) {
                        cFEng::Get()->PopErrorPackage();
                    }
                    if (wasMovieActive) {
                        cFEng::Get()->QueueGameMessage(0xC3960EB9, 0, 0xff);
                    }
                }
            }
        }

        if (g_discErrorOccured != 0) {
            {
                struct PADStatus LocalHardwarePadStatus[4];
                FEngTickSinglePackage(FEngDiscErrorPackage, dvderrorticks);
                eBeginScene();
                FEManager::Get()->Render();
                eEndScene();
                if (plat_lgwheels->IsConnected(0) || plat_lgwheels->IsConnected(1)) {
                    plat_lgwheels->ReadAll();
                    HardwarePadStatus[0].button = plat_lgwheels->Position[0].button;
                    HardwarePadStatus[1].button = plat_lgwheels->Position[1].button;
                } else {
                    {
                        int pad_state_0;
                        int pad_state_1;
                        PADRead(LocalHardwarePadStatus);
                        pad_state_0 = LocalHardwarePadStatus[0].err;
                        pad_state_1 = LocalHardwarePadStatus[1].err;
                        if (pad_state_0 == 0) {
                            bMemCpy(&HardwarePadStatus[0], &LocalHardwarePadStatus[0], 12);
                        }
                        if (pad_state_1 == 0) {
                            bMemCpy(&HardwarePadStatus[1], &LocalHardwarePadStatus[1], 12);
                        }
                    }
                }
            }
        }

    } while (++dvderrorticks, g_discErrorOccured != 0);
}

// STRIPPED
void ServicePlatform() {}
