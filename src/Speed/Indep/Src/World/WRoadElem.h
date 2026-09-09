#ifndef WORLD_WROADELEM_H
#define WORLD_WROADELEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// TODO these show up in Undercover

// static const float ROAD_LENGTH_MAX;
// static const float ROAD_LENGTH_SCALE;
// static const float ROAD_LENGTH_RECIP;
// static const float ROAD_WIDTH_MAX;
// static const float ROAD_WIDTH_SCALE;
// static const float ROAD_WIDTH_RECIP;
// static const int NO_SHORTCUT;

// static const unsigned char kRoadProfileTraffic;
// static const unsigned char kRoadProfileSidewalk;
// static const unsigned char kRoadProfileShoulder;
// static const unsigned char kRoadProfileMedian;
// static const unsigned char kRoadProfileCurbMedian;
// static const unsigned char kRoadProfileGrassMedian;
// static const unsigned char kRoadProfileBarrier;
// static const unsigned char kRoadProfileTrain;
// static const unsigned char kRoadProfileParking;
// static const unsigned char kRoadProfileRoad;
// static const unsigned char kRoadProfileCenter;
// static const unsigned char kRoadProfileDrivable;
// static const unsigned char kRoadProfileUndrivable;
// static const int kRoadProfileMaxZones;

// static const float LANE_MAX;
// static const float LANE_SCALE;
// static const float LANE_RECIP;
// static const int LANE_TYPE_BITS;
// static const int LANE_WIDTH_BITS;
// static const int LANE_OFFSET_BITS;
// static const int LANE_TYPE_OFFSET;
// static const int LANE_WIDTH_OFFSET;
// static const int LANE_OFFSET_OFFSET;
// static const int kRoadNodeMaxSegments;

// total size: 0x8
struct WRoad {
    float GetScale() const {
        unsigned int s = static_cast<unsigned int>(this->nScale) << 8;
        return static_cast<float>(s) * (1.0f / 65536.0f);
    }

    // float GetLength() const {}

    // float GetMinimumWidth() const {}

    // void SetScale(float scale) {}

    // void SetLength(float length) {}

    // void SetMinimumWidth(float width) {}

    uint16 nScale;    // offset 0x0, size 0x2
    uint16 nLength;   // offset 0x2, size 0x2
    uint8 nShortcut;  // offset 0x4, size 0x1
    uint8 nMinWidth;  // offset 0x5, size 0x1
    uint16 nSpeechId; // offset 0x6, size 0x2
};

// total size: 0xE
struct WRoadNetworkInfo {
    unsigned short fNumProfiles;      // offset 0x0, size 0x2
    unsigned short fNumNodes;         // offset 0x2, size 0x2
    unsigned short fNumSegments;      // offset 0x4, size 0x2
    unsigned short fNumIntersections; // offset 0x6, size 0x2
    unsigned short fNumRoads;         // offset 0x8, size 0x2
    unsigned short fNumJunctions;     // offset 0xA, size 0x2
    char fPad[2];                     // offset 0xC, size 0x2
};

// total size: 0x4
struct WRoadLane {
    int GetType() const {
        return this->GetBits(0, 4);
    }

    float GetWidth() const {
        return static_cast<float>(this->GetBitsSigned(4, 14)) * (100.0f / 8191.0f);
    }

    float GetOffset() const {
        return static_cast<float>(this->GetBitsSigned(18, 14)) * (100.0f / 8191.0f);
    }

  private:
    uint32 GetBits(int n_offset, int n_bits) const {
        uint32 n_mask = ~(-1 << n_bits);
        return (this->nBits >> n_offset) & n_mask;
    }

    int GetBitsSigned(int n_offset, int n_bits) const {
        int extra_high_bits = 32 - (n_offset + n_bits);
        int bits = static_cast<int>(this->nBits << extra_high_bits);
        return bits >> (n_offset + extra_high_bits);
    }

    void SetBits(int n_offset, int n_bits, int n_value) {}

    uint32 nBits; // offset 0x0, size 0x4
};

// total size: 0x40
struct WRoadProfile {
    int GetMiddleZone(bool inverted) const {
        return inverted ? (this->fNumZones - this->fMiddleZone) : this->fMiddleZone;
    }

    int GetLaneNumber(int lane, bool inverted) const {
        return inverted ? this->fNumZones - lane - 1 : lane;
    }

    float GetLaneWidth(int lane, bool inverted) const {
        return this->mLanes[this->GetLaneNumber(lane, inverted)].GetWidth();
    }

    float GetLaneOffset(int lane, bool inverted) const {
        return this->mLanes[this->GetLaneNumber(lane, inverted)].GetOffset();
    }

    int GetLaneType(int lane, bool inverted) const {
        return this->mLanes[this->GetLaneNumber(lane, inverted)].GetType();
    }

    float GetRawLaneOffset(int lane) const {
        return this->GetLaneOffset(lane, false);
    }

    float GetRawLaneWidth(int lane) const {
        return this->GetLaneWidth(lane, false);
    }

    float GetRelativeLaneOffset(int lane, bool inverted) const {
        int real_middle = this->GetMiddleZone(inverted);
        float offset = this->GetLaneOffset(lane, inverted);

        return lane < real_middle ? -offset : offset;
    }

    int GetNumForwardLanes() const {
        return this->fNumZones - this->fMiddleZone;
    }

    int GetNumBackwardLanes() const {
        return this->fNumZones - this->GetNumForwardLanes();
    }

    int GetNthForwardLane(int n) const {
        int ret = this->fMiddleZone + n;
        if (ret >= this->fNumZones) {
            ret = this->fNumZones - 1;
        }
        return ret;
    }

    int GetNthBackwardLane(int n) const {
        int ret = this->fMiddleZone - 1 - n;
        if (ret < 0) {
            ret = 0;
        }
        return ret;
    }

    int GetNumTrafficLanes(bool forward) const;
    int GetNumTrafficLanes(bool forward, bool inverted) const {
        return this->GetNumTrafficLanes(forward ^ inverted);
    }

    int GetNthTrafficLane(int n, bool forward) const;
    int GetNthTrafficLane(int n, bool forward, bool inverted) const {
        return this->GetNthTrafficLane(n, forward ^ inverted);
    }

    int GetNthTrafficLaneFromCurb(int n, bool forward) const;
    int GetNthTrafficLaneFromCurb(int n, bool forward, bool inverted) const {
        return this->GetNthTrafficLaneFromCurb(n, forward ^ inverted);
    }

    int GetNumLanes(bool forward) const {
        return forward ? this->GetNumForwardLanes() : this->GetNumBackwardLanes();
    }

    int GetNthLane(int n, bool forward) const {
        return forward ? this->GetNthForwardLane(n) : this->GetNthBackwardLane(n);
    }

    int GetNthLane(int n, bool forward, bool inverted) const {
        return this->GetNthLane(n, forward ^ inverted);
    }

    int GetNumLanes(bool forward, bool inverted) const {
        return this->GetNumLanes(forward ^ inverted);
    }

    unsigned char fNumZones;   // offset 0x0, size 0x1
    unsigned char fMiddleZone; // offset 0x1, size 0x1

  private:
    unsigned char nPadding[2]; // offset 0x2, size 0x2
    WRoadLane mLanes[15];      // offset 0x4, size 0x3C
};

static const unsigned short kRoadSegmentDecision = 1 << 0;
static const unsigned short kRoadSegmentNoTraffic = 1 << 1;
static const unsigned short kRoadSegmentRaceRouteForward = 1 << 2;
static const unsigned short kRoadSegmentIntersection = 1 << 3;
static const unsigned short kRoadSegmentEntrance = 1 << 4;
static const unsigned short kRoadSegmentCopsXorTraffic = 1 << 5;
static const unsigned short kRoadSegmentOneWay = 1 << 6;
static const unsigned short kRoadSegmentShortcut = 1 << 7;
static const unsigned short kRoadSegmentCurved = 1 << 8;
static const unsigned short kRoadSegmentEndInverted = 1 << 9;
static const unsigned short kRoadSegmentStartInverted = 1 << 10;
static const unsigned short kRoadSegmentChopperStayLow = 1 << 11;
static const unsigned short kRoadSegmentCrossesBarrier = 1 << 12;
static const unsigned short kRoadSegmentCrossesDriveThroughBarrier = 1 << 13;
static const unsigned short kRoadSegmentLaneMap = 1 << 14;
static const unsigned short kRoadSegmentInRace = 1 << 15;

// total size: 0x16
struct WRoadSegment {
    bool IsDecision() const {
        return (this->fFlags & kRoadSegmentDecision) != 0;
    }

    bool IsEntrance() const {
        return (this->fFlags & kRoadSegmentEntrance) != 0;
    }

    bool CopsXorTraffic() const {
        return (this->fFlags & kRoadSegmentCopsXorTraffic) != 0;
    }

    bool IsTrafficAllowed() const {
        return (this->fFlags & kRoadSegmentNoTraffic) == 0;
    }

    bool ShouldCopsConsider() const {
        return this->IsTrafficAllowed() ^ this->CopsXorTraffic();
    }

    bool RaceRouteForward() const {
        return (this->fFlags & kRoadSegmentRaceRouteForward) != 0;
    }

    void SetRaceRouteForward(bool forward) {
        if (forward) {
            this->fFlags |= kRoadSegmentRaceRouteForward;
        } else {
            this->fFlags &= ~kRoadSegmentRaceRouteForward;
        }
    }

    bool ShouldChopperStayLow() const {
        return (this->fFlags & kRoadSegmentChopperStayLow) != 0;
    }

    bool CrossesBarrier() const {
        return (this->fFlags & kRoadSegmentCrossesBarrier) != 0;
    }

    bool CrossesDriveThroughBarrier() const {
        return (this->fFlags & kRoadSegmentCrossesDriveThroughBarrier) != 0;
    }

    bool CrossesBarrier(bool player) const {
        bool ret = this->CrossesBarrier();
        if (player) {
            ret |= this->CrossesDriveThroughBarrier();
        }
        return ret;
    }

    void SetCrossesBarrier(bool violates) {
        if (violates) {
            this->fFlags |= kRoadSegmentCrossesBarrier;
        } else {
            this->fFlags &= ~kRoadSegmentCrossesBarrier;
        }
    }

    void SetCrossesDriveThroughBarrier(bool violates) {
        if (violates) {
            this->fFlags |= kRoadSegmentCrossesDriveThroughBarrier;
        } else {
            this->fFlags &= ~kRoadSegmentCrossesDriveThroughBarrier;
        }
    }

    float GetLength() const {
        return static_cast<float>(this->nLength) * (1000.0f / 65535.0f);
    }

    void SetLength(float length) {
        this->nLength = static_cast<unsigned short>(length);
    }

    bool IsInRace() const {
        return (this->fFlags & kRoadSegmentInRace) != 0;
    }

    void SetInRace(bool in_race) {
        if (in_race) {
            this->fFlags |= kRoadSegmentInRace;
        } else {
            this->fFlags &= ~kRoadSegmentInRace;
        }
    }

    bool IsShortcut() const {
        return (this->fFlags & kRoadSegmentShortcut) != 0;
    }

    void SetShortcut(bool shortcut) {
        if (shortcut) {
            this->fFlags |= kRoadSegmentShortcut;
        } else {
            this->fFlags &= ~kRoadSegmentShortcut;
        }
    }

    bool IsOneWay() const {
        return (this->fFlags & kRoadSegmentOneWay) != 0;
    }

    // void SetOneWay(bool one_way) {}

    bool IsEndInverted() const {
        return (this->fFlags & kRoadSegmentEndInverted) != 0;
    }

    bool IsStartInverted() const {
        return (this->fFlags & kRoadSegmentStartInverted) != 0;
    }

    bool IsProfileInverted(int which_end) const {
        return which_end == 0 ? this->IsStartInverted() : this->IsEndInverted();
    }

    // void SetEndInverted(bool inverted) {}

    // void SetStartInverted(bool inverted) {}

    // void SetProfileInverted(int which_end, bool inverted) {}

    void GetEndControl(UMath::Vector3 &v) const {
        float scale = static_cast<float>(this->fEndHandleLength) * (500.0f / (127.0f * 65535.0f));
        float x = scale * this->vEndHandle[0];
        float y = scale * this->vEndHandle[1];
        float z = scale * this->vEndHandle[2];
        v = UMath::Vector3Make(x, y, z);
    }

    void GetStartControl(UMath::Vector3 &v) const {
        float scale = static_cast<float>(this->fStartHandleLength) * (500.0f / (127.0f * 65535.0f));
        float x = scale * this->vStartHandle[0];
        float y = scale * this->vStartHandle[1];
        float z = scale * this->vStartHandle[2];
        v = UMath::Vector3Make(x, y, z);
    }

    void GetEndRightVec(UMath::Vector3 &v) const {
        const float scale = -1.0f / 127.0f;
        float x = this->vEndHandle[0];
        float z = this->vEndHandle[2];
        v = UMath::Vector3Make(scale * z, 0.0f, -scale * x);
    }

    // void GetEndRightVec(UMath::Vector2 &v) const {}

    void GetStartRightVec(UMath::Vector3 &v) const {
        const float scale = 1.0f / 127.0f;
        float x = this->vStartHandle[0];
        float z = this->vStartHandle[2];
        v = UMath::Vector3Make(scale * z, 0.0f, -scale * x);
    }

    // void GetStartRightVec(UMath::Vector2 &v) const {}

    void GetEndForwardVec(UMath::Vector2 &v) const {
        const float scale = -1.0f / 127.0f;
        float x = this->vEndHandle[0];
        float z = this->vEndHandle[2];
        v = UMath::Vector2Make(scale * x, scale * z);
    }

    void GetStartForwardVec(UMath::Vector2 &v) const {
        const float scale = 1.0f / 127.0f;
        float x = scale * static_cast<float>(this->vStartHandle[0]);
        float z = scale * static_cast<float>(this->vStartHandle[2]);
        v = UMath::Vector2Make(x, z);
    }

    void GetEndForwardVec(UMath::Vector3 &v) const {
        const float scale = -1.0f / 127.0f;
        float x = scale * static_cast<float>(this->vEndHandle[0]);
        float y = scale * static_cast<float>(this->vEndHandle[1]);
        float z = scale * static_cast<float>(this->vEndHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    void GetStartForwardVec(UMath::Vector3 &v) const {
        const float scale = 1.0f / 127.0f;
        float x = scale * static_cast<float>(this->vStartHandle[0]);
        float y = scale * static_cast<float>(this->vStartHandle[1]);
        float z = scale * static_cast<float>(this->vStartHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    void GetControl(int which_end, UMath::Vector3 &v) const {
        if (which_end == 0) {
            this->GetStartControl(v);
        } else {
            this->GetEndControl(v);
        }
    }

    // void GetRightVec(int which_end, UMath::Vector2 &v) const {}

    void GetRightVec(int which_end, UMath::Vector3 &v) const {
        if (which_end == 0) {
            this->GetStartRightVec(v);
        } else {
            this->GetEndRightVec(v);
        }
    }

    void GetForwardVec(int which_end, UMath::Vector3 &v) const {
        if (which_end == 0) {
            this->GetStartForwardVec(v);
        } else {
            this->GetEndForwardVec(v);
        }
    }

    void GetForwardVec(int which_end, UMath::Vector2 &v) const {
        if (which_end == 0) {
            this->GetStartForwardVec(v);
        } else {
            this->GetEndForwardVec(v);
        }
    }

    // void SetEndControl(UMath::Vector3 &v) {}

    // void SetStartControl(UMath::Vector3 &v) {}

    // void SetControl(int which_end, UMath::Vector3 &v) {}

    unsigned short fNodeIndex[2]; // offset 0x0, size 0x4
#ifdef EA_BUILD_A124
    short fDistIndex[2];
    short fIntersectionIndex[2];
#endif
    unsigned short nLength;            // offset 0x4, size 0x2
    short fRoadID;                     // offset 0x6, size 0x2
    short fIndex;                      // offset 0x8, size 0x2
    unsigned short fFlags;             // offset 0xA, size 0x2
    unsigned short fEndHandleLength;   // offset 0xC, size 0x2
    unsigned short fStartHandleLength; // offset 0xE, size 0x2
    char vEndHandle[3];                // offset 0x10, size 0x3
    char vStartHandle[3];              // offset 0x13, size 0x3
#ifdef EA_BUILD_A124
    char fNumLeftLanes;
    char fNumRightLanes;
#endif
};

// total size: 0x20
struct WRoadNode {
    bool IsSegment(unsigned short segment_id) const {
        for (int i = 0; i < static_cast<int>(this->fNumSegments); i++) {
            if (this->fSegmentIndex[i] == segment_id) {
                return true;
            }
        }
        return false;
    }

    UMath::Vector3 fPosition;        // offset 0x0, size 0xC
    short fIndex;                    // offset 0xC, size 0x2
    short fProfileIndex;             // offset 0xE, size 0x2
    unsigned char fNumSegments;      // offset 0x10, size 0x1
    unsigned short fSegmentIndex[7]; // offset 0x12, size 0xE
};

// total size: 0x40
struct WRoadIntersection {
    UMath::Vector3 fPosition;    // offset 0x0, size 0xC
    short fIndex;                // offset 0xC, size 0x2
    unsigned char fNumConnected; // offset 0xE, size 0x1
    short fConnected[7];         // offset 0x10, size 0xE
    unsigned char fNumEntrances; // offset 0x1E, size 0x1
    short fEntrances[7];         // offset 0x20, size 0xE
    unsigned char fOccupied[7];  // offset 0x2E, size 0x7
    char fPad[9];                // offset 0x35, size 0x9
};

#endif
