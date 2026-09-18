#include "AnimCandidates.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle_hash.h"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

Attrib::Key SpecialCarList1[8] = {
    Attrib::Hash::pvehicle::key_copmidsize_nis, Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld,
    Attrib::Hash::pvehicle::key_trafminivan,    Attrib::Hash::pvehicle::key_copghost,          Attrib::Hash::pvehicle::key_trafsuva,
    Attrib::Hash::pvehicle::key_trafstwag,      Attrib::Hash::pvehicle::key_traftaxi,
};

Attrib::Key SpecialCarList2[8] = {
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_trafficcoup,
    Attrib::Hash::pvehicle::key_trafpizza,         Attrib::Hash::pvehicle::key_trafminivan,       Attrib::Hash::pvehicle::key_trafstwag,
    Attrib::Hash::pvehicle::key_trafsuva,          Attrib::Hash::pvehicle::key_traftaxi,
};

Attrib::Key SpecialCarList3[8] = {
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld,
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copsuv,
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copsuv,
    Attrib::Hash::pvehicle::key_copmidsize_nis,    Attrib::Hash::pvehicle::key_semilog,
};

Attrib::Key SpecialCarList4[8] = {
    Attrib::Hash::pvehicle::key_eclipsegt,   Attrib::Hash::pvehicle::key_imprezawrx, Attrib::Hash::pvehicle::key_trafficcoup,
    Attrib::Hash::pvehicle::key_trafficcoup, Attrib::Hash::pvehicle::key_trafpizza,  Attrib::Hash::pvehicle::key_trafminivan,
    Attrib::Hash::pvehicle::key_semib,       Attrib::Hash::pvehicle::key_trafpizza,
};

Attrib::Key SpecialCarList5[8] = {
    Attrib::Hash::pvehicle::key_eclipsegt,      Attrib::Hash::pvehicle::key_eclipsegt,  Attrib::Hash::pvehicle::key_semilog,
    Attrib::Hash::pvehicle::key_trafficcoup,    Attrib::Hash::pvehicle::key_traf4dsedc, Attrib::Hash::pvehicle::key_trafminivan,
    Attrib::Hash::pvehicle::key_copmidsize_nis, Attrib::Hash::pvehicle::key_trafstwag,
};

Attrib::Key SpecialCarList6[8] = {
    Attrib::Hash::pvehicle::key_copsportghost, Attrib::Hash::pvehicle::key_copsportghost, Attrib::Hash::pvehicle::key_copsportghost,
    Attrib::Hash::pvehicle::key_copsportghost, Attrib::Hash::pvehicle::key_copsportghost, Attrib::Hash::pvehicle::key_copsportghost,
    Attrib::Hash::pvehicle::key_copsportghost, Attrib::Hash::pvehicle::key_copcross,
};

Attrib::Key SpecialCarList7[8] = {
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld,
    Attrib::Hash::pvehicle::key_trafstwag,         Attrib::Hash::pvehicle::key_trafficcoup,       Attrib::Hash::pvehicle::key_traf4dsedc,
    Attrib::Hash::pvehicle::key_trafminivan,       Attrib::Hash::pvehicle::key_trafpizza,
};

Attrib::Key SpecialCarList8[8] = {
    Attrib::Hash::pvehicle::key_trafamb,     Attrib::Hash::pvehicle::key_trafstwag,   Attrib::Hash::pvehicle::key_trafpizza,
    Attrib::Hash::pvehicle::key_trafminivan, Attrib::Hash::pvehicle::key_trafficcoup, Attrib::Hash::pvehicle::key_traf4dsedc,
    Attrib::Hash::pvehicle::key_trafstwag,   Attrib::Hash::pvehicle::key_trafpizza,
};

Attrib::Key SpecialCarList9[8] = {
    Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld, Attrib::Hash::pvehicle::key_copmidsize_nis_ld,
    Attrib::Hash::pvehicle::key_trafminivan,       Attrib::Hash::pvehicle::key_trafficcoup,       Attrib::Hash::pvehicle::key_traf4dsedc,
    Attrib::Hash::pvehicle::key_trafstwag,         Attrib::Hash::pvehicle::key_trafpizza,
};

CAnimCandidateData *TheAnimCandidateData = nullptr;

int CAnimCandidateData::GetSceneMomentMarkerType(uint32 sceneHash) {
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
    if ((uint32)markerType < NumMomentMarkerTypes) {
        return MarkerNameList[markerType];
    } else {
        return "";
    }
}

float AngleBonusValue = 25.0f;

TrackPositionMarker *CAnimCandidateData::GetClosestMarker(uint32 sceneHash, bVector3 &position, int *ID, float *markerDist, bAngle angle) {
    TrackPositionMarker *closestMarker = nullptr;
    int markerType = GetSceneMomentMarkerType(sceneHash);
    if (markerType != -1) {
        const char *markerName = GetMomentMarkerName(markerType);
        if (*markerName != '\0') {
            uint32 markerHash = bStringHash(markerName);
            int numTrackMarkers = GetNumTrackPositionMarkers(0, markerHash);
            float closestMarkerDist = -1.0f;
            int markersIndex = 0;
            for (TrackPositionMarker *p = TrackPositionMarkerList.GetHead(); p != TrackPositionMarkerList.EndOfList(); p = p->GetNext()) {
                if (p->NameHash == markerHash) {
                    bAngle angleBetween = p->Angle - angle;
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
