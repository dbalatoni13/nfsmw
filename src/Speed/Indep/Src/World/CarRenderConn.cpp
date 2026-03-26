#include "Speed/Indep/Src/World/CarRenderConn.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

struct Car;
struct CarPartDatabase;
extern void *gINISInstance asm("_Q33UTL11Collectionst9Singleton1Z4INIS_mInstance");
extern unsigned int numCopsActiveView;
extern CarPartDatabase CarPartDB;
extern CarType GetCarType__15CarPartDatabaseUi(CarPartDatabase *database, unsigned int model_hash)
    asm("GetCarType__15CarPartDatabaseUi");
extern int GetAppliedAttributeIParam__7CarPartUii(const CarPart *part, unsigned int key, int default_value)
    asm("GetAppliedAttributeIParam__7CarPartUii");
extern int GetCarPartIDFromCrc(UCrc32 part_name);
extern void bRotateVector(bVector3 *dest, const bMatrix4 *m, bVector3 *v);
extern void MakeNoSkid__9SkidMaker(void *skid_maker) asm("MakeNoSkid__9SkidMaker");
extern void MakeSkid__9SkidMakerP3CarP8bVector3T2if(void *skid_maker, Car *car, bVector3 *skid_centre, bVector3 *skid_direction,
                                                    int terrain, float intensity)
    asm("MakeSkid__9SkidMakerP3CarP8bVector3T2if");
void DeleteAllSkids();
extern void TireState_ctor(TireState *state) asm("__9TireState");
extern void TireState_dtor(TireState *state, int in_chrg) asm("_._9TireState");
extern bTList<TireState> gTireStateList;
extern int PhysicsUpgrades_GetLevel(const Attrib::Gen::pvehicle &pvehicle, int type)
    asm("GetLevel__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type");
extern int PhysicsUpgrades_GetMaxLevel(const Attrib::Gen::pvehicle &pvehicle, int type)
    asm("GetMaxLevel__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type");
extern void VU0_Matrix4ToEuler(const UMath::Matrix4 &m, UMath::Vector3 &e)
    asm("VU0_Matrix4ToEuler__FRCQ25UMath7Matrix4RQ25UMath7Vector3");
extern float RealTimeElapsed;
extern float renderModifier;
extern int Tweak_DisableRoadNoise;
extern int NumTimesRenderTestPlayerCar;
extern RoadNoiseRecord Tweak_BlowOutNoise asm("Tweak_BlowOutNoise");
extern CameraAnchor *RVManchor;
extern void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const bMatrix4 *mat, const bVector4 *vel)
    asm("AddXenonEffect__FP12EmitterGroupPCQ26Attrib10CollectionPCQ25UMath7Matrix4PCQ25UMath7Vector4");
void NotifyTireStateEffectOfEmitterDelete(void *tire_state_effect, EmitterGroup *grp);

struct TireSkidMaker {
    TireSkidMaker(unsigned int value)
        : mValue(value) {}

    ~TireSkidMaker() {
        MakeNoSkid__9SkidMaker(this);
    }

    unsigned int mValue;
} __attribute__((packed));

struct TireState : public bTNode<TireState> {
    struct Effect {
        Effect()
            : mNeedsLazyInit(false), //
              mEmitterKey(0), //
              mGroup(0), //
              mMinVel(0.0f), //
              mMaxVel(0.0f), //
              mZeroParticleFrameCount(0) {}

        void ResetGroup() {
            mGroup = 0;
            mEmitterKey = 0;
            mMinVel = 0.0f;
            mNeedsLazyInit = true;
            mZeroParticleFrameCount = 0;
            mMaxVel = 0.0f;
        }

        void FreeUpFX();
        void LazyInit();
        void Set(const TireEffectRecord &record);
        void Update(float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT, const bVector4 &pos);

        bool mNeedsLazyInit;
        unsigned int mEmitterKey;
        EmitterGroup *mGroup;
        float mMinVel;
        float mMaxVel;
        int mZeroParticleFrameCount;
    };

    TireState();
    ~TireState();
    void KillSkids();
    void DoSkids(float intensity, const bVector3 *deltaPos, const bMatrix4 *tirematrix, const bMatrix4 *bodymatrix, float skidWidth);
    void DoFX(float slip, float skid, float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT);
    void SetSurface(const SimSurface &surface);
    void UpdateWorld(const WCollider *wc, bool rain, bool flat);

    static void *operator new(unsigned int size) {
        return gFastMem.Alloc(size, nullptr);
    }

    static void operator delete(void *mem, unsigned int size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    bVector4 mPrevTirePos;
    WWorldPos mWPos;
    TireSkidMaker mSkidMaker;
    bVector4 mTirePos;
    bVector4 mGroundPos;
    float mRoll;
    bool mRaining;
    bool mFlat;
    SimSurface mSurface;
    Effect mSlipFX;
    Effect mSkidFX;
    Effect mDriveFX;
};

static void StopEffect(VehicleRenderConn::Effect *effect) {
    effect->Stop();
}

static inline void EmitterGroupSetOldSurfaceEffectFlag(EmitterGroup *group) {
    *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(group) + 0x18) |= 0x80000;
}

static inline TireState *CreateTireState() {
    TireState *state = reinterpret_cast<TireState *>(gFastMem.Alloc(0xe0, 0));

    TireState_ctor(state);
    return state;
}

struct RenderPktCarOpen {
    void *vtable;
    unsigned int mModelHash;
};

struct TireStateRoadNoiseMirror {
    unsigned char _pad0[0x84];
    Attrib::Gen::simsurface mSurface;
};

static inline const Attrib::Collection *TireState_GetSurfaceCollection(const TireState *state) {
    return *reinterpret_cast<const Attrib::Collection * const *>(reinterpret_cast<const unsigned char *>(state) + 0x88);
}

struct CameraAnchorPovMirror {
    unsigned char _pad0[0xD8];
    short mPOVType;
};

struct LocalReferenceMirror {
    unsigned int mWorldID;
    const bMatrix4 *mMatrix;
    const bVector3 *mVelocity;
    const bVector3 *mAcceleration;
};

static inline float &CarRenderInfoF32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(info) + offset);
}

static inline unsigned int &CarRenderInfoU32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(info) + offset);
}

static inline int &CarRenderInfoI32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(info) + offset);
}

static inline short &CarRenderInfoS16(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(info) + offset);
}

static inline bool eIsGameViewID(int id) {
    return id - 1U < 3;
}

void NotifyTireStateEffectOfEmitterDelete(void *tire_state_effect, EmitterGroup *grp) {
    TireState::Effect *effect = static_cast<TireState::Effect *>(tire_state_effect);
    effect->ResetGroup();
}

void TireState::DoSkids(float intensity, const bVector3 *deltaPos, const bMatrix4 *tirematrix, const bMatrix4 *bodymatrix, float SkidWidth) {
    WWorldPos *world_pos = reinterpret_cast<WWorldPos *>(reinterpret_cast<unsigned char *>(this) + 0x18);
    void *skid_maker = reinterpret_cast<unsigned char *>(this) + 0x54;

    if (!world_pos->OnValidFace() || intensity <= 0.3f || SkidWidth <= 0.0f) {
        MakeNoSkid__9SkidMaker(skid_maker);
    } else {
        bMatrix4 world_matrix;
        bVector3 skid_direction(-deltaPos->y, deltaPos->x, 0.0f);
        bVector3 tire_direction;
        bVector3 up(0.0f, 1.0f, 0.0f);

        bMulMatrix(&world_matrix, bodymatrix, tirematrix);
        bNormalize(&skid_direction, &skid_direction);
        bRotateVector(&tire_direction, &world_matrix, &up);

        float skid_dot = bAbs(bDot(&tire_direction, &skid_direction));
        if (skid_dot < 0.5f) {
            skid_dot = 0.5f;
        }

        float half_skid_width = SkidWidth * skid_dot * 0.5f;
        bNormalize(&skid_direction, &skid_direction, half_skid_width);

        bVector3 tire_position(this->mTirePos.x, this->mTirePos.y, this->mTirePos.z);
        bVector3 right_point = tire_position + skid_direction;
        bVector3 left_point = tire_position - skid_direction;
        UMath::Vector3 right_point_l;
        UMath::Vector3 left_point_l;

        bConvertToBond(right_point_l, right_point);
        bConvertToBond(left_point_l, left_point);

        float right_elevation = world_pos->HeightAtPoint(right_point_l);
        float left_elevation = world_pos->HeightAtPoint(left_point_l);
        bVector3 skid_centre(tire_position.x, tire_position.y, (right_elevation + left_elevation) * 0.5f);

        skid_direction.z = right_elevation * 0.5f - left_elevation * 0.5f;
        bNormalize(&skid_direction, &skid_direction, half_skid_width);

        float skid_intensity_scale = skid_dot * intensity;
        if (skid_intensity_scale < 0.3f) {
            skid_intensity_scale = 0.3f;
        }

        MakeSkid__9SkidMakerP3CarP8bVector3T2if(skid_maker, 0, &skid_centre, &skid_direction, 1, skid_intensity_scale);
    }
}

void TireState::Effect::FreeUpFX() {
    if (this->mGroup != 0) {
        *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this->mGroup) + 0x18) |= 0x80000;
        this->mGroup->UnSubscribe();
    }

    EmitterGroup *group = 0;
    this->mGroup = group;
    this->mNeedsLazyInit = true;
    this->mZeroParticleFrameCount = 0;
}

void TireState::Effect::LazyInit() {
    if (this->mGroup != 0) {
        *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this->mGroup) + 0x18) |= 0x80000;
        this->mGroup->UnSubscribe();
    }

    this->mGroup = 0;
    if (this->mEmitterKey == 0 || this->mEmitterKey == 0xeec2271a) {
        return;
    }

    Attrib::Gen::emittergroup emitter_group_spec(this->mEmitterKey, 0, 0);
    if (!emitter_group_spec.IsValid()) {
        return;
    }

    this->mGroup = gEmitterSystem.CreateEmitterGroup(emitter_group_spec.GetConstCollection(), 0x40000000);
    if (this->mGroup != 0) {
        this->mGroup->Enable();
        this->mGroup->SubscribeToDeletion(this, NotifyTireStateEffectOfEmitterDelete);
    }

    this->mNeedsLazyInit = false;
    this->mZeroParticleFrameCount = 0;
}

void TireState::Effect::Set(const TireEffectRecord &record) {
    unsigned int emitter_key = record.EmitterClass;

    this->mMinVel = record.MinSpeed;
    this->mMaxVel = record.MaxSpeed;
    if (this->mEmitterKey != emitter_key) {
        this->mEmitterKey = emitter_key;
        this->mNeedsLazyInit = true;
        this->mZeroParticleFrameCount = 0;
    }
}

TireState::TireState()
    : mWPos(0.025f), //
      mSkidMaker(0) {
    this->mPrevTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->mTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->mGroundPos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->mRoll = 0.0f;
    this->mRaining = false;
    this->mFlat = false;
    this->SetSurface(SimSurface::kNull);
    gTireStateList.AddTail(reinterpret_cast<bNode *>(this));
}

TireState::~TireState() {
    this->KillSkids();
    this->Remove();

    if (this->mDriveFX.mGroup != 0) {
        this->mDriveFX.mGroup->UnSubscribe();
    }

    if (this->mSkidFX.mGroup != 0) {
        this->mSkidFX.mGroup->UnSubscribe();
    }

    if (this->mSlipFX.mGroup != 0) {
        this->mSlipFX.mGroup->UnSubscribe();
    }
}

void TireState::KillSkids() {
    MakeNoSkid__9SkidMaker(&this->mSkidMaker);
}

void KillSkidsOnRaceRestart() {
    for (TireState *state = gTireStateList.GetHead(); state != gTireStateList.EndOfList(); state = state->GetNext()) {
        state->KillSkids();
    }

    DeleteAllSkids();
}

void TireState::Effect::Update(float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT, const bVector4 &pos) {
    float intensity = 0.0f;
    float speed_range = this->mMaxVel - this->mMinVel;

    if (1e-6f < speed_range) {
        intensity = UMath::Ramp((speed - this->mMinVel) / speed_range, 0.0f, 1.0f);
    }

    if (0.0f < intensity) {
        if (this->mNeedsLazyInit) {
            this->LazyInit();
        }

        if (this->mGroup != 0) {
            bMatrix4 emitter_world = *car_matrix;

            emitter_world.v3 = pos;
            emitter_world.v3.w = 1.0f;
            this->mGroup->SetLocalWorld(&emitter_world);
            this->mGroup->SetInheritVelocity(car_velocity);
            this->mGroup->SetIntensity(intensity);
            this->mGroup->Update(dT);

            if (this->mGroup->GetNumParticles() == 0) {
                this->mZeroParticleFrameCount++;
            }

            if (this->mZeroParticleFrameCount < 11) {
                return;
            }
        } else {
            return;
        }
    } else if (this->mGroup == 0) {
        return;
    }

    EmitterGroupSetOldSurfaceEffectFlag(this->mGroup);
    this->mGroup->UnSubscribe();
    this->mZeroParticleFrameCount = 0;
    this->mNeedsLazyInit = true;
    this->mGroup = 0;
}

void TireState::SetSurface(const SimSurface &surface) {
    unsigned int slip_index;
    unsigned int slide_index;
    unsigned int drive_index;

    unsigned int slip_count = surface.Num_TireSlipEffects();
    unsigned int slide_count = surface.Num_TireSlideEffects();
    unsigned int drive_count = surface.Num_TireDriveEffects();

    if (!this->mFlat || slip_count < 3) {
        slip_index = 0;
        if (this->mRaining) {
            slip_index = slip_count > 1;
        }
    } else {
        slip_index = 2;
    }

    if (!this->mFlat || slide_count < 3) {
        slide_index = 0;
        if (this->mRaining) {
            slide_index = slide_count > 1;
        }
    } else {
        slide_index = 2;
    }

    if (!this->mFlat || drive_count < 3) {
        drive_index = 0;
        if (this->mRaining) {
            drive_index = drive_count > 1;
        }
    } else {
        drive_index = 2;
    }

    this->mSurface = surface;
    this->mSlipFX.Set(surface.TireSlipEffects(slip_index));
    this->mDriveFX.Set(surface.TireDriveEffects(drive_index));
    this->mSkidFX.Set(surface.TireSlideEffects(slide_index));
}

void TireState::UpdateWorld(const WCollider *wc, bool rain, bool flat) {
    UMath::Vector3 tire_pos;
    UMath::Vector3 ground_pos;

    tire_pos.x = -this->mTirePos.y;
    tire_pos.y = this->mTirePos.z;
    tire_pos.z = this->mTirePos.x;

    this->mWPos.FindClosestFace(wc, tire_pos, true);
    if (TireState_GetSurfaceCollection(this) != this->mWPos.GetSurface() || this->mRaining != rain || this->mFlat != flat) {
        this->mRaining = rain;
        this->mFlat = flat;

        SimSurface surface(this->mWPos.GetSurface());
        this->SetSurface(surface);
    }

    ground_pos = tire_pos;
    ground_pos.y = this->mWPos.HeightAtPoint(ground_pos);
    this->mGroundPos.y = -ground_pos.x;
    this->mGroundPos.x = ground_pos.z;
    this->mGroundPos.z = ground_pos.y;
}

void TireState::DoFX(float slip, float skid, float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT) {
    if (1.1920929e-07f < slip || slip < -1.1920929e-07f || 1.1920929e-07f < skid || skid < -1.1920929e-07f || 1.1920929e-07f < speed ||
        speed < -1.1920929e-07f) {
        goto update_fx;
    }

    this->mDriveFX.FreeUpFX();
    this->mSlipFX.FreeUpFX();
    this->mSkidFX.FreeUpFX();
    return;

update_fx:
    if (this->mWPos.OnValidFace()) {
        {
            SimSurface surface(this->mWPos.GetSurface());
            this->SetSurface(surface);
        }

        this->mSlipFX.Update(slip, car_velocity, car_matrix, dT, this->mGroundPos);
        this->mSkidFX.Update(skid, car_velocity, car_matrix, dT, this->mGroundPos);

        if (numCopsActiveView < 2) {
            bool inis_active = gINISInstance != 0;
            if (!inis_active) {
                this->mDriveFX.Update(speed, car_velocity, car_matrix, dT, this->mGroundPos);
            }
        }
    }
}

Sim::Connection *CarRenderConn::Construct(const Sim::ConnectionData &data) {
    RenderConn::Pkt_Car_Open *open = reinterpret_cast<RenderConn::Pkt_Car_Open *>(data.pkt);
    int car_type = GetCarType__15CarPartDatabaseUi(&CarPartDB, open->mModelHash);

    if (car_type == -1 || car_type > 0x53) {
        return 0;
    }

    return new CarRenderConn(data, static_cast<CarType>(car_type), open);
}

CarRenderConn::CarRenderConn(const Sim::ConnectionData &data, CarType ct, RenderConn::Pkt_Car_Open *oc)
    : VehicleRenderConn(data, ct), //
      IAttributeable(), //
      mAttributes(static_cast<const Attrib::Collection *>(0), 0, 0), //
      mPhysics(static_cast<const Attrib::Collection *>(0), 0, 0), //
      mTirePhysics(static_cast<const Attrib::Collection *>(0), 0, 0), //
      mExtraBodyAngle(UMath::Vector2::kZero), //
      mFlatTireAngle(UMath::Vector3::kZero), //
      mWheelHop(UMath::Vector3::kZero), //
      mRoadNoise(UMath::Vector3::kZero), //
      mEnginePower(0.0f), //
      mAnimTime(0.0f), //
      mShiftPitchAngle(0.0f), //
      mEngineTorqueAngle(0.0f), //
      mEngineVibrationAngle(0.0f), //
      mEnginePitchAngle(0.0f), //
      mPerfectLaunchTimer(0.0f), //
      mMaxWheelRenderDeltaAngle(0.0f), //
      mLastRenderFrame(0), //
      mLastVisibleFrame(0), //
      mDistanceToView(0.0f), //
      mFlashTime(0.0f), //
      mShifting(0.0f), //
      mDoContrailEffect(false), //
      mUsage(oc->mUsage), //
      mFlags(0) {
    unsigned int i;

    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&this->mRenderMatrix));
    {
        Attrib::Gen::pvehicle physics(oc->mPhysicsKey, 0, nullptr);
        this->mPhysics = physics;
    }
    {
        Attrib::Gen::tires tire_physics(this->mPhysics.tires(0), 0, nullptr);
        this->mTirePhysics = tire_physics;
    }
    this->mSteering[0] = 0.0f;
    this->mSteering[1] = 0.0f;

    this->mPartState.Clear();

    for (i = 0; i < 4; i++) {
        TireState *state = new TireState;
        this->mTireState[i] = state;
        this->VehicleRenderConn::mAttributes.TireOffsets(reinterpret_cast<UMath::Vector4 &>(this->mTirePositions[i]), i);
        {
            float tire_radius = UMath::Max(this->mTirePositions[i].w, 0.1f);
            this->mTireRadius[i] = tire_radius;
        }

        this->mTirePositions[i].w = 0.0f;
        this->mTireState[i]->mPrevTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
        {
            int is_front = i < 2;
            this->mPhysicsRadius[i] = Physics::Info::WheelDiameter(this->mTirePhysics, is_front) * 0.5f;
        }
    }

    this->mMaxWheelRenderDeltaAngle = 0.017453f;
    this->Load(oc->mWorldID, oc->mUsage, !oc->mSpoolLoad, oc->mCustomizations);
    this->SetFlag(CF_ISPLAYER, oc->mUsage == 0);

    if (this->mUsage == 0 || this->mUsage == 2) {
        int blowoff_level = PhysicsUpgrades_GetLevel(this->mPhysics, 4);
        int blowoff_max_level = PhysicsUpgrades_GetMaxLevel(this->mPhysics, 4);

        if (blowoff_level != 0 || blowoff_level == blowoff_max_level) {
            this->SetFlag(CF_BLOWOFF, true);
        }
    }
}

void CarRenderConn::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

CarRenderConn::~CarRenderConn() {
    while (!this->mPipeEffects.IsEmpty()) {
        VehicleRenderConn::Effect *effect = this->mPipeEffects.GetHead();

        this->mPipeEffects.RemoveHead();
        delete effect;
    }

    while (!this->mEngineEffects.IsEmpty()) {
        VehicleRenderConn::Effect *effect = this->mEngineEffects.GetHead();

        this->mEngineEffects.RemoveHead();
        delete effect;
    }

    for (int i = 0; i < 4; i++) {
        if (this->mTireState[i] != 0) {
            TireState_dtor(this->mTireState[i], 3);
            this->mTireState[i] = 0;
        }
    }
}

bool CarRenderConn::TestVisibility(float distance) {
    if (INIS::Get() != nullptr) {
        return true;
    }

    if (this->IsViewAnchor()) {
        return true;
    }

    bool visible = false;

    if (this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastVisibleFrame != 0) {
        visible = true;
    }

    if (!visible) {
        return false;
    }

    return this->mDistanceToView <= distance;
}

void RenderConn::Pkt_Car_Service::HidePart(const UCrc32 &partname) {
    unsigned int part_id = static_cast<unsigned int>(GetCarPartIDFromCrc(partname));

    if (part_id - 1 < 0x4B) {
        this->mPartState.Set(part_id);
    }
}

void CarRenderConn::OnEvent(EventID id) {
    switch (id) {
    case E_MISS_SHIFT:
        this->SetFlag(CF_MISSSHIFT, true);
        break;

    case E_DOWNSHIFT:
        this->mShifting = -1.0f;
        break;

    case E_UPSHIFT:
        this->mShifting = 1.0f;
        break;

    default:
        return;
    }
}

void CarRenderConn::UpdateSteering(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (!this->TestVisibility(renderModifier * 100.0f)) {
        this->mSteering[0] = 0.0f;
        this->mSteering[1] = 0.0f;
        return;
    }

    const float *max_steering = reinterpret_cast<const float *>(this->VehicleRenderConn::mAttributes.GetAttributePointer(0xa9633fde, 0));
    if (max_steering == 0) {
        max_steering = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    const float *steering_speed =
        reinterpret_cast<const float *>(this->VehicleRenderConn::mAttributes.GetAttributePointer(0x79356463, 0));
    if (steering_speed == 0) {
        steering_speed = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    float steering_limit = *max_steering * 0.017453f;
    float steering_delta = *steering_speed * 0.017453f * dT;

    for (int i = 0; i < 2; i++) {
        float target = data.mSteering[i];
        float current = this->mSteering[i];

        if (current < target - steering_delta) {
            current += steering_delta;
        } else if (target + steering_delta < current) {
            current -= steering_delta;
        } else {
            current = target;
        }

        if (steering_limit < current) {
            current = steering_limit;
        }
        if (current < -steering_limit) {
            current = -steering_limit;
        }

        this->mSteering[i] = current;
    }
}

void CarRenderConn::UpdateParts(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (this->mPartState != data.mPartState) {
        unsigned int i = 0;

        while (i < 0x4c) {
            bool hide = data.mPartState.Test(i);

            if (hide != this->mPartState.Test(i)) {
                if (hide) {
                    this->HidePart(static_cast<CAR_PART_ID>(i));
                } else {
                    this->ShowPart(static_cast<CAR_PART_ID>(i));
                }
            }

            i++;
        }
    }

    this->mPartState = data.mPartState;
}

void CarRenderConn::AddRoadNoise(float speed, unsigned int tires, const RoadNoiseRecord &noise) {
    if (noise.Frequency * noise.Amplitude * noise.MaxSpeed > 0.0f) {
        float fade = 0.0f;
        float speed_range = noise.MaxSpeed - noise.MinSpeed;

        if (1e-6f < speed_range) {
            fade = (speed - noise.MinSpeed) / speed_range;
            if (1.0f < fade) {
                fade = 1.0f;
            }
            if (fade < 0.0f) {
                fade = 0.0f;
            }
        }

        float scale = this->VehicleRenderConn::mAttributes.RoadNoise(0) * noise.Amplitude * 0.017453f * fade;
        float pitch = 0.0f;
        if ((tires & 0xf) != 0) {
            pitch = scale * sinf(this->mAnimTime * noise.Frequency * fade) * 0.5f;
            if ((tires & 3) == 0) {
                pitch = bAbs(pitch);
            }
            if ((tires & 0xc) == 0) {
                pitch = -bAbs(pitch);
            }
        }

        float roll = 0.0f;
        if ((tires & 0xf) != 0) {
            roll = scale * sinf((this->mAnimTime + 0.33f) * noise.Frequency * fade);
            if ((tires & 9) == 0) {
                roll = bAbs(roll);
            }
            if ((tires & 6) == 0) {
                roll = -bAbs(roll);
            }
        }

        this->mRoadNoise.y += pitch;
        this->mRoadNoise.x += roll;
    }
}

void CarRenderConn::UpdateRoadNoise(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data) {
    UMath::Clear(this->mRoadNoise);

    if (this->TestVisibility(renderModifier * 30.0f) && Tweak_DisableRoadNoise == 0) {
        RoadNoiseRecord roughness;

        for (unsigned int i = 0; i < 4; i++) {
            if (((data.mGroundState >> i) & 1U) != 0) {
                const RoadNoiseRecord &tirenoise =
                    reinterpret_cast<TireStateRoadNoiseMirror *>(this->mTireState[i])->mSurface.RenderNoise();

                if (tirenoise.Amplitude * tirenoise.Frequency > roughness.Amplitude * roughness.Frequency) {
                    roughness = tirenoise;
                }
            }
        }

        this->AddRoadNoise(carspeed, static_cast<unsigned int>(data.mGroundState), roughness);
        this->AddRoadNoise(carspeed, static_cast<unsigned int>(data.mGroundState & data.mBlowOuts), Tweak_BlowOutNoise);

        float siny = UMath::Sinr(this->mRoadNoise.y);
        float sinx = UMath::Sinr(this->mRoadNoise.x);

        this->mRoadNoise.z = UMath::Max(this->mTirePositions[0].x * siny, 0.0f);
        this->mRoadNoise.z = UMath::Max(this->mTirePositions[3].x * siny, this->mRoadNoise.z);
        this->mRoadNoise.z = UMath::Max(this->mTirePositions[0].y * sinx, this->mRoadNoise.z);
        this->mRoadNoise.z = UMath::Max(this->mTirePositions[1].y * sinx, this->mRoadNoise.z);
    }
}

void CarRenderConn::UpdateEngineAnimation(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (!this->TestVisibility(renderModifier * 30.0f)) {
        this->mEnginePitchAngle = 0.0f;
        this->mEnginePower = 0.0f;
        this->mEngineVibrationAngle = 0.0f;
        this->mEngineTorqueAngle = 0.0f;
        this->mShiftPitchAngle = 0.0f;
        this->mShifting = 0.0f;
        return;
    }

    float delta;
    float rev_accel;

    if (this->mShifting != 0.0f) {
        const float car_speed = bLength(this->GetVelocity());
        const float shift_speed = DEG2RAD(this->GetAttributes().ShiftSpeed(0));
        const float max_pitch = DEG2RAD(this->GetAttributes().ShiftAngle(0));
        const int gear = data.mGear - 2;

        if (shift_speed <= 0.0f || max_pitch <= 0.0f || gear < 0 || car_speed <= 10.0f) {
            this->mShiftPitchAngle = 0.0f;
            this->mShifting = 0.0f;
        } else {
            float fwd_accel = bDot(this->GetAcceleration(), reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v0));
            float accel_ratio = (UMath::Abs(fwd_accel * 0.10204081f) - 0.1f) / 0.4f;
            float gear_ratio = UMath::Ramp(accel_ratio, 0.0f, 1.0f);

            rev_accel = UMath::Pow(0.95f, static_cast<float>(gear));
            delta = UMath::Sina(UMath::Abs(this->mShifting) * 0.5f) * max_pitch * rev_accel * gear_ratio;

            this->mShiftPitchAngle = delta;
            if (this->mShifting < 0.0f) {
                this->mShifting = UMath::Min(this->mShifting + (dT * shift_speed) / max_pitch, 0.0f);
                this->mShiftPitchAngle *= 0.25f;
            } else if (0.0f < this->mShifting) {
                this->mShifting = UMath::Max(this->mShifting - (dT * shift_speed) / max_pitch, 0.0f);
            }
        }
    } else {
        this->mShiftPitchAngle = 0.0f;
    }

    delta = data.mEnginePower - this->mEnginePower;
    if (UMath::Abs(delta) < 0.005f) {
        delta = 0.0f;
    }

    this->mEnginePower = UMath::Clamp(this->mEnginePower + delta, 0.0f, 1.0f);
    this->mEnginePitchAngle = data.mAnimatedCarPitch;

    if (data.mAnimatedCarRoll == 0.0f) {
        float acceleration = (delta / dT) * this->GetAttributes().EngineRev(0);
        float max_rev = data.mEnginePower * data.mEngineSpeed * DEG2RAD(this->GetAttributes().EngineRevAngle(0));
        float rev_speed = DEG2RAD(this->GetAttributes().EngineRevSpeed(0)) * dT;
        float desired_angle = UMath::Ramp(acceleration, 0.0f, 0.2f) * max_rev;

        if (this->mEngineTorqueAngle < desired_angle) {
            this->mEngineTorqueAngle = UMath::Min(this->mEngineTorqueAngle + rev_speed, desired_angle);
        } else {
            this->mEngineTorqueAngle = UMath::Max(this->mEngineTorqueAngle - rev_speed, desired_angle);
        }

        this->mEngineTorqueAngle = UMath::Clamp(this->mEngineTorqueAngle, 0.0f, max_rev);
    } else {
        this->mEngineTorqueAngle = data.mAnimatedCarRoll;
    }

    if (data.mAnimatedCarShake == 0.0f) {
        float max_vibration = DEG2RAD(this->GetAttributes().EngineVibrationMax(0));
        float min_vibration = DEG2RAD(this->GetAttributes().EngineVibrationMin(0));
        float vibration_freq = this->GetAttributes().EngineVibrationFreq(0);

        this->mEngineVibrationAngle =
            data.mEngineSpeed * bSin(this->mAnimTime * vibration_freq * 6.2831855f) * (min_vibration + max_vibration * data.mEngineSpeed);
    } else {
        this->mEngineVibrationAngle = data.mAnimatedCarShake;
    }
}

void CarRenderConn::UpdateBodyAnimation(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (!this->TestVisibility(renderModifier * 80.0f)) {
        this->mExtraBodyAngle = UMath::Vector2::kZero;
        return;
    }

    const bVector3 *accel = this->GetAcceleration();
    float left_accel = bDot(accel, reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v1)) * 0.10204081f;
    float fwd_accel = bDot(accel, reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v0)) * 0.10204081f;
    const CarBodyMotion &pitch_control =
        fwd_accel < 0.0f ? this->GetAttributes().BodyDive() : this->GetAttributes().BodySquat();
    const CarBodyMotion &roll_control = this->GetAttributes().BodyRoll();
    float max_pitch = data.mExtraBodyPitch * DEG2RAD(pitch_control.DegPerG);
    float rate_pitch = DEG2RAD(pitch_control.DegPerSec) * dT;
    float max_roll = data.mExtraBodyRoll * DEG2RAD(roll_control.DegPerG);
    float rate_roll = DEG2RAD(roll_control.DegPerSec) * dT;

    if (UMath::Abs(left_accel) < 0.2f) {
        left_accel = 0.0f;
    }

    if (UMath::Abs(fwd_accel) < 0.2f) {
        fwd_accel = 0.0f;
    }

    float dest_angle_x = (max_roll + max_roll) * (UMath::Ramp(left_accel, -roll_control.MaxGs, roll_control.MaxGs) - 0.5f);
    float dest_angle_y =
        (max_pitch + max_pitch) * (UMath::Ramp(-fwd_accel * 0.10204081f, -pitch_control.MaxGs, pitch_control.MaxGs) - 0.5f);
    float speed = bLength(this->GetVelocity());

    if (speed < 1.0f) {
        dest_angle_x = 0.0f;
        dest_angle_y = 0.0f;
    }

    if (dest_angle_x > this->mExtraBodyAngle.x) {
        this->mExtraBodyAngle.x += rate_roll;
        this->mExtraBodyAngle.x = UMath::Min(this->mExtraBodyAngle.x, dest_angle_x);
    } else if (dest_angle_x < this->mExtraBodyAngle.x) {
        this->mExtraBodyAngle.x -= rate_roll;
        this->mExtraBodyAngle.x = UMath::Max(this->mExtraBodyAngle.x, dest_angle_x);
    }

    if (dest_angle_y > this->mExtraBodyAngle.y) {
        this->mExtraBodyAngle.y += rate_pitch;
        this->mExtraBodyAngle.y = UMath::Min(this->mExtraBodyAngle.y, dest_angle_y);
    } else if (dest_angle_y < this->mExtraBodyAngle.y) {
        this->mExtraBodyAngle.y -= rate_pitch;
        this->mExtraBodyAngle.y = UMath::Max(this->mExtraBodyAngle.y, dest_angle_y);
    }
}

void CarRenderConn::BuildRenderMatrix(float dT) {
    bVector4 offset(this->GetModelOffset());
    CarRenderInfo *carRenderInfo = this->GetRenderInfo();

    if (0.0f < dT) {
        UMath::Vector3 e0;
        UMath::Vector3 e1;
        UMath::Vector3 v;
        UMath::Vector3 v0;
        UMath::Vector3 v1;

        VU0_Matrix4ToEuler(reinterpret_cast<const UMath::Matrix4 &>(this->mRenderMatrix), e0);
        VU0_Matrix4ToEuler(reinterpret_cast<const UMath::Matrix4 &>(*this->GetBodyMatrix()), e1);
        UMath::Sub(e1, e0, v);
        UMath::Scale(v, (1.0f / dT) * 6.2831855f);
        CarRenderInfoF32(carRenderInfo, 0x14) = v.x;
        CarRenderInfoF32(carRenderInfo, 0x18) = v.y;
        CarRenderInfoF32(carRenderInfo, 0x1C) = v.z;

        v0.x = CarRenderInfoF32(carRenderInfo, 0x4);
        v0.y = CarRenderInfoF32(carRenderInfo, 0x8);
        v0.z = CarRenderInfoF32(carRenderInfo, 0xC);
        v1 = reinterpret_cast<const UMath::Vector3 &>(*this->GetVelocity());
        UMath::Sub(v1, v0, v);
        UMath::Scale(v, 1.0f / dT);
        CarRenderInfoF32(carRenderInfo, 0x24) = v.x;
        CarRenderInfoF32(carRenderInfo, 0x28) = v.y;
        CarRenderInfoF32(carRenderInfo, 0x2C) = v.z;
        CarRenderInfoF32(carRenderInfo, 0x4) = v1.x;
        CarRenderInfoF32(carRenderInfo, 0x8) = v1.y;
        CarRenderInfoF32(carRenderInfo, 0xC) = v1.z;
    }

    this->mRenderMatrix = *this->GetBodyMatrix();

    bVector4 rotOffset;
    eMulVector(&rotOffset, this->GetBodyMatrix(), &offset);
    this->mRenderMatrix.v3.x -= rotOffset.x;
    this->mRenderMatrix.v3.y -= rotOffset.y;
    this->mRenderMatrix.v3.z -= rotOffset.z;
}

void CarRenderConn::UpdateTires(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data) {
    float wheel_hop_roll = 0.0f;
    float wheel_hop_pitch = 0.0f;
    float tire_hop = 0.0f;
    bool flatten_tires = false;
    bool hop_wheels = false;
    bool is_view_anchor;
    bool can_do_fx;
    CarRenderInfo *car_render_info;

    this->mFlatTireAngle = UMath::Vector3::kZero;

    if (this->TestVisibility(renderModifier * 30.0f)) {
        flatten_tires = true;
        float hop_scale = this->GetAttributes().WheelHopScale(0);
        if (0.0f < data.mExtraBodyPitch && 0.0f < hop_scale) {
            hop_scale *= hop_scale;

            wheel_hop_roll = hop_scale * data.mExtraBodyPitch * DEG2RAD(0.15f) * UMath::Abs(bSin(this->mAnimTime + 37.699112f));
            wheel_hop_pitch = hop_scale * data.mExtraBodyPitch * DEG2RAD(0.2f) * UMath::Abs(bSin(this->mAnimTime * 150.79645f));
            tire_hop = hop_scale * data.mExtraBodyPitch * DEG2RAD(0.3f) * UMath::Abs(bSin((this->mAnimTime + 0.5f) * 150.79645f));
            hop_wheels = true;
        }
    }

    this->mWheelHop = UMath::Vector3::kZero;
    is_view_anchor = this->IsViewAnchor();
    can_do_fx = this->TestVisibility(renderModifier * 80.0f);
    car_render_info = this->GetRenderInfo();
    (void)is_view_anchor;

    for (int i = 0; i < 4; i++) {
        const int axle = i >> 1;
        const bool onground = ((data.mGroundState >> i) & 1U) != 0;
        const bool is_flat = ((data.mBlowOuts >> i) & 1U) != 0;
        TireState *state = this->mTireState[i];
        float compression = data.mCompressions[i] + (this->mTireRadius[i] - this->mPhysicsRadius[i]);
        float dW = UMath::Clamp((data.mWheelSpeed[i] / this->mTireRadius[i]) * dT, -this->mMaxWheelRenderDeltaAngle,
                                this->mMaxWheelRenderDeltaAngle);

        eIdentity(&this->mTireMatrices[i]);
        eIdentity(&this->mBrakeMatrices[i]);

        state->mRoll += dW;
        if (6.2831855f <= state->mRoll) {
            state->mRoll -= 6.2831855f;
        } else if (state->mRoll <= -6.2831855f) {
            state->mRoll += 6.2831855f;
        }

        eRotateY(&this->mTireMatrices[i], &this->mTireMatrices[i], static_cast<unsigned short>(state->mRoll * 10430.378f));
        if (i < 2) {
            eRotateZ(&this->mTireMatrices[i], &this->mTireMatrices[i], static_cast<unsigned short>(this->mSteering[i] * 10430.378f));
            eRotateZ(&this->mBrakeMatrices[i], &this->mBrakeMatrices[i], static_cast<unsigned short>(this->mSteering[i] * 10430.378f));
        }

        if (flatten_tires && is_flat) {
            compression += -0.12f;
            float x_angle = UMath::Atan2r(0.12f, UMath::Abs(this->mTirePositions[i].y)) * -3.1415927f;
            float y_angle = UMath::Atan2r(0.12f, UMath::Abs(this->mTirePositions[i].x)) * 3.1415927f;

            if (this->mTirePositions[i].y < 0.0f) {
                x_angle = -x_angle;
            }
            if (this->mTirePositions[i].x < 0.0f) {
                y_angle = -y_angle;
            }

            this->mFlatTireAngle.x += x_angle;
            this->mFlatTireAngle.y += y_angle;
            this->mFlatTireAngle.z += -0.03f;
        }

        if (i > 1 && hop_wheels && onground) {
            float hop_speed_scale;

            if (0.0f < data.mTireSlip[i]) {
                hop_speed_scale = (data.mTireSlip[i] - 1.0f) / 4.0f;
            } else {
                hop_speed_scale = (-data.mTireSlip[i] - 5.0f) / 15.0f;
            }

            hop_speed_scale = UMath::Ramp(hop_speed_scale, 0.0f, 1.0f);
            this->mWheelHop.y = wheel_hop_pitch * hop_speed_scale;
            this->mWheelHop.z =
                UMath::Max(this->mWheelHop.z, this->mTirePositions[0].x * UMath::Sinr(wheel_hop_pitch * hop_speed_scale));
            this->mWheelHop.x = wheel_hop_roll * hop_speed_scale;
            compression += bSin(tire_hop * hop_speed_scale) * (this->mTirePositions[0].x - this->mTirePositions[i].x);
        }

        this->mBrakeMatrices[i].v3.x = this->mTirePositions[i].x;
        this->mBrakeMatrices[i].v3.z += this->mTirePositions[i].z + compression;
        this->mBrakeMatrices[i].v3.y = this->mTirePositions[i].y;

        this->mTireMatrices[i].v3.x = this->mTirePositions[i].x;
        this->mTireMatrices[i].v3.z += this->mTirePositions[i].z + compression;
        this->mTireMatrices[i].v3.y = this->mTirePositions[i].y;

        if (can_do_fx) {
            car_render_info->SetWheelWobble(i, (data.mBlowOuts >> i) & 1U);
        }

        eMulVector(&state->mTirePos, &this->mRenderMatrix, &this->mTireMatrices[i].v3);
        state->UpdateWorld(this->mWCollider, this->GetFlag(CF_ISRAINING), is_flat);

        if (onground) {
            if (can_do_fx) {
                float skid = UMath::Max(UMath::Abs(data.mTireSkid[i] * 0.05f) - 0.1f, 0.0f);
                float slip = UMath::Max(UMath::Abs(data.mTireSlip[i] * 0.2f) - 0.1f, 0.0f);
                float intensity = UMath::Sqrt(skid * skid + slip * slip);

                if (0.0f < intensity) {
                    bVector4 delta_pos = state->mTirePos - state->mPrevTirePos;

                    state->DoSkids(intensity, reinterpret_cast<const bVector3 *>(&delta_pos), &this->mTireMatrices[i], &this->mRenderMatrix,
                                   this->GetAttributes().TireSkidWidth(i));
                } else {
                    state->KillSkids();
                }

                state->DoFX(data.mTireSlip[i] * this->GetAttributes().SlipFX(axle),
                            data.mTireSkid[i] * this->GetAttributes().SkidFX(axle), carspeed,
                            this->GetVelocity(), &this->mRenderMatrix, dT);
            } else {
                state->KillSkids();
            }
        } else {
            state->KillSkids();
        }

        state->mPrevTirePos = state->mTirePos;
    }
}

void CarRenderConn::UpdateRenderMatrix(float dT) {
    if (!this->TestVisibility(renderModifier * 80.0f)) {
        return;
    }

    bMatrix4 tire_matrices[4];
    bMatrix4 brake_matrices[4];
    for (int i = 0; i < 4; i++) {
        eMulMatrix(&tire_matrices[i], &this->mTireMatrices[i], &this->mRenderMatrix);
        eMulMatrix(&brake_matrices[i], &this->mBrakeMatrices[i], &this->mRenderMatrix);
    }

    float rotate_x =
        this->mExtraBodyAngle.x + this->mWheelHop.x + this->mFlatTireAngle.x + this->mRoadNoise.y + this->mEngineTorqueAngle + this->mEngineVibrationAngle;
    float rotate_y = this->GetAttributes().ExtraPitch(0) * 0.017453f + this->mExtraBodyAngle.y + this->mWheelHop.y + this->mFlatTireAngle.y +
                     this->mRoadNoise.x + this->mEnginePitchAngle + this->mShiftPitchAngle;
    float ride_height = this->GetAttributes().RideHeight() * 0.0254f + this->mWheelHop.z + this->mRoadNoise.z + this->mFlatTireAngle.z;

    bMatrix4 identity;
    bMatrix4 x_rotation;
    bMatrix4 y_rotation;
    bMatrix4 rotation;
    bMatrix4 old_render_matrix;
    bMatrix4 inverse_matrix;
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&identity));
    eRotateX(&x_rotation, &identity, static_cast<unsigned short>(rotate_x * 10430.378f));
    eRotateY(&y_rotation, &identity, static_cast<unsigned short>(rotate_y * 10430.378f));
    eMulMatrix(&rotation, &x_rotation, &y_rotation);

    PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&this->mRenderMatrix), *reinterpret_cast<Mtx44 *>(&old_render_matrix));
    eMulMatrix(&this->mRenderMatrix, &rotation, &old_render_matrix);

    this->mRenderMatrix.v3.x += this->mRenderMatrix.v2.x * ride_height;
    this->mRenderMatrix.v3.y += this->mRenderMatrix.v2.y * ride_height;
    this->mRenderMatrix.v3.z += this->mRenderMatrix.v2.z * ride_height;

    bInvertMatrix(&inverse_matrix, &this->mRenderMatrix);
    for (int i = 0; i < 4; i++) {
        eMulMatrix(&this->mTireMatrices[i], &tire_matrices[i], &inverse_matrix);
        eMulMatrix(&this->mBrakeMatrices[i], &brake_matrices[i], &inverse_matrix);
    }

    float wheel_well = this->GetAttributes().WheelWell(0) * 0.0254f;
    if (0.0f < wheel_well) {
        float max_wheel = this->mTireMatrices[0].v3.y + this->mTireRadius[0];
        for (int i = 1; i < 4; i++) {
            float wheel_height = this->mTireMatrices[i].v3.y + this->mTireRadius[i];
            if (max_wheel < wheel_height) {
                max_wheel = wheel_height;
            }
        }

        if (wheel_well < max_wheel) {
            float delta = max_wheel - wheel_well;
            this->mRenderMatrix.v3.x += this->mRenderMatrix.v2.x * delta;
            this->mRenderMatrix.v3.y += this->mRenderMatrix.v2.y * delta;
            this->mRenderMatrix.v3.z += this->mRenderMatrix.v2.z * delta;
            for (int i = 0; i < 4; i++) {
                this->mTireMatrices[i].v3.y -= delta;
                this->mBrakeMatrices[i].v3.y -= delta;
            }
        }
    }

    (void)dT;
}

void CarRenderConn::Update(const RenderConn::Pkt_Car_Service &data, float dT) {
    if (this->CanUpdate()) {
        CarRenderInfo *render_info = this->mRenderInfo;

        if (render_info == 0) {
            return;
        }

        render_info->SetDamageInfo(*reinterpret_cast<const DamageZone::Info *>(&data.mDamageInfo));
        CarRenderInfoF32(render_info, 0x1754) = dT;
        CarRenderInfoU32(render_info, 0x1608) = data.mLights;
        CarRenderInfoU32(render_info, 0x160C) = data.mBrokenLights;
        int flashing = data.mFlashing;

        CarRenderInfoI32(render_info, 0x1770) = flashing;
        if (flashing != 0) {
            CarRenderInfoF32(render_info, 0x1774) += RealTimeElapsed;
            if (0.05f < CarRenderInfoF32(render_info, 0x1774)) {
                CarRenderInfoF32(render_info, 0x1774) -= 0.12f;
            }
        }

        CarRenderInfoU32(render_info, 0x1170) = data.mNos;
        CarRenderInfoS16(render_info, 0x1174) = static_cast<short>(this->mSteering[0] * 10430.378f);
        CarRenderInfoS16(render_info, 0x1176) = static_cast<short>(this->mSteering[1] * 10430.378f);
        float carspeed = bLength(this->GetVelocity());

        this->mAnimTime += dT;
        if (10.0f <= this->mAnimTime) {
            this->mAnimTime -= 10.0f;
        }

        CarRenderInfoF32(render_info, 0x0) = this->mAnimTime;
        this->UpdateParts(dT, data);
        this->BuildRenderMatrix(dT);
        this->SetFlag(CF_ISRAINING, this->CheckForRain());
        this->UpdateSteering(dT, data);
        this->UpdateTires(dT, carspeed, data);
        this->UpdateRoadNoise(dT, carspeed, data);
        this->UpdateBodyAnimation(dT, data);
        this->UpdateEngineAnimation(dT, data);
        if (this->GetFlag(CF_ISPLAYER)) {
            this->UpdateContrails(data, dT);
        }
        this->UpdateRenderMatrix(dT);
        this->UpdateEffects(data, dT);
        this->VehicleRenderConn::Update(dT);
    }
}

void CarRenderConn::UpdateContrails(const RenderConn::Pkt_Car_Service &data, float dT) {
    float carspeed = bLength(this->GetVelocity());

    if (data.mNos || 44.0f <= carspeed) {
        if (!INIS::Exists()) {
            this->mDoContrailEffect = true;
            return;
        }
    }

    this->mDoContrailEffect = false;
}

void CarRenderConn::UpdateEffects(const RenderConn::Pkt_Car_Service &data, float dT) {
    if (!this->TestVisibility(renderModifier * 80.0f)) {
        void (*stop_effect)(VehicleRenderConn::Effect *) = StopEffect;
        VehicleRenderConn::Effect *pipe_effect = this->mPipeEffects.GetHead();
        VehicleRenderConn::Effect *pipe_effect_end = this->mPipeEffects.EndOfList();

        for (;;) {
            if (pipe_effect == pipe_effect_end) {
                break;
            }
            stop_effect(pipe_effect);
            pipe_effect = pipe_effect->GetNext();
        }

        VehicleRenderConn::Effect *engine_effect = this->mEngineEffects.GetHead();
        VehicleRenderConn::Effect *engine_effect_end = this->mEngineEffects.EndOfList();
        for (;;) {
            if (engine_effect == engine_effect_end) {
                break;
            }
            stop_effect(engine_effect);
            engine_effect = engine_effect->GetNext();
        }
        return;
    }

    const Attrib::Gen::ecar &attributes = this->VehicleRenderConn::mAttributes;
    const LocalReferenceMirror *world_ref = reinterpret_cast<const LocalReferenceMirror *>(&this->mWorldRef);
    const bVector3 *velocity = world_ref->mVelocity;
    unsigned int damage_key = attributes.DamageEffect(0).GetCollectionKey();
    unsigned int death_key = attributes.DeathEffect(0).GetCollectionKey();
    unsigned int engine_key = attributes.EngineBlownEffect(0).GetCollectionKey();
    unsigned int missshift_key = attributes.MissShiftEffect(0).GetCollectionKey();
    unsigned int nos_key = attributes.NOSEffect(0).GetCollectionKey();

    for (VehicleRenderConn::Effect *pipe_effect = this->mPipeEffects.GetHead(); pipe_effect != this->mPipeEffects.EndOfList();
         pipe_effect = pipe_effect->GetNext()) {
        if (!data.mNos) {
            if (this->GetFlag(CF_MISSSHIFT)) {
                pipe_effect->Fire(&this->mRenderMatrix, missshift_key, 1.0f, velocity);
            } else if (this->GetFlag(CF_BLOWOFF) && this->mShifting != 0.0f) {
                pipe_effect->Update(&this->mRenderMatrix, nos_key, dT, 1.0f, velocity);
            } else {
                pipe_effect->Stop();
            }
        } else {
            pipe_effect->Update(&this->mRenderMatrix, nos_key, dT, 1.0f, velocity);
        }
    }

    for (VehicleRenderConn::Effect *engine_effect = this->mEngineEffects.GetHead(); engine_effect != this->mEngineEffects.EndOfList();
         engine_effect = engine_effect->GetNext()) {
        if (death_key != 0 && data.mHealth <= 0.0f) {
            engine_effect->Update(&this->mRenderMatrix, death_key, dT, 1.0f, velocity);
        } else if (damage_key != 0 && data.mHealth <= 1.0f) {
            engine_effect->Update(&this->mRenderMatrix, damage_key, dT, 1.0f, velocity);
        } else if (data.mEngineBlown) {
            engine_effect->Update(&this->mRenderMatrix, engine_key, dT, 1.0f, velocity);
        } else {
            engine_effect->Stop();
        }
    }

    this->SetFlag(CF_MISSSHIFT, false);
}

void CarRenderConn::Hide(bool b) {
    if (this->GetFlag(CF_HIDDEN) != b) {
        this->SetFlag(CF_HIDDEN, b);
        if (b) {
            this->mAnimTime = 0.0f;
            for (int i = 0; i < 4; i++) {
                this->mTireState[i]->KillSkids();
            }

            this->mHide = true;

            for (VehicleRenderConn::Effect *effect = this->mEngineEffects.GetHead(); effect != this->mEngineEffects.EndOfList();
                 effect = effect->GetNext()) {
                StopEffect(effect);
            }

            for (VehicleRenderConn::Effect *effect = this->mPipeEffects.GetHead(); effect != this->mPipeEffects.EndOfList();
                 effect = effect->GetNext()) {
                StopEffect(effect);
            }
        }
    }
}

void CarRenderConn::OnFetch(float dT) {
    bool in_view = false;

    if ((this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastVisibleFrame != 0) || this->IsViewAnchor()) {
        in_view = true;
    }

    RenderConn::Pkt_Car_Service pkt(in_view, this->mDistanceToView);
    if (this->Service(&pkt)) {
        this->Hide(false);
        this->Update(pkt, dT);
    } else {
        this->Hide(true);
    }
}

void CarRenderConn::OnLoaded(CarRenderInfo *carrender_info) {
    this->VehicleRenderConn::OnLoaded(carrender_info);
    if (carrender_info == 0) {
        return;
    }

    if (carrender_info->pRideInfo) {
        CarPart *part_rim = carrender_info->pRideInfo->GetPart(0x42);
        if (part_rim) {
            unsigned int numSpokes = static_cast<signed char>(
                bAbs(GetAppliedAttributeIParam__7CarPartUii(part_rim, 0x1b0ea1a9, 0)));

            if (numSpokes == 0) {
                numSpokes = bAbs(this->VehicleRenderConn::mAttributes.WheelSpokeCount());
            }

            if (numSpokes != 0) {
                float spoke_count = static_cast<float>(numSpokes);
                spoke_count += spoke_count;
                this->mMaxWheelRenderDeltaAngle = DEG2RAD(360.0f / spoke_count - 1.0f);
            }
        }
    }

    carrender_info->InitEmitterPositions(this->mTirePositions);

    if (this->mPipeEffects.IsEmpty()) {
        for (CarEmitterPosition *emitter_position = carrender_info->EmitterPositionList[10].GetHead();
             emitter_position != carrender_info->EmitterPositionList[10].EndOfList(); emitter_position = emitter_position->GetNext()) {
            ePositionMarker *position_marker = emitter_position->PositionMarker;
            if (position_marker) {
                this->mPipeEffects.AddTail(new VehicleRenderConn::Effect(&position_marker->Matrix));
            } else {
                bMatrix4 tempmat;
                bIdentity(&tempmat);
                tempmat.v3.x = emitter_position->X;
                tempmat.v3.y = emitter_position->Y;
                tempmat.v3.z = emitter_position->Z;
                this->mPipeEffects.AddTail(new VehicleRenderConn::Effect(&tempmat));
            }
        }
    }

    if (this->mEngineEffects.IsEmpty()) {
        for (CarEmitterPosition *emitter_position = carrender_info->EmitterPositionList[9].GetHead();
             emitter_position != carrender_info->EmitterPositionList[9].EndOfList(); emitter_position = emitter_position->GetNext()) {
            ePositionMarker *position_marker = emitter_position->PositionMarker;
            if (position_marker) {
                this->mEngineEffects.AddTail(new VehicleRenderConn::Effect(&position_marker->Matrix));
            } else {
                bMatrix4 tempmat;
                bIdentity(&tempmat);
                tempmat.v3.x = emitter_position->X;
                tempmat.v3.y = emitter_position->Y;
                tempmat.v3.z = emitter_position->Z;
                this->mEngineEffects.AddTail(new VehicleRenderConn::Effect(&tempmat));
            }
        }
    }
}

void CarRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&this->mRenderMatrix), *reinterpret_cast<Mtx44 *>(matrix));
}

void CarRenderConn::OnRender(eView *view, int reflection) {
    if (!this->CanRender()) {
        return;
    }

    if (this->mLastRenderFrame != eGetFrameCounter()) {
        this->mDistanceToView = 1000000.0f;
    }
    this->mLastRenderFrame = eGetFrameCounter();

    CameraMover *camera_mover = view->GetCameraMover();

    if (camera_mover != 0 && !camera_mover->RenderCarPOV()) {
        CameraAnchor *anchor = camera_mover->GetAnchor();

        if (anchor != 0 && anchor->GetWorldID() == this->GetWorldID()) {
            return;
        }
    }

    if (view->GetID() > 0xF && view->GetID() < 0x16) {
        CameraMover *rear_view_mover = eGetView(1, false)->GetCameraMover();

        if (rear_view_mover != 0) {
            CameraAnchor *anchor = rear_view_mover->GetAnchor();

            if (anchor != 0 && anchor->GetWorldID() == this->GetWorldID()) {
                return;
            }
        }
    }

    if (this->mDoContrailEffect && camera_mover != 0 && camera_mover->OutsidePOV() &&
        (view->GetID() == 1 || view->GetID() == 2)) {
        const Attrib::Collection *xenon_effect = Attrib::FindCollection(0x6F5943F1, 0x16AFDE7B);
        AddXenonEffect(0, xenon_effect, this->GetBodyMatrix(), reinterpret_cast<const bVector4 *>(this->GetVelocity()));
    }

    if (view->GetID() == 3 && camera_mover != 0) {
        RVManchor = camera_mover->GetAnchor();

        if (RVManchor != 0 && RVManchor->GetWorldID() == this->GetWorldID()) {
            return;
        }
    }

    if (camera_mover != 0 && eIsGameViewID(view->GetID())) {
        float distance = camera_mover->GetDistanceTo(reinterpret_cast<bVector3 *>(&this->mRenderMatrix.v3));
        this->mDistanceToView = UMath::Min(this->mDistanceToView, distance);
    }

    CarRenderInfo *render_info = this->mRenderInfo;
    if (render_info == 0) {
        return;
    }

    eGetCurrentViewMode();

    bMatrix4 body_matrix = this->mRenderMatrix;

    if (reflection == 0 && this->IsViewAnchor(view)) {
        CameraMover *anchor_mover = view->GetCameraMover();
        CameraAnchor *anchor = anchor_mover->GetAnchor();

        if (reinterpret_cast<const CameraAnchorPovMirror *>(anchor)->mPOVType == 1) {
            bVector4 translated_offset;

            PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(this->GetBodyMatrix()), *reinterpret_cast<Mtx44 *>(&body_matrix));
            bVector4 offset = this->mModelOffset;
            eMulVector(&translated_offset, &body_matrix, &offset);
            body_matrix.v3.x -= translated_offset.x;
            body_matrix.v3.y -= translated_offset.y;
            body_matrix.v3.z -= translated_offset.z;
        }
    }

    unsigned int extra_render_flags = 0;
    if (reflection != 0) {
        extra_render_flags = 0x401;
        body_matrix.v2.x = -body_matrix.v2.x;
        body_matrix.v2.y = -body_matrix.v2.y;
        body_matrix.v2.z = -body_matrix.v2.z;
    }

    bVector3 world_position;
    world_position.x = body_matrix.v3.x;
    world_position.y = body_matrix.v3.y;
    world_position.z = body_matrix.v3.z;
    body_matrix.v3.x = 0.0f;
    body_matrix.v3.y = 0.0f;
    body_matrix.v3.z = 0.0f;

    bool is_player = true;
    if (!(this->mFlags & CF_ISPLAYER)) {
        is_player = false;
    }

    if (is_player) {
        if (NumTimesRenderTestPlayerCar != 0) {
            for (int i = 0; i < NumTimesRenderTestPlayerCar; i++) {
                CARPART_LOD min_lod = render_info->mMinLodLevel;

                if (render_info->Render(view, &world_position, &body_matrix, this->mTireMatrices, this->mBrakeMatrices, this->mTireMatrices,
                                        extra_render_flags, 0, reflection, static_cast<float>(static_cast<int>(min_lod)), min_lod,
                                        static_cast<CARPART_LOD>(0)) &&
                    view->GetID() < 4) {
                    this->mLastVisibleFrame = eGetFrameCounter();
                }
            }

            goto render_done;
        }
    }

    {
        CARPART_LOD min_lod = render_info->mMinLodLevel;

        if (render_info->Render(view, &world_position, &body_matrix, this->mTireMatrices, this->mBrakeMatrices, this->mTireMatrices,
                                extra_render_flags, 0, reflection, 1.0f, min_lod, min_lod) &&
            view->GetID() < 4) {
            this->mLastVisibleFrame = eFrameCounter;
        }
    }

render_done:
    ;
}
