#include "Speed/Indep/Src/Physics/Behaviors/SpikeStrip.h"

#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"

Behavior *SpikeStrip::Construct(const BehaviorParams &params) {
    return new SpikeStrip(params);
}

SpikeStrip::SpikeStrip(const BehaviorParams &params)
    : Behavior(params, 0) {
    GetOwner()->QueryInterface(&mBody);

    if (mBody) {
        SetupBody();
    }
}

SpikeStrip::~SpikeStrip() {}

void SpikeStrip::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        GetOwner()->QueryInterface(&mBody);

        if (mBody) {
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

    if (irb->QueryInterface(&ispikeable) && irb->QueryInterface(&isuspension)) {
        num_tires = isuspension->GetNumWheels();

        for (unsigned int i = 0; i < num_tires; i++) {
            if (isuspension->IsWheelOnGround(i) && ispikeable->GetTireDamage(i) == TIRE_DAMAGE_NONE) {
                UMath::Vector3 dP;
                UMath::Matrix4 matrix;
                UMath::Vector3 position;
                UMath::Vector3 dim;
                Dynamics::Collision::Geometry tirebox;

                UMath::Scale(irb->GetLinearVelocity(), dT, dP);
                irb->GetMatrix4(matrix);
                position = isuspension->GetWheelCenterPos(i);

                {
                    const float radius = isuspension->GetWheelRadius(i);

                    dim.x = 0.15f;
                    dim.y = radius;
                    dim.z = radius;
                }

                tirebox = Dynamics::Collision::Geometry(
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
}

void SpikeStrip::OnTaskSimulate(float dT) {
    IModel *model;
    const CollisionGeometry::Bounds *geometry;

    if (mBody) {
        model = GetOwner()->GetModel();

        if (model) {
            geometry = model->GetCollisionGeometry();

            if (geometry) {
                SimCollisionMap *cmap = mBody->GetCollisionMap();

                if (cmap) {
                    if (cmap->CollisionWithAny()) {
                        UMath::Matrix4 matrix;
                        UMath::Vector3 position;
                        UMath::Vector3 dim;
                        Dynamics::Collision::Geometry mygeometry;

                        model->GetTransform(matrix);
                        position = UMath::Vector4To3(matrix.v3);
                        matrix.v3 = UMath::Vector4::kIdentity;
                        geometry->GetHalfDimensions(dim);
                        mygeometry = Dynamics::Collision::Geometry(
                            matrix, //
                            position, //
                            dim, //
                            Dynamics::Collision::Geometry::BOX, //
                            UMath::Vector3::kZero
                        );

                        for (int i = 0; i < 0xA0; i++) {
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
                            GetOwner()->Detach(model);
                        }
                    }
                }
            }
        }
    }
}

void SpikeStrip::Reset() {}
