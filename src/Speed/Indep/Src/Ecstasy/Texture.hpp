#pragma once

#ifdef TARGET_GC
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#endif

#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern SlotPool *TexturePackSlotPool;

struct TextureIndexEntry {
    // total size: 0x8
    unsigned int NameHash;            // offset 0x0, size 0x4
    struct TextureInfo *pTextureInfo; // offset 0x4, size 0x4
};

struct TexturePackHeader {
    // total size: 0x7C
    int Version;                               // offset 0x0, size 0x4
    char Name[28];                             // offset 0x4, size 0x1C
    char Filename[64];                         // offset 0x20, size 0x40
    unsigned int FilenameHash;                 // offset 0x60, size 0x4
    unsigned int PermChunkByteOffset;          // offset 0x64, size 0x4
    unsigned int PermChunkByteSize;            // offset 0x68, size 0x4
    int EndianSwapped;                         // offset 0x6C, size 0x4
    struct TexturePack *pTexturePack;          // offset 0x70, size 0x4
    TextureIndexEntry *TextureIndexEntryTable; // offset 0x74, size 0x4
    eStreamingEntry *TextureStreamEntryTable;  // offset 0x78, size 0x4

    void EndianSwap() {}
};

class TexturePack : public bTNode<TexturePack> {
    // total size: 0x20
    const char *Name;               // offset 0x8, size 0x4
    const char *Filename;           // offset 0xC, size 0x4
    unsigned int NameHash;          // offset 0x10, size 0x4
    TexturePackHeader *pPackHeader; // offset 0x14, size 0x4
    int NumTextures;                // offset 0x18, size 0x4
    int TextureDataSize;            // offset 0x1C, size 0x4

  public:
    TextureInfo *GetLoadedTexture(unsigned int name_hash);

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {}

    const char *GetName() {}

    const char *GetFilename() {}

    unsigned int GetNameHash() {}

    int GetNumTextures() {
        return this->NumTextures;
    }

    void SetTextureDataSize(int size) {}

    int GetTextureDataSize() {}

    TexturePackHeader *GetTexturePackHeader() {}
};

struct TextureInfo : public TextureInfoPlatInterface, public bTNode<TextureInfo> {
    // total size: 0x7C
    char DebugName[24];                   // offset 0xC, size 0x18
    unsigned int NameHash;                // offset 0x24, size 0x4
    unsigned int ClassNameHash;           // offset 0x28, size 0x4
    unsigned int ImageParentHash;         // offset 0x2C, size 0x4
    int ImagePlacement;                   // offset 0x30, size 0x4
    int PalettePlacement;                 // offset 0x34, size 0x4
    int ImageSize;                        // offset 0x38, size 0x4
    int PaletteSize;                      // offset 0x3C, size 0x4
    int BaseImageSize;                    // offset 0x40, size 0x4
    short Width;                          // offset 0x44, size 0x2
    short Height;                         // offset 0x46, size 0x2
    char ShiftWidth;                      // offset 0x48, size 0x1
    char ShiftHeight;                     // offset 0x49, size 0x1
    unsigned char ImageCompressionType;   // offset 0x4A, size 0x1
    unsigned char PaletteCompressionType; // offset 0x4B, size 0x1
    short NumPaletteEntries;              // offset 0x4C, size 0x2
    char NumMipMapLevels;                 // offset 0x4E, size 0x1
    char TilableUV;                       // offset 0x4F, size 0x1
    char BiasLevel;                       // offset 0x50, size 0x1
    char RenderingOrder;                  // offset 0x51, size 0x1
    char ScrollType;                      // offset 0x52, size 0x1
    char UsedFlag;                        // offset 0x53, size 0x1
    char ApplyAlphaSorting;               // offset 0x54, size 0x1
    char AlphaUsageType;                  // offset 0x55, size 0x1
    char AlphaBlendType;                  // offset 0x56, size 0x1
    char Flags;                           // offset 0x57, size 0x1
    short ScrollTimeStep;                 // offset 0x58, size 0x2
    short ScrollSpeedS;                   // offset 0x5A, size 0x2
    short ScrollSpeedT;                   // offset 0x5C, size 0x2
    short OffsetS;                        // offset 0x5E, size 0x2
    short OffsetT;                        // offset 0x60, size 0x2
    short ScaleS;                         // offset 0x62, size 0x2
    short ScaleT;                         // offset 0x64, size 0x2
    TexturePack *pTexturePack;            // offset 0x68, size 0x4
    TextureInfo *pImageParent;            // offset 0x6C, size 0x4
    void *ImageData;                      // offset 0x70, size 0x4
    void *PaletteData;                    // offset 0x74, size 0x4
    int ReferenceCount;                   // offset 0x78, size 0x4

  public:
    TextureInfo() {}

    char *GetName() {}

    int IsLoaded() {}

    int IsPartiallyLoaded() {}

    void *GetImageData() {}

    void *GetPaletteData() {}

    void SetScrollTimeStep(float v) {}

    void SetScrollSpeedS(float v) {}

    void SetScrollSpeedT(float v) {}

    void SetOffsetS(float v) {}

    void SetOffsetT(float v) {}

    void SetScaleS(float v) {}

    void SetScaleT(float v) {}

    float GetScrollTimeStep() {}

    float GetScrollSpeedS() {}

    float GetScrollSpeedT() {}

    float GetOffsetS() {}

    float GetOffsetT() {}

    float GetScaleS() {}

    float GetScaleT() {}

    float GetScrollS(float time_s) {}

    float GetScrollT(float time_s) {}

    float GetS(unsigned char u, float time_s) {}

    float GetT(unsigned char v, float time_s) {}

    bool IsZWrite() {}

    bool IsDoubleSided() {}

    void EndianSwap() {}
};

struct TextureVRAMDataHeader : public bTNode<TextureVRAMDataHeader> {
    // total size: 0x18
    int Version;               // offset 0x8, size 0x4
    unsigned int FilenameHash; // offset 0xC, size 0x4
    int EndianSwapped;         // offset 0x10, size 0x4
    bChunk *VRAMDataChunk;     // offset 0x14, size 0x4

    void EndianSwap() {}
};

struct TextureAnimEntry {
    // total size: 0x10
    unsigned int NameHash;     // offset 0x0, size 0x4
    TextureInfo *pTextureInfo; // offset 0x4, size 0x4
    void *pPlatAnimData;       // offset 0x8, size 0x4
    unsigned int pad;          // offset 0xC, size 0x4

    void EndianSwap() {}
};

struct TextureAnim {
    // total size: 0x34
    char Name[24];                      // offset 0x0, size 0x18
    unsigned int NameHash;              // offset 0x18, size 0x4
    int NumFrames;                      // offset 0x1C, size 0x4
    int FramesPerSecond;                // offset 0x20, size 0x4
    int TimeBase;                       // offset 0x24, size 0x4
    TextureAnimEntry *TextureAnimTable; // offset 0x28, size 0x4
    BOOL Valid;                         // offset 0x2C, size 0x4
    int CurrentFrame;                   // offset 0x30, size 0x4

    void EndianSwap() {}
};

struct TextureAnimPack : public bTNode<TextureAnimPack> {
    // total size: 0x1C
    TexturePack *ParentTexturePack;          // offset 0x8, size 0x4
    TextureAnim *TextureAnimTable;           // offset 0xC, size 0x4
    TextureAnimEntry *TextureAnimEntryTable; // offset 0x10, size 0x4
    int NumTextureAnims;                     // offset 0x14, size 0x4
    int NumTextureAnimEntries;               // offset 0x18, size 0x4

    TextureAnimPack(TexturePack *parent_texture_pack, TextureAnim *texture_anim_table, TextureAnimEntry *texture_anim_entry_table, int num_anims,
                    int num_anim_entries);
    ~TextureAnimPack();
    void InitAnims();

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {
        bFree(TexturePackSlotPool, ptr);
    }

    void EndianSwap() {}
};

struct TextureAnimPackHeader {
    // total size: 0x10
    int Version;                       // offset 0x0, size 0x4
    TextureAnimPack *pTextureAnimPack; // offset 0x4, size 0x4
    int EndianSwapped;                 // offset 0x8, size 0x4
    int pad;                           // offset 0xC, size 0x4

    TextureAnimPackHeader() {}

    void EndianSwap() {}
};

void eInitTextures();
void UpdateTextureAnimations();
