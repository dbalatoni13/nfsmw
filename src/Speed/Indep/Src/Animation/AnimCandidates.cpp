#include "AnimCandidates.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

CAnimCandidateData *TheAnimCandidateData;

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
