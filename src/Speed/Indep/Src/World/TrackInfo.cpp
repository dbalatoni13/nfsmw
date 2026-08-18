#include "TrackInfo.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

TrackInfo *TrackInfo::TrackInfoTable = nullptr;
int TrackInfo::NumTrackInfo = 0;
TrackInfo *TrackInfo::LoadedTrackInfo = nullptr;
bChunkLoader bChunkLoaderTrackInfo(BCHUNK_TRACK_INFOS, TrackInfo::LoaderTrackInfo, TrackInfo::UnloaderTrackInfo);

TrackInfo *TrackInfo::GetTrackInfo(int track_number) {
    for (int n = 0; n < NumTrackInfo; n++) {
        TrackInfo *info = &TrackInfoTable[n];
        if (info->TrackNumber == track_number) {
            return info;
        }
    }

    return nullptr;
}

int TrackInfo::LoaderTrackInfo(bChunk *chunk) {
    int i;
    int j;

    if (chunk->GetID() == BCHUNK_TRACK_INFOS) {
        TrackInfoTable = reinterpret_cast<TrackInfo *>(chunk->GetData());
        NumTrackInfo = chunk->GetSize() / sizeof(TrackInfo);

        for (int n = 0; n < static_cast<int>(NumTrackInfo); n++) {
            TrackInfo *info = &TrackInfoTable[n];
            bPlatEndianSwap(&info->LocationNumber);
            // TODO we have to make sure that enums are 4 bytes long on every platform
            bPlatEndianSwap(reinterpret_cast<int32 *>(&info->LocationName));
            bPlatEndianSwap(reinterpret_cast<int32 *>(&info->DriftType));
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
            bPlatEndianSwap(reinterpret_cast<int32 *>(&info->ForwardDifficulty));
            bPlatEndianSwap(reinterpret_cast<int32 *>(&info->ReverseDifficulty));
            bPlatEndianSwap(&info->NumSecondsBeforeShortcutsAllowed);
            bPlatEndianSwap(&info->nDriftSecondsMin);
            bPlatEndianSwap(&info->nDriftSecondsMax);

            for (i = 0; i < 2; i++) {
                for (j = 0; j < 4; j++) {
                    bPlatEndianSwap(&info->OverrideStartingRouteForAI[i][j]);
                }
            }

            for (i = 0; i < 4; i++) {
                for (j = 0; j < 2; j++) {
                    bPlatEndianSwap(&info->MaxTrafficCars[i][j]);
                }
            }

            for (i = 0; i < 2; i++) {
                bPlatEndianSwap(&info->TrafficAllowedNearStartLine[i]);
            }

            for (i = 0; i < 2; i++) {
                bPlatEndianSwap(&info->TrafficMinInitialDistanceBetweenCars[i]);
            }

            for (i = 0; i < 2; i++) {
                bPlatEndianSwap(&info->TrafficMinInitialDistanceFromStartLine[i]);
            }
        }

        return 1;
    }
    return 0;
}

int TrackInfo::UnloaderTrackInfo(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TRACK_INFOS) {
        TrackInfoTable = nullptr;
        NumTrackInfo = 0;
        return 1;
    }
    return 0;
}
