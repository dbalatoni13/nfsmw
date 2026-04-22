#ifndef ANIMATION_ANIMCANDIDATES_H
#define ANIMATION_ANIMCANDIDATES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0xC
class CRaceStartAnimCandidate {
  public:
    CRaceStartAnimCandidate() {}

    ~CRaceStartAnimCandidate() {}

    void EndianSwap() {
        bPlatEndianSwap(&mSceneHash);
    }

    uint32 GetSceneHash() {
        return mSceneHash;
    }

    int GetCameraTrack() {
        return mCameraTrack;
    }

    bool IsLive() {}

    bool PlayerCanBeInThisPosition(int position) {}

    uint32 mSceneHash;          // offset 0x0, size 0x4
    uint8 mCameraTrack;         // offset 0x4, size 0x1
    uint8 mFlags;               // offset 0x5, size 0x1
    uint8 mMinimumNumberOfCars; // offset 0x6, size 0x1
    uint8 mMaximumNumberOfCars; // offset 0x7, size 0x1
    uint8 mCarConfig;           // offset 0x8, size 0x1
    uint8 mPad2;                // offset 0x9, size 0x1
    uint8 mPad3;                // offset 0xA, size 0x1
    uint8 mPad4;                // offset 0xB, size 0x1
};

// total size: 0x8
class CRaceEndAnimCandidate {
  public:
    CRaceEndAnimCandidate() {}

    ~CRaceEndAnimCandidate() {}

    void EndianSwap() {
        bPlatEndianSwap(&mSceneHash);
    }

    uint32 GetSceneHash() {
        return mSceneHash;
    }

    int GetCameraTrack() {
        return mCameraTrack;
    }

    bool IsLive() {}

    bool IsWinnerFocus() {}

    int GetWinnerCarNumber() {}

    int GetLoserCarNumber() {}

    uint32 mSceneHash;      // offset 0x0, size 0x4
    uint8 mCameraTrack;     // offset 0x4, size 0x1
    uint8 mFlags;           // offset 0x5, size 0x1
    uint8 mWinnerCarNumber; // offset 0x6, size 0x1
    uint8 mLoserCarNumber;  // offset 0x7, size 0x1
};

// total size: 0x14
class COnlineRaceStartAnimCandidate {
  public:
    COnlineRaceStartAnimCandidate() {}

    ~COnlineRaceStartAnimCandidate() {}

    void EndianSwap() {
        bPlatEndianSwap(&mSceneHash);
    }

    uint32 GetSceneHash() {
        return mSceneHash;
    }

    int GetCameraTrack() {
        return mCameraTrack;
    }

    bool IsLive() {}

    bool PlayerCanBeInThisPosition(int position) {}

    uint32 mSceneHash;            // offset 0x0, size 0x4
    uint8 mCameraTrack;           // offset 0x4, size 0x1
    uint8 mFlags;                 // offset 0x5, size 0x1
    uint8 mMinimumNumberOfCars;   // offset 0x6, size 0x1
    uint8 mMaximumNumberOfCars;   // offset 0x7, size 0x1
    uint8 mCarConfig;             // offset 0x8, size 0x1
    uint8 mPad2;                  // offset 0x9, size 0x1
    uint8 mPad3;                  // offset 0xA, size 0x1
    uint8 mPad4;                  // offset 0xB, size 0x1
    uint8 mPerCarCameraTracks[8]; // offset 0xC, size 0x8
};

// total size: 0x8
class CMomentAnim {
  public:
    CMomentAnim() {}

    ~CMomentAnim() {}

    void EndianSwap() {
        bPlatEndianSwap(&mSceneHash);
        bPlatEndianSwap(&mMomentType);
    }

    uint32 mSceneHash;  // offset 0x0, size 0x4
    uint32 mMomentType; // offset 0x4, size 0x4
};

// total size: 0x8
class CSpecialCarListAnimCandidate {
  public:
    enum SpecialCarList {
        NONE = -1,
        ONE_COP = 0,
        LOTS_OF_COPS = 1,
        TRAFFIC_MIX1 = 2,
        TRAFFIC_MIX2 = 3,
        TRAFFIC_MIX3 = 4,
        TRAFFIC_MIX4 = 5,
        TRAFFIC_MIX5 = 6,
        TRAFFIC_MIX6 = 7,
        TRAFFIC_MIX7 = 8,
        TRAFFIC_MIX8 = 9,
        TRAFFIC_MIX9 = 10,
        ONE_SPORT_COP = 11,
    };

    CSpecialCarListAnimCandidate() {}

    ~CSpecialCarListAnimCandidate() {}

    void EndianSwap() {
        bPlatEndianSwap(&mSceneHash);
        bPlatEndianSwap(&mSpecialCarList);
    }

    uint32 mSceneHash;      // offset 0x0, size 0x4
    uint32 mSpecialCarList; // offset 0x4, size 0x4
};

// total size: 0xC
class CTrackRaceAnimEnableMask {
  public:
    CTrackRaceAnimEnableMask() {}

    ~CTrackRaceAnimEnableMask() {}

    void EndianSwap() {
        bPlatEndianSwap(&mTrackNum);
        bPlatEndianSwap(&mOnlineRaceStartAnimEnable);
        bPlatEndianSwap(&mRaceStartAnimEnable1);
        bPlatEndianSwap(&mRaceStartAnimEnable2);
        bPlatEndianSwap(&mRaceEndAnimEnable);
    }

    uint16 mTrackNum;                  // offset 0x0, size 0x2
    uint16 mOnlineRaceStartAnimEnable; // offset 0x2, size 0x2
    uint32 mRaceStartAnimEnable1;      // offset 0x4, size 0x4
    uint16 mRaceStartAnimEnable2;      // offset 0x8, size 0x2
    uint16 mRaceEndAnimEnable;         // offset 0xA, size 0x2
};

enum eMomentMarkerTypes {
    CameraCutMomentMarker = 0,
    GasStationMomentMarker = 1,
    FirehallMomentMarker = 2,
    PoliceStationMomentMarker = 3,
    WaterTowerMomentMarker = 4,
    PorchCollapseMomentMarker = 5,
    SignageCollapseMomentMarker = 6,
    ScaffoldingCollapseMomentMarker = 7,
    GazeboCollapseMomentMarker = 8,
    FishMarketMomentMarker = 9,
    BoatKnockdownMomentMarker = 10,
    CopHitsObstructionMomentMarker = 11,
    BustedMomementMarker = 12,
    TollboothMarker = 13,
    GarageMarker = 14,
    PoliceStationGroundMomentMarker = 15,
    DriveInMomentMarker = 16,
    TrailerParkMomentMarker = 17,
    TorusRoofMomentMarker = 18,
    AmphitheatreMomentMarker = 19,
    StripMallMomentMarker = 20,
    NumMomentMarkerTypes = 21,
};

// total size: 0x1054
class CAnimCandidateData {
  public:
    CAnimCandidateData() {}

    ~CAnimCandidateData() {}

    void Clear() {}

    void AddRaceStartAnimCandidate(CRaceStartAnimCandidate &ac) {}

    void AddRaceEndAnimCandidate(CRaceEndAnimCandidate &ac) {}

    void AddOnlineRaceStartAnimCandidate(COnlineRaceStartAnimCandidate &ac) {}

    void AddMomentAnim(CMomentAnim &am) {}

    void AddSpecialCarListScene(CSpecialCarListAnimCandidate &am) {}

    struct CTrackRaceAnimEnableMask *MakeForwardTrackRaceAnimEnableMask(int track) {}

    struct CTrackRaceAnimEnableMask *MakeReverseTrackRaceAnimEnableMask(int track) {}

    void EndianSwap() {
        bPlatEndianSwap(&mRaceStartAnimCandidateCount);
        bPlatEndianSwap(&mRaceEndAnimCandidateCount);
        bPlatEndianSwap(&mOnlineRaceStartAnimCandidateCount);
        bPlatEndianSwap(&mForwardTrackRaceAnimEnableMaskCount);
        bPlatEndianSwap(&mReverseTrackRaceAnimEnableMaskCount);
        bPlatEndianSwap(&mMomentAnimCount);
        bPlatEndianSwap(&mSpecialCarListCount);

        uint32 i;
        for (i = 0; i < mRaceStartAnimCandidateCount; i++) {
            mRaceStartAnimCandidates[i].EndianSwap();
        }
        for (i = 0; i < mRaceEndAnimCandidateCount; i++) {
            mRaceEndAnimCandidates[i].EndianSwap();
        }
        for (i = 0; i < mOnlineRaceStartAnimCandidateCount; i++) {
            mOnlineRaceStartAnimCandidates[i].EndianSwap();
        }
        for (i = 0; i < mForwardTrackRaceAnimEnableMaskCount; i++) {
            mForwardTrackRaceAnimEnableMasks[i].EndianSwap();
        }
        for (i = 0; i < mReverseTrackRaceAnimEnableMaskCount; i++) {
            mReverseTrackRaceAnimEnableMasks[i].EndianSwap();
        }
        for (i = 0; i < mMomentAnimCount; i++) {
            mMomentAnim[i].EndianSwap();
        }
        for (i = 0; i < mSpecialCarListCount; i++) {
            mSpecialCarListAnim[i].EndianSwap();
        }
    }

    int GetSpecialCarList(uint32 sceneHash) {}

    int GetSceneMomentMarkerType(uint32 sceneHash);

    static const char *GetMomentMarkerName(int markerType);

    TrackPositionMarker *GetClosestMarker(uint32 sceneHash, bVector3 &position, int *ID, float *markerDist, bAngle angle);

    uint32 mRaceStartAnimCandidateCount;                             // offset 0x0, size 0x4
    uint32 mRaceEndAnimCandidateCount;                               // offset 0x4, size 0x4
    uint32 mOnlineRaceStartAnimCandidateCount;                       // offset 0x8, size 0x4
    uint32 mForwardTrackRaceAnimEnableMaskCount;                     // offset 0xC, size 0x4
    uint32 mReverseTrackRaceAnimEnableMaskCount;                     // offset 0x10, size 0x4
    uint32 mMomentAnimCount;                                         // offset 0x14, size 0x4
    uint32 mSpecialCarListCount;                                     // offset 0x18, size 0x4
    CRaceStartAnimCandidate mRaceStartAnimCandidates[8];             // offset 0x1C, size 0x60
    CRaceEndAnimCandidate mRaceEndAnimCandidates[8];                 // offset 0x7C, size 0x40
    COnlineRaceStartAnimCandidate mOnlineRaceStartAnimCandidates[8]; // offset 0xBC, size 0xA0
    CTrackRaceAnimEnableMask mForwardTrackRaceAnimEnableMasks[120];  // offset 0x15C, size 0x5A0
    CTrackRaceAnimEnableMask mReverseTrackRaceAnimEnableMasks[120];  // offset 0x6FC, size 0x5A0
    CMomentAnim mMomentAnim[64];                                     // offset 0xC9C, size 0x200
    CSpecialCarListAnimCandidate mSpecialCarListAnim[55];            // offset 0xE9C, size 0x1B8
};

extern CAnimCandidateData *TheAnimCandidateData;

#endif
