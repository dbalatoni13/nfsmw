#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#ifdef EA_PLATFORM_GAMECUBE
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#elif defined(EA_PLATFORM_XENON)
#include "Speed/Xenon/Src/Ecstasy/TextureInfoPlat.hpp"
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include "Speed/PSX2/Src/Ecstasy/TextureInfoPlat.hpp"
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern SlotPool *TexturePackSlotPool;

class TexturePack;
class TextureInfo;

// total size: 0x8
class TextureIndexEntry {
  public:
    uint32 NameHash;           // offset 0x0, size 0x4
    TextureInfo *pTextureInfo; // offset 0x4, size 0x4
};

// total size: 0x7C
class TexturePackHeader {
  public:
    void EndianSwap() {}

    uint32 Version;                                  // offset 0x0, size 0x4
    char Name[28];                                   // offset 0x4, size 0x1C
    char Filename[64];                               // offset 0x20, size 0x40
    uint32 FilenameHash;                             // offset 0x60, size 0x4
    uint32 PermChunkByteOffset;                      // offset 0x64, size 0x4
    uint32 PermChunkByteSize;                        // offset 0x68, size 0x4
    int32 EndianSwapped;                             // offset 0x6C, size 0x4
    TexturePack *pTexturePack;                       // offset 0x70, size 0x4
    TextureIndexEntry *TextureIndexEntryTable;       // offset 0x74, size 0x4
    struct eStreamingEntry *TextureStreamEntryTable; // offset 0x78, size 0x4
};

// total size: 0x18
class TextureVRAMDataHeader : public bTNode<TextureVRAMDataHeader> {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
        bPlatEndianSwap(&this->FilenameHash);
    }

    int32 Version;         // offset 0x8, size 0x4
    uint32 FilenameHash;   // offset 0xC, size 0x4
    int32 EndianSwapped;   // offset 0x10, size 0x4
    bChunk *VRAMDataChunk; // offset 0x14, size 0x4
};

// total size: 0x20
class TexturePack : public bTNode<TexturePack> {
  public:
    TexturePack(TexturePackHeader *pack_header, int num_textures, const char *pack_name, const char *filename);
    ~TexturePack();

    USE_SLOTALLOC(TexturePackSlotPool);

    const char *GetName() {
        return this->Name;
    }

    const char *GetFilename() {
        return this->Filename;
    }

    uint32 GetNameHash() {
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

    TextureInfo *GetTexture(uint32 name_hash);
    TextureInfo *GetLoadedTexture(uint32 name_hash);
    TextureInfo *GetNthTexture(int count);
    void AttachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info);
    void UnattachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info);
    void AttachTextureInfo(TextureInfo *texture_info, TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry);
    void UnattachTextureInfo(TextureInfo *texture_info, TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry);
    void AssignTextureData(char *texture_data, int begin_pos, int num_bytes);
    void UnAssignTextureData(int begin_pos, int num_bytes);

    TexturePackHeader *GetTexturePackHeader() {
        return this->pPackHeader;
    }

  private:
    TextureIndexEntry *GetTextureIndexEntry(uint32 name_hash);

    const char *Name;               // offset 0x8, size 0x4
    const char *Filename;           // offset 0xC, size 0x4
    uint32 NameHash;                // offset 0x10, size 0x4
    TexturePackHeader *pPackHeader; // offset 0x14, size 0x4
    int NumTextures;                // offset 0x18, size 0x4
    int TextureDataSize;            // offset 0x1C, size 0x4
};

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

// total size: 0x4
class TextureInfoPlatInterface {
  public:
    TextureInfoPlatInfo *GetPlatInfo() {
        return this->PlatInfo;
    }

    void SetPlatInfo(TextureInfoPlatInfo *info);

    // TextureInfo *GetTextureInfo() {}

    void Init();
    void Close();
    void *LockImage(TextureLockType lock);
    void UnlockImage(void *image_lock);
    void *LockPalette(TextureLockType lock);
    void UnlockPalette(void *palette_lock);
    void *CreateAnimData();
    void ReleaseAnimData(void *anim_data);
    void SetAnimData(void *anim_data);

    TextureInfoPlatInfo *PlatInfo; // offset 0x0, size 0x4
};

// total size: 0x7C
class TextureInfo : public TextureInfoPlatInterface, public bTNode<TextureInfo> {
  public:
    TextureInfo() {}

    char *GetName() {
        return this->DebugName;
    }

    void Print();
    void Setup(char *name, void *image_data, int image_width, int image_height, int comp_type, void *palette_data, int num_palette_entries,
               int alpha_usage, int alpha_blend, int alpha_sort);

    int IsLoaded() {}

    int IsPartiallyLoaded() {}

    void *GetImageData() {
        return this->ImageData;
    }

    void *GetPaletteData() {
        return this->PaletteData;
    }

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

    float GetScroll(float time, float speed, int scroll_type, float time_step);

    float GetScrollS(float time_s) {}

    float GetScrollT(float time_s) {}

    float GetS(unsigned char u, float time_s) {}

    float GetT(unsigned char v, float time_s) {}

    bool IsZWrite() {}

    bool IsDoubleSided() {}

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->ClassNameHash);
        bPlatEndianSwap(&this->ImageParentHash);
        bPlatEndianSwap(&this->ImagePlacement);
        bPlatEndianSwap(&this->PalettePlacement);
        bPlatEndianSwap(&this->ImageSize);
        bPlatEndianSwap(&this->PaletteSize);
        bPlatEndianSwap(&this->BaseImageSize);
        bPlatEndianSwap(&this->Width);
        bPlatEndianSwap(&this->Height);
        bPlatEndianSwap(&this->NumPaletteEntries);
        bPlatEndianSwap(&this->ScrollTimeStep);
        bPlatEndianSwap(&this->ScrollSpeedS);
        bPlatEndianSwap(&this->ScrollSpeedT);
        bPlatEndianSwap(&this->OffsetS);
        bPlatEndianSwap(&this->OffsetT);
        bPlatEndianSwap(&this->ScaleS);
        bPlatEndianSwap(&this->ScaleT);
    }

  public:
    char DebugName[24];           // offset 0xC, size 0x18
    uint32 NameHash;              // offset 0x24, size 0x4
    uint32 ClassNameHash;         // offset 0x28, size 0x4
    uint32 ImageParentHash;       // offset 0x2C, size 0x4
    int32 ImagePlacement;         // offset 0x30, size 0x4
    int32 PalettePlacement;       // offset 0x34, size 0x4
    int32 ImageSize;              // offset 0x38, size 0x4
    int32 PaletteSize;            // offset 0x3C, size 0x4
    int32 BaseImageSize;          // offset 0x40, size 0x4
    int16 Width;                  // offset 0x44, size 0x2
    int16 Height;                 // offset 0x46, size 0x2
    int8 ShiftWidth;              // offset 0x48, size 0x1
    int8 ShiftHeight;             // offset 0x49, size 0x1
    uint8 ImageCompressionType;   // offset 0x4A, size 0x1
    uint8 PaletteCompressionType; // offset 0x4B, size 0x1
    int16 NumPaletteEntries;      // offset 0x4C, size 0x2
    int8 NumMipMapLevels;         // offset 0x4E, size 0x1
    int8 TilableUV;               // offset 0x4F, size 0x1
    int8 BiasLevel;               // offset 0x50, size 0x1
    int8 RenderingOrder;          // offset 0x51, size 0x1
    int8 ScrollType;              // offset 0x52, size 0x1
    int8 UsedFlag;                // offset 0x53, size 0x1
    int8 ApplyAlphaSorting;       // offset 0x54, size 0x1
    int8 AlphaUsageType;          // offset 0x55, size 0x1
    int8 AlphaBlendType;          // offset 0x56, size 0x1
    int8 Flags;                   // offset 0x57, size 0x1
  private:
    int16 ScrollTimeStep; // offset 0x58, size 0x2
    int16 ScrollSpeedS;   // offset 0x5A, size 0x2
    int16 ScrollSpeedT;   // offset 0x5C, size 0x2
    int16 OffsetS;        // offset 0x5E, size 0x2
    int16 OffsetT;        // offset 0x60, size 0x2
    int16 ScaleS;         // offset 0x62, size 0x2
    int16 ScaleT;         // offset 0x64, size 0x2
  public:
    TexturePack *pTexturePack; // offset 0x68, size 0x4
    TextureInfo *pImageParent; // offset 0x6C, size 0x4
    void *ImageData;           // offset 0x70, size 0x4
    void *PaletteData;         // offset 0x74, size 0x4
    int32 ReferenceCount;      // offset 0x78, size 0x4
};

// total size: 0x10
class TextureAnimEntry {
  public:
    uint32 NameHash;           // offset 0x0, size 0x4
    TextureInfo *pTextureInfo; // offset 0x4, size 0x4
    void *pPlatAnimData;       // offset 0x8, size 0x4
    uint32 pad;                // offset 0xC, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
    }
};

// total size: 0x34
class TextureAnim {
  public:
    char Name[24];                      // offset 0x0, size 0x18
    uint32 NameHash;                    // offset 0x18, size 0x4
    int32 NumFrames;                    // offset 0x1C, size 0x4
    int32 FramesPerSecond;              // offset 0x20, size 0x4
    int32 TimeBase;                     // offset 0x24, size 0x4
    TextureAnimEntry *TextureAnimTable; // offset 0x28, size 0x4
    int32 Valid;                        // offset 0x2C, size 0x4
    int32 CurrentFrame;                 // offset 0x30, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->NumFrames);
        bPlatEndianSwap(&this->FramesPerSecond);
        bPlatEndianSwap(&this->TimeBase);
        bPlatEndianSwap(&this->Valid);
        bPlatEndianSwap(&this->CurrentFrame);
    }
};

// total size: 0x1C
class TextureAnimPack : public bTNode<TextureAnimPack> {
  public:
    TextureAnimPack(TexturePack *parent_texture_pack, TextureAnim *texture_anim_table, TextureAnimEntry *texture_anim_entry_table, int32 num_anims,
                    int32 num_anim_entries);
    ~TextureAnimPack();

    USE_SLOTALLOC(TexturePackSlotPool);

    void InitAnims();
    void EndianSwap() {}

    TexturePack *ParentTexturePack;          // offset 0x8, size 0x4
    TextureAnim *TextureAnimTable;           // offset 0xC, size 0x4
    TextureAnimEntry *TextureAnimEntryTable; // offset 0x10, size 0x4
    int32 NumTextureAnims;                   // offset 0x14, size 0x4
    int32 NumTextureAnimEntries;             // offset 0x18, size 0x4
};

// total size: 0x10
class TextureAnimPackHeader {
  public:
    TextureAnimPackHeader() {}

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
    }

    int32 Version;                     // offset 0x0, size 0x4
    TextureAnimPack *pTextureAnimPack; // offset 0x4, size 0x4
    int32 EndianSwapped;               // offset 0x8, size 0x4
    int32 pad;                         // offset 0xC, size 0x4
};

void eInitTextures();
void UpdateTextureAnimations();
TextureInfo *GetTextureInfo(uint32 name_hash, int return_default_texture_if_not_found, int include_unloaded_textures);
TextureInfo *FixupTextureInfo(TextureInfo *texture_info, uint32 name_hash, TexturePack *texture_pack, bool loading);
void MaybePrintUnusedTextures();
int eLoadStreamingTexturePack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);
void eLoadStreamingTexture(uint32 *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
void eWaitForStreamingTexturePackLoading(const char *filename);
void eUnloadAllStreamingTextures(const char *filename);
void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes);
void eUnloadStreamingTexturePack(const char *filename);

TextureInfo *eCreateTextureInfo();
void eDestroyTextureInfo(TextureInfo *texture_info);

extern TextureInfo *DefaultTextureInfo;

inline TextureInfo *FixupTextureInfoLoading(TextureInfo *texture_info, uint32 name_hash, TexturePack *texture_pack) {
    return FixupTextureInfo(texture_info, name_hash, texture_pack, true);
}

inline TextureInfo *FixupTextureInfoUnloading(TextureInfo *texture_info, uint32 name_hash, TexturePack *texture_pack) {
    return FixupTextureInfo(texture_info, name_hash, texture_pack, false);
}

inline int eLoadStreamingTexturePack(const char *filename) {
    return eLoadStreamingTexturePack(filename, nullptr, nullptr, 0);
}

inline void eLoadStreamingTexturePack(const char *filename, void (*callback_func)(uintptr_t), uintptr_t callback_param, int memory_pool_num) {
    eLoadStreamingTexturePack(filename, reinterpret_cast<void (*)(void *)>(callback_func), reinterpret_cast<void *>(callback_param), memory_pool_num);
}

inline void eLoadStreamingTexture(uint32 *name_hash_table, int num_hashes) {
    eLoadStreamingTexture(name_hash_table, num_hashes, nullptr, nullptr, 0);
}

inline void eLoadStreamingTexture(uint32 name_hash) {
    eLoadStreamingTexture(&name_hash, 1);
}

inline void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(uintptr_t), uintptr_t param0, int memory_pool_num) {
    eLoadStreamingTexture(name_hash_table, num_hashes, reinterpret_cast<void (*)(void *)>(callback), reinterpret_cast<void *>(param0),
                          memory_pool_num);
}

#endif
