#ifndef EAX_SOUND_HPP
#define EAX_SOUND_HPP

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"

class AudioSettings;

// yes that is the correct name for the file

#define MAX_EAXMODS 32;           // :39
#define MAX_NUM_DEBUG_BANKS 15    // :41
#define SND_HIGH_DETAIL 44100     // :43
#define SND_LOW_DETAIL 22050      // :44
#define MAX_EAX_AITUNERCARS 10    // :46
#define SNDPRINTF_BUFFER_SIZE 512 // :467
#define SNDPRINTF_CHANNEL 9       // :468
#define NO_SNDPRINTF              // :471
#define SndPrintf if (0)          // :478

// TODO probably move all of these enums
typedef enum {
    SSC_LINEAR = 0,
    SSC_EQPWR = 1,
    SSC_EQPWR_SQ = 2,
    SSC_ONE_MIN_EQPWR = 3,
    SSC_ONE_MIN_EQPWR_SQ = 4,
    SSC_COS_X_ONE_MIN_EQPWR = 5,
    SSC_COS_PLUS_DIST_ROLL = 6,
    SSC_DIST_ROLLOFF = 7,
    SSC_ONEMIN_REVDIST_ROLLOFF = 8
} eNFSSndShapeCurve;

typedef eNFSSndShapeCurve reflection_typedef_eNFSSndShapeCurve;

typedef enum { eSFXOBJ_AZIMUTH = 0, eSFXOBJ_IN_GAMEVOL = 1, eSFXOBJ_IN_DMIX_OUT = 2, eSFXOBJ_IN_MAXNUM = 3 } eSFXOBJ_IN_GENERIC;

typedef eSFXOBJ_IN_GENERIC reflection_typedef_eSFXOBJ_IN_GENERIC;

typedef enum { eSFX_AZIMUTH = 0, eSFX_VOL_COMMON2 = 1, eSFX_VOL_COMMON3 = 2, eSFX_VOL_COMMON4 = 3 } eSFX_VOLUMES;

typedef eSFX_VOLUMES reflection_typedef_eSFX_VOLUMES;

typedef enum { SFXOBJ_PATHFINDER = 0, SFXOBJ_PFEATRAX = 1 } eSFXOBJ_MUSIC_TYPES;

typedef eSFXOBJ_MUSIC_TYPES reflection_typedef_eSFXOBJ_MUSIC_TYPES;

typedef enum {
    eVOL_FE_LICENSED = 0,
    eVOL_IG_LICENSED = 1,
    eVOL_FE_COMPOSED = 2,
    eVOL_IG_COMPOSED = 3,
    eVOL_FE_AMBIENCE = 4,
    eVOL_IG_AMBIENCE = 5,
    eTRG_LICENSED_PLAYING = 6,
    eTRG_COMPOSED_PLAYING = 7,
    eTRG_AMBIENCE_PLAYING = 8
} eVOL_MUSIC;

typedef eVOL_MUSIC reflection_typedef_eVOL_MUSIC;

typedef enum { eSFXOBJ_UNKNOWN = 0 } eSFXOBJ_GENERIC_TYPES;

typedef eSFXOBJ_GENERIC_TYPES reflection_typedef_eSFXOBJ_GENERIC_TYPES;

typedef enum { eSFXCTL_UNKNOWN = 0 } eSFXCTL_GENERIC_TYPES;

typedef eSFXCTL_GENERIC_TYPES reflection_typedef_eSFXCTL_GENERIC_TYPES;

typedef enum { eSFXOBJ_COLLISION = 0, eSFXOBJ_SCRAPES = 1 } eSFXOBJ_COLLISON_TYPES;

typedef eSFXOBJ_COLLISON_TYPES reflection_typedef_eSFXOBJ_COLLISON_TYPES;

typedef enum { SFXOBJ_WORLDOBJECT = 0 } eSFXOBJ_ENVIRONMENT_TYPES;

typedef eSFXOBJ_ENVIRONMENT_TYPES reflection_typedef_eSFXOBJ_ENVIRONMENT_TYPES;

typedef enum { SFXOBJ_PLANES = 0 } eSFXOBJ_PLANES;

typedef eSFXOBJ_PLANES reflection_typedef_eSFXOBJ_PLANES;

typedef enum { SFXOBJ_TRAIN = 0 } eSFXOBJ_TRAINS;

typedef eSFXOBJ_TRAINS reflection_typedef_eSFXOBJ_TRAINS;

typedef enum {
    eCARSFX_AEMS_ENGINE = 0,
    eCARSFX_SINGLE_GINSU = 1,
    eCARSFX_DUAL_GINSU = 2,
    eCARSFX_SHIFTS = 3,
    eCARSFX_TURBOS = 4,
    eCARSFX_NITRO = 5,
    eCARSFX_SPRKCH = 6,
    eCARSFX_SKIDS = 7,
    eCARSFX_ROADNZ = 8,
    eCARSFX_WINDNZ = 9,
    eCARSFX_HYDRAULICS = 10,
    eCARSFX_RAIN = 11,
    eCARSFX_WINDWEATHER = 12,
    eCARSFX_BOTTOMOUT = 13,
    eCARSFX_DRIVEBY = 14,
    eCARSFX_CARDAMAGE = 15,
    eSFXOBJ_REVERB = 16,
    eCARSFX_SIREN = 17,
    eCARSFX_PRECOLWOOSH = 18,
    eCARSFX_TRUCKFX = 19,
    eCARSFX_HORN = 20,
    eSFX_MAXCARSFXOBJGROUPS = 21
} eSFXOBJ_PLAYER_TYPES;

typedef eSFXOBJ_PLAYER_TYPES reflection_typedef_eSFXOBJ_PLAYER_TYPES;

typedef enum { eVOL_WINDWEATHER_MAIN = 0 } eVOL_WINDWEATHER;

typedef eVOL_WINDWEATHER reflection_typedef_eVOL_WINDWEATHER;

typedef enum { eCARSFX_TRAFFIC_ENG = 0, eCARSFX_TRAFFIC_WOOSH = 1, eCARSFX_TRAFFIC_HORN = 2, eCARSFX_TRAFFIC_SKIDS = 3 } eSFXOBJ_TRAFFIC_TYPES;

typedef eSFXOBJ_TRAFFIC_TYPES reflection_typedef_eSFXOBJ_TRAFFIC_TYPES;

typedef enum { eSFXOBJ_WOOSH = 0 } eSFXOBJ_DRIVEBY;

typedef eSFXOBJ_DRIVEBY reflection_typedef_eSFXOBJ_DRIVEBY;

typedef enum { SFXOBJ_HELICOPTER = 0 } eSFXOBJ_HELI;

typedef eSFXOBJ_HELI reflection_typedef_eSFXOBJ_HELI;

typedef enum { eRACE_CIRCUIT = 0, eRACE_DRAG = 1, eRACE_TWOCIRC = 2, eRACE_TWODRG = 3 } eRACETYPE;

typedef eRACETYPE reflection_typedef_eRACETYPE;

typedef enum {
    eAZI_CLOSESTCOP_AZI = 0,
    eVOL_FE_SPEECH = 1,
    eVOL_CELL_CALL = 2,
    eVOL_MAIN_COP_1 = 3,
    eVOL_MAIN_COP_2 = 4,
    eVOL_MAIN_COP_3 = 5,
    eVOL_REG_COP_1 = 6,
    eVOL_REG_COP_2 = 7,
    eVOL_REG_COP_3 = 8,
    eVOL_REG_COP_4 = 9,
    eVOL_HELI_COP = 10,
    eVOL_DISPATCH = 11,
    eFLT_SPEECHFILTER_FLT = 12,
    eVOL_CMN_RADIO_VOL = 13,
    eVOL_MISC_SPEECH = 14
} eVOL_SPEECH;

typedef eVOL_SPEECH reflection_typedef_eVOL_SPEECH;

typedef enum {
    eTRG_SPEECH_ISPLAYING = 0,
    eSCL_SPEECH_INENSITY = 1,
    eTRG_SPEECH_PANNING = 2,
    eSCL_SPEECH_CLARITY = 3,
    eTRG_CELLCALL_PLAYING = 4,
    eSCL_BUSTEDMETER = 5
} eSFXOUT_SPEECH;

typedef eSFXOUT_SPEECH reflection_typedef_eSFXOUT_SPEECH;

typedef enum {
    eAZI_PRE_COL_WOOSH_AZI = 0,
    eVOL_PRE_COL_WOOSH_WOOSH = 1,
    TRIG_PRE_COL_WOOSH_TRIG = 2,
    eVRB_PRE_COL_WOOSH_VERB = 3
} eVOL_PRE_COL_WOOSH;

typedef eVOL_PRE_COL_WOOSH reflection_typedef_eVOL_PRE_COL_WOOSH;

typedef enum {
    eAZI_BOTTOMOUT_AZI = 0,
    eVOL_BOTTOMOUT_JUMPLAND = 1,
    eVOL_BOTTOMOUT_BOTTOMOUT = 2,
    eVOL_BOTTOMOUT_JUMPCAMCRASH = 3,
    eVRB_BOTTOMOUT_VERB = 4
} eVOL_BOTTOMOUT;

typedef eVOL_BOTTOMOUT reflection_typedef_eVOL_BOTTOMOUT;

typedef enum { eAZI_CARDAMAGE_AZI = 0, eVOL_CARDAMAGE_TRUNK_BOUNCE = 1, eVOL_CARDAMAGE_WINDOW = 2, eVRB_CARDAMAGE_VERB = 3 } eVOL_CARDAMAGE;

typedef eVOL_CARDAMAGE reflection_typedef_eVOL_CARDAMAGE;

typedef enum {
    eAZI_ENGINE_AZI = 0,
    eVOL_ENGINE_AEMS = 1,
    eVOL_ENGINE_GINSU = 2,
    eVOL_ENGINE_TRANNY = 3,
    ePCH_ENGINE_PITCH = 4,
    eFLT_ENGINE_FILTER = 5,
    eVRB_ENGINE_AEMS_VERB = 6,
    eVRB_ENGINE_GINSU_VERB = 7
} eVOL_ENGINE;

typedef eVOL_ENGINE reflection_typedef_eVOL_ENGINE;

typedef enum {
    eAZI_NITROUS_AZ = 0,
    eVOL_NITROUS_MAIN = 1,
    eVOL_NITROUS_PURGE = 2,
    ePCH_NITROUS_PCH = 3,
    eFLT_NITROUS_FLTR = 4,
    eVRB_NITROUS_VERB = 5
} eVOL_NITROUS;

typedef eVOL_NITROUS reflection_typedef_eVOL_NITROUS;

typedef enum { eVOL_RAIN_OUTSIDE = 0, eVOL_RAIN_INSIDE = 1, eTRG_RAINING_TRG = 2 } eVOL_RAIN;

typedef eVOL_RAIN reflection_typedef_eVOL_RAIN;

typedef enum { eTRIG_ROADNOISE_TRANSITION = 0 } eTRIG_ROADNOISE;

typedef eTRIG_ROADNOISE reflection_typedef_eTRIG_ROADNOISE;

typedef enum {
    eAZI_SHIFTING_AZI = 0,
    eVOL_SHIFTING_UP = 1,
    eVOL_SHIFTING_DOWN = 2,
    eVOL_SHIFTING_ENGAGE = 3,
    eVOL_SHIFTING_DISENGAGE = 4,
    eVOL_SHIFTING_ON = 5,
    eVOL_SHIFTING_OFF = 6,
    eVOL_SHIFTING_UP_TRIG = 7,
    eVOL_WHINE = 8,
    eVRB_SHIFTING_VERB = 9,
    eVOL_BRAKEPEDAL_HIT = 10
} eVOL_SHIFTING;

typedef eVOL_SHIFTING reflection_typedef_eVOL_SHIFTING;

typedef enum {
    eAZI_SIREN_AZI = 0,
    eVOL_SIREN_MAIN = 1,
    eVOL_SIREN_BED = 2,
    ePCH_SIREN_PITCH = 3,
    eVRB_SIREN_VERB = 4,
    eTRG_SIREN_TRIG = 5
} eVOL_SIREN;

typedef eVOL_SIREN reflection_typedef_eVOL_SIREN;

typedef enum {
    eAZI_SKIDS_RIGHT_AZI = 0,
    eAZI_SKIDS_LEFT_AZI = 1,
    eVOL_SKIDS_RIGHT_FORWARD = 2,
    eVOL_SKIDS_RIGHT_BACK = 3,
    eVOL_SKIDS_RIGHT_SIDE = 4,
    eVOL_SKIDS_LEFT_FORWARD = 5,
    eVOL_SKIDS_LEFT_BACK = 6,
    eVOL_SKIDS_LEFT_SIDE = 7,
    ePCH_SKIDS_PITCH = 8,
    eRVB_SKIDS_REVERB = 9
} eVOL_SKIDS;

typedef eVOL_SKIDS reflection_typedef_eVOL_SKIDS;

typedef enum { eAZI_SPARKCHATTER_AZI = 0, eVOL_SPARKCHATTER_MAIN = 1, eTRG_SPARKCHATTER_TRIG = 2, eVRB_SPARKCHATTER_VERB = 3 } eVOL_SPARKCHATTER;

typedef eVOL_SPARKCHATTER reflection_typedef_eVOL_SPARKCHATTER;

typedef enum { eAZI_STEREO_AZI = 0, eVOL_STEREO_MAIN = 1 } eVOL_STEREO;

typedef eVOL_STEREO reflection_typedef_eVOL_STEREO;

typedef enum {
    eAZI_TRAFFIC_AZI = 0,
    eVOL_TRAFFIC_ENG_MAIN = 1,
    eVOL_TRAFFIC_HORN = 2,
    eVOL_TRAFFIC_WOOSH = 3,
    ePCH_TRAFFIC_PITCH = 4,
    eTRG_TRAFFIC_WOOSH = 5,
    eTRG_TRAFFIC_WOOSH_BIG = 6
} eVOL_TRAFFIC_ENG;

typedef eVOL_TRAFFIC_ENG reflection_typedef_eVOL_TRAFFIC_ENG;

typedef enum { eAZI_TURBO_AZI = 0, eVOL_TURBO_SPOOLING = 1, eVOL_TURBO_BLOWOFF1 = 2, eVOL_TURBO_BLOWOFF2 = 3 } eVOL_TURBO;

typedef eVOL_TURBO reflection_typedef_eVOL_TURBO;

typedef enum {
    eAZI_WINDNOISE_LEFT_AZI = 0,
    eAZI_WINDNOISE_RIGHT_AZI = 1,
    eVOL_WINDNOISE_LEFT_VOL = 2,
    eVOL_WINDNOISE_RIGHT_VOL = 3,
    eVOL_WINDNOISE_RUMBLE = 4,
    ePCH_WINDNOISE_PITCH = 5
} eVOL_WINDNOISE;

typedef eVOL_WINDNOISE reflection_typedef_eVOL_WINDNOISE;

typedef enum { eVOL_BACKGROUND = 0 } eVOL_AMBIENCE;

typedef eVOL_AMBIENCE reflection_typedef_eVOL_AMBIENCE;

typedef enum { eAZI_FE_AZI = 0, eVOL_COMMON = 1, eVOL_FRONTEND = 2, eVOL_RADAR = 3 } eVOL_FEHUD;

typedef eVOL_FEHUD reflection_typedef_eVOL_FEHUD;

typedef enum { HELI_AZIMUTH = 0, HELI_VOLUME = 1, HELI_PITCH = 2 } eSFX_HELI;

typedef eSFX_HELI reflection_typedef_eSFX_HELI;

typedef enum {
    eAZI_NIS_AZI = 0,
    eVOL_NIS_INTRO = 1,
    eVOL_NIS_END = 2,
    eVOL_GAMEBREAKER_NOS = 3,
    eVOL_COLLISION_SWEETENER = 4,
    eVOL_STORM = 5,
    eTRG_NISPLAYTOGGLE = 6,
    eTRG_END_NIS = 7
} eVOL_NISSTREAMS;

typedef eVOL_NISSTREAMS reflection_typedef_eVOL_NISSTREAMS;

typedef enum { REVERB_ENTER_TUNNEL = 0, REVERB_ENTER_NEWZONE = 1 } eREVERB_TRIGGERS;

typedef eREVERB_TRIGGERS reflection_typedef_eREVERB_TRIGGERS;

typedef enum { eVOL_COMMON_CAMERASNAP = 0, eVOL_COMMON_CAMERACHARGE = 1, eVOL_COMMON_UVES = 2 } eVOL_COMMONFX;

typedef eVOL_COMMONFX reflection_typedef_eVOL_COMMONFX;

typedef enum { eAZI_TRUCK_AZI = 0, eVOL_TRUCK_SFX = 1, ePCH_TRUCK_PITCH = 2 } eVOL_TRUCKSOUNDS;

typedef eVOL_TRUCKSOUNDS reflection_typedef_eVOL_TRUCKSOUNDS;

typedef enum { eAZI_WORLDOBJ_AZI = 0, eVOL_WORLDOBJ_FOUNTAIN = 1, ePCH_WORLDOBJ_PITCH = 2, eCUT_WORLDOBJ_CUTOFF = 3 } eVOL_WORLDOBJ;

typedef eVOL_WORLDOBJ reflection_typedef_eVOL_WORLDOBJ;

typedef enum { eDEPTH_PLAYER = 0 } eDEPTH_REVERB;

typedef eDEPTH_REVERB reflection_typedef_eDEPTH_REVERB;

typedef enum { eINVERTED_MIX_CTRL_NONE = 0 } eINVERTED_MIX_CTRL;

typedef eINVERTED_MIX_CTRL reflection_typedef_eINVERTED_MIX_CTRL;

typedef enum { eMISC_SOUNDS_RADAR = 0, eMISC_SOUNDS_UVES = 1, eMISC_SOUNDS_CAMERA = 2, eMISC_SOUNDS_MAX = 3 } eMISC_SOUNDS;

typedef eMISC_SOUNDS reflection_typedef_eMISC_SOUNDS;

// typedef eGameFlowSndState reflection_typedef_eGameFlowSndState;

// TODO move
// Decl: speed/indep/src/eaxsound/soundpause.h:8
enum eSNDPAUSE_REASON {
    eSNDPAUSE_SMS_MESSAGE = 11,
    eSNDPAUSE_PHOTOFINISH = 10,
    eSNDPAUSE_NISON = 9,
    eSNDPAUSE_QUITTOFE = 8,
    eSNDPAUSE_STARTNEWGAME = 7,
    eSNDPAUSE_MOVIE = 6,
    eSNDPAUSE_ONLINE = 5,
    eSNDPAUSE_MEMCARD = 4,
    eSNDPAUSE_SIMSTATE_ACTIVE = 3,
    eSNDPAUSE_SIMSTATE_INIT = 2,
    eSNDPAUSE_SIMSTATE_IDLE = 1,
    eSNDPAUSE_PAUSEMENU = 0,
    ePAUSE_ERROR = -1,
};

// TODO move
// Decl: speed/indep/src/eaxsound/soundpause.h:29
enum eSNDCTLSTATE {
    MAX_SNDCTL_STATES = 18,
    SNDSTATE_SYSTEM_HUD = 17,
    SNDSTATE_ERROR = 16,
    SNDSTATE_FADEOUT = 15,
    SNDSTATE_MINILOAD = 14,
    SNDSTATE_GAMESTARTRACE = 13,
    SNDSTATE_STOP_MUSIC = 12,
    SNDSTATE_FMV = 11,
    SNDSTATE_NIS_ARREST = 10,
    SNDSTATE_NIS_BLK = 9,
    SNDSTATE_NIS_321 = 8,
    SNDSTATE_NIS_INTRO = 7,
    SNDSTATE_NIS_STORY = 6,
    SNDSTATE_FE_SMS_MESSAGE = 5,
    SNDSTATE_FE_UPSCREEN = 4,
    SNDSTATE_FE = 3,
    SNDSTATE_INGAME = 2,
    SNDSTATE_PAUSE = 1,
    SNDSTATE_OFF = 0,
};

// File: speed/indep/src/eaxsound/EAXSOund.hpp
// total size: 0xBC
// Decl: speed/indep/src/eaxsound/EAXSOund.hpp:131
class EAXSound : public AudioMemBase {
  public:
    EAXSound();

    ~EAXSound() override;

    void Update(float t);

    void InitializeDriver();

    void RestoreDriver();

    void InitializeSoundBootLoad();

    void Destroy();

    void QueueNISButtonThrough(uint32 anim_id, int camera_track_number);

    void QueueNISStream(uint32 anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int));

    bool IsNISStreamQueued();

    void PlayNIS();

    void NISFinished();

    bool AreResourceLoadsPending();

    void START_321Countdown();

    static struct SndBase *GetSndBase_Object(int nID);

    struct SFX_Base *GetSFXBase_Object(int nID);

    void SetSndBaseObject(struct SndBase *psb, eMAINMAPSTATES estate, int ntype, int instance);

    void SetSFXBaseObject(struct SFX_Base *psb, eMAINMAPSTATES estate, int ntype, int instance);

    static int *GetPointerCallback(int nid);

    static void SetSFXOutCallback(int nid, int *ptr);

    static bool SetSFXInputCallback(int nid, int *ptr);

    static int GetStateRefCount(int nstate);

    static void MixMapReadyCallback();

    void StartSND11();

    void StopSND11();

    void InitSndCars();

    void SetCarSoundPlayback(bool _On) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:197

    bool IsPlayingCarSounds() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:198

    struct stSongInfo *GetNewSongInfo();

    void UpdateSongInfo();

    void InitEATRAX();

    void PlayEATraxSong(int nindex);

    void PlayFEMusic(int nIndex);

    void PauseFEMusic(const char *pMovieString);

    void ResumeFEMusic();

    bool IsAudioStreamReading() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:211

    bool IsAudioStreamingBlockedByWDR() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:212

    bool DidAudioInterruptWDR() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:213

    void ReInitMasterVolumes();

    int GetMasterVolume(eMasterMixChannel eMasterMixChannel);

    void UpdateVolumes(AudioSettings *paudiosettings, float NewValue);

    void StartNewGamePlay();

    void InitializeFrontEnd();

    void InitializeInGame();

    void LoadInGameSoundBanks(void (*callback)(int32 callback_param));

    void LoadFrontEndSoundBanks(void (*callback)(int32 callback_param));

    void UnloadFrontEndSoundBanks();

    void UnLoadInGameSoundBanks();

    void EnterPauseMenu(eSNDPAUSE_REASON pause_reason);

    void ExitPauseMenu(eSNDPAUSE_REASON pause_reason);

    void CloseSound();

    struct NFSMixMaster *GetMixMaster() {}

    void SetSndGameMode(eSndGameMode eGameMode) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:243

    eSndGameMode GetSndGameMode() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:244

    eSndGameMode GetPrevSndGameMode() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:245

    void SetDebugStreamState(int nstate) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:247

    EAXFrontEnd *GetFrontEnd() { // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:250
        return m_pFESnd;
    }

    struct EAXCar *ConnectCarSnd(struct EAX_CarState *pcar);

    struct EAXCar *GetPlayerTunerCar(int nindex);

    struct EAXCar *GetAITunerCar(int nindex);

    void DestroyEAXCar(struct EAX_CarState *pCar);

    struct CSTATE_Helicopter *SpawnHelicopter(struct EAX_HeliState *pHeli);

    void DestroyEAXHeli(struct EAX_HeliState *pHeli);

    char *GetCsisName() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:276

    void PlayUISoundFX(eMenuSoundTriggers etriggertype);

    void StopUISoundFX(eMenuSoundTriggers etriggertype);

    struct EAXS_StreamManager *GetStreamManager() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:282

    int IsSpeechDone() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:284

    void StopSpeechStream() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:285

    e3DPlayerMix GetPlayerMixMode() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:287

    void SetPlayerMixMode(e3DPlayerMix emix) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:288

    void PauseAudioStreams() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:289

    void ResumeAudioStreams() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:290

    void ChangeLanguage(int new_language) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:291

    void CommitAssets();

    struct cSTICH_PlayBack *GetStichPlayer() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:307

    eSndAudioMode GetDefaultPlatformAudioMode();

    eSndAudioMode SetAudioRenderMode(eSndAudioMode mode);

    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);

    struct AudioSettings *GetCurAudioSettings() {}

    float GetCurMusicVolume();

    void PlayCameraSnapShot() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:317

    bool PauseFadeComplete() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:326

  private:
    void DebugAndProfile();

    void AttachPlayerCars();

  public:
    static struct CSTATEMGR_Base *GetStateMgr(eMAINMAPSTATES estate) {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:391

    int ncompiletest; // offset 0x4, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:144

    struct Attrib::Gen::audiosystem &GetAttributes() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:394

    struct Attrib::Gen::audiosystem &GetLocalAttr() {} // Decl: speed/indep/src/eaxsound/EAXSOund.hpp:395

    void ReStartRace(bool bIs321);

    void RefreshLocalAttr();

    int m_nCopAIStateParam;    // offset 0x8, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:195
    bool bPlayCameraSnapShot;  // offset 0xC, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:318
    bool bPlayCarSounds;       // offset 0x10, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:319
    bool m_bIsSpecialUGMovie;  // offset 0x14, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:320
    bool EngineLoadingBlocked; // offset 0x18, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:321
    bool m_bIsPaused;          // offset 0x1C, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:322
    bool m_bLostFocus;         // offset 0x20, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:323
    float t_Paused;            // offset 0x24, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:324
    float t_CurTime;           // offset 0x28, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:325
    int FrameCnt;              // offset 0x2C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:328

    static bool m_bAreInGameBanksLoaded; // size: 0x1, address: 0xFFFFFFFF, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:330

    int m_nDebugStreamState;            // offset 0x30, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:331
    AudioSettings *m_pCurAudioSettings; // offset 0x34, size 0x4
    bool m_bPause_MainFNG;              // offset 0x38, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:333

  private:
    void *m_pMemoryPoolMem;             // offset 0x3C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:339
    int m_memoryPoolSize;               // offset 0x40, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:340
    int m_numMemoryAllocations;         // offset 0x44, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:341
    eSNDPAUSE_REASON m_LastPauseReason; // offset 0x48, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:345

  public:
    static struct CSTATEMGR_Base *m_pStateMgr[13]; // size: 0x34, address: 0x804F4288, Decl: speed/indep/src/eaxsound/EAXSound.cpp:346

  private:
    int m_transStartTime;     // offset 0x4C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:347
    int m_startingLoopVolume; // offset 0x50, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:348
    bool m_bAudioIsPaused;    // offset 0x54, size 0x1, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:350

  public:
    bool m_X360_UI_Override; // offset 0x58, size 0x1

  private:
    char *m_pcsCsisName;                      // offset 0x5C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:359
    stSongInfo *m_pNewSongInfoSt;             // offset 0x60, size 0x4
    eEAXGameState m_streamManagerState;       // offset 0x64, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:361
    char *m_pEAX_SysHeap;                     // offset 0x68, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:363
    EAXFrontEnd *m_pFESnd;                    // offset 0x6C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:365
    struct EAXCommon *m_pCmnSnd;              // offset 0x70, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:366
    struct NFSLiveLink *m_pNFSLiveLink;       // offset 0x74, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:367
    e3DPlayerMix m_ePlayerMixMode;            // offset 0x78, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:370
    int m_nStereoUpgradeLevel;                // offset 0x7C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:373
    int m_nGameMode;                          // offset 0x80, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:374
    eSndGameMode m_eSndGameMode;              // offset 0x84, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:376
    eSndGameMode m_prevSndGameMode;           // offset 0x88, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:377
    int m_nNumCarsInGame;                     // offset 0x8C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:378
    struct EAXSND8Wrapper *m_pEAXSND8Wrapper; // offset 0x90, size 0x4
    EAXS_StreamManager *m_pStreamManager;     // offset 0x94, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:386
    NFSMixMaster *m_pNFSMixMaster;            // offset 0x98, size 0x4

  public:
    void SetCsisName(struct SndBase *psndbase);

    void SetCsisName(char *pcsAllocName);

    unsigned int Random(int range);

    float Random(float range);

    bool ValidateStreamChunks(const char *filepath, int start, int end);

  private:
    cSTICH_PlayBack *m_pSTICH_Playback;    // offset 0x9C, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:404
    eAemsStreamBanks m_eSpeechLoadBank;    // offset 0xA0, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:407
    int m_nSpeechLoadBankIndex;            // offset 0xA4, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:408
    Attrib::Gen::audiosystem *mAttributes; // offset 0xA8, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:410
    Attrib::Gen::audiosystem *mLocalAttr;  // offset 0xAC, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:411
    Hermes::HHANDLER mmsgMRestartRace;     // offset 0xB0, size 0x4, Decl: speed/indep/src/eaxsound/EAXSOund.hpp:413
    int mEventID;                          // offset 0xB4, size 0x4
    Event::StaticData mData;               // offset 0xB8, size 0x4
};

void InitializeSoundDriver();

bool g_EAXIsPaused(void);

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause);

void FESoundControl(bool bOn, const char *name);

extern EAXSound *g_pEAXSound;

// TODO move these to Ecstasy
extern int32 eDisableFixUpTables;
extern int32 eDirtySolids;
extern int32 eDirtyTextures;
extern int32 eDirtyAnimations;

#endif
