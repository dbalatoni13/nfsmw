#include "TrackInfo.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void bEndianSwap16(void *value);
void bEndianSwap32(void *value);
void bPlatEndianSwap(bVector2 *value);

static TrackInfo *TrackInfoTable = 0;
static unsigned int NumTrackInfo = 0;
TrackInfo *LoadedTrackInfo = 0;
bChunkLoader bChunkLoaderTrackInfo(0x34201, TrackInfo::LoaderTrackInfo, TrackInfo::UnloaderTrackInfo);

int TrackInfo::LoaderTrackInfo(bChunk *chunk) {
    if (chunk->GetID() == 0x34201) {
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

            int n = 0;
            do {
                short *override_starting_route_for_ai = &info->OverrideStartingRouteForAI[n][0];
                int m = 0;
                do {
                    bEndianSwap16(&override_starting_route_for_ai[m]);
                    m += 1;
                } while (m <= 3);
                n += 1;
            } while (n <= 1);

            n = 0;
            do {
                bEndianSwap32(&info->TrafficMinInitialDistanceBetweenCars[n]);
                n += 1;
            } while (n <= 1);

            n = 0;
            do {
                bEndianSwap32(&info->TrafficMinInitialDistanceFromStartLine[n]);
                n += 1;
            } while (n <= 1);
        }

        return 1;
    }
    return 0;
}

TrackInfo *TrackInfo::GetTrackInfo(int track_number) {
    for (int n = 0; n < static_cast<int>(NumTrackInfo); n++) {
        TrackInfo *info = &TrackInfoTable[n];
        if (info->TrackNumber == track_number) {
            return info;
        }
    }

    return 0;
}

int TrackInfo::UnloaderTrackInfo(bChunk *chunk) {
    if (chunk->GetID() == 0x34201) {
        TrackInfoTable = 0;
        NumTrackInfo = 0;
        return 1;
    }
    return 0;
}
