#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

struct eStripDataE;

void eSolidPlatInfo::FixStripEntryTable(eSolid *solid, uint8 *strip_entry_data, uint8 *previous_strip_entry_data) {
    eTextureEntry *texture_table;
    eStripEntry *strip_entry_table = StripEntryTable;
    int num_strips = NumStrips;

    for (int n = 0; n < num_strips; n++) {
        eStripEntry *strip_entry = &strip_entry_table[n];

        strip_entry->DataOffset = strip_entry->DataOffset + reinterpret_cast<uintptr_t>(strip_entry_data) - reinterpret_cast<uintptr_t>(previous_strip_entry_data);
    }
}

int eSolidPlatInterface::LoaderPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk->GetLastChunk();
    eSolidPlatInfo *plat_info = nullptr;

    for (current_chunk = chunk->GetFirstChunk(); current_chunk < last_chunk; current_chunk = current_chunk->GetNext()) {
        unsigned int chunk_id = current_chunk->GetID();

        if (chunk_id == BCHUNK_SPEED_ESOLID_GAMECUBE_PLATINFO) {
            plat_info = reinterpret_cast<eSolidPlatInfo *>(current_chunk->GetAlignedData(16));
            SetPlatInfo(plat_info);
        } else if (chunk_id == BCHUNK_SPEED_ESOLID_GAMECUBE_STRIP_ENTRY_TABLE) {
            plat_info->StripEntryTable = reinterpret_cast<eStripEntry *>(current_chunk->GetAlignedData(16));
        } else if (chunk_id == BCHUNK_SPEED_ESOLID_GAMECUBE_STRIP_DATA) {
            plat_info->StripDataStart = reinterpret_cast<uint8 *>(current_chunk->GetAlignedData(32));
        }
    }
    return 1;
}

int eSolidPlatInterface::UnloaderPlatChunks(bChunk *chunk) {
    SetPlatInfo(nullptr);
    return 1;
}

int eSolidPlatInterface::FixPlatInfo() {
    eSolidPlatInfo *plat_info = GetPlatInfo();

    plat_info->FixStripEntryTable(static_cast<eSolid *>(this), plat_info->StripDataStart, nullptr);
    return 1;
}

int eSolidPlatInterface::UnFixPlatInfo() {
    eSolidPlatInfo *plat_info = GetPlatInfo();

    plat_info->FixStripEntryTable(static_cast<eSolid *>(this), nullptr, plat_info->StripDataStart);
    return 1;
}

void eSolidPlatInterface::SetSmoothVertex(uint32 vertex_offset, float nx, float ny, float nz) {
    eSolid *solid;
    eSolidPlatInfo *plat_info;
    uint32 vertex_num;
    uint32 strip_index;
    eStripEntry *strip_entry;
    uint32 strip_num_verts;
    eStripDataE *strip_datae;
    float ox;
    float oy;
    float oz;
    float mx;
    float my;
    float mz;
}

// UNSOLVED
int eLoadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}

// UNSOLVED
int eUnloadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}
