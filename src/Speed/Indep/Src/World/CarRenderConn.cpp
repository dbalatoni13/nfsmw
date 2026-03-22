#include "Speed/Indep/Src/World/CarRenderConn.h"

struct CarPartDatabase;
extern void *gINISInstance asm("_Q33UTL11Collectionst9Singleton1Z4INIS_mInstance");
extern CarPartDatabase CarPartDB;
extern CarType GetCarType__15CarPartDatabaseUi(CarPartDatabase *database, unsigned int model_hash)
    asm("GetCarType__15CarPartDatabaseUi");
extern void KillSkids__9TireState(TireState *state) asm("KillSkids__9TireState");

namespace {

struct RenderPktCarOpen {
    void *vtable;
    unsigned int mModelHash;
};

void StopEffect(VehicleRenderConn::Effect *effect) {
    effect->Stop();
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

void CarRenderConn::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

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

void CarRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&this->mRenderMatrix), *reinterpret_cast<Mtx44 *>(matrix));
}
