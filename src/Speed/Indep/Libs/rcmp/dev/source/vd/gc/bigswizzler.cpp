#include "rcmp/rcmp.h"
#include "dolphin.h"

extern "C" char lbl_8041010C[];

struct tTileSize2d {
    unsigned int TileWidth;
    unsigned int TileHeight;
    unsigned int Width;
    unsigned int Height;
    unsigned int NumberOfTilesX;
    unsigned int NumberOfTilesY;
    unsigned int TileSize;
};

struct tPixAdr2d {
    unsigned int X;
    unsigned int Y;
};

struct tBigSTPix {
    unsigned char X;
    unsigned char Y;
};

struct tBigSwizzler {
    unsigned int TileWidth;
    unsigned int TileHeight;
    unsigned int Width;
    unsigned int Height;
    tBigSTPix *TextureData;
    GXTexObj TextureObj;
    unsigned int TextureWidth;
    unsigned int TextureHeight;
    tBigSTPix *TextureData2;
    GXTexObj TextureObj2;
};

struct _GXTexObj;

static void CON_tTileSize2d(tTileSize2d *tsize, unsigned long TileSizeX,
                            unsigned long TileSizeY, unsigned long SizeX,
                            unsigned long SizeY) {
    unsigned long NumberOfTilesX;
    unsigned long NumberOfTilesY;

    tsize->TileWidth = TileSizeX;
    tsize->TileHeight = TileSizeY;
    tsize->Width = SizeX;
    tsize->Height = SizeY;
    NumberOfTilesX = SizeX - 1;
    NumberOfTilesX += TileSizeX;
    NumberOfTilesY = SizeY - 1;
    NumberOfTilesY += TileSizeY;
    tsize->NumberOfTilesX = NumberOfTilesX / TileSizeX;
    tsize->NumberOfTilesY = NumberOfTilesY / TileSizeY;
    tsize->TileSize = TileSizeX * TileSizeY;
}

static void CON_tPixAdr2d(tPixAdr2d *padr, unsigned long x, unsigned long y,
                          tTileSize2d *tsize) {
    unsigned int TileX;
    unsigned int TileY;
    unsigned int PixX;
    unsigned int PixY;

    TileX = ((y * tsize->Width + x) / tsize->TileSize) % tsize->NumberOfTilesX;
    TileY = (y / tsize->TileHeight) * tsize->TileHeight;
    PixX = (x % tsize->TileWidth) + TileX * tsize->TileWidth;
    PixY = (((y * tsize->Width + x) % tsize->TileSize) / tsize->TileWidth) + TileY;
    padr->X = PixX;
    padr->Y = PixY;
}

static int GC_swizzleGetPixelOffset16(int x, int y, int width) {
    int NumberOfBlocksX;
    int XBlock;
    int YBlock;
    int XPix;
    int YPix;

    NumberOfBlocksX = (width + 3) >> 2;
    XBlock = x >> 2;
    YBlock = y >> 2;
    XPix = (x & 3) * 2;
    YPix = (y & 3) * 8;
    return (((YBlock * NumberOfBlocksX + XBlock) << 5) + (YPix + XPix));
}

void DELETE_tBigSwizzler(tBigSwizzler *swizzler) {
    RCMP::rcmp_sys.FreeMem(swizzler->TextureData);
    RCMP::rcmp_sys.FreeMem(swizzler->TextureData2);
    RCMP::rcmp_sys.FreeMem(swizzler);
}

struct tBigSwizzler *NEW_tBigSwizzlerTexture(_GXTexObj *tTexp) {
    unsigned int TileSizeX;
    unsigned int TileSizeY;
    unsigned int SizeX;
    unsigned int SizeY;
    tTileSize2d tsize;
    unsigned int x;
    unsigned int y;
    tPixAdr2d padr;
    tBigSwizzler *This;
    tBigSTPix *CurPixLut;
    tBigSTPix *CurTileLut;

    TileSizeX = 0;
    TileSizeY = 0;
    SizeX = GXGetTexObjWidth(tTexp);
    SizeY = GXGetTexObjHeight(tTexp);

    switch (GXGetTexObjFmt(tTexp)) {
      case GX_TF_I4:
      case GX_TF_C4:
        TileSizeX = 8;
        TileSizeY = 8;
        break;
      case GX_TF_I8:
      case GX_TF_IA4:
      case GX_TF_C8:
        TileSizeX = 8;
        TileSizeY = 4;
        break;
      case GX_TF_IA8:
      case GX_TF_RGB565:
      case GX_TF_RGB5A3:
        TileSizeX = 4;
        TileSizeY = 4;
        break;
      default:
        break;
    }

    CON_tTileSize2d(&tsize, TileSizeX, TileSizeY, SizeX, SizeY);
    This = static_cast<tBigSwizzler *>(RCMP::rcmp_sys.AllocMem(
        lbl_8041010C, 0x60, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir));
    This->TileWidth = TileSizeX;
    This->TileHeight = TileSizeY;
    This->Width = SizeX;
    This->Height = TileSizeY;
    This->TextureData = static_cast<tBigSTPix *>(RCMP::rcmp_sys.AllocMem(
        lbl_8041010C, static_cast<int>(SizeX * TileSizeY * 2), 0x20, 0,
        RCMP::rcmp_sys.m_DefaultMemDir));
    This->TextureWidth = SizeX;
    This->TextureHeight = TileSizeY;
    This->TextureData2 = static_cast<tBigSTPix *>(RCMP::rcmp_sys.AllocMem(
        lbl_8041010C, static_cast<int>(SizeX * TileSizeY * 2), 0x20, 0,
        RCMP::rcmp_sys.m_DefaultMemDir));

    for (y = 0; y < This->Height; ++y) {
        for (x = 0; x < This->Width; ++x) {
            CON_tPixAdr2d(&padr, x, y, &tsize);
            CurPixLut = This->TextureData;
            CurTileLut = CurPixLut + (GC_swizzleGetPixelOffset16(x, y, This->TextureWidth) / 2);
            CurTileLut->X = static_cast<unsigned char>(padr.X / TileSizeX);
            CurTileLut->Y = static_cast<unsigned char>(padr.Y / TileSizeY);
        }
    }

    GXInitTexObj(&This->TextureObj, This->TextureData, This->Width, This->Height,
                 GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&This->TextureObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f,
                    GX_FALSE, GX_FALSE, GX_ANISO_1);

    for (y = 0; y < This->TextureHeight; ++y) {
        for (x = 0; x < This->TextureWidth; ++x) {
            CON_tPixAdr2d(&padr, x, y, &tsize);
            CurPixLut = reinterpret_cast<tBigSTPix *>(
                reinterpret_cast<unsigned char *>(This->TextureData2) +
                (GC_swizzleGetPixelOffset16(x, y, This->TextureWidth) / 2) *
                    2);
            CurTileLut = This->TextureData2;
            CurPixLut += CurTileLut - CurTileLut;
            CurPixLut->X = static_cast<unsigned char>(padr.X % TileSizeX);
            CurPixLut->Y = static_cast<unsigned char>((padr.Y % TileSizeY) - y + 0x80);
        }
    }

    GXInitTexObj(&This->TextureObj2, This->TextureData2, This->TextureWidth,
                 This->TextureHeight, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&This->TextureObj2, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f,
                    GX_FALSE, GX_FALSE, GX_ANISO_1);

    DCFlushRange(This->TextureData, This->Width * This->Height * 2);
    DCFlushRange(This->TextureData2, This->TextureWidth * This->TextureHeight * 2);
    return This;
}
