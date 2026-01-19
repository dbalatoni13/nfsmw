#include "EcstasyE.hpp"
#include "Ecstasy.hpp"
#include "EcstasyEx.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyEx.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin.h"

enum VIDEO_MODE {
    MODE_PAL = 0,
    MODE_PAL60 = 1,
    MODE_NTSC = 2,
    NUM_VIDEO_MODES = 3,
};

// TODO put these in correct headers
unsigned int bGetTicker();
int bGetFixTickerDifference(unsigned int start_ticks, unsigned int end_ticks);
int eSetDisplaySystem(int video_mode);
void eInitGX();
void eInitTexture();
void eExStartup();
void eDrawStartup();
void InitSlotPools();
int eInitEnvMap();
void SetScreenBuffers();
void eInitSunPat();
void eWaitUntilRenderingDone();
void InitSlotPoolsEx();
void eSetScissor(int xOrig, int yOrig, int wd, int ht);
void eSetCopyFilter(FILTER_ID filter_index, bool enable);
void eSetBackgroundColor(_GXColor clr);
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
void eSetPixelFormat(int nZFormat);
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

GXRenderModeObj *_rmode;                   // size: 0x4
GXRenderModeObj _rmodeObj;                 // size: 0x3C
Mtx44 projMOrthographic;                   // size: 0x40
Mtx viewMOrthographic;                     // size: 0x30
static const float GCNFEScale_NTSC = 0.0f; // size: 0x4
static const float GCNFEScale_PAL = 0.0f;  // size: 0x4
Mtx44 projMOrthographicScreenQuad;         // size: 0x40
Mtx viewMOrthographicScreenQuad;           // size: 0x30
EVIEWMODE CurrentViewMode = EVIEWMODE_NONE;

// idk where these go
Camera FlailerCamera;
Camera Player1Camera;
Camera Player2Camera;
Camera Player1RVMCamera;
Camera Player1HeadlightCamera;
Camera Player2HeadlightCamera;
Mtx44 Player1ReflectionProjection;
Mtx44 Player1SpecularProjection;
Mtx44 Player2ReflectionProjection;
Mtx44 Player2SpecularProjection;

volatile int FrameCounter = 0;
volatile unsigned int LastFrameCounterTick = 0;
VIDEO_MODE eCurrentVideoMode;
int ScreenWidth;
int ScreenHeight;
Mtx g_ScreenPositionMatrix;
SlotPool *ActiveTextureSlotPool;

Bool bEURGB60;                                                      // size: 0x1
int _firstFrame;                                                    // size: 0x4
void *_frameBuffer1;                                                // size: 0x4
void *_frameBuffer2;                                                // size: 0x4
void *_currentBuffer;                                               // size: 0x4
int _GxInitialized;                                                 // size: 0x4
static unsigned char e_bDither;                                     // size: 0x1
static unsigned int fbSize;                                         // size: 0x4
void *_defaultFIFO;                                                 // size: 0x4
GXFifoObj *_defaultFIFOObj;                                         // size: 0x4
volatile Bool bHangDiagnose;                                        // size: 0x1
volatile Bool bHangRecovery;                                        // size: 0x1
volatile int e_resync;                                              // size: 0x4
volatile int e_keepalive;                                           // size: 0x4
volatile int e_recover;                                             // size: 0x4
static const unsigned char DEFAULT_STALL = 0;                       // size: 0x1
static const unsigned char DEFAULT_DLSAVE = 0;                      // size: 0x1
Bool bStallWorkaround;                                              // size: 0x1
Bool bDLSaveContext;                                                // size: 0x1
int scis_xOrig;                                                     // size: 0x4
int scis_yOrig;                                                     // size: 0x4
int scis_wd;                                                        // size: 0x4
int scis_ht;                                                        // size: 0x4
int filt_00;                                                        // size: 0x4
int filt_01;                                                        // size: 0x4
int filt_10;                                                        // size: 0x4
int filt_11;                                                        // size: 0x4
int filt_12;                                                        // size: 0x4
int filt_sum;                                                       // size: 0x4
unsigned char CopyFilter[11][7];                                    // size: 0x4D
Bool bESyncError;                                                   // size: 0x1
volatile unsigned long eMAX_ITERATIONS;                             // size: 0x4
static char _err[256];                                              // size: 0x100
static char _err2[256];                                             // size: 0x100
static const bool EnableSunRender = false;                          // size: 0x1
static const bool EnableLetterBoxes = false;                        // size: 0x1
static const float TweakOverrideSunIntensityForRoadSpecular = 0.0f; // size: 0x4
bool EnableRainIn2P;                                                // size: 0x1
int efbWcrt;                                                        // size: 0x4
int xfbWcrt;                                                        // size: 0x4
bool IsPal50Mode;                                                   // size: 0x1
int xfbHcrt = 574;                                                  // size: 0x4
int efbHcrt = 480;                                                  // size: 0x4
float efbxfbRatio = 1.0f;                                           // size: 0x4
GXRenderModeObj PalNFS01IntDfScale;                                 // size: 0x3C
Bool bEProgressive;
volatile unsigned short e_sync;          // size: 0x2
volatile unsigned short e_endsync;       // size: 0x2
volatile unsigned short load_sync;       // size: 0x2
volatile unsigned short last_sync_token; // size: 0x2
Bool bNoWait;                            // size: 0x1
Bool bAlwaysCopyDisp;                    // size: 0x1
int nFilterUpdates;
int nDepthFormat;
TextureInfo *pTexPrev; // size: 0x4
GXTexObj HeadlightClipTextureObj;
void *HeadlightClipTextureTestData = (void *)0xFFFF0000;
int FastForwardEnabled = 0;
int FastForwardRate = 4;
int EnableTexturing = true;
int DrawWireframe = 0;
cReflectMap ReflectMap;
cQuarterSizeMap QuarterSizeMap;

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
    ActiveTextureSlotPool = bNewSlotPool(8, 256, "ActiveTextureSlotPool", 0);
    ActiveTextureSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
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
        view->SetActive(1);
        view->Precipitation = eGetView(1, false)->Precipitation;
        view->GetPlatInfo()->SetLightPerspectiveProjection(&Player1SpecularProjection);
    }
    {
        eView *view = eGetView(7, false);
        view->SetRenderTarget0(eGetRenderTarget(5));
        view->SetCamera(&Player2Camera);
        view->SetActive(0);
        view->Precipitation = eGetView(2, false)->Precipitation;
        view->GetPlatInfo()->SetLightPerspectiveProjection(&Player2SpecularProjection);
    }
    {
        eView *view = eGetView(8, false);
        view->SetCamera(&Player1Camera);
        view->SetRenderTarget(eGetRenderTarget(6), 0);
        view->SetActive(0);
    }
    {
        eView *view = eGetView(9, false);
        view->SetCamera(&Player1Camera);
        view->SetRenderTarget0(eGetRenderTarget(7));
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

void eUpdateViewMode(void) {
    bNode *node1;
    bNode *node2;
    bNode *node3;

    eView *eview_player1 = eGetView(1, false);
    eView *eview_player2 = eGetView(2, false);
    eView *eview_player1_rvm = eGetView(3, false);

    node1 = (eview_player1->CameraMoverList).HeadNode.GetNext();
    node3 = (bNode *)0x0;
    if (node1 != (bNode *)&eview_player1->CameraMoverList) {
        node3 = node1;
    }
    if (node3 != (bNode *)0x0) {
        node1 = (eview_player2->CameraMoverList).HeadNode.GetNext();
        node3 = (bNode *)0x0;
        if (node1 != (bNode *)&eview_player2->CameraMoverList) {
            node3 = node1;
        }
        if (node3 != (bNode *)0x0) {
            CurrentViewMode = EVIEWMODE_TWOH;
            return;
        }
    }
    node1 = (eview_player1->CameraMoverList).HeadNode.GetNext();
    node3 = (bNode *)0x0;
    if (node1 != (bNode *)&eview_player1->CameraMoverList) {
        node3 = (eview_player1->CameraMoverList).HeadNode.GetNext();
    }
    if (node3) {
        node2 = (eview_player1_rvm->CameraMoverList).HeadNode.GetNext();
        node3 = (bNode *)0x0;
        if (node2 != (bNode *)&eview_player1_rvm->CameraMoverList) {
            node3 = node2;
        }
        if (node3) {
            CurrentViewMode = EVIEWMODE_ONE_RVM;
            return;
        }
    }
    CurrentViewMode = EVIEWMODE_NONE;
    if (node1 != (bNode *)&eview_player1->CameraMoverList) {
        CurrentViewMode = EVIEWMODE_TWOH; // (EVIEWMODE)node1;
    }
    if (CurrentViewMode != EVIEWMODE_NONE) {
        CurrentViewMode = EVIEWMODE_ONE;
    }
}

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

TextureInfo *pTextureInfoRadialBlur;
int VifTime;

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

int eClampTopLeft(bool bOnOff, int nUnused) {
    return false;
}

// UNSOLVED https://decomp.me/scratch/xURfH
bool IsSunInFrustrum(eView *player_view) {
    SunChunkInfo *sun_info = SunInfo;

    if (!sun_info) {
        return false;
    }
    bVector2 sunpos_xy(sun_info->PositionX, sun_info->PositionY);
    bVector2 campos_xy(player_view->GetCamera()->GetPosition()->x, player_view->GetCamera()->GetPosition()->y);

    bVector2 to_pt_xy_un = sunpos_xy - campos_xy;
    bVector2 cam_dir_xy_un(player_view->GetCamera()->GetDirection()->x, player_view->GetCamera()->GetDirection()->y);
    bVector2 to_pt_xy = bNormalize(to_pt_xy_un);
    bVector2 cam_dir_xy = bNormalize(cam_dir_xy_un);

    float dotp = bDot(&to_pt_xy, &cam_dir_xy);

    return dotp > 0.5f;
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
    return ScreenWidth;
}

void eSetScreenDisplayOffsets(int offset_x, int offset_y) {}

void eSetScreenDisplayOffsets(int &offset_x, int &offset_y) {}

eRenderTarget RenderTargets[15];
TextureInfo RenderTargetTextureInfos[15];
eRenderTarget *CurrentRenderTarget;

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
    if (render_target) {
        GXSetViewport(render_target->ScissorX, render_target->ScissorY, render_target->FrameWidth, render_target->FrameHeight, 0.0, 1.0);
        eSetScissor(CurrentRenderTarget->ScissorX, CurrentRenderTarget->ScissorY, CurrentRenderTarget->ScissorW, CurrentRenderTarget->ScissorH);
        eSetBackgroundColor(CurrentRenderTarget->BackgroundColour);
        eSetCopyFilter(CurrentRenderTarget->GetCopyFilter(), false);
    }
}

eRenderTarget *eGetRenderTarget(int render_target) {
    return &RenderTargets[render_target];
}

void eWaitUntilRenderingDone() {}

float CalculateH(unsigned short alpha) {
    unsigned short beta = alpha >> 1;
    float tan = bTan(beta);
    if (tan != 0.0f) {
        return 256.0f / tan;
    }
    return 10.0f;
}

// int epSetAllStripsVisibleState(eSolid *param1, int param2) {
//   param1->GetNext();
//   return 0;
// }

void eInitGX() {
    __InitRenderMode();
    __InitMem();
    VIConfigure(_rmode);
    _defaultFIFO = bMalloc(0x42000, "TODO", __LINE__, 0x800);
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
    switch (VIGetTvFormat()) {
        case 0:
            if (bEProgressive) {
                _rmode = &GXNtsc480Prog;
            } else {
                _rmode = &GXNtsc480IntDf;
            }
            break;
        case 1:
            _rmode = &PalNFS01IntDfScale;
            break;
        case 5:
            _rmode = &GXEurgb60Hz480IntDf;
            break;
        case 2:
            _rmode = &GXMpal480IntDf;
            break;
        default:
            OSPanic("src/ecstasy/EcstasyE.cpp", 0x1281, "Init: invalid TV format\n");
            break;
    }
    efbxfbRatio = static_cast<float>(efbHcrt) / xfbHcrt;
    GXAdjustForOverscan(_rmode, &_rmodeObj, 0, 0x10);

    _rmodeObj = *_rmode;
    _rmode = &_rmodeObj;
}

void __InitMem() {
    fbSize = (((_rmode->fbWidth + 15) & 0xfff0) * _rmode->xfbHeight) * 2;
    void *pFB = _frameBuffer1 = bMalloc(fbSize * 2, "TODO", __LINE__, 0x800);
    _currentBuffer = (u8 *)pFB + fbSize;
    _frameBuffer2 = (u8 *)pFB + fbSize;
}

void __InitGXlite(void) {
    GXSetViewport(0.0f, 0.0f, _rmode->fbWidth, _rmode->xfbHeight, 0.0f, 1.0f);
    scis_xOrig = 0;
    scis_wd = _rmode->fbWidth;
    scis_ht = _rmode->efbHeight - 24;
    scis_yOrig = 12;
    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    eSetCulling(GX_CULL_NONE);
    eResetBlendMode();
    eResetZBuffering();
    eSetColourUpdate(1, 1);
    for (int i = 0; i < 16; i++) {
        GXSetTevKColorSel(static_cast<GXTevStageID>(i), GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel(static_cast<GXTevStageID>(i), GX_TEV_KASEL_1);
    }
    vsReset(0);
    vsResetTexGen(0, 0);
    psReset(RESET_OPTION_DEFAULT);
    ps_NoLighting(0, 0);
}

void __InitGX(void) {
    GXSetViewport(0.0f, 0.0f, _rmode->fbWidth, _rmode->xfbHeight, 0.0f, 1.0f);
    scis_yOrig = 12;
    scis_wd = _rmode->fbWidth;
    scis_ht = _rmode->efbHeight - 24;
    scis_xOrig = 0;
    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    GXSetDispCopySrc(0, 0, _rmode->fbWidth, _rmode->efbHeight);
    GXSetDispCopyDst(_rmode->fbWidth, _rmode->xfbHeight);
    GXSetDispCopyYScale(static_cast<float>(_rmode->xfbHeight) / static_cast<float>(_rmode->efbHeight));
    filt_00 = _rmode->vfilter[0];
    filt_01 = _rmode->vfilter[1];
    filt_10 = _rmode->vfilter[2];
    filt_11 = _rmode->vfilter[3];
    filt_12 = _rmode->vfilter[4];
    GXSetCopyFilter(_rmode->aa, _rmode->sample_pattern, GX_TRUE, _rmode->vfilter);
    GXSetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
    GXSetFieldMode(_rmode->field_rendering, _rmode->xfbHeight < _rmode->viHeight);
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
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    eSetColourUpdate(1, 1);
    eSetZBuffering(1, 1);
    vsReset(0);
    vsResetTexGen(0, 0);
    vsVtxAttrFmt(0);
    psReset(RESET_OPTION_DEFAULT);
    ps_NoLighting(0, 0);
    psGouraud(0);
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
    return;
}

void __InitMatrices(void) {
    Mtx fe_scale;

    float transx = (_rmode->fbWidth - _rmode->fbWidth * 0.95f) * 0.5f;
    float transy = (_rmode->efbHeight - _rmode->efbHeight * 0.95f) * 0.5f;
    MTXScale(fe_scale, 0.95f, 0.95f, 1.0f);
    MTXTransApply(fe_scale, viewMOrthographic, transx, transy, 0.0f);
    C_MTXOrtho(projMOrthographic, 0.0f, _rmode->efbHeight, 0.0f, _rmode->fbWidth, 0.0f, -16777215.0f);
    MTXIdentity(viewMOrthographicScreenQuad);
    C_MTXOrtho(projMOrthographicScreenQuad, 0.0f, _rmode->efbHeight, 0.0f, _rmode->fbWidth, 0.0f, -16777215.0f);
    GXSetCurrentMtx(0);
    GXLoadPosMtxImm(viewMOrthographic, 0);
    GXSetProjection(projMOrthographic, GX_ORTHOGRAPHIC);
}

void eBeginScene(void) {
    static Bool bFirstTime = false;

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
    GXSetViewport(0.0f, 0.0f, _rmode->fbWidth, _rmode->xfbHeight, 0.0f, 1.0f);
    if (nFilterUpdates == 0) {
        eUpdateCopyFilter2(0);
    } else {
        eUpdateCopyFilter2(1);
        nFilterUpdates--;
    }
    eSetScissor(scis_xOrig, scis_yOrig, scis_wd, scis_ht);
    eStallWorkaround(bStallWorkaround);
    eDLSaveContext(bDLSaveContext);
    eSetPixelFormat(nDepthFormat);
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
    static Bool _enabled;
    if (bEnabled != _enabled) {
        GXSetMisc(GX_MT_DL_SAVE_CONTEXT, bEnabled != 0);
        _enabled = bEnabled;
    }
}

void eSetBackgroundColor(_GXColor clr) {
    GXSetCopyClear(clr, 0x00FFFFFF);
}

void eSetPixelFormat(int nZFormat) {
    static GXZFmt16 prevFmt;
    static GXZFmt16 _formats[4];

    GXZFmt16 fmt = _formats[nZFormat];
    if (fmt != prevFmt) {
        GXSetPixelFmt(GX_PF_RGBA6_Z24, fmt);
        prevFmt = fmt;
    }
}

void eSetScissor(int xOrig, int yOrig, int wd, int ht) {
    static int _xOrig;
    static int _yOrig;
    static int _wd;
    static int _ht;

    if ((xOrig != _xOrig) || (yOrig != _yOrig) || (wd != _wd) || (ht != _ht)) {
        GXSetScissor(xOrig, yOrig, wd, ht);
        _xOrig = xOrig;
        _yOrig = yOrig;
        _wd = wd;
        _ht = ht;
    }
}

void eSetCopyFilter(FILTER_ID filter_index, bool enable) {
    GXSetCopyFilter(_rmode->aa, _rmode->sample_pattern, GX_TRUE, CopyFilter[filter_index]);
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
    GXColor clr = {0};
    clr.a = 0xFF;
    GXSetFog(GX_FOG_LIN, 16.0f, 512.0f, 0.5f, 10000.0f, clr);
    GXInitTexObj(&HeadlightClipTextureObj, &HeadlightClipTextureTestData, 2, 1, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&HeadlightClipTextureObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
}

cSphereMap SphereMap;
cSpecularMap SpecularMap;

cQuarterSizeMap QSizeI8_Z8;
cQuarterSizeMap QSizeScratchPad;
cQuarterSizeMap QSizeAccumulationI8;
;

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

    PSMTXScale(g_m1, 0.5f, -0.5f, 0.0f);
    PSMTXTrans(g_m0, 0.5f, 0.5f, 1.0f);
    PSMTXConcat(g_m0, g_m1, g_ScreenPositionMatrix);

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

unsigned int e_retrace_count; // size: 0x4

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

static volatile Bool bDrawDoneEncountered;

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
extern "C" {
unsigned short GXReadDrawSync();
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

float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks);

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
    static Bool _enabled = false;
    if (bEnabled != _enabled) {
        GXSetMisc(GX_MT_XF_FLUSH, bEnabled ? 8 : 0);
        _enabled = bEnabled;
    }
}

void eDiagnoseHang(void) {
    unsigned long xfTop0; // r1+0x10
    unsigned long xfBot0; // r1+0xC
    unsigned long suRdy0; // r1+0x18
    unsigned long r0Rdy0; // r1+0x14
    unsigned long xfTop1; // r1+0x20
    unsigned long xfBot1; // r1+0x1C
    unsigned long suRdy1; // r1+0x28
    unsigned long r0Rdy1; // r1+0x24
    unsigned long xfTopD; // r29
    unsigned long xfBotD; // r31
    unsigned long suRdyD;
    unsigned long r0RdyD;
    unsigned char readIdle; // r1+0x9
    unsigned char cmdIdle;  // r1+0xA
    unsigned char junk;     // r1+0x8

    GXReadXfRasMetric(&xfBot0, &xfTop0, &r0Rdy0, &suRdy0);
    GXReadXfRasMetric(&xfBot1, &xfTop1, &r0Rdy1, &suRdy1);
    GXGetGPStatus(&junk, &junk, &readIdle, &cmdIdle, &junk);
}

void eHangMetric(Bool bEnable) {
    if (!bEnable) {
        // *0xCC008000 = 0x61;
        // *0xCC008000 = 0x2402C004;
        // *0xCC008000 = 0x61;
        // *0xCC008000 = 0x02300020;
        // *0xCC008000 = 0x10;
        // *0xCC008000 = 0;
        // *0xCC008000 = 0x1006;
        // *0xCC008000 = 0x84400;
    } else {
        GXSetGPMetric(GX_PERF0_NONE, GX_PERF1_NONE);
        // *0xCC008000 = 0x61;
        // *0xCC008000 = 0x24000000;
        // *0xCC008000 = 0x61;
        // *0xCC008000 = 0x23000000;
        // *0xCC008000 = 0x10;
        // *0xCC008000 = arg0;
        // *0xCC008000 = 0x1006;
        // *0xCC008000 = arg0;
    }
}

// ...

// TODO
void eDEMOSetupScrnSpc(long width /* r3 */, long height /* r4 */, float depth /* f1 */);

void eDEMOInitCaption(long font_type, long width, long height) {
    eDEMOSetupScrnSpc(width, height, 100.0f);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
}
