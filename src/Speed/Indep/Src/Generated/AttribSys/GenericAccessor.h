#ifndef _genericaccessor_h_
#define _genericaccessor_h_

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {

namespace Gen {

class GenericAccessor : private Instance {
  public:
    bool AERO_CG(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xa073de78, index, val);
    }

    unsigned int Num_AERO_CG() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa073de78);
    }

    bool AERO_COEFFICIENT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xc29b0f17, index, val);
    }

    unsigned int Num_AERO_COEFFICIENT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc29b0f17);
    }

    bool AI_AVOIDABLE(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0x6e4de905, index, val);
    }

    unsigned int Num_AI_AVOIDABLE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x6e4de905);
    }

    bool AIR_RESISTANCE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xd28fd39c, index, val);
    }

    unsigned int Num_AIR_RESISTANCE() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd28fd39c);
    }

    bool AIR_RESISTANCE_SCALE(UMath::Vector4 &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(UMath::Vector4, 0x1807dfdf, index, val);
    }

    unsigned int Num_AIR_RESISTANCE_SCALE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1807dfdf);
    }

    bool aivehicle(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0x22515733, index, val);
    }

    unsigned int Num_aivehicle() const {
        ATTRIB_CODEGEN_GETLENGTH(0x22515733);
    }

    bool ALLOW_OFF_WORLD(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0xbee139f1, index, val);
    }

    unsigned int Num_ALLOW_OFF_WORLD() const {
        ATTRIB_CODEGEN_GETLENGTH(0xbee139f1);
    }

    bool AvailableOnline(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0x39509746, index, val);
    }

    unsigned int Num_AvailableOnline() const {
        ATTRIB_CODEGEN_GETLENGTH(0x39509746);
    }

    bool AvailableQR(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0xb39ed8c3, index, val);
    }

    unsigned int Num_AvailableQR() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb39ed8c3);
    }

    bool BaseMaterial(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0x5497a394, index, val);
    }

    unsigned int Num_BaseMaterial() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5497a394);
    }

    bool BEHAVIOR_MECHANIC_AUDIO(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0xfb0b5be9, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_AUDIO() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfb0b5be9);
    }

    bool BEHAVIOR_MECHANIC_DAMAGE(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x858ed6e3, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_DAMAGE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x858ed6e3);
    }

    bool BEHAVIOR_MECHANIC_DRAW(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0xb230ade1, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_DRAW() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb230ade1);
    }

    bool BEHAVIOR_MECHANIC_ENGINE(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0xa3e13328, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_ENGINE() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa3e13328);
    }

    bool BEHAVIOR_MECHANIC_INPUT(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0xc3fa0cc4, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_INPUT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc3fa0cc4);
    }

    bool BEHAVIOR_MECHANIC_RIGIDBODY(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x8ba55001, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_RIGIDBODY() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8ba55001);
    }

    bool BEHAVIOR_MECHANIC_SUSPENSION(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x511abd7b, index, val);
    }

    unsigned int Num_BEHAVIOR_MECHANIC_SUSPENSION() const {
        ATTRIB_CODEGEN_GETLENGTH(0x511abd7b);
    }

    bool BEHAVIOR_ORDER(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x104e9d16, index, val);
    }

    unsigned int Num_BEHAVIOR_ORDER() const {
        ATTRIB_CODEGEN_GETLENGTH(0x104e9d16);
    }

    bool CAMERA_AVOIDABLE(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0xe9d83d0c, index, val);
    }

    unsigned int Num_CAMERA_AVOIDABLE() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe9d83d0c);
    }

    bool CAN_SIMPLIFY(int &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(int, 0xd1d3909a, index, val);
    }

    unsigned int Num_CAN_SIMPLIFY() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd1d3909a);
    }

    bool chopperspecs(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0x5d898ee7, index, val);
    }

    unsigned int Num_chopperspecs() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5d898ee7);
    }

    bool CLASS(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x0ef6ddf2, index, val);
    }

    unsigned int Num_CLASS() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0ef6ddf2);
    }

    bool CollectionName(const char *&val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(const char *, 0x9ca1c8f9, index, val);
    }

    unsigned int Num_CollectionName() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9ca1c8f9);
    }

    bool CSIS_EFFECT(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0xbec30f42, index, val);
    }

    unsigned int Num_CSIS_EFFECT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xbec30f42);
    }

    bool damagespecs(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0xc1f0b434, index, val);
    }

    unsigned int Num_damagespecs() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc1f0b434);
    }

    bool DESCRIPTION(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x09925106, index, val);
    }

    unsigned int Num_DESCRIPTION() const {
        ATTRIB_CODEGEN_GETLENGTH(0x09925106);
    }

    bool DETACH_FORCE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xbe71dbad, index, val);
    }

    unsigned int Num_DETACH_FORCE() const {
        ATTRIB_CODEGEN_GETLENGTH(0xbe71dbad);
    }

    bool DRAG_COEFFICIENT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x3c860745, index, val);
    }

    unsigned int Num_DRAG_COEFFICIENT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x3c860745);
    }

    bool DRIVE_SPEED(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x3244ec00, index, val);
    }

    unsigned int Num_DRIVE_SPEED() const {
        ATTRIB_CODEGEN_GETLENGTH(0x3244ec00);
    }

    bool DROPOUT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x44f1273b, index, val);
    }

    unsigned int Num_DROPOUT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x44f1273b);
    }

    bool DZ_BOTTOM(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xadd2023d, index, val);
    }

    unsigned int Num_DZ_BOTTOM() const {
        ATTRIB_CODEGEN_GETLENGTH(0xadd2023d);
    }

    bool DZ_FRONT(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0x1e21c772, index, val);
    }

    unsigned int Num_DZ_FRONT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1e21c772);
    }

    bool DZ_LEFT(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0x698e7c31, index, val);
    }

    unsigned int Num_DZ_LEFT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x698e7c31);
    }

    bool DZ_LFRONT(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xa6f71a1c, index, val);
    }

    unsigned int Num_DZ_LFRONT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa6f71a1c);
    }

    bool DZ_LREAR(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xb631bf4e, index, val);
    }

    unsigned int Num_DZ_LREAR() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb631bf4e);
    }

    bool DZ_REAR(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xe1707b8a, index, val);
    }

    unsigned int Num_DZ_REAR() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe1707b8a);
    }

    bool DZ_RFRONT(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0x0380056f, index, val);
    }

    unsigned int Num_DZ_RFRONT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0380056f);
    }

    bool DZ_RIGHT(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xf700fe17, index, val);
    }

    unsigned int Num_DZ_RIGHT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf700fe17);
    }

    bool DZ_RREAR(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xcc3f0ac3, index, val);
    }

    unsigned int Num_DZ_RREAR() const {
        ATTRIB_CODEGEN_GETLENGTH(0xcc3f0ac3);
    }

    bool DZ_TOP(DamageScaleRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(DamageScaleRecord, 0xc4ce6d74, index, val);
    }

    unsigned int Num_DZ_TOP() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc4ce6d74);
    }

    bool engineaudio(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0x50eab0e6, index, val);
    }

    unsigned int Num_engineaudio() const {
        ATTRIB_CODEGEN_GETLENGTH(0x50eab0e6);
    }

    bool EventSequencer(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x5aab860f, index, val);
    }

    unsigned int Num_EventSequencer() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5aab860f);
    }

    bool HornType(unsigned char &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(unsigned char, 0xa481f8ff, index, val);
    }

    unsigned int Num_HornType() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa481f8ff);
    }

    bool KILL_OFF_SCREEN(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xc141f7f8, index, val);
    }

    unsigned int Num_KILL_OFF_SCREEN() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc141f7f8);
    }

    bool MASS(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x4a56503d, index, val);
    }

    unsigned int Num_MASS() const {
        ATTRIB_CODEGEN_GETLENGTH(0x4a56503d);
    }

    bool MAX_SPEED_MPS(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x9bbdb702, index, val);
    }

    unsigned int Num_MAX_SPEED_MPS() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9bbdb702);
    }

    bool MODEL(StringKey &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(StringKey, 0x9047c9e0, index, val);
    }

    unsigned int Num_MODEL() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9047c9e0);
    }

    bool MOMENT(Vector3 &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(Vector3, 0xfb19212f, index, val);
    }

    unsigned int Num_MOMENT() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfb19212f);
    }

    bool NO_CAR_EFFECT(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0x1f989f01, index, val);
    }

    unsigned int Num_NO_CAR_EFFECT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1f989f01);
    }

    bool OnBottomOut(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0x31047ebc, index, val);
    }

    unsigned int Num_OnBottomOut() const {
        ATTRIB_CODEGEN_GETLENGTH(0x31047ebc);
    }

    bool OnBottomScrape(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xfc03ad6a, index, val);
    }

    unsigned int Num_OnBottomScrape() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfc03ad6a);
    }

    bool OnDetached(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xd177bdaa, index, val);
    }

    unsigned int Num_OnDetached() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd177bdaa);
    }

    bool OnHitGround(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xd9c6cdfd, index, val);
    }

    unsigned int Num_OnHitGround() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd9c6cdfd);
    }

    bool OnHitObject(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0x18915735, index, val);
    }

    unsigned int Num_OnHitObject() const {
        ATTRIB_CODEGEN_GETLENGTH(0x18915735);
    }

    bool OnHitWorld(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xe3167336, index, val);
    }

    unsigned int Num_OnHitWorld() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe3167336);
    }

    bool OnScrapeGround(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xdb823931, index, val);
    }

    unsigned int Num_OnScrapeGround() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdb823931);
    }

    bool OnScrapeObject(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0xdacb1c11, index, val);
    }

    unsigned int Num_OnScrapeObject() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdacb1c11);
    }

    bool OnScrapeWorld(EffectLinkageRecord &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(EffectLinkageRecord, 0x7100960c, index, val);
    }

    unsigned int Num_OnScrapeWorld() const {
        ATTRIB_CODEGEN_GETLENGTH(0x7100960c);
    }

    bool PITCH_ALIGN_SCALE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x699b2b3c, index, val);
    }

    unsigned int Num_PITCH_ALIGN_SCALE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x699b2b3c);
    }

    bool PITCH_ANG(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x180485aa, index, val);
    }

    unsigned int Num_PITCH_ANG() const {
        ATTRIB_CODEGEN_GETLENGTH(0x180485aa);
    }

    bool PITCH_SLOW_DOWN_RATE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x37894b0f, index, val);
    }

    unsigned int Num_PITCH_SLOW_DOWN_RATE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x37894b0f);
    }

    bool PITCH_STOP_SPEED(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x0e1da662, index, val);
    }

    unsigned int Num_PITCH_STOP_SPEED() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0e1da662);
    }

    bool RESPAWN_TIME(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x5f84f834, index, val);
    }

    unsigned int Num_RESPAWN_TIME() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5f84f834);
    }

    bool ROLL_ALIGN_SCALE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x39950018, index, val);
    }

    unsigned int Num_ROLL_ALIGN_SCALE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x39950018);
    }

    bool ROLL_ANG(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x287e9321, index, val);
    }

    unsigned int Num_ROLL_ANG() const {
        ATTRIB_CODEGEN_GETLENGTH(0x287e9321);
    }

    bool ROLL_SLOW_DOWN_RATE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x62a08af6, index, val);
    }

    unsigned int Num_ROLL_SLOW_DOWN_RATE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x62a08af6);
    }

    bool ROLL_SPEED_MIN(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xd6c490c9, index, val);
    }

    unsigned int Num_ROLL_SPEED_MIN() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd6c490c9);
    }

    bool ROLL_START_SPEED(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xb2d7c5b8, index, val);
    }

    unsigned int Num_ROLL_START_SPEED() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb2d7c5b8);
    }

    bool SAME_ALIGN_SCALE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x8b54c4ae, index, val);
    }

    unsigned int Num_SAME_ALIGN_SCALE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8b54c4ae);
    }

    bool SCALE_STEERING(bool &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(bool, 0x5c470cc8, index, val);
    }

    unsigned int Num_SCALE_STEERING() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5c470cc8);
    }

    bool SHOCK_FORCE(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x907930df, index, val);
    }

    unsigned int Num_SHOCK_FORCE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x907930df);
    }

    bool SHOCK_TIME(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x78059ff8, index, val);
    }

    unsigned int Num_SHOCK_TIME() const {
        ATTRIB_CODEGEN_GETLENGTH(0x78059ff8);
    }

    bool STITCH_LEVEL_0(STICH_COLLISION_TYPE &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(STICH_COLLISION_TYPE, 0xc15856df, index, val);
    }

    unsigned int Num_STITCH_LEVEL_0() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc15856df);
    }

    bool STITCH_LEVEL_1(STICH_COLLISION_TYPE &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(STICH_COLLISION_TYPE, 0xdadb5580, index, val);
    }

    unsigned int Num_STITCH_LEVEL_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdadb5580);
    }

    bool STITCH_LEVEL_2(STICH_COLLISION_TYPE &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(STICH_COLLISION_TYPE, 0xc9218f8c, index, val);
    }

    unsigned int Num_STITCH_LEVEL_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc9218f8c);
    }

    bool STITCH_LEVEL_3(STICH_COLLISION_TYPE &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(STICH_COLLISION_TYPE, 0xefbca3c9, index, val);
    }

    unsigned int Num_STITCH_LEVEL_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xefbca3c9);
    }

    bool STRAFE_SCALEX(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x90316cbc, index, val);
    }

    unsigned int Num_STRAFE_SCALEX() const {
        ATTRIB_CODEGEN_GETLENGTH(0x90316cbc);
    }

    bool STRAFE_SCALEY(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x1ebefd4a, index, val);
    }

    unsigned int Num_STRAFE_SCALEY() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1ebefd4a);
    }

    bool SUPPRESS_DIST(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xc802a18d, index, val);
    }

    unsigned int Num_SUPPRESS_DIST() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc802a18d);
    }

    bool TENSOR_SCALE(Vector4 &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(Vector4, 0x868dffdd, index, val);
    }

    unsigned int Num_TENSOR_SCALE() const {
        ATTRIB_CODEGEN_GETLENGTH(0x868dffdd);
    }

    bool Trailer(RefSpec &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(RefSpec, 0x9a5537fe, index, val);
    }

    unsigned int Num_Trailer() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9a5537fe);
    }

    bool TURN_BOOST_SPEED(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x21997946, index, val);
    }

    unsigned int Num_TURN_BOOST_SPEED() const {
        ATTRIB_CODEGEN_GETLENGTH(0x21997946);
    }

    bool YAW_BOOST_LIMIT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x8509f111, index, val);
    }

    unsigned int Num_YAW_BOOST_LIMIT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8509f111);
    }

    bool YAW_DAMP(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0xc68cae8f, index, val);
    }

    unsigned int Num_YAW_DAMP() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc68cae8f);
    }

    bool YAW_LIMIT_FRONT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x3e984672, index, val);
    }

    unsigned int Num_YAW_LIMIT_FRONT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x3e984672);
    }

    bool YAW_LIMIT_REAR(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x09030d44, index, val);
    }

    unsigned int Num_YAW_LIMIT_REAR() const {
        ATTRIB_CODEGEN_GETLENGTH(0x09030d44);
    }

    bool YAW_STRENGTH_FRONT(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x9656d8f8, index, val);
    }

    unsigned int Num_YAW_STRENGTH_FRONT() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9656d8f8);
    }

    bool YAW_STRENGTH_REAR(float &val, unsigned int index) const {
        ATTRIB_CODEGEN_GETVALIDATTRIB(float, 0x2da26212, index, val);
    }

    unsigned int Num_YAW_STRENGTH_REAR() const {
        ATTRIB_CODEGEN_GETLENGTH(0x2da26212);
    }

  private:
    GenericAccessor();
};

}; // namespace Gen

namespace Hash {

namespace Generic {

static const Key AERO_CG = 0xa073de78;
static const Key AERO_COEFFICIENT = 0xc29b0f17;
static const Key AI_AVOIDABLE = 0x6e4de905;
static const Key AIR_RESISTANCE = 0xd28fd39c;
static const Key AIR_RESISTANCE_SCALE = 0x1807dfdf;
static const Key aivehicle = 0x22515733;
static const Key ALLOW_OFF_WORLD = 0xbee139f1;
static const Key AvailableOnline = 0x39509746;
static const Key AvailableQR = 0xb39ed8c3;
static const Key BaseMaterial = 0x5497a394;
static const Key BEHAVIOR_MECHANIC_AUDIO = 0xfb0b5be9;
static const Key BEHAVIOR_MECHANIC_DAMAGE = 0x858ed6e3;
static const Key BEHAVIOR_MECHANIC_DRAW = 0xb230ade1;
static const Key BEHAVIOR_MECHANIC_ENGINE = 0xa3e13328;
static const Key BEHAVIOR_MECHANIC_INPUT = 0xc3fa0cc4;
static const Key BEHAVIOR_MECHANIC_RIGIDBODY = 0x8ba55001;
static const Key BEHAVIOR_MECHANIC_SUSPENSION = 0x511abd7b;
static const Key BEHAVIOR_ORDER = 0x104e9d16;
static const Key CAMERA_AVOIDABLE = 0xe9d83d0c;
static const Key CAN_SIMPLIFY = 0xd1d3909a;
static const Key chopperspecs = 0x5d898ee7;
static const Key CLASS = 0x0ef6ddf2;
static const Key CollectionName = 0x9ca1c8f9;
static const Key CSIS_EFFECT = 0xbec30f42;
static const Key damagespecs = 0xc1f0b434;
static const Key DESCRIPTION = 0x09925106;
static const Key DETACH_FORCE = 0xbe71dbad;
static const Key DRAG_COEFFICIENT = 0x3c860745;
static const Key DRIVE_SPEED = 0x3244ec00;
static const Key DROPOUT = 0x44f1273b;
static const Key DZ_BOTTOM = 0xadd2023d;
static const Key DZ_FRONT = 0x1e21c772;
static const Key DZ_LEFT = 0x698e7c31;
static const Key DZ_LFRONT = 0xa6f71a1c;
static const Key DZ_LREAR = 0xb631bf4e;
static const Key DZ_REAR = 0xe1707b8a;
static const Key DZ_RFRONT = 0x0380056f;
static const Key DZ_RIGHT = 0xf700fe17;
static const Key DZ_RREAR = 0xcc3f0ac3;
static const Key DZ_TOP = 0xc4ce6d74;
static const Key engineaudio = 0x50eab0e6;
static const Key EventSequencer = 0x5aab860f;
static const Key HornType = 0xa481f8ff;
static const Key KILL_OFF_SCREEN = 0xc141f7f8;
static const Key MASS = 0x4a56503d;
static const Key MAX_SPEED_MPS = 0x9bbdb702;
static const Key MODEL = 0x9047c9e0;
static const Key MOMENT = 0xfb19212f;
static const Key NO_CAR_EFFECT = 0x1f989f01;
static const Key OnBottomOut = 0x31047ebc;
static const Key OnBottomScrape = 0xfc03ad6a;
static const Key OnDetached = 0xd177bdaa;
static const Key OnHitGround = 0xd9c6cdfd;
static const Key OnHitObject = 0x18915735;
static const Key OnHitWorld = 0xe3167336;
static const Key OnScrapeGround = 0xdb823931;
static const Key OnScrapeObject = 0xdacb1c11;
static const Key OnScrapeWorld = 0x7100960c;
static const Key PITCH_ALIGN_SCALE = 0x699b2b3c;
static const Key PITCH_ANG = 0x180485aa;
static const Key PITCH_SLOW_DOWN_RATE = 0x37894b0f;
static const Key PITCH_STOP_SPEED = 0x0e1da662;
static const Key RESPAWN_TIME = 0x5f84f834;
static const Key ROLL_ALIGN_SCALE = 0x39950018;
static const Key ROLL_ANG = 0x287e9321;
static const Key ROLL_SLOW_DOWN_RATE = 0x62a08af6;
static const Key ROLL_SPEED_MIN = 0xd6c490c9;
static const Key ROLL_START_SPEED = 0xb2d7c5b8;
static const Key SAME_ALIGN_SCALE = 0x8b54c4ae;
static const Key SCALE_STEERING = 0x5c470cc8;
static const Key SHOCK_FORCE = 0x907930df;
static const Key SHOCK_TIME = 0x78059ff8;
static const Key STITCH_LEVEL_0 = 0xc15856df;
static const Key STITCH_LEVEL_1 = 0xdadb5580;
static const Key STITCH_LEVEL_2 = 0xc9218f8c;
static const Key STITCH_LEVEL_3 = 0xefbca3c9;
static const Key STRAFE_SCALEX = 0x90316cbc;
static const Key STRAFE_SCALEY = 0x1ebefd4a;
static const Key SUPPRESS_DIST = 0xc802a18d;
static const Key TENSOR_SCALE = 0x868dffdd;
static const Key Trailer = 0x9a5537fe;
static const Key TURN_BOOST_SPEED = 0x21997946;
static const Key YAW_BOOST_LIMIT = 0x8509f111;
static const Key YAW_DAMP = 0xc68cae8f;
static const Key YAW_LIMIT_FRONT = 0x3e984672;
static const Key YAW_LIMIT_REAR = 0x09030d44;
static const Key YAW_STRENGTH_FRONT = 0x9656d8f8;
static const Key YAW_STRENGTH_REAR = 0x2da26212;

}; // namespace Generic

}; // namespace Hash

}; // namespace Attrib

#endif
