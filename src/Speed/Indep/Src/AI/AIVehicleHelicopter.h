//
//
//
//
//
//
//
//
//
//
//
#ifndef __AIVEHICLEHELICOPTER_H
#define __AIVEHICLEHELICOPTER_H 1

#include "AIVehiclePursuit.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/World/HeliSheet.hpp"

// total size: 0x8C0
class AIVehicleHelicopter : public AIVehiclePursuit, public IAIHelicopter {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: AIVehicle
    void Update(float dT) override;

    void UpdateFuel(float dT);

    // Overrides: IPursuitAI
    bool CanSeeTarget(AITarget *target) override;

  protected:
    AIVehicleHelicopter(const BehaviorParams &bp);
    ~AIVehicleHelicopter() override;

    // Overrides: IAIHelicopter
    float GetDesiredHeightOverDest() const override {
        return this->mHeight;
    }

    // Overrides: IAIHelicopter
    void SetDesiredHeightOverDest(const float height) override {
        this->mHeight = height;
    }

    // Overrides: IAIHelicopter
    void SetLookAtPosition(UMath::Vector3 la) override {
        this->mLookAtPosition = la;
    }

    // Overrides: IAIHelicopter
    UMath::Vector3 GetLookAtPosition() const override {
        return this->mLookAtPosition;
    }

    // Overrides: IAIHelicopter
    void SetDestinationVelocity(const UMath::Vector3 &v) override;
    void SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest) override;
    bool StartPathToPoint(UMath::Vector3 &point) override;
    void SetFuelFull() override;

    // Overrides: IAIHelicopter
    bool StrafeToDestIsSet() const override {
        return this->mStrafeToDest;
    }

    // Overrides: IAIHelicopter
    void SetStrafeToDest(bool strafe) override {
        this->mStrafeToDest = strafe;
    }

    // Overrides: IAIHelicopter
    bool FilterHeliAltitude(UMath::Vector3 &point) override;

    // Overrides: IAIHelicopter
    void RestrictPointToRoadNet(UMath::Vector3 &seekPosition) override;

    virtual const HeliSheetCoordinate &GetHeliSheetCoord() const {
        return this->mHeliSheetCoord;
    }

    // Overrides: IAIHelicopter
    float GetFuelTimeRemaining() override {
        return this->mHeliFuelTimeRemaining;
    }

    // Overrides: IAIHelicopter
    void SetShadowScale(float s) override {
        this->mShadowScale = s;
    }

    // Overrides: IAIHelicopter
    float GetShadowScale() override {
        return this->mShadowScale;
    }

    // Overrides: IAIHelicopter
    void SetDustStormIntensity(float d) override {
        this->mDustStormIntensity = d;
    }

    // Overrides: IAIHelicopter
    float GetDustStormIntensity() override {
        return this->mDustStormIntensity;
    }

  private:
    // Overrides: AIVehicle
    void OnDriving(float dT) override;

    bool CheckHeliSheet(const UMath::Vector3 &myPosition, const UMath::Vector3 &LookAheadDest, const UMath::Vector3 &myWorkingPosition,
                        UMath::Vector3 &dest, UMath::Vector3 &smoothingVel);

    void AvoidCamera(UMath::Vector3 &dest);

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

#endif
