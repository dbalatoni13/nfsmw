#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

eViewPlatInfo ViewPlatInfoTable[22];

bool ForceFERenderStates;

SlotPool *eTextureBucketSlotPool;
SlotPool *eDataRenderSlotPool;
int g_NumTextureBuckets;

// total size: 0xC
struct eDataRenderDynamic {
    uint32 *colourtable0; // offset 0x0, size 0x4
    uint32 *colourtable1; // offset 0x4, size 0x4
    Mtx *trm;             // offset 0x8, size 0x4
};

// total size: 0x38
struct eDataRender : public bTNode<eDataRender> {
    void Render(TextureInfo *texture);

    void *Data;                    // offset 0x8, size 0x4
    eView *View;                   // offset 0xC, size 0x4
    eSolid *Solid;                 // offset 0x10, size 0x4
    uint32 Flags;                  // offset 0x14, size 0x4
    eLightContext *LightContext;   // offset 0x18, size 0x4
    eLightMaterial *LightMaterial; // offset 0x1C, size 0x4
    bMatrix4 *LocalWorld;          // offset 0x20, size 0x4
    bMatrix4 *BlendingMatrices;    // offset 0x24, size 0x4
    unsigned short Entries;        // offset 0x28, size 0x2
    eDataRenderDynamic DRD;        // offset 0x2C, size 0xC
};

// total size: 0x14
struct eTextureBucket : public bTNode<eTextureBucket> {
    void *operator new(size_t size) {
        return bOMalloc(eTextureBucketSlotPool);
    }

    eTextureBucket(TextureInfo *texture_info) {
        this->Texture = texture_info;
        this->DataRenderList.InitList();
    }

    void AddMeshRender(eStripEntry *mesh, unsigned short entries, eView *view, eSolid *solid, uint32 flags, bMatrix4 *local_world,
                       eLightContext *light_context, eLightMaterial *light_mat, bMatrix4 *blending_matrices, eDataRenderDynamic *drd) {
        eDataRender *data = static_cast<eDataRender *>(bOMalloc(eDataRenderSlotPool));

        data->Data = mesh;
        data->Solid = solid;
        data->View = view;
        data->Flags = flags;
        data->LightContext = light_context;
        data->LocalWorld = local_world;
        data->LightMaterial = light_mat;
        data->Entries = entries;
        data->BlendingMatrices = blending_matrices;
        data->DRD.colourtable0 = drd->colourtable0;
        data->DRD.colourtable1 = drd->colourtable1;
        data->DRD.trm = drd->trm;

        this->DataRenderList.AddTail(data);
    }

    void Flush();

    TextureInfo *Texture;               // offset 0x8, size 0x4
    bTList<eDataRender> DataRenderList; // offset 0xC, size 0x8
};

bTList<eTextureBucket> g_TextureBucketList[16];

void eViewPlatInterface::FEBeginBatchRender(int numPolys) {
    ForceFERenderStates = 1;
}

void eViewPlatInterface::FEEndBatchRender() {
    ForceFERenderStates = 0;
}

eViewPlatInfo *eViewPlatInterface::GimmeMyViewPlatInfo(int view_id) {
    return &ViewPlatInfoTable[view_id];
}

void eViewPlatInterface::GetScreenPosition(bVector3 *screen_position, const bVector3 *world_position) {
    eViewPlatInfo *plat_info = GetPlatInfo();

    eRotTransPers(screen_position, world_position, &plat_info->WorldViewMatrix, &plat_info->ViewScreenMatrix, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f,
                  1.0f);
}

void eSubmitMesh(eStripEntry *mesh, unsigned short entries, eView *view, eSolid *solid, uint32 flags, TextureInfo *texture_info,
                 bMatrix4 *local_world, eLightContext *light_context, eLightMaterial *light_material, bMatrix4 *blending_matrices,
                 eDataRenderDynamic *drd) {
    TextureInfoPlatInfo *plat_info = texture_info->GetPlatInfo();

    if (plat_info->GetActiveBucket() == nullptr) {
        eTextureBucket *bucket = new eTextureBucket(texture_info);

        plat_info->pActiveBucket = bucket;
        if (texture_info->ApplyAlphaSorting) {
            g_TextureBucketList[texture_info->RenderingOrder + 8].AddTail(bucket);
        } else {
            g_TextureBucketList[texture_info->RenderingOrder].AddTail(bucket);
        }
        g_NumTextureBuckets++;
    }

    plat_info->GetActiveBucket()->AddMeshRender(mesh, entries, view, solid, flags, local_world, light_context, light_material,
                                                blending_matrices, drd);
}

void eTextureBucket::Flush() {
    {
        eDataRender *data;

        while ((data = this->DataRenderList.GetHead()) != this->DataRenderList.EndOfList()) {
            data->Remove();
            if (data) {
                data->Render(this->Texture);
                bFree(eDataRenderSlotPool, data);
            }
        }
    }
    this->Texture->GetPlatInfo()->pActiveBucket = nullptr;
}
