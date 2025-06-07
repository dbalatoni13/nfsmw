#pragma once

#ifdef TARGET_GC
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#endif

struct TextureIndexEntry {
  // total size: 0x8
  unsigned int NameHash;            // offset 0x0, size 0x4
  struct TextureInfo *pTextureInfo; // offset 0x4, size 0x4
};

struct eStreamingEntry {
  // total size: 0x18
  unsigned int NameHash;        // offset 0x0, size 0x4
  unsigned int ChunkByteOffset; // offset 0x4, size 0x4
  int ChunkByteSize;            // offset 0x8, size 0x4
  int UncompressedSize;         // offset 0xC, size 0x4
  unsigned char UserFlags;      // offset 0x10, size 0x1
  unsigned char Flags;          // offset 0x11, size 0x1
  unsigned short RefCount;      // offset 0x12, size 0x2
  unsigned char *ChunkData;     // offset 0x14, size 0x4
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
};

struct TexturePack : public bTNode<TexturePack> {
  // total size: 0x20
  const char *Name;               // offset 0x8, size 0x4
  const char *Filename;           // offset 0xC, size 0x4
  unsigned int NameHash;          // offset 0x10, size 0x4
  TexturePackHeader *pPackHeader; // offset 0x14, size 0x4
  int NumTextures;                // offset 0x18, size 0x4
  int TextureDataSize;            // offset 0x1C, size 0x4
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
};

struct TextureVRAMDataHeader : public bTNode<TextureVRAMDataHeader> {
  // total size: 0x18
  int Version;                  // offset 0x8, size 0x4
  unsigned int FilenameHash;    // offset 0xC, size 0x4
  int EndianSwapped;            // offset 0x10, size 0x4
  struct bChunk *VRAMDataChunk; // offset 0x14, size 0x4

  void EndianSwap() {}
};
