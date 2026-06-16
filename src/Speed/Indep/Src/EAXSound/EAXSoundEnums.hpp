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

#define NUM_CAR_VS_CAR_FRONT 4
#define SIZE_CAR_VS_CAR_FRONT 4
#define NUM_CAR_2_CAR_SIDE 4
#define SIZE_CAR_2_CAR_SIDE 4
#define NUM_CAR_WALL_FRONT 4
#define SIZE_CAR_WALL_FRONT 4
#define NUM_CAR_WALL_SIDE 4
#define SIZE_CAR_WALL_SIDE 4
#define NUM_CAR_BOTM 4
#define SIZE_CAR_BOTM 4
#define NUM_CAR_ROLL 4
#define SIZE_CAR_ROLL 2
#define NUM_SMAKABLE_VS_CAR 2
#define SIZE_SMAKABLE_VS_CAR 3
#define NUM_SMAKABLE_VS_WRLD 3
#define SIZE_SMAKABLE_VS_WRLD 3
#define NUM_BRIDGE_BLOCKS 3
#define SIZE_PER_BRIDGE_BLOCK 3
#define NUM_POST_BLOCKS 3
#define SIZE_PER_POST_BLOCK 6
#define NUM_TREE_BLOCKS 3
#define SIZE_PER_TREE_BLOCK 6
#define NUM_TUNNEL_BLOCKS 3
#define SIZE_PER_TUNNEL_BLOCK 3
#define NUM_TUNNEL_OUT_BLOCKS 2
#define SIZE_PER_TUNNEL_OUT_BLOCK 3

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

enum eSongPlayability {
    ePLAY_OFF = 0,
    ePLAY_MENU = 1,
    ePLAY_RACE = 2,
    ePLAY_ALL = 3,
};

#endif
