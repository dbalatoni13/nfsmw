#include "TrackInfo.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void bEndianSwap16(void *value);
void bEndianSwap32(void *value);
void bPlatEndianSwap(bVector2 *value);

static TrackInfo *TrackInfoTable = 0;
static unsigned int NumTrackInfo = 0;
TrackInfo *LoadedTrackInfo = 0;

int TrackInfo::LoaderTrackInfo(bChunk *chunk) {
    if (chunk->GetID() != 0x34201) {
        return 0;
    }

    TrackInfoTable = reinterpret_cast<TrackInfo *>(chunk->GetData());
    NumTrackInfo = chunk->Size / sizeof(TrackInfo);

    for (unsigned int i = 0; i < NumTrackInfo; i++) {
        TrackInfo *info = &TrackInfoTable[i];
        bEndianSwap32(&info->LocationNumber);
        bEndianSwap32(&info->LocationName);
        bEndianSwap32(&info->DriftType);
        bEndianSwap16(&info->TrackNumber);
        bEndianSwap16(&info->SameAsTrackNumber);
        bEndianSwap32(&info->TimeToBeatForwards_ms);
        bEndianSwap32(&info->TimeToBeatReverse_ms);
        bEndianSwap32(&info->ScoreToBeatForwards_DriftOnly);
        bEndianSwap32(&info->ScoreToBeatReverse_DriftOnly);
        bEndianSwap32(&info->SunInfoNameHash);
        bEndianSwap32(&info->UsageFlags);
        bPlatEndianSwap(&info->TrackMapCalibrationUpperLeft);
        bEndianSwap32(&info->TrackMapCalibrationMapWidthMetres);
        bEndianSwap16(&info->TrackMapCalibrationRotation);
        bEndianSwap16(&info->TrackMapStartLineAngle);
        bEndianSwap16(&info->TrackMapFinishLineAngle);
        bEndianSwap32(&info->ForwardDifficulty);
        bEndianSwap32(&info->ReverseDifficulty);
        bEndianSwap16(&info->NumSecondsBeforeShortcutsAllowed);
        bEndianSwap16(&info->nDriftSecondsMin);
        bEndianSwap16(&info->nDriftSecondsMax);

        for (int n = 0; n < 2; n++) {
            for (int m = 0; m < 4; m++) {
                bEndianSwap16(&info->OverrideStartingRouteForAI[n][m]);
            }
        }

        for (int n = 0; n < 2; n++) {
            bEndianSwap32(&info->TrafficMinInitialDistanceBetweenCars[n]);
        }

        for (int n = 0; n < 2; n++) {
            bEndianSwap32(&info->TrafficMinInitialDistanceFromStartLine[n]);
        }
    }

    return 1;
}
