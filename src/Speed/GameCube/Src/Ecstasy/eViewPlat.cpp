#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// 0x1FF8 / sizeof(eViewPlatInfo) = 22 entradas, segun symbols.txt
eViewPlatInfo ViewPlatInfoTable[22];

// El original emite stw, no stb: es int aunque el DWARF lo anote como bool.
int ForceFERenderStates;

SlotPool *eDataRenderSlotPool;

// total size: 0xC
struct eDataRenderDynamic {
    uint32 *colourtable0; // offset 0x0, size 0x4
    uint32 *colourtable1; // offset 0x4, size 0x4
    float (*trm)[4][3];   // offset 0x8, size 0x4
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
    void Flush();

    TextureInfo *Texture;                // offset 0x8, size 0x4
    bTList<eDataRender> DataRenderList;  // offset 0xC, size 0x8
};

void eViewPlatInterface::FEBeginBatchRender(int numPolys) {
    ForceFERenderStates = 1;
}

void eViewPlatInterface::FEEndBatchRender() {
    ForceFERenderStates = 0;
}

eViewPlatInfo *eViewPlatInterface::GimmeMyViewPlatInfo(int view_id) {
    return &ViewPlatInfoTable[view_id];
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
