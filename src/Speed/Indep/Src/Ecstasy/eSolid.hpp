#ifndef ECSTASY_ESOLID_H
#define ECSTASY_ESOLID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Ecstasy.hpp"
#include "EcstasyData.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "eModel.hpp"

#define BCHUNK_GEOMETRY_PACK 0x80134000
#define BCHUNK_MESH_CONTAINER_INFO 0x80134001
#define BCHUNK_MESH_CONTAINER_EMPTY 0x80134008
#define BCHUNK_SOLID_PACK 0x80134010
#define BCHUNK_MESH_INFO_CONTAINER 0x80134100

#define BCHUNK_MESH_CONTAINER_HEADER 0x00134002
#define BCHUNK_MESH_CONTAINER_KEYS 0x00134003
#define BCHUNK_MESH_CONTAINER_OFFSETS 0x00134004
#define BCHUNK_SOLID_INFO 0x00134011
#define BCHUNK_MESH_NORMAL_SMOOTHER 0x00134017
#define BCHUNK_MESH_SMOTH_VERTICES 0x00134018
#define BCHUNK_MESH_SMOTH_VERTEX_PLATS 0x00134019
#define BCHUNK_SOLID_MARKERS 0x0013401A
// TODO 0x0013401B missing

struct eSolid : public eSolidPlatInterface, public bTNode<eSolid> {
    // total size: 0xE0
    unsigned char Version;                      // offset 0xC, size 0x1
    unsigned char EndianSwapped;                // offset 0xD, size 0x1
    unsigned short Flags;                       // offset 0xE, size 0x2
    unsigned int NameHash;                      // offset 0x10, size 0x4
    short NumPolys;                             // offset 0x14, size 0x2
    short NumVerts;                             // offset 0x16, size 0x2
    char NumBones;                              // offset 0x18, size 0x1
    char NumTextureTableEntries;                // offset 0x19, size 0x1
    char NumLightMaterials;                     // offset 0x1A, size 0x1
    char NumPositionMarkerTableEntries;         // offset 0x1B, size 0x1
    int ReferencedFrameCounter;                 // offset 0x1C, size 0x4
    float AABBMinX;                             // offset 0x20, size 0x4
    float AABBMinY;                             // offset 0x24, size 0x4
    float AABBMinZ;                             // offset 0x28, size 0x4
    eTextureEntry *pTextureTable;               // offset 0x2C, size 0x4
    float AABBMaxX;                             // offset 0x30, size 0x4
    float AABBMaxY;                             // offset 0x34, size 0x4
    float AABBMaxZ;                             // offset 0x38, size 0x4
    eLightMaterialEntry *LightMaterialTable;    // offset 0x3C, size 0x4
    bMatrix4 PivotMatrix;                       // offset 0x40, size 0x40
    ePositionMarker *PositionMarkerTable;       // offset 0x80, size 0x4
    eNormalSmoother *NormalSmoother;            // offset 0x84, size 0x4
    bTList<eModel> ModelList;                   // offset 0x88, size 0x8
    struct eDamageVertex *DamageVertexTable;    // offset 0x90, size 0x4
    struct eConnectivityData *ConnectivityData; // offset 0x94, size 0x4
    float Volume;                               // offset 0x98, size 0x4
    float Density;                              // offset 0x9C, size 0x4
    char Name[64];                              // offset 0xA0, size 0x40

    void GetBoundingBox(bVector3 *min, bVector3 *max);
    void FixTextureTable();
    void FixLightMaterialTable();
    bool NotifyTextureLoading(TexturePack *texture_pack);
    bool NotifyTextureUnloading(TexturePack *texture_pack);
    void NotifyTextureMoving(TexturePack *texture_pack);
    void NotifyTextureMoving(TexturePack *texture_pack, TextureInfo *texture_info);
    void ReplaceLightMaterial(unsigned int old_name_hash, eLightMaterial *new_light_material);
    ePositionMarker *GetPostionMarker(ePositionMarker *prev_marker);
    ePositionMarker *GetPostionMarker(unsigned int namehash);
};

struct eLoadedSolidStats {
    // total size: 0x14
    unsigned int NumLoadedLists;           // offset 0x0, size 0x4
    unsigned int NumLoadedSolids;          // offset 0x4, size 0x4
    unsigned int TotalSolidsByteSize;      // offset 0x8, size 0x4
    unsigned int TotalNormalSmootherBytes; // offset 0xC, size 0x4
    unsigned int TotalDamageBytes;         // offset 0x10, size 0x4
};

extern eLoadedSolidStats LoadedSolidStats;

void eInitSolids();
void eSolidNotifyTextureLoading(struct TexturePack *texture_pack /* r27 */, struct TextureInfo *texture_info /* r4 */, bool loading /* r5 */);
eSolid *eFindSolid(unsigned int name_hash, eSolidListHeader *solid_list_header);
int eSmoothNormals(eSolid **solid_table, int num_solids);

#endif
