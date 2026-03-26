#include "Speed/Indep/Src/Physics/Behaviors/SpikeStrip.h"

#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"

static inline ISimpleBody *FindSimpleBody(ISimable *owner) {
    return reinterpret_cast<ISimpleBody *>(
        (*reinterpret_cast<UTL::COM::Object **>(owner))->_mInterfaces.Find((HINTERFACE)ISimpleBody::_IHandle)
    );
}

Behavior *SpikeStrip::Construct(const BehaviorParams &params) {
    return new SpikeStrip(params);
}

SpikeStrip::SpikeStrip(const BehaviorParams &params)
    : Behavior(params, 0) {
    bool hasbody;

    mBody = FindSimpleBody(GetOwner());
    hasbody = mBody != nullptr;

    if (hasbody) {
        SetupBody();
    }
}

SpikeStrip::~SpikeStrip() {}

void SpikeStrip::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        bool hasbody;

        mBody = FindSimpleBody(GetOwner());
        hasbody = mBody != nullptr;

        if (hasbody) {
            SetupBody();
        }
    }

    Behavior::OnBehaviorChange(mechanic);
}

void SpikeStrip::SetupBody() {
    mBody->ModifyFlags(0x20B, 0x8001);
}

void SpikeStrip::OnCollide(const Dynamics::Collision::Geometry &mygeometry, IRigidBody *irb, float dT) {
    ISpikeable *ispikeable;
    ISuspension *isuspension;
    unsigned int num_tires;

    if (!irb->QueryInterface(&ispikeable)) {
        return;
    }

    if (!irb->QueryInterface(&isuspension)) {
        return;
    }

    num_tires = isuspension->GetNumWheels();

    for (unsigned int i = 0; i < num_tires; i++) {
        if (isuspension->IsWheelOnGround(i) && ispikeable->GetTireDamage(i) == TIRE_DAMAGE_NONE) {
            UMath::Vector3 dP;
            UMath::Matrix4 matrix;

            UMath::Scale(irb->GetLinearVelocity(), dT, dP);
            irb->GetMatrix4(matrix);

            UMath::Vector3 position = isuspension->GetWheelCenterPos(i);
            UMath::Vector3 dim;

            dim.y = isuspension->GetWheelRadius(i);
            dim.z = dim.y;
            dim.x = 0.15f;

            Dynamics::Collision::Geometry tirebox(
                matrix, //
                position, //
                dim, //
                Dynamics::Collision::Geometry::BOX, //
                dP
            );
            if (Dynamics::Collision::Geometry::FindIntersection(&mygeometry, &tirebox, &tirebox)) {
                ispikeable->Puncture(i);
            }
        }
    }
}

void SpikeStrip::OnTaskSimulate(float dT) {
    IModel *model;
    const CollisionGeometry::Bounds *geometry;

    if (!mBody) {
        return;
    }

    model = GetOwner()->GetModel();
    if (!model) {
        return;
    }

    geometry = model->GetCollisionGeometry();
    if (!geometry) {
        return;
    }

    if (!mBody) {
        return;
    }

    {
        SimCollisionMap *cmap = mBody->GetCollisionMap();

        if (cmap && cmap->CollisionWithAny()) {
            UMath::Matrix4 matrix;
            UMath::Vector3 position;
            UMath::Vector3 dim;

            model->GetTransform(matrix);
            position = UMath::Vector4To3(matrix.v3);
            matrix.v3 = UMath::Vector4::kIdentity;
            geometry->GetHalfDimensions(dim);

            Dynamics::Collision::Geometry mygeometry(
                matrix, //
                position, //
                dim, //
                Dynamics::Collision::Geometry::BOX, //
                UMath::Vector3::kZero
            );

            for (int i = 0; static_cast<unsigned int>(i) < 0xA0; i++) {
                if (cmap->CollisionWithOrderedBody(i)) {
                    IRigidBody *irb = cmap->GetOrderedBody(i);

                    if (irb) {
                        OnCollide(mygeometry, irb, dT);
                    }
                }
            }
        } else {
            ITriggerableModel *itrigger;

            if (model->QueryInterface(&itrigger)) {
                UMath::Matrix4 mat;

                GetOwner()->GetTransform(mat);
                itrigger->PlaceTrigger(mat, true);
                GetOwner()->Kill();
            }
        }
    }
}

void SpikeStrip::Reset() {}
