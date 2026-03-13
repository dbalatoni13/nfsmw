#include "AnimCandidates.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

int SpecialCarList1[] = {
    0x9537ACB7,
	0x5D6E3E54,
	0x5D6E3E54,
	0xE6D4BEE9,
	0xA3A1D182,
	0x3E8EA090,
	0x43CE4BC3,
	0xD601789E
};

int SpecialCarList2[] = {
	0x5D6E3E54,
	0x5D6E3E54,
	0x531A3488,
	0xEBAF2926,
	0xE6D4BEE9,
	0x43CE4BC3,
	0x3E8EA090,
	0xD601789E,
};

int SpecialCarList3[] = {
	0x5D6E3E54,
	0x5D6E3E54,
	0x5D6E3E54,
	0x38B38226,
	0x5D6E3E54,
	0x38B38226,
	0x9537ACB7,
	0x0A92CC67,
};

int SpecialCarList4[] = {
	0xA2D70BBD,
	0x5F3B560D,
	0x531A3488,
	0x531A3488,
	0xEBAF2926,
	0xE6D4BEE9,
	0xA9B509C4,
	0xEBAF2926,
};

int SpecialCarList5[] = {
	0xA2D70BBD,
	0xA2D70BBD,
	0x0A92CC67,
	0x531A3488,
	0x5EC98756,
	0xE6D4BEE9,
	0x9537ACB7,
	0x43CE4BC3,
};

int SpecialCarList6[] = {
	0xA4EB6688,
	0xA4EB6688,
	0xA4EB6688,
	0xA4EB6688,
	0xA4EB6688,
	0xA4EB6688,
	0xA4EB6688,
	0xD37C806D,
};

int SpecialCarList7[] = {
	0x5D6E3E54,
	0x5D6E3E54,
	0x5D6E3E54,
	0x43CE4BC3,
	0x531A3488,
	0x5EC98756,
	0xE6D4BEE9,
	0xEBAF2926,
};

int SpecialCarList8[] = {
	0x6F67F7F5,
	0x43CE4BC3,
	0xEBAF2926,
	0xE6D4BEE9,
	0x531A3488,
	0x5EC98756,
	0x43CE4BC3,
	0xEBAF2926,
};

int SpecialCarList9[] = {
	0x5D6E3E54,
	0x5D6E3E54,
	0x5D6E3E54,
	0xE6D4BEE9,
	0x531A3488,
	0x5EC98756,
	0x43CE4BC3,
	0xEBAF2926,
};

CAnimCandidateData *TheAnimCandidateData = nullptr;

int CAnimCandidateData::GetSceneMomentMarkerType(unsigned int sceneHash) {
    for (int slotIndex = 0; slotIndex < (int)mMomentAnimCount; slotIndex++) {
        if (sceneHash == mMomentAnim[slotIndex].mSceneHash) {
            return mMomentAnim[slotIndex].mMomentType;
        }
    }
    return -1;
}

static const char MarkerNameList[21][28] = {
    "CameraCutMoment",      "GasStationMoment",    "FirehallMoment",        "PoliceStationMoment",
    "WaterTowerMoment",     "PorchCollapseMoment", "SignageCollapseMoment", "ScaffoldingCollapseMoment",
    "GazeboCollapseMoment", "FishMarketMoment",    "BoatKnockdownMoment",   "CopHitsObstructionMoment",
    "BustedMomentMarker",   "TollboothMarker",     "GarageMarker",          "PoliceStationGroundMoment",
    "DriveInMoment",        "TrailerParkMoment",   "TorusRoofMoment",       "AmphitheatreMoment",
    "StripMallMoment",
};

const char *CAnimCandidateData::GetMomentMarkerName(int markerType) {
    if ((unsigned int)markerType < NumMomentMarkerTypes) {
        return MarkerNameList[markerType];
    } else {
        return "";
    }
}

float AngleBonusValue = 25.0f;

TrackPositionMarker *CAnimCandidateData::GetClosestMarker(unsigned int sceneHash, bVector3 &position, int *ID, float *markerDist,
                                                          unsigned short angle) {
    TrackPositionMarker *closestMarker = nullptr;
    int markerType = GetSceneMomentMarkerType(sceneHash);
    if (markerType != -1) {
        const char *markerName = GetMomentMarkerName(markerType);
        if (*markerName != '\0') {
            unsigned int markerHash = bStringHash(markerName);
            int numTrackMarkers = GetNumTrackPositionMarkers(0, markerHash);
            float closestMarkerDist = -1.0f;
            int markersIndex = 0;
            for (TrackPositionMarker *p = TrackPositionMarkerList.GetHead(); p != TrackPositionMarkerList.EndOfList(); p = p->GetNext()) {
                if (p->NameHash == markerHash) {
                    unsigned short angleBetween = p->Angle - angle;
                    if (angleBetween > 32768) {
                        angleBetween = angle - p->Angle;
                    }
                    const float bAngle360 = bDegToAng(angle) - bDegToAng(p->Angle);
                    float angleBonus = angleBetween * 1.5258789e-05f * AngleBonusValue;
                    float dist = bDistBetween(p->Position, position) + angleBonus;
                    if (!closestMarker || (dist < closestMarkerDist)) {
                        closestMarkerDist = dist;
                        closestMarker = p;
                        if (ID) {
                            *ID = markersIndex;
                        }
                        if (markerDist) {
                            *markerDist = dist;
                        }
                    }
                    markersIndex++;
                }
            }
        }
    }
    return closestMarker;
}
