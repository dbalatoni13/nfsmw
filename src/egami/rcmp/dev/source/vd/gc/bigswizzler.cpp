#include <dolphin/gx/GXGet.h>
#include <dolphin/gx/GXTexture.h>
#include <dolphin/os/OSCache.h>

namespace RCMP {

class RCMP_SYSTEM {
  public:
    void *(*AllocMemFunc)(const char *, int, int, int, int);
    void (*FreeMemFunc)(void *);
    int m_DefaultMemDir;

    void *AllocMem(const char *name, int size, int alignment, int headersize, int type) {
        return AllocMemFunc(name, size, alignment, headersize, type);
    }
    void FreeMem(void *memadr) {
        FreeMemFunc(memadr);
    }
};

extern RCMP_SYSTEM rcmp_sys;

} // namespace RCMP

struct tBigSTPix {
    unsigned char s;
    unsigned char t;
};

struct tPixAdr2d {
    unsigned long PixX;
    unsigned long PixY;
};

struct tTileSize2d {
    unsigned long TileSizeX;
    unsigned long TileSizeY;
    unsigned long SizeX;
    unsigned long SizeY;
    unsigned long MaxTileX;
    unsigned long MaxTileY;
    unsigned long TileSize;
};

struct tBigSwizzler {
    unsigned long TileSizeX;
    unsigned long TileSizeY;
    unsigned long TileLutSizeX;
    unsigned long TileLutSizeY;
    tBigSTPix *TileLut;
    GXTexObj TileLutTex;
    unsigned long PixLutSizeX;
    unsigned long PixLutSizeY;
    tBigSTPix *PixLut;
    GXTexObj PixLutTex;
};

tBigSwizzler *NEW_tBigSwizzlerTexture(GXTexObj *tTexp);
void DELETE_tBigSwizzler(tBigSwizzler *This);

static void CON_tTileSize2d(tTileSize2d *tsize, unsigned long TileSizeX, unsigned long TileSizeY, unsigned long SizeX, unsigned long SizeY) {
    tsize->TileSizeX = TileSizeX;
    tsize->TileSizeY = TileSizeY;
    tsize->SizeX = SizeX;
    tsize->SizeY = SizeY;
    tsize->MaxTileX = (SizeX + (TileSizeX - 1)) / TileSizeX;
    tsize->MaxTileY = (SizeY + (TileSizeY - 1)) / TileSizeY;
    tsize->TileSize = TileSizeX * TileSizeY;
}

static void CON_tPixAdr2d(tPixAdr2d *padr, unsigned long x, unsigned long y, tTileSize2d *tsize) {
    unsigned long TileX = ((y * tsize->SizeX + x) / tsize->TileSize) % tsize->MaxTileX;
    unsigned long TileY = y / tsize->TileSizeY;

    unsigned long PixX = x % tsize->TileSizeX;
    unsigned long PixY = ((y * tsize->SizeX + x) % tsize->TileSize) / tsize->TileSizeX;

    padr->PixX = PixX + TileX * tsize->TileSizeX;
    padr->PixY = PixY + TileY * tsize->TileSizeY;
}

static int GC_swizzleGetPixelOffset16(int x, int y, int width) {
    int NumberOfBlocksX = (width + 3) >> 2;
    int XBlock = x >> 2;
    int YBlock = y >> 2;
    int XPix = x & 3;
    int YPix = y & 3;

    return ((YBlock * NumberOfBlocksX + XBlock) * 32) + (YPix * 8 + XPix * 2);
}

void DELETE_tBigSwizzler(tBigSwizzler *This) {
    RCMP::rcmp_sys.FreeMem(This->TileLut);
    RCMP::rcmp_sys.FreeMem(This->PixLut);
    RCMP::rcmp_sys.FreeMem(This);
}

tBigSwizzler *NEW_tBigSwizzlerTexture(GXTexObj *tTexp) {
    unsigned int TileSizeX = 0;
    unsigned int TileSizeY = 0;
    unsigned int SizeX = GXGetTexObjWidth(tTexp);
    unsigned int SizeY = GXGetTexObjHeight(tTexp);
    tTileSize2d tsize;
    unsigned int x;
    unsigned int y;
    tPixAdr2d padr;
    tBigSwizzler *This;
    tBigSTPix *CurPixLut;
    tBigSTPix *CurTileLut;

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
    }

    CON_tTileSize2d(&tsize, TileSizeX, TileSizeY, SizeX, SizeY);

    This = (tBigSwizzler *)RCMP::rcmp_sys.AllocMem("VD::tBigSwizzler", sizeof(tBigSwizzler), 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);

    This->TileSizeX = TileSizeX;
    This->TileSizeY = TileSizeY;
    This->TileLutSizeX = SizeX;
    This->TileLutSizeY = TileSizeY;

    This->TileLut = (tBigSTPix *)RCMP::rcmp_sys.AllocMem("VD::tBigSwizzler", SizeX * TileSizeY * sizeof(tBigSTPix), 32, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    This->PixLutSizeX = SizeX;
    This->PixLutSizeY = TileSizeY;

    This->PixLut = (tBigSTPix *)RCMP::rcmp_sys.AllocMem("VD::tBigSwizzler", SizeX * TileSizeY * sizeof(tBigSTPix), 32, 0, RCMP::rcmp_sys.m_DefaultMemDir);

    for (y = 0; y < This->TileLutSizeY; y++) {
        for (x = 0; x < This->TileLutSizeX; x++) {
            CON_tPixAdr2d(&padr, x, y, &tsize);
            CurTileLut = &This->TileLut[GC_swizzleGetPixelOffset16(x, y, This->PixLutSizeX) / 2];
            CurTileLut->s = padr.PixX / TileSizeX;
            CurTileLut->t = padr.PixY / TileSizeY;
        }
    }

    GXInitTexObj(&This->TileLutTex, This->TileLut, This->TileLutSizeX, This->TileLutSizeY, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&This->TileLutTex, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    for (y = 0; y < This->PixLutSizeY; y++) {
        for (x = 0; x < This->PixLutSizeX; x++) {
            CON_tPixAdr2d(&padr, x, y, &tsize);
            CurPixLut = &This->PixLut[GC_swizzleGetPixelOffset16(x, y, This->PixLutSizeX) / 2];
            CurPixLut->s = padr.PixX % TileSizeX;
            CurPixLut->t = padr.PixY % TileSizeY - y + 128;
        }
    }

    GXInitTexObj(&This->PixLutTex, This->PixLut, This->PixLutSizeX, This->PixLutSizeY, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&This->PixLutTex, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    DCFlushRange(This->TileLut, This->TileLutSizeX * This->TileLutSizeY * sizeof(tBigSTPix));
    DCFlushRange(This->PixLut, This->PixLutSizeX * This->PixLutSizeY * sizeof(tBigSTPix));

    return This;
}

