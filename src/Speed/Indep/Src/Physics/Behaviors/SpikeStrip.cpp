#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

// total size: 0x50
class SpikeStrip : public Behavior {
  public:
    USE_FASTALLOC(Behavior);

    SpikeStrip(const BehaviorParams &params);

    // Overrides: Behavior
    void Reset() override {}
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    static Behavior *Construct(const BehaviorParams &params) {
        return new SpikeStrip(params);
    }

    void OnCollide(const Dynamics::Collision::Geometry &mygeometry, IRigidBody *irb, float dT);
    void SetupBody();

    ISimpleBody *mBody; // offset 0x4C, size 0x4
};

BIND_BEHAVIOR_FACTORY(SpikeStrip);

SpikeStrip::SpikeStrip(const BehaviorParams &params) : Behavior(params, 0) {
    if (this->GetOwner()->QueryInterface(&mBody)) {
        this->SetupBody();
    }
}

void SpikeStrip::OnBehaviorChange(const UCrc32 &mechanic) {
    Behavior::OnBehaviorChange(mechanic);
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        if (this->GetOwner()->QueryInterface(&mBody)) {
            this->SetupBody();
        }
    }
}

void SpikeStrip::SetupBody() {
    mBody->ModifyFlags(0x20B, 0x8001);
}

void SpikeStrip::OnCollide(const Dynamics::Collision::Geometry &mygeometry, IRigidBody *irb, float dT) {
    ISpikeable *ispikeable;

    if (!irb->QueryInterface(&ispikeable)) {
        return;
    }

    ISuspension *isuspension;
    if (!irb->QueryInterface(&isuspension)) {
        return;
    }

    unsigned int num_tires = isuspension->GetNumWheels();

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

            Dynamics::Collision::Geometry tirebox(matrix, position, dim, Dynamics::Collision::Geometry::BOX, dP);

            if (Dynamics::Collision::Geometry::FindIntersection(&mygeometry, &tirebox, &tirebox)) {
                ispikeable->Puncture(i);
            }
        }
    }
}

void SpikeStrip::OnTaskSimulate(float dT) {
    if (this->mBody == nullptr) {
        return;
    }

    IModel *model = this->GetOwner()->GetModel();
    if (model == nullptr) {
        return;
    }

    const CollisionGeometry::Bounds *geometry = model->GetCollisionGeometry();
    if (geometry == nullptr) {
        return;
    }

    if (this->mBody != nullptr) {
        SimCollisionMap *cmap = this->mBody->GetCollisionMap();

        if (cmap != nullptr && cmap->CollisionWithAny()) {
            UMath::Matrix4 matrix;
            UMath::Vector3 position;
            UMath::Vector3 dim;

            model->GetTransform(matrix);
            position = UMath::Vector4To3(matrix.v3);
            matrix.v3 = UMath::Vector4::kIdentity;
            geometry->GetHalfDimensions(dim);

            Dynamics::Collision::Geometry mygeometry(matrix, position, dim, Dynamics::Collision::Geometry::BOX, UMath::Vector3::kZero);

            for (int i = 0; i < Sim::MaxAnyBodies; i++) {
                if (cmap->CollisionWithOrderedBody(i)) {
                    IRigidBody *irb = cmap->GetOrderedBody(i);

                    if (irb != nullptr) {
                        this->OnCollide(mygeometry, irb, dT);
                    }
                }
            }
        } else {
            ITriggerableModel *itrigger;

            if (model->QueryInterface(&itrigger)) {
                UMath::Matrix4 mat;

                this->GetOwner()->GetTransform(mat);
                itrigger->PlaceTrigger(mat, true);
                this->GetOwner()->Kill();
            }
        }
    }
}
