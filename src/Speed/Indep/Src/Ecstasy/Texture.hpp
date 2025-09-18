#ifndef ECSTASY_TEXTURE_H
#define ECSTASY_TEXTURE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#ifdef TARGET_GC
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define BCHUNK_TEXTURE_ANIM_PACK 0xB0300100

#define BCHUNK_TEXTURE_PACK 0xB3300000
#define BCHUNK_TEXTURE_PACK_INFO 0xB3310000
#define BCHUNK_TEXTURE_PACK_BINARY 0xB3312000
#define BCHUNK_TEXTURE_PACK_ANIM 0xB3312004
#define BCHUNK_TEXTURE_PACK_DATA 0xB3320000

#define BCHUNK_TEXTURE_PACK_INFO_HEADER 0x33310001
#define BCHUNK_TEXTURE_PACK_INFO_KEYS 0x33310002
#define BCHUNK_TEXTURE_PACK_INFO_ENTRIES 0x33310003
#define BCHUNK_TEXTURE_PACK_INFO_TEXTURES 0x33310004
#define BCHUNK_TEXTURE_PACK_INFO_COMPS 0x33310005
#define BCHUNK_TEXTURE_PACK_ANIM_NAMES 0x33312001
#define BCHUNK_TEXTURE_PACK_ANIM_FRAMES 0x33312002
#define BCHUNK_TEXTURE_PACK_DATA_HEADER 0x33320001
#define BCHUNK_TEXTURE_PACK_DATA_ARRAY 0x33320002
#define BCHUNK_TEXTURE_PACK_DATA_UNKNOWN 0x33320003

extern SlotPool *TexturePackSlotPool;

enum TextureScrollType {
    TEXSCROLL_OFFSETSCALE = 3,
    TEXSCROLL_SNAP = 2,
    TEXSCROLL_SMOOTH = 1,
    TEXSCROLL_NONE = 0,
};

enum TextureLockType {
    TEXLOCK_READWRITE = 2,
    TEXLOCK_WRITE = 1,
    TEXLOCK_READ = 0,
};

enum TextureCompressionType {
    TEXCOMP_8BIT_64 = 129,
    TEXCOMP_8BIT_16 = 128,
    TEXCOMP_16BIT_3555 = 19,
    TEXCOMP_16BIT_565 = 18,
    TEXCOMP_16BIT_1555 = 17,
    TEXCOMP_DXTC5 = 38,
    TEXCOMP_DXTC3 = 36,
    TEXCOMP_DXTC1 = 34,
    TEXCOMP_S3TC = 34,
    TEXCOMP_DXT = 33,
    TEXCOMP_32BIT = 32,
    TEXCOMP_24BIT = 24,
    TEXCOMP_16BIT = 16,
    TEXCOMP_8BIT = 8,
    TEXCOMP_4BIT = 4,
    TEXCOMP_DEFAULT = 0,
};

enum TextureAlphaUsageType {
    TEXUSAGE_MODULATED = 2,
    TEXUSAGE_PUNCHTHRU = 1,
    TEXUSAGE_NONE = 0,
};

enum TextureAlphaBlendType {
    TEXBLEND_DEST_OVERBRIGHT = 8,
    TEXBLEND_DEST_SUBTRACTIVE = 7,
    TEXBLEND_DEST_ADDATIVE = 6,
    TEXBLEND_DEST_BLEND = 5,
    TEXBLEND_OVERBRIGHT = 4,
    TEXBLEND_SUBTRACTIVE = 3,
    TEXBLEND_ADDATIVE = 2,
    TEXBLEND_BLEND = 1,
    TEXBLEND_SRCCOPY = 0,
};

struct TextureIndexEntry {
    // total size: 0x8
    unsigned int NameHash;            // offset 0x0, size 0x4
    struct TextureInfo *pTextureInfo; // offset 0x4, size 0x4
};

struct TexturePackHeader {
    // total size: 0x7C
    int Version;                                     // offset 0x0, size 0x4
    char Name[28];                                   // offset 0x4, size 0x1C
    char Filename[64];                               // offset 0x20, size 0x40
    unsigned int FilenameHash;                       // offset 0x60, size 0x4
    unsigned int PermChunkByteOffset;                // offset 0x64, size 0x4
    unsigned int PermChunkByteSize;                  // offset 0x68, size 0x4
    BOOL EndianSwapped;                              // offset 0x6C, size 0x4
    struct TexturePack *pTexturePack;                // offset 0x70, size 0x4
    TextureIndexEntry *TextureIndexEntryTable;       // offset 0x74, size 0x4
    struct eStreamingEntry *TextureStreamEntryTable; // offset 0x78, size 0x4

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
    TexturePack(TexturePackHeader *pack_header, int num_textures, const char *pack_name, const char *filename);
    ~TexturePack();
    void AttachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info);
    void UnattachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info);
    void AttachTextureInfo(TextureInfo *texture_info, TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry);
    void UnattachTextureInfo(TextureInfo *texture_info, struct TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry);
    void AssignTextureData(char *texture_data, int begin_pos, int num_bytes);
    void UnAssignTextureData(int begin_pos, int num_bytes);
    TextureIndexEntry *GetTextureIndexEntry(unsigned int name_hash);
    TextureInfo *GetLoadedTexture(unsigned int name_hash);
    TextureInfo *GetTexture(unsigned int name_hash);

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {
        bFree(TexturePackSlotPool, ptr);
    }

    const char *GetName() {
        return this->Name;
    }

    const char *GetFilename() {
        return this->Filename;
    }

    unsigned int GetNameHash() {
        return this->NameHash;
    }

    int GetNumTextures() {
        return this->NumTextures;
    }

    void SetTextureDataSize(int size) {
        this->TextureDataSize = size;
    }

    int GetTextureDataSize() {
        return this->GetTextureDataSize();
    }

    TexturePackHeader *GetTexturePackHeader() {
        return this->pPackHeader;
    }
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
    float GetScroll(float time, float speed, int scroll_type, float time_step);
    void Print();

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

    void EndianSwap() {
        bEndianSwap32(&this->NameHash);
        bEndianSwap32(&this->ClassNameHash);
        bEndianSwap32(&this->ImageParentHash);
        bEndianSwap32(&this->ImagePlacement);
        bEndianSwap32(&this->PalettePlacement);
        bEndianSwap32(&this->ImageSize);
        bEndianSwap32(&this->PaletteSize);
        bEndianSwap32(&this->BaseImageSize);
        bEndianSwap16(&this->Width);
        bEndianSwap16(&this->Height);
        bEndianSwap16(&this->NumPaletteEntries);
        bEndianSwap16(&this->ScrollTimeStep);
        bEndianSwap16(&this->ScrollSpeedS);
        bEndianSwap16(&this->ScrollSpeedT);
        bEndianSwap16(&this->OffsetS);
        bEndianSwap16(&this->OffsetT);
        bEndianSwap16(&this->ScaleS);
        bEndianSwap16(&this->ScaleT);
    }
};

struct TextureVRAMDataHeader : public bTNode<TextureVRAMDataHeader> {
    // total size: 0x18
    int Version;               // offset 0x8, size 0x4
    unsigned int FilenameHash; // offset 0xC, size 0x4
    BOOL EndianSwapped;        // offset 0x10, size 0x4
    bChunk *VRAMDataChunk;     // offset 0x14, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
        bPlatEndianSwap(&this->FilenameHash);
    }
};

struct TextureAnimEntry {
    // total size: 0x10
    unsigned int NameHash;     // offset 0x0, size 0x4
    TextureInfo *pTextureInfo; // offset 0x4, size 0x4
    void *pPlatAnimData;       // offset 0x8, size 0x4
    unsigned int pad;          // offset 0xC, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
    }
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

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->NumFrames);
        bPlatEndianSwap(&this->FramesPerSecond);
        bPlatEndianSwap(&this->TimeBase);
        bPlatEndianSwap(&this->Valid);
        bPlatEndianSwap(&this->CurrentFrame);
    }
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

    void *operator new(size_t size) {
        return bMalloc(TexturePackSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(TexturePackSlotPool, ptr);
    }

    void EndianSwap() {}
};

struct TextureAnimPackHeader {
    // total size: 0x10
    int Version;                       // offset 0x0, size 0x4
    TextureAnimPack *pTextureAnimPack; // offset 0x4, size 0x4
    BOOL EndianSwapped;                // offset 0x8, size 0x4
    int pad;                           // offset 0xC, size 0x4

    TextureAnimPackHeader() {}

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
    }
};

extern TextureInfo *DefaultTextureInfo;

void eInitTextures();
void UpdateTextureAnimations();
TextureInfo *GetTextureInfo(unsigned int name_hash, BOOL return_default_texture_if_not_found, BOOL include_unloaded_textures);
TextureInfo *FixupTextureInfo(TextureInfo *texture_info, unsigned int name_hash, TexturePack *texture_pack, bool loading);

TextureInfo *eCreateTextureInfo();

inline TextureInfo *FixupTextureInfoLoading(TextureInfo *texture_info, unsigned int name_hash, TexturePack *texture_pack) {
    return FixupTextureInfo(texture_info, name_hash, texture_pack, true);
}

inline TextureInfo *FixupTextureInfoUnloading(TextureInfo *texture_info, unsigned int name_hash, TexturePack *texture_pack) {
    return FixupTextureInfo(texture_info, name_hash, texture_pack, false);
}

#endif
