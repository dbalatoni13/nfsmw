#ifndef GAMEPLAY_GINFRACTIONMANAGER_H
#define GAMEPLAY_GINFRACTIONMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern const float TWEAK_RecklessDrivingLimit;
extern const float TWEAK_SpeedingLimit;
extern const float TWEAK_RacingLimit;

// total size: 0x14
class GInfractionManager {
  public:
    enum InfractionType {
        kInfraction_Speeding = 1,
        kInfraction_Racing = 2,
        kInfraction_Reckless = 4,
        kInfraction_Assault = 8,
        kInfraction_HitAndRun = 16,
        kInfraction_Damage = 32,
        kInfraction_Resist = 64,
        kInfraction_OffRoad = 128,
    };

    GInfractionManager();

    ~GInfractionManager();

    static void Init();

    static void Shutdown();

    void PursuitStarted();

    void ReportInfraction(InfractionType infraction);

    unsigned int GetNumInfractions();

    bool DidInfractionOccur(InfractionType infraction);

    static GInfractionManager &Get() {
        return *mObj;
    }

    static bool Exists() {
        return mObj != NULL;
    }

    void ClearInfractions() {
        mInfractions = 0;
        mNumThisPursuit = 0;
        mSpeeding = false;
        mDrivingRecklessly = false;
        mRacing = false;
    }

    void ReportResistingArrest() {
        ReportInfraction(kInfraction_Resist);
    }

    void ReportSpeeding(bool speeding) {
        if (speeding && !mSpeeding) {
            ReportInfraction(kInfraction_Speeding);
        }
        mSpeeding = speeding;
    }

    void ReportRecklessDriving(bool reckless) {
        if (reckless && !mDrivingRecklessly) {
            ReportInfraction(kInfraction_Reckless);
        }
        mDrivingRecklessly = reckless;
    }

    void ReportRacing(bool racing) {
        if (racing && !mRacing) {
            ReportInfraction(kInfraction_Racing);
        }
        mRacing = racing;
    }

    void ReportAssaultingPoliceOfficer() {
        ReportInfraction(kInfraction_Assault);
    }

    void ReportHitAndRun() {
        ReportInfraction(kInfraction_HitAndRun);
    }

    void ReportDamageToProperty() {
        ReportInfraction(kInfraction_Damage);
    }

    void ReportDrivingOffRoadWay() {
        ReportInfraction(kInfraction_OffRoad);
    }

    float GetRecklessSpeedThreshold() {
        return TWEAK_RecklessDrivingLimit;
    }

    float GetSpeedLimit() {
        return TWEAK_SpeedingLimit;
    }

    float GetRacingSpeedLimit() {
        return TWEAK_RacingLimit;
    }

    unsigned int GetInfractions() {
        return mInfractions;
    }

  private:
    static GInfractionManager *mObj; // size: 0x4

    unsigned int mInfractions;    // offset 0x0, size 0x4
    unsigned int mNumThisPursuit; // offset 0x4, size 0x4
    bool mSpeeding;               // offset 0x8, size 0x1
    bool mDrivingRecklessly;      // offset 0xC, size 0x1
    bool mRacing;                 // offset 0x10, size 0x1
};

#endif
