#include "Speed/Indep/Src/Animation/AnimChooser.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x8
class NISCar {
  public:
    NISCar(UCrc32 channel, IVehicle *vehicle);
    ~NISCar();

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

// total size: 0x438
struct NISActivity : public Sim::Activity, public INIS, public EventSequencer::IContext, public IVehicleCache {
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

    NISActivity();
    void OnMovieComplete(const MNotifyMovieFinished &message);

  private:
    HSIMTASK mUpdate;                                         // offset 0x6C, size 0x4
    float mNISElapsedTime;                                    // offset 0x70, size 0x4
    ActionQueue mActionQ;                                     // offset 0x74, size 0x294
    NISACTIVITY_STATE mState;                                 // offset 0x308, size 0x4
    Hermes::HHANDLER mMsgMovieComplete;                       // offset 0x30C, size 0x4
    CAnimChooser::eType mNISType;                             // offset 0x310, size 0x4
    bVector3 mNISPosition;                                    // offset 0x314, size 0x10
    float mNISDirection;                                      // offset 0x324, size 0x4
    char mPreMovie[64];                                       // offset 0x328, size 0x40
    char mPostMovie[64];                                      // offset 0x368, size 0x40
    unsigned int mSceneHash;                                  // offset 0x3A8, size 0x4
    UCrc32 mSequencerID;                                      // offset 0x3AC, size 0x4
    CAnimScene *mAnimScene;                                   // offset 0x3B0, size 0x4
    int mAnimHandle;                                          // offset 0x3B4, size 0x4
    int mCameraTrackNumber;                                   // offset 0x3B8, size 0x4
    float mDefault_MaxTicksPerTimestep;                       // offset 0x3BC, size 0x4
    UTL::Std::map<UCrc32, NISCar *, _type_map> mVehicleTable; // offset 0x3C0, size 0x10
    EventSequencer::IEngine *mSequencer;                      // offset 0x3D0, size 0x4
    UMath::Vector3 mStartLocation;                            // offset 0x3D4, size 0xC
    UMath::Vector3 mStartCameraLocation;                      // offset 0x3E0, size 0xC
    bool mStartPlayingNow;                                    // offset 0x3EC, size 0x1
    bool mRunningThroughICE;                                  // offset 0x3F0, size 0x1
    int mLoadAttemptCount;                                    // offset 0x3F4, size 0x4
    int mSuspensionSettle;                                    // offset 0x3F8, size 0x4
    struct CAnimMomentScene *mMomentScene;                    // offset 0x3FC, size 0x4
    bool mUsingFEngOverlay;                                   // offset 0x400, size 0x1
    bool mCareerNIS;                                          // offset 0x404, size 0x1
    bool mDDayNIS;                                            // offset 0x408, size 0x1
    bool mBlackListNIS;                                       // offset 0x40C, size 0x1
    bool mNonSkipableNIS;                                     // offset 0x410, size 0x1
    char mSkipToNIS[16];                                      // offset 0x414, size 0x10
    bool mNISSkipped;                                         // offset 0x424, size 0x1
    float mNISNoSkipTime;                                     // offset 0x428, size 0x4
    int mIsPrecipitationEnable;                               // offset 0x42C, size 0x4
    bool mPause;                                              // offset 0x430, size 0x1
    float loadStartTime;                                      // offset 0x434, size 0x4
};

extern int PrecipitationEnable;

// UNSOLVED TODO Hermess::Handler stuff
NISActivity::NISActivity()
    : Sim::Activity(3),               //
      INIS(this),                     //
      EventSequencer::IContext(this), //
      IVehicleCache(this),            //
      mNISElapsedTime(0.0f),          //
      mActionQ(true),                 //
      mState(NISACTIVITY_NONE),       //
      mMsgMovieComplete(Hermes::Handler::Create<MNotifyMovieFinished, NISActivity, NISActivity>(this, &NISActivity::OnMovieComplete,
                                                                                                UCrc32(0x20d60dbf), 0)), // TODO magic
      mNISType(CAnimChooser::Intro),                                                                                     //
      mSceneHash(0),                                                                                                     //
      mSequencerID(),                                                                                                    //
      mAnimScene(nullptr),                                                                                               //
      mAnimHandle(0),                                                                                                    //
      mCameraTrackNumber(0),                                                                                             //
      mDefault_MaxTicksPerTimestep(4.0f),                                                                                //
      mSequencer(nullptr),                                                                                               //
      mRunningThroughICE(false),                                                                                         //
      mLoadAttemptCount(0),                                                                                              //
      mSuspensionSettle(0),                                                                                              //
      mMomentScene(nullptr),                                                                                             //
      mUsingFEngOverlay(false),                                                                                          //
      mCareerNIS(false),                                                                                                 //
      mDDayNIS(false),                                                                                                   //
      mBlackListNIS(false),                                                                                              //
      mNonSkipableNIS(false),                                                                                            //
      mNISSkipped(false),                                                                                                //
      mNISNoSkipTime(0.0f),                                                                                              //
      mPause(false),                                                                                                     //
      loadStartTime(0.0f),                                                                                               //
      mStartPlayingNow(true) {
    mPreMovie[0] = '\0';
    mPostMovie[0] = '\0';
    mSkipToNIS[0] = '\0';

    mUpdate = AddTask(UCrc32("WorldUpdate"), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mUpdate, "NIS");
    mIsPrecipitationEnable = PrecipitationEnable;
}
