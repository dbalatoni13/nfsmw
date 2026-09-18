#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <dolphin.h>

// Los "vertex shaders" de GameCube son descripciones de vertice: cada uno fija
// el GXVtxDesc y devuelve el GXVtxFmt con el que hay que dibujar.  Igual que en
// ePShader, prevVShader evita reprogramar lo que ya esta puesto.

enum VShaderMode {
    eVS_SCREEN = 0,
    eVS_MODEL,
    eVS_PRIM,
    eVS_DEBUG_TEXT,
    eVS_MAXNUM,
};

static VShaderMode prevVShader = eVS_MAXNUM;


















int vsReset(int opt) {
    prevVShader = eVS_MAXNUM;
    return 0;
}


int vsResetTexGen(int opt, int stage) {



    GXSetTexCoordGen((GXTexCoordID)stage, GX_TG_MTX2x4, (GXTexGenSrc)(GX_TG_TEX0 + stage), GX_IDENTITY);

    return 0;
}


int vsScreen(int opt) {
    static int prevVtxFmt;
    static int prevopt;

    _GXVtxFmt crtVtxFmt = GX_VTXFMT0;

    if (prevVShader != eVS_SCREEN || prevopt != opt || prevVtxFmt != crtVtxFmt) {




        prevVShader = eVS_SCREEN;

        GXClearVtxDesc();
        if (opt == 0) {



            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        }







        prevopt = opt;
        prevVtxFmt = crtVtxFmt;
    }
    return crtVtxFmt;
}



int vsModel(int vertex_format, int vertex_description) {
    int vtxfmt = GX_VTXFMT0;
    static int prevVertexFormat;
    static int prevVertexDescription;

    if (vertex_format == 0) {

        vtxfmt = GX_VTXFMT0;
    }
    else if (vertex_format == 1) {

        vtxfmt = GX_VTXFMT1;
    }
    else if (vertex_format == 2 || vertex_format == 0x16) {

        vtxfmt = GX_VTXFMT2;
    }
    else if (vertex_format == 3 || vertex_format == 0x20) {

        vtxfmt = GX_VTXFMT3;
    }
    else if (vertex_format == 4) {

        vtxfmt = GX_VTXFMT4;
    }
    else if (vertex_format == 5) {

        vtxfmt = GX_VTXFMT5;
    }




    // Si nada ha cambiado el GXVtxDesc ya esta puesto.

    if (prevVShader == eVS_MODEL && prevVertexFormat == vertex_format && prevVertexDescription == vertex_description) {

        return vtxfmt;
    }

    prevVShader = eVS_MODEL;

    GXClearVtxDesc();

    if (vertex_description < 0 || vertex_description == 1) {

        if (vertex_format == 0) {



            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        }
        else if (vertex_format == 1) {



            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        }
        else if (vertex_format == 2) {



            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        }
        else if (vertex_format == 3) {



            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        }
        else if (vertex_format == 4) {



            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        }
        else if (vertex_format == 5) {



            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        }
        else if (vertex_format == 0x16) {



            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        }
        else if (vertex_format == 0x20) {



            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);



            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        }

    }



    else if (vertex_format == 2 || vertex_format == 0x16) {

        switch (vertex_description) {
        case -1:
            break;
        case 0x00:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;


        case 0x02:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x04:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x08:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x10:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;


        case 0x18:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x12:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x06:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x0C:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x14:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x0A:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;


        case 0x1C:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x1A:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x16:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x0E:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;


        case 0x1E:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR1, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;









        }
    }

    else if (vertex_format == 3 || vertex_format == 0x20 || vertex_format == 1) {

        switch (vertex_description) {
        case -1:
            break;
        case 0x00:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;


        case 0x02:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x04:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x08:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x10:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;


        case 0x18:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x12:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;





        case 0x0C:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;





        case 0x0A:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;


        case 0x14: case 0x1C:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            break;
        case 0x1A:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x16:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        case 0x06: case 0x0E:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;


        case 0x1E:
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
            break;
        }
    }
















    prevVertexFormat = vertex_format;
    prevVertexDescription = vertex_description;
    return vtxfmt;
}



int vsScreenMultiTexture(int opt) {
    prevVShader = eVS_PRIM;


    // Como vsScreen pero con una segunda coordenada de textura.
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);

    return GX_VTXFMT6;
}



















int vsVtxAttrFmt(int opt) {

    // VTXFMT0: pantalla, todo en coma flotante.
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);


    // VTXFMT1: como el 0 pero con UV en 1.15 fijo.

    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);



    // VTXFMT2: mundo comprimido, con normal y dos canales de color.

    GXSetVtxAttrFmt(GX_VTXFMT2, GX_VA_POS, GX_POS_XYZ, GX_S16, 11);
    GXSetVtxAttrFmt(GX_VTXFMT2, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT2, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);


    // VTXFMT3: mundo sin normal.
    GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT3, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);


    // VTXFMT4: coches, con normal en coma flotante.
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA4, 0);
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);


    // VTXFMT5: coches sin normal.
    GXSetVtxAttrFmt(GX_VTXFMT5, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT5, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT5, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);


    // VTXFMT6: pantalla multitextura.
    GXSetVtxAttrFmt(GX_VTXFMT6, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT6, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT6, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT6, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);

    return 0;
}

