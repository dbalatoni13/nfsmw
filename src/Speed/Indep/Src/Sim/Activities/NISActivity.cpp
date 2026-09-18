#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Animation/AnimCandidates.hpp"
#include "Speed/Indep/Src/Animation/AnimChooser.hpp"
#include "Speed/Indep/Src/Animation/AnimLocator.hpp"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Animation/AnimScene.hpp"
#include "Speed/Indep/Src/Animation/AnimWorldTypes.hpp" // needed for WAM_
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/ELoadingScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayRaceMovie.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayEndNIS.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNISComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"

#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"

#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EBecomePlayerCar.hpp"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Speech/EAXDispatch.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Generated/Events/EBecomeAiCar.hpp"
#include "Speed/Indep/Src/World/FacePixelate.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "types.h"

// total size: 0x8
class NISCar {
  public:
    NISCar(UCrc32 channel, IVehicle *vehicle);
    ~NISCar();

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    IVehicle *mIVehiclePtr; // offset 0x0, size 0x4
    UCrc32 mChannelName;    // offset 0x4, size 0x4
};

// Why not pass channel?
NISCar::NISCar(UCrc32 channel, IVehicle *vehicle) : mIVehiclePtr(vehicle), mChannelName() {}

NISCar::~NISCar() {
    if (mIVehiclePtr) {
        mIVehiclePtr = nullptr;
    }
}

// total size: 0x94
class CAnimMomentScene : public ICEScene {
  public:
    CAnimMomentScene(unsigned int sceneHash, int camera_track_number, bMatrix4 &rot, bMatrix4 &transform)
        : mSceneHash(sceneHash),                     //
          mCamera_track_number(camera_track_number), //
          mTimeElapsed(0.0f),                        //
          mTotalTime(1.0f),                          //
          mSceneRotationMatrix(rot),                 //
          mSceneTransformMatrix(transform) {
        if (TheICEManager.GetCameraData(sceneHash, camera_track_number)) {
            ICETrack *track = TheICEManager.GetPlaybackTrack();
            if (track) {
                mTotalTime = track->Length;
            }
        }
    }

    // El cuerpo va DENTRO de la clase a proposito: en GCC 2.9 eso lo manda al
    // bloque de inlines diferidos del final de la unidad, que es donde lo tiene
    // el objetivo (0x16F38). Definido fuera salia aqui y desplazaba 64 funciones.
    virtual ~CAnimMomentScene() {}

    uint32 GetSceneHash() override {
        return mSceneHash;
    }
    int GetCameraTrackNumber() override {
        return mCamera_track_number;
    }
    bool IsControllingCamera() override {
        return true;
    }
    bool IsCameraFixingElevation() override {
        return false;
    }
    void SetTime(float time) override {
    }
    bool Pause() override {
        return true;
    }
    bool UnPause() override {
        return true;
    }
    bool IsPlaying() override {
        return true;
    }
    void Update(float dT) {
        mTimeElapsed += dT;
    }
    bool IsFinished() {
        return mTimeElapsed >= mTotalTime;
    }
    float GetTimeStart() override {
        return 0.0f;
    }
    float GetTimeTotalLength() override {
        return mTotalTime;
    }
    float GetTimeElapsed() override {
        return mTimeElapsed;
    }
    const bMatrix4 &GetSceneRotationMatrix() override {
        return mSceneRotationMatrix;
    }
    const bMatrix4 &GetSceneTransformMatrix() override {
        return mSceneTransformMatrix;
    }

    uint32 mSceneHash;              // offset 0x4, size 0x4
    int mCamera_track_number;       // offset 0x8, size 0x4
    float mTimeElapsed;             // offset 0xC, size 0x4
    float mTotalTime;               // offset 0x10, size 0x4
    bMatrix4 mSceneRotationMatrix;  // offset 0x14, size 0x40
    bMatrix4 mSceneTransformMatrix; // offset 0x54, size 0x40
};

// TODO is this really in here? probably not and it's a bug in debug_lines.txt
// total size: 0x438
class NISActivity : public Sim::Activity, public INIS, public EventSequencer::IContext, public IVehicleCache {
  public:
    enum NISACTIVITY_STATE {
        NISACTIVITY_NONE = 0,
        NISACTIVITY_CREATING = 1,
        NISACTIVITY_LOADING = 2,
        NISACTIVITY_READY_TO_PLAY = 3,
        NISACTIVITY_PRE_MOVIE = 4,
        NISACTIVITY_PLAYING = 5,
        NISACTIVITY_POST_MOVIE = 6,
        NISACTIVITY_COMPLETE = 7,
    };

    typedef struct UTL::Std::map<UCrc32, NISCar *, _type_map> CarList;

    static void NISStreamTimeCallback(unsigned int animid, int mselapsed);

    static Sim::IActivity *Construct(Sim::Param params) {
        INIS *nis = INIS::Get();
        if (nis) {
            return nullptr;
        }
        return new NISActivity();
    }

    NISActivity();

    // Virtual functions
    // IUnknown
    ~NISActivity() override;

    void Load(CAnimChooser::eType nisType, const char *scene, int cameratrack, bool PlayAsSoonAsLoaded) override;
    void AddCar(UCrc32 channel, IVehicle *vehicle) override;
    IVehicle *GetCar(UCrc32 channelname) override;
    void StartLocation(const UMath::Vector3 &position, float direction) override;
    void StartLocationInRenderCoords(const bVector3 &position, unsigned short direction) override;
    const UMath::Vector3 *GetStartLocation() override;
    const UMath::Vector3 *GetStartCameraLocation() override;

    CAnimChooser::eType GetType() override {
        return mNISType;
    }

    void SetPreMovie(const char *movieName) override;
    void SetPostMovie(const char *movieName) override;
    void ResetEvents(float SetTime) override;
    void StartEvents() override;
    void FireEventTag(const char *tagName) override;
    void ServiceLoads() override;
    ICEScene *GetScene() const override;
    CAnimScene *GetAnimScene() const override;

    bool IsLoaded() const override {
        return mState > NISACTIVITY_LOADING;
    }

    bool IsPlaying() const override {
        return mState == NISACTIVITY_PLAYING;
    }

    bool InMovie() const override {
        return mState == NISACTIVITY_PRE_MOVIE || mState == NISACTIVITY_POST_MOVIE;
    }

    void StartPlayingNow() override {
        if (IsLoaded() && !mStartPlayingNow) {
            mStartPlayingNow = true;
        }
    }

    void Pause() override;
    void UnPause() override;
    void Release() override;

    // IContext
    bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) override;

#ifndef EA_BUILD_A124
    // IVehicleCache
    const char *GetCacheName() const override {
        return "NISActivity";
    }
#endif

    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    // ITaskable
    bool OnTask(HSIMTASK task, float dT) override;

    // IAttachable
    void OnDetached(IAttachable *pOther) override;

    // INIS
    bool SkipOverNIS() override;

    bool IsWorldMomement() const override {
        return mNISType == CAnimChooser::Moment;
    }

  private:
    void OnMovieComplete(const MNotifyMovieFinished &message);
    void Unload();
    void RemoveCar(IVehicle *vehicle);
    bool GetNISStartLocation(UMath::Vector3 &position);
    void UpdatePreloading();
    void PrepareVehicles(int carListType);
    void UpdateLoading();
    void Play();
    void UpdatePlaying(float dT);
    bool IsAudioStreamQueued();
    bool IsCarListLoaded();
    void JoyHandle(IPlayer *player);

    HSIMTASK mUpdate;                             // offset 0x6C, size 0x4
    float mNISElapsedTime;                        // offset 0x70, size 0x4
    ActionQueue mActionQ;                         // offset 0x74, size 0x294
    NISACTIVITY_STATE mState;                     // offset 0x308, size 0x4
    Hermes::HHANDLER mMsgMovieComplete;           // offset 0x30C, size 0x4
    CAnimChooser::eType mNISType;                 // offset 0x310, size 0x4
    bVector3 mNISPosition;                        // offset 0x314, size 0x10
    float mNISDirection;                          // offset 0x324, size 0x4
    char mPreMovie[64];                           // offset 0x328, size 0x40
    char mPostMovie[64];                          // offset 0x368, size 0x40
    unsigned int mSceneHash;                      // offset 0x3A8, size 0x4
    UCrc32 mSequencerID;                          // offset 0x3AC, size 0x4
    CAnimScene *mAnimScene;                       // offset 0x3B0, size 0x4
    int mAnimHandle;                              // offset 0x3B4, size 0x4
    int mCameraTrackNumber;                       // offset 0x3B8, size 0x4
    float mDefault_MaxTicksPerTimestep;           // offset 0x3BC, size 0x4
    CarList mVehicleTable;                        // offset 0x3C0, size 0x10
    EventSequencer::IEngine *mSequencer;          // offset 0x3D0, size 0x4
    ALIGN_16 UMath::Vector3 mStartLocation;       // offset 0x3D4, size 0xC
    ALIGN_16 UMath::Vector3 mStartCameraLocation; // offset 0x3E0, size 0xC
    bool mStartPlayingNow;                        // offset 0x3EC, size 0x1
    bool mRunningThroughICE;                      // offset 0x3F0, size 0x1
    int mLoadAttemptCount;                        // offset 0x3F4, size 0x4
    int mSuspensionSettle;                        // offset 0x3F8, size 0x4
    CAnimMomentScene *mMomentScene;               // offset 0x3FC, size 0x4
    bool mUsingFEngOverlay;                       // offset 0x400, size 0x1
    bool mCareerNIS;                              // offset 0x404, size 0x1
    bool mDDayNIS;                                // offset 0x408, size 0x1
#ifndef EA_BUILD_A124
    bool mBlackListNIS;   // offset 0x40C, size 0x1
    bool mNonSkipableNIS; // offset 0x410, size 0x1
    char mSkipToNIS[16];  // offset 0x414, size 0x10
    bool mNISSkipped;     // offset 0x424, size 0x1
    float mNISNoSkipTime; // offset 0x428, size 0x4
#else
    bool mDDayNISSkipped;
#endif
    int mIsPrecipitationEnable; // offset 0x42C, size 0x4
    bool mPause;                // offset 0x430, size 0x1
    float loadStartTime;        // offset 0x434, size 0x4

    static int mElapsedmsAudioTime;
};

extern int PrecipitationEnable;
extern int SkipNISs;
extern unsigned int SpecialCarList1[];
extern unsigned int SpecialCarList2[];
extern unsigned int SpecialCarList3[];
extern unsigned int SpecialCarList4[];
extern unsigned int SpecialCarList5[];
extern unsigned int SpecialCarList6[];
extern unsigned int SpecialCarList7[];
extern unsigned int SpecialCarList8[];
extern unsigned int SpecialCarList9[];
extern int AnimCfg_DisableAnimations;
void NIS_NukeSmackablesWithinRange(const UMath::Vector3 &position, float range);

namespace ICE {
void HideOverlay();
}
extern bool gTWEAKER_NISLightEnabled;
extern int Tweak_DisableRoadNoise;
extern float MaxTicksPerTimestep;
void SetNISRaceDriverVisible(int visible);
void EmitterSystem_OnStartNIS();
void EmitterSystem_OnEndNis();
void InitTopologyAndSceneryGroups();
void CloseTopologyAndSceneryGroups();

IMPLEMENT_SINGLETON(INIS)

const char *NisNamesToDisablePreculler[] = {"IntroNisBL09"};

int NISActivity::mElapsedmsAudioTime = -1;

int Tweak_EnableNISMomements = 1;

// UNSOLVED TODO Hermess::Handler stuff
NISActivity::NISActivity()
    : Sim::Activity(3),               //
      INIS(this),                     //
      EventSequencer::IContext(this), //
      IVehicleCache(this),            //
      mNISElapsedTime(0.0f),          //
      mActionQ(true),                 //
      mState(NISACTIVITY_NONE),       //
                                      // TODO magic
      mMsgMovieComplete(
          Hermes::Handler::Create<MNotifyMovieFinished, NISActivity, NISActivity>(this, &NISActivity::OnMovieComplete, UCrc32(0x20d60dbf), 0)),
      mNISType(CAnimChooser::Intro),      //
      mSceneHash(0),                      //
      mSequencerID(),                     //
      mAnimScene(nullptr),                //
      mAnimHandle(0),                     //
      mCameraTrackNumber(0),              //
      mDefault_MaxTicksPerTimestep(4.0f), //
      mSequencer(nullptr),                //
      mRunningThroughICE(false),          //
      mLoadAttemptCount(0),               //
      mSuspensionSettle(0),               //
      mMomentScene(nullptr),              //
      mUsingFEngOverlay(false),           //
      mCareerNIS(false),                  //
      mDDayNIS(false),                    //
#ifndef EA_BUILD_A124
      mBlackListNIS(false),   //
      mNonSkipableNIS(false), //
      mNISSkipped(false),     //
      mNISNoSkipTime(0.0f),   //
#endif
      mPause(false),       //
      loadStartTime(0.0f), //
      mStartPlayingNow(true) {
    mPreMovie[0] = '\0';
    mPostMovie[0] = '\0';
#ifndef EA_BUILD_A124
    mSkipToNIS[0] = '\0';
#endif

    mUpdate = AddTask("WorldUpdate", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mUpdate, "NIS");
    mIsPrecipitationEnable = PrecipitationEnable;
}

NISActivity::~NISActivity() {
    Hermes::Handler::Destroy(mMsgMovieComplete);
    RemoveTask(mUpdate);
    Unload();
    g_pEAXSound->NISFinished();

    if (bStringHash("IntroNisFlyInDD") == mSceneHash) {
        SetSoundControlState(false, SNDSTATE_NIS_321, "DDAY flyin");
    }

    for (int i = 0; i < 8; i++) {
        char channelName[32];
        bSPrintf(channelName, "cop%d", i + 1);
        IVehicle *copCar = GetCar(UCrc32(channelName));
        if (copCar) {
            RemoveCar(copCar);
            ISimable *iSim;
            if (copCar->QueryInterface(&iSim)) {
                iSim->Kill();
            }
        }
    }

    for (CarList::iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
        NISCar *car = i->second;
        if (car->mIVehiclePtr) {
            car->mIVehiclePtr->SetAnimating(false);
        }
        if (car) {
            delete car;
        }
    }

    mVehicleTable.clear();

    if (mMomentScene) {
        delete mMomentScene;
        mMomentScene = nullptr;
    }

    ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;

    new ESndGameState(4, false);
#ifndef EA_BUILD_A124
    if (mNISSkipped) {
        new EPlayEndNIS(mSkipToNIS);
    } else {
        MNISComplete(nullptr).Post(UCrc32(0x20d60dbf)); // magic
    }
#else
    // TODO PS2
#endif
}

ICEScene *NISActivity::GetScene() const {
    if (mMomentScene) {
        return mMomentScene;
    } else {
        return GetAnimScene();
    }
}

CAnimScene *NISActivity::GetAnimScene() const {
    if (IsLoaded() && mAnimHandle) {
        return TheAnimPlayer.FindAnimScene(mAnimHandle);
    }
    return nullptr;
}

void NISActivity::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        RemoveCar(ivehicle);
    }
}

eVehicleCacheResult NISActivity::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    for (CarList::const_iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
        const NISCar *car = (*i).second;
        if (UTL::COM::ComparePtr(removethis, car->mIVehiclePtr)) {
            return VCR_WANT;
        }
    }
    return VCR_DONTCARE;
}

void NISActivity::OnRemovedVehicleCache(IVehicle *ivehicle) {}

void NISActivity::RemoveCar(IVehicle *vehicle) {
    for (CarList::iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
        const NISCar *car = (*i).second;
        if (UTL::COM::ComparePtr(vehicle, car->mIVehiclePtr)) {
            mVehicleTable.erase(i);
            vehicle->SetAnimating(false);
            delete car;
            break;
        }
    }
}

void NISActivity::AddCar(UCrc32 channel, IVehicle *vehicle) {
    if (!static_cast<IActivity *>(this)->Attach(vehicle)) {
        return;
    }
    vehicle->SetAnimating(true);
    vehicle->SetSpeed(0.0f);

    CarList::iterator iter = mVehicleTable.find(channel);
    if (iter != mVehicleTable.end()) {
        NISCar *car = (*iter).second;
        delete car;
    }
    mVehicleTable[channel] = new NISCar(channel, vehicle);
}

IVehicle *NISActivity::GetCar(UCrc32 channelname) {
    IVehicle *car = nullptr;
    CarList::iterator iter = mVehicleTable.find(channelname);
    if (iter != mVehicleTable.end()) {
        NISCar *nisCar = (*iter).second;
        car = nisCar->mIVehiclePtr;
    }
    return car;
}

void NISActivity::StartLocation(const UMath::Vector3 &position, float direction) {
    mStartLocation = position;
    eSwizzleWorldVector(position, mNISPosition);
    mNISDirection = direction;
    unsigned short bang = static_cast<unsigned short>(direction * 65536.0f);
    CAnimLocator::SetAnimOriginPosition(mNISPosition, bang);
}

void NISActivity::StartLocationInRenderCoords(const bVector3 &position, unsigned short direction) {
    mNISPosition = position;
    eUnSwizzleWorldVector(position, *reinterpret_cast<bVector3 *>(&mStartLocation));
    mNISDirection = direction / 65536.0f;
    CAnimLocator::SetAnimOriginPosition(position, direction);
}

const UMath::Vector3 *NISActivity::GetStartLocation() {
    return &mStartLocation;
}

const UMath::Vector3 *NISActivity::GetStartCameraLocation() {
    if (mLoadAttemptCount == 0) {
        bool foundPos = false;
        UMath::Vector3 NIS_Position;
        if (GetNISStartLocation(NIS_Position)) {
            foundPos = true;
            mStartCameraLocation = NIS_Position;
        }
        if (!foundPos) {
            mStartCameraLocation = mStartLocation;
        }
    }
    return &mStartCameraLocation;
}

void NISActivity::SetPreMovie(const char *movieName) {
    mPreMovie[0] = '\0';
    if (movieName && bStrLen(movieName) > 0) {
        bSafeStrCpy(mPreMovie, movieName, sizeof(mPreMovie));
    }
}

void NISActivity::SetPostMovie(const char *movieName) {
    mPostMovie[0] = '\0';
    if (movieName && bStrLen(movieName) > 0) {
        bSafeStrCpy(mPostMovie, movieName, sizeof(mPostMovie));
    }
}

bool NISActivity::GetNISStartLocation(UMath::Vector3 &position) {
    if (mSceneHash != 0) {
        ICEData *iceData = TheICEManager.GetCameraData(mSceneHash, mCameraTrackNumber);
        if (!iceData) {
            return false;
        } else {
            ICE::Vector3 w_eye;
            if (iceData->nSpaceEye == 1) {
                iceData->GetEye(0, &w_eye);
            } else {
                if (iceData->nSpaceEye != 3) {
                    return false;
                }
                ICE::Vector3 v_eye;
                bMatrix4 scene_rotation_matrix;
                bMatrix4 scene_translation_matrix;
                bMatrix4 scene_transform_matrix;
                iceData->GetEye(0, &v_eye);
                CAnimLocator::GetInitialAnimMatricies(&scene_rotation_matrix, &scene_translation_matrix, true);
                bMulMatrix(&scene_transform_matrix, &scene_translation_matrix, &scene_rotation_matrix);
                ICE::MulVector(&w_eye, reinterpret_cast<ICE::Matrix4 *>(&scene_transform_matrix), &v_eye);
            }
            position.x = -w_eye.y;
            position.y = w_eye.z;
            position.z = w_eye.x;
            return true;
        }
    }
    return false;
}

void NISActivity::Load(CAnimChooser::eType nisType, const char *scene, int cameratrack, bool PlayAsSoonAsLoaded) {
    loadStartTime = RealTimer.GetSeconds();
    mNISType = nisType;
    mSceneHash = bStringHash(scene);

#ifndef EA_BUILD_A124
    if (bStrNICmp(scene, "IntroNisBL", 10) == 0 || bStrNICmp(scene, "IntroNisDD", 10) == 0 || bStrNICmp(scene, "IntroNis10", 10) == 0 ||
        bStrNICmp(scene, "EndingNIS", 9) == 0) {
        mNonSkipableNIS = true;
    }
#endif

    if (bStrNICmp(scene, "IntroNisDD", 10) == 0) {
        mDDayNIS = true;
    }
#ifndef EA_BUILD_A124
    if (bStrNICmp(scene, "IntroNisBL", 10) == 0) {
        mBlackListNIS = true;
    }
#endif

    if (bStrNICmp(scene, "IntroNisBL", 10) == 0 || bStrNICmp(scene, "IntroNis10", 10) == 0 || bStrNICmp(scene, "EndingNIS", 9) == 0) {
        mCareerNIS = true;
    }

    bool animationsDisabled = AnimCfg_DisableAnimations || Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;

    if (!animationsDisabled) {
        if (Tweak_EnableNISMomements || nisType != CAnimChooser::Moment || TheICEManager.IsEditorOn()) {
            int markerID = -1;
            float markerDist = 75.0f;
            TrackPositionMarker *marker =
                TheAnimCandidateData->GetClosestMarker(mSceneHash, mNISPosition, &markerID, &markerDist, bDegToAng(mNISDirection * 360.0f));
            if (marker) {
                NISActivity::StartLocationInRenderCoords(marker->Position, marker->Angle);
                if (!TheICEManager.IsEditorOn()) {
                    if (nisType == CAnimChooser::Arrest) {
                        float height = 0.0f;
                        UMath::Vector3 pt = {-marker->Position.y, marker->Position.z, marker->Position.x};
                        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(pt, height, nullptr)) {
                            marker->Position.z = height;
                            NISActivity::StartLocationInRenderCoords(marker->Position, marker->Angle);

                            NIS_NukeSmackablesWithinRange(mStartLocation, 10.0f);
                        }
                    }

                    bMatrix4 sceneRotation;
                    bMatrix4 sceneTranslation;
                    CAnimLocator::GetInitialAnimMatricies(&sceneRotation, &sceneTranslation, true);
                    bMatrix4 sceneTransform;
                    bMulMatrix(&sceneTransform, &sceneTranslation, &sceneRotation);
                    cameratrack = TheICEManager.ChooseGoodSceneCameraTrackIndex(bStringHash(scene), reinterpret_cast<const ICE::Matrix4 *>(&sceneTransform));

                    if (nisType == CAnimChooser::Moment) {
                        if (markerDist < 75.0f) {
                            new EBecomeAiCar();
                            mState = NISACTIVITY_PLAYING;
                            mMomentScene = new CAnimMomentScene(bStringHash(scene), cameratrack, sceneRotation, sceneTransform);
                        } else {
                            mState = NISACTIVITY_COMPLETE;
                        }
                        return;
                    }
                }
            }
        } else {
            mState = NISACTIVITY_COMPLETE;
            return;
        }
    } else {
        mState = NISACTIVITY_COMPLETE;
        return;
    }

    mStartPlayingNow = PlayAsSoonAsLoaded;
    if (TheICEManager.IsEditorOn()) {
        mRunningThroughICE = true;
    }

    TheTrackStreamer.EnableZoneSwitching();
    bStreamingPositionFromICE = 1;

    bool hasPreMovie = bStrLen(mPreMovie) > 0;

    if (scene) {
        PrepareVehicles(TheAnimCandidateData->GetSpecialCarList(mSceneHash));

        if ((mNISType == CAnimChooser::Arrest || mNISType == CAnimChooser::Ending) && !hasPreMovie) {
            mUsingFEngOverlay = true;

            new ESndGameState(11, true);
        }

        new EFadeScreenOn(false);

        mSequencerID = UCrc32(scene);
        mCameraTrackNumber = cameratrack;
    }

    if (hasPreMovie) {
        mUsingFEngOverlay = true;
        new EPlayRaceMovie(mPreMovie);
        mState = NISACTIVITY_PRE_MOVIE;
    } else {
        mState = NISACTIVITY_CREATING;
        UpdatePreloading();
    }

    if (FindSceneryGroup(bStringHash("SCENERY_GROUP_DOOR"))) {
        DisableSceneryGroup(bStringHash("SCENERY_GROUP_DOOR"));
    }
}

void NISActivity::Unload() {
    if (mSequencer) {
        mSequencer->Release();
        mSequencer = nullptr;
    }
    if (mAnimHandle != 0) {
        TheAnimPlayer.Stop(mAnimHandle, true);
        mAnimHandle = 0;
    }
    if (mSceneHash != 0) {
        TheAnimPlayer.Unload(mSceneHash);
    }
}

FECustomizationRecord *GetCustomCar(char *rideName) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    unsigned int car = FEHashUpper(rideName);

    RideInfo rideInfo;
    stable->BuildRideForPlayer(car, 0, &rideInfo);
    FECarRecord *carRecord = stable->GetCarRecordByHandle(car);
    FECustomizationRecord *customizationRecord = stable->GetCustomizationRecordByHandle(carRecord->Customization);

    return customizationRecord;
}

void NISActivity::PrepareVehicles(int carListType) {
    if (carListType == CSpecialCarListAnimCandidate::NONE) {
        return;
    }

    UMath::Vector3 startVec = {1.0f, 0.0f, 0.0f};
    UMath::Vector3 startPos = {0.0f, 0.0f, 0.0f};

    if (carListType == CSpecialCarListAnimCandidate::ONE_COP || carListType == CSpecialCarListAnimCandidate::LOTS_OF_COPS) {
        // TODO magic
        ISimable *iSim = ISimable::CreateInstance("PVehicle", VehicleParams(this, DRIVER_NIS, 0x9537acb7, startVec, startPos, VPF_SNAP_TO_GROUND, nullptr, nullptr));

        IVehicle *iCar;
        if (iSim && iSim->QueryInterface(&iCar)) {
            IDamageable *carDamage;

            INIS::Get()->AddCar(UCrc32("cop1"), iCar);

            if (iCar->QueryInterface(&carDamage)) {
                carDamage->ResetDamage();
            }
        }

        if (carListType == CSpecialCarListAnimCandidate::LOTS_OF_COPS) {
            for (int i = 1; i <= 7; i++) {
                // TODO magic
                ISimable *iSim = ISimable::CreateInstance("PVehicle", VehicleParams(this, DRIVER_NIS, 0x5d6e3e54, startVec, startPos, VPF_SNAP_TO_GROUND, nullptr, nullptr));

                IVehicle *iCar;
                if (iSim && iSim->QueryInterface(&iCar)) {
                    char channelName[32];
                    IDamageable *carDamage;

                    bSPrintf(channelName, "cop%d", i + 1);
                    INIS::Get()->AddCar(UCrc32(channelName), iCar);

                    if (iCar->QueryInterface(&carDamage)) {
                        carDamage->ResetDamage();
                    }
                }
            }
        }
    } else if (carListType == CSpecialCarListAnimCandidate::ONE_SPORT_COP) {
        // TODO magic
        ISimable *iSim = ISimable::CreateInstance(
            "PVehicle", VehicleParams(this, DRIVER_NIS, 0x7a49cccb, startVec, startPos, VPF_SNAP_TO_GROUND, GetCustomCar("COP_CROSS"), nullptr));

        IVehicle *iCar;
        if (iSim && iSim->QueryInterface(&iCar)) {
            IDamageable *carDamage;

            INIS::Get()->AddCar(UCrc32("cop1"), iCar);

            if (iCar->QueryInterface(&carDamage)) {
                carDamage->ResetDamage();
            }
        }
    } else if (carListType >= CSpecialCarListAnimCandidate::TRAFFIC_MIX1 && carListType <= CSpecialCarListAnimCandidate::TRAFFIC_MIX9) {
        unsigned int *specialCarList = SpecialCarList1;

        switch (carListType) {
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX2:
                specialCarList = SpecialCarList2;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX3:
                specialCarList = SpecialCarList3;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX4:
                specialCarList = SpecialCarList4;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX5:
                specialCarList = SpecialCarList5;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX6:
                specialCarList = SpecialCarList6;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX7:
                specialCarList = SpecialCarList7;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX8:
                specialCarList = SpecialCarList8;
                break;
            case CSpecialCarListAnimCandidate::TRAFFIC_MIX9:
                specialCarList = SpecialCarList9;
                break;
        }

        for (int i = 0; i <= 7; i++) {
            ISimable *iSim = ISimable::CreateInstance("PVehicle", VehicleParams(this, DRIVER_NIS, specialCarList[i], startVec, startPos, VPF_SNAP_TO_GROUND, nullptr, nullptr));

            IVehicle *iCar;
            if (iSim && iSim->QueryInterface(&iCar)) {
                char channelName[32];
                IDamageable *carDamage;

                bSPrintf(channelName, "cop%d", i + 1);
                INIS::Get()->AddCar(UCrc32(channelName), iCar);

                if (iCar->QueryInterface(&carDamage)) {
                    carDamage->ResetDamage();
                }
            }
        }
    }
}

bool NISActivity::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    CarList::iterator iter = mVehicleTable.find(UCrc32(system->ID()));

    if (iter != mVehicleTable.end()) {
        NISCar *car = (*iter).second;
        IVehicle *ivehicle = car->mIVehiclePtr;
        IContext *icontext;
        if (ivehicle && ivehicle->QueryInterface(&icontext)) {
            return icontext->SetDynamicData(system, data);
        }
    } else {
        // TODO magic
        if (system->ID() == 0x8da18577) {
            data->fhActivity = reinterpret_cast<uintptr_t>(GetInstanceHandle());
            return true;
        }
    }
    return false;
}

void NISActivity::UpdatePreloading() {
    if (mSceneHash != 0) {
        bool LockTrackStreamer = mNISType == CAnimChooser::Arrest;
        if (TheAnimPlayer.Load(mSceneHash, mCameraTrackNumber, LockTrackStreamer)) {
            g_pEAXSound->QueueNISStream(mSceneHash, mCameraTrackNumber, NISActivity::NISStreamTimeCallback);
            if (g_pNISRevMgr) {
                g_pNISRevMgr->OpenNISRevData(mSceneHash);
            }
            mState = NISACTIVITY_LOADING;
            return;
        }
    }
    mState = NISACTIVITY_COMPLETE;
}

void NISActivity::NISStreamTimeCallback(unsigned int animid, int mselapsed) {
    mElapsedmsAudioTime = mselapsed;
}

bool NISActivity::IsAudioStreamQueued() {
    return g_pEAXSound->IsNISStreamQueued();
}

static bool IsValidModelToNuke(const IModel *model) {
    if (!model) {
        return false;
    }
    if (UTL::COM::Is<ISceneryModel>(model) || UTL::COM::Is<IPlaceableScenery>(model)) {
        return true;
    }
    if (!model->IsRootModel()) {
        const IModel *root = model->GetRootModel();
        if (root) {
            if (UTL::COM::Is<ISceneryModel>(root)) {
                return true;
            }
            if (UTL::COM::Is<IVehicle>(root->GetSimable())) {
                return true;
            }
            if (UTL::COM::Is<IPlaceableScenery>(root)) {
                return true;
            }
        }
    }
    return false;
}

void NIS_NukeSmackablesWithinRange(const UMath::Vector3 &position, float radius) {
    float radiusSquared = radius * radius;
    for (IModel::List::const_iterator iter = IModel::GetList().begin(); iter != IModel::GetList().end(); iter++) {
        IModel *model = *iter;
        if (IsValidModelToNuke(model)) {
            UMath::Matrix4 transform;
            model->GetTransform(transform);

            float dist = UMath::DistanceSquare(position, UMath::Vector4To3(transform.v3));
            if (dist < radiusSquared) {
                model->HideModel();
                model->ReleaseModel();
            }
        }
    }
}

bool NISActivity::IsCarListLoaded() {
    if (TheCarLoader.IsLoadingInProgress()) {
        return false;
    }
    bool loaded = true;

    for (CarList::iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
        NISCar *car = (*i).second;
        if (car && car->mIVehiclePtr) {
            IRenderable *iRenderableCar;
            if (car->mIVehiclePtr->QueryInterface(&iRenderableCar) && !iRenderableCar->IsRenderable()) {
                loaded = false;
                break;
            }
        }
    }

    return loaded;
}

void NISActivity::UpdateLoading() {
    GetStartCameraLocation();

    mLoadAttemptCount++;
    if (TheAnimPlayer.IsLoaded(mSceneHash) && !TheTrackStreamer.IsLoadingInProgress() && !mPause) {
        if (mAnimScene == nullptr) {
            mAnimHandle = TheAnimPlayer.CreateAndPlayAnim(mSceneHash, mCameraTrackNumber, mNISType, 0);
            mAnimScene = TheAnimPlayer.FindAnimScene(mAnimHandle);
            mAnimScene->Pause();
        }
    }

    bool audioQueued = IsAudioStreamQueued();

    if (TheAnimPlayer.IsLoaded(mSceneHash) && IsCarListLoaded() && mAnimScene && audioQueued) {
        if (mNISType != CAnimChooser::Arrest || TheICEManager.IsEditorOn() || loadStartTime < RealTimer.GetSeconds() - 2.0f) {
            mState = NISACTIVITY_READY_TO_PLAY;
            if (mNISType != CAnimChooser::Moment) {
                if (mDDayNIS) {
                    SetSoundControlState(true, SNDSTATE_NIS_INTRO, "NIS Load DDay");
                } else if (mCareerNIS || mNISType == CAnimChooser::Intro) {
#ifndef EA_BUILD_A124
                    if (mBlackListNIS) {
                        SetSoundControlState(true, SNDSTATE_NIS_BLK, "NIS Play BLK");
                    } else if (mCareerNIS) {
                        SetSoundControlState(true, SNDSTATE_NIS_STORY, "NIS Play Story");
                    } else {
                        SetSoundControlState(true, SNDSTATE_NIS_INTRO, "NIS Play Intro");
                    }
#else
                    if (mCareerNIS) {
                        SetSoundControlState(true, SNDSTATE_NIS_STORY, "NIS Play Story");
                    } else {
                        SetSoundControlState(true, SNDSTATE_NIS_INTRO, "NIS Play Intro");
                    }
#endif
                } else if (mNISType == CAnimChooser::Arrest) {
                    SetSoundControlState(true, SNDSTATE_NIS_ARREST, "NIS Play Arrest");
                }
            }
        }

        if (mNISType == CAnimChooser::Arrest) {
            Speech::Manager::ClearPlayback();

            SoundAI *soundAI = SoundAI::Get();
            if (soundAI) {
                soundAI->GetDispatch()->ArrestReply();
            }
        }

        if (mNISType == CAnimChooser::Intro) {
            for (CarList::iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
                NISCar *car = (*i).second;
                if (car && car->mIVehiclePtr) {
                    IDamageable *iDamageable;
                    if (car->mIVehiclePtr->QueryInterface(&iDamageable)) {
                        iDamageable->ResetDamage();
                    }
                }
            }
        }
    }
}

void NISActivity::OnMovieComplete(const MNotifyMovieFinished &message) {
    if (mState == NISACTIVITY_PRE_MOVIE) {
        mState = NISACTIVITY_CREATING;
        UpdatePreloading();
    } else if (mState == NISACTIVITY_POST_MOVIE) {
        mState = NISACTIVITY_COMPLETE;
        Release();
    }
}

void NISActivity::Play() {
    EmitterSystem_OnStartNIS();

    for (int i = 0; i < 1; i++) {
        if (mSceneHash == bStringHash(NisNamesToDisablePreculler[i])) {
            DisablePreculler();
        }
    }

    mDefault_MaxTicksPerTimestep = MaxTicksPerTimestep;
    MaxTicksPerTimestep = 7.0f;

    bStreamingPositionFromICE = 0;
    mNISElapsedTime = 0.0f;

    if (mSequencer) {
        mSequencer->Release();
    }

    CloseTopologyAndSceneryGroups();

    mSequencer = EventSequencer::Create(this, this, mSequencerID, Sim::GetTime(), 1.0f);

    mState = NISACTIVITY_PLAYING;

    StartEvents();
    g_pEAXSound->PlayNIS();

    if (g_pNISRevMgr) {
        g_pNISRevMgr->StartNISReving();
    }

    mAnimScene->Play();

    // TODO magic
    if (mAnimScene->GetSceneType() == 2) {
        SetNISRaceDriverVisible(0);

        IVisualTreatment *visualTreatment = IVisualTreatment::Get();
        if (visualTreatment) {
            visualTreatment->SetState(IVisualTreatment::COPCAM_LOOK);
        }

        new ESndGameState(4, true);
    }

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player) {
        IFeedback *ffb = player->GetFFB();
        if (ffb) {
            ffb->PauseEffects();
        }
    }
}

void NISActivity::StartEvents() {
    if (mSequencer) {
        mSequencer->Reset(Sim::GetTime());
        // TODO magic
        mSequencer->ProcessStimulus(0xa6a56ea1, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
    }
}

void NISActivity::FireEventTag(const char *tagName) {
    if (mSequencer) {
        mSequencer->FireEventTag(stringhash(tagName), nullptr);
    }
}

void NISActivity::Pause() {
    mPause = true;
}

void NISActivity::UnPause() {
    mPause = false;
}

void NISActivity::UpdatePlaying(float dT) {
    if (mPause) {
        dT = 0.0f;
    }

    mNISElapsedTime += dT;

    if (FadeScreen::IsFadeScreenOn()) {
        // TODO magic
        new EFadeScreenOff(0x161a918);
        new ESndGameState(11, false);
    }

    if (mUsingFEngOverlay) {
        mUsingFEngOverlay = false;

        new ELoadingScreenOff();
    }

    if (mMomentScene) {
        if (!mMomentScene->IsFinished()) {
            mMomentScene->Update(dT);
        } else {
            mState = NISACTIVITY_COMPLETE;
            Release();
        }
    } else {
        CAnimScene *animScene = mAnimScene;
        if (animScene && (animScene->GetTimeElapsed() <= animScene->GetTimeTotalLength() || TheICEManager.IsEditorOn())) {
            if (!mAnimScene->IsPaused()) {
                mAnimScene->UpdateTime(dT);
                mAnimScene->AnimatedCars_Update(dT);
            } else if (mAnimScene->IsPaused()) {
                mAnimScene->AnimatedCars_Update(0.0f);
            }
        } else if (bStrLen(mPostMovie) > 0) {
            new EPlayRaceMovie(mPostMovie);
            mState = NISACTIVITY_POST_MOVIE;
        } else {
            mState = NISACTIVITY_COMPLETE;
            Release();
        }
    }

    if (!TheICEManager.IsEditorOn() && SkipNISs) {
        SkipOverNIS();
    }
}

void NISActivity::ResetEvents(float SetTime) {
    if (mSequencer) {
        mSequencer->SetVerbose(false);
        mSequencer->Reset(Sim::GetTime());
    }

    for (CarList::iterator i = mVehicleTable.begin(); i != mVehicleTable.end(); i++) {
        NISCar *car = (*i).second;
        if (car && car->mIVehiclePtr) {
            car->mIVehiclePtr->SetAnimating(true);

            INISCarControl *inisCarControl;
            if (car->mIVehiclePtr->QueryInterface(&inisCarControl)) {
                inisCarControl->RestoreState();
            }

            IDamageable *iDamageable;
            if (car->mIVehiclePtr->QueryInterface(&iDamageable)) {
                iDamageable->ResetDamage();
            }
        }
    }
}

void NISActivity::ServiceLoads() {
    switch (mState) {
        case NISACTIVITY_CREATING:
            UpdatePreloading();
            break;
        case NISACTIVITY_LOADING:
        case NISACTIVITY_READY_TO_PLAY:
            UpdateLoading();
            break;
        default:
            break;
    }
}

void NISActivity::Release() {
    IVisualTreatment *visualTreatment = IVisualTreatment::Get();
    if (visualTreatment) {
        visualTreatment->SetState(IVisualTreatment::HEAT_LOOK);
    }

    gTWEAKER_NISLightEnabled = false;

    ICE::HideOverlay();

    bool wasInterrupted = false;
    if (mNISType == CAnimChooser::Arrest && !mRunningThroughICE) {
        wasInterrupted = true;
    } else if (mNISType == CAnimChooser::Moment) {
        new EBecomePlayerCar();
    } else if (mNISType == CAnimChooser::Ending && !mRunningThroughICE) {
        new EFadeScreenOn(false);
    }

    FacePixelation::Disable();

    SetNISRaceDriverVisible(1);

    Tweak_DisableRoadNoise = 0;

    PrecipitationEnable = mIsPrecipitationEnable;

    SetOverRideRainIntensity(0.0f);

    if (mUsingFEngOverlay) {
        mUsingFEngOverlay = false;
        cFEng::Get()->QueuePackagePop(1);
    }

    MaxTicksPerTimestep = mDefault_MaxTicksPerTimestep;

    InitTopologyAndSceneryGroups();

    if (TheICEManager.IsEditorOn()) {
        delete this;
    } else {
        Sim::Activity::Release();
    }

    for (int i = 0; i < 1; i++) {
        if (mSceneHash == bStringHash(NisNamesToDisablePreculler[i])) {
            EnablePreculler();
        }
    }

    EmitterSystem_OnEndNis();

    if (g_pNISRevMgr) {
        g_pNISRevMgr->Start321Reving();
    }

    if (mNISType != CAnimChooser::Moment) {
        SoundPause(false, eSNDPAUSE_NISON);
    }

    if (wasInterrupted) {
        if (GRaceStatus::Get().GetRaceParameters() == nullptr) {
            new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "Infractions.fng");
        } else {
            new EPause(0, 1, 0);
        }
    }
}

void NISActivity::JoyHandle(IPlayer *player) {
    bool wheel_connected = false;
    if (player->GetSteeringDevice() && player->GetSteeringDevice()->IsConnected()) {
        wheel_connected = true;
    }
    mActionQ.SetPort(player->GetControllerPort());
    mActionQ.SetConfig(player->GetSettings()->GetControllerAttribs(CA_HUD, wheel_connected), "FEngHud");

    while (!mActionQ.IsEmpty()) {
        ActionRef aRef = mActionQ.GetAction();
        // TODO magic
        if (aRef.ID() == 50 && !TheOnlineManager.IsOnlineRace() && INIS::Get()) {
            INIS::Get()->SkipOverNIS();
        }

        mActionQ.PopAction();
    }
}

// TODO move
extern bool Tweak_FullSpeedMode;

bool NISActivity::OnTask(HSIMTASK task, float dT) {
    if (task != mUpdate) {
        Sim::Object::OnTask(task, dT);
    } else {
        JoyHandle(IPlayer::First(PLAYER_LOCAL));
        switch (mState) {
            case NISACTIVITY_READY_TO_PLAY:
                if (mStartPlayingNow && !mPause && mSuspensionSettle >= 31) {
                    Play();
                } else {
                    mSuspensionSettle++;
                    if (mAnimScene) {
                        mAnimScene->UpdateTime(0.0f);
                    }
                }
                break;
            case NISACTIVITY_PLAYING:
                UpdatePlaying(dT);
                break;
            case NISACTIVITY_COMPLETE:
                Tweak_FullSpeedMode = false;
                Release();
                break;
            default:
            case NISACTIVITY_CREATING:
            case NISACTIVITY_LOADING:
            case NISACTIVITY_PRE_MOVIE:
            case NISACTIVITY_POST_MOVIE:
                break;
        }
        return true;
    }
    return false;
}

bool NISActivity::SkipOverNIS() {
    if (!TheICEManager.IsEditorOn()) {
#ifndef EA_BUILD_A124
        if ((!mNonSkipableNIS || SkipMovies) && mState == NISACTIVITY_PLAYING && mNISType != CAnimChooser::Arrest &&
            mNISType != CAnimChooser::Ending) {
            if (mSkipToNIS[0] != 0) {
                if (mNISElapsedTime > mNISNoSkipTime) {
                    return false;
                }
                mNISSkipped = true;
            }
#else
        if (true) {
            // TODO PS2
#endif
            if (mSequencer) {
                mSequencer->Complete(Sim::GetTime(), true, nullptr);
            }
            mState = NISACTIVITY_COMPLETE;
            Speech::Manager::ClearPlayback();
            Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
            if (cop_speech) {
                cop_speech->ReleaseResource();
            }
            ICECompleteEventTags();
            if (mAnimScene) {
                mAnimScene->JumpToEnd();
            }
            new EFadeScreenOn(false);
            return true;
        }
    }
    return false;
}

BIND_ACTIVITY_FACTORY(NISActivity)
