#include "Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "eLight.hpp"

#include <types.h>

int eUnloadSolidListPlatChunks(bChunk *chunk);
eSolidIndexEntry *GetSolidIndexEntry(eSolidListHeader *list_header, uint32 name_hash);

bTList<eSolidListHeader> SolidListHeaderList;
bTList<eSolid> InvalidSolidList;
bTList<eSolid> SolidList;
LoadedTable SolidLoadedTable;
eLoadedSolidStats LoadedSolidStats;
int AllowDuplicateSolids = 0;
int32 eDisableFixUpTables = 0;
int eDirtySolids = 0;
int32 eDirtyTextures = 0;
int32 eDirtyAnimations = 0;
float TotalFindSolidTime = 0;

void eSolid::GetBoundingBox(bVector3 *min, bVector3 *max) {
    float minx = this->AABBMinX;
    float miny = this->AABBMinY;
    float minz = this->AABBMinZ;

    float maxx = this->AABBMaxX;
    float maxy = this->AABBMaxY;
    float maxz = this->AABBMaxZ;

    min->x = minx;
    min->y = miny;
    min->z = minz;
    max->x = maxx;
    max->y = maxy;
    max->z = maxz;
}

void eSolid::FixTextureTable() {
    if (!AreChunksBeingMoved()) {
        for (int n = 0; n < this->NumTextureTableEntries; n++) {
            eTextureEntry *texture_entry = &this->pTextureTable[n];
            TextureInfo *texture_info = GetTextureInfo(texture_entry->NameHash, 1, 0);
            texture_entry->pTextureInfo = texture_info;
        }
    }
}

void eSolid::FixLightMaterialTable() {
    eLightMaterialEntry *elme = this->LightMaterialTable;
    int num_light_materials = this->NumLightMaterials;
    if (num_light_materials != 0) {
        for (int i = 0; i < num_light_materials; i++) {
            elme->LightMaterial = elGetLightMaterial(elme->NameHash);
            elme++;
        }
    }
}

void EmptySolidTextureFixupInfo(eSolidListHeader *list_header) {
    while (!list_header->TexturePackList.IsEmpty()) {
        list_header->TexturePackList.RemoveHead();
    }
    while (!list_header->DefaultTextureList.IsEmpty()) {
        list_header->DefaultTextureList.RemoveHead();
    }
    list_header->NumTexturePacks = 0;
    list_header->NumDefaultTextures = 0;
}

static inline bPNode *FindDefaultTexture(eSolidListHeader *list_header, uint32 name_hash) {
    for (bPNode *p = list_header->DefaultTextureList.GetHead(); p != list_header->DefaultTextureList.EndOfList(); p = p->GetNext()) {
        eTextureEntry *texture_entry = reinterpret_cast<eTextureEntry *>(p->GetObject());

        if (texture_entry->NameHash == name_hash) {
            return p;
        }
    }
    return nullptr;
}

static inline bPNode *FindTexturePack(eSolidListHeader *list_header, TexturePack *texture_pack) {
    for (bPNode *p = list_header->TexturePackList.GetHead(); p != list_header->TexturePackList.EndOfList(); p = p->GetNext()) {
        if (texture_pack == p->GetObject()) {
            return p;
        }
    }
    return nullptr;
}

void RebuildSolidTextureFixupInfo(eSolidListHeader *list_header) {
    EmptySolidTextureFixupInfo(list_header);

    for (int i = 0; i < list_header->NumSolids; i++) {
        eSolidIndexEntry *index_entry = &list_header->SolidIndexEntryTable[i];
        eSolid *solid = index_entry->Solid;

        if (solid) {
            for (int j = 0; j < solid->NumTextureTableEntries; j++) {
                eTextureEntry *texture_entry = &solid->pTextureTable[j];
                TextureInfo *texture_info = texture_entry->pTextureInfo;

                if (texture_info == DefaultTextureInfo) {
                    if (!FindDefaultTexture(list_header, texture_info->NameHash)) {
                        list_header->DefaultTextureList.AddHead(texture_entry);
                        list_header->NumDefaultTextures++;
                    }
                } else {
                    if (!FindTexturePack(list_header, texture_info->pTexturePack)) {
                        list_header->TexturePackList.AddHead(texture_info->pTexturePack);
                        list_header->NumTexturePacks++;
                    }
                }
            }
        }
    }
}

void eSolidNotifyTextureMoving(TexturePack *texture_pack, TextureInfo *texture_info) {
    for (eSolidListHeader *list_header = SolidListHeaderList.GetHead(); list_header != SolidListHeaderList.EndOfList();
         list_header = list_header->GetNext()) {
        if (list_header->NumTexturePacks == -1) {
            RebuildSolidTextureFixupInfo(list_header);
        }

        if (FindTexturePack(list_header, texture_pack)) {
            for (int n = 0; n < list_header->NumSolids; n++) {
                eSolidIndexEntry *index_entry = &list_header->SolidIndexEntryTable[n];
                eSolid *solid = index_entry->Solid;

                if (solid) {
                    if (texture_info) {
                        solid->NotifyTextureMoving(texture_pack, texture_info);
                    } else {
                        solid->NotifyTextureMoving(texture_pack);
                    }
                }
            }
        }
    }
}

void eSolidNotifyTextureLoading(TexturePack *texture_pack, TextureInfo *texture_info, bool loading) {
    if (loading) {
        if (!AreChunksBeingMoved()) {
            for (eSolidListHeader *list_header = SolidListHeaderList.GetHead(); list_header != SolidListHeaderList.EndOfList();
                 list_header = list_header->GetNext()) {
                if (list_header->NumTexturePacks == -1) {
                    RebuildSolidTextureFixupInfo(list_header);
                }

                bool textures_changed = false;

                for (bPNode *p = list_header->DefaultTextureList.GetHead(); p != list_header->DefaultTextureList.EndOfList();) {
                    bPNode *next_p = p->GetNext();
                    eTextureEntry *texture_entry = reinterpret_cast<eTextureEntry *>(p->GetObject());
                    TextureInfo *texture_info = FixupTextureInfoLoading(texture_entry->pTextureInfo, texture_entry->NameHash, texture_pack);

                    if (texture_info != texture_entry->pTextureInfo) {
                        list_header->DefaultTextureList.Remove(p);
                        list_header->NumDefaultTextures--;
                        textures_changed = true;
                    }
                    p = next_p;
                }

                if (textures_changed) {
                    bPNode *p2 = FindTexturePack(list_header, texture_pack);

                    if (!p2) {
                        list_header->TexturePackList.AddHead(texture_pack);
                        list_header->NumTexturePacks++;
                    }

                    for (int n = 0; n < list_header->NumSolids; n++) {
                        eSolidIndexEntry *index_entry = &list_header->SolidIndexEntryTable[n];
                        eSolid *solid = index_entry->Solid;

                        if (solid) {
                            solid->NotifyTextureLoading(texture_pack);
                        }
                    }
                }
            }
        }
    } else if (AreChunksBeingMoved()) {
        eSolidNotifyTextureMoving(texture_pack, texture_info);
    } else {
        for (eSolidListHeader *list_header = SolidListHeaderList.GetHead(); list_header != SolidListHeaderList.EndOfList();
             list_header = list_header->GetNext()) {
            if (list_header->NumTexturePacks == -1) {
                RebuildSolidTextureFixupInfo(list_header);
            }

            if (FindTexturePack(list_header, texture_pack)) {
                bool textures_changed = false;

                for (int n = 0; n < list_header->NumSolids; n++) {
                    eSolidIndexEntry *index_entry = &list_header->SolidIndexEntryTable[n];
                    eSolid *solid = index_entry->Solid;

                    if (solid) {
                        if (solid->NotifyTextureUnloading(texture_pack)) {
                            textures_changed = true;
                        }
                    }
                }

                if (textures_changed) {
                    RebuildSolidTextureFixupInfo(list_header);
                }
            }
        }
    }
}

// UNSOLVED https://decomp.me/scratch/sdOFh
bool eSolid::NotifyTextureLoading(TexturePack *texture_pack /* r27 */) {
    bool textures_changed = false;

    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = FixupTextureInfoLoading(texture_entry->pTextureInfo, texture_entry->NameHash, texture_pack);

        if (texture_info != texture_entry->pTextureInfo) {
            texture_entry->pTextureInfo = texture_info;
            textures_changed = true;
        }
    }
    return textures_changed;
}

// UNSOLVED
bool eSolid::NotifyTextureUnloading(TexturePack *texture_pack) {
    bool textures_changed = false;

    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = FixupTextureInfoUnloading(texture_entry->pTextureInfo, texture_entry->NameHash, texture_pack);

        if (texture_info != texture_entry->pTextureInfo) {
            texture_entry->pTextureInfo = texture_info;
            textures_changed = true;
        }
    }
    return textures_changed;
}

void eSolid::NotifyTextureMoving(TexturePack *texture_pack) {
    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = texture_entry->pTextureInfo;

        if (texture_info->pTexturePack == texture_pack) {
            // what?...
            texture_entry->pTextureInfo = reinterpret_cast<TextureInfo *>((reinterpret_cast<char *>(texture_info)) + GetChunkMovementOffset());
        }
    }
}

void eSolid::NotifyTextureMoving(TexturePack *texture_pack, TextureInfo *texture_info) {
    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];

        if (texture_entry->pTextureInfo == texture_info) {
            // what?...
            texture_entry->pTextureInfo = reinterpret_cast<TextureInfo *>((reinterpret_cast<char *>(texture_info)) + GetChunkMovementOffset());
        }
    }
}

void eSolid::ReplaceLightMaterial(uint32 old_name_hash, eLightMaterial *new_light_material) {
    if (!new_light_material) {
        return;
    }
    eLightMaterialEntry *elme = this->LightMaterialTable;
    uint32 new_name_hash;
    int num_light_materials = this->NumLightMaterials;

    for (int i = 0; i < num_light_materials; i++) {
        if (elme->NameHash == old_name_hash) {
            elme->LightMaterial = new_light_material;
        }
        elme++;
    }
}

ePositionMarker *eSolid::GetPostionMarker(ePositionMarker *prev_marker) {
    ePositionMarker *position_marker_table = this->PositionMarkerTable;
    int num_position_markers = this->NumPositionMarkerTableEntries;
    ePositionMarker *next_marker;

    if (!position_marker_table || (num_position_markers == 0)) {
        return nullptr;
    }
    if (prev_marker) {
        if (prev_marker >= position_marker_table) {
            if (prev_marker < &position_marker_table[num_position_markers - 1]) {
                next_marker = prev_marker + 1;
                return next_marker;
            }
        }
        return nullptr;
    }
    return position_marker_table;
}

// UNSOLVED
ePositionMarker *eSolid::GetPostionMarker(uint32 namehash /* r31 */) {
    ePositionMarker *position_marker = nullptr;
    while ((position_marker = this->GetPostionMarker(position_marker)) != nullptr) {
        if (position_marker->NameHash == namehash) {
            return position_marker;
        }
    }
    return nullptr;
}

void eSolid::SmoothNormals(eSmoothVertex **smooth_vertex_table, int num_smooth_verts) {
    eNormalSmoother *normal_smoother = this->NormalSmoother;

    if (normal_smoother && smooth_vertex_table && num_smooth_verts > 0) {
        int num_plat_verts = normal_smoother->NumSmoothVertexPlat;

        for (int i = 0; i < num_plat_verts; i++) {
            eSmoothVertexPlat *smooth_vertex_plat = &normal_smoother->SmoothVertexPlatTable[i];
            unsigned int smoothing_group = smooth_vertex_plat->SmoothingGroup;
            unsigned int vertex_hash = smooth_vertex_plat->VertexHash;
            int smooth_vertex_index = -1;
            bVector3 norm(0.0f, 0.0f, 0.0f);

            int low_index = 0;
            int high_index = num_smooth_verts - 1;

            while (low_index <= high_index && smooth_vertex_index == -1) {
                int mid_index = (low_index + high_index) / 2;
                unsigned int mid_vertex_hash = smooth_vertex_table[mid_index]->VertexHash;

                if (vertex_hash < mid_vertex_hash) {
                    high_index = mid_index - 1;
                } else if (vertex_hash > mid_vertex_hash) {
                    low_index = mid_index + 1;
                } else {
                    smooth_vertex_index = mid_index;
                }
            }

            if (smooth_vertex_index != -1) {
                int debug_print;

                while (smooth_vertex_index > 0 && smooth_vertex_table[smooth_vertex_index - 1]->VertexHash == vertex_hash) {
                    smooth_vertex_index--;
                }

                for (; smooth_vertex_index < num_smooth_verts && smooth_vertex_table[smooth_vertex_index]->VertexHash == vertex_hash;
                     smooth_vertex_index++) {
                    eSmoothVertex *smv = smooth_vertex_table[smooth_vertex_index];

                    if (smoothing_group & (1 << smv->SmoothingGroupNumber)) {
                        bVector3 tnorm;

                        tnorm.x = smv->NX * (1.0f / 127.0f);
                        tnorm.y = smv->NY * (1.0f / 127.0f);
                        tnorm.z = smv->NZ * (1.0f / 127.0f);
                        bNormalize(&tnorm, &tnorm);
                        bAdd(&norm, &norm, &tnorm);
                    }
                }
                bNormalize(&norm, &norm);
                this->SetSmoothVertex(smooth_vertex_plat->VertexOffset, norm.x, norm.y, norm.z);
            }
        }
    }
}

int eSmoothNormals(eSolid **solid_table, int num_solids) {
    if (!solid_table || num_solids < 0) {
        return 0;
    }

    if (num_solids == 0) {
        return 1;
    }

    int *smooth_vertex_index_table = new ("smooth_vertex_index_table", 0) int[num_solids];
    int num_verts_indep = 0;

    for (int i = 0; i < num_solids; i++) {
        if (solid_table[i] && solid_table[i]->NormalSmoother) {
            num_verts_indep += solid_table[i]->NormalSmoother->NumSmoothVertex;
        } else {
            solid_table[i] = nullptr;
        }

        smooth_vertex_index_table[i] = 0;
    }

    int num_verts_plat;
    unsigned int start_time = bGetTicker();

    int num_smooth_verts = 0;
    eSmoothVertex **smooth_vertex_table = new ("eSmoothVertices", 0) eSmoothVertex *[num_verts_indep];

    for (int v = 0; v < num_verts_indep; v++) {
        eSmoothVertex *smv = nullptr;
        int solid_index = 0;

        for (int j = 0; j < num_solids; j++) {
            if (solid_table[j] && smooth_vertex_index_table[j] < solid_table[j]->NormalSmoother->NumSmoothVertex) {
                eSmoothVertex *test_smv = &solid_table[j]->NormalSmoother->SmoothVertexTable[smooth_vertex_index_table[j]];

                if (!smv || test_smv->VertexHash < smv->VertexHash) {
                    smv = test_smv;
                    solid_index = j;
                }
            }
        }

        if (smv) {
            smooth_vertex_table[num_smooth_verts] = smv;
            num_smooth_verts++;

            smooth_vertex_index_table[solid_index]++;
        }
    }

    for (int s = 0; s < num_solids; s++) {
        if (solid_table[s]) {
            solid_table[s]->SmoothNormals(smooth_vertex_table, num_smooth_verts);
        }
    }

    delete smooth_vertex_index_table;
    delete smooth_vertex_table;

    return 1;
}

eSolidListHeader *InternalLoaderSolidHeaderChunks(bChunk *chunk) {
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();

    while (current_chunk < last_chunk) {
        uint32 current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_HEADER) {
            solid_list_header = reinterpret_cast<eSolidListHeader *>(current_chunk->GetData());
            if (!solid_list_header->EndianSwapped) {
                solid_list_header->EndianSwap();
            }
            SolidListHeaderList.AddTail(solid_list_header);
            LoadedSolidStats.NumLoadedLists++;
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_KEYS) {
            solid_list_header->SolidIndexEntryTable = reinterpret_cast<eSolidIndexEntry *>(current_chunk->GetData());
            if (solid_list_header->EndianSwapped == 0) {
                for (int i = 0; i < solid_list_header->NumSolids; i++) {
                    solid_list_header->SolidIndexEntryTable[i].EndianSwap();
                };
            }
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_OFFSETS) {
            solid_list_header->SolidStreamEntryTable = reinterpret_cast<eStreamingEntry *>(current_chunk->GetData());
            if (solid_list_header->EndianSwapped == 0) {
                for (int i = 0; i < solid_list_header->NumSolids; i++) {
                    solid_list_header->SolidStreamEntryTable[i].EndianSwap();
                };
            }
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_EMPTY) {
            eLoadSolidListPlatChunks(current_chunk);
        }
        current_chunk = current_chunk->GetNext();
    }
    if (solid_list_header) {
        solid_list_header->EndianSwapped = 1;
    }
    solid_list_header->TexturePackList.InitList();
    solid_list_header->DefaultTextureList.InitList();
    return solid_list_header;
}

eSolid *InternalLoaderSolidChunks(bChunk *chunk, eSolidListHeader *solid_list_header) {
    if (chunk->GetID() != BCHUNK_SOLID_PACK) {
        return nullptr;
    }
    bChunk *solid_chunk = chunk->GetFirstChunk();
    bChunk *solid_last_chunk = chunk->GetLastChunk();
    eSolid *solid = nullptr;
    int solid_is_valid = 1;

    while (solid_chunk < solid_last_chunk) {
        uint32 solid_chunk_id = solid_chunk->GetID();

        if (solid_chunk_id == BCHUNK_SOLID_INFO) {
            solid = reinterpret_cast<eSolid *>(solid_chunk->GetAlignedData(16));
            solid->ModelList.InitList();

            if (!solid->EndianSwapped) {
                solid->EndianSwap();
            }
        } else if (solid_chunk_id == BCHUNK_SOLID_TEXTURES) {
            solid->pTextureTable = reinterpret_cast<eTextureEntry *>(solid_chunk->GetData());

            if (!solid->EndianSwapped) {
                for (int i = 0; i < solid->NumTextureTableEntries; i++) {
                    {
                        uint32 textureinfoNH = solid->pTextureTable[i].NameHash;
                        bStringHash("DEFAULT");
                    }
                    solid->pTextureTable[i].EndianSwap();
                }
            }
        } else if (solid_chunk_id == BCHUNK_SOLID_LIGHT_MATERIALS) {
            solid->LightMaterialTable = reinterpret_cast<eLightMaterialEntry *>(solid_chunk->GetData());

            if (!solid->EndianSwapped) {
                for (int i = 0; i < solid->NumLightMaterials; i++) {
                    solid->LightMaterialTable[i].EndianSwap();
                }
            }
        } else if (solid_chunk_id == BCHUNK_MESH_NORMAL_SMOOTHER) {
            solid->NormalSmoother = reinterpret_cast<eNormalSmoother *>(solid_chunk->GetData());

            if (!solid->EndianSwapped) {
                solid->NormalSmoother->EndianSwap();
            }
            LoadedSolidStats.TotalNormalSmootherBytes += solid_chunk->GetSize();
        } else if (solid_chunk_id == BCHUNK_MESH_SMOTH_VERTICES) {
            solid->NormalSmoother->SmoothVertexTable = reinterpret_cast<eSmoothVertex *>(solid_chunk->GetData());
            solid->NormalSmoother->NumSmoothVertex = solid_chunk->GetSize() / sizeof(eSmoothVertex);

            if (!solid->EndianSwapped) {
                eSmoothVertex *smooth_vertex = solid->NormalSmoother->SmoothVertexTable;
                int num_smooth_verts = solid->NormalSmoother->NumSmoothVertex;

                for (int i = 0; i < num_smooth_verts; i++) {
                    smooth_vertex->EndianSwap();
                    smooth_vertex++;
                }
            }
            LoadedSolidStats.TotalNormalSmootherBytes += solid_chunk->GetSize();
        } else if (solid_chunk_id == BCHUNK_MESH_SMOTH_VERTEX_PLATS) {
            solid->NormalSmoother->SmoothVertexPlatTable = reinterpret_cast<eSmoothVertexPlat *>(solid_chunk->GetData());
            solid->NormalSmoother->NumSmoothVertexPlat = solid_chunk->GetSize() / sizeof(eSmoothVertexPlat);

            if (!solid->EndianSwapped) {
                eSmoothVertexPlat *smooth_vertex = solid->NormalSmoother->SmoothVertexPlatTable;
                int num_smooth_verts = solid->NormalSmoother->NumSmoothVertexPlat;

                for (int i = 0; i < num_smooth_verts; i++) {
                    smooth_vertex->EndianSwap();
                    smooth_vertex++;
                }
            }
            LoadedSolidStats.TotalNormalSmootherBytes += solid_chunk->GetSize();
        } else if (solid_chunk_id == BCHUNK_SOLID_DAMAGE_VERTICES) {
            int num_damage_verts = solid_chunk->GetSize() / sizeof(eDamageVertex);

            solid->DamageVertexTable = reinterpret_cast<eDamageVertex *>(solid_chunk->GetData());

            if (!solid->EndianSwapped) {
                for (int i = 0; i < num_damage_verts; i++) {
                    solid->DamageVertexTable[i].EndianSwap();
                }
            }
            LoadedSolidStats.TotalDamageBytes += solid_chunk->GetSize();
        } else if (solid_chunk_id == BCHUNK_SOLID_MARKERS) {
            solid->PositionMarkerTable = reinterpret_cast<ePositionMarker *>(solid_chunk->GetAlignedData(16));
            solid->NumPositionMarkerTableEntries = solid_chunk->GetAlignedSize(16) / sizeof(ePositionMarker);

            if (!solid->EndianSwapped) {
                for (int i = 0; i < solid->NumPositionMarkerTableEntries; i++) {
                    solid->PositionMarkerTable[i].EndianSwap();
                }
            }
        } else if (solid_chunk_id == BCHUNK_MESH_INFO_CONTAINER) {
            if (!solid->LoaderPlatChunks(solid_chunk)) {
                solid_is_valid = 0;
            }
        }
        solid_chunk = solid_chunk->GetNext();
    }

    if (solid) {
        solid->EndianSwapped = 1;
    }

    solid_list_header->NumTexturePacks = -1;

    solid->FixTextureTable();
    solid->FixLightMaterialTable();

    if (!AreChunksBeingMoved() && !(solid->Flags & 0x10)) {
        eSolid *other_solid = eFindSolid(solid->NameHash);

        if (other_solid) {
            solid->Flags |= 0x100;
            other_solid->Flags |= 0x100;

            if (!AllowDuplicateSolids) {
                solid->Flags |= 0x200;
                other_solid->Flags |= 0x200;
            }
        }
    }

    if (!solid->FixPlatInfo()) {
        solid_is_valid = 0;
    }

    eSolidIndexEntry *index_entry = GetSolidIndexEntry(solid_list_header, solid->NameHash);

    if (!index_entry) {
    }

    if (solid_is_valid) {
        if (index_entry) {
            index_entry->Solid = solid;
        }
        SolidList.AddTail(solid);
        SolidLoadedTable.SetLoaded(solid->NameHash);
    } else {
        if (index_entry) {
            index_entry->Solid = nullptr;
        }
        InvalidSolidList.AddTail(solid);
    }

    eDirtySolids = 1;

    LoadedSolidStats.NumLoadedSolids++;
    LoadedSolidStats.TotalSolidsByteSize += chunk->GetSize();

    return solid;
}

eSolidListHeader *InternalUnloaderSolidHeaderChunks(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_MESH_CONTAINER_INFO) {
        return nullptr;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        uint32 current_chunk_id = current_chunk->GetID();

        if (current_chunk_id == BCHUNK_MESH_CONTAINER_HEADER) {
            solid_list_header = reinterpret_cast<eSolidListHeader *>(current_chunk->GetData());
            solid_list_header->Remove();
            EmptySolidTextureFixupInfo(solid_list_header);
            LoadedSolidStats.NumLoadedLists--;
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_KEYS) {
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_OFFSETS) {
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_EMPTY) {
            eUnloadSolidListPlatChunks(current_chunk);
        }
        current_chunk = current_chunk->GetNext();
    }

    return solid_list_header;
}

void InternalUnloaderSolidChunks(bChunk *chunk, eSolidListHeader *solid_list_header) {
    if (chunk->GetID() != BCHUNK_SOLID_PACK) {
        return;
    }
    bChunk *solid_chunk = chunk->GetFirstChunk();
    bChunk *solid_last_chunk = chunk->GetLastChunk();
    eSolid *solid = nullptr;
    while (solid_chunk < solid_last_chunk) {
        uint32 current_chunk_id = solid_chunk->GetID();
        if (current_chunk_id == BCHUNK_SOLID_INFO) {
            solid = reinterpret_cast<eSolid *>(solid_chunk->GetAlignedData(16));
            solid->UnFixPlatInfo();
        } else if (current_chunk_id == BCHUNK_MESH_NORMAL_SMOOTHER) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_MESH_SMOTH_VERTICES) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_MESH_SMOTH_VERTEX_PLATS) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == 0x13401b) {
            LoadedSolidStats.TotalDamageBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_SOLID_MARKERS) {
        } else if (current_chunk_id == BCHUNK_MESH_INFO_CONTAINER) {
            solid->UnloaderPlatChunks(solid_chunk);
        }
        solid_chunk = solid_chunk->GetNext();
    }
    if (solid_list_header) {
        eSolidIndexEntry *index_entry = GetSolidIndexEntry(solid_list_header, solid->NameHash);
        if (index_entry) {
            index_entry->Solid = nullptr;
            SolidLoadedTable.SetUnloaded(solid->NameHash);
        }
    }
    solid_list_header->NumTexturePacks = -1;
    NotifySolidUnloader(solid);
    solid->Remove();
    eDirtySolids = 1;
    LoadedSolidStats.NumLoadedSolids--;
    LoadedSolidStats.TotalSolidsByteSize -= chunk->GetSize();
}

int LoaderSolidList(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_GEOMETRY_PACK) {
        return 0;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        uint32 current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_INFO) {
            if (solid_list_header) {
                NotifySolidLoader(solid_list_header);
            }
            solid_list_header = InternalLoaderSolidHeaderChunks(current_chunk);
        } else if (current_chunk_id == BCHUNK_SOLID_PACK) {
            InternalLoaderSolidChunks(current_chunk, solid_list_header);
        }
        current_chunk = current_chunk->GetNext();
    }
    if (solid_list_header) {
        NotifySolidLoader(solid_list_header);
    }
    return 1;
}

int UnloaderSolidList(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_GEOMETRY_PACK) {
        return 0;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        uint32 current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_INFO) {
            solid_list_header = InternalUnloaderSolidHeaderChunks(current_chunk);
        } else if (current_chunk_id == BCHUNK_SOLID_PACK) {
            InternalUnloaderSolidChunks(current_chunk, solid_list_header);
        }
        current_chunk = current_chunk->GetNext();
    }
    return 1;
}

void SolidLoadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    if (chunk->GetID() != BCHUNK_SOLID_PACK) {
        EndianSwapChunkHeader(chunk);
        EndianSwapChunkHeadersRecursive(chunk->GetFirstChunk(), chunk->GetLastChunk());
    }
    InternalLoaderSolidChunks(chunk, streaming_pack->SolidListHeader);
    NotifySolidLoader(streaming_pack->SolidListHeader);
}

void SolidUnloadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    eSolid *solid = eFindSolid(streaming_entry->NameHash);
    if (solid) {
        InternalUnloaderSolidChunks(chunk, streaming_pack->SolidListHeader);
    }
}

void eLoadStreamingSolid(uint32 *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num) {
    StreamingSolidPackLoader.LoadStreamingEntry(name_hash_table, num_hashes, callback, param0, memory_pool_num);
}

void eUnloadStreamingSolid(uint32 *name_hash_table, int num_hashes) {
    StreamingSolidPackLoader.UnloadStreamingEntry(name_hash_table, num_hashes);
}

void eWaitForStreamingSolidPackLoading(const char *filename) {
    StreamingSolidPackLoader.WaitForLoadingToFinish(filename);
}

int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num) {
    eStreamingPack *streaming_pack = StreamingSolidPackLoader.CreateStreamingPack(filename, callback_function, callback_param, memory_pool_num);
    return streaming_pack != nullptr;
}

void SolidLoadingStreamingPackPhase1(eStreamingPackHeaderLoadingInfoPhase1 *loading_info) {
    bChunk *list_chunk = loading_info->TempHeaderChunks;
    EndianSwapChunkHeader(list_chunk);
    bChunk *null_chunk = list_chunk->GetFirstChunk();
    EndianSwapChunkHeader(null_chunk);
    bChunk *header_chunk = null_chunk->GetNext();
    EndianSwapChunkHeader(header_chunk);
    int header_chunks_size = header_chunk->GetSize();
    loading_info->NextLoadAmount = header_chunks_size + 16;
    loading_info->NextLoadPosition = (uintptr_t)header_chunk - (uintptr_t)list_chunk;
}

void SolidLoadingStreamingPackPhase2(eStreamingPackHeaderLoadingInfoPhase2 *loading_info) {
    bChunk *header_chunks = loading_info->HeaderChunks;
    if (loading_info->HeaderChunksMemCopied == 0) {
        EndianSwapChunkHeader(header_chunks);
    }
    EndianSwapChunkHeadersRecursive(header_chunks->GetFirstChunk(), header_chunks->GetLastChunk());

    eSolidListHeader *solid_list_header = InternalLoaderSolidHeaderChunks(header_chunks);
    loading_info->StreamingEntryTable = solid_list_header->SolidStreamEntryTable;
    loading_info->StreamingEntryNumEntries = solid_list_header->NumSolids;
    loading_info->SolidListHeader = solid_list_header;

    if (solid_list_header->PermChunkByteSize != 0) {
        loading_info->LoadResourceFilePosition = solid_list_header->PermChunkByteOffset;
        loading_info->LoadResourceFileAmount = solid_list_header->PermChunkByteSize;
    }
}

int eUnloadStreamingSolidPack(const char *filename) {
    return StreamingSolidPackLoader.DeleteStreamingPack(filename);
}

void SolidUnloadingStreamingPack(eStreamingPack *streaming_pack) {
    InternalUnloaderSolidHeaderChunks(streaming_pack->HeaderChunks);
}

void eInitSolids() {
    bMemSet(&LoadedSolidStats, 0, sizeof(LoadedSolidStats));
    InitStreamingPacks();
}

eSolidIndexEntry *GetSolidIndexEntry(eSolidListHeader *list_header, uint32 name_hash) {
    if (!list_header) {
        return nullptr;
    } else {
        return reinterpret_cast<eSolidIndexEntry *>(ScanHashTableKey32(name_hash, list_header->SolidIndexEntryTable, list_header->NumSolids, 0, 8));
    }
}

eSolid *eFindSolid(uint32 name_hash) {
    return eFindSolid(name_hash, nullptr);
}

// UNSOLVED
eSolid *eFindSolid(uint32 name_hash /* r31 */, eSolidListHeader *solid_list_header /* r30 */) {
    if (SolidLoadedTable.IsLoaded(name_hash)) {
        return nullptr;
    }
    uint32 start_time = bGetTicker();
    eSolid *solid = nullptr;
    if (solid_list_header) {
        eSolidIndexEntry *index_entry = GetSolidIndexEntry(solid_list_header, name_hash);
        if (index_entry) {
            solid = index_entry->Solid;
        }
    } else {
        for (eSolidListHeader *list_header /* r30 */ = SolidListHeaderList.GetHead(); list_header != SolidListHeaderList.EndOfList();
             list_header = list_header->GetNext()) {
            eSolidIndexEntry *index_entry = GetSolidIndexEntry(list_header, name_hash);
            if (!index_entry) {
                continue;
            }
            if (index_entry->Solid) {
                solid = index_entry->Solid;
                SolidListHeaderList.Remove(list_header);
                SolidListHeaderList.AddHead(list_header);
                break;
            }
        }
    }
    TotalFindSolidTime += bGetTickerDifference(start_time, bGetTicker());

    return solid;
}

bChunkLoader bChunkLoaderSolidList(0x80134000, LoaderSolidList, UnloaderSolidList);

eStreamPackLoader StreamingSolidPackLoader(0x20, SolidLoadedStreamingEntryCallback, SolidUnloadedStreamingEntryCallback,
                                           SolidLoadingStreamingPackPhase1, SolidLoadingStreamingPackPhase2,
                                           SolidUnloadingStreamingPack);
