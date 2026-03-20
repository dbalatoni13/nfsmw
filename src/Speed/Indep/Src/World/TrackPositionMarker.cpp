#include "TrackPositionMarker.hpp"

#include "TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

bTList<TrackPositionMarker> TrackPositionMarkerList;
bChunkLoader bChunkLoaderTrackPositionMarkers(0x34146, LoaderTrackPositionMarkers, UnloaderTrackPositionMarkers);

static void NotifyTrackMarkersChanged() {}

void ForEachTrackPositionMarker(bool (*callback)(TrackPositionMarker *, unsigned int), unsigned int tag) {
    for (TrackPositionMarker *marker = TrackPositionMarkerList.GetHead(); marker != TrackPositionMarkerList.EndOfList();
         marker = marker->GetNext()) {
        if (!callback(marker, tag)) {
            break;
        }
    }
}

int LoaderTrackPositionMarkers(bChunk *chunk) {
    if (chunk->GetID() == 0x34146) {
        TrackPositionMarker *marker_table = reinterpret_cast<TrackPositionMarker *>(chunk->GetAlignedData(0x10));
        int num_markers = chunk->GetAlignedSize(0x10) / sizeof(TrackPositionMarker);

        for (int n = 0; n < num_markers; n++) {
            TrackPositionMarker *marker = &marker_table[n];
            bPlatEndianSwap(&marker->NameHash);
            bPlatEndianSwap(&marker->Param);
            bPlatEndianSwap(&marker->Position);
            bPlatEndianSwap(&marker->Angle);
            bPlatEndianSwap(&marker->TrackNumber);
            TrackPositionMarkerList.AddTail(marker);
        }

        NotifyTrackMarkersChanged();
        return 1;
    }

    return 0;
}

int UnloaderTrackPositionMarkers(bChunk *chunk) {
    if (chunk->GetID() == 0x34146) {
        TrackPositionMarker *marker_table = reinterpret_cast<TrackPositionMarker *>(chunk->GetAlignedData(0x10));
        int num_markers = chunk->GetAlignedSize(0x10) / sizeof(TrackPositionMarker);

        for (int n = 0; n < num_markers; n++) {
            TrackPositionMarkerList.Remove(&marker_table[n]);
        }

        NotifyTrackMarkersChanged();
        return 1;
    }

    return 0;
}

int GetNumTrackPositionMarkers(int track_number, unsigned int name_hash) {
    int count = 0;

    for (TrackPositionMarker *marker = TrackPositionMarkerList.GetHead(); marker != TrackPositionMarkerList.EndOfList();
         marker = marker->GetNext()) {
        if (marker->TrackNumber == track_number && marker->NameHash == name_hash) {
            count += 1;
        }
    }

    return count;
}

TrackPositionMarker *GetTrackPositionMarker(int track_number, unsigned int name_hash, int index) {
    for (TrackPositionMarker *marker = TrackPositionMarkerList.GetHead(); marker != TrackPositionMarkerList.EndOfList();
         marker = marker->GetNext()) {
        if (marker->TrackNumber == track_number && marker->NameHash == name_hash) {
            if (index == 0) {
                return marker;
            }

            index -= 1;
        }
    }

    return 0;
}

TrackPositionMarker *GetTrackPositionMarker(unsigned int name_hash, int index) {
    int track_number = 0;
    if (LoadedTrackInfo) {
        track_number = LoadedTrackInfo->TrackNumber;
    }

    return GetTrackPositionMarker(track_number, name_hash, index);
}
