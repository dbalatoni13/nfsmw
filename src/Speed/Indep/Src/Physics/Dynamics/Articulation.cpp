// d:/mw/speed/indep/src/Physics/dynamics/Articulation.cpp

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Physics/Dynamics/Articulation.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"


namespace Dynamics {
namespace Articulation {

inline UVector3 operator*(const UVector3 &v1, const UMath::Matrix4 &m) {
    UMath::Vector3 result;
    UMath::Rotate(v1, m, result);
    return UVector3(result);
}

// total size: 0x8
class JoinList : public bTList<Joint> {
  public:
    Joint *Find(HJOINT hjoint) {
        for (Joint *joint = GetHead(); joint != EndOfList(); joint = joint->GetNext()) {
            if (joint->mHandle == hjoint) {
                return joint;
            }
        }
        return 0;
    }
};

static JoinList Joints;

HJOINT Joint::mNextHandle = reinterpret_cast<HJOINT>(1);

// d:/mw/speed/indep/src/Physics/dynamics/Articulation.cpp
void Constrain(HJOINT hJoint, IEntity *female, const UMath::Matrix4 &orient, float minTheta, float maxTheta, const UMath::Vector3 &post, eConstraint type) {
    Joint *joint = Joints.Find(hJoint);
    if (joint != 0) {
        joint->AddConstraint(female, orient, minTheta, maxTheta, post, type);
    }
}

bool IsJoined(const IEntity *rb0, const IEntity *rb1) {
    for (Joint *joint = Joints.GetHead(); joint != Joints.EndOfList(); joint = joint->GetNext()) {
        if (joint->Owns(rb0) && joint->Owns(rb1)) {
            return true;
        }
    }
    return false;
}

bool IsJoined(const IEntity *rb) {
    for (Joint *joint = Joints.GetHead(); joint != Joints.EndOfList(); joint = joint->GetNext()) {
        if (joint->Owns(rb)) {
            return true;
        }
    }
    return false;
}

void Resolve() {
    for (Joint *joint = Joints.GetHead(); joint != Joints.EndOfList(); joint = joint->GetNext()) {
        joint->Resolve();
    }
}

HJOINT Create(IEntity *female, const UMath::Vector3 &female_arm, IEntity *male, const UMath::Vector3 &male_arm, eJointFlags flags) {
    Release(female);
    Release(male);
    Joint *joint = new Joint(female, female_arm, male, male_arm, flags);
    Joints.AddTail(joint);
    return joint->mHandle;
}

void Release(IEntity *rb0) {
    for (Joint *joint = Joints.GetHead(); joint != Joints.EndOfList();) {
        Joint *next = joint->GetNext();
        if (joint->Owns(rb0)) {
            Joints.Remove(joint);
            delete joint;
        }
        joint = next;
    }
}

Joint::Joint(IEntity *female, const UMath::Vector3 &female_arm, IEntity *male, const UMath::Vector3 &male_arm, eJointFlags flags)
    : mFemale(female, female_arm, (flags & JF_IMMOBILE_FEMALE) != 0), mMale(male, male_arm, (flags & JF_IMMOBILE_MALE) ? true : false),
      mHandle(mNextHandle++) {
}

void Joint::OnDebugDraw() {}

Joint::~Joint() {
    for (Constraint::List::iterator constraint = mConstraints.begin(); constraint != mConstraints.end(); constraint++) {
        delete *constraint;
    }
}

bool Joint::Owns(const IEntity *rb) const {
    if (rb == mFemale.mEntity || rb == mMale.mEntity) {
        return true;
    }
    return false;
}

void Joint::Resolve() {
    IEntity *female = mFemale.GetEntity();
    IEntity *male = mMale.GetEntity();
    if (female == 0 || male == 0) {
        return;
    }
    if (mFemale.IsImmobile() && mMale.IsImmobile()) {
        return;
    }

    UVector3 pos0 = UVector3(mFemale.GetArm()) * female->GetRotation() + UVector3(female->GetPosition());
    UVector3 pos1 = UVector3(mMale.GetArm()) * male->GetRotation() + UVector3(male->GetPosition());

    float m0;
    float m1;
    if (mMale.IsImmobile()) {
        m0 = 0.0f;
    } else if (mFemale.IsImmobile()) {
        m0 = 1.0f;
    } else {
        m0 = female->GetMass() / (female->GetMass() + male->GetMass());
    }

    m1 = 1.0f - m0;
    UVector3 fulcrum = pos0 * m0 + pos1 * m1;

    mFemale.SetFulcrum(fulcrum, !mFemale.mImmoblie);
    mMale.SetFulcrum(fulcrum, !mMale.mImmoblie);

    Collision::Joint joint;
    joint.p = fulcrum;

    if (mMale.IsImmobile()) {
        Collision::Moment mo(female);
        if (mo.React(joint, 0x20)) {
            female->SetLinearVelocity(mo.GetLinearVelocity());
            female->SetAngularVelocity(mo.GetAngularVelocity());
        }
    } else if (mFemale.IsImmobile()) {
        Collision::Moment mo(male);
        if (mo.React(joint, 0x20)) {
            male->SetLinearVelocity(mo.GetLinearVelocity());
            male->SetAngularVelocity(mo.GetAngularVelocity());
        }
    } else {
        Collision::Moment mo0(female);
        Collision::Moment mo1(male);
        if (mo0.React(mo1, joint, 0x20)) {
            female->SetLinearVelocity(mo0.GetLinearVelocity());
            female->SetAngularVelocity(mo0.GetAngularVelocity());
            male->SetLinearVelocity(mo1.GetLinearVelocity());
            male->SetAngularVelocity(mo1.GetAngularVelocity());
        }
    }

    for (Constraint::List::iterator iter = mConstraints.begin(); iter != mConstraints.end(); iter++) {
        (*iter)->Resolve(fulcrum);
    }
}

void Lever::SetFulcrum(const UVector3 &fulcrum, bool rotate) {
    if (rotate) {
        UVector3 arm = mArm * mEntity->GetRotation();
        if (arm.Normalize() > 0.0f) {
            UVector3 arm2 = fulcrum - mEntity->GetPosition();
            if (arm2.Normalize() != 0.0f) {
                Quaternion q(arm, arm2);
                Quaternion q2;
                UMath::Mult(*reinterpret_cast<UMath::Vector4 *>(&q), mEntity->GetOrientation(), *reinterpret_cast<UMath::Vector4 *>(&q2));
                mEntity->SetOrientation(*reinterpret_cast<UMath::Vector4 *>(&q2));
            }
        }
    }
    mEntity->SetPosition(fulcrum - mArm * mEntity->GetRotation());
}

void Lever::OnDebugDraw() {}

void Joint::AddConstraint(IEntity *female, const UMath::Matrix4 &orient, float minTheta, float maxTheta, const UMath::Vector3 &post, eConstraint type) {
    Lever *fem = &mFemale;
    if (female == mFemale.GetEntity()) {
        Constraint *constraint = new Constraint(orient, minTheta, maxTheta, *fem, mMale, post, type);
        mConstraints.push_back(constraint);
    } else {
        Constraint *constraint = new Constraint(orient, minTheta, maxTheta, mMale, *fem, post, type);
        mConstraints.push_back(constraint);
    }
}

Constraint::Constraint(const UMath::Matrix4 &orient, float minTheta, float maxTheta, Lever &female, Lever &male, const UMath::Vector3 &post, eConstraint type)
    : mFemale(female), mMale(male), mPost(post), mType(type), mEnabled(true), mOuterA(DEG2ANGLE(maxTheta)), mInnerA(DEG2ANGLE(minTheta)) {
    UMath::Matrix4ToQuaternion(orient, mOrientation);
    float radius = UMath::Length(post);
    mInnerN.x = -UMath::Cosa(mInnerA);
    mInnerN.y = 0.0f;
    mInnerN.z = UMath::Sina(mInnerA);
    mInnerR.x = UMath::Sina(mInnerA) * radius;
    mInnerR.y = 0.0f;
    mInnerR.z = UMath::Cosa(mInnerA) * radius;
    mOuterN.x = UMath::Cosa(mOuterA);
    mOuterN.y = 0.0f;
    mOuterN.z = UMath::Sina(mOuterA);
    mOuterR.x = -UMath::Sina(mOuterA) * radius;
    mOuterR.y = 0.0f;
    mOuterR.z = UMath::Cosa(mOuterA) * radius;
}

void Constraint::Resolve(const UVector3 &joint_fulcrum) {
    if (!mEnabled) {
        return;
    }

    IEntity *female = mFemale.GetEntity();
    IEntity *male = mMale.GetEntity();

    UVector3 opp_point = (mPost + UVector3(mMale.GetArm())) * male->GetRotation() + UVector3(male->GetPosition());
    opp_point -= joint_fulcrum;

    UMath::Vector4 joint_rotation;
    UMath::Vector4 joint_rotationInv;
    const UMath::Vector4 &female_orient = female->GetOrientation();
    UMath::Mult(female_orient, mOrientation, joint_rotation);
    UMath::Transpose(joint_rotation, joint_rotationInv);

    if (mType != PRISMATIC) {
        UVector3 pt;
        UMath::Rotate(opp_point, joint_rotationInv, pt);
        float angle_pitch = -UMath::Atan2a(pt.y, pt.z);
        Quaternion pitch(UVector3(1.0f, 0.0f, 0.0f), angle_pitch);

        if (mType == CONICAL) {
            float angle_roll = UMath::Atan2a(pt.y, pt.x);
            Quaternion roll(UVector3(0.0f, 0.0f, 1.0f), angle_roll);
            UMath::Mult(*reinterpret_cast<UMath::Vector4 *>(&roll), *reinterpret_cast<UMath::Vector4 *>(&pitch), *reinterpret_cast<UMath::Vector4 *>(&pitch));
        }

        Quaternion m;
        UMath::Mult(mOrientation, *reinterpret_cast<UMath::Vector4 *>(&pitch), *reinterpret_cast<UMath::Vector4 *>(&m));
        UMath::Mult(female_orient, *reinterpret_cast<UMath::Vector4 *>(&m), joint_rotation);
        UMath::Transpose(joint_rotation, joint_rotationInv);
    }

    UMath::Rotate(opp_point, joint_rotationInv, opp_point);
    float ang = VU0_Atan2(opp_point.x, opp_point.z);
    if (ang > mInnerA) {
        React(mInnerN, mInnerR, joint_fulcrum, mInnerA - ang, *reinterpret_cast<Quaternion *>(&joint_rotation),
              *reinterpret_cast<Quaternion *>(&joint_rotationInv));
    } else if (ang < -mOuterA) {
        React(mOuterN, mOuterR, joint_fulcrum, -mOuterA - ang, *reinterpret_cast<Quaternion *>(&joint_rotation),
              *reinterpret_cast<Quaternion *>(&joint_rotationInv));
    }
}

void Constraint::Rotate(Lever &lever, const Quaternion &joint_rotation, const Quaternion &joint_rotationInv, const UVector3 &joint_fulcrum, float angle) {
    UMath::Vector4 qy;
    const UVector3 axis(0.0f, 1.0f, 0.0f);
    const UMath::Vector3 &rotation_axis = axis;
    float a = angle * 0.5f;
    IEntity *entity = lever.mEntity;
    float s = VU0_Sin(a * 6.28318548f);
    qy.w = VU0_Cos(a * 6.28318548f);
    UMath::Scale(rotation_axis, s, UMath::Vector4To3(qy));

    const UMath::Vector4 &orient = entity->GetOrientation();
    UMath::Vector4 temp;
    UMath::Vector4 dest;
    UMath::Mult(*reinterpret_cast<const UMath::Vector4 *>(&joint_rotationInv), orient, dest);
    UMath::Mult(qy, dest, temp);
    UMath::Mult(*reinterpret_cast<const UMath::Vector4 *>(&joint_rotation), temp, dest);
    UMath::Unit(dest, dest);
    entity->SetOrientation(dest);
    lever.SetFulcrum(joint_fulcrum, false);
}

void Constraint::OnDebugDraw() {}

bool Constraint::React(const UVector3 &joint_collision_normal, const UVector3 &joint_collision_point, const UVector3 &joint_fulcrum, const float deltaA,
                       const Quaternion &joint_rotation, const Quaternion &joint_rotationInv) {
    IEntity *female = mFemale.GetEntity();
    IEntity *male = mMale.GetEntity();

    if (mFemale.mImmoblie) {
        Rotate(mMale, joint_rotation, joint_rotationInv, joint_fulcrum, deltaA);
        UVector3 av(male->GetAngularVelocity());
        UMath::Rotate(av, *reinterpret_cast<const UMath::Vector4 *>(&joint_rotationInv), av);
        if (av.y * deltaA < 0.0f) {
            av.y = 0.0f;
            UMath::Rotate(av, *reinterpret_cast<const UMath::Vector4 *>(&joint_rotation), av);
            male->SetAngularVelocity(av);
        }
    } else if (mMale.mImmoblie) {
        Rotate(mFemale, joint_rotation, joint_rotationInv, joint_fulcrum, -deltaA);
        UVector3 av(female->GetAngularVelocity());
        UMath::Rotate(av, *reinterpret_cast<const UMath::Vector4 *>(&joint_rotationInv), av);
        if (av.y * deltaA > 0.0f) {
            av.y = 0.0f;
            UMath::Rotate(av, *reinterpret_cast<const UMath::Vector4 *>(&joint_rotation), av);
            female->SetAngularVelocity(av);
        }
    } else {
        float weight_opp = female->GetMass() / (female->GetMass() + male->GetMass());
        float weight_anch = weight_opp - 1.0f;
        Rotate(mFemale, joint_rotation, joint_rotationInv, joint_fulcrum, weight_anch * deltaA);
        Rotate(mMale, joint_rotation, joint_rotationInv, joint_fulcrum, weight_opp * deltaA);

        Collision::Plane plane;
        plane.normal = joint_collision_normal;
        UMath::Rotate(plane.normal, *reinterpret_cast<const UMath::Vector4 *>(&joint_rotation), plane.normal);
        VU0_v3quatrotate_xlate(*reinterpret_cast<const UMath::Vector4 *>(&joint_rotation), joint_collision_point, joint_fulcrum, plane.point);

        Collision::Moment mo_female(female);
        mo_female.MakeImmobile(mFemale.mImmoblie, 0.0f);
        Collision::Moment mo_male(male);
        mo_male.MakeImmobile(mMale.mImmoblie, 0.0f);
        if (mo_male.React(mo_female, plane, 0x20)) {
            female->SetLinearVelocity(mo_female.GetLinearVelocity());
            female->SetAngularVelocity(mo_female.GetAngularVelocity());
            male->SetLinearVelocity(mo_male.GetLinearVelocity());
            male->SetAngularVelocity(mo_male.GetAngularVelocity());
        }
    }
    return true;
}

}; // namespace Articulation
}; // namespace Dynamics
