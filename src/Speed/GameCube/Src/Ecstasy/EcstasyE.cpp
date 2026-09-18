#include "EcstasyE.hpp"
#include "Ecstasy.hpp"
#include "EcstasyEx.hpp"
#include "eMatrixE.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyEx.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/FacePixelate.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Src/World/SkyRender.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/GameCube/Src/Ecstasy/xSprites.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/Src/Ecstasy/eEnvMapE.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/World/Clans.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

// TODO put these in correct headers
int eSetDisplaySystem(int video_mode);
void eInitGX();
void eInitTexture();
void eExStartup();
void eDrawStartup();
void InitSlotPools();
void SetScreenBuffers();
void eInitSunPat();
void eWaitUntilRenderingDone();
void InitSlotPoolsEx();
void eSetScissor(int xOrig, int yOrig, int wd, int ht);
void eSetCopyFilter(FILTER_ID filter_index, int enable);
void eSetBackgroundColor(GXColor clr);
void __InitRenderMode();
void __InitMem();
void __InitGX();
void __InitVI();
void __InitMatrices();
void eHangMetric(Bool bEnable);
void cb_DrawDone();
void sync_cb(unsigned short token);
void eSetCulling(GXCullMode mode);
void eResetBlendMode();
void eResetZBuffering();
void eSetZBuffering(unsigned char eEnable, unsigned char eWriteEnable);
void eSetColourUpdate(Bool bRGB, Bool bAlpha);
void eWaitRetrace(unsigned int in);
void eUpdateCopyFilter2(unsigned char);
void eWaitDrawDone();
void VIAdvanceFrame();
void eSetPixelFormat(int nPixelFormat, int nZFormat);
void SetVideoMode(VIDEO_MODE mode);
void ReadLGWheelDataForProgressiveMenu();
unsigned short ReadLGWheelButtonsForProgressiveMenu(int channel);
int IsWheelActiveForProgressiveMenu(int channel);
int eProgressiveScan_EURGB60DialogBox(int mode);
void eProgressiveScan_EURGB60Proceed(int mode);
void eNTSCInterlace_PALProceed(int mode);
extern PADStatus HardwarePadStatus[4];
// El original la define (local, .rodata) en este TU: PAD_CHAN0_BIT..PAD_CHAN3_BIT.
static const unsigned int PADMASKS[4] = {0x80000000u, 0x40000000u, 0x20000000u, 0x10000000u};
// El original direcciona __OSBusClock con @ha/@l sobre un simbolo absoluto
// (lis 0x8000 + lwz 248); AT_ADDRESS no hace eso con GCC 2.9, asi que lo
// declaramos aqui como simbolo absoluto igual que GXWGFifo.
extern unsigned int e_OSBusClock;
int eDEMORFPrintf(short x, short y, short z, char *fmt, ...);
void eDEMOInitCaption(long font_type, long width, long height);
void eDEMOBeforeRender();
void eDEMODoneRender();
int eSetTexture(TextureInfo *texture_info, int stage);
void eSetBlendMode(TextureInfo *texture_info, unsigned char opt);
void eSetAlphaTest(unsigned char bOn);
void eSetBlendModeSrcInvSrc();
void eSetBlendModeSrcAlphaOne();
void eSetBlendModeNone();
void eSetTevSwapStage(GXTevStageID stage, int ras_sel, int tex_sel);
void eResetTevSwapStages();
int vsScreen(int opt);
int vsScreenMultiTexture(int opt);
int psModulate();
int psMotionBlur();
int psGlowBloom(unsigned char intensity_clip);
int psDepthTexture();
int psDepthOfField(unsigned char clip_plane);
int psIntensityReplacement(GXColor intensity_subtractionRGBA);
int psIntensityAccumulate(GXColor intensity_additionRGBA);
int psTint();
int psFEMultiTexture();
int psMWScreenContrast();
int psRVM();
int epRenderStrips(eView *view, eSolid *solid, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags, bMatrix4 *blending_matricies);
void eSubmitMesh(eStripEntry *mesh, unsigned short entries, eView *view, eSolid *solid, uint32 flags, TextureInfo *texture_info,
                 bMatrix4 *local_world, eLightContext *light_context, eLightMaterial *light_material, bMatrix4 *blending_matrices,
                 eDataRenderDynamic *drd);
void epCalculateLocalDirectionalPOS16(unsigned int *colour_table0, unsigned int *colour_table1, int num_colour_entries,
                                      unsigned short *position_table_16, int *normal_table, unsigned char *index_buffer,
                                      int vertex_description, int num_indicies, eLightMaterial *light_material,
                                      eLightContext *light_context);
eLightMaterial *elGetDefaultLightMaterial();
namespace SimpleModelAnim {
void Animate(eModel *model, eSolid *solid, bMatrix4 *local_to_world);
}
void eRecalculateOthographicProjection(int nWhich, float fScale);
void eStallWorkaround(Bool bEnabled);
void eDLSaveContext(Bool bEnabled);
unsigned short eEmitSync(Bool bFlush);
void eCopyDisp(Bool bClear);
void eSendDrawDone(Bool bFlush);
void eProgressiveScanSetMode(int);
void eProgressiveScanModeCheck();
OSFontHeader *eDEMOInitROMFont();
void eDEMODeleteROMFont();
static Bool KeepAlive();
void eDiagnoseHang();
void PlatformInitJoystick();
int ActualReadJoystickData();
void eProgressiveScan_EURGB60SetMode(int os_restarted /* r31 */, int tv_mode /* r4 */);
void eNTSCInterlace_PALSetMode(int os_restarted /* r31 */, int tv_mode /* r4 */);
void eEURGB60ModeCheck();
void eInitContrastSurface();
void eForceResetTevSwapStages();
void eResetIndirectTextureSetup();
void eSetOrthographicMatrixToHW();
void eLoadTevSwapTable();

extern "C" {
unsigned short GXReadDrawSync();
}

Mtx g_ScreenPositionMatrix;
Mtx44 Player1SpecularProjection;
Mtx44 Player2SpecularProjection;
eRenderTarget RenderTargets[NUM_RENDER_TARGETS];
GXRenderModeObj *_rmode;                   // size: 0x4
GXRenderModeObj _rmodeObj;                 // size: 0x3C
Mtx44 projMOrthographic;                   // size: 0x40
Mtx viewMOrthographic;                     // size: 0x30
static const float GCNFEScale_NTSC = 0.0f; // size: 0x4
static const float GCNFEScale_PAL = 0.0f;  // size: 0x4
Mtx44 projMOrthographicScreenQuad;         // size: 0x40
Mtx viewMOrthographicScreenQuad;           // size: 0x30
IVisualTreatmentPlat *pVisualTreatmentPlat = 0;
bool EnableRainIn2P = 0;
int IsPal50Mode = 0;
#ifdef EA_PLATFORM_GAMECUBE
int xfbHcrt = 520; // size: 0x4
#else
int xfbHcrt = 574; // size: 0x4
#endif
int efbHcrt = 480;
float efbxfbRatio = 1.0f;
float PALefbxfbFOVscl = 1.0f; // size: 0x4, address: 0x8041ABFC
float PALefbxfbAspect = 1.0f; // size: 0x4, address: 0x8041AC00
int eGXZFmt16 = 0;
int eGXPixelFmt888 = 0;
int eGXPixelFmt6666 = 1;
int eGXPixelFmt565AA = 2;
float Global3DAspectRatio = 0.8f;
int WaitBufferSwapTime = 0;
int VifTime = 0;
int dummy_vif_time = 0x9EB10;
int FastForwardEnabled = 0;
int FastForwardRate = 4;
int EnableTexturing = true;
int DrawWireframe = 0;
uint32 eFrameCounter = 0;

Mtx44 Player1ReflectionProjection;
Mtx44 Player2ReflectionProjection;

VIDEO_MODE eCurrentVideoMode = static_cast<VIDEO_MODE>(-1);
TextureInfo *pTextureInfoRVMInfo = 0;
TextureInfo *pTextureInfoRVMMask = 0;
TextureInfo *pTextureInfoCarSelectEnvMap = 0;
TextureInfo *pTextureInfoRadialBlur = 0;
TextureInfo *pTextureInfoRadialMask = 0;
TextureInfo *pTextureInfoWhite16x16 = 0;
TextureInfo *pTextureInfoWhite16x16NoAlpha = 0;
int ScreenWidth = 640;
int ScreenHeight = 480;
int EnableLODZ = 1;
int EnableMinimalEnvMap = 0;
int EnableEnvMap = 1;
const char *EnvmapTargetNames[7] = {"TARGET_ENVMAP0F", "TARGET_ENVMAP0R", "TARGET_ENVMAP0B", "TARGET_ENVMAP0L",
                                    "TARGET_ENVMAP0U", "TARGET_ENVMAP0D", "TARGET_ENVMAP0_FULL"};
class eRenderTarget;
eRenderTarget *CurrentRenderTarget = 0;
EVIEWMODE CurrentViewMode = EVIEWMODE_NONE;
EVIEWMODE RenderingViewMode = EVIEWMODE_NONE;
EVIEWMODE TweakerViewMode = EVIEWMODE_NONE;
int g_original_amount_free = 0;
int g_original_largest_malloc = 0;
int do_dumpFastMem = 0;
int RenderCars_psReset = 0;
int PreFE_psReset = 1;
int epRenderStrips_psReset = 0;
int epRenderStrips_ps_NoLighting = 0;
int UpdateVTIndirectTexture = 0;
extern SlotPool *eAnimTextureSlotPool;

extern Bool bEURGB60;
extern int _firstFrame;
extern void *_frameBuffer1;
extern void *_frameBuffer2;
extern void *_currentBuffer;
extern int _GxInitialized;
static unsigned int fbSize;                                         // size: 0x4
extern void *_defaultFIFO;
extern GXFifoObj *_defaultFIFOObj;
extern volatile Bool bHangDiagnose;
extern volatile int e_resync;
extern volatile int e_keepalive;
volatile int e_recover;                                             // size: 0x4
static const unsigned char DEFAULT_STALL = 0;                       // size: 0x1
static const unsigned char DEFAULT_DLSAVE = 0;                      // size: 0x1
extern Bool bStallWorkaround;
extern Bool bDLSaveContext;
int scis_xOrig;                                                     // size: 0x4
int scis_yOrig;                                                     // size: 0x4
int scis_wd;                                                        // size: 0x4
int scis_ht;                                                        // size: 0x4
PADStatus g_InitPad[4];
PADStatus g_LastInitPad[4];
int filt_00;                                                        // size: 0x4
int filt_01;                                                        // size: 0x4
int filt_10;                                                        // size: 0x4
int filt_11;                                                        // size: 0x4
int filt_12;                                                        // size: 0x4
int filt_sum;                                                       // size: 0x4
extern unsigned char CopyFilter[11][7];
extern Bool bESyncError;
extern volatile unsigned long eMAX_ITERATIONS;
static char _err[256];                                              // size: 0x100
static char _err2[256];                                             // size: 0x100
static const bool EnableSunRender = false;                          // size: 0x1
static const bool EnableLetterBoxes = false;                        // size: 0x1
static const bool EnableSafezone = false;                           // size: 0x1
static const float TweakOverrideSunIntensityForRoadSpecular = 0.0f; // size: 0x4
int efbWcrt;                                                        // size: 0x4
int xfbWcrt;                                                        // size: 0x4
GXRenderModeObj PalNFS01IntDfScale = {VI_TVMODE_PAL_INT, 640, (u16)efbHcrt, (u16)xfbHcrt, 40, (574 - xfbHcrt) / 2, 640, (u16)xfbHcrt, VI_XFBMODE_DF, 0, 0, {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}}, {8, 8, 10, 12, 10, 8, 8}};
extern Bool bProgressiveScan;
extern vu16 e_sync;
extern vu16 e_endsync;
extern volatile unsigned short last_sync_token;
extern Bool bNoWait;
extern Bool bAlwaysCopyDisp;
int nFilterUpdates;
int nDepthFormat;
extern TextureInfo *pTexPrev; // size: 0x4
GXTexObj HeadlightClipTextureObj;
void *HeadlightClipTextureTestData = (void *)0xFFFF0000;

bool IsRainDisabled() {
    bool rain_disabled = !EnableRainIn2P && (eGetCurrentViewMode() == EVIEWMODE_TWOH || eGetCurrentViewMode() == EVIEWMODE_TWOV);
    return rain_disabled;
}

// total size: 0x20
struct ParticleSystemStats {
    int num_particles;
    int max_num_particles;
    int num_particle_textures;
    int max_num_particle_textures;
    int num_emitters;
    int max_num_emitters;
    int num_groups;
    int max_num_groups;

    ParticleSystemStats()
    { num_particles = max_num_particles = num_particle_textures = max_num_particle_textures = num_emitters = max_num_emitters = num_groups = max_num_groups = 0; }
};

static ParticleSystemStats g_ParticleStats;

// idk where these go
Camera FlailerCamera;
Camera Player1Camera;
Camera Player2Camera;
Camera Player1RVMCamera;

TextureInfo RenderTargetTextureInfos[17];

static const int bBaselineFov = 0;
static const int bUseFastBaselineFov = 0;
static const unsigned short aBaselineFovMip = bMax(0x4000, 0);
static const unsigned short aBaselineFovZ = bMax(0x238E, 0);

void SetParticleSystemStats(int num_particles, int max_num_particles, int num_particle_textures, int max_num_particle_textures, int num_emitters,
                            int max_num_emitters, int num_groups, int max_num_groups) {
    g_ParticleStats.num_particles = num_particles;
    g_ParticleStats.max_num_particles = max_num_particles;
    g_ParticleStats.num_particle_textures = num_particle_textures;
    g_ParticleStats.max_num_particle_textures = max_num_particle_textures;
    g_ParticleStats.num_emitters = num_emitters;
    g_ParticleStats.max_num_emitters = max_num_emitters;
    g_ParticleStats.num_groups = num_groups;
    g_ParticleStats.max_num_groups = max_num_groups;
}

void PrintParticleSystemStats() {}

void cb_PreRetrace(unsigned long param) {}

void cb_PostRetrace(unsigned long param) {
    LastFrameCounterTick = bGetTicker();
    FrameCounter++;
}

int eInitEnginePlat() {
    eSetDisplaySystem(MODE_NTSC);
    eInitGX();
    eInitTexture();
    eExStartup();
    eDrawStartup();
    InitSlotPools();
    eInitEnvMap();
    SetScreenBuffers();
    eInitSunPat();
    return 1;
}

int eSetDisplaySystem(int video_mode) {
    if (eCurrentVideoMode != video_mode) {
        eCurrentVideoMode = static_cast<VIDEO_MODE>(video_mode);
        ScreenWidth = 640;
        ScreenHeight = 480;
        VISetPreRetraceCallback(cb_PreRetrace);
        VISetPostRetraceCallback(cb_PostRetrace);
    }
    return 1;
}

void InitSlotPools(void) {
    eAnimTextureSlotPool = bNewSlotPool(8, 256, "eAnimTextureSlotPool", 0);
    eAnimTextureSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
    InitSlotPoolsEx();
}

void epInitViews(void) {
    {
        eView *view = eGetView(0, false);
        view->SetRenderTarget(eGetRenderTarget(0), 0);
        view->SetCamera(&FlailerCamera);
        view->SetActive(1);
    }
    {
        eView *view = eGetView(1, false);
        view->SetRenderTarget(eGetRenderTarget(1), 0);
        view->SetCamera(&Player1Camera);
        view->SetActive(1);
        view->ScreenEffects = new ScreenEffectDB();
        view->ScreenEffects->SetMyView(view);
        view->Precipitation = new Rain(view, RAIN);
        view->facePixelation = new FacePixelation(view);
    }
    {
        eView *view = eGetView(2, false);
        view->SetRenderTarget(eGetRenderTarget(2), 0);
        view->SetCamera(&Player2Camera);
        view->SetActive(0);
        view->ScreenEffects = new ScreenEffectDB();
        view->ScreenEffects->SetMyView(view);
        view->Precipitation = EnableRainIn2P ? new Rain(view, RAIN) : nullptr;
    }
    {
        eView *view = eGetView(3, false);
        view->SetRenderTarget0(eGetRenderTarget(3));
        view->SetCamera(&Player1RVMCamera);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(6, false);
        view->SetRenderTarget0(eGetRenderTarget(4));
        view->SetCamera(&Player1Camera);
        view->Precipitation = eGetView(1, false)->Precipitation;
        view->GetPlatInfo()->SetLightPerspectiveProjection(&Player1SpecularProjection);
        view->SetActive(1);
    }
    {
        eView *view = eGetView(7, false);
        view->SetRenderTarget0(eGetRenderTarget(5));
        view->SetCamera(&Player2Camera);
        view->Precipitation = eGetView(2, false)->Precipitation;
        view->GetPlatInfo()->SetLightPerspectiveProjection(&Player2SpecularProjection);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(8, false);
        view->SetRenderTarget(eGetRenderTarget(6), 0);
        view->SetCamera(&Player1Camera);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(9, false);
        view->SetRenderTarget0(eGetRenderTarget(7));
        view->SetCamera(&Player1Camera);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(10, false);
        view->SetRenderTarget(eGetRenderTarget(8), 0);
        view->SetCamera(&Player1Camera);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(11, false);
        view->SetRenderTarget(eGetRenderTarget(9), 0);
        view->SetCamera(&Player1Camera);
        view->SetActive(0);
    }
}

EVIEWMODE eGetCurrentViewMode() {
    return CurrentViewMode;
}

void eUpdateViewMode(void) {
    bNode *node1;
    bNode *node2;
    bNode *node3;

    eView *eview_player1 = eGetView(1, false);
    eView *eview_player2 = eGetView(2, false);
    eView *eview_player1_rvm = eGetView(3, false);

    if (eview_player1->GetCameraMover() != nullptr && eview_player2->GetCameraMover() != nullptr) {
        CurrentViewMode = EVIEWMODE_TWOH;
    } else if (eview_player1->GetCameraMover() != nullptr && eview_player1_rvm->GetCameraMover() != nullptr &&
               !eview_player1->GetCameraMover()->OutsidePOV()) {
        CurrentViewMode = EVIEWMODE_ONE_RVM;
    } else if (eview_player1->GetCameraMover() != nullptr) {
        CurrentViewMode = EVIEWMODE_ONE;
    } else {
        CurrentViewMode = EVIEWMODE_NONE;
    }
}

// NON_MATCHING: 98.1% - CurrentViewMode@ha address pseudo is shared (cross-jumped store) instead of rematerialised
void MaybeChangeViewMode() {
    eView *viewp1;
    eView *viewp2;
    eView *viewr1;
    eRenderTarget *targetp1;
    eRenderTarget *targetp2;
    eRenderTarget *targetr1;
    eView *viewspcp1;
    eView *viewspcp2;
    eRenderTarget *targetspcp1;
    eRenderTarget *targetspcp2;
    eView *viewquad;
    eRenderTarget *targetquad;
    int screen_width;
    int screen_height;

    if (TweakerViewMode != EVIEWMODE_NONE) {
        if (RenderingViewMode != TweakerViewMode) {
            RenderingViewMode = TweakerViewMode;
        }
    } else {
        if (RenderingViewMode != CurrentViewMode) {
            RenderingViewMode = CurrentViewMode;
        }
    }

    viewp1 = eGetView(1, false);
    viewp2 = eGetView(2, false);
    viewr1 = eGetView(3, false);
    targetp1 = viewp1->GetRenderTarget0();
    targetp2 = viewp2->GetRenderTarget0();
    targetr1 = viewr1->GetRenderTarget0();
    viewspcp1 = eGetView(6, false);
    viewspcp2 = eGetView(7, false);
    targetspcp1 = viewspcp1->GetRenderTarget0();
    targetspcp2 = viewspcp2->GetRenderTarget0();
    screen_width = ScreenWidth;
    screen_height = ScreenHeight;

    if (CurrentViewMode == EVIEWMODE_ONE) {
        CameraMover *cmp = viewp1->GetCameraMover();

        if (viewp1->GetCameraMover() && viewr1->GetCameraMover() && !viewp1->GetCameraMover()->OutsidePOV()) {
            CurrentViewMode = EVIEWMODE_ONE_RVM;
        }
    } else if (CurrentViewMode == EVIEWMODE_ONE_RVM) {
        if (!viewr1->GetCameraMover() || viewp1->GetCameraMover()->OutsidePOV()) {
            CurrentViewMode = EVIEWMODE_ONE;
        }
    }

    switch (RenderingViewMode) {
        case EVIEWMODE_TWOH:
            targetp1->ScissorX = 0;
            targetp1->ScissorY = 0;
            targetp1->ScissorW = screen_width;
            targetp1->ScissorH = screen_height >> 1;
            targetp1->FrameWidth = screen_width;
            targetp1->FrameHeight = screen_height >> 1;
            targetp2->ScissorX = 0;
            targetp2->ScissorY = screen_height >> 1;
            targetp2->ScissorW = screen_width;
            targetp2->ScissorH = screen_height >> 1;
            targetp2->FrameWidth = screen_width;
            targetp2->FrameHeight = screen_height >> 1;
            targetr1->ScissorX = 0;
            targetr1->ScissorY = 0;
            targetr1->ScissorW = 0;
            targetr1->ScissorH = 0;
            targetp1->SetActive(1);
            viewp1->SetActive(1);
            targetp2->SetActive(1);
            viewp2->SetActive(1);
            targetr1->SetActive(0);
            viewr1->SetActive(0);
            targetspcp1->SetActive(1);
            viewspcp1->SetActive(1);
            targetspcp2->SetActive(1);
            viewspcp2->SetActive(1);

            for (int quad = 8; quad <= 11; quad++) {
                viewquad = eGetView(quad, false);
                targetquad = viewquad->GetRenderTarget0();

                viewquad->SetActive(1);
                targetquad->SetActive(1);
            }
            break;
        case EVIEWMODE_TWOV:
            targetp1->ScissorX = 0;
            targetp1->ScissorY = 0;
            targetp1->ScissorW = screen_width >> 1;
            targetp1->ScissorH = screen_height;
            targetp2->ScissorX = screen_width >> 1;
            targetp2->ScissorW = screen_width >> 1;
            targetp2->ScissorH = screen_height;
            targetp2->ScissorY = 0;
            targetr1->ScissorX = 0;
            targetr1->ScissorY = 0;
            targetr1->ScissorW = 0;
            targetr1->ScissorH = 0;
            targetp1->SetActive(1);
            viewp1->SetActive(1);
            targetp2->SetActive(1);
            viewp2->SetActive(1);
            targetr1->SetActive(0);
            viewr1->SetActive(0);

            for (int quad = 8; quad <= 11; quad++) {
                viewquad = eGetView(quad, false);
                targetquad = viewquad->GetRenderTarget0();

                viewquad->SetActive(1);
                targetquad->SetActive(0);
            }
            break;
        case EVIEWMODE_ONE_RVM:
            targetp1->ScissorX = 0;
            targetp1->ScissorY = 0;
            targetp1->ScissorW = screen_width;
            targetp1->ScissorH = screen_height;
            targetp1->FrameWidth = screen_width;
            targetp1->FrameHeight = screen_height;
            targetp2->ScissorX = 0;
            targetp2->ScissorY = 0;
            targetp2->ScissorW = 0;
            targetp2->ScissorH = 0;
            targetp2->FrameWidth = 0;
            targetp2->FrameHeight = 0;
            targetr1->ScissorX = 0;
            targetr1->ScissorY = 0;
            targetr1->ScissorW = 64;
            targetr1->ScissorH = 64;
            targetr1->FrameWidth = 64;
            targetr1->FrameHeight = 64;
            targetp1->SetActive(1);
            viewp1->SetActive(1);
            targetp2->SetActive(0);
            viewp2->SetActive(0);
            targetr1->SetActive(1);
            viewr1->SetActive(1);

            for (int quad = 8; quad <= 11; quad++) {
                viewquad = eGetView(quad, false);
                targetquad = viewquad->GetRenderTarget0();

                viewquad->SetActive(1);
                targetquad->SetActive(1);
            }
            break;
        case EVIEWMODE_ONE:
            targetp1->ScissorX = 0;
            targetp1->ScissorY = 0;
            targetp1->ScissorW = screen_width;
            targetp1->ScissorH = screen_height;
            targetp1->FrameWidth = screen_width;
            targetp1->FrameHeight = screen_height;
            targetp2->ScissorX = 0;
            targetp2->ScissorY = 0;
            targetp2->ScissorW = 0;
            targetp2->ScissorH = 0;
            targetp2->FrameWidth = 0;
            targetp2->FrameHeight = 0;
            targetr1->ScissorX = 0;
            targetr1->ScissorY = 0;
            targetr1->ScissorW = 0;
            targetr1->ScissorH = 0;
            targetp1->SetActive(1);
            viewp1->SetActive(1);
            targetp2->SetActive(0);
            viewp2->SetActive(0);
            targetr1->SetActive(0);
            viewr1->SetActive(0);

            for (int quad = 8; quad <= 11; quad++) {
                viewquad = eGetView(quad, false);
                targetquad = viewquad->GetRenderTarget0();

                viewquad->SetActive(1);
                targetquad->SetActive(1);
            }
            break;
        case EVIEWMODE_NONE:
            targetp1->ScissorX = 0;
            targetp1->ScissorY = 0;
            targetp1->ScissorW = screen_width;
            targetp1->ScissorH = screen_height;
            targetp1->FrameWidth = screen_width;
            targetp1->FrameHeight = screen_height;
            targetp2->ScissorX = 0;
            targetp2->ScissorY = 0;
            targetp2->ScissorW = 0;
            targetp2->ScissorH = 0;
            targetp2->FrameWidth = 0;
            targetp2->FrameHeight = 0;
            targetr1->ScissorX = 0;
            targetr1->ScissorY = 0;
            targetr1->ScissorW = 0;
            targetr1->ScissorH = 0;
            targetp1->SetActive(1);
            viewp1->SetActive(1);
            targetp2->SetActive(0);
            viewp2->SetActive(0);
            targetr1->SetActive(0);
            viewr1->SetActive(0);
            targetspcp2->SetActive(0);
            viewspcp2->SetActive(0);

            for (int quad = 8; quad <= 11; quad++) {
                viewquad = eGetView(quad, false);
                targetquad = viewquad->GetRenderTarget0();

                viewquad->SetActive(1);
                targetquad->SetActive(0);
            }
            break;
    }
}

void eFixUpTablesPlat() {
    pTextureInfoRadialBlur = GetTextureInfo(0xAB9A6E21, 0, 0);
    pTextureInfoRVMInfo = GetTextureInfo(0x0000DBD4, 0, 0);
    pTextureInfoRVMMask = GetTextureInfo(0xCF29D0DF, 0, 0);
    pTextureInfoRadialMask = GetTextureInfo(bStringHash("RADIALMASK"), 0, 0);
    pTextureInfoWhite16x16 = GetTextureInfo(bStringHash("WHITE16X16"), 0, 0);
    pTextureInfoWhite16x16NoAlpha = GetTextureInfo(bStringHash("WHITE16X16_NOALPHA"), 0, 0);
}

void EnvMapTextureLoadedCallback(int param) {
    char buffer[64];

    bSPrintf(buffer, "%s_%s", FEManager::Get()->GetGaragePrefixFromType(FEManager::Get()->GetGarageType()), "CARSELECTENVMAP");
    pTextureInfoCarSelectEnvMap = GetTextureInfo(bStringHash(buffer), 0, 0);
}

void eInitFEEnvMapPlat() {
    char buffer[64];

    bSPrintf(buffer, "%s_%s", FEManager::Get()->GetGaragePrefixFromType(FEManager::Get()->GetGarageType()), "CARSELECTENVMAP");
    eLoadStreamingTexture(bStringHash(buffer), (void (*)(uintptr_t))EnvMapTextureLoadedCallback, 0, 0);
}

void eRemoveFEEnvMapPlat() {
    char buffer[64];

    if (FEManager::Get()->GetPreviousGarageType() != 0) {
        bSPrintf(buffer, "%s_%s", FEManager::Get()->GetGaragePrefixFromType(FEManager::Get()->GetPreviousGarageType()), "CARSELECTENVMAP");
        unsigned int hash = bStringHash(buffer);
        if (GetTextureInfo(hash, 0, 0)) {
            eUnloadStreamingTexture(hash);
        }
    }

    if (FEManager::Get()->GetGarageType() != 0) {
        bSPrintf(buffer, "%s_%s", FEManager::Get()->GetGaragePrefixFromType(FEManager::Get()->GetGarageType()), "CARSELECTENVMAP");
        unsigned int hash = bStringHash(buffer);
        if (GetTextureInfo(hash, 0, 0)) {
            eUnloadStreamingTexture(hash);
        }
    }

    pTextureInfoCarSelectEnvMap = nullptr;
}

// void eUpdateViewMode(void) {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//   int iVar4;

//   eView::eView *eview_player1 = eView::eGetView(1);
//   eView::eView *eview_player2 = eView::eGetView(2);
//   eView::eView *eview_player1_rvm = eView::eGetView(3);

//   if (eview_player1->GetCameraMover()) {
//     if (eview_player2->GetCameraMover()) {
//       CurrentViewMode = EVIEWMODE_TWOH;
//     }
//   } else if (eview_player1->GetCameraMover()) {
//     if (eview_player1_rvm->GetCameraMover()) {
//       CurrentViewMode = EVIEWMODE_ONE_RVM;
//     }
//   } else {
//     CurrentViewMode = EVIEWMODE_NONE;
//     // if (eview_player1->GetCameraMover())
//     CurrentViewMode = EVIEWMODE_ONE;
//   }
// }

// TODO move RaceCoordinator
struct RaceCoordinator {
    int unk_00;
    uint8_t unk04[0x4C];
    int *unk_50;
};

RaceCoordinator *pRaceCoordinator;

eView *GetPlayerView(int nPlayerNumber) {
    int viewId;

    if (((pRaceCoordinator) && (pRaceCoordinator->unk_00 == 5)) && (pRaceCoordinator->unk_50)) {
        if (nPlayerNumber != pRaceCoordinator->unk_50[5]) {
            return nullptr;
        }
        return eGetView(1, false);
    } else {
        return eGetView(nPlayerNumber + 1, false);
    }
}

// TODO not a thing in this game
void RadialBlurOn(eView *view) {
    if (!pRaceCoordinator || (pRaceCoordinator->unk_00 != 8)) {
        view->pBlendMask = pTextureInfoRadialBlur;
    }
}

void RadialBlurOn(int nPlayerNumber) {
    eView *view = GetPlayerView(nPlayerNumber);
    if (view) {
        RadialBlurOn(view);
    }
}

void RadialBlurOff(eView *view) {
    view->pBlendMask = nullptr;
}

void RadialBlurOff(int nPlayerNumber) {
    eView *view = GetPlayerView(nPlayerNumber);
    if (view != (eView *)0x0) {
        RadialBlurOff(view);
    }
}

int RadialBlurOnOff(bool on, int nPlayerNumber) {
    if (on) {
        RadialBlurOn(nPlayerNumber);
    } else {
        RadialBlurOff(nPlayerNumber);
    }
    return nPlayerNumber;
}

void RadialBlurAlpha(int param_1, float param_2, float param_3) {}

bool bStraddlesNearZ(Camera *camera, Car *car) {
    return false;
}

void eForceBackgroundColour(unsigned char, unsigned char, unsigned char, float, const char *) {}

void SetupSceneryCullInfoPlat(eView *view, SceneryCullInfo &info) {
    bCopy(&info.ClipMatrix, view->GetPlatInfo()->GetWorldClipMatrix());
    info.pView = view;
}

int eClampTopLeft(bool bOnOff, int nUnused) {
    return false;
}

extern cSphereMap SphereMap;
extern cSpecularMap SpecularMap;
extern cQuarterSizeMap QSizeScratchPad;
void eSetOrthographicScreenQuadProjection(eRenderTarget *target);
int eMotionBlurEffect(eView *view);
void eDisplayLetterBoxes();
void eDisplaySafezone();
extern float EnvMapScreenZ;
extern cQuarterSizeMap QSizeI8_Z8;
extern GXColor KColorWorldSpecular;
extern GXColor KShadowRGBA;
extern GXColor KColorSky;
extern int DrawCarsReflections;
extern float MinSkySpecular;
extern float MaxSkySpecular;

void ePreDisplay();
void ePostDisplay();
void eBeginScene();
void eEndScene();
void eSetCurrentRenderTarget(eRenderTarget *render_target);
void eFlushTextureBucketList();
void eRenderSky(eView *view);
void eDisplayEnvRenderTargets(eView *view);
void eFacePixelate(eView *view);
void DisplayRVMs(eView *view);
void eRenderSun(eView *view);
void eSetupFog(eView *view);
void eSetFogEnableState(int state);
void eSetFogBrightnessConstant(float brightness);
void eRenderLightFlarePool(eView *view);
void elBeginFrame();
void eSwapFrameMallocBuffers();
void eSwapDynamicFrameMemory();
void UpdateEnvironmentMapCameras();
void SetupSceneryCullInfo(eView *view, SceneryCullInfo &info, int exclude_flags);
void RenderGpsArrows(eView *view);
void DrawTestCars(eView *view, int reflection);
void RenderFrontEndCars(eView *view, int reflection);
void RenderFEFlares(eView *view, int reflection);
void RenderVehicleFlares(eView *view, int reflection, int renderFlareFlags);
void RenderTrackMarkers(eView *view);
void RenderVisibleZones(eView *view);
void RenderWorldModels(eView *view, int exc_flag);
void PrintParticleSystemStats();

ePoly::ePoly() {
    Vertices[0].x = 0.0f;
    Vertices[0].y = 0.0f;
    Vertices[0].z = 0.0f;
    Vertices[1].x = 64.0f;
    Vertices[1].y = 0.0f;
    Vertices[1].z = 0.0f;
    Vertices[2].x = 64.0f;
    Vertices[2].y = 64.0f;
    Vertices[2].z = 0.0f;
    Vertices[3].x = 0.0f;
    Vertices[3].y = 64.0f;
    Vertices[3].z = 0.0f;

    *reinterpret_cast<unsigned int *>(Colours[0]) = 0x80808080;
    *reinterpret_cast<unsigned int *>(Colours[1]) = 0x80808080;
    *reinterpret_cast<unsigned int *>(Colours[2]) = 0x80808080;
    *reinterpret_cast<unsigned int *>(Colours[3]) = 0x80808080;

    UVs[0][0] = 0.0f;
    UVs[0][1] = 0.0f;
    UVs[0][2] = 1.0f;
    UVs[0][3] = 0.0f;
    UVs[1][0] = 1.0f;
    UVs[1][1] = 1.0f;
    UVs[1][2] = 0.0f;
    UVs[1][3] = 1.0f;

    SetFlailer(0);
}

bool IsSunInFrustrum(eView *player_view) {
    SunChunkInfo *sun_info = SunInfo; // r9

    if (!sun_info) {
        return false;
    }
    const bVector2 sunpos_xy(sun_info->PositionX, sun_info->PositionY);                                         // sp8
    bVector2 campos_xy(player_view->GetCamera()->GetPosition()->x, player_view->GetCamera()->GetPosition()->y); // sp10

    const bVector2 to_pt_xy_un = sunpos_xy - campos_xy;                                                                     // sp18
    const bVector2 cam_dir_xy_un(player_view->GetCamera()->GetDirection()->x, player_view->GetCamera()->GetDirection()->y); // sp20
    bVector2 to_pt_xy = bNormalize(cam_dir_xy_un);                                                                          // sp38
    bVector2 cam_dir_xy = bNormalize(to_pt_xy_un);                                                                          // sp40;

    float dotp = bDot(&to_pt_xy, &cam_dir_xy);

    return dotp > 0.5f;
}

void eDisplayFrame() {

    int start_time_0 = bGetTicker();
    unsigned int the_edisplay_frame_is_game_flow_in_game = IsGameFlowInGame();

    if (GiveTheMoviePlayerBandwidth()) {
        the_edisplay_frame_is_game_flow_in_game = 0;
    }

    if (the_edisplay_frame_is_game_flow_in_game) {
        bNoWait = 1;
    } else {
        bNoWait = 0;
    }

    int *pfree = &g_original_amount_free;
    int *plargest = &g_original_largest_malloc;
    *pfree = bCountFreeMemory(0);
    *plargest = bLargestMalloc(0);
    if (do_dumpFastMem) {
        gFastMem.DumpRecord();
        do_dumpFastMem = 0;
    }

    eBeginScene();

    WaitBufferSwapTime = bGetFixTickerDifference(start_time_0, bGetTicker());

    VifTime = dummy_vif_time;

    eSetCulling(GX_CULL_NONE);

    if (FastForwardEnabled) {
        if (RealLoopCounter % FastForwardRate != 0) {
            return;
        }
    }

    {
        ePreDisplay();

        int current_video_mode = GetVideoMode();
        if (eCurrentVideoMode != current_video_mode) {
            eSetDisplaySystem(current_video_mode);
        }

        MaybeChangeViewMode();

        if (eGetCurrentViewMode() == EVIEWMODE_TWOH) {
            EnableLODZ = 0;
            DrawCarsReflections = 0;
            EnableMinimalEnvMap = 1;
        } else {
            EnableLODZ = 1;
            DrawCarsReflections = 1;
            EnableMinimalEnvMap = 0;
        }

        if (EnableEnvMap) {
            UpdateEnvironmentMapCameras();
        }

        elBeginFrame();

        for (int view_id = EVIEW_FIRST; view_id <= EVIEW_LAST; view_id++) {
            eView *view = eGetView(view_id, true);
            eViewPlatInfo *plat_view = view->GetPlatInfo();

            plat_view->ReflectionView = 0;
            if (view->IsActive()) {

                switch (view_id) {
                case EVIEW_PLAYER1_SPECULAR:
                    MTXLightPerspective(Player1SpecularProjection, view->FovDegrees, plat_view->aspect, 0.5f, -0.5f, 0.5f, 0.5f);
                    break;
                case EVIEW_PLAYER2_SPECULAR:
                    MTXLightPerspective(Player2SpecularProjection, view->FovDegrees, plat_view->aspect, 0.5f, -0.5f, 0.5f, 0.5f);
                    break;
                }

                float force_far_z = 0.0f;
                float force_near_z = 0.0f;
                if (view_id >= EVIEW_FIRST_ENVMAP && view_id <= EVIEW_LAST_ENVMAP) {
                    force_far_z = EnvMapScreenZ;
                } else if (view_id >= EVIEW_PLAYER1_SPECULAR && view_id <= EVIEW_PLAYER2_SPECULAR) {
                    force_far_z = 450.0f;
                } else if (view_id > EVIEW_PLAYER2) {
                    force_far_z = 120.0f;
                    force_near_z = 0.5f;
                }
                plat_view->CalculateViewMatricies(view, force_near_z, force_far_z, 0.0f);
            }
        }

        eView *player1_view = eGetView(EVIEW_PLAYER1, true);
        player1_view->GetPlatInfo()->ReflectionView = 0;
        player1_view->GetPlatInfo()->SpecularView = eGetView(EVIEW_PLAYER1_SPECULAR, true);

        eView *player2_view = eGetView(EVIEW_PLAYER2, true);
        player2_view->GetPlatInfo()->ReflectionView = 0;
        player2_view->GetPlatInfo()->SpecularView = eGetView(EVIEW_PLAYER2_SPECULAR, true);

        if (the_edisplay_frame_is_game_flow_in_game) {
            pVisualTreatmentPlat->Update(player1_view);
            if (eGetCurrentViewMode() == EVIEWMODE_TWOH) {
                pVisualTreatmentPlat->Update(player2_view);
            }
        }

        if (UpdateVTIndirectTexture) {
            pVisualTreatmentPlat->UpdateIndirectTexture();
        }
    }

    GrandSceneryCullInfo grand_scenery_cull_info;
    grand_scenery_cull_info.Init();

    Player *pPlayer1 = Player::GetPlayerByIndex(0);

    if (!GiveTheMoviePlayerBandwidth()) {

        grand_scenery_cull_info.DoCulling();

        for (int i = 0; i <= 1; i++) {
            eView *playerview = eGetView(EVIEW_PLAYER1 + i, true);
            eView *view = eGetView(EVIEW_PLAYER1_SPECULAR + i, true);
            eViewPlatInfo *plat_view = view->GetPlatInfo();
            eRenderTarget *render_target = view->GetRenderTarget();

            if (view->IsActive() && render_target->IsActive()) {

                eSetCurrentRenderTarget(render_target);

                GXSetProjection(*reinterpret_cast<Mtx44 *>(plat_view->GetViewScreenMatrix()), GX_PERSPECTIVE);

                SunChunkInfo *sun_info = SunInfo;
                if (sun_info) {
                    if (IsSunInFrustrum(playerview)) {
                        float sun_intensity = GetSunIntensity(playerview);
                        float sunintensity_to_specalpha = MinSkySpecular + sun_intensity * (MaxSkySpecular - MinSkySpecular);
                        KColorWorldSpecular.a = static_cast<unsigned char>(sunintensity_to_specalpha);
                    } else {
                        KColorWorldSpecular.a = 0;
                    }
                } else {
                    KColorWorldSpecular.a = 0;
                }

                eRenderSky(view);

                eFlushTextureBucketList();

                SpecularMap.specBuffer[i].CaptureEFB(1, 0, GX_TF_RGB565);
            }
        }
    }

    if (!GiveTheMoviePlayerBandwidth()) {

        if (the_edisplay_frame_is_game_flow_in_game) {

            for (int view_id = EVIEW_PLAYER1; view_id <= EVIEW_PLAYER2; view_id++) {
                eView *view = eGetView(view_id, true);
                eRenderTarget *render_target = view->GetRenderTarget();
                if (view->IsActive() && render_target->IsActive()) {
                    SceneryCullInfo *scenery_cull_info = grand_scenery_cull_info.AddCullInfo();
                    SetupSceneryCullInfo(view, *scenery_cull_info, 0x4000);
                }
            }

            for (int view_id = EVIEW_FIRST_RVM; view_id <= EVIEW_LAST_RVM; view_id++) {
                eView *view = eGetView(view_id, true);
                eRenderTarget *render_target = view->GetRenderTarget();
                if (view->IsActive() && render_target->IsActive()) {
                    SceneryCullInfo *scenery_cull_info = grand_scenery_cull_info.AddCullInfo();
                    SetupSceneryCullInfo(view, *scenery_cull_info, 0);
                }
            }

            if (EnableEnvMap && !EnableMinimalEnvMap) {
                for (int view_id = EVIEW_FIRST_ENVMAP; view_id <= EVIEW_LAST_ENVMAP; view_id++) {
                    eView *view = eGetView(view_id, true);
                    eRenderTarget *render_target = view->GetRenderTarget();
                    if (view->IsActive() && render_target->IsActive()) {
                        SceneryCullInfo *scenery_cull_info = grand_scenery_cull_info.AddCullInfo();
                        SetupSceneryCullInfo(view, *scenery_cull_info, 0x1100);
                    }
                }
            }
        }

        grand_scenery_cull_info.DoCulling();

        if (EnableEnvMap) {
            for (int i = 0; i <= 5; i++) {
                eView *view = eGetView(EVIEW_ENVMAP0F + i, true);
                eViewPlatInfo *plat_view = view->GetPlatInfo();
                eRenderTarget *render_target = view->GetRenderTarget();
                if (view->IsActive() && render_target->IsActive()) {

                    eSetCurrentRenderTarget(render_target);

                    GXSetProjection(*reinterpret_cast<Mtx44 *>(plat_view->GetViewScreenMatrix()), GX_PERSPECTIVE);

                    if (!EnableMinimalEnvMap) {
                        eRenderSky(view);
                        grand_scenery_cull_info.StuffScenery(view, 0);
                    }

                    eRenderWorldLightFlares(view, FLARE_ENV);
                    eRenderLightFlarePool(view);
                    RenderVehicleFlares(view, 0, 1);

                    eFlushTextureBucketList();

                    SphereMap.cubeBuffer[i].CaptureEFB(1, 0, GX_TF_RGB5A3);
                }
            }

            SphereMap.BuildSphereMap();
        }

        if (IsGameFlowInGame()) {
            if (FEDatabase->GetGameplaySettings()->RearviewOn && !FEManager::ShouldPauseSimulation(true)) {
                for (int view_id = EVIEW_FIRST_RVM; view_id <= EVIEW_LAST_RVM; view_id++) {
                    eView *view = eGetView(view_id, true);
                    eViewPlatInfo *plat_view = view->GetPlatInfo();
                    {
                        eRenderTarget *render_target = view->GetRenderTarget();
                        if (view->IsActive() && render_target && render_target->IsActive()) {

                            eSetCurrentRenderTarget(render_target);

                            GXSetProjection(*reinterpret_cast<Mtx44 *>(plat_view->GetViewScreenMatrix()), GX_PERSPECTIVE);

                            StuffSkyLayer(view, SKY_LAYER_LOWREZ);

                            grand_scenery_cull_info.StuffScenery(view, 0);

                            eFlushTextureBucketList();

                            DrawTestCars(view, 0);

                            eRenderWorldLightFlares(view, FLARE_NORM);

                            eFlushTextureBucketList();

                            SphereMap.cubeBuffer[0].CaptureEFB(1, 0, GX_TF_RGB5A3);
                        }
                    }
                }
            }
        }

        if (_rmode->aa) {
            eSetPixelFormat(eGXPixelFmt565AA, eGXZFmt16);
        } else {
            eSetPixelFormat(eGXPixelFmt888, eGXZFmt16);
        }

        for (int view_id = EVIEW_PLAYER1; view_id <= EVIEW_PLAYER2; view_id++) {
            eView *view = eGetView(view_id, true);
            eViewPlatInfo *plat_view = view->GetPlatInfo();
            {
                eRenderTarget *render_target = view->GetRenderTarget();
                if (view->IsActive() && render_target && render_target->IsActive()) {

                    eSetCurrentRenderTarget(render_target);

                    GXSetProjection(*reinterpret_cast<Mtx44 *>(plat_view->GetViewScreenMatrix()), GX_PERSPECTIVE);

                    eSetupFog(view);
                    eSetFogEnableState(1);

                    eSetFogBrightnessConstant(0.4f);

                    if (the_edisplay_frame_is_game_flow_in_game) {
                        eRenderSky(view);
                    }

                    unsigned char world_shadow_poly_color = 255;
                    unsigned char world_shadow_poly_alpha_modulation_value =
                        view->Precipitation ? static_cast<unsigned char>((1.0f - view->Precipitation->GetCloudIntensity()) * 255.0f)
                                            : 255;

                    KColorSky.r = world_shadow_poly_color;
                    KColorSky.g = world_shadow_poly_color;
                    KColorSky.b = world_shadow_poly_color;
                    KColorSky.a = world_shadow_poly_alpha_modulation_value;
                    KShadowRGBA.a = world_shadow_poly_alpha_modulation_value;

                    eFlushTextureBucketList();

                    eSetFogBrightnessConstant(0.7f);

                    grand_scenery_cull_info.StuffScenery(view, 0x20);

                    eFlushTextureBucketList();

                    eSetFogEnableState(0);

                    RenderClans(view);

                    RenderTrackMarkers(view);

                    RenderVisibleZones(view);

                    RenderCameraMovers(view);

                    RenderWorldModels(view, 0);

                    if (TheGameFlowManager.IsInFrontend()) {
                        GarageMainScreen *garageMainScreen = GarageMainScreen::GetInstance();
                        if (garageMainScreen) {
                            garageMainScreen->HandleRender(1);
                        }
                    }

                    eRenderWorldLightFlares(view, FLARE_NORM);

                    eRenderLightFlarePool(view);

                    eFlushTextureBucketList();
                }
            }
        }

    }

    if (the_edisplay_frame_is_game_flow_in_game) {

        eSetCopyFilter(FILTER_MOTIONBLUR, 1);

        QSizeScratchPad.quarterSizeBuffer.SetEFBCaptureRegion(0, 0, 320, 240);
        QSizeScratchPad.quarterSizeBuffer.CaptureEFB(0, 1, GX_TF_RGBA8);

        for (int view_id = EVIEW_PLAYER1; view_id <= EVIEW_PLAYER2; view_id++) {
            eView *view = eGetView(view_id, true);
            eRenderTarget *render_target = view->GetRenderTarget();
            if (view->IsActive() && render_target->IsActive()) {

                eSetCurrentRenderTarget(render_target);

                eSetOrthographicScreenQuadProjection(render_target);

                eMotionBlurEffect(view);
            }
        }
    }

    if (!GiveTheMoviePlayerBandwidth()) {

        for (int view_id = EVIEW_PLAYER1; view_id <= EVIEW_PLAYER2; view_id++) {
            eView *view = eGetView(view_id, true);
            eViewPlatInfo *plat_view = view->GetPlatInfo();
            eRenderTarget *render_target = view->GetRenderTarget();
            if (view->IsActive() && render_target && render_target->IsActive()) {

                eSetCurrentRenderTarget(render_target);

                GXSetProjection(*reinterpret_cast<Mtx44 *>(plat_view->GetViewScreenMatrix()), GX_PERSPECTIVE);

                RenderGpsArrows(view);

                DrawTestCars(view, 0);

                if (IsGameFlowInFrontEnd()) {
                    RenderFrontEndCars(view, 0);
                }

                eFlushTextureBucketList();

                gEmitterSystem.Render(view);

                NGSpriteManager.RenderAll(view);

                eFlushTextureBucketList();

                if (RenderCars_psReset) {
                    psReset(RESET_OPTION_DEFAULT);
                }

                RenderVehicleFlares(view, 0, 0);

                RenderFEFlares(view, 0);

                eFlushTextureBucketList();
            }
        }
    }

    {
        eView *view = eGetView(EVIEW_FLAILER, true);
        eViewPlatInfo *plat_view = view->GetPlatInfo();
        {
            eRenderTarget *render_target = view->GetRenderTarget();
            bool active = render_target != 0 && render_target->IsActive();

            static int lastactive = 0;

            lastactive = active;

            if (view->IsActive() && active) {

                eSetCurrentRenderTarget(render_target);
                eSetOrthographicMatrixToHW();

                DisplayRVMs(view);
            }
        }
    }

    if (the_edisplay_frame_is_game_flow_in_game) {

        for (int i = 0; i <= 3; i++) {
            eView *quad_view = eGetView(EVIEW_QUADRANT_TOP_LEFT + i, true);
            eRenderTarget *render_target = quad_view->GetRenderTarget();
            if (quad_view->IsActive()) {

                eSetCurrentRenderTarget(render_target);

                eSetOrthographicScreenQuadProjection(render_target);

                eSetCopyFilter(FILTER_CONTRAST_INTENSITY, 1);

                QSizeI8_Z8.quarterSizeBuffer.SetEFBCaptureRegion(render_target->ScissorX, render_target->ScissorY,
                                                                 render_target->ScissorW, render_target->ScissorH);
                QSizeI8_Z8.quarterSizeBuffer.CaptureEFB(0, 0, GX_TF_I8);

                eSetCopyFilter(FILTER_DEFAULT, 0);

                QSizeScratchPad.quarterSizeBuffer.SetEFBCaptureRegion(render_target->ScissorX, render_target->ScissorY,
                                                                      render_target->ScissorW, render_target->ScissorH);
                QSizeScratchPad.quarterSizeBuffer.CaptureEFB(0, 0, GX_TF_RGBA8);

                pVisualTreatmentPlat->RenderMWVisualLook(quad_view);
            }
        }

        eResetIndirectTextureSetup();
    }

    if (PreFE_psReset) {
        psReset(RESET_OPTION_DEFAULT);
    }

    {
        eView *view = eGetView(EVIEW_FLAILER, true);
        eViewPlatInfo *plat_view = view->GetPlatInfo();
        {
            eRenderTarget *render_target = view->GetRenderTarget();
            bool active = render_target != 0 && render_target->IsActive();

            static int lastactive = 0;

            lastactive = active;

            if (view->IsActive() && active) {

                eSetCurrentRenderTarget(render_target);

                eSetOrthographicMatrixToHW();

                eView *player_view = eGetView(EVIEW_PLAYER1, true);

                eRenderSun(player_view);

                eFacePixelate(player_view);

                FEManager::Get()->Render();
            }
        }
    }

    eDisplayLetterBoxes();

    eDisplaySafezone();

    eDisplayEnvRenderTargets(eGetView(EVIEW_PLAYER1, true));

    eSetCurrentRenderTarget(0);

    eEndScene();

    PrintParticleSystemStats();

    eSwapFrameMallocBuffers();
    eSwapDynamicFrameMemory();

    ePostDisplay();

    UpdateTextureAnimations();

    eResestLightFlarePool();

    eFrameCounter++;
}

float GetVifTime() {
    return VifTime * (1.0f / 65536);
}

int DisplayCullingStats(int screen_x, int screen_y) {
    return 0;
}

int eGetScreenWidth() {
    return ScreenWidth;
}

int eGetScreenHeight() {
    return ScreenHeight;
}

void eSetScreenDisplayOffsets(int offset_x, int offset_y) {}

void eSetScreenDisplayOffsets(int &offset_x, int &offset_y) {}

TextureInfo *eRenderTarget::GetTextureInfo() {
    return &RenderTargetTextureInfos[static_cast<int>(this->ID)];
}

TextureInfo *eGetRenderTargetTextureInfo(int name_hash) {
    for (int i = FIRST_RENDER_TARGET; i < NUM_RENDER_TARGETS; i++) {
        eRenderTarget *render_target = eGetRenderTarget(i);
        TextureInfo *info = render_target->GetTextureInfo();
        if (name_hash == info->NameHash) {
            return info;
        }
    }
    return nullptr;
}

eRenderTarget *eGetCurrentRenderTarget() {
    return CurrentRenderTarget;
}

void eSetCurrentRenderTarget(eRenderTarget *render_target) {
    CurrentRenderTarget = render_target;
    if (CurrentRenderTarget) {
        if (_rmode->field_rendering && CurrentRenderTarget->ID <= 1u) {
            GXSetViewportJitter(CurrentRenderTarget->ScissorX, CurrentRenderTarget->ScissorY, CurrentRenderTarget->FrameWidth,
                                CurrentRenderTarget->FrameHeight / 2.0f, 0.0, 1.0, VIGetNextField());
        } else {
            GXSetViewport(CurrentRenderTarget->ScissorX, CurrentRenderTarget->ScissorY, CurrentRenderTarget->FrameWidth,
                          CurrentRenderTarget->FrameHeight, 0.0, 1.0);
        }
        eSetScissor(CurrentRenderTarget->ScissorX, CurrentRenderTarget->ScissorY, CurrentRenderTarget->ScissorW, CurrentRenderTarget->ScissorH);
        eSetBackgroundColor(CurrentRenderTarget->BackgroundColour);
        if (CurrentRenderTarget->GetCopyFilter() == FILTER_OFF) {
            eSetCopyFilter(CurrentRenderTarget->GetCopyFilter(), false);
        } else {
            eSetCopyFilter(CurrentRenderTarget->GetCopyFilter(), true);
        }
    }
}

static vu16 __sync_token = 0;
int EnableHarmonicClear = 1;
GXColor DOF_color = {255, 255, 255, 255};
GXFogType DOF_depth_curve = GX_FOG_PERSP_EXP2;
float DOF_znear = 1.0f;
float DOF_zfar = 80.0f;
float DOF_startz = 1.0f;
float DOF_endz = 256.0f;
unsigned char BLOOM_intensity_clip = 120;
GXColor BLOOM_intensity_sub = {28, 28, 28, 255};
GXColor BLOOM_intensity_add = {128, 128, 128, 160};

eRenderTarget *eGetRenderTarget(int render_target) {
    return &RenderTargets[render_target];
}

void eWaitUntilRenderingDone() {
    GXSetDrawSync(__sync_token);
    while (__sync_token != GXReadDrawSync()) {
        // nop
    }
    __sync_token++;
}

float CalculateH(unsigned short alpha) {
    unsigned short beta = alpha >> 1;
    float tan = bTan(beta);
    if (tan != 0.0f) {
        return 256.0f / tan;
    }
    return 10.0f;
}

void CreateViewMatricies(eView *view, float force_near_z, float force_far_z, float force_screen_far_z) {
    Camera *camera;
    eRenderTarget *render_target = view->GetRenderTarget0();
    eViewPlatInfo *plat_view = view->GetPlatInfo();
    float aspectscale;
    float nearz;
    float farz;
    float h;
    bMatrix4 mV2ST;

    camera = view->GetCamera();
    if (!camera) {
        bIdentity(view->GetPlatInfo()->GetWorldViewMatrix());
        bIdentity(view->GetPlatInfo()->GetViewScreenMatrix());
        return;
    }

    if (IsPal50Mode) {
        aspectscale = 0.86f;
    } else {
        aspectscale = 0.8f;
    }

    if (view->ID == EVIEW_PLAYER1 || view->ID == EVIEW_PLAYER2 || view->ID == EVIEW_PLAYER1_SPECULAR || view->ID == EVIEW_PLAYER2_SPECULAR) {
        aspectscale = Global3DAspectRatio;
    }

    nearz = camera->GetNearZ();
    farz = camera->GetFarZ();
    if (force_near_z != 0.0f) {
        nearz = force_near_z;
    }
    if (force_far_z != 0.0f) {
        farz = force_far_z;
    }
    view->NearZ = nearz;
    view->FarZ = farz;
    h = CalculateH(camera->GetFieldOfView());
    view->H = h;

    switch (view->ID) {
        case EVIEW_PLAYER1_RVM:
        case EVIEW_PLAYER1_SPECULAR:
        case EVIEW_PLAYER2_SPECULAR:
        case EVIEW_ENVMAP0F:
        case EVIEW_ENVMAP0R:
        case EVIEW_ENVMAP0B:
        case EVIEW_ENVMAP0L:
        case EVIEW_ENVMAP0U:
        case EVIEW_ENVMAP0D:
            plat_view->fovscl = 1.0f;
            plat_view->aspect = (float)ScreenWidth * aspectscale / (float)ScreenHeight;
            break;
        default:
            plat_view->fovscl = (float)render_target->FrameHeight / (float)eGetScreenHeight();
            plat_view->aspect = (float)render_target->FrameWidth * aspectscale / (float)render_target->FrameHeight;
            plat_view->fovscl *= PALefbxfbFOVscl;
            plat_view->aspect *= PALefbxfbAspect;
            break;
    }

    view->FovDegrees = bAngToDeg(camera->GetFieldOfView()) * plat_view->fovscl;
    view->ViewDirection = *camera->GetDirection();
    MTXPerspective(*reinterpret_cast<Mtx44 *>(view->GetPlatInfo()->GetViewScreenMatrix()), view->FovDegrees, plat_view->aspect, view->NearZ,
                   view->FarZ);

    eCopyMatrix(view->GetPlatInfo()->GetWorldViewMatrix(), camera->GetCameraMatrix());
    // TODO this is weird
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v0.y) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v1.y) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v2.y) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v3.y) ^= 0x80000000;

    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v0.z) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v1.z) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v2.z) ^= 0x80000000;
    *reinterpret_cast<unsigned int *>(&view->GetPlatInfo()->WorldViewMatrix.v3.z) ^= 0x80000000;
    eTransposeMatrix(&mV2ST, view->GetPlatInfo()->GetViewScreenMatrix());
    eMulMatrix(&view->GetPlatInfo()->WorldClipMatrix, &view->GetPlatInfo()->WorldViewMatrix, &mV2ST);

    for (int i = 0; i < 6; i++) {
        switch (i) {
            case 0:
                view->GetPlatInfo()->ClippingPlanes[0].x = view->GetPlatInfo()->WorldClipMatrix.v0.w + view->GetPlatInfo()->WorldClipMatrix.v0.x;
                view->GetPlatInfo()->ClippingPlanes[0].y = view->GetPlatInfo()->WorldClipMatrix.v1.w + view->GetPlatInfo()->WorldClipMatrix.v1.x;
                view->GetPlatInfo()->ClippingPlanes[0].z = view->GetPlatInfo()->WorldClipMatrix.v2.w + view->GetPlatInfo()->WorldClipMatrix.v2.x;
                view->GetPlatInfo()->ClippingPlanes[0].w = view->GetPlatInfo()->WorldClipMatrix.v3.w + view->GetPlatInfo()->WorldClipMatrix.v3.x;
                break;
            case 1:
                view->GetPlatInfo()->ClippingPlanes[1].x = view->GetPlatInfo()->WorldClipMatrix.v0.w - view->GetPlatInfo()->WorldClipMatrix.v0.x;
                view->GetPlatInfo()->ClippingPlanes[1].y = view->GetPlatInfo()->WorldClipMatrix.v1.w - view->GetPlatInfo()->WorldClipMatrix.v1.x;
                view->GetPlatInfo()->ClippingPlanes[1].z = view->GetPlatInfo()->WorldClipMatrix.v2.w - view->GetPlatInfo()->WorldClipMatrix.v2.x;
                view->GetPlatInfo()->ClippingPlanes[1].w = view->GetPlatInfo()->WorldClipMatrix.v3.w - view->GetPlatInfo()->WorldClipMatrix.v3.x;
                break;
            case 2:
                view->GetPlatInfo()->ClippingPlanes[2].x = view->GetPlatInfo()->WorldClipMatrix.v0.w - view->GetPlatInfo()->WorldClipMatrix.v0.y;
                view->GetPlatInfo()->ClippingPlanes[2].y = view->GetPlatInfo()->WorldClipMatrix.v1.w - view->GetPlatInfo()->WorldClipMatrix.v1.y;
                view->GetPlatInfo()->ClippingPlanes[2].z = view->GetPlatInfo()->WorldClipMatrix.v2.w - view->GetPlatInfo()->WorldClipMatrix.v2.y;
                view->GetPlatInfo()->ClippingPlanes[2].w = view->GetPlatInfo()->WorldClipMatrix.v3.w - view->GetPlatInfo()->WorldClipMatrix.v3.y;
                break;
            case 3:
                view->GetPlatInfo()->ClippingPlanes[i].x = view->GetPlatInfo()->WorldClipMatrix.v0.w + view->GetPlatInfo()->WorldClipMatrix.v0.y;
                view->GetPlatInfo()->ClippingPlanes[i].y = view->GetPlatInfo()->WorldClipMatrix.v1.w + view->GetPlatInfo()->WorldClipMatrix.v1.y;
                view->GetPlatInfo()->ClippingPlanes[i].z = view->GetPlatInfo()->WorldClipMatrix.v2.w + view->GetPlatInfo()->WorldClipMatrix.v2.y;
                view->GetPlatInfo()->ClippingPlanes[i].w = view->GetPlatInfo()->WorldClipMatrix.v3.w + view->GetPlatInfo()->WorldClipMatrix.v3.y;
                break;
            case 4:
                view->GetPlatInfo()->ClippingPlanes[4].x = -view->GetPlatInfo()->WorldClipMatrix.v0.z;
                view->GetPlatInfo()->ClippingPlanes[4].y = -view->GetPlatInfo()->WorldClipMatrix.v1.z;
                view->GetPlatInfo()->ClippingPlanes[4].z = -view->GetPlatInfo()->WorldClipMatrix.v2.z;
                view->GetPlatInfo()->ClippingPlanes[4].w = -view->GetPlatInfo()->WorldClipMatrix.v3.z;
                break;
            case 5:
                view->GetPlatInfo()->ClippingPlanes[5].x = view->GetPlatInfo()->WorldClipMatrix.v0.w + view->GetPlatInfo()->WorldClipMatrix.v0.z;
                view->GetPlatInfo()->ClippingPlanes[5].y = view->GetPlatInfo()->WorldClipMatrix.v1.w + view->GetPlatInfo()->WorldClipMatrix.v1.z;
                view->GetPlatInfo()->ClippingPlanes[5].z = view->GetPlatInfo()->WorldClipMatrix.v2.w + view->GetPlatInfo()->WorldClipMatrix.v2.z;
                view->GetPlatInfo()->ClippingPlanes[5].w = view->GetPlatInfo()->WorldClipMatrix.v3.w + view->GetPlatInfo()->WorldClipMatrix.v3.z;
                break;
        }

        {
            float len = 1.0f / bLength(reinterpret_cast<bVector3 *>(&view->GetPlatInfo()->ClippingPlanes[i]));

            view->GetPlatInfo()->ClippingPlanes[i].x *= len;
            view->GetPlatInfo()->ClippingPlanes[i].y *= len;
            view->GetPlatInfo()->ClippingPlanes[i].z *= len;
            view->GetPlatInfo()->ClippingPlanes[i].w *= len;
        }
    }
}

extern cSphereMap SphereMap;
extern cSpecularMap SpecularMap;
extern cQuarterSizeMap QSizeScratchPad;

// NON_MATCHING: 81.8% - store order matches now; frame_height splits into two pseudos (r26+r30 vs r30),
// which shifts every callee-saved register by one
void SetScreenBuffers() {
    bMatrix4 *identity = eGetIdentityMatrix();
    int frame_width = ScreenWidth;
    int frame_height = ScreenHeight;
    GXColor background_color = {0, 0, 0, 0xFF};

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_FLAYER);

        rc->SetID(TARGET_FLAYER);
        rc->SetName("TARGET_FLAYER");
        rc->SetActive(1);
        rc->SetCopyFilterID(FILTER_DEFAULT);
        background_color.r = 0;
        background_color.a = 0xFF;
        background_color.g = 0;
        background_color.b = 0;
        rc->SetBackgroundColour(background_color);
        rc->ScissorW = frame_width;
        rc->ScissorH = frame_height;
        rc->FrameWidth = frame_width;
        rc->FrameHeight = frame_height;
        rc->ClearBackground = 1;
        rc->ScissorX = 0;
        rc->ScissorY = 0;
        rc->FrameAddress = 0;
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_PLAYER1);

        rc->SetID(TARGET_PLAYER1);
        rc->SetName("TARGET_PLAYER1");
        rc->SetActive(1);
        rc->SetCopyFilterID(FILTER_DEFAULT);
        background_color.r = 0;
        background_color.a = 0xFF;
        background_color.g = 0;
        background_color.b = 0;
        rc->SetBackgroundColour(background_color);
        rc->ScissorW = frame_width;
        rc->ScissorH = frame_height;
        rc->FrameWidth = frame_width;
        rc->FrameHeight = frame_height;
        rc->ClearBackground = 1;
        rc->ScissorX = 0;
        rc->ScissorY = 0;
        rc->FrameAddress = 0;
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_PLAYER2);

        rc->SetID(TARGET_PLAYER2);
        rc->SetName("TARGET_PLAYER2");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_DEFAULT);
        background_color.r = 0;
        background_color.a = 0xFF;
        background_color.g = 0;
        background_color.b = 0;
        rc->SetBackgroundColour(background_color);
        rc->ScissorY = frame_height / 2;
        rc->ScissorW = frame_width;
        rc->ScissorH = frame_height / 2;
        rc->FrameWidth = frame_width;
        rc->FrameHeight = frame_height / 2;
        rc->ClearBackground = 1;
        rc->ScissorX = 0;
        rc->FrameAddress = 0;
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_PLAYER1_RVM);

        rc->SetID(TARGET_PLAYER1_RVM);
        rc->SetName("TARGET_PLAYER1_RVM");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_DEFAULT);
        background_color.r = 0;
        background_color.a = 0xFF;
        background_color.g = 0;
        background_color.b = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = SphereMap.cubeBuffer[0].xOrigin;
        rc->ScissorY = SphereMap.cubeBuffer[0].yOrigin;
        rc->ScissorW = SphereMap.cubeBuffer[0].width;
        rc->ScissorH = SphereMap.cubeBuffer[0].height;
        rc->FrameWidth = SphereMap.cubeBuffer[0].width;
        rc->FrameHeight = SphereMap.cubeBuffer[0].height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(SphereMap.cubeBuffer[0].pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_PLAYER1_SPECULAR);

        rc->SetID(TARGET_PLAYER1_SPECULAR);
        rc->SetName("TARGET_PLAYER1_SPECULAR");
        rc->SetActive(1);
        rc->SetCopyFilterID(FILTER_REFLECTION);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = SpecularMap.specBuffer[0].xOrigin;
        rc->ScissorY = SpecularMap.specBuffer[0].yOrigin;
        rc->ScissorW = SpecularMap.specBuffer[0].width;
        rc->ScissorH = SpecularMap.specBuffer[0].height;
        rc->FrameWidth = SpecularMap.specBuffer[0].width;
        rc->FrameHeight = SpecularMap.specBuffer[0].height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(SpecularMap.specBuffer[0].pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_PLAYER2_SPECULAR);

        rc->SetID(TARGET_PLAYER2_SPECULAR);
        rc->SetName("TARGET_PLAYER2_SPECULAR");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_DEFAULT);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = SpecularMap.specBuffer[1].xOrigin;
        rc->ScissorY = SpecularMap.specBuffer[1].yOrigin;
        rc->ScissorW = SpecularMap.specBuffer[1].width;
        rc->ScissorH = SpecularMap.specBuffer[1].height;
        rc->FrameWidth = SpecularMap.specBuffer[1].width;
        rc->FrameHeight = SpecularMap.specBuffer[1].height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(SpecularMap.specBuffer[1].pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_QUADRANT_TOP_LEFT);

        rc->SetID(TARGET_QUADRANT_TOP_LEFT);
        rc->SetName("TARGET_QUADRANT_TOP_LEFT");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_OFF);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = 0;
        rc->ScissorY = 0;
        rc->ScissorW = QSizeScratchPad.quarterSizeBuffer.width;
        rc->ScissorH = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameWidth = QSizeScratchPad.quarterSizeBuffer.width;
        rc->FrameHeight = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(QSizeScratchPad.quarterSizeBuffer.pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_QUADRANT_TOP_RIGHT);

        rc->SetID(TARGET_QUADRANT_TOP_RIGHT);
        rc->SetName("TARGET_QUADRANT_TOP_RIGHT");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_OFF);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = 0x140;
        rc->ScissorY = 0;
        rc->ScissorW = QSizeScratchPad.quarterSizeBuffer.width;
        rc->ScissorH = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameWidth = QSizeScratchPad.quarterSizeBuffer.width;
        rc->FrameHeight = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(QSizeScratchPad.quarterSizeBuffer.pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_QUADRANT_BOTTOM_LEFT);

        rc->SetID(TARGET_QUADRANT_BOTTOM_LEFT);
        rc->SetName("TARGET_QUADRANT_BOTTOM_LEFT");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_OFF);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = 0;
        rc->ScissorY = 0xF0;
        rc->ScissorW = QSizeScratchPad.quarterSizeBuffer.width;
        rc->ScissorH = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameWidth = QSizeScratchPad.quarterSizeBuffer.width;
        rc->FrameHeight = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(QSizeScratchPad.quarterSizeBuffer.pCaptureTexture);
    }

    {
        eRenderTarget *rc = eGetRenderTarget(TARGET_QUADRANT_BOTTOM_RIGHT);

        rc->SetID(TARGET_QUADRANT_BOTTOM_RIGHT);
        rc->SetName("TARGET_QUADRANT_BOTTOM_RIGHT");
        rc->SetActive(0);
        rc->SetCopyFilterID(FILTER_OFF);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = 0x140;
        rc->ScissorY = 0xF0;
        rc->ScissorW = QSizeScratchPad.quarterSizeBuffer.width;
        rc->ScissorH = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameWidth = QSizeScratchPad.quarterSizeBuffer.width;
        rc->FrameHeight = QSizeScratchPad.quarterSizeBuffer.height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(QSizeScratchPad.quarterSizeBuffer.pCaptureTexture);
    }

    for (int i = 0; i < 6; i++) {
        eRenderTarget *rc = eGetRenderTarget(i + 10);

        rc->SetID(i + 10);
        rc->SetName(EnvmapTargetNames[i]);
        rc->SetActive(1);
        rc->SetCopyFilterID(FILTER_CUBE_FACES);
        background_color.r = 0;
        background_color.g = 0;
        background_color.b = 0;
        background_color.a = 0;
        rc->SetBackgroundColour(background_color);
        rc->ClearBackground = 1;
        rc->ScissorX = SphereMap.cubeBuffer[i].xOrigin + 1;
        rc->ScissorY = SphereMap.cubeBuffer[i].yOrigin + 1;
        rc->ScissorW = SphereMap.cubeBuffer[i].width - 2;
        rc->ScissorH = SphereMap.cubeBuffer[i].height - 2;
        rc->FrameWidth = SphereMap.cubeBuffer[i].width;
        rc->FrameHeight = SphereMap.cubeBuffer[i].height;
        rc->FrameAddress = reinterpret_cast<intptr_t>(SphereMap.cubeBuffer[i].pCaptureTexture);
    }

    for (int i = 0; i < 17; i++) {
        eRenderTarget *rc = eGetRenderTarget(i);

        rc->WorldClip = identity;
        rc->WorldView = identity;
        rc->ViewScreen = identity;
    }
}

// int epSetAllStripsVisibleState(eSolid *param1, int param2) {
//   param1->GetNext();
//   return 0;
// }

extern int epRenderStrips_psReset;
extern int epRenderStrips_ps_NoLighting;
extern int EnableHarmonicClear;
extern GXColor DOF_color;
extern GXFogType DOF_depth_curve;
extern float DOF_znear;
extern float DOF_zfar;
extern float DOF_startz;
extern float DOF_endz;
extern unsigned char BLOOM_intensity_clip;
extern GXColor BLOOM_intensity_sub;
extern GXColor BLOOM_intensity_add;
extern int SphericalPS;
extern float testl1[2];
extern float testl2[2];
extern float testl3[2];
extern float teste1[2];
extern float teste2[2];
extern float teste3[2];
extern float testc0[2];
extern float testc1[2];
extern cQuarterSizeMap QSizeAccumulationI8;

int epRenderStrips(eView *view, eSolid *solid, bMatrix4 *local_world, eLightContext *light_context, unsigned int flags,
                   bMatrix4 *blending_matrices) {
    if (solid == 0) {
        return 0;
    }

    eRenderTarget *current_render_target = eGetCurrentRenderTarget();
    int render_target_id = current_render_target->GetID();

    eViewPlatInfo *view_plat_info = view->GetPlatInfo();
    bMatrix4 *world_view = view_plat_info->GetWorldViewMatrix();
    bMatrix4 *world_clip = view_plat_info->GetWorldClipMatrix();

    eSolidPlatInfo *solid_plat_info = solid->GetPlatInfo();

    eLightMaterial *default_light_material = elGetDefaultLightMaterial();
    eTextureEntry *texture_table = solid->pTextureTable;
    int num_texture_entries = solid->NumTextureTableEntries;
    int num_strip_entries = solid_plat_info->NumStrips;
    eStripEntry *strip_entry = solid_plat_info->StripEntryTable;
    eStripEntry *sentinal_strip_entry = strip_entry + num_strip_entries;

    if (num_strip_entries == 0) {
        return 0;
    }

    unsigned char *strip_data_start = solid_plat_info->StripDataStart;
    unsigned short *position_table_16 = (unsigned short *)(strip_data_start + solid_plat_info->DataOffset0);
    int *normal_table = (int *)(strip_data_start + solid_plat_info->DataOffset1);
    unsigned int *colour_table1 = (unsigned int *)(strip_data_start + solid_plat_info->DataOffset2);
    unsigned int *colour_table0 = colour_table1;

    eDataRenderDynamic drd;
    Mtx *texcoordgen_reflection_matrix = 0;

    if (blending_matrices == 0) {
        if (SphericalPS && light_context != 0 && (solid_plat_info->StripFlags & 0x4180)) {
            int num_colour_entries = solid->GetPlatInfo()->NumIdxClrTable + 1;

            unsigned int *lit_colour_table0 = (unsigned int *)eFrameMalloc(num_colour_entries * sizeof(unsigned int) + 32);
            unsigned int *lit_colour_table1 = (unsigned int *)eFrameMalloc(num_colour_entries * sizeof(unsigned int) + 32);

            if (lit_colour_table0 != 0 && lit_colour_table1 != 0) {
                lit_colour_table0 = (unsigned int *)roundup((unsigned int)lit_colour_table0, 32);
                lit_colour_table1 = (unsigned int *)roundup((unsigned int)lit_colour_table1, 32);

                if (EnableHarmonicClear) {
                    bMemSet(lit_colour_table0, 0, num_colour_entries * sizeof(unsigned int));
                }

                eLightMaterialEntry *light_material_table = solid->LightMaterialTable;
                int num_light_material_entries = solid->NumLightMaterials;

                for (int strip_index = 0; strip_index < solid_plat_info->NumStrips; strip_index++) {
                    eStripEntry *lit_strip_entry = &solid_plat_info->StripEntryTable[strip_index];

                    if (lit_strip_entry->Flags & 0x4180) {
                        eLightMaterial *light_material = default_light_material;
                        int light_material_index = lit_strip_entry->LightMaterialIndex;

                        if (light_material_index >= 0 && light_material_index < num_light_material_entries) {
                            light_material = light_material_table[light_material_index].LightMaterial;

                            if (light_material == 0) {
                                light_material = default_light_material;
                            }
                        }

                        unsigned char *index_buffer = (unsigned char *)lit_strip_entry->DataOffset;
                        int num_indicies = lit_strip_entry->NumVerts;

                        int vertex_description = lit_strip_entry->VertexDescription;

                        epCalculateLocalDirectionalPOS16(lit_colour_table0, lit_colour_table1, num_colour_entries, position_table_16,
                                                         normal_table, index_buffer, vertex_description, num_indicies, light_material,
                                                         light_context);
                    }
                }

                DCStoreRange(lit_colour_table0, num_colour_entries * sizeof(unsigned int));
                DCStoreRange(lit_colour_table1, num_colour_entries * sizeof(unsigned int));

                colour_table0 = lit_colour_table0;
                colour_table1 = lit_colour_table1;
            }
        }

        if (light_context == 0 || (solid_plat_info->StripFlags & 0x80) == 0) {
            if (solid_plat_info->StripFlags & 0x40) {
                bMatrix4 mL2V;
                Mtx nMtx;

                eMulMatrix(&mL2V, local_world, world_view);

                eConvertToGX34(nMtx, mL2V);

                texcoordgen_reflection_matrix = (Mtx *)eFrameMalloc(sizeof(Mtx));

                eView *reflection_view = view->GetPlatInfo()->SpecularView;
                if (reflection_view != 0) {
                    eViewPlatInfo *reflection_view_plat_info = reflection_view->GetPlatInfo();

                    PSMTXConcat(*reflection_view_plat_info->GetLightPerspectiveProjection(), nMtx, *texcoordgen_reflection_matrix);
                }
            }
        }
    }

    do {
        int current_poly_group = strip_entry->PolyGroupNumber;
        int texture_index = strip_entry->TextureNumber;
        TextureInfo *texture_info = texture_table[texture_index].pTextureInfo;

        eLightMaterial *light_material = default_light_material;
        if (solid->LightMaterialTable != 0) {
            if (strip_entry->LightMaterialIndex >= 0 && strip_entry->LightMaterialIndex < solid->NumLightMaterials) {
                light_material = solid->LightMaterialTable[strip_entry->LightMaterialIndex].LightMaterial;
            }
        }

        eStripEntry *last_group_strip_entry = strip_entry;
        unsigned short num_group_strip = 1;

        while ((last_group_strip_entry + 1)->PolyGroupNumber == current_poly_group && (last_group_strip_entry + 1) < sentinal_strip_entry) {
            last_group_strip_entry++;
            num_group_strip++;
        }

        drd.colourtable0 = colour_table0;
        drd.colourtable1 = colour_table1;

        drd.trm = texcoordgen_reflection_matrix;

        eSubmitMesh(strip_entry, num_group_strip, view, solid, flags, texture_info, local_world, light_context, light_material,
                    blending_matrices, &drd);

        strip_entry = last_group_strip_entry + 1;
    } while (strip_entry < sentinal_strip_entry);

    if (epRenderStrips_ps_NoLighting) {
        ps_NoLighting(0, 0);
    }

    if (epRenderStrips_psReset) {
        psReset(RESET_OPTION_MODULATE);
    }

    return 1;
}

extern int ForceFERenderStates;

void eViewPlatInterface::FEBeginBatchRender(int numPolys) {
    ForceFERenderStates = 1;
}

void eViewPlatInterface::FEEndBatchRender() {
    ForceFERenderStates = 0;
}

void eViewPlatInterface::Render(eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, uint32 flags, bMatrix4 *blending_matricies) {
    eSolid *solid = model->GetSolid();
    eView *view = (eView *)this;

    if (solid == nullptr || local_to_world == nullptr) {
        return;
    }

    SimpleModelAnim::Animate(model, solid, local_to_world);

    eVisibleState visibility_state = (eVisibleState)2;
    int num_strips_visible;

    if (!(flags & 4)) {
        eSolidPlatInfo *solid_plat_info = solid->GetPlatInfo();
        eViewPlatInfo *view_plat_info = GetPlatInfo();
        bMatrix4 local_clipsb;
        visibility_state = GetVisibleStateSB((const bVector3 *)&solid->AABBMinX, (const bVector3 *)&solid->AABBMaxX, local_to_world);
    }

    if (visibility_state != 0) {
        TextureInfo **replaced_textures[128];
        model->ApplyReplacementTextureTable(replaced_textures);

        num_strips_visible = epRenderStrips(view, solid, local_to_world, light_context, flags, blending_matricies);

        model->RestoreReplacementTextureTable(replaced_textures);
    }
}

void eViewPlatInterface::Render(ePoly *poly, TextureInfo *texture_info, bMatrix4 *local_to_world, int use_previous_data, float bbRad) {
    if (poly != nullptr && texture_info != nullptr) {
        bMatrix4 mat;

        eMulMatrix(&mat, local_to_world, &GetPlatInfo()->WorldViewMatrix);
        eLoadPosMtxImm(mat, GX_PNMTX0);
        Render(poly, texture_info, use_previous_data);
    }
}

void eViewPlatInterface::FERender(ePoly *poly, TextureInfo *texture_info, TextureInfo *texture_info_mask, int use_previous_data) {
    Render(poly, texture_info, texture_info_mask, use_previous_data);
}

void eViewPlatInterface::Render(ePoly *poly, TextureInfo *texture_info, TextureInfo *texture_info_mask, int use_previous_data) {
    if (texture_info_mask != nullptr) {
        eSetTexture(texture_info_mask, 1);
    }

    Render(poly, texture_info, 0x72);
}

void eViewPlatInterface::FERender(ePoly *poly, TextureInfo *texture_info, int use_previous_data) {
    Render(poly, texture_info, use_previous_data);
}

struct ePolyUVPairs {
    float UV[4][2];
};

void eViewPlatInterface::Render(ePoly *poly, TextureInfo *texture_info, int use_previous_data) {
    extern TextureInfo *pContrastRampTextureInfo;
    // 5285
    if (poly == nullptr || texture_info == nullptr) {
        return;
    }

    // ~5296
    eView *view = (eView *)this;
    // 5297
    ScreenEffectDB *SE_db = view->ScreenEffects;

    // 5299
    eSetTexture(texture_info, 0);
    // 5300
    eSetBlendMode(texture_info, 0);

    // ~5303 / ~5304   (declaradas, sin inicializar: el DWARF las pone aqui)
    TextureInfo *TEV_stage1_texture_info;
    TextureInfo *TEV_stage2_texture_info;

    // 5309
    int crtVtxFmt = vsScreen(0);
    // ~5311
    const int nVerts = 4;

    // 5316
    switch (use_previous_data) {
    // ~5318   cuerpo VACIO: no emite nada pero cuenta en el arbol del switch
    case 0x64:
        break;

    // ~5327
    case 0x66:
        // ~5328
        TEV_stage1_texture_info = QSizeAccumulationI8.quarterSizeBuffer.GetTexture();
        // 5329
        if (TEV_stage1_texture_info != nullptr) {
            // 5331
            eSetTexture(TEV_stage1_texture_info, 1);
        }
        // 5334
        psGlowBloom(BLOOM_intensity_clip);
        // ~5336 (cola duplicada -> cross-jumping con el case 0x72)
        eSetBlendModeSrcInvSrc();
        // ~5337
        eSetAlphaTest(0);
        // ~5338
        eSetZBuffering(0, 0);
        // 5340
        break;

    // ~5344
    case 0x67:
        // ~5345
        TEV_stage1_texture_info = QSizeI8_Z8.quarterSizeBuffer.GetTexture();
        // 5346
        if (TEV_stage1_texture_info != nullptr) {
            // 5348
            eSetTexture(TEV_stage1_texture_info, 1);
        }
        // 5357
        psMotionBlur();
        // ~5359 (cola duplicada)
        eSetBlendModeSrcInvSrc();
        // ~5361
        eSetAlphaTest(0);
        // ~5363
        eSetZBuffering(0, 0);
        // 5365
        break;

    // ~5367
    case 0x65:
        // 5368
        eSetBlendModeSrcInvSrc();
        // 5369
        eSetAlphaTest(0);
        // 5370
        eSetZBuffering(0, 0);
        // 5372
        psRVM();
        // 5374
        break;

    // ~5376
    case 0x6b:
        // 5377
        eSetBlendModeNone();
        // 5378
        eSetAlphaTest(0);
        // 5379
        eSetZBuffering(0, 0);
        // 5381
        psIntensityReplacement(BLOOM_intensity_sub);
        // 5383
        break;

    // ~5385
    case 0x6c:
        // 5386
        eSetBlendModeSrcAlphaOne();
        // 5387
        eSetAlphaTest(0);
        // 5388
        eSetZBuffering(0, 0);
        // 5391
        if (SE_db != nullptr) {
            // 5393
            if (SE_db->IsActive(SE_GLARE)) {
                // 5395
                BLOOM_intensity_add.a = (unsigned char)(SE_db->GetIntensity(SE_GLARE) * 127.0f + 128.0f);
            }
        }
        // 5403
        psIntensityAccumulate(BLOOM_intensity_add);
        // 5405
        break;

    // ~5409
    case 0x70:
        // 5410
        TEV_stage1_texture_info = pContrastRampTextureInfo;
        // 5411
        if (TEV_stage1_texture_info != nullptr) {
            // 5413
            eSetTexture(TEV_stage1_texture_info, 1);
        }
        // ~5417
        TEV_stage2_texture_info = QSizeI8_Z8.quarterSizeBuffer.GetTexture();
        // 5419
        if (TEV_stage2_texture_info != nullptr) {
            // 5421
            eSetTexture(TEV_stage2_texture_info, 2);
        }
        // 5427
        eSetTevSwapStage(GX_TEVSTAGE2, 0, 1);
        // 5428
        eSetTevSwapStage(GX_TEVSTAGE3, 0, 2);
        // 5433
        eSetBlendModeNone();
        // 5436
        eSetAlphaTest(0);
        // 5437
        eSetZBuffering(0, 0);
        // 5439
        psMWScreenContrast();
        // 5440
        break;

    // ~5442
    case 0x6a:
        // 5443
        eSetBlendModeNone();
        // 5444
        eSetAlphaTest(0);
        // 5445
        eSetZBuffering(0, 0);
        // 5447
        psModulate();
        // 5448
        break;

    // ~5450 / ~5451 / ~5452
    case 0x6d:
    case 0x6e:
    case 0x6f:
        // 5453
        eSetBlendModeSrcInvSrc();
        // 5455
        eSetAlphaTest(0);
        // 5456
        eSetZBuffering(0, 0);
        // 5457
        psTint();
        // 5459
        break;

    // ~5461
    case 0x68:
        // 5462
        GXSetFog(DOF_depth_curve, DOF_startz, DOF_endz, DOF_znear, DOF_zfar, DOF_color);
        // 5463
        GXSetZTexture(GX_ZT_REPLACE, GX_TF_Z24X8, 0);
        // 5465
        psDepthTexture();
        // 5467
        eSetBlendModeNone();
        // 5468 (cola duplicada -> cross-jumping con el case 0x72, sin el blend mode)
        eSetAlphaTest(0);
        // 5469
        eSetZBuffering(0, 0);
        // 5470
        break;

    // ~5473
    case 0x69:
        // ~5474
        TEV_stage1_texture_info = QSizeI8_Z8.quarterSizeBuffer.GetTexture();
        // 5475
        if (TEV_stage1_texture_info != nullptr) {
            // 5477
            eSetTexture(TEV_stage1_texture_info, 1);
        }
        // 5480
        psDepthOfField(40);
        // ~5482 (cola duplicada)
        eSetBlendModeSrcInvSrc();
        // ~5483
        eSetAlphaTest(0);
        // ~5484
        eSetZBuffering(0, 0);
        // 5486
        break;

    // ~5488
    case 0x72:
        // 5489
        crtVtxFmt = vsScreenMultiTexture(0);
        // 5491
        psFEMultiTexture();
        // 5492  <-- copia superviviente de la cola
        eSetBlendModeSrcInvSrc();
        // 5494
        eSetAlphaTest(0);
        // 5495
        eSetZBuffering(0, 0);
        // 5497
        break;

    // ~5499
    default:
        // 5500
        psModulate();
        // ~5501
        break;
    }

    // 5505
    ps_NoLighting(1, 0);

    // 5513
    if (crtVtxFmt == GX_VTXFMT6) {
        // 5517
        GXBegin(GX_QUADS, GX_VTXFMT6, nVerts);

        // 5518
        for (int i = 0; i < nVerts; i++) {
            // 5520
            GXPosition3f32(poly->Vertices[i].x, poly->Vertices[i].y, poly->Vertices[i].z);
            // 5521
            GXColor1u32(*reinterpret_cast<unsigned int *>(poly->Colours[i]));
            // 5522
            GXTexCoord2f32(((ePolyUVPairs *)poly->UVs)->UV[i][0], ((ePolyUVPairs *)poly->UVs)->UV[i][1]);
            // 5523
            GXTexCoord2f32(((ePolyUVPairs *)poly->UVsMask)->UV[i][0], ((ePolyUVPairs *)poly->UVsMask)->UV[i][1]);
            // 5524
        }

        // 5525
        GXEnd();
    } else {
        // 5553
        GXBegin(GX_QUADS, (GXVtxFmt)crtVtxFmt, nVerts);

        // 5554
        for (int i = 0; i < nVerts; i++) {
            // 5556
            GXPosition3f32(poly->Vertices[i].x, poly->Vertices[i].y, poly->Vertices[i].z);
            // 5557
            GXColor1u32(*reinterpret_cast<unsigned int *>(poly->Colours[i]));
            // 5558
            GXTexCoord2f32(((ePolyUVPairs *)poly->UVs)->UV[i][0], ((ePolyUVPairs *)poly->UVs)->UV[i][1]);
            // 5559
        }

        // ~5561
        GXEnd();
    }

    // 5574
    if (use_previous_data == 0x70) {
        // 5576
        eResetZBuffering();
        // 5577
        eResetTevSwapStages();
    }
    // 5581
    if (use_previous_data == 0x68) {
        // 5584
        GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z24X8, 0);
        // 5586
        GXSetFog(GX_FOG_NONE, DOF_startz, DOF_endz, DOF_znear, DOF_zfar, DOF_color);
    }

    // 5590
    ps_NoLighting(0, 0);
    // 5591
}

void DisplayRVMs(eView *view) {
    if (!FEngHud::ShouldRearViewMirrorBeVisible(EVIEW_PLAYER1)) {
        return;
    }
    TextureInfo *player1_rvm_mask_info = pTextureInfoRVMMask;
    if (player1_rvm_mask_info == nullptr) {
        return;
    }

    ePoly poly;

    poly.UVs[0][0] = 1.0f;
    poly.UVs[0][1] = 0.0f;
    poly.UVs[0][2] = 0.0f;
    poly.UVs[0][3] = 0.0f;
    poly.UVs[1][0] = 0.0f;
    poly.UVs[1][1] = 1.0f;
    poly.UVs[1][2] = 1.0f;
    poly.UVs[1][3] = 1.0f;
    poly.UVsMask[0][0] = 1.0f;
    poly.UVsMask[0][1] = 0.0f;
    poly.UVsMask[0][2] = 0.0f;
    poly.UVsMask[0][3] = 0.0f;
    poly.UVsMask[1][0] = 0.0f;
    poly.UVsMask[1][1] = 1.0f;
    poly.UVsMask[1][2] = 1.0f;
    poly.UVsMask[1][3] = 1.0f;

    for (int view_id = EVIEW_FIRST_RVM; view_id <= EVIEW_LAST_RVM; view_id++) {
        eView *rvm_view = eGetView(view_id, false);
        eRenderTarget *rvm_target = rvm_view->GetRenderTarget();
        TextureInfo *texture_info = SphereMap.cubeBuffer[0].GetTexture();
        if (rvm_view->IsActive() && rvm_target->IsActive()) {
            eSetTexture(player1_rvm_mask_info, 1);
            poly.Vertices[0].x = 192.0f;
            poly.Vertices[0].y = 24.0f;
            poly.Vertices[0].z = 9000.0f;
            poly.Vertices[1].x = 452.0f;
            poly.Vertices[1].y = 24.0f;
            poly.Vertices[1].z = 9000.0f;
            poly.Vertices[2].x = 452.0f;
            poly.Vertices[2].y = 88.0f;
            poly.Vertices[2].z = 9000.0f;
            poly.Vertices[3].x = 192.0f;
            poly.Vertices[3].y = 88.0f;
            poly.Vertices[3].z = 9000.0f;
            view->Render(&poly, texture_info, 0x65);
        }
    }
}

void eFacePixelate(eView *view) {
    FacePixelation *pixelation = view->facePixelation;

    if (pixelation == nullptr) {
        return;
    }
    if (!pixelation->IsEnabled()) {
        return;
    }

    {
        static float downSampledWidth = 80.0f;
        static float downSampledHeight = 60.0f;

        eRenderTarget *view_render_target = view->GetRenderTarget();
        if (view_render_target != nullptr) {
            eSetOrthographicScreenQuadProjection(view_render_target);
        }

        view->facePixelation->Render();

        float screenX, screenY, width, height;
        pixelation->GetData(&screenX, &screenY, &width, &height);

        screenX -= width * 0.5f;
        screenY -= height * 0.5f;

        TextureInfo *saveBuffer = QSizeI8_Z8.quarterSizeBuffer.GetTexture();
        TextureInfo *downSample = QSizeScratchPad.quarterSizeBuffer.GetTexture();

        saveBuffer->Width = 160;
        saveBuffer->Height = 120;
        saveBuffer->GetPlatInfo()->Format = GX_TF_RGBA8;
        saveBuffer->GetPlatInfo()->SetImage(saveBuffer);

        eSetCopyFilter(FILTER_PIXELATE, true);

        GXInitTexObjLOD(&downSample->GetPlatInfo()->ImageInfos.obj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

        QSizeI8_Z8.quarterSizeBuffer.SetEFBCaptureRegion(0, 0, 160, 120);
        QSizeI8_Z8.quarterSizeBuffer.CaptureEFB(0, 0, GX_TF_RGBA8);

        QSizeScratchPad.quarterSizeBuffer.SetEFBCaptureRegion(0, 0, 320, 240);
        QSizeScratchPad.quarterSizeBuffer.CaptureEFB(0, 1, GX_TF_RGBA8);

        {
            ePoly poly;

            poly.UVs[0][0] = 0.0f;
            poly.UVs[0][1] = 0.0f;
            poly.UVs[0][2] = 1.0f;
            poly.UVs[0][3] = 0.0f;
            poly.UVs[1][0] = 1.0f;
            poly.UVs[1][1] = 1.0f;
            poly.UVs[1][2] = 0.0f;
            poly.UVs[1][3] = 1.0f;

            poly.Vertices[0].x = 0.0f;
            poly.Vertices[0].y = 0.0f;
            poly.Vertices[1].x = downSampledWidth;
            poly.Vertices[1].y = 0.0f;
            poly.Vertices[2].x = downSampledWidth;
            poly.Vertices[2].y = downSampledHeight;
            poly.Vertices[3].x = 0.0f;
            poly.Vertices[3].y = downSampledHeight;

            view->Render(&poly, downSample, 0x6a);
        }

        QSizeScratchPad.quarterSizeBuffer.SetEFBCaptureRegion(0, 0, 320, 240);
        QSizeScratchPad.quarterSizeBuffer.CaptureEFB(0, 0, GX_TF_RGBA8);

        {
            ePoly poly;

            poly.UVs[0][0] = 0.0f;
            poly.UVs[0][1] = 0.0f;
            poly.UVs[0][2] = 1.0f;
            poly.UVs[0][3] = 0.0f;
            poly.UVs[1][0] = 1.0f;
            poly.UVs[1][1] = 1.0f;
            poly.UVs[1][2] = 0.0f;
            poly.UVs[1][3] = 1.0f;

            poly.Vertices[0].x = 0.0f;
            poly.Vertices[0].y = 0.0f;
            poly.Vertices[1].x = 160.0f;
            poly.Vertices[1].y = 0.0f;
            poly.Vertices[2].x = 160.0f;
            poly.Vertices[2].y = 120.0f;
            poly.Vertices[3].x = 0.0f;
            poly.Vertices[3].y = 120.0f;

            view->Render(&poly, saveBuffer, 0x6a);
        }

        {
            ePoly poly;

            poly.Vertices[0].x = screenX;
            poly.Vertices[0].y = screenY;
            poly.Vertices[1].x = screenX + width;
            poly.Vertices[1].y = screenY;
            poly.Vertices[2].x = screenX + width;
            poly.Vertices[2].y = screenY + height;
            poly.Vertices[3].x = screenX;
            poly.Vertices[3].y = screenY + height;

            float u_scale = downSampledWidth * 4.882812845607987e-06f;
            float v_scale = downSampledHeight * 8.680556675244588e-06f;

            for (int i = 0; i < 4; i++) {
                ((ePolyUVPairs *)poly.UVs)->UV[i][0] = poly.Vertices[i].x * u_scale;
                ((ePolyUVPairs *)poly.UVs)->UV[i][1] = poly.Vertices[i].y * v_scale;
            }

            view->Render(&poly, downSample, 0x6a);
        }

        saveBuffer->Width = 320;
        saveBuffer->Height = 240;
        saveBuffer->GetPlatInfo()->Format = GX_TF_I8;
        saveBuffer->GetPlatInfo()->SetImage(saveBuffer);
        downSample->GetPlatInfo()->SetImage(downSample);

        eSetCopyFilter(FILTER_DEFAULT, true);

        eSetOrthographicMatrixToHW();
    }
}

Bool bProgressiveScan = false;
Bool bEURGB60 = false;
int _firstFrame = 2;
void *_frameBuffer1 = nullptr;
void *_frameBuffer2 = nullptr;
void *_currentBuffer = nullptr;
int _GxInitialized = 0;
static Bool e_bDither = false;
void *_defaultFIFO = 0;
GXFifoObj *_defaultFIFOObj = 0;
volatile Bool bHangDiagnose = 0;
vu16 e_sync = 0;
vu16 e_endsync = 0;
volatile unsigned short last_sync_token = 0;
volatile int e_resync = 0;
volatile int e_keepalive = 0;
Bool bStallWorkaround = 0;
Bool bDLSaveContext = 0;
unsigned char CopyFilter[11][7] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                                   {0x08, 0x08, 0x0A, 0x0C, 0x0A, 0x08, 0x08},
                                   {0x08, 0x08, 0x10, 0x12, 0x10, 0x08, 0x08},
                                   {0x0C, 0x0C, 0x0E, 0x10, 0x0E, 0x0C, 0x0C},
                                   {0x08, 0x08, 0x0A, 0x0C, 0x0A, 0x08, 0x08},
                                   {0x08, 0x08, 0x0A, 0x0C, 0x0A, 0x08, 0x08},
                                   {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F},
                                   {0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18},
                                   {0x06, 0x06, 0x08, 0x0A, 0x08, 0x06, 0x06},
                                   {0x00, 0x00, 0x1A, 0x1B, 0x1A, 0x00, 0x00},
                                   {0x00, 0x00, 0x01, 0x3F, 0x01, 0x00, 0x00}};                                    // size: 0x4D
Bool bESyncError = 0;
volatile unsigned long eMAX_ITERATIONS = 0x03000000;
Bool bNoWait = 0;
Bool bAlwaysCopyDisp = true;
unsigned int e_retrace_count = 0;

void eDisplayLetterBoxes() {
    TextureInfo *white = pTextureInfoWhite16x16NoAlpha;
    if (white == nullptr) {
        return;
    }

    ePoly poly;
    *reinterpret_cast<unsigned int *>(poly.Colours[0]) = 0;
    *reinterpret_cast<unsigned int *>(poly.Colours[1]) = 0;
    *reinterpret_cast<unsigned int *>(poly.Colours[2]) = 0;
    *reinterpret_cast<unsigned int *>(poly.Colours[3]) = 0;
    for (int id = 1; id <= 3; id++) {
        eView *view = &eViews[id];
        Camera *camera = view->GetCamera();
        eRenderTarget *target = view->GetRenderTarget();
        if (camera->GetLetterBox() <= 0.0f) {
            continue;
        }
        if (!view->IsActive()) {
            continue;
        }
        if (target->Active == 0) {
            continue;
        }

        int scissor_h = target->ScissorH;
        int scissor_y = target->ScissorY;
        int scissor_x = target->ScissorX;
        int scissor_w = target->ScissorW + 2;
        int bar = static_cast<int>(static_cast<float>(scissor_h) * camera->GetLetterBox());
        eSetOrthographicScreenQuadProjection(target);

        float x0 = static_cast<float>(scissor_x);
        float y0 = static_cast<float>(scissor_y);
        float x1 = static_cast<float>(scissor_x + scissor_w);
        float y1 = static_cast<float>(scissor_y + bar);
        poly.Vertices[0].x = x0;
        poly.Vertices[0].y = y0;
        poly.Vertices[0].z = 9000.0f;
        poly.Vertices[1].x = x1;
        poly.Vertices[1].y = y0;
        poly.Vertices[1].z = 9000.0f;
        poly.Vertices[2].x = x1;
        poly.Vertices[2].y = y1;
        poly.Vertices[2].z = 9000.0f;
        poly.Vertices[3].x = x0;
        poly.Vertices[3].y = y1;
        poly.Vertices[3].z = 9000.0f;
        view->Render(&poly, white, 0);

        float y2 = static_cast<float>(scissor_y + scissor_h - bar);
        float y3 = static_cast<float>(scissor_y + scissor_h);
        poly.Vertices[0].x = x0;
        poly.Vertices[0].y = y2;
        poly.Vertices[0].z = 9000.0f;
        poly.Vertices[1].x = x1;
        poly.Vertices[1].y = y2;
        poly.Vertices[1].z = 9000.0f;
        poly.Vertices[2].x = x1;
        poly.Vertices[2].y = y3;
        poly.Vertices[2].z = 9000.0f;
        poly.Vertices[3].x = x0;
        poly.Vertices[3].y = y3;
        poly.Vertices[3].z = 9000.0f;
        view->Render(&poly, white, 0);
    }
}

void eDisplaySafezone() {
    if (EnableSafezone) {
        ePoly poly;
        eView *view = eGetView(1, false);
        if (pTextureInfoWhite16x16NoAlpha != nullptr) {
            view->Render(&poly, pTextureInfoWhite16x16NoAlpha, 0);
        }
    }
}

void eInitGX() {
    __InitRenderMode();
    __InitMem();
    VIConfigure(_rmode);
    _defaultFIFO = bMalloc(0x42000, __FILE__, __LINE__, 0x800);
    _defaultFIFOObj = GXInit(_defaultFIFO, 0x42000);
    GXSetDrawDoneCallback(cb_DrawDone);
    GXSetDrawSyncCallback(sync_cb);
    __InitGX();
    __InitVI();
    __InitMatrices();
    if (bHangDiagnose) {
        eHangMetric(1);
    }
    _GxInitialized = 1;
}

void __InitRenderMode() {
    PALefbxfbFOVscl = 1.0f;
    PALefbxfbAspect = 1.0f;
    switch (VIGetTvFormat()) {
        case VI_TVMODE_NTSC_INT:
            if (bProgressiveScan) {
                _rmode = &GXNtsc480Prog;
            } else {
                _rmode = &GXNtsc480IntDf;
            }
            break;
        case VI_TVMODE_NTSC_DS:
            if (!bEURGB60) {
                _rmode = &PalNFS01IntDfScale;
                IsPal50Mode = true;
                PALefbxfbFOVscl = (float)efbHcrt / (float)xfbHcrt;
                PALefbxfbAspect = (float)xfbHcrt / (float)efbHcrt;
                break;
            }
            // fallthrough
        case VI_TVMODE_PAL_DS:
            _rmode = &GXEurgb60Hz480IntDf;
            break;
        case VI_TVMODE_NTSC_PROG:
            _rmode = &GXMpal480IntDf;
            break;
        default:
            OSPanic("d:/mw/speed/gamecube/src/ecstasy/EcstasyE.cpp", 0x1880, "Init: invalid TV format\n");
            break;
    }
    efbxfbRatio = static_cast<float>(efbHcrt) / xfbHcrt;
    GXAdjustForOverscan(_rmode, &_rmodeObj, 0, 0x10);

    _rmodeObj = *_rmode;
    _rmode = &_rmodeObj;
}

void __InitMem() {
#define ROUND_UP(x, round) (((x) + ((round) - 1)) & ~((round) - 1))

    fbSize = (u16)ROUND_UP(_rmode->fbWidth, 16) * _rmode->xfbHeight * 2;
    void *pFB = bMalloc(fbSize * 2, __FILE__, __LINE__, 0x800);
    _frameBuffer1 = _frameBuffer2 = _currentBuffer = pFB;
    _currentBuffer = _frameBuffer2 = (u8 *)pFB + fbSize;
#undef ROUND_UP
}

void __InitGXlite(void) {
    GXSetViewport(0.0f, 0.0f, _rmode->fbWidth, _rmode->xfbHeight, 0.0f, 1.0f);
    scis_xOrig = 0;
    scis_yOrig = 12;
    scis_wd = _rmode->fbWidth;
    scis_ht = _rmode->efbHeight - 24;
    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    eSetCulling(GX_CULL_NONE);
    eResetBlendMode();
    eResetZBuffering();
    eSetColourUpdate(1, 1);
    eForceResetTevSwapStages();
    eResetIndirectTextureSetup();
    for (int i = 0; i < 16; i++) {
        GXSetTevKColorSel(static_cast<GXTevStageID>(i), GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel(static_cast<GXTevStageID>(i), GX_TEV_KASEL_1);
    }
    vsReset(RESET_OPTION_DEFAULT);
    vsResetTexGen(0, 0);
    psReset(RESET_OPTION_DEFAULT);
    ps_NoLighting(0, 0);
    pTexPrev = nullptr;
}

void __InitGX(void) {
    GXSetViewport(0.0f, 0.0f, _rmode->fbWidth, _rmode->xfbHeight, 0.0f, 1.0f);
    scis_xOrig = 0;
    scis_yOrig = 12;
    scis_wd = _rmode->fbWidth;
    scis_ht = _rmode->efbHeight - 24;
    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    GXSetDispCopySrc(0, 0, _rmode->fbWidth, _rmode->efbHeight);
    GXSetDispCopyDst(_rmode->fbWidth, _rmode->xfbHeight);
    GXSetDispCopyYScale(static_cast<float>(_rmode->xfbHeight) / static_cast<float>(_rmode->efbHeight));
    // filt_00 = _rmode->vfilter[0];
    // filt_01 = _rmode->vfilter[1];
    // filt_10 = _rmode->vfilter[2];
    // filt_11 = _rmode->vfilter[3];
    // filt_12 = _rmode->vfilter[4];
    GXSetCopyFilter(_rmode->aa, _rmode->sample_pattern, GX_TRUE, _rmode->vfilter);
    if (_rmode->aa) {
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    } else {
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    }
    GXSetFieldMode(_rmode->field_rendering, _rmode->viHeight == _rmode->xfbHeight * 2);
    GXColor clr;
    clr.r = 0;
    clr.g = 0;
    clr.b = 0;
    clr.a = 0xFF;
    GXSetCopyClear(clr, 0x00FFFFFF);
    GXSetDispCopyGamma(GX_GM_1_0);
    e_bDither = 1;
    GXSetDither(1);
    GXSetCullMode(GX_CULL_NONE);
    eSetCulling(GX_CULL_NONE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    eSetColourUpdate(1, 1);
    eSetZBuffering(1, 1);
    vsReset(0);
    vsResetTexGen(0, 0);
    vsVtxAttrFmt(0);
    psReset(RESET_OPTION_DEFAULT);
    ps_NoLighting(0, 0);
    psGouraud();
    GXCopyDisp(_currentBuffer, 1);
}

void __InitVI(void) {
    VISetNextFrameBuffer(_frameBuffer1);
    _currentBuffer = _frameBuffer2;
    VIFlush();
    eWaitRetrace(0);
    if ((_rmode->viTVmode & 1) != 0) {
        eWaitRetrace(0);
    }
}

void __InitMatrices(void) {
    Mtx fe_scale;
    float transx = 0.0f;
    float transy = 0.0f;
    float gcn_scale = IsPal50Mode ? 0.81f : 0.84f;
    transx = ((float)_rmode->fbWidth - (float)_rmode->fbWidth * gcn_scale * (448.f / 378.f)) / 2.0f;
    float y_scale = gcn_scale / 448.0f;
    float height_scaled = (float)_rmode->efbHeight * gcn_scale / 448.0f;
    transy = ((float)_rmode->efbHeight - height_scaled * (float)_rmode->xfbHeight) / 2.0f;
    MTXScale(fe_scale, gcn_scale * (448.f / 378.f), y_scale * (float)_rmode->xfbHeight, 1.0f);
    MTXTransApply(fe_scale, viewMOrthographic, transx, transy, 10.0f);
    MTXOrtho(projMOrthographic, 0.0f, _rmode->efbHeight, 0.0f, _rmode->fbWidth, 0.0f, -100000.0f);
    MTXIdentity(viewMOrthographicScreenQuad);
    MTXOrtho(projMOrthographicScreenQuad, 0.0f, _rmode->efbHeight, 0.0f, _rmode->fbWidth, 0.0f, -100000.0f);
    eSetOrthographicMatrixToHW();
}

void eRecalculateOthographicProjection(int nForce, float fDepth) {
    static int WasPal50 = 0;
    static int WasWidescreen = 0;

    int pal_changed = (WasPal50 != IsPal50Mode);
    int wide_changed = FEDatabase ? (WasWidescreen != *(int *)&FEDatabase->GetUserProfile(0)->GetOptions()->TheVideoSettings.WideScreen) : 0;

    if (!pal_changed && !wide_changed && !nForce) {
        return;
    }

    WasPal50 = IsPal50Mode;
    WasWidescreen = FEDatabase ? *(int *)&FEDatabase->GetUserProfile(0)->GetOptions()->TheVideoSettings.WideScreen : 0;

    Mtx fe_scale;

    float depth = 0.0f;

    if (fDepth > 0.0f) {
        depth = fDepth;
    } else {
        depth = 10.0f;
    }

    float efbxfbScale = 1.0f;
    float virtual_width = 640.0f;

    if (FEDatabase) {
        if (*(int *)&FEDatabase->GetUserProfile(0)->GetOptions()->TheVideoSettings.WideScreen) {
            virtual_width = 481.88232421875f;
        }
    }

    if (_rmode->field_rendering) {
        efbxfbScale = 0.5f;
    }

    float gcn_scale = IsPal50Mode ? 0.81f : 0.84f;
    float xscale = 1.0f / 540.0f;
    float yscale = 1.0f / 448.0f;
    float transx = ((float)_rmode->fbWidth - (float)_rmode->fbWidth * gcn_scale * xscale * virtual_width) * 0.5f;
    float transy = ((float)_rmode->efbHeight - (float)_rmode->efbHeight * gcn_scale * yscale * (float)_rmode->xfbHeight) * 0.5f;

    MTXScale(fe_scale, gcn_scale * xscale * virtual_width, gcn_scale * yscale * (float)_rmode->xfbHeight, 1.0f);
    MTXTransApply(fe_scale, viewMOrthographic, transx, transy, depth);

    MTXOrtho(projMOrthographic, 0.0f, (float)_rmode->efbHeight / efbxfbScale, 0.0f, (float)_rmode->fbWidth, 0.0f, -100000.0f);
}

void eSetOrthographicScreenQuadProjection(eRenderTarget *target) {
    MTXOrtho(projMOrthographicScreenQuad, (float)target->ScissorY, (float)target->ScissorY + (float)target->ScissorH, (float)target->ScissorX,
             (float)target->ScissorX + (float)target->ScissorW, 0.0f, -16777215.0f);

    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadPosMtxImm(viewMOrthographicScreenQuad, GX_PNMTX0);
    GXSetProjection(projMOrthographicScreenQuad, GX_ORTHOGRAPHIC);
}

void eSetOrthographicMatrixToHW() {
    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadPosMtxImm(viewMOrthographic, GX_PNMTX0);
    GXSetProjection(projMOrthographic, GX_ORTHOGRAPHIC);
}

void eBeginScene(void) {
    static Bool bFirstTime = true;

    if (bFirstTime) {
        bFirstTime = false;
        VIAdvanceFrame();
        eWaitRetrace(0);
    } else {
        volatile unsigned long count = VIGetRetraceCount();
        volatile unsigned long iterations = 0;
        eWaitDrawDone();
        VIAdvanceFrame();
        if (!bNoWait) {
            eWaitRetrace(count);
        }
    }

    if (_rmode->field_rendering) {
        GXSetViewportJitter(0.0f, 0.0f, (float)_rmode->fbWidth, (float)_rmode->efbHeight, 0.0f, 1.0f, VIGetNextField());
    } else {
        GXSetViewport(0.0f, 0.0f, (float)_rmode->fbWidth, (float)_rmode->efbHeight, 0.0f, 1.0f);
    }

    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    eStallWorkaround(bStallWorkaround);
    eDLSaveContext(bDLSaveContext);

    int movie_playing = 0;
    if (gMoviePlayer) {
        movie_playing = (gMoviePlayer->GetStatus() == 5);
    }

    if ((TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING || TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) && !movie_playing) {
        eSetPixelFormat(eGXPixelFmt6666, eGXZFmt16);
    }

    eRecalculateOthographicProjection(0, 0.0f);

    if (FEDatabase) {
        if (*(int *)&FEDatabase->GetUserProfile(0)->GetOptions()->TheVideoSettings.WideScreen) {
            Global3DAspectRatio = 1.0f;
        } else {
            Global3DAspectRatio = IsPal50Mode ? 0.86f : 0.8f;
        }
    }
}

void eEndScene(void) {
    pTexPrev = 0;
    e_endsync = eEmitSync(false);
    eResetZBuffering();
    eSetColourUpdate(true, true);
    if (bAlwaysCopyDisp != false) {
        eCopyDisp(true);
    }
    eSendDrawDone(true);
}

void VIAdvanceFrame(void) {
    void *iVar1;

    VISetNextFrameBuffer(_currentBuffer);
    if ((_firstFrame != 0) && (--_firstFrame == 0)) {
        VISetBlack(0);
    }
    VIFlush();
    _currentBuffer = _currentBuffer == _frameBuffer1 ? _frameBuffer2 : _frameBuffer1;
}

void eCopyDisp(Bool bClear) {
    if (_rmode->aa) {
        eSetCopyFilter(FILTER_EFB_XFB_AA, true);
    } else {
        eSetCopyFilter(FILTER_EFB_XFB, true);
    }
    GXCopyDisp(_currentBuffer, bClear != 0);
}

void eDLSaveContext(Bool bEnabled) {
    static Bool _enabled = true;
    if (bEnabled != _enabled) {
        GXSetMisc(GX_MT_DL_SAVE_CONTEXT, bEnabled != 0);
        _enabled = bEnabled;
    }
}

void eSetBackgroundColor(GXColor clr) {
    GXSetCopyClear(clr, 0x00FFFFFF);
}

void eSetPixelFormat(int nPixelFormat, int nZFormat) {
    static GXPixelFmt prevPFmt = GX_PF_RGBA6_Z24;
    static GXPixelFmt _pformats[3] = {GX_PF_RGB8_Z24, GX_PF_RGBA6_Z24, GX_PF_RGB565_Z16};
    static GXZFmt16 prevZFmt = GX_ZC_LINEAR;
    static GXZFmt16 _zformats[4] = {GX_ZC_LINEAR, GX_ZC_NEAR, GX_ZC_MID, GX_ZC_FAR};

    GXPixelFmt pfmt = _pformats[nPixelFormat];
    GXZFmt16 zfmt = _zformats[nZFormat];

    if (pfmt != prevPFmt || zfmt != prevZFmt) {
        GXSetPixelFmt(pfmt, zfmt);
        prevPFmt = pfmt;
        prevZFmt = zfmt;
    }
}

void eSetScissor(int xOrig, int yOrig, int wd, int ht) {
    static int _xOrig = -1;
    static int _yOrig = -1;
    static int _wd = -1;
    static int _ht = -1;

    if ((xOrig != _xOrig) || (yOrig != _yOrig) || (wd != _wd) || (ht != _ht)) {
        GXSetScissor(xOrig, yOrig, wd, ht);
        _xOrig = xOrig;
        _yOrig = yOrig;
        _wd = wd;
        _ht = ht;
    }
}

void eSetCopyFilter(FILTER_ID filter_index, int enable) {
    GXSetCopyFilter(_rmode->aa, _rmode->sample_pattern, enable, CopyFilter[filter_index]);
}

void eUpdateCopyFilter2(Bool enable) {
    static FILTER_ID _vfilter[7];
    int i;
    int j;

    for (i = 0; i < 7; i++) {
        if (_rmode->vfilter[i] != _vfilter[i])
            break;
    }
    if (enable || (i != 7)) {
        for (i = 0, j = 7; j != 0; i++, j--) {
            _vfilter[i] = static_cast<FILTER_ID>(_rmode->vfilter[i]);
        }
        GXSetCopyFilter(_rmode->aa, _rmode->sample_pattern, GX_TRUE, _rmode->vfilter);
    }
}

void eDrawStartup(void) {
    GXColor fog_color = {0, 0, 0, 0xff};
    GXSetFog(GX_FOG_LIN, 16.0f, 512.0f, 0.5f, 12000.0f, fog_color);
    eLoadTevSwapTable();
    // GXInitTexObj(&HeadlightClipTextureObj, &HeadlightClipTextureTestData, 2, 1, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    // GXInitTexObjLOD(&HeadlightClipTextureObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
}

cSpecularMap SpecularMap;

cQuarterSizeMap QSizeI8_Z8;
cQuarterSizeMap QSizeScratchPad;
cQuarterSizeMap QSizeAccumulationI8;

static const bool TweakShowSafezone = false;
static const float SafezoneWidth_NTSC = 0.0f;
static const float SafezoneHeight_NTSC = 0.0f;
static const float SafezoneWidth_PAL = 0.0f;
static const float SafezoneHeight_PAL = ((float)xfbHcrt - 463.79202f) * 0.5f / (float)xfbHcrt;

void eExStartup(void) {
    unsigned int iter_countA;
    Mtx g_m0;
    Mtx g_m1;

    PlatformInitJoystick();

    for (iter_countA = 0;; iter_countA++) {
        if (ActualReadJoystickData() != 0 || iter_countA >= 0x1F4)
            break;
    }

    SpecularMap.Init();
    QSizeI8_Z8.Init(1, 1, 5);
    QSizeScratchPad.Init(0, 6, 4);
    QSizeAccumulationI8.Init(0, 1, 7);

    eInitContrastSurface();
    eInitHorizonFogDisplayList();

    MTXScale(g_m1, 0.5f, -0.5f, 0.0f);
    MTXTrans(g_m0, 0.5f, 0.5f, 1.0f);
    MTXConcat(g_m0, g_m1, g_ScreenPositionMatrix);

    eDEMOInitROMFont();

    if (OSGetResetCode() > 0x7FFFFFFF && OSGetProgressiveMode() != 0 && VIGetDTVStatus() == 1) {
        eProgressiveScan_EURGB60SetMode(1, 0);
    } else {
        if (OSGetResetCode() > 0x7FFFFFFF && (VIGetTvFormat() == 1 || VIGetTvFormat() == 5)) {
            if (OSGetEuRgb60Mode()) {
                eProgressiveScan_EURGB60SetMode(1, 1);
            } else {
                eNTSCInterlace_PALSetMode(1, 1);
            }
        } else {
            __InitRenderMode();
            eProgressiveScanModeCheck();
            eEURGB60ModeCheck();
        }
    }

    vsVtxAttrFmt(0);
    eDEMODeleteROMFont();
}

void eWaitRetrace(unsigned int in) {
    volatile unsigned long count;
    volatile unsigned long iterations;

    count = in ? in : VIGetRetraceCount();
    iterations = 0;
    e_retrace_count = count;
    while (VIGetRetraceCount() == count) {
        iterations++;
    }
}

void sync_cb(unsigned short token) {
    e_sync = token;
}

unsigned short eEmitSync(Bool bFlush) {
    last_sync_token++;
    GXSetDrawSync(last_sync_token);
    if (bFlush) {
        GXFlush();
    }
    return last_sync_token;
}

static volatile Bool bDrawDoneEncountered = 0;

void cb_DrawDone() {
    bDrawDoneEncountered = true;
}

void eSendDrawDone(unsigned char bFlush) {
    bDrawDoneEncountered = false;
    GXSetDrawDone();
    if (bFlush) {
        GXFlush();
    }
}

Bool eIsDrawDone() {
    return bDrawDoneEncountered;
}

Bool IsSyncValid() {
    if (last_sync_token == e_sync) {
        return true;
    }
    return last_sync_token == GXReadDrawSync();
}

void eWaitDrawDone(void) {
    volatile unsigned long iterations = 0;
    while (true) {
        if (eIsDrawDone()) {
            break;
        }
        iterations++;
        if (eMAX_ITERATIONS == 0) {
            break;
        }
        if ((iterations >= eMAX_ITERATIONS) || bESyncError) {
            if (bHangDiagnose) {
                eDiagnoseHang();
            }
            if (IsSyncValid()) {
                e_resync++;
            } else if (KeepAlive()) {
                e_keepalive++;
            }
            break;
        }
    }
}

static Bool KeepAlive(void) {
    volatile float ms;
    volatile unsigned int t_entry = bGetTicker();
    volatile unsigned short sent_token = eEmitSync(true);
    do {
        if (e_sync == sent_token) {
            return true;
        }
        ms = bGetTickerDifference(t_entry, bGetTicker());
        if ((ms > 4.0f) || ms < 0.0f) {
            return GXReadDrawSync() == sent_token;
        }
    } while (true);
}

void eStallWorkaround(Bool bEnabled) {
    static Bool _enabled = true;
    if (bEnabled != _enabled) {
        GXSetMisc(GX_MT_XF_FLUSH, bEnabled ? 8 : 0);
        _enabled = bEnabled;
    }
}

void eDiagnoseHang(void) {
    u32 xfTop0; // r1+0x10
    u32 xfBot0; // r1+0xC
    u32 suRdy0; // r1+0x18
    u32 r0Rdy0; // r1+0x14
    u32 xfTop1; // r1+0x20
    u32 xfBot1; // r1+0x1C
    u32 suRdy1; // r1+0x28
    u32 r0Rdy1; // r1+0x24
    u32 xfTopD;
    u32 xfBotD;
    u32 suRdyD;
    u32 r0RdyD;
    u8 readIdle; // r1+0x9
    u8 cmdIdle;  // r1+0xA
    u8 junk;     // r1+0x8

    GXReadXfRasMetric(&xfBot0, &xfTop0, &r0Rdy0, &suRdy0);
    GXReadXfRasMetric(&xfBot1, &xfTop1, &r0Rdy1, &suRdy1);
    xfTopD = xfTop1 == xfTop0;
    xfBotD = xfBot1 == xfBot0;
    suRdyD = suRdy1 != suRdy0;
    r0RdyD = r0Rdy0 != r0Rdy1;

    GXGetGPStatus(&junk, &junk, &readIdle, &cmdIdle, &junk);

    // not sure, might be a fake match
    if (!xfBotD && suRdyD) {
    } else if (!xfTopD && xfBotD && suRdyD) {
    } else if (!cmdIdle && xfTopD && xfBotD && suRdyD) {
    } else if (readIdle && cmdIdle) {
    } else if (!cmdIdle && readIdle) {
    } else if (readIdle) {
    }
}

#include "dolphin/gx/GXPriv.h"

// Programa a mano los registros BP/XF del contador de rendimiento: sin esto el
// GP se queda colgado sin decir por que. El else devuelve todo a cero.
void eHangMetric(Bool bEnable) {
    if (bEnable) {

        GXSetGPMetric(GX_PERF0_NONE, GX_PERF1_NONE);

        GX_WRITE_U8(GX_LOAD_BP_REG);
        GX_WRITE_U32(0x2402C004);

        GX_WRITE_U8(GX_LOAD_BP_REG);
        GX_WRITE_U32(0x02300020);

        GX_WRITE_U8(GX_LOAD_XF_REG);
        GX_WRITE_U16(0);
        GX_WRITE_U16(0x1006);
        GX_WRITE_U32(0x00084400);

    } else {

        GX_WRITE_U8(GX_LOAD_BP_REG);
        GX_WRITE_U32(0x24000000);

        GX_WRITE_U8(GX_LOAD_BP_REG);
        GX_WRITE_U32(0x23000000);

        GX_WRITE_U8(GX_LOAD_XF_REG);
        GX_WRITE_U16(0);
        GX_WRITE_U16(0x1006);
        GX_WRITE_U32(0);
    }
}

// ...

int eScreenQuadReplace(eView *view, int e_poly_mode) {
    ePoly poly;
    unsigned int poly_color = 0xFFFFFFFF;
    unsigned int r, g, b, a, intensity;
    ScreenEffectDB *SE_db = view->ScreenEffects;
    TextureInfo *texture_info = 0;

    switch (e_poly_mode) {
    case 0x68:
        texture_info = QSizeI8_Z8.quarterSizeDepthBuffer.GetTexture();
        break;
    case 0x6b:
        texture_info = QSizeAccumulationI8.quarterSizeBuffer.GetTexture();
        break;
    case 0x6c:
        texture_info = QSizeI8_Z8.quarterSizeBuffer.GetTexture();
        break;
    case 0x6e:
        texture_info = pTextureInfoWhite16x16;
        intensity = (int)(SE_db->GetIntensity(SE_TINT) * 128.0f);
        SE_db->GetRGBA(SE_TINT, &r, &g, &b, &a);
        poly_color = intensity | (b << 8) | (g << 16) | (r << 24);
        break;
    case 0x6d:
    case 0x6f:
        texture_info = pTextureInfoWhite16x16;
        break;
    case 0x6a:
    case 0x70:
        texture_info = QSizeScratchPad.quarterSizeBuffer.GetTexture();
        break;
    }

    eRenderTarget *render_target = view->GetRenderTarget();

    if (view->IsActive() && render_target->IsActive()) {
        float ScissorX = static_cast<float>(render_target->ScissorX);
        float ScissorY = static_cast<float>(render_target->ScissorY);
        float ScissorW = static_cast<float>(render_target->ScissorW);
        float ScissorH = static_cast<float>(render_target->ScissorH);
        float sWx = 0.0f;
        float sHy = 0.0f;
        if (ScissorX > 0.0f) {
            sWx = 1.0f;
        }
        if (ScissorY > 0.0f) {
            sHy = 1.0f;
        }
        float sx = 0.0f;
        float sy = 0.0f;
        float sw = sWx + 1.0f;
        float sh = sHy + 1.0f;
        if (e_poly_mode == 0x70 && CurrentViewMode == EVIEWMODE_TWOH) {
            if (view->ID == EVIEW_PLAYER1) {
                sh = 0.5f;
            } else if (view->ID == EVIEW_PLAYER2) {
                sy = 0.5f;
                sh = 1.0f;
            }
        }
        float dx = ScissorX;
        float dy = ScissorY;
        float dw = dx + ScissorW;
        float dh = dy + ScissorH;

        poly.Vertices[0].x = dx;
        poly.Vertices[0].y = dy;
        poly.Vertices[0].z = 65535.0f;
        poly.Vertices[1].x = dx + dw;
        poly.Vertices[1].y = dy;
        poly.Vertices[1].z = 65535.0f;
        poly.Vertices[2].x = dx + dw;
        poly.Vertices[2].y = dy + dh;
        poly.Vertices[2].z = 65535.0f;
        poly.Vertices[3].x = dx;
        poly.Vertices[3].y = dy + dh;
        poly.Vertices[3].z = 65535.0f;

        poly.UVs[0][0] = sx;
        poly.UVs[0][1] = sy;
        poly.UVs[0][2] = sx + sw;
        poly.UVs[0][3] = sy;
        poly.UVs[1][0] = sx + sw;
        poly.UVs[1][1] = sy + sh;
        poly.UVs[1][2] = sx;
        poly.UVs[1][3] = sy + sh;

        *reinterpret_cast<unsigned int *>(poly.Colours[0]) = poly_color;
        *reinterpret_cast<unsigned int *>(poly.Colours[1]) = poly_color;
        *reinterpret_cast<unsigned int *>(poly.Colours[2]) = poly_color;
        *reinterpret_cast<unsigned int *>(poly.Colours[3]) = poly_color;

        view->Render(&poly, texture_info, e_poly_mode);
    }

    return 1;
}

int DrawSmear = 2;
int SmearInvert = 1;
float SmearBiasBASE = 2.0f;
float SmearBiasNOS = 15.0f;
int NOStimer = 15;
int SmearStage2 = 1;
int num_smears = 6;
int SmearSubStageEnable[6] = {1, 1, 1, 1, 0, 0};
float SmearDistances[6] = {2.3f, 6.4f, 8.8f, 11.4f, 22.0f, 40.0f};
float SmearAlphas[6] = {50.0f, 35.0f, 28.0f, 17.0f, 8.0f, 4.0f};
unsigned int SmearRGBs[6][3] = {{130, 130, 130}, {130, 130, 130}, {130, 130, 130},
                                {130, 130, 130}, {130, 130, 130}, {130, 130, 130}};
float SmearDistModNOS = 2.0f;
int SmearR = 130;
int SmearG = 130;
int SmearB = 130;
int Rsub = -10;
int Gsub = -10;
int Bsub = -28;
float SmearMaxAlphaScale = 0.75f;
float SmearMinSpeed = 30.0f;
float SmearMaxSpeed = 60.0f;
float SmearAlphaScale = 0.0f;
float SmearDistanceMultiplier = 2.0f;
float SmearNOSAlphaBonus = 1.5f;
int useTweakablePoly = 0;
float polyTweakDX = 0.0f;
float polyTweakDW = 640.0f;
float polyTweakDY = 0.0f;
float polyTweakDH = 480.0f;
float polyTweakSX = 0.0f;
float polyTweakSW = 1.0f;
float polyTweakSY = 0.0f;
float polyTweakSH = 1.0f;

int eMotionBlurEffect(eView *view) {
    if (DrawSmear != 2) {
        return 0;
    }

    static int NOStime = 0;
    static int NOSfade = 0;
    int NOSon = 0;
    float SmearBias;
    float SmearDistMod = 2.0f;
    float SmearNOSBonus;

    ePoly poly;
    TextureInfo *texture_info = QSizeScratchPad.quarterSizeBuffer.GetTexture();
    eRenderTarget *render_target = view->GetRenderTarget();

    if (view->IsActive() && render_target->IsActive()) {
        ScreenEffectDB *SE_db = view->ScreenEffects;

        if (SE_db->IsActive(SE_MOTION_BLUR)) {
            if (NOSfade == 0) {
                NOSfade = 1;
                NOStime = 0;
            }

            NOSon = 1;

            SmearSubStageEnable[4] = NOSon;
            SmearSubStageEnable[5] = NOSon;
        } else {
            if (NOSfade != 0) {
                NOSfade = 2;
                NOSon = 1;
            } else {
                SmearSubStageEnable[4] = NOSon;
                SmearSubStageEnable[5] = NOSon;
            }
        }

        if (NOSfade == 1) {
            NOStime += 2;
            if (NOStime >= NOStimer) {
                NOStime = NOStimer;
                NOSfade = 3;
            }
        } else if (NOSfade == 2) {
            if (--NOStime <= 0) {
                NOStime = 0;
                NOSfade = 0;
            }
        }

        if (NOSon) {
            int smear_substage;
            SmearBias = (float)NOStime * (SmearBiasNOS - SmearBiasBASE) / (float)NOStimer + SmearBiasBASE;
            SmearDistMod = (float)NOStime * (SmearDistModNOS - 2.0f) / (float)NOStimer + 2.0f;
            SmearAlphas[4] = (float)NOStime * 8.0f / (float)NOStimer;
            SmearAlphas[5] = (float)NOStime * 4.0f / (float)NOStimer;

            SmearNOSBonus = SmearNOSAlphaBonus;

            for (smear_substage = 0; smear_substage < num_smears; smear_substage++) {
                SmearRGBs[smear_substage][0] = SmearR - smear_substage * Rsub;
                SmearRGBs[smear_substage][1] = SmearG - smear_substage * Gsub;
                SmearRGBs[smear_substage][2] = SmearB - smear_substage * Bsub;
            }
        } else {
            SmearNOSBonus = 1.0f;
        }

        if (SmearStage2) {
            Camera *camera = view->GetCamera();
            bMatrix4 *world_view = camera->GetCameraMatrix();
            bVector3 *camera_world_velocity = camera->GetVelocityPosition();
            bVector3 *camera_world_direction = camera->GetDirection();

            float camera_speed = 0.0f;
            camera_speed = bLength(camera_world_velocity);

            bVector4 camera_view_velocity;

            camera_view_velocity.x = camera_world_velocity->x;
            camera_view_velocity.y = camera_world_velocity->y;
            camera_view_velocity.z = camera_world_velocity->z;
            camera_view_velocity.w = 0.0f;

            eMulVector(&camera_view_velocity, world_view, &camera_view_velocity);

            float min_speed = SmearMinSpeed;
            float max_speed = SmearMaxSpeed;
            float alpha_scale;

            if (camera_speed <= min_speed) {
                alpha_scale = 0.0f;
            } else if (camera_speed >= max_speed) {
                alpha_scale = 1.0f;
            } else {
                alpha_scale = (camera_speed - min_speed) / (max_speed - min_speed);
            }

            if (alpha_scale > SmearMaxAlphaScale) {
                alpha_scale = SmearMaxAlphaScale;
            }
            if (SmearAlphaScale != 0.0f) {
                alpha_scale = SmearAlphaScale;
            }
            if (alpha_scale < 1.0f / 255.0f) {
                return 0;
            }

            bVector4 s0;
            bVector4 s2;
            bVector4 camera_view_velocityN;

            bNormalize(&camera_view_velocityN, &camera_view_velocity);

            if (SmearInvert) {
                s0.x = camera_view_velocityN.x + camera_view_velocityN.z;
                s0.y = camera_view_velocityN.y + camera_view_velocityN.z;

                s2.x = camera_view_velocityN.x - camera_view_velocityN.z;
                s2.y = camera_view_velocityN.y - camera_view_velocityN.z;
            } else {
                s0.x = -(camera_view_velocityN.x + camera_view_velocityN.z);
                s0.y = -(camera_view_velocityN.y + camera_view_velocityN.z);

                s2.x = camera_view_velocityN.z - camera_view_velocityN.x;
                s2.y = camera_view_velocityN.z - camera_view_velocityN.y;
            }

            for (int smear_substage = 0; smear_substage < num_smears; smear_substage++) {
                if (SmearSubStageEnable[smear_substage]) {
                    int UseSmearR = SmearR;
                    int UseSmearG = SmearG;
                    int UseSmearB = SmearB;

                    if (NOSon) {
                        UseSmearR = SmearRGBs[smear_substage][0];
                        UseSmearG = SmearRGBs[smear_substage][1];
                        UseSmearB = SmearRGBs[smear_substage][2];
                    }

                    int stage_alpha = (int)(SmearAlphas[smear_substage] * alpha_scale * SmearNOSBonus);

                    float smear_distance = -SmearDistances[smear_substage] * SmearDistMod;

                    float ScissorX = (float)render_target->ScissorX;
                    float ScissorY = (float)render_target->ScissorY;
                    float ScissorW = (float)render_target->ScissorW;
                    float ScissorH = (float)render_target->ScissorH;

                    float s0x = 0.0f;
                    float s0y = (float)render_target->ScissorY / (float)eGetScreenHeight();
                    float s2x = 1.0f;
                    float s2y = (float)(render_target->ScissorY + render_target->ScissorH) / (float)eGetScreenHeight();

                    float d0x = ScissorX - s0.x * smear_distance * SmearDistanceMultiplier;
                    float d0y = ScissorY - s0.y * smear_distance * SmearDistanceMultiplier;
                    float d2x = ScissorX + ScissorW - 1.0f - s2.x * smear_distance * SmearDistanceMultiplier;
                    float d2y = ScissorY + ScissorH - 1.0f - s2.y * smear_distance * SmearDistanceMultiplier;

                    float sx = s0x;
                    float sy = s0y;
                    float sw = bAbs(s2x - s0x);
                    float sh = bAbs(s2y - s0y);

                    float dx = d0x;
                    float dy = d0y;
                    float dw = bAbs(d2x - d0x);
                    float dh = bAbs(d2y - d0y);

                    if (useTweakablePoly) {
                        poly.Vertices[0].x = polyTweakDX;
                        poly.Vertices[0].y = polyTweakDY;
                        poly.Vertices[0].z = 65535.0f;
                        poly.Vertices[1].x = polyTweakDX + polyTweakDW;
                        poly.Vertices[1].y = polyTweakDY;
                        poly.Vertices[1].z = 65535.0f;
                        poly.Vertices[2].x = polyTweakDX + polyTweakDW;
                        poly.Vertices[2].y = polyTweakDY + polyTweakDH;
                        poly.Vertices[2].z = 65535.0f;
                        poly.Vertices[3].x = polyTweakDX;
                        poly.Vertices[3].y = polyTweakDY + polyTweakDH;
                        poly.Vertices[3].z = 65535.0f;

                        poly.UVs[0][0] = polyTweakSX;
                        poly.UVs[0][1] = polyTweakSY;
                        poly.UVs[0][2] = polyTweakSX + polyTweakSW;
                        poly.UVs[0][3] = polyTweakSY;
                        poly.UVs[1][0] = polyTweakSX + polyTweakSW;
                        poly.UVs[1][1] = polyTweakSY + polyTweakSH;
                        poly.UVs[1][2] = polyTweakSX;
                        poly.UVs[1][3] = polyTweakSY + polyTweakSH;
                        poly.UVsMask[0][0] = 1.0f;
                    } else {
                        poly.Vertices[0].x = dx;
                        poly.Vertices[0].y = dy;
                        poly.Vertices[0].z = 65535.0f;
                        poly.Vertices[1].x = dx + dw;
                        poly.Vertices[1].y = dy;
                        poly.Vertices[1].z = 65535.0f;
                        poly.Vertices[2].x = dx + dw;
                        poly.Vertices[2].y = dy + dh;
                        poly.Vertices[2].z = 65535.0f;
                        poly.Vertices[3].x = dx;
                        poly.Vertices[3].y = dy + dh;
                        poly.Vertices[3].z = 65535.0f;

                        poly.UVs[0][0] = sx;
                        poly.UVs[0][1] = sy;
                        poly.UVs[0][2] = sx + sw;
                        poly.UVs[0][3] = sy;
                        poly.UVs[1][0] = sx + sw;
                        poly.UVs[1][1] = sy + sh;
                        poly.UVs[1][2] = sx;
                        poly.UVs[1][3] = sy + sh;
                    }

                    *reinterpret_cast<unsigned int *>(poly.Colours[0]) =
                        stage_alpha | (UseSmearB << 8) | (UseSmearG << 16) | (UseSmearR << 24);
                    *reinterpret_cast<unsigned int *>(poly.Colours[1]) =
                        stage_alpha | (UseSmearB << 8) | (UseSmearG << 16) | (UseSmearR << 24);
                    *reinterpret_cast<unsigned int *>(poly.Colours[2]) =
                        stage_alpha | (UseSmearB << 8) | (UseSmearG << 16) | (UseSmearR << 24);
                    *reinterpret_cast<unsigned int *>(poly.Colours[3]) =
                        stage_alpha | (UseSmearB << 8) | (UseSmearG << 16) | (UseSmearR << 24);

                    view->Render(&poly, texture_info, 0x67);
                }
            }
        }
    }

    return 1;
}

extern GXColor KColorSky;
int TweakDumpSkyLayerColors = 0;
void eFlushTextureBucketList();

void eRenderSky(eView *view) {
    float modKonst[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    if (static_cast<unsigned int>(view->GetID() - 1) <= 1) {
        for (int layerIx = 0; layerIx <= 2; layerIx++) {
            GetLayerMod(view, static_cast<SKY_LAYER>(layerIx), &modKonst[0], &modKonst[1], &modKonst[2], &modKonst[3]);
            unsigned char rgb_mod = 255;
            if (layerIx == 2) {
                rgb_mod = 64;
            }
            unsigned char a_mod = 255;
            if (layerIx == 2) {
                a_mod = 127;
            }
            KColorSky.r = static_cast<unsigned char>(modKonst[0] * rgb_mod);
            KColorSky.g = static_cast<unsigned char>(modKonst[1] * rgb_mod);
            KColorSky.b = static_cast<unsigned char>(modKonst[2] * rgb_mod);
            KColorSky.a = static_cast<unsigned char>(modKonst[3] * a_mod);
            if (TweakDumpSkyLayerColors) {
                // El volcado de depuracion original esta compilado a nada, pero la RAMA con
                // dos literales hace falta: sin ella GCC borra el `if` antes de loop.c, no
                // nace el pseudo del @ha de TweakDumpSkyLayerColors y no se iza al preheader
                // (el `stw` posterior al bucle lo reusa). Los literales NO llegan a .rodata:
                // el codigo muerto se borra despues. Con `layer == 2` no vale: CSE lo pliega
                // con el de rgb_mod y la rama desaparece.
                const char *layer_name = (layerIx == 0) ? "SKY_BASE" : "SKY_LAYER";
            }
            StuffSkyLayer(view, static_cast<SKY_LAYER>(layerIx));
            eFlushTextureBucketList();
        }
        if (TweakDumpSkyLayerColors != 0) {
            TweakDumpSkyLayerColors = 0;
        }
        return;
    }

    if (static_cast<unsigned int>(view->GetID() - 6) <= 1) {
        StuffSpecular(view);
        return;
    }

    GetLayerMod(view, SKY_LAYER_LOWREZ, &modKonst[0], &modKonst[1], &modKonst[2], &modKonst[3]);
    KColorSky.r = static_cast<unsigned char>(modKonst[0] * 127.0f);
    KColorSky.g = static_cast<unsigned char>(modKonst[1] * 127.0f);
    KColorSky.b = static_cast<unsigned char>(modKonst[2] * 127.0f);
    KColorSky.a = static_cast<unsigned char>(modKonst[3] * 255.0f);
    StuffSkyLayer(view, SKY_LAYER_LOWREZ);
    eFlushTextureBucketList();
}
void eEURGB60ModeCheck() {
    for (int i = 0; i < 4; i++) {
        g_InitPad[i].button = 0;
        g_LastInitPad[i].button = 0;
    }

    if (VIGetTvFormat() == VI_PAL || VIGetTvFormat() == VI_EURGB60) {

        GXColor black = {0, 0, 0, 0};
        int bShowDialog = 1;

        GXSetCopyClear(black, 0x00FFFFFF);

        if (!OSGetEuRgb60Mode()) {

            PADRead(g_InitPad);

            for (int i = 0; i < 4; i++) {
                HardwarePadStatus[i].button = g_InitPad[i].button;
            }

            ReadLGWheelDataForProgressiveMenu();

            int bFound = 0;

            for (int channel = 0; !bFound && channel < 2; channel++) {

                if (g_InitPad[channel].err == 0 && (g_InitPad[channel].button & PAD_BUTTON_B)) {

                    bShowDialog = 1;
                    bFound = 1;

                } else if (IsWheelActiveForProgressiveMenu(channel)) {

                    int timeout = 0;
                    unsigned short buttons;

                    do {
                        ReadLGWheelDataForProgressiveMenu();
                        buttons = ReadLGWheelButtonsForProgressiveMenu(channel);
                        HardwarePadStatus[channel].button = buttons;
                        timeout++;
                    } while (buttons == 0 && timeout <= 499);

                    if (buttons & PAD_BUTTON_B) {
                        bShowDialog = 1;
                        bFound = 1;
                    }
                }
            }
        }

        if (bShowDialog) {

            if (eProgressiveScan_EURGB60DialogBox(1)) {

                OSSetEuRgb60Mode(1);
                eProgressiveScan_EURGB60SetMode(0, 1);
                eProgressiveScan_EURGB60Proceed(1);

            } else {

                OSSetEuRgb60Mode(0);
                eNTSCInterlace_PALSetMode(0, 1);
                eNTSCInterlace_PALProceed(1);
            }
        }
    }

    VISetBlack(FALSE);
    VIFlush();

    eDEMOBeforeRender();
    eDEMODoneRender();
    VIWaitForRetrace();

    eDEMOBeforeRender();
    eDEMODoneRender();
    VIWaitForRetrace();
}

void eProgressiveScanModeCheck() {
    for (int i = 0; i < 4; i++) {
        g_InitPad[i].button = 0;
        g_LastInitPad[i].button = 0;
    }

    if (VIGetDTVStatus() == 1 && VIGetTvFormat() == VI_NTSC) {

        int bShowDialog = 0;

        GXColor black = {0, 0, 0, 0};
        GXSetCopyClear(black, 0x00FFFFFF);
        VISetBlack(TRUE);
        VIFlush();
        VIWaitForRetrace();

        if (OSGetProgressiveMode()) {

            bShowDialog = 1;

        } else {

            PADRead(g_InitPad);

            for (int i = 0; i < 4; i++) {
                HardwarePadStatus[i].button = g_InitPad[i].button;
            }

            ReadLGWheelDataForProgressiveMenu();

            int bFound = 0;

            for (int channel = 0; !bFound && channel < 2; channel++) {

                if (g_InitPad[channel].err == 0 && (g_InitPad[channel].button & PAD_BUTTON_B)) {

                    bShowDialog = 1;
                    bFound = 1;

                } else if (IsWheelActiveForProgressiveMenu(channel)) {

                    int timeout = 0;
                    unsigned short buttons;

                    do {
                        ReadLGWheelDataForProgressiveMenu();
                        buttons = ReadLGWheelButtonsForProgressiveMenu(channel);
                        HardwarePadStatus[channel].button = buttons;
                        timeout++;
                    } while (buttons == 0 && timeout <= 499);

                    if (buttons & PAD_BUTTON_B) {
                        bFound = 1;
                        bShowDialog = 1;
                    }
                }
            }
        }

        if (bShowDialog) {

            if (eProgressiveScan_EURGB60DialogBox(0)) {

                OSSetProgressiveMode(1);
                eProgressiveScan_EURGB60SetMode(0, 0);
                eProgressiveScan_EURGB60Proceed(0);

            } else {

                OSSetProgressiveMode(0);
                eNTSCInterlace_PALSetMode(0, 0);
                eNTSCInterlace_PALProceed(0);
            }

        } else {

            OSSetProgressiveMode(0);
        }

    } else {

        OSSetProgressiveMode(0);
    }

    VISetBlack(FALSE);
    VIFlush();

    eDEMOBeforeRender();
    eDEMODoneRender();
    VIWaitForRetrace();

    eDEMOBeforeRender();
    eDEMODoneRender();
    VIWaitForRetrace();
}

void eProgressiveScan_EURGB60SetMode(int os_restarted, int tv_mode) {
    if (tv_mode == 0) {

        bProgressiveScan = 1;

    } else if (tv_mode == 1) {

        bEURGB60 = tv_mode;
        SetVideoMode(MODE_PAL60);
    }

    VISetBlack(TRUE);
    VIFlush();
    VIWaitForRetrace();

    if (os_restarted == 0) {
        for (int i = 0; i < 10; i++) {
            VIWaitForRetrace();
        }
    }

    __InitRenderMode();
    __InitGX();
    VIConfigure(_rmode);
    __InitVI();

    GXColor black = {0, 0, 0, 0};
    GXSetCopyClear(black, 0x00FFFFFF);

    if (os_restarted == 0) {
        for (int i = 0; i < 100; i++) {
            eDEMOBeforeRender();
            eDEMODoneRender();
            VIWaitForRetrace();
        }
    }

    VISetBlack(FALSE);
}

void eProgressiveScan_EURGB60Proceed(int mode) {
    int bWaiting = 1;
    int timeout = 0;
    OSTime start = OSGetTime() / (e_OSBusClock / 4 / 1000);

    if (mode == 0) {

        timeout = 10000;

    } else if (mode == 1) {

        timeout = 3000;
    }

    while (bWaiting) {

        for (int channel = 0; channel < 4; channel++) {

            if (g_InitPad[channel].err == 0) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
                g_LastInitPad[channel].stickX = g_InitPad[channel].stickX;
                g_LastInitPad[channel].stickY = g_InitPad[channel].stickY;
                g_LastInitPad[channel].err = g_InitPad[channel].err;

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
            }
        }

        PADRead(g_InitPad);

        for (int i = 0; i < 4; i++) {
            HardwarePadStatus[i].button = g_InitPad[i].button;
        }

        ReadLGWheelDataForProgressiveMenu();

        for (int channel = 0; channel < 2; channel++) {

            if (g_InitPad[channel].err == 0) {

                unsigned int cur = g_InitPad[channel].button & PAD_BUTTON_A;
                unsigned int mask = g_LastInitPad[channel].button & PAD_BUTTON_A;
                unsigned int changed = cur ^ mask;

                if (changed & mask) {
                    bWaiting = 0;
                }

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                unsigned short buttons = ReadLGWheelButtonsForProgressiveMenu(channel);
                g_InitPad[channel].button = buttons;
                HardwarePadStatus[channel].button = buttons;

                unsigned int mask = g_LastInitPad[channel].button & PAD_BUTTON_A;
                unsigned int changed = (buttons & PAD_BUTTON_A) ^ mask;

                if (changed & mask) {
                    bWaiting = 0;
                }

            } else {

                PADReset(PADMASKS[channel]);
            }
        }

        if (OSGetTime() / (e_OSBusClock / 4 / 1000) > start + timeout) {
            bWaiting = 0;
        }

        eDEMOBeforeRender();
        eDEMOInitCaption(0, 640, 448);

        if (mode == 0) {

            eDEMORFPrintf(0x78, 0x82, 0, "The display mode has switched");
            eDEMORFPrintf(0xBE, 0xAA, 0, "to Progressive Mode.");
            eDEMORFPrintf(0x78, 0x104, 0, "Press the A Button to continue.");

        } else if (mode == 1) {

            switch (GC_GetOSLanguage()) {

                case eLANGUAGE_GERMAN:
                    eDEMORFPrintf(0x8C, 0x82, 0, "Die Bildschirmdarstellung erfolgt");
                    eDEMORFPrintf(0xDC, 0xAA, 0, "im 60Hz-Modus.");
                    break;

                case eLANGUAGE_FRENCH:
                    eDEMORFPrintf(0xC8, 0x82, 0, "L'affichage est en");
                    eDEMORFPrintf(0xF0, 0xAA, 0, "mode 60 Hz.");
                    break;

                case eLANGUAGE_ENGLISH:
                default:
                    eDEMORFPrintf(0x8C, 0x82, 0, "Screen display has been set");
                    eDEMORFPrintf(0xDC, 0xAA, 0, "to 60 Hz mode.");
                    break;
            }
        }

        eDEMODoneRender();
        VIWaitForRetrace();
    }
}

void eNTSCInterlace_PALSetMode(int os_restarted, int tv_mode) {
    Bool bOff = FALSE;

    if (tv_mode == 0) {

        bProgressiveScan = bOff;

    } else if (tv_mode == 1) {

        bEURGB60 = bOff;
    }

    VISetBlack(TRUE);
    VIFlush();
    VIWaitForRetrace();

    if (os_restarted == 0) {
        for (int i = 0; i < 10; i++) {
            VIWaitForRetrace();
        }
    }

    __InitRenderMode();
    __InitGX();
    VIConfigure(_rmode);
    __InitVI();

    __InitMatrices();

    GXColor black = {0, 0, 0, 0};
    GXSetCopyClear(black, 0x00FFFFFF);

    if (os_restarted == 0) {
        for (int i = 0; i < 100; i++) {
            eDEMOBeforeRender();
            eDEMODoneRender();
            VIWaitForRetrace();
        }
    }

    VISetBlack(FALSE);
}

void eNTSCInterlace_PALProceed(int mode) {
    int bWaiting = 1;
    int timeout = 0;
    OSTime start = OSGetTime() / (e_OSBusClock / 4 / 1000);

    if (mode == 0) {

        timeout = 10000;

    } else if (mode == 1) {

        timeout = 3000;
    }

    while (bWaiting) {

        for (int channel = 0; channel < 4; channel++) {

            if (g_InitPad[channel].err == 0) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
                g_LastInitPad[channel].stickX = g_InitPad[channel].stickX;
                g_LastInitPad[channel].stickY = g_InitPad[channel].stickY;
                g_LastInitPad[channel].err = g_InitPad[channel].err;

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
            }
        }

        PADRead(g_InitPad);

        for (int i = 0; i < 4; i++) {
            HardwarePadStatus[i].button = g_InitPad[i].button;
        }

        ReadLGWheelDataForProgressiveMenu();

        int bFound = 0;

        for (int channel = 0; !bFound && channel < 2; channel++) {

            if (g_InitPad[channel].err == 0) {

                unsigned int cur = g_InitPad[channel].button & PAD_BUTTON_A;
                unsigned int mask = g_LastInitPad[channel].button & PAD_BUTTON_A;
                unsigned int changed = cur ^ mask;

                if (changed & mask) {
                    bWaiting = 0;
                    bFound = 1;
                }

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                unsigned short buttons = ReadLGWheelButtonsForProgressiveMenu(channel);
                g_InitPad[channel].button = buttons;
                HardwarePadStatus[channel].button = buttons;

                unsigned int mask = g_LastInitPad[channel].button & PAD_BUTTON_A;
                unsigned int changed = (buttons & PAD_BUTTON_A) ^ mask;

                if (changed & mask) {
                    bWaiting = 0;
                    bFound = 1;
                }

            } else {

                PADReset(PADMASKS[channel]);
            }
        }

        if (OSGetTime() / (e_OSBusClock / 4 / 1000) > start + timeout) {
            bWaiting = 0;
        }

        eDEMOBeforeRender();
        eDEMOInitCaption(0, 640, 448);

        if (mode == 0) {

            eDEMORFPrintf(0x78, 0x82, 0, "The display mode has switched");
            eDEMORFPrintf(0xBE, 0xAA, 0, "to Interlaced Mode.");
            eDEMORFPrintf(0x78, 0x104, 0, "Press the A Button to continue.");

        } else if (mode == 1) {

            switch (GC_GetOSLanguage()) {

                case eLANGUAGE_GERMAN:
                    eDEMORFPrintf(0x8C, 0x82, 0, "Die Bildschirmdarstellung erfolgt");
                    eDEMORFPrintf(0xDC, 0xAA, 0, "im 50Hz-Modus.");
                    break;

                case eLANGUAGE_FRENCH:
                    eDEMORFPrintf(0xC8, 0x82, 0, "L'affichage est en");
                    eDEMORFPrintf(0xF0, 0xAA, 0, "mode 50 Hz.");
                    break;

                case eLANGUAGE_ENGLISH:
                default:
                    eDEMORFPrintf(0x8C, 0x82, 0, "Screen display has been set");
                    eDEMORFPrintf(0xDC, 0xAA, 0, "to 50 Hz mode.");
                    break;
            }
        }

        eDEMODoneRender();
        VIWaitForRetrace();
    }
}

static inline int eProgressiveScanButtonReleased(int now_masked, int last_masked) {
    return (now_masked ^ last_masked) & last_masked;
}

int eProgressiveScan_EURGB60DialogBox(int mode) {
    int selection = 0;

    if (mode == 0) {

        selection = 1;

    } else if (mode == 1) {

        if (OSGetEuRgb60Mode()) {
            selection = 1;
        }
    }

    int bRunning = 1;
    OSTime start = OSGetTime() / (e_OSBusClock / 4 / 1000);

    do {

        for (int channel = 0; channel < 4; channel++) {

            if (g_InitPad[channel].err == 0) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
                g_LastInitPad[channel].stickX = g_InitPad[channel].stickX;
                g_LastInitPad[channel].stickY = g_InitPad[channel].stickY;
                g_LastInitPad[channel].err = g_InitPad[channel].err;

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                g_LastInitPad[channel].button = g_InitPad[channel].button;
            }
        }

        PADRead(g_InitPad);

        for (int i = 0; i < 4; i++) {
            PADClamp(&g_InitPad[i]);
            HardwarePadStatus[i].button = g_InitPad[i].button;
        }

        ReadLGWheelDataForProgressiveMenu();

        bool bDone = false;

        for (int channel = 0; !bDone && channel < 2; channel++) {

            if (g_InitPad[channel].err == 0) {

                if ((g_InitPad[channel].stickY > 20 && g_LastInitPad[channel].stickY < 20) ||
                    (g_InitPad[channel].stickY < -20 && g_LastInitPad[channel].stickY > -20) ||
                    eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_DOWN, g_LastInitPad[channel].button & PAD_BUTTON_DOWN) ||
                    eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_UP, g_LastInitPad[channel].button & PAD_BUTTON_UP)) {

                    selection = !selection;
                    start = OSGetTime() / (e_OSBusClock / 4 / 1000);
                    bDone = true;
                }

                if (eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_A, g_LastInitPad[channel].button & PAD_BUTTON_A) != 0) {

                    bRunning = 0;
                    bDone = true;
                }

            } else if (IsWheelActiveForProgressiveMenu(channel)) {

                g_InitPad[channel].button = ReadLGWheelButtonsForProgressiveMenu(channel);

                HardwarePadStatus[channel].button = g_InitPad[channel].button;

                if (eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_DOWN, g_LastInitPad[channel].button & PAD_BUTTON_DOWN) ||
                    eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_UP, g_LastInitPad[channel].button & PAD_BUTTON_UP)) {

                    bDone = true;
                    selection = !selection;
                    start = OSGetTime() / (e_OSBusClock / 4 / 1000);
                }

                if (eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_A, g_LastInitPad[channel].button & PAD_BUTTON_A) != 0) {

                    bDone = true;

                    bRunning = 0;
                }

            } else {

                PADReset(PADMASKS[channel]);
            }
        }

        if (OSGetTime() / (e_OSBusClock / 4 / 1000) > start + 10000) {

            bRunning = 0;
        }

        eDEMOBeforeRender();
        eDEMOInitCaption(0, 640, 448);

        if (mode == 0) {

            eDEMORFPrintf(0x6E, 0x82, 0, "Do you want to display the game");
            eDEMORFPrintf(0xB4, 0xAA, 0, "in Progressive Mode?");

            eDEMORFPrintf(0x118, 0x10E, 0, "Yes");
            eDEMORFPrintf(0x118, 0x12C, 0, "No");

        } else if (mode == 1) {

            switch (GC_GetOSLanguage()) {

                case eLANGUAGE_GERMAN:
                    eDEMORFPrintf(0xA0, 0x32, 0, "Dieses Spiel ist kompatibel");
                    eDEMORFPrintf(0xC8, 0x4B, 0, "zum 60Hz-Modus.");
                    eDEMORFPrintf(0xAF, 0x64, 0, "Bitte Modus ausw\344hlen.");

                    eDEMORFPrintf(0xA0, 0x96, 0, "Wenn das Spiel auf deinem");
                    eDEMORFPrintf(0x96, 0xAF, 0, "Fernsehger\344t im 60Hz-Modus");
                    eDEMORFPrintf(0x96, 0xC8, 0, "nicht korrekt dargestellt wird,");
                    eDEMORFPrintf(0x96, 0xE1, 0, "w\344hle bitte den 50Hz-Modus.");

                    eDEMORFPrintf(0xFA, 0x10E, 0, "60Hz-Modus");
                    eDEMORFPrintf(0xFA, 0x12C, 0, "50Hz-Modus");
                    break;

                case eLANGUAGE_FRENCH:
                    eDEMORFPrintf(0xBE, 0x4B, 0, "Ce jeu est compatible");
                    eDEMORFPrintf(0xC8, 0x64, 0, "avec le mode 60 Hz.");
                    eDEMORFPrintf(0x8C, 0x7D, 0, "Veuillez s\351lectionner un mode.");

                    eDEMORFPrintf(0xA0, 0xAF, 0, "Si l'image ne s'affiche pas");
                    eDEMORFPrintf(0x8C, 0xC8, 0, "correctement en mode 60 Hz,");
                    eDEMORFPrintf(0x78, 0xE1, 0, "veuillez s\351lectionner le mode 50 Hz.");

                    eDEMORFPrintf(0xFA, 0x10E, 0, "Mode 60 Hz");
                    eDEMORFPrintf(0xFA, 0x12C, 0, "Mode 50 Hz");
                    break;

                case eLANGUAGE_ENGLISH:
                default:
                    eDEMORFPrintf(0xA5, 0x4B, 0, "This game is compatible");
                    eDEMORFPrintf(0xC8, 0x64, 0, "with 60Hz mode.");
                    eDEMORFPrintf(0xB4, 0x7D, 0, "Please select mode.");

                    eDEMORFPrintf(0x9B, 0xAF, 0, "If your television does not");
                    eDEMORFPrintf(0x87, 0xC8, 0, "display correctly in 60Hz mode,");
                    eDEMORFPrintf(0xA0, 0xE1, 0, "please select 50Hz mode.");

                    eDEMORFPrintf(0xFA, 0x10E, 0, "60 Hz Mode");
                    eDEMORFPrintf(0xFA, 0x12C, 0, "50 Hz Mode");
                    break;
            }
        }

        if (selection) {

            eDEMORFPrintf(0xDC, 0x10E, 0, "*");

        } else {

            eDEMORFPrintf(0xDC, 0x12C, 0, "*");
        }

        eDEMODoneRender();
        VIWaitForRetrace();

    } while (bRunning);

    return selection;
}

OSFontHeader *FontData;
void *LastSheet;
unsigned short FontSize;
short FontSpace;

OSFontHeader *eDEMOInitROMFont() {
    if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
        FontData = (OSFontHeader *)bMalloc(OS_FONT_SIZE_SJIS, 0x800);
    } else {
        FontData = (OSFontHeader *)bMalloc(OS_FONT_SIZE_ANSI, 0x800);
    }

    if (FontData == NULL) {
        OSPanic("d:/mw/speed/gamecube/src/ecstasy/EcstasyE.cpp", 9304, "Ins. memory to load ROM font.");
    }
    if (!OSInitFont(FontData)) {
        OSPanic("d:/mw/speed/gamecube/src/ecstasy/EcstasyE.cpp", 9308, "ROM font is available in boot ROM ver 0.8 or later.");
    }

    FontSize = FontData->cellWidth * 16;
    FontSpace = -16;

    return FontData;
}

void eDEMODeleteROMFont() {
    if (FontData != NULL) {
        bFree(FontData);
    }

    FontData = NULL;
    FontSize = 0;
    FontSpace = 0;
}

void eDrawFontChar(int x, int y, int z, int sheet_x, int sheet_y) {
    int size = FontSize;

    short x0 = (short)x;
    short x1 = (short)(x0 + size);
    short y0 = (short)(y - FontData->ascent * (short)size / FontData->cellWidth);
    short y1 = (short)(y + FontData->descent * (short)size / FontData->cellWidth);

    short s0 = (short)sheet_x;
    short s1 = (short)(FontData->cellWidth + sheet_x);
    short t0 = (short)sheet_y;
    short t1 = (short)(FontData->cellHeight + sheet_y);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(x0, y0, (short)z);
    GXTexCoord2s16(s0, t0);
    GXPosition3s16(x1, y0, (short)z);
    GXTexCoord2s16(s1, t0);
    GXPosition3s16(x1, y1, (short)z);
    GXTexCoord2s16(s1, t1);
    GXPosition3s16(x0, y1, (short)z);
    GXTexCoord2s16(s0, t1);
    GXEnd();
}

void eLoadSheet(void *image, GXTexMapID id) {
    Mtx mtx;
    GXTexObj texObj;

    if (LastSheet == image) {
        return;
    }

    LastSheet = image;

    GXInitTexObj(&texObj, image, FontData->sheetWidth, FontData->sheetHeight, (GXTexFmt)FontData->sheetFormat, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    GXLoadTexObj(&texObj, id);

    PSMTXScale(mtx, 1.0f / FontData->sheetWidth, 1.0f / FontData->sheetHeight, 1.0f);

    GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

int eDEMORFPuts(short x, short y, short z, char *string) {
    void *image;
    long sheet_x;
    long sheet_y;
    long width;

    LastSheet = NULL;

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 4);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);

    x = (short)(x * 16);
    y = (short)(y * 16);
    z = (short)(z * 16);

    int xoffset = 0;

    while (*string != '\0') {

        if (*string == '\n') {

            xoffset = 0;
            y = (short)(y + FontData->leading * (short)FontSize / FontData->cellWidth);
            string++;

        } else if (*string == '\t') {

            int tab = ((short)FontSize + FontSpace) * 8;
            xoffset += tab;
            xoffset = xoffset / tab * tab;
            string++;

        } else {

            string = OSGetFontTexture(string, &image, &sheet_x, &sheet_y, &width);

            eLoadSheet(image, GX_TEXMAP0);
            eDrawFontChar(x + xoffset, y, z, sheet_x, sheet_y);
            xoffset += (short)FontSize * width / FontData->cellWidth + FontSpace;
        }
    }
    return (xoffset + 15) / 16;
}

int eDEMORFPrintf(short x, short y, short z, char *fmt, ...) {
    va_list vlist;
    char buffer[256];

    va_start(vlist, fmt);
    vsprintf(buffer, fmt, vlist);
    va_end(vlist);

    return eDEMORFPuts(x, y, z, buffer);
}

void eDEMOBeforeRender() {
    GXSetViewport(0.0f, 0.0f, (float)_rmode->fbWidth, (float)_rmode->efbHeight, 0.0f, 1.0f);

    GXInvalidateVtxCache();

    GXInvalidateTexAll();
}

void eDEMODoneRender() {
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);

    eCopyDisp(GX_TRUE);

    GXDrawDone();

    bSyncTaskRun();

    VIAdvanceFrame();
    VIWaitForRetrace();
}

void eDEMOSetupScrnSpc(long width, long height, float depth) {
    Mtx44 p;
    Mtx v;
    float zero = 0.0f;

    C_MTXOrtho(p, zero, (float)height, zero, (float)width, zero, -depth);
    GXSetProjection(p, GX_ORTHOGRAPHIC);
    PSMTXIdentity(v);
    GXLoadPosMtxImm(v, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

void eDEMOInitCaption(long font_type, long width, long height) {
    eDEMOSetupScrnSpc(width, height, 100.0f);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
}
