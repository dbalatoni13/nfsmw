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
    int num_markers = 0;

    for (TrackPositionMarker *p = TrackPositionMarkerList.GetHead(); p != TrackPositionMarkerList.EndOfList();
         p = p->GetNext()) {
        if (p->NameHash == name_hash && (track_number == 0 || p->TrackNumber == track_number)) {
            num_markers += 1;
        }
    }

    return num_markers;
}

TrackPositionMarker *GetTrackPositionMarker(int track_number, unsigned int name_hash, int index) {
    int num_markers = 0;

    for (TrackPositionMarker *p = TrackPositionMarkerList.GetHead(); p != TrackPositionMarkerList.EndOfList();
         p = p->GetNext()) {
        if (p->NameHash == name_hash && (p->TrackNumber == 0 || p->TrackNumber == track_number)) {
            if (num_markers == index) {
                return p;
            }

            num_markers += 1;
        }
    }

    return 0;
}

TrackPositionMarker *GetTrackPositionMarker(unsigned int name_hash, int index) {
    int track_number = TrackInfo::GetLoadedTrackNumber();

    return GetTrackPositionMarker(track_number, name_hash, index);
}
