#include "eModel.hpp"
#include "DefragFixer.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Texture.hpp"
#include "eSolid.hpp"

#include <types.h>

struct ReplacementTextureTableFixup {
    // total size: 0x8
    eReplacementTextureTable *pReplacementTextureTable; // offset 0x0, size 0x4
    short NumReplacementTextures;                       // offset 0x4, size 0x2
    short RefCount;                                     // offset 0x6, size 0x2
};

int NumReplacementTextureTableFixups;
ReplacementTextureTableFixup ReplacementTextureTableFixups[182];
bTList<eModel> UnattachedModelList;
bTList<eModel> MovedModelList;
SlotPool *eModelSlotPool;
float MaxNotifyTimeLoad;
float MaxNotifyTimeUnload;
float TotalNotifyTimeLoad;
float TotalNotifyTimeUnload;
float TotalNotifyTime;
int NumNotifyLoads;
int NumNotifyUnloads;

#define NUM_REPLACEMENTS (182)

void AddReplacementTextureTableFixup(eReplacementTextureTable *r, int num) {
    ReplacementTextureTableFixup *free_fixup = nullptr;
    for (int index = 0; index < NUM_REPLACEMENTS; index++) {
        ReplacementTextureTableFixup *fixup = &ReplacementTextureTableFixups[index];
        if (!free_fixup && !fixup->pReplacementTextureTable) {
            free_fixup = fixup;
            continue;
        }
        if (fixup->pReplacementTextureTable == r) {
            fixup->RefCount++;
            return;
        }
    }
    NumReplacementTextureTableFixups++;
    free_fixup->pReplacementTextureTable = r;
    free_fixup->NumReplacementTextures = num;
    free_fixup->RefCount = 1;
}

void RemoveReplacementTextureTableFixup(eReplacementTextureTable *r) {
    for (int index = 0; index < NUM_REPLACEMENTS; index++) {
        ReplacementTextureTableFixup *fixup = &ReplacementTextureTableFixups[index];
        if (fixup->pReplacementTextureTable == r) {
            fixup->RefCount--;
            if (fixup->RefCount == 0) {
                fixup->pReplacementTextureTable = nullptr;
                NumReplacementTextureTableFixups--;
            }
            return;
        }
    }
}

void eFixupReplacementTextureTables() {
    for (int index = 0; index < NUM_REPLACEMENTS; index++) {
        ReplacementTextureTableFixup *fixup = &ReplacementTextureTableFixups[index];
        if (fixup->pReplacementTextureTable) {
            for (int n = 0; n < fixup->NumReplacementTextures; n++) {
                TextureInfo *texture_info = reinterpret_cast<TextureInfo *>(gDefragFixer.Fix(fixup->pReplacementTextureTable[n].pTextureInfo));
                fixup->pReplacementTextureTable[n].SetCurrentTexture(texture_info);
            }
        }
    }
}

void eFixupReplacementTexturesAfterUnloading(TextureInfo *texture_info) {
    for (int index = 0; index < NUM_REPLACEMENTS; index++) {
        ReplacementTextureTableFixup *fixup = &ReplacementTextureTableFixups[index];
        if (fixup->pReplacementTextureTable) {
            for (int n = 0; n < fixup->NumReplacementTextures; n++) {
                if (fixup->pReplacementTextureTable[n].GetCurrentTexture() == texture_info) {
                    fixup->pReplacementTextureTable[n].InvalidateTexture();
                }
            }
        }
    }
}

void eModel::Init(unsigned int name_hash) {
    this->NameHash = name_hash;
    this->pReplacementTextureTable = nullptr;
    this->NumReplacementTextures = 0;
    if (name_hash != 0) {
        UnattachedModelList.AddTail(this);
    }
    this->ReconnectSolid(nullptr);
}

void eModel::UnInit() {
    if (this->NameHash != 0 || this->Solid) {
        this->Remove();
    }
    this->Solid = nullptr;
    this->NameHash = 0;
    if (this->pReplacementTextureTable) {
        RemoveReplacementTextureTableFixup(this->pReplacementTextureTable);
    }
    this->pReplacementTextureTable = nullptr;
    this->NumReplacementTextures = 0;
}

void eModel::ReconnectSolid(eSolidListHeader *solid_list_header) {
    unsigned int name_hash = this->NameHash;
    if (name_hash != 0) {
        eSolid *new_solid = eFindSolid(name_hash, solid_list_header);
        this->ConnectSolid(new_solid);
    }
}

void eModel::ConnectSolid(eSolid *new_solid) {
    unsigned int name_hash = this->NameHash;
    if (name_hash == 0) {
        return;
    }
    eSolid *old_solid = this->Solid;
    if (new_solid == old_solid) {
        return;
    }
    this->Remove();

    if (new_solid) {
        new_solid->ModelList.AddTail(this);
    } else {
        UnattachedModelList.AddTail(new_solid);
    }
    this->Solid = new_solid;
}

void eModel::AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures, int instance_index) {
    if (this->pReplacementTextureTable) {
        RemoveReplacementTextureTableFixup(this->pReplacementTextureTable);
    }
    this->pReplacementTextureTable = replacement_texture_table;
    this->NumReplacementTextures = num_textures;
    AddReplacementTextureTableFixup(replacement_texture_table, num_textures);
}

void eModel::RestoreReplacementTextureTable(TextureInfo ***replaced_textures) {
    eReplacementTextureTable *replacement_texture_table = this->pReplacementTextureTable;
    int num_replacement_textures = this->NumReplacementTextures;
    eSolid *solid = this->Solid;

    if (!replacement_texture_table || !solid || !this->NumReplacementTextures) {
        return;
    }
    eReplacementTextureTable *replacement_texture_entry = replacement_texture_table;

    for (int n = 0; n < num_replacement_textures; n++) {
        TextureInfo **replaced_texture = replaced_textures[n];
        if (replaced_texture) {
            TextureInfo *new_texture_info = *replaced_texture;
            TextureInfo *old_texture_info = replacement_texture_entry->pTextureInfo;

            *replaced_texture = old_texture_info;
            replacement_texture_entry->pTextureInfo = new_texture_info;
        }
        replacement_texture_entry++;
    }
}

void eModel::ApplyReplacementTextureTable(TextureInfo ***replaced_textures) {
    eReplacementTextureTable *replacement_texture_table = this->pReplacementTextureTable;
    eReplacementTextureTable *replacement_texture_entry = replacement_texture_table;
    eSolid *solid = this->Solid;
    int num_replacement_textures = this->NumReplacementTextures;
    int num_solid_textures = solid->NumTextureTableEntries;

    if (!replacement_texture_table || !solid || !num_replacement_textures) {
        return;
    }

    for (int u = 0; u < num_replacement_textures; u++) {
        replaced_textures[u] = nullptr;
        if (replacement_texture_entry->pTextureInfo == reinterpret_cast<TextureInfo *>(-1)) {
            replacement_texture_entry->pTextureInfo = GetTextureInfo(replacement_texture_entry->hNewNameHash, 1, 0);
        }
        replacement_texture_entry++;
    }
    for (int index = 0; index < num_solid_textures; index++) {
        unsigned int solid_tex_namehash = solid->pTextureTable[index].NameHash;
        replacement_texture_entry = replacement_texture_table;
        for (int n = 0; n < num_replacement_textures; n++) {
            if ((replacement_texture_entry->hNewNameHash != 0) && (replacement_texture_entry->hOldNameHash == solid_tex_namehash)) {
                TextureInfo *new_texture_info = replacement_texture_entry->pTextureInfo;
                TextureInfo *old_texture_info = solid->pTextureTable[index].pTextureInfo;

                solid->pTextureTable[index].pTextureInfo = new_texture_info;
                replaced_textures[n] = &solid->pTextureTable[index].pTextureInfo;
                replacement_texture_entry->pTextureInfo = old_texture_info;
                break;
            }
            replacement_texture_entry++;
        }
    }
}

int eModel::GetBoundingBox(bVector3 *min, bVector3 *max) {
    if (this->Solid) {
        this->Solid->GetBoundingBox(min, max);
        return 1;
    } else {
        min->x = 0.0f;
        min->y = 0.0f;
        min->z = 0.0f;
        max->x = 0.0f;
        max->y = 0.0f;
        max->z = 0.0f;
        return 0;
    }
}

bVector4 *eModel::GetPivotPosition() {
    if (this->Solid) {
        return &this->Solid->PivotMatrix.v3;
    }
    return nullptr;
}

bMatrix4 *eModel::GetPivotMatrix() {
    if (this->Solid) {
        return &this->Solid->PivotMatrix;
    }
    return nullptr;
}

void eModel::ReplaceLightMaterial(unsigned int old_name_hash, eLightMaterial *new_light_material) {
    if (this->Solid) {
        this->Solid->ReplaceLightMaterial(old_name_hash, new_light_material);
    }
}

ePositionMarker *eModel::GetPostionMarker(ePositionMarker *prev_marker) {
    if (this->Solid) {
        return this->Solid->GetPostionMarker(prev_marker);
    }
    return nullptr;
}

ePositionMarker *eModel::GetPostionMarker(unsigned int namehash) {
    if (this->Solid) {
        return this->Solid->GetPostionMarker(namehash);
    }
    return nullptr;
}

void eInitModels(void) {
    eModelSlotPool = bNewSlotPool(0x18, 0xa28, "Ecstacy:ModelSlotPool", 0);
}

// UNSOLVED
void NotifySolidLoader(eSolidListHeader *solid_list_header /* r31 */) {
    if (GetChunkMovementOffset() != 0) {
        for (eModel *model = MovedModelList.GetHead(); !MovedModelList.IsEmpty(); model = model->GetNext()) {
            MovedModelList.RemoveHead();
            MovedModelList.AddTail(model);
        }
    } else {
        unsigned int start_time = bGetTicker();
        for (eModel *model = UnattachedModelList.GetHead(); model != UnattachedModelList.EndOfList(); model = model->GetNext()) {
            model->ReconnectSolid(solid_list_header);
        }
        float time = bGetTickerDifference(start_time, bGetTicker());
        TotalNotifyTimeLoad += time;
        MaxNotifyTimeLoad = bMax(MaxNotifyTimeLoad, time);
        NumNotifyLoads++;
        TotalNotifyTime = TotalNotifyTimeLoad + TotalNotifyTimeUnload;
    }
}

int eSmoothNormals(eModel **model_table, int num_models) {
    eSolid *solid_table[256];
    int num_solids;

    if (!model_table || (num_models > 0xff) || (num_models < 0)) {
        return 0;
    } else if (num_models == 0) {
        return 1;
    } else {
        num_solids = 0;
        for (int i = 0; i < num_models; i++) {
            if (model_table[i]) {
                if (model_table[i]->Solid && model_table[i]->Solid->NormalSmoother) {
                    solid_table[num_solids] = model_table[i]->Solid;
                    num_solids++;
                }
            }
        }
        return eSmoothNormals(solid_table, num_solids);
    }
}
