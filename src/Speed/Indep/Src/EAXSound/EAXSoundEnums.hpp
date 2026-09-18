#ifndef EAXSOUNDENUMS
#define EAXSOUNDENUMS

#define LEFT_SIDE 0
#define RIGHT_SIDE 1
#define MAX_NUM_ENGINE_BANKS 3
#define PFSTATE_TRANSITION_MASK 0xfffff00
#define PFSTATE_STATEMASK 0x00000ff

enum eMUSIC_TYPE {
    eMUSIC_TYPE_LICENCED = 0,
    eMUSIC_TYPE_INTERACTIVE = 1,
    eMUSIC_TYPE_AMBIENCE = 2,
    eMUSIC_TYPE_SPLASH = 3,
};

enum eNISSFX_TYPE {
    STRM_NONE = -1,
    STRM_NIS_RACE_START = 0,
    STRM_NIS_RACE_BUTTONTHROUGH = 1,
    STRM_NIS_BUSTED = 2,
    STRM_THUNDER = 3,
    STRM_SFX_COLLISION = 4,
    STRM_SFX_MOMENT = 5,
};

enum e3DPlayerMix {
    EAXS3D_SINGLE_PLAYER_MIX = 0,
    EAXS3D_TWO_PLAYER_MIX = 1,
};

enum AEMS_SHIFTING_SAMPLES {
    AEMS_SHIFTING_UP = 1,
    AEMS_SHIFTING_DOWN = 0,
    AEMS_BREAKING = 2,
    MAX_NUM_SHIFTING_SAMPLES = 3,
};

enum eGINSU_ENG_TYPE {
    eGINSU_ENG_AEMS = 0,
    eGINSU_ENG_SINGLE = 1,
    eGINSU_ENG_DUAL = 2,
};

#define GEN_RND_OFFSET(ID, Intensity, Base, NumBlocks, SizePerBlock)                                                                                 \
    {                                                                                                                                                \
        ID = 0;                                                                                                                                      \
        static int LastRandom = 0;                                                                                                                   \
        LastRandom = LastRandom % SizePerBlock;                                                                                                      \
        ID = Base + (int)((float)Intensity / 128.0f * (float)NumBlocks) * SizePerBlock + LastRandom++;                                               \
    }

#define GEN_UNEVEN_OFFSET(ID, Intensity, Base, NumBlocks, SizePerBlock)                                                                              \
    {                                                                                                                                                \
        ID = 0;                                                                                                                                      \
        static int LastRandom = 0;                                                                                                                   \
        LastRandom = LastRandom % SizePerBlock;                                                                                                      \
        int IntensityDistOffset = (int)((float)Intensity / 128.0f * (float)(sizeof(IMPACT_INTENS_DISTRIBUTION) / sizeof(float)));                    \
        IntensityDistOffset = bClamp(IntensityDistOffset, 0, sizeof(IMPACT_INTENS_DISTRIBUTION) / sizeof(float) - 1);                                \
        ID = Base + (int)(IMPACT_INTENS_DISTRIBUTION[IntensityDistOffset] * (float)NumBlocks) * SizePerBlock + LastRandom++;                         \
    }

#define NUM_CAR_VS_CAR_FRONT 4  // Decl: 333
#define SIZE_CAR_VS_CAR_FRONT 4 // Decl: 334

#define NUM_CAR_2_CAR_SIDE 4  // Decl: 336
#define SIZE_CAR_2_CAR_SIDE 4 // Decl: 337

#define NUM_CAR_WALL_FRONT 4  // Decl: 339
#define SIZE_CAR_WALL_FRONT 4 // Decl: 340

#define NUM_CAR_WALL_SIDE 4  // Decl: 343
#define SIZE_CAR_WALL_SIDE 4 // Decl: 344

#define NUM_CAR_BOTM 4  // Decl: 346
#define SIZE_CAR_BOTM 4 // Decl: 347

#define NUM_CAR_ROLL 4  // Decl: 350
#define SIZE_CAR_ROLL 2 // Decl: 351

#define NUM_SMAKABLE_VS_CAR 2  // Decl: 353
#define SIZE_SMAKABLE_VS_CAR 3 // Decl: 354

#define NUM_SMAKABLE_VS_WRLD 3  // Decl: 357
#define SIZE_SMAKABLE_VS_WRLD 3 // Decl: 358

#define NUM_BRIDGE_BLOCKS 3     // Decl: 360
#define SIZE_PER_BRIDGE_BLOCK 3 // Decl: 361

#define NUM_POST_BLOCKS 3     // Decl: 363
#define SIZE_PER_POST_BLOCK 6 // Decl: 364

#define NUM_TREE_BLOCKS 3     // Decl: 366
#define SIZE_PER_TREE_BLOCK 6 // Decl: 367

#define NUM_TUNNEL_BLOCKS 3     // Decl: 370
#define SIZE_PER_TUNNEL_BLOCK 3 // Decl: 371

#define NUM_TUNNEL_OUT_BLOCKS 2     // Decl: 373
#define SIZE_PER_TUNNEL_OUT_BLOCK 3 // Decl: 374

enum FXROADNOISE_LOOP {
    FXROADNOISE_LOOP_NONE = -1,
    FXROADNOISE_LOOP_GRAVEL00 = 0,
    FXROADNOISE_LOOP_SIDEWALK = 1,
    FXROADNOISE_LOOP_COBBLESTONE00 = 2,
    FXROADNOISE_LOOP_DEEPWATER = 3,
    FXROADNOISE_LOOP_WETROAD = 4,
    FXROADNOISE_LOOP_ASHPHALT00 = 5,
    FXROADNOISE_LOOP_ASHPHALT01 = 6,
    FXROADNOISE_LOOP_METAL = 7,
    FXROADNOISE_LOOP_STITCH_LOOP = 8,
    MAX_NUM_FXROADNOISE_LOOP = 9,
};

enum FXROADNOISE_TRANSITION {
    FXROADNOISE_TRANSITION_NONE = -1,
    FXROADNOISE_TRANSITION_CURBON = 0,
    FXROADNOISE_TRANSITION_CURBOFF = 1,
    FXROADNOISE_TRANSITION_SPIKESTRIP = 2,
    FXROADNOISE_TRANSITION_BLOWN = 3,
    FXROADNOISE_TRANSITION_DONTPLAY = 4,
    MAX_NUM_FXROADNOISE_TRANSITION = 5,
};

enum eMasterMixChannel {
    eMASTER_VOL = 0,
    eSPEECH_VOL = 1,
    eMUSIC_VOL = 2,
    eFEMUSIC_VOL = 3,
    eSFX_VOL = 4,
    eENGINE_VOL = 5,
    eTIRE_VOL = 6,
    eAMBIENT_VOL = 7,
    eSPEED_VOL = 8,
    eCOLLISION_VOL = 9,
};

enum eDynMixChannel {
    EAXSDYNMIX_NONE = 0,
    EAXSDYNMIX_PLAYER_1 = 1,
    EAXSDYNMIX_PLAYER_2 = 2,
    EAXSDYNMIX_PLAYER_ENGINE = 3,
    EAXSDYNMIX_AI_ENGINE = 4,
    EAXSDYNMIX_TRAFFIC = 5,
    EAXSDYNMIX_FRONTEND = 6,
};

enum SPEECH_MODE {
    SPEECH_NONE_MODE = 0,
    SPEECH_GAME_MODE = 1,
    SPEECH_FRONTEND_MODE = 2,
    SPEECH_SPLITSCREEN_MODE = 3,
    NUM_SPEECH_MODES = 4,
};

enum SpeechModuleIndex {
    NISSFX_MODULE = 0,
    COPSPEECH_MODULE = 1,
    NUM_SPEECH_MODULES = 2,
};

// TODO huh, is this really here? the dwarf is weird
extern int GameFlowSndState[15];

enum eSongPlayability {
    ePLAY_OFF = 0,
    ePLAY_MENU = 1,
    ePLAY_RACE = 2,
    ePLAY_ALL = 3,
};

enum eSFXOBJ_COLLISON_TYPES {
    eSFXOBJ_COLLISION = 0,
    eSFXOBJ_SCRAPES = 1,
};

typedef eSFXOBJ_COLLISON_TYPES reflection_typedef_eSFXOBJ_COLLISON_TYPES;
enum eSFXOBJ_ENVIRONMENT_TYPES {
    SFXOBJ_WORLDOBJECT = 0,
};

typedef eSFXOBJ_ENVIRONMENT_TYPES reflection_typedef_eSFXOBJ_ENVIRONMENT_TYPES;
enum eSFXOBJ_PLANES {
    SFXOBJ_PLANES = 0,
};

typedef eSFXOBJ_PLANES reflection_typedef_eSFXOBJ_PLANES;
enum eSFXOBJ_TRAINS {
    SFXOBJ_TRAIN = 0,
};

typedef eSFXOBJ_TRAINS reflection_typedef_eSFXOBJ_TRAINS;
enum eSFXOBJ_PLAYER_TYPES {
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
    eSFX_MAXCARSFXOBJGROUPS = 21,
};

typedef eSFXOBJ_PLAYER_TYPES reflection_typedef_eSFXOBJ_PLAYER_TYPES;
enum eVOL_WINDWEATHER {
    eVOL_WINDWEATHER_MAIN = 0,
};

typedef eVOL_WINDWEATHER reflection_typedef_eVOL_WINDWEATHER;
enum eSFXOBJ_TRAFFIC_TYPES {
    eCARSFX_TRAFFIC_ENG = 0,
    eCARSFX_TRAFFIC_WOOSH = 1,
    eCARSFX_TRAFFIC_HORN = 2,
    eCARSFX_TRAFFIC_SKIDS = 3,
};

typedef eSFXOBJ_TRAFFIC_TYPES reflection_typedef_eSFXOBJ_TRAFFIC_TYPES;
enum eSFXOBJ_DRIVEBY {
    eSFXOBJ_WOOSH = 0,
};

typedef eSFXOBJ_DRIVEBY reflection_typedef_eSFXOBJ_DRIVEBY;
enum eSFXOBJ_HELI {
    SFXOBJ_HELICOPTER = 0,
};

typedef eSFXOBJ_HELI reflection_typedef_eSFXOBJ_HELI;
enum eVOL_SPEECH {
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
    eVOL_MISC_SPEECH = 14,
};

typedef eVOL_SPEECH reflection_typedef_eVOL_SPEECH;
enum eSFXOUT_SPEECH {
    eTRG_SPEECH_ISPLAYING = 0,
    eSCL_SPEECH_INENSITY = 1,
    eTRG_SPEECH_PANNING = 2,
    eSCL_SPEECH_CLARITY = 3,
    eTRG_CELLCALL_PLAYING = 4,
    eSCL_BUSTEDMETER = 5,
};

typedef eSFXOUT_SPEECH reflection_typedef_eSFXOUT_SPEECH;
enum eVOL_PRE_COL_WOOSH {
    eAZI_PRE_COL_WOOSH_AZI = 0,
    eVOL_PRE_COL_WOOSH_WOOSH = 1,
    TRIG_PRE_COL_WOOSH_TRIG = 2,
    eVRB_PRE_COL_WOOSH_VERB = 3,
};

typedef eVOL_PRE_COL_WOOSH reflection_typedef_eVOL_PRE_COL_WOOSH;
enum eVOL_BOTTOMOUT {
    eAZI_BOTTOMOUT_AZI = 0,
    eVOL_BOTTOMOUT_JUMPLAND = 1,
    eVOL_BOTTOMOUT_BOTTOMOUT = 2,
    eVOL_BOTTOMOUT_JUMPCAMCRASH = 3,
    eVRB_BOTTOMOUT_VERB = 4,
};

typedef eVOL_BOTTOMOUT reflection_typedef_eVOL_BOTTOMOUT;
enum eVOL_CARDAMAGE {
    eAZI_CARDAMAGE_AZI = 0,
    eVOL_CARDAMAGE_TRUNK_BOUNCE = 1,
    eVOL_CARDAMAGE_WINDOW = 2,
    eVRB_CARDAMAGE_VERB = 3,
};

typedef eVOL_CARDAMAGE reflection_typedef_eVOL_CARDAMAGE;
enum eVOL_ENGINE {
    eAZI_ENGINE_AZI = 0,
    eVOL_ENGINE_AEMS = 1,
    eVOL_ENGINE_GINSU = 2,
    eVOL_ENGINE_TRANNY = 3,
    ePCH_ENGINE_PITCH = 4,
    eFLT_ENGINE_FILTER = 5,
    eVRB_ENGINE_AEMS_VERB = 6,
    eVRB_ENGINE_GINSU_VERB = 7,
};

typedef eVOL_ENGINE reflection_typedef_eVOL_ENGINE;
enum eVOL_NITROUS {
    eAZI_NITROUS_AZ = 0,
    eVOL_NITROUS_MAIN = 1,
    eVOL_NITROUS_PURGE = 2,
    ePCH_NITROUS_PCH = 3,
    eFLT_NITROUS_FLTR = 4,
    eVRB_NITROUS_VERB = 5,
};

typedef eVOL_NITROUS reflection_typedef_eVOL_NITROUS;
enum eVOL_RAIN {
    eVOL_RAIN_OUTSIDE = 0,
    eVOL_RAIN_INSIDE = 1,
    eTRG_RAINING_TRG = 2,
};

typedef eVOL_RAIN reflection_typedef_eVOL_RAIN;
enum eTRIG_ROADNOISE {
    eTRIG_ROADNOISE_TRANSITION = 0,
};

typedef eTRIG_ROADNOISE reflection_typedef_eTRIG_ROADNOISE;
enum eVOL_SHIFTING {
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
    eVOL_BRAKEPEDAL_HIT = 10,
};

typedef eVOL_SHIFTING reflection_typedef_eVOL_SHIFTING;
enum eVOL_SIREN {
    eAZI_SIREN_AZI = 0,
    eVOL_SIREN_MAIN = 1,
    eVOL_SIREN_BED = 2,
    ePCH_SIREN_PITCH = 3,
    eVRB_SIREN_VERB = 4,
    eTRG_SIREN_TRIG = 5,
};

typedef eVOL_SIREN reflection_typedef_eVOL_SIREN;
enum eVOL_SKIDS {
    eAZI_SKIDS_RIGHT_AZI = 0,
    eAZI_SKIDS_LEFT_AZI = 1,
    eVOL_SKIDS_RIGHT_FORWARD = 2,
    eVOL_SKIDS_RIGHT_BACK = 3,
    eVOL_SKIDS_RIGHT_SIDE = 4,
    eVOL_SKIDS_LEFT_FORWARD = 5,
    eVOL_SKIDS_LEFT_BACK = 6,
    eVOL_SKIDS_LEFT_SIDE = 7,
    ePCH_SKIDS_PITCH = 8,
    eRVB_SKIDS_REVERB = 9,
};

typedef eVOL_SKIDS reflection_typedef_eVOL_SKIDS;
enum eVOL_SPARKCHATTER {
    eAZI_SPARKCHATTER_AZI = 0,
    eVOL_SPARKCHATTER_MAIN = 1,
    eTRG_SPARKCHATTER_TRIG = 2,
    eVRB_SPARKCHATTER_VERB = 3,
};

typedef eVOL_SPARKCHATTER reflection_typedef_eVOL_SPARKCHATTER;
enum eVOL_STEREO {
    eAZI_STEREO_AZI = 0,
    eVOL_STEREO_MAIN = 1,
};

typedef eVOL_STEREO reflection_typedef_eVOL_STEREO;
enum eVOL_TRAFFIC_ENG {
    eAZI_TRAFFIC_AZI = 0,
    eVOL_TRAFFIC_ENG_MAIN = 1,
    eVOL_TRAFFIC_HORN = 2,
    eVOL_TRAFFIC_WOOSH = 3,
    ePCH_TRAFFIC_PITCH = 4,
    eTRG_TRAFFIC_WOOSH = 5,
    eTRG_TRAFFIC_WOOSH_BIG = 6,
};

typedef eVOL_TRAFFIC_ENG reflection_typedef_eVOL_TRAFFIC_ENG;
enum eVOL_TURBO {
    eAZI_TURBO_AZI = 0,
    eVOL_TURBO_SPOOLING = 1,
    eVOL_TURBO_BLOWOFF1 = 2,
    eVOL_TURBO_BLOWOFF2 = 3,
};

typedef eVOL_TURBO reflection_typedef_eVOL_TURBO;
enum eVOL_WINDNOISE {
    eAZI_WINDNOISE_LEFT_AZI = 0,
    eAZI_WINDNOISE_RIGHT_AZI = 1,
    eVOL_WINDNOISE_LEFT_VOL = 2,
    eVOL_WINDNOISE_RIGHT_VOL = 3,
    eVOL_WINDNOISE_RUMBLE = 4,
    ePCH_WINDNOISE_PITCH = 5,
};

typedef eVOL_WINDNOISE reflection_typedef_eVOL_WINDNOISE;
enum eVOL_AMBIENCE {
    eVOL_BACKGROUND = 0,
};

typedef eVOL_AMBIENCE reflection_typedef_eVOL_AMBIENCE;
enum eVOL_FEHUD {
    eAZI_FE_AZI = 0,
    eVOL_COMMON = 1,
    eVOL_FRONTEND = 2,
    eVOL_RADAR = 3,
    eTRG_NEW_ZONE = 4,
};

typedef eVOL_FEHUD reflection_typedef_eVOL_FEHUD;
enum eSFX_HELI {
    HELI_AZIMUTH = 0,
    HELI_VOLUME = 1,
    HELI_PITCH = 2,
};

typedef eSFX_HELI reflection_typedef_eSFX_HELI;
enum eVOL_NISSTREAMS {
    eAZI_NIS_AZI = 0,
    eVOL_NIS_INTRO = 1,
    eVOL_NIS_END = 2,
    eVOL_GAMEBREAKER_NOS = 3,
    eVOL_COLLISION_SWEETENER = 4,
    eVOL_STORM = 5,
    eTRG_NISPLAYTOGGLE = 6,
    eTRG_END_NIS = 7,
};

typedef eVOL_NISSTREAMS reflection_typedef_eVOL_NISSTREAMS;
enum eREVERB_TRIGGERS {
    REVERB_ENTER_TUNNEL = 0,
    REVERB_ENTER_NEWZONE = 1,
};

typedef eREVERB_TRIGGERS reflection_typedef_eREVERB_TRIGGERS;
enum eVOL_COMMONFX {
    eVOL_COMMON_CAMERASNAP = 0,
    eVOL_COMMON_CAMERACHARGE = 1,
    eVOL_COMMON_UVES = 2,
    eVOL_COMMON_PURSUIT_START = 3,
    eVOL_COMMON_PURSUIT_STOP = 4,
};

typedef eVOL_COMMONFX reflection_typedef_eVOL_COMMONFX;
enum eVOL_TRUCKSOUNDS {
    eAZI_TRUCK_AZI = 0,
    eVOL_TRUCK_SFX = 1,
    ePCH_TRUCK_PITCH = 2,
};

typedef eVOL_TRUCKSOUNDS reflection_typedef_eVOL_TRUCKSOUNDS;
enum eVOL_WORLDOBJ {
    eAZI_WORLDOBJ_AZI = 0,
    eVOL_WORLDOBJ_FOUNTAIN = 1,
    ePCH_WORLDOBJ_PITCH = 2,
    eCUT_WORLDOBJ_CUTOFF = 3,
};

typedef eVOL_WORLDOBJ reflection_typedef_eVOL_WORLDOBJ;
enum eDEPTH_REVERB {
    eDEPTH_PLAYER = 0,
};

typedef eDEPTH_REVERB reflection_typedef_eDEPTH_REVERB;
enum eINVERTED_MIX_CTRL {
    eINVERTED_MIX_CTRL_NONE = 0,
};

typedef eINVERTED_MIX_CTRL reflection_typedef_eINVERTED_MIX_CTRL;
enum eMISC_SOUNDS {
    eMISC_SOUNDS_RADAR = 0,
    eMISC_SOUNDS_UVES = 1,
    eMISC_SOUNDS_CAMERA = 2,
    eMISC_SOUNDS_PURSUIT_START = 3,
    eMISC_SOUNDS_PURSUIT_END = 4,
    eMISC_SOUNDS_MAX = 5,
};

typedef eMISC_SOUNDS reflection_typedef_eMISC_SOUNDS;

#endif
