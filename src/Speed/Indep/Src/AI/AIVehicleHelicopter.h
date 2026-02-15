#ifndef AI_AIVEHICLEHELICOPTER_H
#define AI_AIVEHICLEHELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AIVehiclePursuit.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/World/HeliSheet.hpp"

// total size: 0x8C0
class AIVehicleHelicopter : public AIVehiclePursuit, public IAIHelicopter {
  public:
    AIVehicleHelicopter(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleHelicopter() override;

    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: IAIHelicopter
    void SetFuelFull() override;

    // Overrides: IAIHelicopter
    void SetDestinationVelocity(const UMath::Vector3 &v) override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    void UpdateFuel(float dT);

    // Overrides: IPursuitAI
    bool CanSeeTarget(AITarget *target) override;

    // Overrides: IAIHelicopter
    bool StartPathToPoint(UMath::Vector3 &point) override;

    // Overrides: IAIHelicopter
    void SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest) override;

    // Overrides: IAIHelicopter
    bool FilterHeliAltitude(UMath::Vector3 &point) override;

    bool CheckHeliSheet(const UMath::Vector3 &myPosition, const UMath::Vector3 &LookAheadDest, const UMath::Vector3 &myWorkingPosition,
                        UMath::Vector3 &dest, UMath::Vector3 &smoothingVel);

    // Overrides: IAIHelicopter
    void RestrictPointToRoadNet(UMath::Vector3 &seekPosition) override;

    void AvoidCamera(UMath::Vector3 &dest);

    // Overrides: AIVehicle
    void OnDriving(float dT) override;

    // Overrides: IAIHelicopter
    float GetDesiredHeightOverDest() const override {
        return mHeight;
    }

    // Overrides: IAIHelicopter
    void SetDesiredHeightOverDest(const float height) override {
        mHeight = height;
    }

    // Overrides: IAIHelicopter
    void SetLookAtPosition(UMath::Vector3 la) override {
        mLookAtPosition = la;
    }

    // Overrides: IAIHelicopter
    UMath::Vector3 GetLookAtPosition() const override {
        return mLookAtPosition;
    }

    // Overrides: IAIHelicopter
    bool StrafeToDestIsSet() const override {
        return mStrafeToDest;
    }

    // Overrides: IAIHelicopter
    void SetStrafeToDest(bool strafe) override {
        mStrafeToDest = strafe;
    }

    virtual const HeliSheetCoordinate &GetHeliSheetCoord() const {
        return mHeliSheetCoord;
    }

    // Overrides: IAIHelicopter
    float GetFuelTimeRemaining() override {
        return mHeliFuelTimeRemaining;
    }

    // Overrides: IAIHelicopter
    void SetShadowScale(float s) override {
        mShadowScale = s;
    }

    // Overrides: IAIHelicopter
    float GetShadowScale() override {
        return mShadowScale;
    }

    // Overrides: IAIHelicopter
    void SetDustStormIntensity(float d) override {
        mDustStormIntensity = d;
    }

    // Overrides: IAIHelicopter
    float GetDustStormIntensity() override {
        return mDustStormIntensity;
    }

  private:
    UMath::Vector3 mDestinationVelocity;          // offset 0x7D8, size 0xC
    UMath::Vector3 mLookAtPosition;               // offset 0x7E4, size 0xC
    UMath::Vector3 mLastPlaceHeliSawPerp;         // offset 0x7F0, size 0xC
    float mHeight;                                // offset 0x7FC, size 0x4
    bool mStrafeToDest;                           // offset 0x800, size 0x1
    bool mPerpHiddenFromMe;                       // offset 0x804, size 0x1
    float mHeliFuelTimeRemaining;                 // offset 0x808, size 0x4
    float mShadowScale;                           // offset 0x80C, size 0x4
    float mDustStormIntensity;                    // offset 0x810, size 0x4
    HeliSheetCoordinate mHeliSheetCoord;          // offset 0x814, size 0x38
    HeliSheetCoordinate mSecondaryHeliSheetCoord; // offset 0x84C, size 0x38
    HeliSheetCoordinate mThirdHeliSheetCoord;     // offset 0x884, size 0x38
    ISimpleChopper *mISimpleChopper;              // offset 0x8BC, size 0x4
};

bool HeliVehicleActive();

extern AIVehicleHelicopter *gHeliVehicle;

#endif
