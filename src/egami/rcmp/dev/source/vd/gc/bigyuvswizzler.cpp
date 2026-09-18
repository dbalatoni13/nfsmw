struct _GXTexObj {
    unsigned long dummy[8];
};

struct _GXTlutObj {
    unsigned long dummy[3];
};

struct _GXColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

typedef struct _GXColor GXColor;

extern "C" {
void DCFlushRange(void *addr, unsigned long nBytes);
void GXInitTexObjLOD(struct _GXTexObj *obj, int min_filt, int mag_filt, float min_lod, float max_lod, float lod_bias, unsigned char bias_clamp, unsigned char do_edge_lod, int max_aniso);
void GXInitTlutObj(struct _GXTlutObj *tlut_obj, void *lut, int fmt, unsigned short n_entries);
void GXLoadTlut(struct _GXTlutObj *tlut_obj, unsigned long tlut_name);
void GXLoadTexObj(struct _GXTexObj *obj, int id);
void GXSetIndTexCoordScale(int ind_stage, int scale_s, int scale_t);
void GXSetIndTexMtx(int mtx_id, const float offset[3][2], signed char scale_exp);
void GXSetIndTexOrder(int ind_stage, int tex_coord, int tex_map);
void GXSetNumIndStages(unsigned char nIndStages);
void GXSetNumTevStages(unsigned char nStages);
void GXSetNumTexGens(unsigned char nTexGens);
void GXSetTevAlphaIn(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp(int stage, int op, int bias, int scale, unsigned char clamp, int out_reg);
void GXSetTevColorIn(int stage, int a, int b, int c, int d);
void GXSetTevColorOp(int stage, int op, int bias, int scale, unsigned char clamp, int out_reg);
void GXSetTevIndirect(int tev_stage, int ind_stage, int format, int bias_sel, int matrix_sel, int wrap_s, int wrap_t, unsigned char add_prev, unsigned char utc_lod, int alpha_sel);
void GXSetTevIndRepeat(int tev_stage);
void GXSetTevKColor(int id, GXColor color);
void GXSetTevKColorSel(int stage, int sel);
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevSwapMode(int stage, int ras_sel, int tex_sel);
void GXSetTevSwapModeTable(int table, int red, int green, int blue, int alpha);
void GXSetTexCoordGen2(int dst_coord, int func, int src_param, unsigned long mtx, unsigned char normalize, unsigned long pt_texmtx);
}

static inline void GXSetTexCoordGen(int dst_coord, int func, int src_param, unsigned long mtx) {
    GXSetTexCoordGen2(dst_coord, func, src_param, mtx, 0, 0x7d);
}

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

struct tBigSwizzler {
    unsigned int TileSizeX;
    unsigned int TileSizeY;
    unsigned int TileLutSizeX;
    unsigned int TileLutSizeY;
    tBigSTPix *TileLut;
    _GXTexObj TileLutTex;
    unsigned int PixLutSizeX;
    unsigned int PixLutSizeY;
    tBigSTPix *PixLut;
    _GXTexObj PixLutTex;
};

struct tBigYVUSwizzlerAIPix {
    unsigned char a;
    unsigned char i;
};

struct tBigYUVSwizzler {
    tBigYVUSwizzlerAIPix YClutData[256];
    tBigYVUSwizzlerAIPix CrClutData[256];
    tBigYVUSwizzlerAIPix CbClutData[256];
    tBigSwizzler *YSwizzler;
    tBigSwizzler *UVSwizzler;
    _GXTlutObj YClut;
    _GXTlutObj CrClut;
    _GXTlutObj CbClut;
} __attribute__((aligned(32)));

tBigSwizzler *NEW_tBigSwizzlerTexture(_GXTexObj *tTexp);
void DELETE_tBigSwizzler(tBigSwizzler *This);

tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(_GXTexObj *tYTexp, _GXTexObj *tUTexp, _GXTexObj *tVTexp);
void DELETE_tBigYUVSwizzler(tBigYUVSwizzler *This);
void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler *This, _GXTexObj *YImage, _GXTexObj *CrImage, _GXTexObj *CbImage);

void DELETE_tBigYUVSwizzler(tBigYUVSwizzler *This) {
    DELETE_tBigSwizzler(This->YSwizzler);
    DELETE_tBigSwizzler(This->UVSwizzler);
    RCMP::rcmp_sys.FreeMem(This);
}

tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(_GXTexObj *tYTexp, _GXTexObj *tUTexp, _GXTexObj *tVTexp) {
    int i;
    float fi;
    tBigYUVSwizzler *This;

    This = (tBigYUVSwizzler *)RCMP::rcmp_sys.AllocMem("VD::tBigSwizzler", sizeof(tBigYUVSwizzler), 32, 0, RCMP::rcmp_sys.m_DefaultMemDir);

    This->YSwizzler = NEW_tBigSwizzlerTexture(tYTexp);
    This->UVSwizzler = NEW_tBigSwizzlerTexture(tUTexp);

    for (i = 0; i < 256; i++) {

        fi = (float)i - 128.0f;
        This->YClutData[i].a = 255;
        This->YClutData[i].i = i;

        This->CrClutData[i].a = fi * 0.701f + 128.0f;
        This->CrClutData[i].i = fi * -0.35705f + 85.333336f;
        This->CbClutData[i].a = fi * -0.17205f + 42.666668f;
        This->CbClutData[i].i = fi * 0.886f + 128.0f;
    }

    DCFlushRange(This->YClutData, sizeof(This->YClutData));
    DCFlushRange(This->CrClutData, sizeof(This->CrClutData));
    DCFlushRange(This->CbClutData, sizeof(This->CbClutData));

    GXInitTlutObj(&This->YClut, This->YClutData, 0, 256);
    GXInitTlutObj(&This->CrClut, This->CrClutData, 0, 256);
    GXInitTlutObj(&This->CbClut, This->CbClutData, 0, 256);

    return This;
}

void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler *This, _GXTexObj *YImage, _GXTexObj *CrImage, _GXTexObj *CbImage) {
    float mtx[3][2];
    tBigSwizzler *YSwizzler = This->YSwizzler;
    tBigSwizzler *UVSwizzler = This->UVSwizzler;

    GXColor RGMask = {255, 255, 0, 0};

    GXColor GBMask = {0, 255, 255, 0};

    GXSetIndTexCoordScale(0, 0, 0);
    GXSetIndTexCoordScale(1, 0, 0);
    GXSetIndTexCoordScale(2, 0, 0);
    GXSetIndTexCoordScale(3, 0, 0);

    GXInitTexObjLOD(CbImage, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjLOD(CrImage, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjLOD(YImage, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);

    GXLoadTlut(&This->CbClut, 0);
    GXLoadTlut(&This->CrClut, 1);
    GXLoadTlut(&This->YClut, 2);

    GXLoadTexObj(CbImage, 0);
    GXLoadTexObj(CrImage, 1);
    GXLoadTexObj(YImage, 2);

    GXLoadTexObj(&UVSwizzler->TileLutTex, 3);
    GXLoadTexObj(&UVSwizzler->PixLutTex, 4);

    GXLoadTexObj(&YSwizzler->TileLutTex, 5);
    GXLoadTexObj(&YSwizzler->PixLutTex, 6);

    GXSetTevKColor(0, RGMask);
    GXSetTevKColor(1, GBMask);

    GXSetTevSwapModeTable(1, 3, 0, 0, 0);
    GXSetTevSwapModeTable(2, 1, 3, 1, 1);

    GXSetNumTexGens(2);

    GXSetTexCoordGen(0, 1, 0xc, 0x3c);
    GXSetTexCoordGen(1, 1, 0xc, 0x3c);

    mtx[0][0] = (float)UVSwizzler->TileSizeX * 0.0625f;
    mtx[0][1] = 0.0f;
    mtx[2][0] = (float)UVSwizzler->TileSizeY * 0.0625f;
    mtx[1][1] = 0.0f;
    mtx[1][0] = 0.0f;
    mtx[2][1] = 0.0f;
    GXSetIndTexMtx(1, mtx, 4);

    mtx[0][0] = 0.0625f;
    mtx[0][1] = 0.0f;
    mtx[1][1] = 0.0f;
    mtx[1][0] = 0.0f;
    mtx[2][0] = 0.0625f;
    mtx[2][1] = 0.0f;
    GXSetIndTexMtx(2, mtx, 4);

    GXSetNumIndStages(4);

    GXSetIndTexOrder(0, 0, 3);
    GXSetTevIndirect(0, 0, 0, 0, 1, 6, 6, 0, 0, 0);

    GXSetIndTexOrder(1, 0, 4);
    GXSetTevIndirect(1, 1, 0, 2, 2, 6, 0, 1, 0, 0);

    GXSetIndTexOrder(2, 1, 5);
    GXSetTevIndirect(3, 2, 0, 0, 1, 6, 6, 0, 0, 0);

    GXSetIndTexOrder(3, 1, 6);
    GXSetTevIndirect(4, 3, 0, 2, 2, 6, 0, 1, 0, 0);

    GXSetNumTevStages(5);

    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevOrder(1, 0, 0, 0xff);
    GXSetTevIndRepeat(2);
    GXSetTevOrder(2, 0, 1, 0xff);
    GXSetTevOrder(3, 1, 2, 0xff);
    GXSetTevOrder(4, 1, 2, 0xff);

    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xf);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);

    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 0xf, 8, 0xe, 0xf);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 7);
    GXSetTevSwapMode(1, 1, 2);
    GXSetTevKColorSel(1, 0xd);

    GXSetTevColorOp(2, 0, 2, 1, 0, 0);
    GXSetTevColorIn(2, 0xf, 8, 0xe, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 7, 7, 7, 7);
    GXSetTevSwapMode(2, 1, 1);
    GXSetTevKColorSel(2, 0xc);

    GXSetTevColorOp(3, 0, 0, 0, 0, 0);
    GXSetTevColorIn(3, 0xf, 0xf, 0xf, 0);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 7, 7, 7);

    GXSetTevColorOp(4, 0, 0, 0, 1, 0);
    GXSetTevColorIn(4, 8, 0xf, 0xf, 0);
    GXSetTevAlphaOp(4, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(4, 7, 7, 7, 4);
}
