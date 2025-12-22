#ifndef ECSTASY_EMODEL_H
#define ECSTASY_EMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EcstasyData.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *eModelSlotPool;

struct eModel : public bTNode<eModel> {
    // total size: 0x18
    unsigned int NameHash;                                     // offset 0x8, size 0x4
    eSolid *Solid;                                             // offset 0xC, size 0x4
    struct eReplacementTextureTable *pReplacementTextureTable; // offset 0x10, size 0x4
    int NumReplacementTextures;                                // offset 0x14, size 0x4

    void Init(unsigned int name_hash);
    void UnInit();
    void ReconnectSolid(eSolidListHeader *solid_list_header);
    void ConnectSolid(eSolid *new_solid);
    void AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures, int instance_index);
    void RestoreReplacementTextureTable(TextureInfo ***replaced_textures);
    void ApplyReplacementTextureTable(TextureInfo ***replaced_textures);
    int GetBoundingBox(bVector3 *min, bVector3 *max);
    bVector4 *GetPivotPosition();
    bMatrix4 *GetPivotMatrix();
    void ReplaceLightMaterial(unsigned int old_name_hash, eLightMaterial *new_light_material);
    ePositionMarker *GetPostionMarker(ePositionMarker *prev_marker);
    ePositionMarker *GetPostionMarker(unsigned int namehash);

    static void * operator new(size_t size) {}

    void operator delete(void * ptr) {
        bFree(eModelSlotPool, ptr);
    }

    eModel() {}

    ~eModel() {
        this->UnInit();
    }

    eSolid *GetMovedSolid() {} // TODO

    eSolid *GetSolid() {
        return Solid;
    }
};

void eInitModels();
void NotifySolidLoader(eSolidListHeader *solid_list_header);
void NotifySolidUnloader(eSolid *solid);

#endif
