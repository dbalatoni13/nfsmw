#include "Speed/Indep/Src/World/CarRenderConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/Src/World/Skids.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include "Speed/GameCube/Src/xSparks.h"
#else
// TODO
#include "Speed/PSX2/Src/xSparks.h"
#endif

// TODO these are in Render.hpp in carbon, do we want that to exist here?
extern unsigned int numCopsActiveView;
extern float renderModifier;

static const float Tweak_MaxCarAnimTime = 10.0f;

struct TireState;

bTList<TireState> gTireStateList;

struct TireState : public bTNode<TireState> {
    struct Effect {
        Effect()
            : mNeedsLazyInit(false), //
              mEmitterKey(0),        //
              mGroup(nullptr),       //
              mMinVel(0.0f),         //
              mMaxVel(0.0f)          //
#ifndef EA_BUILD_A124
              ,
              mZeroParticleFrameCount(0)
#endif
        {
        }

        void ResetGroup() {
            mGroup = nullptr;
            mEmitterKey = 0;
            mMinVel = 0.0f;
            mNeedsLazyInit = true;
#ifndef EA_BUILD_A124
            mZeroParticleFrameCount = 0;
#endif
            mMaxVel = 0.0f;
        }

#ifndef EA_BUILD_A124
        void FreeUpFX();
#endif
        void LazyInit();
        void Set(const TireEffectRecord &record);
        void Update(float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT, const bVector4 &pos);

        bool mNeedsLazyInit;     // offset 0x0, size 0x1
        Attrib::Key mEmitterKey; // offset 0x4, size 0x4
        EmitterGroup *mGroup;    // offset 0x8, size 0x4
        float mMinVel;           // offset 0xC, size 0x4
        float mMaxVel;           // offset 0x10, size 0x4
#ifndef EA_BUILD_A124
        int mZeroParticleFrameCount; // offset 0x14, size 0x4
#endif
    };

    TireState();

    ~TireState();

    USE_FASTALLOC(TireState)

    void UpdateWorld(const WCollider *wc, bool rain, bool flat);

    void KillSkids();

    void DoFX(float slip, float skid, float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT);

    void DoSkids(float intensity, const bVector3 *deltaPos, const bMatrix4 *tirematrix, const bMatrix4 *bodymatrix, float SkidWidth);

    bVector4 mPrevTirePos; // offset 0x8, size 0x10
    WWorldPos mWPos;       // offset 0x18, size 0x3C
    SkidMaker mSkidMaker;  // offset 0x54, size 0x4
    bVector4 mTirePos;     // offset 0x58, size 0x10
    bVector4 mGroundPos;   // offset 0x68, size 0x10
    float mRoll;           // offset 0x78, size 0x4
    bool mRaining;         // offset 0x7C, size 0x1
    bool mFlat;            // offset 0x80, size 0x1
    SimSurface mSurface;   // offset 0x84, size 0x14
    Effect mSlipFX;        // offset 0x98, size 0x18
    Effect mSkidFX;        // offset 0xB0, size 0x18
    Effect mDriveFX;       // offset 0xC8, size 0x18

  private:
    void SetSurface(const SimSurface &surface);
};

void NotifyTireStateEffectOfEmitterDelete(void *tire_state_effect, EmitterGroup *grp) {
    TireState::Effect *effect = static_cast<TireState::Effect *>(tire_state_effect);
    effect->ResetGroup();
}

void KillSkidsOnRaceRestart() {
    for (TireState *ts = gTireStateList.GetHead(); ts != gTireStateList.EndOfList(); ts = ts->GetNext()) {
        ts->KillSkids();
    }

    DeleteAllSkids();
}

#ifndef EA_BUILD_A124
void TireState::Effect::FreeUpFX() {
    if (this->mGroup != nullptr) {
        this->mGroup->SetOldSurfaceEffectFlag();
        this->mGroup->UnSubscribe();
    }

    this->mGroup = nullptr;
    this->mNeedsLazyInit = true;
    this->mZeroParticleFrameCount = 0;
}
#endif

void TireState::Effect::LazyInit() {
    if (this->mGroup != nullptr) {
        this->mGroup->SetOldSurfaceEffectFlag();
        this->mGroup->UnSubscribe();
    }

    this->mGroup = nullptr;
    if (this->mEmitterKey == 0 || this->mEmitterKey == 0xeec2271a) {
        return;
    }

    Attrib::Gen::emittergroup atr(this->mEmitterKey, 0, nullptr);
    if (!atr.IsValid()) {
        return;
    }

    this->mGroup = gEmitterSystem.CreateEmitterGroup(atr.GetConstCollection(), 0x40000000);
    if (this->mGroup != nullptr) {
        this->mGroup->Enable();
        this->mGroup->SubscribeToDeletion(this, NotifyTireStateEffectOfEmitterDelete);
    }

    this->mNeedsLazyInit = false;
#ifndef EA_BUILD_A124
    this->mZeroParticleFrameCount = 0;
#endif
}

void TireState::Effect::Set(const TireEffectRecord &record) {
    unsigned int collection = record.mEmitter.GetCollectionKey();

    this->mMinVel = record.mMinSpeed;
    this->mMaxVel = record.mMaxSpeed;
    if (this->mEmitterKey != collection) {
        this->mEmitterKey = collection;
        this->mNeedsLazyInit = true;
#ifndef EA_BUILD_A124
        this->mZeroParticleFrameCount = 0;
#endif
    }
}

void TireState::Effect::Update(float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT, const bVector4 &pos) {
    float intensity = UMath::Ramp(speed, this->mMinVel, this->mMaxVel);

    if (intensity <= 0.0f) {
        if (this->mGroup != nullptr) {
            this->mGroup->SetOldSurfaceEffectFlag();
            this->mGroup->UnSubscribe();
            this->mGroup = nullptr;
            this->mNeedsLazyInit = true;
#ifndef EA_BUILD_A124
            this->mZeroParticleFrameCount = 0;
#endif
        }
        return;
    }

    if (this->mNeedsLazyInit) {
        this->LazyInit();
    }

    if (this->mGroup == nullptr || intensity <= 0.0f) {
        return;
    } else {
        // TODO
        const bMatrix4 *orientation = false ? this->mGroup->GetLocalWorld() : car_matrix;

        bMatrix4 emitter_world;
        bCopy(&emitter_world, orientation);
        emitter_world.v3 = pos;
        emitter_world.v3.w = 1.0f;

        this->mGroup->SetLocalWorld(&emitter_world);
        this->mGroup->SetInheritVelocity(car_velocity);
        this->mGroup->SetIntensity(intensity);
        this->mGroup->Update(dT);

#ifndef EA_BUILD_A124
        if (this->mGroup->GetNumParticles() == 0) {
            this->mZeroParticleFrameCount++;
        }

        if (this->mZeroParticleFrameCount < 11) {
            return;
        }
#endif

        this->mGroup->SetOldSurfaceEffectFlag();
    }

    this->mGroup->UnSubscribe();
    this->mGroup = nullptr;
    this->mNeedsLazyInit = true;
#ifndef EA_BUILD_A124
    this->mZeroParticleFrameCount = 0;
#endif
}

TireState::TireState()
    : mWPos(0.025f), //
      mSkidMaker() {
    this->mRoll = 0.0f;
    this->mPrevTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->mTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->mGroundPos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    this->SetSurface(SimSurface::kNull);
    this->mRaining = false;
    this->mFlat = false;
    gTireStateList.AddTail(this);
}

TireState::~TireState() {
    this->KillSkids();
    this->Remove();

    if (this->mDriveFX.mGroup != nullptr) {
        this->mDriveFX.mGroup->UnSubscribe();
    }

    if (this->mSkidFX.mGroup != nullptr) {
        this->mSkidFX.mGroup->UnSubscribe();
    }

    if (this->mSlipFX.mGroup != nullptr) {
        this->mSlipFX.mGroup->UnSubscribe();
    }
}

static const float MinimumSkidWidthScale = 0.3f;
static const float MinimumSkidIntensityScale = 0.3f;
static const bool twkDrawSkids = true;

void bRotateVector(bVector3 *dest, const bMatrix4 *m, bVector3 *v) {
    dest->x = m->v0.x * v->x + m->v1.x * v->y + m->v2.x * v->z;
    dest->y = m->v0.y * v->x + m->v1.y * v->y + m->v2.y * v->z;
    dest->z = m->v0.z * v->x + m->v1.z * v->y + m->v2.z * v->z;
}

void TireState::KillSkids() {
    mSkidMaker.MakeNoSkid();
}

void TireState::DoSkids(float intensity, const bVector3 *deltaPos, const bMatrix4 *tirematrix, const bMatrix4 *bodymatrix, float SkidWidth) {
    if (this->mWPos.OnValidFace() && intensity > MinimumSkidWidthScale && SkidWidth > 0.0f && twkDrawSkids) {
        bMatrix4 WorldMatrix;
        int Terrain;
        bVector2 *skid_direction;

        bMulMatrix(&WorldMatrix, bodymatrix, tirematrix);

        bVector3 delta_pos;

        delta_pos.x = -deltaPos->y;
        delta_pos.y = deltaPos->x;
        delta_pos.z = 0.0f;
        bNormalize(&delta_pos, &delta_pos);

        bVector3 tire_direction;
        bVector3 temp(0.0f, 1.0f, 0.0f);

        bRotateVector(&tire_direction, &WorldMatrix, &temp);

        float skid_dot = bAbs(bDot(&tire_direction, &delta_pos));
        float skid_width_scale = skid_dot;

        if (skid_width_scale < 0.5f) {
            skid_width_scale = 0.5f;
        }

        float skid_width = SkidWidth;
        float half_skid_width = skid_width * skid_width_scale * 0.5f;

        bNormalize(&delta_pos, &delta_pos, half_skid_width);

        bVector3 right_point = reinterpret_cast<const bVector3 &>(WorldMatrix.v3) + delta_pos;
        bVector3 left_point = reinterpret_cast<const bVector3 &>(WorldMatrix.v3) - delta_pos;

        UMath::Vector3 right_point_l;
        UMath::Vector3 left_point_l;
        eUnSwizzleWorldVector(right_point, reinterpret_cast<bVector3 &>(right_point_l));
        eUnSwizzleWorldVector(left_point, reinterpret_cast<bVector3 &>(left_point_l));

        float right_elevation = this->mWPos.HeightAtPoint(right_point_l);
        float left_elevation = this->mWPos.HeightAtPoint(left_point_l);

        bVector3 skid_centre(WorldMatrix.v3.x, WorldMatrix.v3.y, (right_elevation + left_elevation) * 0.5f);

        delta_pos.z = right_elevation * 0.5f - left_elevation * 0.5f;

        bNormalize(&delta_pos, &delta_pos, half_skid_width);

        float skid_intensity_scale = skid_dot * intensity;

        if (skid_intensity_scale < MinimumSkidIntensityScale) {
            skid_intensity_scale = MinimumSkidIntensityScale;
        }

        this->mSkidMaker.MakeSkid(nullptr, &skid_centre, &delta_pos, 1, skid_intensity_scale);
    } else {
        this->mSkidMaker.MakeNoSkid();
    }
}

#define APPROX_ZERO(flt) (!(flt > 1.192092896e-07f || flt < (-1.192092896e-07f)))

void TireState::DoFX(float slip, float skid, float speed, const bVector3 *car_velocity, const bMatrix4 *car_matrix, float dT) {
    // TODO PS2
#ifndef EA_BUILD_A124
    if (APPROX_ZERO(slip) && APPROX_ZERO(skid) && APPROX_ZERO(speed)) {
        this->mDriveFX.FreeUpFX();
        this->mSlipFX.FreeUpFX();
        this->mSkidFX.FreeUpFX();
        return;
    }
#endif

    if (this->mWPos.OnValidFace()) {
        this->SetSurface(SimSurface(this->mWPos.GetSurface()));

        this->mSlipFX.Update(slip, car_velocity, car_matrix, dT, this->mGroundPos);
        this->mSkidFX.Update(skid, car_velocity, car_matrix, dT, this->mGroundPos);

        if (numCopsActiveView < 2) {
            if (!INIS::Exists()) {
                this->mDriveFX.Update(speed, car_velocity, car_matrix, dT, this->mGroundPos);
            }
        }
    }
}

void TireState::SetSurface(const SimSurface &surface) {
    unsigned int numslip = surface.Num_TireSlipEffects();
    unsigned int numslide = surface.Num_TireSlideEffects();
    unsigned int numdrive = surface.Num_TireDriveEffects();

    unsigned int slip_index = !this->mFlat || numslip < 3 ? (this->mRaining && numslip > 1 ? 1 : 0) : 2;
    unsigned int slide_index = !this->mFlat || numslide < 3 ? (this->mRaining && numslide > 1 ? 1 : 0) : 2;
    unsigned int drive_index = !this->mFlat || numdrive < 3 ? (this->mRaining && numdrive > 1 ? 1 : 0) : 2;

    const TireEffectRecord &slip_record = surface.TireSlipEffects(slip_index);
    const TireEffectRecord &slide_record = surface.TireSlideEffects(slide_index);
    const TireEffectRecord &drive_record = surface.TireDriveEffects(drive_index);

    this->mSurface = surface;
    this->mSlipFX.Set(slip_record);
    this->mDriveFX.Set(drive_record);
    this->mSkidFX.Set(slide_record);
}

void TireState::UpdateWorld(const WCollider *wc, bool rain, bool flat) {
    UMath::Vector3 pt;
    eUnSwizzleWorldVector(reinterpret_cast<bVector3 &>(this->mTirePos), reinterpret_cast<bVector3 &>(pt));

    this->mWPos.FindClosestFace(wc, pt, true);

    UMath::Vector3 ground;

    if (this->mSurface.GetConstCollection() != this->mWPos.GetSurface() || this->mRaining != rain || this->mFlat != flat) {
        this->mRaining = rain;
        this->mFlat = flat;
        this->SetSurface(this->mWPos.GetSurface());
    }

    ground = pt;
    ground.y = this->mWPos.HeightAtPoint(ground);

    eSwizzleWorldVector(reinterpret_cast<bVector3 &>(ground), reinterpret_cast<bVector3 &>(this->mGroundPos));
}

void RenderConn::Pkt_Car_Service::HidePart(const UCrc32 &partname) {
    int id = GetCarPartIDFromCrc(partname);

    // TODO in Undercover this check is inside the inline, but that doesn't seem to match here
    if (id - 1 < 75U) {
        this->mPartState.Set(id);
    }
}

Sim::Connection *CarRenderConn::Construct(const Sim::ConnectionData &data) {
    RenderConn::Pkt_Car_Open *oc = reinterpret_cast<RenderConn::Pkt_Car_Open *>(data.pkt);
    CarType ct = CarPartDB.GetCarType(oc->mModelHash);

    if (ct == -1 || ct > 0x53) {
        return nullptr;
    }

    return new CarRenderConn(data, ct, oc);
}

BIND_SIM_CONN(CarRenderConn);

static const float Tweak_MinRenderTireRadius = 0.1f;

CarRenderConn::CarRenderConn(const Sim::ConnectionData &data, CarType ct, RenderConn::Pkt_Car_Open *oc)
    : VehicleRenderConn(data, ct),                                          //
      IAttributeable(),                                                     //
      mAttributes(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),  //
      mPhysics(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),     //
      mTirePhysics(static_cast<Attrib::Collection *>(nullptr), 0, nullptr), //
      mExtraBodyAngle(UMath::Vector2::kZero),                               //
      mFlatTireAngle(UMath::Vector3::kZero),                                //
      mWheelHop(UMath::Vector3::kZero),                                     //
      mRoadNoise(UMath::Vector3::kZero),                                    //
      mEnginePower(0.0f),                                                   //
      mAnimTime(0.0f),                                                      //
      mShiftPitchAngle(0.0f),                                               //
      mEngineTorqueAngle(0.0f),                                             //
      mEngineVibrationAngle(0.0f),                                          //
      mEnginePitchAngle(0.0f),                                              //
      mLastRenderFrame(0),                                                  //
      mLastVisibleFrame(0),                                                 //
      mDistanceToView(0.0f),                                                //
      mFlashTime(0.0f),                                                     //
      mShifting(0.0f),                                                      //
      mDoContrailEffect(false),                                             //
      mUsage(oc->mUsage),                                                   //
      mFlags() {
    bIdentity(&this->mRenderMatrix);

    this->mPhysics = Attrib::Gen::pvehicle(oc->mPhysicsKey, 0, nullptr);
    this->mTirePhysics = Attrib::Gen::tires(this->mPhysics.tires(0), 0, nullptr);

    this->mSteering[0] = this->mSteering[1] = 0.0f;

    for (unsigned int i = 0; i < 4; i++) {
        this->mTireState[i] = new TireState();

        GetAttributes().TireOffsets(reinterpret_cast<UMath::Vector4 &>(this->mTirePositions[i]), i);
        // missing stripped out ... = bVector4();
        this->mTireRadius[i] = UMath::Max(this->mTirePositions[i].w, Tweak_MinRenderTireRadius);

        this->mTirePositions[i].w = 0.0f;
        this->mTireState[i]->mPrevTirePos = bVector4(0.0f, 0.0f, 0.0f, 0.0f);

        this->mPhysicsRadius[i] = Physics::Info::WheelDiameter(this->mTirePhysics, i < 2) * 0.5f;
    }

    this->mMaxWheelRenderDeltaAngle = DEG2RAD(17.0f);

    this->Load(oc->mWorldID, oc->mUsage, !oc->mSpoolLoad, oc->mCustomizations);
    this->SetFlag(CF_ISPLAYER, oc->mUsage == 0);

    if (this->mUsage == 0 || this->mUsage == 2) {
        int engine_level = Physics::Upgrades::GetLevel(this->mPhysics, Physics::Upgrades::PUT_ENGINE);
        int max_engine_level = Physics::Upgrades::GetMaxLevel(this->mPhysics, Physics::Upgrades::PUT_ENGINE);

        if (engine_level != 0 || engine_level == max_engine_level) {
            this->SetFlag(CF_BLOWOFF, true);
        }
    }

    IAttributeable::Register(this, Attrib::Gen::ecar::ClassKey());
    IAttributeable::Register(this, Attrib::Gen::pvehicle::ClassKey());
    IAttributeable::Register(this, Attrib::Gen::tires::ClassKey());
}

void CarRenderConn::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

CarRenderConn::~CarRenderConn() {
    IAttributeable::UnRegister(this);

    this->mPipeEffects.DeleteAllElements();
    this->mEngineEffects.DeleteAllElements();

    for (int i = 0; i < NUM_ELEMENTS(this->mTireState); i++) {
        delete this->mTireState[i];
    }
}

bool CarRenderConn::TestVisibility(float distance) {
    if (INIS::Get() != nullptr) {
        return true;
    }

    if (this->IsViewAnchor()) {
        return true;
    }

    bool visible = this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastVisibleFrame != 0;

    if (!visible) {
        return false;
    }

    return this->mDistanceToView <= distance;
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

static const float Tweak_MaxDistForSteeringAnim = 100.0f;

void CarRenderConn::UpdateSteering(float dT, const RenderConn::Pkt_Car_Service &data) {

    if (!this->TestVisibility(renderModifier * Tweak_MaxDistForSteeringAnim)) {
        this->mSteering[1] = 0.0f;
        this->mSteering[0] = 0.0f;
        return;
    }

    float maxsteer = DEG2RAD(this->GetAttributes().MaxTireSteer());
    float steer_delta = DEG2RAD(this->GetAttributes().SteerSpeed()) * dT;

    for (unsigned int i = 0; i < NUM_ELEMENTS(this->mSteering); i++) {
        float newangle = data.mSteering[i];

        if (this->mSteering[i] < newangle - steer_delta) {
            this->mSteering[i] = this->mSteering[i] + steer_delta;
        } else {
            if (this->mSteering[i] > newangle + steer_delta) {
                this->mSteering[i] = this->mSteering[i] - steer_delta;
            } else {
                this->mSteering[i] = newangle;
            }
        }

        this->mSteering[i] = UMath::Clamp(this->mSteering[i], -maxsteer, maxsteer);
    }
}

// UNSOLVED, the BitArray inlines are weird
void CarRenderConn::UpdateParts(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (this->mPartState != data.mPartState) {
        // TODO magic
        for (unsigned int i = 0; i < 0x4c; i++) {
            bool hide = data.mPartState.Test(i);

            if (hide != this->mPartState.Test(i)) {
                if (hide) {
                    this->HidePart(static_cast<CAR_PART_ID>(i));
                } else {
                    this->ShowPart(static_cast<CAR_PART_ID>(i));
                }
            }
        }
    }

    this->mPartState = data.mPartState;
}

// TODO the multification by M_TWOPI inside Sinr is getting optimized out I think
void CarRenderConn::AddRoadNoise(float speed, unsigned int tires, const RoadNoiseRecord &noise) {
    if (noise.Frequency * noise.Amplitude * noise.MaxSpeed > 0.0f) {
        float intensity = UMath::Ramp(speed, noise.MinSpeed, noise.MaxSpeed);
        float frequency = noise.Frequency * intensity;
        float amplitude = this->GetAttributes().RoadNoise() * DEG2RAD(noise.Amplitude) * intensity;

        unsigned int front = 3;
        unsigned int rear = 12;
        unsigned int right = 9;
        unsigned int left = 6;

        unsigned int do_front = tires & front;
        unsigned int do_rear = tires & rear;
        unsigned int do_left = tires & left;
        unsigned int do_right = tires & right;
        unsigned int do_pitch;
        unsigned int do_roll = do_front | do_rear;

        float noise_pitch = 0.0f;
        if (do_roll) {
            noise_pitch = amplitude * UMath::Sinr(this->mAnimTime * frequency) * 0.5f;
            if (!do_front) {
                noise_pitch = UMath::Abs(noise_pitch);
            }
            if (!do_rear) {
                noise_pitch = -UMath::Abs(noise_pitch);
            }
        }

        float noise_roll = 0.0f;
        if (do_roll) {
            noise_roll = amplitude * UMath::Sinr((this->mAnimTime + 0.33f) * frequency);
            if (!do_right) {
                noise_roll = UMath::Abs(noise_roll);
            }
            if (!do_left) {
                noise_roll = -UMath::Abs(noise_roll);
            }
        }

        this->mRoadNoise.y += noise_pitch;
        this->mRoadNoise.x += noise_roll;
    }
}

static const RoadNoiseRecord Tweak_BlowOutNoise(4.0f, 1.0f, 0.0f, 10.0f);

static const float Tweak_MaxDistForRoadNoiseAnim = 30.0f;

int Tweak_DisableRoadNoise = 0;

void CarRenderConn::UpdateRoadNoise(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data) {
    UMath::Clear(this->mRoadNoise);

    if (!this->TestVisibility(renderModifier * Tweak_MaxDistForRoadNoiseAnim) || Tweak_DisableRoadNoise != 0) {
        return;
    }

    RoadNoiseRecord roughness;
    for (unsigned int i = 0; i < 4; i++) {
        if (((data.mGroundState >> i) & 1U) != 0) {
            const RoadNoiseRecord &tirenoise = this->mTireState[i]->mSurface.RenderNoise();

            if (tirenoise.Amplitude * tirenoise.Frequency > roughness.Amplitude * roughness.Frequency) {
                roughness = tirenoise;
            }
        }
    }

    this->AddRoadNoise(carspeed, data.mGroundState, roughness);
    this->AddRoadNoise(carspeed, data.mGroundState & data.mBlowOuts, Tweak_BlowOutNoise);

    float siny = UMath::Sinr(this->mRoadNoise.y);
    float sinx = UMath::Sinr(this->mRoadNoise.x);

    this->mRoadNoise.z = UMath::Max(this->mTirePositions[0].x * siny, 0.0f);
    this->mRoadNoise.z = UMath::Max(this->mTirePositions[3].x * siny, this->mRoadNoise.z);
    this->mRoadNoise.z = UMath::Max(this->mTirePositions[0].y * sinx, this->mRoadNoise.z);
    this->mRoadNoise.z = UMath::Max(this->mTirePositions[1].y * sinx, this->mRoadNoise.z);
}

static const float Tweak_MaxDistanceForEngineAnim = 30.0f;
static const float Tweak_EngineAnimNoiseReduction = 0.005f;
static const float Tweak_EngineAnimShiftMinAcc = 0.1f;
static const float Tweak_EngineAnimShiftMaxAcc = 0.5f;
static const float Tweak_EngineAnimShiftGearScale = 0.95f;
static const float Tweak_EngineAnimShiftDownScale = 0.25f;
static const float Tweak_EngineAnimShiftMinSpeed = 10.0f;

void CarRenderConn::UpdateEngineAnimation(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (!this->TestVisibility(renderModifier * Tweak_MaxDistanceForEngineAnim)) {
        this->mEnginePower = 0.0f;
        this->mEngineVibrationAngle = 0.0f;
        this->mEngineTorqueAngle = 0.0f;
        this->mShiftPitchAngle = 0.0f;
        this->mShifting = 0.0f;
        this->mEnginePitchAngle = 0.0f;
        return;
    }

    if (this->mShifting != 0.0f) {
        const float car_speed = bLength(this->GetVelocity());
        const float shift_speed = DEG2RAD(this->GetAttributes().ShiftSpeed());
        const float max_pitch = DEG2RAD(this->GetAttributes().ShiftAngle());
        const int gear = data.mGear - G_FIRST;

        if (0.0f < shift_speed && 0.0f < max_pitch && gear >= 0 && Tweak_EngineAnimShiftMinSpeed < car_speed) {
            float fwd_accel = bDot(this->GetAcceleration(), reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v0)) / 9.8f;
            float accel_ratio = UMath::Ramp(UMath::Abs(fwd_accel), Tweak_EngineAnimShiftMinAcc, Tweak_EngineAnimShiftMaxAcc);
            float gear_ratio = UMath::Pow(Tweak_EngineAnimShiftGearScale, static_cast<float>(gear));

            this->mShiftPitchAngle = UMath::Sina(UMath::Abs(this->mShifting) * 0.5f) * max_pitch * gear_ratio * accel_ratio;

            if (this->mShifting > 0.0f) {
                this->mShifting = this->mShifting - (dT * shift_speed) / max_pitch;
                this->mShifting = UMath::Max(this->mShifting, 0.0f);
            } else if (this->mShifting < 0.0f) {
                this->mShiftPitchAngle *= Tweak_EngineAnimShiftDownScale;
                this->mShifting = this->mShifting + (dT * shift_speed) / max_pitch;
                this->mShifting = UMath::Min(mShifting, 0.0f);
            }
        } else {
            this->mShifting = 0.0f;
            this->mShiftPitchAngle = 0.0f;
        }
    } else {
        this->mShiftPitchAngle = 0.0f;
    }

    float delta = data.mEnginePower - this->mEnginePower;
    if (UMath::Abs(delta) < Tweak_EngineAnimNoiseReduction) {
        delta = 0.0f;
    }

    float rev_accel = delta / dT;

    this->mEnginePower = UMath::Clamp(this->mEnginePower + delta, 0.0f, 1.0f);
    this->mEnginePitchAngle = data.mAnimatedCarPitch;

    if (data.mAnimatedCarRoll != 0.0f) {
        this->mEngineTorqueAngle = data.mAnimatedCarRoll;
    } else {
        float acceleration; // TODO
        float max_rev = data.mEnginePower * data.mEngineSpeed * DEG2RAD(this->GetAttributes().EngineRevAngle());
        float rev_speed = DEG2RAD(this->GetAttributes().EngineRevSpeed());
        float desired_angle = max_rev * UMath::Ramp(rev_accel * this->GetAttributes().EngineRev(), 0.0f, 0.2f);

        if (this->mEngineTorqueAngle < desired_angle) {
            this->mEngineTorqueAngle = rev_speed * dT + this->mEngineTorqueAngle;
            this->mEngineTorqueAngle = UMath::Min(this->mEngineTorqueAngle, desired_angle);
        } else {
            this->mEngineTorqueAngle = this->mEngineTorqueAngle - rev_speed * dT;
            this->mEngineTorqueAngle = UMath::Max(mEngineTorqueAngle, desired_angle);
        }

        this->mEngineTorqueAngle = UMath::Clamp(this->mEngineTorqueAngle, 0.0f, max_rev);
    }

    if (data.mAnimatedCarShake != 0.0f) {
        this->mEngineVibrationAngle = data.mAnimatedCarShake;
    } else {
        float max_vibration = DEG2RAD(this->GetAttributes().EngineVibrationMax());
        float min_vibration = DEG2RAD(this->GetAttributes().EngineVibrationMin());

        this->mEngineVibrationAngle = data.mEngineSpeed * bSin(this->mAnimTime * (float)M_TWOPI * this->GetAttributes().EngineVibrationFreq()) *
                                      (min_vibration + max_vibration * data.mEngineSpeed);
    }
}

static const float Tweak_BodyAnimNoiseReduction = 0.2f;
static const float Tweak_MaxDistanceForBodyAnim = 80.0f;

void CarRenderConn::UpdateBodyAnimation(float dT, const RenderConn::Pkt_Car_Service &data) {
    if (!this->TestVisibility(renderModifier * Tweak_MaxDistanceForBodyAnim)) {
        this->mExtraBodyAngle = UMath::Vector2::kZero;
        return;
    }

    const bVector3 *accel = this->GetAcceleration();
    float left_accel = bDot(accel, reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v1)) / 9.8f;
    float fwd_accel = bDot(accel, reinterpret_cast<const bVector3 *>(&this->mRenderMatrix.v0)) / 9.8f;
    const CarBodyMotion &pitch_control = fwd_accel < 0.0f ? this->GetAttributes().BodyDive() : this->GetAttributes().BodySquat();
    const CarBodyMotion &roll_control = this->GetAttributes().BodyRoll();
    float max_pitch = data.mExtraBodyPitch * DEG2RAD(pitch_control.DegPerG);
    float rate_pitch = DEG2RAD(pitch_control.DegPerSec) * dT;
    float max_roll = data.mExtraBodyRoll * DEG2RAD(roll_control.DegPerG);
    float rate_roll = DEG2RAD(roll_control.DegPerSec) * dT;

    if (UMath::Abs(left_accel) < Tweak_BodyAnimNoiseReduction) {
        left_accel = 0.0f;
    }

    if (UMath::Abs(fwd_accel) < Tweak_BodyAnimNoiseReduction) {
        fwd_accel = 0.0f;
    }

    float dest_angle_x = (max_roll + max_roll) * (UMath::Ramp(left_accel, -roll_control.MaxGs, roll_control.MaxGs) - 0.5f);
    float dest_angle_y = (max_pitch + max_pitch) * (UMath::Ramp(-fwd_accel / 9.8f, -pitch_control.MaxGs, pitch_control.MaxGs) - 0.5f);
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

static const bool twkDoContrails = true;

void CarRenderConn::UpdateContrails(const RenderConn::Pkt_Car_Service &data, float dT) {
    float carspeed = bLength(this->GetVelocity());

    if (twkDoContrails && (data.mNos || 44.0f <= carspeed)) {
        if (!INIS::Exists()) {
            this->mDoContrailEffect = true;
            return;
        }
    }

    this->mDoContrailEffect = false;
}

static const float Tweak_WheelHopMinBurnout = 1.0f;
static const float Tweak_WheelHopMaxBurnout = 5.0f;
static const float Tweak_WheelHopMinBrake = 5.0f;
static const float Tweak_WheelHopMaxBrake = 20.0f;
static const float Tweak_WheelHopFrequency = 24.0f;
static const float Tweak_WheelHopAngle = 0.2f;
static const float Tweak_WheelHopTireAngle = 0.3f;
static const float Tweak_WheelHopRollAngle = 0.15f;
static const bool Tweak_UseWheelHop = true;
static const float Tweak_MaxDistanceForWheelHopAnim = 30.0f;
static const float Tweak_TireBlowOffset = -0.12f;

static const float Tweak_MaxDistanceForVehicleEffects = 80.0f;

// UNSOLVED
void CarRenderConn::UpdateTires(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data) {
    float wheel_hop_roll = 0.0f;
    float wheel_hop_pitch = 0.0f;
    float tire_hop = 0.0f;
    bool hop_wheels = false;
    bool flatten_tires = false;

    this->mFlatTireAngle = UMath::Vector3::kZero;

    if (this->TestVisibility(renderModifier * Tweak_MaxDistanceForWheelHopAnim)) {
        flatten_tires = true;

        float hop_scale = this->GetAttributes().WheelHopScale();

        if (Tweak_UseWheelHop && 0.0f < data.mExtraBodyPitch && 0.0f < hop_scale) {
            wheel_hop_roll = (hop_scale * hop_scale) * data.mExtraBodyPitch * DEG2RAD(Tweak_WheelHopRollAngle) *
                             UMath::Abs(bSin(this->mAnimTime + (Tweak_WheelHopFrequency * (float)M_PI_2)));

            wheel_hop_pitch = (hop_scale * hop_scale) * data.mExtraBodyPitch * DEG2RAD(Tweak_WheelHopAngle) *
                              UMath::Abs(bSin(this->mAnimTime * (Tweak_WheelHopFrequency * (float)M_TWOPI)));

            tire_hop = (hop_scale * hop_scale) * data.mExtraBodyPitch * DEG2RAD(Tweak_WheelHopTireAngle) *
                       UMath::Abs(bSin((this->mAnimTime + 0.5f) * (Tweak_WheelHopFrequency * (float)M_TWOPI)));

            hop_wheels = true;
        }
    }

    this->mWheelHop = UMath::Vector3::kZero;
    bool is_view_anchor = this->IsViewAnchor();
    bool candofx = this->TestVisibility(renderModifier * Tweak_MaxDistanceForVehicleEffects);
    CarRenderInfo *car_render_info = this->GetRenderInfo();

    for (unsigned int i = 0; i < 4; i++) {
        unsigned int axle = i >> 1;
        bool onground = ((data.mGroundState >> i) & 1U) != 0;
        bool is_flat = ((data.mBlowOuts >> i) & 1U) != 0;
        TireState *state = this->mTireState[i];

        eIdentity(&this->mTireMatrices[i]);
        eIdentity(&this->mBrakeMatrices[i]);

        float dW = UMath::Clamp((data.mWheelSpeed[i] / this->mTireRadius[i]) * dT, -this->mMaxWheelRenderDeltaAngle, this->mMaxWheelRenderDeltaAngle);
        float compression = data.mCompressions[i] + (this->mTireRadius[i] - this->mPhysicsRadius[i]);

        state->mRoll += dW;

        if (static_cast<float>(M_TWOPI) <= state->mRoll) {
            state->mRoll -= static_cast<float>(M_TWOPI);
        } else if (state->mRoll <= -static_cast<float>(M_TWOPI)) {
            state->mRoll += static_cast<float>(M_TWOPI);
        }

        eRotateY(&this->mTireMatrices[i], &this->mTireMatrices[i], bRadToAng(state->mRoll));

        if (i < 2) {
            eRotateZ(&this->mTireMatrices[i], &this->mTireMatrices[i], bRadToAng(this->mSteering[i]));
            eRotateZ(&this->mBrakeMatrices[i], &this->mBrakeMatrices[i], bRadToAng(this->mSteering[i]));
        }

        if (flatten_tires && is_flat) {
            compression += Tweak_TireBlowOffset;

            float x_angle = UMath::Atan2r(-Tweak_TireBlowOffset, UMath::Abs(this->mTirePositions[i].y)) * -static_cast<float>(M_PI);
            float y_angle = UMath::Atan2r(-Tweak_TireBlowOffset, UMath::Abs(this->mTirePositions[i].x)) * static_cast<float>(M_PI);

            if (this->mTirePositions[i].y < 0.0f) {
                x_angle = -x_angle;
            }

            if (this->mTirePositions[i].x < 0.0f) {
                y_angle = -y_angle;
            }

            this->mFlatTireAngle.x += x_angle;
            this->mFlatTireAngle.y += y_angle;
            this->mFlatTireAngle.z += Tweak_TireBlowOffset * 0.25f;
        }

        if (i > 1 && hop_wheels && onground) {
            float hop_speed_scale;

            if (0.0f < data.mTireSlip[i]) {
                hop_speed_scale = UMath::Ramp(data.mTireSlip[i], Tweak_WheelHopMinBurnout, Tweak_WheelHopMaxBurnout);
            } else {
                hop_speed_scale = UMath::Ramp(-data.mTireSlip[i], Tweak_WheelHopMinBrake, Tweak_WheelHopMaxBrake);
            }

            this->mWheelHop.y = wheel_hop_pitch * hop_speed_scale;
            this->mWheelHop.z = UMath::Max(this->mWheelHop.z, this->mTirePositions[0].x * UMath::Sinr(wheel_hop_pitch * hop_speed_scale));
            this->mWheelHop.x = wheel_hop_roll * hop_speed_scale;
            compression += bSin(tire_hop * hop_speed_scale) * (this->mTirePositions[0].x - this->mTirePositions[i].x);
        }

        this->mBrakeMatrices[i].v3.x = this->mTirePositions[i].x;
        this->mBrakeMatrices[i].v3.z += this->mTirePositions[i].z + compression;
        this->mBrakeMatrices[i].v3.y = this->mTirePositions[i].y;

        this->mTireMatrices[i].v3.x = this->mTirePositions[i].x;
        this->mTireMatrices[i].v3.z += this->mTirePositions[i].z + compression;
        this->mTireMatrices[i].v3.y = this->mTirePositions[i].y;

        if (candofx) {
            car_render_info->SetWheelWobble(i, (data.mBlowOuts >> i) & 1U);
        }

        eMulVector(&state->mTirePos, &this->mRenderMatrix, &this->mTireMatrices[i].v3);
        state->UpdateWorld(this->GetWCollider(), this->GetFlag(CF_ISRAINING), is_flat);

        if (onground) {
            if (candofx) {
                float skid = UMath::Max(UMath::Abs(data.mTireSkid[i] * 0.05f) - 0.1f, 0.0f);
                float slip = UMath::Max(UMath::Abs(data.mTireSlip[i] * 0.2f) - 0.1f, 0.0f);
                float skidmark_intensity = UMath::Sqrt(skid * skid + slip * slip);

                if (0.0f < skidmark_intensity) {
                    bVector4 delta_pos;
                    float SkidWidth;

                    bSub(&delta_pos, &state->mTirePos, &state->mPrevTirePos);
                    SkidWidth = this->GetAttributes().TireSkidWidth(i);

                    state->DoSkids(skidmark_intensity, reinterpret_cast<const bVector3 *>(&delta_pos), &this->mTireMatrices[i], &this->mRenderMatrix,
                                   SkidWidth);
                } else {
                    state->KillSkids();
                }

                float slipfx_ratio = data.mTireSlip[i] * this->GetAttributes().SlipFX(axle);
                float skidfx_ratio = data.mTireSkid[i] * this->GetAttributes().SkidFX(axle);

                state->DoFX(slipfx_ratio, skidfx_ratio, carspeed, this->GetVelocity(), &this->mRenderMatrix, dT);
            } else {
                state->KillSkids();
            }
        } else {
            state->KillSkids();
        }

        state->mPrevTirePos = state->mTirePos;
    }
}

static void StopEffect(VehicleRenderConn::Effect *effect) {
    effect->Stop();
}

void CarRenderConn::UpdateEffects(const RenderConn::Pkt_Car_Service &data, float dT) {
    if (!this->TestVisibility(renderModifier * Tweak_MaxDistanceForVehicleEffects)) {
        std::for_each(this->mPipeEffects.begin(), this->mPipeEffects.end(), StopEffect);
        std::for_each(this->mEngineEffects.begin(), this->mEngineEffects.end(), StopEffect);
        return;
    }

    unsigned int damage_key = this->GetAttributes().DamageEffect().GetCollectionKey();
    unsigned int death_key = this->GetAttributes().DeathEffect().GetCollectionKey();
    unsigned int engine_key = this->GetAttributes().EngineBlownEffect().GetCollectionKey();
    unsigned int missshift_key = this->GetAttributes().MissShiftEffect().GetCollectionKey();
    unsigned int nos_key = this->GetAttributes().NOSEffect().GetCollectionKey();

    for (VehicleRenderConn::Effect *pipe_effect = this->mPipeEffects.GetHead(); pipe_effect != this->mPipeEffects.EndOfList();
         pipe_effect = pipe_effect->GetNext()) {
        if (data.mNos) {
            pipe_effect->Update(&this->mRenderMatrix, nos_key, dT, 1.0f, this->GetVelocity());
        } else if (this->GetFlag(CF_MISSSHIFT)) {
            pipe_effect->Fire(&this->mRenderMatrix, missshift_key, 1.0f, this->GetVelocity());
        } else if (this->GetFlag(CF_BLOWOFF) && this->mShifting != 0.0f) {
            pipe_effect->Update(&this->mRenderMatrix, nos_key, dT, 1.0f, this->GetVelocity());
        } else {
            pipe_effect->Stop();
        }
    }

    for (VehicleRenderConn::Effect *engine_effect = this->mEngineEffects.GetHead(); engine_effect != this->mEngineEffects.EndOfList();
         engine_effect = engine_effect->GetNext()) {
        if (death_key != 0 && data.mHealth <= 0.0f) {
            engine_effect->Update(&this->mRenderMatrix, death_key, dT, 1.0f, this->GetVelocity());
        } else if (damage_key != 0 && data.mHealth <= 0.5f) {
            engine_effect->Update(&this->mRenderMatrix, damage_key, dT, 1.0f, this->GetVelocity());
        } else if (data.mEngineBlown) {
            engine_effect->Update(&this->mRenderMatrix, engine_key, dT, 1.0f, this->GetVelocity());
        } else {
            engine_effect->Stop();
        }
    }

    this->SetFlag(CF_MISSSHIFT, false);
}

void CarRenderConn::Update(const RenderConn::Pkt_Car_Service &data, float dT) {
    if (!this->CanUpdate()) {
        return;
    }
    CarRenderInfo *car_render_info = this->GetRenderInfo();

    if (car_render_info == 0) {
        return;
    }

    car_render_info->SetDamageInfo(*reinterpret_cast<const DamageZone::Info *>(&data.mDamageInfo));
    car_render_info->SetDeltaTime(dT);
    car_render_info->SetLights(data.mLights);
    car_render_info->SetBrokenLights(data.mBrokenLights);
    car_render_info->SetFlashing(data.mFlashing);
    car_render_info->UpdateFlashing();

    car_render_info->NOSstate = data.mNos;
    car_render_info->mSteeringR = bRadToAng(this->mSteering[0]);
    car_render_info->mSteeringL = bRadToAng(this->mSteering[1]);

    float carspeed = bLength(this->GetVelocity());

    this->mAnimTime += dT;
    if (10.0f <= this->mAnimTime) {
        this->mAnimTime -= 10.0f;
    }

    car_render_info->SetAnimationTime(this->mAnimTime);

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

static const bool Tweak_AllowRenderBodyMotion = true;
static const float Tweak_MaxDistanceForRenderBodyMotion = 80.0f;

void CarRenderConn::BuildRenderMatrix(float dT) {
    CarRenderInfo *car_render_info = this->GetRenderInfo();
    bVector4 offset(this->GetModelOffset());

    if (0.0f < dT) {
        UMath::Vector3 e0;
        UMath::Vector3 e1;
        UMath::Matrix4ToEuler(reinterpret_cast<const UMath::Matrix4 &>(this->mRenderMatrix), e0);
        UMath::Matrix4ToEuler(reinterpret_cast<const UMath::Matrix4 &>(*this->GetBodyMatrix()), e1);

        UMath::Vector3 v;
        UMath::Sub(e1, e0, v);
        UMath::Scale(v, (1.0f / dT) * (float)M_TWOPI);

        car_render_info->mAngularVelocity = reinterpret_cast<bVector3 &>(v);

        UMath::Vector3 v0 = reinterpret_cast<const UMath::Vector3 &>(car_render_info->mVelocity);
        UMath::Vector3 v1 = reinterpret_cast<const UMath::Vector3 &>(*this->GetVelocity());

        UMath::Sub(v1, v0, v);
        UMath::Scale(v, 1.0f / dT);

        car_render_info->mAcceleration = reinterpret_cast<bVector3 &>(v);
        car_render_info->mVelocity = *this->GetVelocity();
    }

    this->mRenderMatrix = *this->GetBodyMatrix();

    bVector4 rotOffset;
    eMulVector(&rotOffset, this->GetBodyMatrix(), &offset);
    this->mRenderMatrix.v3.x -= rotOffset.x;
    this->mRenderMatrix.v3.y -= rotOffset.y;
    this->mRenderMatrix.v3.z -= rotOffset.z;
}

void CarRenderConn::UpdateRenderMatrix(float dT) {
    if (Tweak_AllowRenderBodyMotion && this->TestVisibility(renderModifier * Tweak_MaxDistanceForRenderBodyMotion)) {
        bMatrix4 tire_matrices_world[4];
        bMatrix4 brake_matrices_world[4];

        for (int i = 0; i < 4; i++) {
            eMulMatrix(&tire_matrices_world[i], &this->mTireMatrices[i], &this->mRenderMatrix);
            eMulMatrix(&brake_matrices_world[i], &this->mBrakeMatrices[i], &this->mRenderMatrix);
        }

        float roll = this->mWheelHop.x + this->mExtraBodyAngle.x + this->mFlatTireAngle.x + this->mRoadNoise.y + this->mEngineTorqueAngle +
                     this->mEngineVibrationAngle;
        float pitch = this->mWheelHop.y + this->mExtraBodyAngle.y + this->mFlatTireAngle.y + this->mRoadNoise.y + this->mEnginePitchAngle +
                      this->mShiftPitchAngle + DEG2RAD(this->GetAttributes().ExtraPitch());
        float ride = this->mWheelHop.z + this->mRoadNoise.z + this->mFlatTireAngle.z + INCH2METERS(this->GetAttributes().RideHeight());

        bMatrix4 rotmat;
        bMatrix4 ident;
        bMatrix4 xmat;
        bMatrix4 ymat;

        bIdentity(&ident);
        eRotateX(&xmat, &ident, bRadToAng(roll));
        eRotateY(&ymat, &ident, bRadToAng(pitch));
        eMulMatrix(&rotmat, &xmat, &ymat);

        bMatrix4 tmp(this->mRenderMatrix);
        eMulMatrix(&this->mRenderMatrix, &rotmat, &tmp);

        this->mRenderMatrix.v3.x += this->mRenderMatrix.v2.x * ride;
        this->mRenderMatrix.v3.y += this->mRenderMatrix.v2.y * ride;
        this->mRenderMatrix.v3.z += this->mRenderMatrix.v2.z * ride;

        bMatrix4 invmat;
        bInvertMatrix(&invmat, &this->mRenderMatrix);

        for (int i = 0; i < 4; i++) {
            eMulMatrix(&this->mTireMatrices[i], &tire_matrices_world[i], &invmat);
            eMulMatrix(&this->mBrakeMatrices[i], &brake_matrices_world[i], &invmat);
        }

        float wheel_well = INCH2METERS(this->GetAttributes().WheelWell());

        if (0.0f < wheel_well) {
            float tire_top = this->mTireMatrices[0].v3.z + this->mTireRadius[0];

            for (int i = 1; i < 4; i++) {
                tire_top = bMax(tire_top, this->mTireMatrices[i].v3.z + this->mTireRadius[i]);
            }

            if (tire_top > wheel_well) {
                float delta = tire_top - wheel_well;
                bVector4 offset(this->mRenderMatrix.v2);

                offset *= delta;
                offset.w = 0.0f;
                this->mRenderMatrix.v3 += offset;

                for (int i = 0; i < 4; i++) {
                    this->mTireMatrices[i].v3.z -= delta;
                    this->mBrakeMatrices[i].v3.z -= delta;
                }
            }
        }
    }
}

void CarRenderConn::Hide(bool b) {
    if (this->GetFlag(CF_HIDDEN) != b) {
        this->SetFlag(CF_HIDDEN, b);
        if (b) {
            this->mAnimTime = 0.0f;
            for (int i = 0; i < 4; i++) {
                this->mTireState[i]->KillSkids();
            }

            VehicleRenderConn::Hide();

            VehicleRenderConn::Effect *effect;
            for (effect = this->mEngineEffects.GetHead(); effect != this->mEngineEffects.EndOfList(); effect = effect->GetNext()) {
                StopEffect(effect);
            }

            for (effect = this->mPipeEffects.GetHead(); effect != this->mPipeEffects.EndOfList(); effect = effect->GetNext()) {
                StopEffect(effect);
            }
        }
    }
}

void CarRenderConn::OnFetch(float dT) {
    bool visible = false;

    if ((this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastVisibleFrame != 0) || this->IsViewAnchor()) {
        visible = true;
    }

    RenderConn::Pkt_Car_Service data(visible, this->mDistanceToView);
    if (this->Service(&data)) {
        this->Hide(false);
        this->Update(data, dT);
    } else {
        this->Hide(true);
    }
}

void CarRenderConn::OnLoaded(CarRenderInfo *carrender_info) {
    ProfileNode profile_node("TODO", 0);
    VehicleRenderConn::OnLoaded(carrender_info);

    if (carrender_info == nullptr) {
        return;
    }

    if (carrender_info->pRideInfo) {
        CarPart *part_rim = carrender_info->pRideInfo->GetPart(0x42);

        if (part_rim) {
            unsigned int numSpokes = static_cast<signed char>(part_rim->GetSpokeCount());

            if (numSpokes == 0) {
                numSpokes = bAbs(GetAttributes().WheelSpokeCount());
            }

            if (numSpokes != 0) {
                mMaxWheelRenderDeltaAngle = DEG2RAD(360.0f / (2.0f * numSpokes) - 1.0f);
            }
        }
    }

    carrender_info->InitEmitterPositions(mTirePositions);

    if (mPipeEffects.IsEmpty()) {
        for (CarEmitterPosition *emitter_position = carrender_info->EmitterPositionList[10].GetHead();
             emitter_position != carrender_info->EmitterPositionList[10].EndOfList(); emitter_position = emitter_position->GetNext()) {

            ePositionMarker *position_marker = emitter_position->PositionMarker;

            if (position_marker) {
                mPipeEffects.AddTail(new VehicleRenderConn::Effect(&position_marker->Matrix));
            } else {
                bMatrix4 tempmat;
                bIdentity(&tempmat);

                tempmat.v3.x = emitter_position->X;
                tempmat.v3.y = emitter_position->Y;
                tempmat.v3.z = emitter_position->Z;

                mPipeEffects.AddTail(new VehicleRenderConn::Effect(&tempmat));
            }
        }
    }

    if (mEngineEffects.IsEmpty()) {
        for (CarEmitterPosition *emitter_position = carrender_info->EmitterPositionList[9].GetHead();
             emitter_position != carrender_info->EmitterPositionList[9].EndOfList(); emitter_position = emitter_position->GetNext()) {

            ePositionMarker *position_marker = emitter_position->PositionMarker;

            if (position_marker) {
                mEngineEffects.AddTail(new VehicleRenderConn::Effect(&position_marker->Matrix));
            } else {
                bMatrix4 tempmat;
                bIdentity(&tempmat);

                tempmat.v3.x = emitter_position->X;
                tempmat.v3.y = emitter_position->Y;
                tempmat.v3.z = emitter_position->Z;

                mEngineEffects.AddTail(new VehicleRenderConn::Effect(&tempmat));
            }
        }
    }
}

void CarRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    bCopy(matrix, &this->mRenderMatrix);
}

CameraAnchor *RVManchor;
int NumTimesRenderTestPlayerCar = 0;

void CarRenderConn::OnRender(eView *view, int reflection) {
    if (!this->CanRender()) {
        return;
    }

    if (this->mLastRenderFrame != eGetFrameCounter()) {
        this->mDistanceToView = 999999.0f;
    }
    this->mLastRenderFrame = eGetFrameCounter();

    CameraMover *mover = view->GetCameraMover();

    if (mover != nullptr && !mover->RenderCarPOV()) {
        CameraAnchor *anchor = mover->GetAnchor();

        if (anchor != nullptr && anchor->GetWorldID() == this->GetWorldID()) {
            return;
        }
    }

    if (view->GetID() > EVIEW_SHADOWMATTE && view->GetID() < NUM_EVIEWS) {
        CameraMover *mover = eGetView(1, false)->GetCameraMover();

        if (mover != nullptr) {
            CameraAnchor *anchor = mover->GetAnchor();

            if (anchor != nullptr && anchor->GetWorldID() == this->GetWorldID()) {
                return;
            }
        }
    }

    if (this->mDoContrailEffect && mover != nullptr && mover->OutsidePOV() && (view->GetID() == 1 || view->GetID() == 2)) {
        const Attrib::Collection *cspec = Attrib::FindCollection(0x6F5943F1, 0x16AFDE7B);
        AddXenonEffect(0, cspec, reinterpret_cast<const UMath::Matrix4 *>(this->GetBodyMatrix()),
                       reinterpret_cast<const UMath::Vector4 *>(this->GetVelocity()));
    }

    if (view->GetID() == EVIEW_PLAYER1_RVM) {
        CameraAnchor *anchor = mover->GetAnchor();

        RVManchor = anchor;
        if (anchor != nullptr && anchor->GetWorldID() == this->GetWorldID()) {
            return;
        }
    }

    if (mover != nullptr && eIsGameViewID(view->GetID())) {
        this->mDistanceToView = UMath::Min(this->mDistanceToView, mover->GetDistanceTo(reinterpret_cast<bVector3 *>(&this->mRenderMatrix.v3)));
    }

    CarRenderInfo *car_render_info = this->GetRenderInfo();
    if (car_render_info == nullptr) {
        return;
    }

    EVIEWMODE view_mode = eGetCurrentViewMode();

    if (reflection == 0) {
        bMatrix4 render_matrix(this->mRenderMatrix);

        if (this->IsViewAnchor(view)) {
            if (view->GetCameraMover()->GetAnchor()->GetPOVType() == 1) {
                bCopy(&render_matrix, this->GetBodyMatrix());

                bVector4 rotOffset;
                bVector4 offset(this->GetModelOffset());
                eMulVector(&rotOffset, &render_matrix, &offset);
                render_matrix.v3.x -= rotOffset.x;
                render_matrix.v3.y -= rotOffset.y;
                render_matrix.v3.z -= rotOffset.z;
            }
        }

        int render_flags = 0;
        if (reflection != 0) {
            render_flags = 0x401;
            render_matrix.v2.x = -render_matrix.v2.x;
            render_matrix.v2.y = -render_matrix.v2.y;
            render_matrix.v2.z = -render_matrix.v2.z;
        }

        bVector3 Position = bVector3(render_matrix.v3.x, render_matrix.v3.y, render_matrix.v3.z);
        render_matrix.v3.x = 0.0f;
        render_matrix.v3.y = 0.0f;
        render_matrix.v3.z = 0.0f;

        if (this->GetFlag(CF_ISPLAYER) && NumTimesRenderTestPlayerCar != 0) {
            for (int i = 0; i < NumTimesRenderTestPlayerCar; i++) {
                if (car_render_info->Render(view, &Position, &render_matrix, this->mTireMatrices, this->mBrakeMatrices, this->mTireMatrices,
                                            render_flags, 0, reflection, static_cast<float>(static_cast<int>(car_render_info->GetMinLodLevel())),
                                            car_render_info->GetMinLodLevel(), static_cast<CARPART_LOD>(0)) &&
                    view->GetID() < EVIEW_PLAYER1_GLOW) {
                    this->mLastVisibleFrame = eGetFrameCounter();
                }
            }
        } else {
            if (car_render_info->Render(view, &Position, &render_matrix, this->mTireMatrices, this->mBrakeMatrices, this->mTireMatrices, render_flags,
                                        0, reflection, 1.0f, car_render_info->GetMinLodLevel(), car_render_info->GetMinLodLevel()) &&
                view->GetID() < 4) {
                this->mLastVisibleFrame = eGetFrameCounter();
            }
        }
    }
}
