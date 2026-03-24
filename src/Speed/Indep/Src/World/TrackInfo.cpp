#include "TrackInfo.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static TrackInfo *TrackInfoTable = 0;
static unsigned int NumTrackInfo = 0;
TrackInfo *LoadedTrackInfo = 0;
bChunkLoader bChunkLoaderTrackInfo(0x34201, TrackInfo::LoaderTrackInfo, TrackInfo::UnloaderTrackInfo);

TrackInfo *TrackInfo::GetTrackInfo(int track_number) {
    for (int n = 0; n < static_cast<int>(NumTrackInfo); n++) {
        TrackInfo *info = &TrackInfoTable[n];
        if (info->TrackNumber == track_number) {
            return info;
        }
    }

    return 0;
}

int TrackInfo::LoaderTrackInfo(bChunk *chunk) {
    int i;
    int j;

    if (chunk->GetID() == 0x34201) {
        TrackInfoTable = reinterpret_cast<TrackInfo *>(chunk->GetData());
        NumTrackInfo = chunk->GetSize() / sizeof(TrackInfo);

        for (int n = 0; n < static_cast<int>(NumTrackInfo); n++) {
            TrackInfo *info = &TrackInfoTable[n];
            bPlatEndianSwap(&info->LocationNumber);
            bPlatEndianSwap(reinterpret_cast<int *>(&info->LocationName));
            bPlatEndianSwap(reinterpret_cast<int *>(&info->DriftType));
            bPlatEndianSwap(&info->TrackNumber);
            bPlatEndianSwap(&info->SameAsTrackNumber);
            bPlatEndianSwap(&info->TimeToBeatForwards_ms);
            bPlatEndianSwap(&info->TimeToBeatReverse_ms);
            bPlatEndianSwap(&info->ScoreToBeatForwards_DriftOnly);
            bPlatEndianSwap(&info->ScoreToBeatReverse_DriftOnly);
            bPlatEndianSwap(&info->SunInfoNameHash);
            bPlatEndianSwap(&info->UsageFlags);
            bPlatEndianSwap(&info->TrackMapCalibrationUpperLeft);
            bPlatEndianSwap(&info->TrackMapCalibrationMapWidthMetres);
            bPlatEndianSwap(&info->TrackMapCalibrationRotation);
            bPlatEndianSwap(&info->TrackMapStartLineAngle);
            bPlatEndianSwap(&info->TrackMapFinishLineAngle);
            bPlatEndianSwap(reinterpret_cast<int *>(&info->ForwardDifficulty));
            bPlatEndianSwap(reinterpret_cast<int *>(&info->ReverseDifficulty));
            bPlatEndianSwap(&info->NumSecondsBeforeShortcutsAllowed);
            bPlatEndianSwap(&info->nDriftSecondsMin);
            bPlatEndianSwap(&info->nDriftSecondsMax);

            i = 0;
            do {
                j = 0;
                do {
                    bPlatEndianSwap(&info->OverrideStartingRouteForAI[i][j]);
                    j += 1;
                } while (j < 4);
                i += 1;
            } while (i < 2);

            i = 0;
            do {
                j = 0;
                do {
                    bPlatEndianSwap(&info->MaxTrafficCars[i][j]);
                    j += 1;
                } while (j < 2);
                i += 1;
            } while (i < 4);

            i = 0;
            do {
                bPlatEndianSwap(&info->TrafficAllowedNearStartLine[i]);
                i += 1;
            } while (i < 2);

            i = 0;
            do {
                bPlatEndianSwap(&info->TrafficMinInitialDistanceBetweenCars[i]);
                i += 1;
            } while (i < 2);

            i = 0;
            do {
                bPlatEndianSwap(&info->TrafficMinInitialDistanceFromStartLine[i]);
                i += 1;
            } while (i < 2);
        }

        return 1;
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
