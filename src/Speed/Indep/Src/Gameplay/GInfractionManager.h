#ifndef GAMEPLAY_GINFRACTIONMANAGER_H
#define GAMEPLAY_GINFRACTIONMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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
        return mObj != nullptr;
    }

    void ClearInfractions() {
        mInfractions = 0;
    }

    void ReportResistingArrest() {
        ReportInfraction(kInfraction_Resist);
    }

    void ReportSpeeding(bool speeding) {
        mSpeeding = speeding;
        if (speeding) {
            ReportInfraction(kInfraction_Speeding);
        }
    }

    void ReportRecklessDriving(bool reckless) {
        mDrivingRecklessly = reckless;
        if (reckless) {
            ReportInfraction(kInfraction_Reckless);
        }
    }

    void ReportRacing(bool racing) {
        mRacing = racing;
        if (racing) {
            ReportInfraction(kInfraction_Racing);
        }
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

    float GetRecklessSpeedThreshold();

    float GetSpeedLimit();

    float GetRacingSpeedLimit();

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
