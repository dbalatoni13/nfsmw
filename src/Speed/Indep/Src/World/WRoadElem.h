#ifndef WORLD_WROADELEM_H
#define WORLD_WROADELEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x8
struct WRoad {
    float GetScale() const {
        unsigned int s = static_cast< unsigned int >(nScale) << 8;
        return static_cast< float >(s) * (1.0f / 65536.0f);
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

// total size: 0x20
struct WRoadNode {
    // bool IsSegment(unsigned short segment_id) const {}

    UMath::Vector3 fPosition;        // offset 0x0, size 0xC
    short fIndex;                    // offset 0xC, size 0x2
    short fProfileIndex;             // offset 0xE, size 0x2
    unsigned char fNumSegments;      // offset 0x10, size 0x1
    unsigned short fSegmentIndex[7]; // offset 0x12, size 0xE
};

// total size: 0x4
struct WRoadLane {
    int GetType() const {
        return GetBits(0, 4);
    }

    unsigned int GetBits(int n_offset, int n_bits) const;
    int GetBitsSigned(int n_offset, int n_bits) const;

    float GetWidth() const {
        return static_cast< float >(GetBitsSigned(4, 14)) * (100.0f / 8191.0f);
    }

    float GetOffset() const {
        return static_cast< float >(GetBitsSigned(18, 14)) * (100.0f / 8191.0f);
    }

    unsigned int nBits; // offset 0x0, size 0x4
};

// total size: 0x40
struct WRoadProfile {
    int GetLaneType(int lane, bool inverted) const {
        int lane_number;
        if (inverted) {
            lane_number = fNumZones - lane - 1;
        } else {
            lane_number = lane;
        }
        return mLanes[lane_number].GetType();
    }

    int GetNumForwardLanes() const { return fNumZones - fMiddleZone; }
    int GetNumBackwardLanes() const { return fMiddleZone; }
    int GetNumLanes(bool forward) const {
        if (forward) {
            return GetNumForwardLanes();
        }
        return GetNumBackwardLanes();
    }
    int GetMiddleZone(bool inverted) const {
        if (inverted) {
            return fNumZones - fMiddleZone;
        }
        return fMiddleZone;
    }
    int GetNthForwardLane(int n) const {
        int ret = fMiddleZone - n - 1;
        if (ret < 0) {
            ret = 0;
        }
        return ret;
    }
    int GetNthBackwardLane(int n) const {
        int ret = fMiddleZone + n;
        if (ret >= fNumZones) {
            ret = fNumZones - 1;
        }
        return ret;
    }
    int GetNthLane(int n, bool forward) const {
        if (forward) {
            return GetNthForwardLane(n);
        }
        return GetNthBackwardLane(n);
    }
    int GetLaneNumber(int lane, bool inverted) const {
        return inverted ? fNumZones - lane - 1 : lane;
    }
    int GetNumTrafficLanes(bool forward) const;
    int GetNthTrafficLane(int n, bool forward) const;
    int GetNthTrafficLaneFromCurb(int n, bool forward) const;

    int GetNumTrafficLanes(bool forward, bool inverted) const {
        if (inverted) {
            return GetNumTrafficLanes(!forward);
        }
        return GetNumTrafficLanes(forward);
    }
    int GetNthTrafficLane(int n, bool forward, bool inverted) const {
        if (inverted) {
            return GetNthTrafficLane(n, !forward);
        }
        return GetNthTrafficLane(n, forward);
    }
    int GetNthTrafficLaneFromCurb(int n, bool forward, bool inverted) const {
        if (inverted) {
            return GetNthTrafficLaneFromCurb(n, !forward);
        }
        return GetNthTrafficLaneFromCurb(n, forward);
    }
    float GetLaneOffset(int lane, bool inverted) const {
        return mLanes[GetLaneNumber(lane, inverted)].GetOffset();
    }
    float GetLaneWidth(int lane, bool inverted) const {
        int lane_number = GetLaneNumber(lane, inverted);
        return mLanes[lane_number].GetWidth();
    }
    float GetRelativeLaneOffset(int lane, bool inverted) const {
        int real_middle = GetMiddleZone(inverted);
        float offset = GetLaneOffset(lane, inverted);
        if (lane < real_middle) {
            offset = -offset;
        }
        return offset;
    }

    float GetRawLaneOffset(int lane) const {
        return mLanes[lane].GetOffset();
    }
    float GetRawLaneWidth(int lane) const {
        return mLanes[lane].GetWidth();
    }

    unsigned char fNumZones;   // offset 0x0, size 0x1
    unsigned char fMiddleZone; // offset 0x1, size 0x1
    unsigned char nPadding[2]; // offset 0x2, size 0x2
    WRoadLane mLanes[15];      // offset 0x4, size 0x3C
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

// total size: 0x16
struct WRoadSegment {
    bool IsDecision() const {
        return fFlags & 1;
    }

    // bool IsEntrance() const {}

    bool CopsXorTraffic() const {
        return fFlags & (1 << 5);
    }

    bool IsTrafficAllowed() const {
        return !(fFlags & 2);
    }

    bool ShouldCopsConsider() const {
        return IsTrafficAllowed() ^ CopsXorTraffic();
    }

    bool RaceRouteForward() const {
        return fFlags & (1 << 2);
    }

    // void SetRaceRouteForward(bool forward) {}

    // bool ShouldChopperStayLow() const {}

    // bool CrossesBarrier() const {}

    // bool CrossesDriveThroughBarrier() const {}

    // bool CrossesBarrier(bool player) const {}

    // void SetCrossesBarrier(bool violates) {}

    // void SetCrossesDriveThroughBarrier(bool violates) {}

    float GetLength() const {
        return static_cast< float >(nLength) * (1000.0f / 65535.0f);
    }

    // void SetLength(float length) {}

    bool IsInRace() const {
        return fFlags & (1 << 15);
    }

    void SetInRace(bool in_race) {
        if (in_race) {
            fFlags |= (1 << 15);
        } else {
            fFlags &= ~(1 << 15);
        }
    }

    void SetRaceRouteForward(bool forward) {
        if (forward) {
            fFlags |= (1 << 2);
        } else {
            fFlags &= ~(1 << 2);
        }
    }

    bool CrossesBarrier() const {
        return fFlags & (1 << 12);
    }

    bool CrossesDriveThroughBarrier() const {
        return fFlags & (1 << 13);
    }

    void SetCrossesBarrier(bool violates) {
        if (violates) {
            fFlags |= (1 << 12);
        } else {
            fFlags &= ~(1 << 12);
        }
    }

    void SetCrossesDriveThroughBarrier(bool violates) {
        if (violates) {
            fFlags |= (1 << 13);
        } else {
            fFlags &= ~(1 << 13);
        }
    }

    bool IsShortcut() const {
        return (fFlags & 0x80) != 0;
    }

    void SetShortcut(bool shortcut) {
        if (shortcut) {
            fFlags |= 0x80;
        } else {
            fFlags &= ~0x80;
        }
    }

    bool IsOneWay() const {
        return (fFlags & 0x40) != 0;
    }

    bool IsCurved() const {
        return (fFlags & 0x100) != 0;
    }

    // void SetOneWay(bool one_way) {}

    bool IsStartInverted() const {
        return (fFlags >> 9) & 1;
    }

    bool IsEndInverted() const {
        return (fFlags >> 10) & 1;
    }

    bool IsProfileInverted(int which_end) const {
        if (!which_end) {
            return IsEndInverted();
        }
        return IsStartInverted();
    }

    // void SetEndInverted(bool inverted) {}

    // void SetStartInverted(bool inverted) {}

    // void SetProfileInverted(int which_end, bool inverted) {}

    void GetEndControl(UMath::Vector3 &v) const {
        float scale = static_cast< float >(fEndHandleLength) * (500.0f / (127.0f * 65535.0f));
        float x = scale * static_cast< float >(vEndHandle[0]);
        float y = scale * static_cast< float >(vEndHandle[1]);
        float z = scale * static_cast< float >(vEndHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    void GetStartControl(UMath::Vector3 &v) const {
        float scale = static_cast< float >(fStartHandleLength) * (500.0f / (127.0f * 65535.0f));
        float x = scale * static_cast< float >(vStartHandle[0]);
        float y = scale * static_cast< float >(vStartHandle[1]);
        float z = scale * static_cast< float >(vStartHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    // void GetEndRightVec(UMath::Vector3 &v) const {}

    // void GetEndRightVec(UMath::Vector2 &v) const {}

    // void GetStartRightVec(UMath::Vector3 &v) const {}

    // void GetStartRightVec(UMath::Vector2 &v) const {}

    void GetEndForwardVec(UMath::Vector3 &v) const {
        const float scale = -1.0f / 127.0f;
        float x = scale * static_cast< float >(vEndHandle[0]);
        float y = scale * static_cast< float >(vEndHandle[1]);
        float z = scale * static_cast< float >(vEndHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    void GetStartForwardVec(UMath::Vector3 &v) const {
        const float scale = 1.0f / 127.0f;
        float x = scale * static_cast< float >(vStartHandle[0]);
        float y = scale * static_cast< float >(vStartHandle[1]);
        float z = scale * static_cast< float >(vStartHandle[2]);
        v = UMath::Vector3Make(x, y, z);
    }

    // void GetControl(int which_end, UMath::Vector3 &v) const {}

    // void GetRightVec(int which_end, UMath::Vector2 &v) const {}

    // void GetRightVec(int which_end, UMath::Vector3 &v) const {}

    void GetForwardVec(int which_end, UMath::Vector3 &v) const {
        if (which_end == 0) {
            GetStartForwardVec(v);
        } else {
            GetEndForwardVec(v);
        }
    }

    // void SetEndControl(UMath::Vector3 &v) {}

    // void SetStartControl(UMath::Vector3 &v) {}

    // void SetControl(int which_end, UMath::Vector3 &v) {}

    // Members
    unsigned short fNodeIndex[2];      // offset 0x0, size 0x4
    unsigned short nLength;            // offset 0x4, size 0x2
    short fRoadID;                     // offset 0x6, size 0x2
    short fIndex;                      // offset 0x8, size 0x2
    unsigned short fFlags;             // offset 0xA, size 0x2
    unsigned short fEndHandleLength;   // offset 0xC, size 0x2
    unsigned short fStartHandleLength; // offset 0xE, size 0x2
    char vEndHandle[3];                // offset 0x10, size 0x3
    char vStartHandle[3];              // offset 0x13, size 0x3
};

#endif
