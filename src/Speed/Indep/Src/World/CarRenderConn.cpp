#include "Speed/Indep/Src/World/CarRenderConn.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"

struct CarPartDatabase;
extern void *gINISInstance asm("_Q33UTL11Collectionst9Singleton1Z4INIS_mInstance");
extern CarPartDatabase CarPartDB;
extern CarType GetCarType__15CarPartDatabaseUi(CarPartDatabase *database, unsigned int model_hash)
    asm("GetCarType__15CarPartDatabaseUi");
extern int GetAppliedAttributeIParam__7CarPartUii(const CarPart *part, unsigned int key, int default_value)
    asm("GetAppliedAttributeIParam__7CarPartUii");
extern void KillSkids__9TireState(TireState *state) asm("KillSkids__9TireState");
extern void TireState_ctor(TireState *state) asm("__9TireState");
extern void TireState_dtor(TireState *state, int in_chrg) asm("_._9TireState");
extern int PhysicsUpgrades_GetLevel(const Attrib::Gen::pvehicle &pvehicle, int type)
    asm("GetLevel__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type");
extern int PhysicsUpgrades_GetMaxLevel(const Attrib::Gen::pvehicle &pvehicle, int type)
    asm("GetMaxLevel__Q27Physics8UpgradesRCQ36Attrib3Gen8pvehicleQ37Physics8Upgrades4Type");
extern float RealTimeElapsed;
extern float renderModifier;
extern int Tweak_DisableRoadNoise;
extern RoadNoiseRecord Tweak_BlowOutNoise asm("Tweak_BlowOutNoise");

namespace {

struct RenderPktCarOpen {
    void *vtable;
    unsigned int mModelHash;
};

struct TireStateRoadNoiseMirror {
    unsigned char _pad0[0x84];
    Attrib::Gen::simsurface mSurface;
};

void StopEffect(VehicleRenderConn::Effect *effect) {
    effect->Stop();
}

TireState *CreateTireState() {
    TireState *state = reinterpret_cast<TireState *>(gFastMem.Alloc(0xe0, 0));

    TireState_ctor(state);
    return state;
}

float &CarRenderInfoF32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(info) + offset);
}

unsigned int &CarRenderInfoU32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(info) + offset);
}

int &CarRenderInfoI32(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(info) + offset);
}

short &CarRenderInfoS16(CarRenderInfo *info, unsigned int offset) {
    return *reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(info) + offset);
}

} // namespace

Sim::Connection *CarRenderConn::Construct(const Sim::ConnectionData &data) {
    const RenderPktCarOpen *open = reinterpret_cast<const RenderPktCarOpen *>(data.pkt);
    int car_type = GetCarType__15CarPartDatabaseUi(&CarPartDB, open->mModelHash);

    if (car_type == -1 || car_type > 0x53) {
        return 0;
    }

    return new CarRenderConn(data, static_cast<CarType>(car_type), reinterpret_cast<RenderConn::Pkt_Car_Open *>(data.pkt));
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
    int i;

    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&this->mRenderMatrix));
    this->mPhysics.Change(oc->mPhysicsKey);
    this->mTirePhysics.Change(this->mPhysics.tires(0));
    this->mSteering[0] = 0.0f;
    this->mSteering[1] = 0.0f;

    for (i = 0; i < 3; i++) {
        this->mPartState[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        this->mTireState[i] = CreateTireState();
        this->mTirePositions[i] = this->VehicleRenderConn::mAttributes.TireOffsets(i);
        this->mTireRadius[i] = this->mTirePositions[i].w;
        if (this->mTireRadius[i] < 0.1f) {
            this->mTireRadius[i] = 0.1f;
        }

        this->mTirePositions[i].w = 0.0f;
        this->mPhysicsRadius[i] = Physics::Info::WheelDiameter(this->mTirePhysics, i < 2) * 0.5f;
    }

    this->mMaxWheelRenderDeltaAngle = 0.017453f;
    this->Load(oc->mWorldID, oc->mUsage, !oc->mSpoolLoad, oc->mCustomizations);
    this->SetFlag(CF_ISPLAYER, oc->mUsage == 0);

    if ((this->mUsage == 0 || this->mUsage == 2) &&
        (PhysicsUpgrades_GetLevel(this->mPhysics, 4) != 0 || PhysicsUpgrades_GetMaxLevel(this->mPhysics, 4) == 0)) {
        this->SetFlag(CF_BLOWOFF, true);
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
    if (gINISInstance == 0 && !this->IsViewAnchor()) {
        bool visible = false;

        if (this->mLastRenderFrame <= this->mLastVisibleFrame && this->mLastVisibleFrame != 0) {
            visible = true;
        }

        if (visible) {
            return this->mDistanceToView <= distance;
        }

        return false;
    }

    return true;
}

void CarRenderConn::OnEvent(EventID id) {
    if (id == E_UPSHIFT) {
        this->mShifting = 1.0f;
        return;
    }

    if (id < E_DOWNSHIFT) {
        if (id == E_MISS_SHIFT) {
            this->mFlags |= CF_MISSSHIFT;
        }
        return;
    }

    if (id == E_DOWNSHIFT) {
        this->mShifting = -1.0f;
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
    bool changed = false;
    int i;

    for (i = 0; i < 3; i++) {
        if (this->mPartState[i] != data.mPartState[i]) {
            changed = true;
            break;
        }
    }

    if (changed) {
        unsigned int part_id = 0;

        while (part_id < 0x4c) {
            unsigned int word_index = part_id >> 5;
            unsigned int shift = part_id & 0x1f;
            bool hide_part = ((data.mPartState[word_index] >> shift) & 1) != 0;

            if (hide_part != (((this->mPartState[word_index] >> shift) & 1) != 0)) {
                if (hide_part) {
                    this->HidePart(static_cast<CAR_PART_ID>(part_id));
                } else {
                    this->ShowPart(static_cast<CAR_PART_ID>(part_id));
                }
            }

            part_id++;
        }

        for (i = 0; i < 3; i++) {
            this->mPartState[i] = data.mPartState[i];
        }
    }
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
    this->mRoadNoise.z = 0.0f;
    this->mRoadNoise.x = 0.0f;
    this->mRoadNoise.y = 0.0f;

    if (this->TestVisibility(renderModifier * 30.0f) && Tweak_DisableRoadNoise == 0) {
        RoadNoiseRecord road_noise = {0.0f, 0.0f, 0.0f, 0.0f};

        for (unsigned int i = 0; i < 4; i++) {
            if (((data.mGroundState >> i) & 1U) != 0) {
                const RoadNoiseRecord &tire_noise =
                    reinterpret_cast<TireStateRoadNoiseMirror *>(this->mTireState[i])->mSurface.RenderNoise();

                if (road_noise.Frequency * road_noise.Amplitude < tire_noise.Frequency * tire_noise.Amplitude) {
                    road_noise = tire_noise;
                }
            }
        }

        this->AddRoadNoise(carspeed, static_cast<unsigned int>(data.mGroundState), road_noise);
        this->AddRoadNoise(carspeed, static_cast<unsigned int>(data.mGroundState & data.mBlowOuts), Tweak_BlowOutNoise);

        float pitch = sinf(this->mRoadNoise.y);
        float roll = sinf(this->mRoadNoise.x);
        float body_noise = 0.0f;
        float candidate = this->mTirePositions[0].x * pitch;

        if (body_noise < candidate) {
            body_noise = candidate;
        }

        candidate = this->mTirePositions[3].x * pitch;
        if (body_noise < candidate) {
            body_noise = candidate;
        }

        candidate = this->mTirePositions[0].y * roll;
        if (body_noise < candidate) {
            body_noise = candidate;
        }

        candidate = this->mTirePositions[1].y * roll;
        if (body_noise < candidate) {
            body_noise = candidate;
        }

        this->mRoadNoise.z = body_noise;
    }
}

void CarRenderConn::Update(const RenderConn::Pkt_Car_Service &data, float dT) {
    if (this->CanUpdate() && this->mRenderInfo != 0) {
        this->mRenderInfo->SetDamageInfo(*reinterpret_cast<const DamageZone::Info *>(&data.mDamageInfo));
        CarRenderInfoF32(this->mRenderInfo, 0x1754) = dT;
        CarRenderInfoU32(this->mRenderInfo, 0x1608) = data.mLights;
        CarRenderInfoU32(this->mRenderInfo, 0x160C) = data.mBrokenLights;
        CarRenderInfoI32(this->mRenderInfo, 0x1770) = data.mBlowOuts;
        if (data.mBlowOuts != 0) {
            float blown_timer = CarRenderInfoF32(this->mRenderInfo, 0x1774) + RealTimeElapsed;

            CarRenderInfoF32(this->mRenderInfo, 0x1774) = blown_timer;
            if (0.05f < blown_timer) {
                CarRenderInfoF32(this->mRenderInfo, 0x1774) = blown_timer - 0.12f;
            }
        }

        CarRenderInfoU32(this->mRenderInfo, 0x1170) = data.mNos ? 1 : 0;
        CarRenderInfoS16(this->mRenderInfo, 0x1174) = static_cast<short>(this->mSteering[0] * 10430.378f);
        CarRenderInfoS16(this->mRenderInfo, 0x1176) = static_cast<short>(this->mSteering[1] * 10430.378f);

        const bVector3 *velocity = this->mWorldRef.GetVelocity();
        float carspeed = bSqrt(velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z);

        this->mAnimTime += dT;
        if (10.0f <= this->mAnimTime) {
            this->mAnimTime -= 10.0f;
        }

        CarRenderInfoF32(this->mRenderInfo, 0x0) = this->mAnimTime;
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
    const bVector3 *velocity = this->mWorldRef.GetVelocity();
    float speed = bSqrt(velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z);

    if ((data.mNos || 44.0f <= speed) && gINISInstance == 0) {
        this->mDoContrailEffect = true;
        return;
    }

    this->mDoContrailEffect = false;
}

void CarRenderConn::Hide(bool b) {
    unsigned int flags = this->mFlags;

    if (((flags & CF_HIDDEN) != 0) != b) {
        if (b) {
            flags |= CF_HIDDEN;
        } else {
            flags &= ~CF_HIDDEN;
        }

        this->mFlags = flags;
        if (b) {
            this->mAnimTime = 0.0f;
            for (int i = 0; i < 4; i++) {
                KillSkids__9TireState(this->mTireState[i]);
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

    if ((this->mLastRenderFrame <= this->mLastVisibleFrame && this->mLastVisibleFrame != 0) || this->IsViewAnchor()) {
        in_view = true;
    }

    RenderConn::Pkt_Car_Service pkt(in_view, this->mDistanceToView);
    if (!this->Service(&pkt)) {
        this->Hide(true);
    } else {
        this->Hide(false);
        this->Update(pkt, dT);
    }
}

void CarRenderConn::OnLoaded(CarRenderInfo *carrender_info) {
    this->VehicleRenderConn::OnLoaded(carrender_info);
    if (carrender_info == 0) {
        return;
    }

    if (carrender_info->pRideInfo != 0) {
        CarPart *wheel = carrender_info->pRideInfo->GetPart(0x42);
        if (wheel != 0) {
            int num_spokes = GetAppliedAttributeIParam__7CarPartUii(wheel, 0x1b0ea1a9, 0);

            if (num_spokes < 0) {
                num_spokes = -num_spokes;
            }

            if (num_spokes == 0) {
                num_spokes = this->VehicleRenderConn::mAttributes.WheelSpokeCount();
                if (num_spokes < 0) {
                    num_spokes = -num_spokes;
                }
            }

            if (num_spokes != 0) {
                float spoke_count = static_cast<float>(num_spokes + num_spokes);
                this->mMaxWheelRenderDeltaAngle = (360.0f / spoke_count - 1.0f) * 0.017453f;
            }
        }
    }

    carrender_info->InitEmitterPositions(this->mTirePositions);

    if (this->mPipeEffects.IsEmpty()) {
        for (CarEmitterPosition *position = carrender_info->EmitterPositionList[10].GetHead();
             position != carrender_info->EmitterPositionList[10].EndOfList(); position = position->GetNext()) {
            bMatrix4 emitter_matrix;
            const bMatrix4 *effect_matrix = 0;

            if (position->PositionMarker == 0) {
                PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&emitter_matrix));
                emitter_matrix.v3.x = position->X;
                emitter_matrix.v3.y = position->Y;
                emitter_matrix.v3.z = position->Z;
                effect_matrix = &emitter_matrix;
            } else {
                effect_matrix = &position->PositionMarker->Matrix;
            }

            this->mPipeEffects.AddTail(new VehicleRenderConn::Effect(effect_matrix));
        }
    }

    if (this->mEngineEffects.IsEmpty()) {
        for (CarEmitterPosition *position = carrender_info->EmitterPositionList[9].GetHead();
             position != carrender_info->EmitterPositionList[9].EndOfList(); position = position->GetNext()) {
            bMatrix4 emitter_matrix;
            const bMatrix4 *effect_matrix = 0;

            if (position->PositionMarker == 0) {
                PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&emitter_matrix));
                emitter_matrix.v3.x = position->X;
                emitter_matrix.v3.y = position->Y;
                emitter_matrix.v3.z = position->Z;
                effect_matrix = &emitter_matrix;
            } else {
                effect_matrix = &position->PositionMarker->Matrix;
            }

            this->mEngineEffects.AddTail(new VehicleRenderConn::Effect(effect_matrix));
        }
    }
}

void CarRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&this->mRenderMatrix), *reinterpret_cast<Mtx44 *>(matrix));
}
