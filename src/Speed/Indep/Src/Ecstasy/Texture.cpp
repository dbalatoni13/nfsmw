#include "./Texture.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "types.h"

SlotPool *TexturePackSlotPool;
BOOL DuplicateTextureWarningEnabled;
bTList<TextureVRAMDataHeader> TextureVRAMDataHeaderList;

void eInitTextures(void) {
  TexturePackSlotPool = bNewSlotPool(bMax(28, 32), 128, "TexturePackSlotPool", 0);
}

void SetDuplicateTextureWarning(BOOL enabled) {
  DuplicateTextureWarningEnabled = enabled;
}

TextureVRAMDataHeader *FindVRAMData(unsigned int filename_hash) {
  for (TextureVRAMDataHeader *vram_header = TextureVRAMDataHeaderList.GetHead(); vram_header != TextureVRAMDataHeaderList.EndOfList();
       vram_header = vram_header->GetNext()) {
    if (vram_header->FilenameHash == filename_hash) {
      return vram_header;
    }
  }
  return nullptr;
}
