#include "rcmp/rcmp.h"
#include "dolphin.h"

struct tBigSwizzler;

extern "C" char lbl_804100A4[];

extern void DELETE_tBigSwizzler(tBigSwizzler *This);

struct tBigYUVSwizzler {
    unsigned char m_YTable[0x200];
    unsigned char m_UTable[0x200];
    unsigned char m_VTable[0x200];
    tBigSwizzler *m_YSwizzler;
    tBigSwizzler *m_UVSwizzler;
    GXTlutObj m_YTlut;
    GXTlutObj m_UTlut;
    GXTlutObj m_VTlut;
};

void DELETE_tBigYUVSwizzler(tBigYUVSwizzler *This) {
    DELETE_tBigSwizzler(This->m_YSwizzler);
    DELETE_tBigSwizzler(This->m_UVSwizzler);
    RCMP::rcmp_sys.FreeMem(This);
}

extern tBigSwizzler *NEW_tBigSwizzlerTexture(struct _GXTexObj *tTexp);

struct tBigYUVSwizzler *NEW_tBigYUVSwizzlerTexture(struct _GXTexObj *tYTexp,
                                                   struct _GXTexObj *tUTexp,
                                                   struct _GXTexObj *) {
    int i;
    float fi;
    tBigYUVSwizzler *This;

    This = static_cast<tBigYUVSwizzler *>(RCMP::rcmp_sys.AllocMem(
        lbl_804100A4, 0x640, 0x20, 0, RCMP::rcmp_sys.m_DefaultMemDir));
    This->m_YSwizzler = NEW_tBigSwizzlerTexture(tYTexp);
    This->m_UVSwizzler = NEW_tBigSwizzlerTexture(tUTexp);

    for (i = 0; i < 0x100; ++i) {
        fi = static_cast<float>(i) - 128.0f;
        This->m_YTable[i * 2] = 0xff;
        This->m_YTable[i * 2 + 1] = static_cast<unsigned char>(i);
        This->m_UTable[i * 2] = static_cast<unsigned char>(fi * 0.7009999752044678f + 128.0f);
        This->m_UTable[i * 2 + 1] = static_cast<unsigned char>(fi * -0.35705000162124634f + 85.33333587646484f);
        This->m_VTable[i * 2] = static_cast<unsigned char>(fi * -0.17204999923706055f + 42.66666793823242f);
        This->m_VTable[i * 2 + 1] = static_cast<unsigned char>(fi * 0.8859999775886536f + 128.0f);
    }

    DCFlushRange(This->m_YTable, 0x200);
    DCFlushRange(This->m_UTable, 0x200);
    DCFlushRange(This->m_VTable, 0x200);
    GXInitTlutObj(&This->m_YTlut, This->m_YTable, GX_TL_IA8, 0x100);
    GXInitTlutObj(&This->m_UTlut, This->m_UTable, GX_TL_IA8, 0x100);
    GXInitTlutObj(&This->m_VTlut, This->m_VTable, GX_TL_IA8, 0x100);
    return This;
}
