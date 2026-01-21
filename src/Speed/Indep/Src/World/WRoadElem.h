#ifndef WORLD_WROADELEM_H
#define WORLD_WROADELEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x8
struct WRoad {
    // float GetScale() const {}

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
    // int GetType() const {}

    // float GetWidth() const {}

    // float GetOffset() const {}

    // void SetType(int type) {}

    // void SetWidth(float width) {}

    // void SetOffset(float offset) {}

    // unsigned int &GetBits() {}

    // void SwapEndian() {}

    // unsigned int GetBits(int n_offset, int n_bits) const {}

    // int GetBitsSigned(int n_offset, int n_bits) const {}

    // void SetBits(int n_offset, int n_bits, int n_value) {}

    unsigned int nBits; // offset 0x0, size 0x4
};

// total size: 0x40
struct WRoadProfile {
    // int GetMiddleZone(bool inverted) const {}

    // int GetLaneNumber(int lane, bool inverted) const {}

    // float GetLaneWidth(int lane, bool inverted) const {}

    // void SetLaneWidth(int lane, float width, bool inverted) {}

    // float GetLaneOffset(int lane, bool inverted) const {}

    // void SetLaneOffset(int lane, float width, bool inverted) {}

    // int GetLaneType(int lane, bool inverted) const {}

    // void SetLaneType(int lane, int type, bool inverted) {}

    // float GetRawLaneOffset(int lane) const {}

    // float GetRawLaneWidth(int lane) const {}

    // float GetRelativeLaneOffset(int lane, bool inverted) const {}

    // unsigned int &GetLaneBits(int lane, bool inverted) {}

    // int GetNumForwardLanes() const {}

    // int GetNumBackwardLanes() const {}

    // int GetNumForwardLanes(bool inverted) const {}

    // int GetNumBackwardLanes(bool inverted) const {}

    // int GetNthForwardLane(int n) const {}

    // int GetNthBackwardLane(int n) const {}

    // int GetNthForwardLane(int n, bool inverted) const {}

    // int GetNthBackwardLane(int n, bool inverted) const {}

    // int GetNumTrafficLanes(bool forward, bool inverted) const {}

    // int GetNthTrafficLane(int n, bool forward, bool inverted) const {}

    // int GetNthTrafficLaneFromCurb(int n, bool forward, bool inverted) const {}

    // int GetNumLanes(bool forward) const {}

    // int GetNumLanes(bool forward, bool inverted) const {}

    // int GetNthLane(int n, bool forward) const {}

    // int GetNthLane(int n, bool forward, bool inverted) const {}

    // float GetNthOffset(int n, bool forward) const {}

    // float GetNthOffset(int n, bool forward, bool inverted) const {}

    // float GetNthWidth(int n, bool forward) const {}

    // float GetNthWidth(int n, bool forward, bool inverted) const {}

    // float GetEntireWidth() const {}

    // static short ScaleToProfile(float value) {}

    // void InvertProfile(WRoadProfile &dest) const {}

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

// total size: 0x16
struct WRoadSegment {
    bool IsDecision() const {
        return fFlags & 1;
    }

    // bool IsEntrance() const {}

    // bool CopsXorTraffic() const {}

    // bool IsTrafficAllowed() const {}

    // bool ShouldCopsConsider() const {}

    // bool RaceRouteForward() const {}

    // void SetRaceRouteForward(bool forward) {}

    // bool ShouldChopperStayLow() const {}

    // bool CrossesBarrier() const {}

    // bool CrossesDriveThroughBarrier() const {}

    // bool CrossesBarrier(bool player) const {}

    // void SetCrossesBarrier(bool violates) {}

    // void SetCrossesDriveThroughBarrier(bool violates) {}

    // float GetLength() const {}

    // void SetLength(float length) {}

    bool IsInRace() const {
        return fFlags & (1 << 15);
    }

    // void SetInRace(bool in_race) {}

    // bool IsShortcut() const {}

    // void SetShortcut(bool shortcut) {}

    // bool IsOneWay() const {}

    // void SetOneWay(bool one_way) {}

    // bool IsEndInverted() const {}

    // bool IsStartInverted() const {}

    // bool IsProfileInverted(int which_end) const {}

    // void SetEndInverted(bool inverted) {}

    // void SetStartInverted(bool inverted) {}

    // void SetProfileInverted(int which_end, bool inverted) {}

    // void GetEndControl(UMath::Vector3 &v) const {}

    // void GetStartControl(UMath::Vector3 &v) const {}

    // void GetEndRightVec(UMath::Vector3 &v) const {}

    // void GetEndRightVec(UMath::Vector2 &v) const {}

    // void GetStartRightVec(UMath::Vector3 &v) const {}

    // void GetStartRightVec(UMath::Vector2 &v) const {}

    // void GetEndForwardVec(UMath::Vector2 &v) const {}

    // void GetEndForwardVec(UMath::Vector3 &v) const {}

    // void GetStartForwardVec(UMath::Vector2 &v) const {}

    // void GetStartForwardVec(UMath::Vector3 &v) const {}

    // void GetControl(int which_end, UMath::Vector3 &v) const {}

    // void GetRightVec(int which_end, UMath::Vector2 &v) const {}

    // void GetRightVec(int which_end, UMath::Vector3 &v) const {}

    // void GetForwardVec(int which_end, UMath::Vector2 &v) const {}

    // void GetForwardVec(int which_end, UMath::Vector3 &v) const {}

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
