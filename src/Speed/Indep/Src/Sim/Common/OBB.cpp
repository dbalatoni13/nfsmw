#include "../OBB.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

OBB::OBB() {}

OBB::~OBB() {}

void OBB::Reset(const UMath::Matrix4 &orient, const UMath::Vector3 &position, const UMath::Vector3 &dimension) {
    shape = BOX;
    *reinterpret_cast<UMath::Vector3 *>(&this->position) = position;

    this->dimension[0] = dimension.x;
    this->dimension[1] = dimension.y;
    this->dimension[2] = dimension.z;

    this->normal[0].x = orient[0][0];
    this->normal[0].y = orient[0][1];
    this->normal[0].z = orient[0][2];

    this->normal[1].x = orient[1][0];
    this->normal[1].y = orient[1][1];
    this->normal[1].z = orient[1][2];

    this->normal[2].x = orient[2][0];
    this->normal[2].y = orient[2][1];
    this->normal[2].z = orient[2][2];

    this->extent[0].x = this->normal[0].x * this->dimension[0];
    this->extent[0].y = this->normal[0].y * this->dimension[0];
    this->extent[0].z = this->normal[0].z * this->dimension[0];

    this->extent[1].x = this->normal[1].x * this->dimension[1];
    this->extent[1].y = this->normal[1].y * this->dimension[1];
    this->extent[1].z = this->normal[1].z * this->dimension[1];

    this->extent[2].x = this->normal[2].x * this->dimension[2];
    this->extent[2].y = this->normal[2].y * this->dimension[2];
    this->extent[2].z = this->normal[2].z * this->dimension[2];

    collision_normal.w = 0.0f;
    collision_point.w = 0.0f;
    penetration_depth = -100000.0f;
}

// UNSOLVED but sould be functionally equivalent
bool OBB::CheckOBBOverlap(OBB *other) {
    if (this->shape != BOX || other->shape != BOX) {
        return CheckOBBOverlapAndFindIntersection(other);
    }

    UMath::Vector4 rel_position;
    UMath::Vector4 a_normal;
    UMath::Vector4 *b_extent;
    float projected_interval;
    float b_projected_interval;
    OBB *a = this;
    OBB *b = other;

    for (int cycle = 0; cycle < 2; cycle++) {
        if (cycle == 1) {
            a = other;
            b = this;
        }
        for (int a_lp = 0; a_lp < 3; a_lp++) {
            int a_normal_index = 2;
            if (a_lp != 1) {
                a_normal_index = (a_lp ^ 2) == 0; // really written this way?
            }
            a_normal = a->normal[a_normal_index];
            UMath::Subxyz(a->position, b->position, rel_position);
            projected_interval = fabsf(UMath::Dotxyz(rel_position, a_normal));
            projected_interval -= a->dimension[a_normal_index];
            b_extent = b->extent;
            for (int b_normal_index = 0; b_normal_index < 3; b_normal_index++) {
                b_projected_interval = UMath::Dotxyz(a_normal, *b_extent);
                projected_interval -= fabsf(b_projected_interval);
                b_extent++;
            }
            if (projected_interval > 0.0f) {
                return false;
            }
        }
    }
    return true;
}

bool OBB::BoxVsBox(OBB *a, OBB *b, OBB *result) {
    // TODO
}

bool OBB::SphereVsBox(OBB *a, OBB *b, OBB *result) {
    // TODO
}

bool OBB::SphereVsSphere(OBB *a, OBB *b, OBB *result) {
    UMath::Vector4 rPos;
    UMath::Subxyz(a->position, b->position, rPos);
    float dist = UMath::Lengthxyz(rPos);
    float radius_total = a->dimension[0] + b->dimension[0];

    if (dist == 0.0f) {
        result->collision_normal.x = 0.0f;
        result->collision_normal.y = -1.0f;
        result->collision_normal.z = 0.0f;

        result->collision_point.x = a->position.x;
        result->collision_point.x = a->position.y + a->dimension[0];
        result->collision_point.x = a->position.z;

        result->penetration_depth = -a->dimension[0];
    } else if (dist >= radius_total) {
        return false;
    } else {
        UMath::Unitxyz(rPos, result->collision_normal);
        result->penetration_depth = dist - radius_total;
        UMath::ScaleAddxyz(result->collision_normal, b->dimension[0] + result->penetration_depth, b->position, result->collision_point);
    }
    if (a == result) {
        result->collision_normal.x = -result->collision_normal.x;
        result->collision_normal.y = -result->collision_normal.y;
        result->collision_normal.z = -result->collision_normal.z;
    }
    return false;
}

bool OBB::CheckOBBOverlapAndFindIntersection(OBB *other) {
    this->penetration_depth = -100000.0f;
    if (this->shape == BOX) {
        if (other->shape == BOX) {
            return OBB::BoxVsBox(this, other, this);
        }
        if (other->shape == SPHERE) {
            return OBB::SphereVsBox(other, this, this);
        }
    } else if (this->shape == SPHERE) {
        if (other->shape == SPHERE) {
            return OBB::SphereVsSphere(this, other, this);
        }
        if (other->shape == BOX) {
            return OBB::SphereVsBox(this, other, this);
        }
    }
    return false;
}
