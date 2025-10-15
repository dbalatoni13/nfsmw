/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d9types.h
 *  Content:    Direct3D capabilities include file
 *
 ***************************************************************************/

#ifndef _d3d9TYPES_H_
#define _d3d9TYPES_H_

#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION         0x0900
#endif  //DIRECT3D_VERSION

// include this file content only if compiling for DX9 interfaces
#if(DIRECT3D_VERSION >= 0x0900)

// The PPC back-end of the C compiler by default defines bitfields to be
// ordered from the MSB to the LSB, which is opposite the convention on
// the X86 platform.  Use the 'bitfield_order' pragma to switch the
// ordering.  Note that this does not affect endianness in any way.

#if defined(_M_PPCBE)
#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
#endif

#include <float.h>
#include "d3d9gpu.h"

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201 4200) // anonymous unions warning
#if defined(_X86_) || defined(_IA64)
#pragma pack(4)
#endif

// D3DCOLOR is equivalent to D3DFMT_A8R8G8B8
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

// Canonical type for writing to GPU ring-buffers and command-buffers.
typedef DWORD* PRING;

// maps unsigned 8 bits/channel to D3DCOLOR
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)

#define D3DCOLOR_XYUV(y,u,v)   D3DCOLOR_ARGB(0xff,y,u,v)
#define D3DCOLOR_AYUV(a,y,u,v) D3DCOLOR_ARGB(a,y,u,v)

// maps floating point channels (0.f to 1.f range) to D3DCOLOR
#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))

// decodes a D3DCOLOR
#define D3DCOLOR_GETALPHA(argb)      (((argb) >> 24) & 0xff)
#define D3DCOLOR_GETRED(argb)        (((argb) >> 16) & 0xff)
#define D3DCOLOR_GETGREEN(argb)      (((argb) >> 8) & 0xff)
#define D3DCOLOR_GETBLUE(argb)       ((argb) & 0xff)

#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

#ifndef D3DCOLORVALUE_DEFINED
typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;
#define D3DCOLORVALUE_DEFINED
#endif

#ifndef D3DRECT_DEFINED
typedef struct _D3DRECT {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
} D3DRECT;
#define D3DRECT_DEFINED
#endif

#ifndef D3DPOINT_DEFINED
typedef struct _D3DPOINT {
    LONG x;
    LONG y;
} D3DPOINT;
#define D3DPOINT_DEFINED
#endif

#ifndef D3DMATRIX_DEFINED
typedef struct _D3DMATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
} D3DMATRIX;
#define D3DMATRIX_DEFINED
#endif

typedef struct _D3DVIEWPORT9 {
    DWORD       X;
    DWORD       Y;            /* Viewport Top left */
    DWORD       Width;
    DWORD       Height;       /* Viewport Dimensions */
    float       MinZ;         /* Min/max of clip Volume */
    float       MaxZ;
} D3DVIEWPORT9;

/*
 * Values for clip fields.
 */

// These bits could be ORed together to use with D3DRS_CLIPPLANEENABLE
//
#define D3DCLIPPLANE0 (1 << 0)
#define D3DCLIPPLANE1 (1 << 1)
#define D3DCLIPPLANE2 (1 << 2)
#define D3DCLIPPLANE3 (1 << 3)
#define D3DCLIPPLANE4 (1 << 4)
#define D3DCLIPPLANE5 (1 << 5)

// The following bits are used in the ClipUnion and ClipIntersection
// members of the D3DCLIPSTATUS9
//

#define D3DCS_LEFT        0x00000001L
#define D3DCS_RIGHT       0x00000002L
#define D3DCS_TOP         0x00000004L
#define D3DCS_BOTTOM      0x00000008L
#define D3DCS_FRONT       0x00000010L
#define D3DCS_BACK        0x00000020L
#define D3DCS_PLANE0      0x00000040L
#define D3DCS_PLANE1      0x00000080L
#define D3DCS_PLANE2      0x00000100L
#define D3DCS_PLANE3      0x00000200L
#define D3DCS_PLANE4      0x00000400L
#define D3DCS_PLANE5      0x00000800L

#define D3DCS_ALL (D3DCS_LEFT   | \
                   D3DCS_RIGHT  | \
                   D3DCS_TOP    | \
                   D3DCS_BOTTOM | \
                   D3DCS_FRONT  | \
                   D3DCS_BACK   | \
                   D3DCS_PLANE0 | \
                   D3DCS_PLANE1 | \
                   D3DCS_PLANE2 | \
                   D3DCS_PLANE3 | \
                   D3DCS_PLANE4 | \
                   D3DCS_PLANE5)

typedef struct _D3DCLIPSTATUS9 {
    DWORD ClipUnion;
    DWORD ClipIntersection;
} D3DCLIPSTATUS9;

/*
 * Options for Clear
 */
#define D3DCLEAR_TARGET0           0x00000001l  /* Clear target surface 0 */
#define D3DCLEAR_TARGET1           0x00000002l  /* Clear target surface 1 */
#define D3DCLEAR_TARGET2           0x00000004l  /* Clear target surface 2 */
#define D3DCLEAR_TARGET3           0x00000008l  /* Clear target surface 3 */
#define D3DCLEAR_ALLTARGETS        (D3DCLEAR_TARGET0 | D3DCLEAR_TARGET1 | D3DCLEAR_TARGET2 | D3DCLEAR_TARGET3) /* Clear all target surfaces */
#define D3DCLEAR_TARGET            D3DCLEAR_ALLTARGETS

#define D3DCLEAR_ZBUFFER           0x00000010l  /* Clear target z buffer */
#define D3DCLEAR_STENCIL           0x00000020l  /* Clear stencil planes */

/*
 * Options for Resolve
 */
#define D3DRESOLVE_RENDERTARGET0            0x0000  /* Resolve from render target 0 */
#define D3DRESOLVE_RENDERTARGET1            0x0001
#define D3DRESOLVE_RENDERTARGET2            0x0002
#define D3DRESOLVE_RENDERTARGET3            0x0003
#define D3DRESOLVE_DEPTHSTENCIL             0x0004  /* Resolve from depth/stencil buffer */

#define D3DRESOLVE_ALLFRAGMENTS             0x0000  /* Automatically resolve all multisample fragments */
#define D3DRESOLVE_FRAGMENT0                0x0010  /* Resolve only fragment 0 */
#define D3DRESOLVE_FRAGMENT1                0x0020
#define D3DRESOLVE_FRAGMENT2                0x0030
#define D3DRESOLVE_FRAGMENT3                0x0040
#define D3DRESOLVE_FRAGMENTS01              0x0050  /* Resolve fragments 0 and 1, with weight 0.5 */
#define D3DRESOLVE_FRAGMENTS23              0x0060  /* Resolve fragments 2 and 3, with weight 0.5 */
#define D3DRESOLVE_FRAGMENTS0123            0x0070  /* Resolve fragments 0, 1, 2 and 3, with weight 0.25 */

#define D3DRESOLVE_CLEARRENDERTARGET        0x0100  /* Clear the specified render target after resolving */
#define D3DRESOLVE_CLEARDEPTHSTENCIL        0x0200  /* Clear the depth/stencil buffer after resolving */

#define D3DRESOLVE_EXPONENTBIAS_SHIFT       26
#define D3DRESOLVE_EXPONENTBIAS(Bias)       (Bias << D3DRESOLVE_EXPONENTBIAS_SHIFT) /* Bias applied to float number before formatting (-32 to 31) */

/*
 * Options for BeginTiling
 */
#define D3DTILING_SKIP_FIRST_TILE_CLEAR             0x00000001
#define D3DTILING_CPU0                              0x01000000
#define D3DTILING_CPU1                              0x02000000
#define D3DTILING_CPU2                              0x04000000
#define D3DTILING_CPU3                              0x08000000
#define D3DTILING_CPU4                              0x10000000
#define D3DTILING_CPU5                              0x20000000

/*
 * Options for BeginZPass
 */
#define D3DZPASS_NO_AUTOMATIC_INVOKERENDERPASS      0x0001

/*
 * Options for BeginCommandBuffer
 */
#define D3DCOMMANDBUFFER_INHERIT_SAMPLERS_01        0x0001  /* Don't reset samplers 0 and 1 */

/*
 * Options for EndExport
 */
#define D3DENDEXPORT_NO_AUTOMATIC_SYNCHRONIZEEXPORT 0x0200

/*
 * Options for SetPredication
 */
#define D3DPRED_TILE_Z(Tile)                (0x1 << (2*Tile))
#define D3DPRED_TILE_RENDER(Tile)           (0x2 << (2*Tile))
#define D3DPRED_TILE(Tile)                  (D3DPRED_TILE_Z(Tile) | D3DPRED_TILE_RENDER(Tile))

#define D3DPRED_ALL_Z                       0x15555555
#define D3DPRED_ALL_RENDER                  0x2AAAAAAA

/*
 * Options for BeginConditionalSurvey
 */

#define D3DSURVEYBEGIN_CULLGEOMETRY         0x1

/*
 * Options for EndConditionalSurvey
 */

#define D3DSURVEYEND_SUSPEND                0x1

/*
 * The following defines the rendering states
 */
typedef enum _D3DFILLMODE {
    D3DFILL_POINT               = 0x01,
    D3DFILL_WIREFRAME           = 0x25,
    D3DFILL_SOLID               = 0x00,
    D3DFILL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
} D3DFILLMODE;

typedef enum _D3DBLEND {
    D3DBLEND_ZERO               = 0,
    D3DBLEND_ONE                = 1,
    D3DBLEND_SRCCOLOR           = 4,
    D3DBLEND_INVSRCCOLOR        = 5,
    D3DBLEND_SRCALPHA           = 6,
    D3DBLEND_INVSRCALPHA        = 7,
    D3DBLEND_DESTCOLOR          = 8,
    D3DBLEND_INVDESTCOLOR       = 9,
    D3DBLEND_DESTALPHA          = 10,
    D3DBLEND_INVDESTALPHA       = 11,
    D3DBLEND_BLENDFACTOR        = 12,
    D3DBLEND_INVBLENDFACTOR     = 13,
    D3DBLEND_CONSTANTALPHA      = 14,  // Xbox 360 extension
    D3DBLEND_INVCONSTANTALPHA   = 15,  // Xbox 360 extension
    D3DBLEND_SRCALPHASAT        = 16,

    // The following are not supported on Xbox 360:
    //
    // D3DBLEND_BOTHSRCALPHA
    // D3DBLEND_BOTHINVSRCALPHA

    D3DBLEND_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} D3DBLEND;

typedef enum _D3DBLENDOP {
    D3DBLENDOP_ADD              = 0,
    D3DBLENDOP_SUBTRACT         = 1,
    D3DBLENDOP_MIN              = 2,
    D3DBLENDOP_MAX              = 3,
    D3DBLENDOP_REVSUBTRACT      = 4,
    D3DBLENDOP_FORCE_DWORD      = 0x7fffffff, /* force 32-bit size enum */
} D3DBLENDOP;

typedef enum _D3DTEXTUREADDRESS {
    D3DTADDRESS_WRAP                    = 0,
    D3DTADDRESS_MIRROR                  = 1,
    D3DTADDRESS_CLAMP                   = 2,
    D3DTADDRESS_MIRRORONCE              = 3,
    D3DTADDRESS_BORDER_HALF             = 4,
    D3DTADDRESS_MIRRORONCE_BORDER_HALF  = 5,
    D3DTADDRESS_BORDER                  = 6,
    D3DTADDRESS_MIRRORONCE_BORDER       = 7,

    D3DTADDRESS_FORCE_DWORD             = 0x7fffffff, /* force 32-bit size enum */
} D3DTEXTUREADDRESS;

typedef enum _D3DCULL {
    D3DCULL_NONE                = 0x0,
    D3DCULL_CW                  = 0x2,
    D3DCULL_CCW                 = 0x6,
    D3DCULL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
} D3DCULL;

typedef enum _D3DCMPFUNC {
    D3DCMP_NEVER                = 0,
    D3DCMP_LESS                 = 1,
    D3DCMP_EQUAL                = 2,
    D3DCMP_LESSEQUAL            = 3,
    D3DCMP_GREATER              = 4,
    D3DCMP_NOTEQUAL             = 5,
    D3DCMP_GREATEREQUAL         = 6,
    D3DCMP_ALWAYS               = 7,
    D3DCMP_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} D3DCMPFUNC;

typedef enum _D3DSTENCILOP {
    D3DSTENCILOP_KEEP           = 0,
    D3DSTENCILOP_ZERO           = 1,
    D3DSTENCILOP_REPLACE        = 2,
    D3DSTENCILOP_INCRSAT        = 3,
    D3DSTENCILOP_DECRSAT        = 4,
    D3DSTENCILOP_INVERT         = 5,
    D3DSTENCILOP_INCR           = 6,
    D3DSTENCILOP_DECR           = 7,
    D3DSTENCILOP_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DSTENCILOP;

typedef enum _D3DZBUFFERTYPE {
    D3DZB_FALSE                 = 0,
    D3DZB_TRUE                  = 1, // Z buffering
    D3DZB_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */

    // D3DZB_USEW is not supported on Xbox 360

} D3DZBUFFERTYPE;

typedef enum _D3DTESSELLATIONMODE {
    D3DTM_DISCRETE              = 0,
    D3DTM_CONTINUOUS            = 1,
    D3DTM_PEREDGE               = 2,
    D3DTM_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} D3DTESSELLATIONMODE;

// Primitives supported by draw-primitive API
typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST             = 1,
    D3DPT_LINELIST              = 2,
    D3DPT_LINESTRIP             = 3,
    D3DPT_TRIANGLELIST          = 4,
    D3DPT_TRIANGLEFAN           = 5,
    D3DPT_TRIANGLESTRIP         = 6,
    D3DPT_RECTLIST              = 8,  // Xbox 360 extension
    D3DPT_QUADLIST              = 13, // Xbox 360 extension

    D3DPT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
} D3DPRIMITIVETYPE;

// Primitives supported by DrawTessellatedPrimitive and DrawIndexedTessellatedPrimitive.
typedef enum _D3DTESSPRIMITIVETYPE {
    D3DTPT_LINELIST             = 2,
    D3DTPT_LINESTRIP            = 3,
    D3DTPT_TRIANGLELIST         = 4,
    D3DTPT_TRIANGLEFAN          = 5,
    D3DTPT_TRIANGLESTRIP        = 6,
    D3DTPT_QUADLIST             = 13,
    D3DTPT_LINEPATCH            = 16,
    D3DTPT_TRIPATCH             = 17,
    D3DTPT_QUADPATCH            = 18,

    D3DTPT_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} D3DTESSPRIMITIVETYPE;

typedef struct _D3DBLENDSTATE {
    DWORD SrcBlend                  : 5;     /* D3DBLEND */
    DWORD BlendOp                   : 3;     /* D3DBLENDOP */
    DWORD DestBlend                 : 8;     /* D3DBLEND */
    DWORD SrcBlendAlpha             : 5;     /* D3DBLEND */
    DWORD BlendOpAlpha              : 3;     /* D3DBLENDOP */
    DWORD DestBlendAlpha            : 8;     /* D3DBLEND */
} D3DBLENDSTATE;

// Values for D3DRS_HISTENCILFUNC
typedef enum _D3DHISTENCILCMPFUNC {
    D3DHSCMP_EQUAL              = 0,
    D3DHSCMP_NOTEQUAL           = 1,

    D3DHSCMP_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} D3DHISTENCILCMPFUNC;

typedef enum _D3DRENDERSTATETYPE {

    // The following are ignored on Xbox 360:

    D3DRS_PATCHEDGESTYLE             = 0,
    D3DRS_POSITIONDEGREE             = 4,
    D3DRS_NORMALDEGREE               = 8,
    D3DRS_ANTIALIASEDLINEENABLE      = 12,
    D3DRS_ADAPTIVETESS_X             = 16,
    D3DRS_ADAPTIVETESS_Y             = 20,
    D3DRS_ADAPTIVETESS_Z             = 24,
    D3DRS_ADAPTIVETESS_W             = 28,
    D3DRS_ENABLEADAPTIVETESSELLATION = 32,
    D3DRS_DITHERENABLE               = 36,

    // The following are standard render states:

    D3DRS_ZENABLE                    = 40,   /* TRUE to enable Z buffering */
    D3DRS_ZFUNC                      = 44,   /* D3DCMPFUNC */
    D3DRS_ZWRITEENABLE               = 48,   /* TRUE to enable z writes */
    D3DRS_FILLMODE                   = 52,   /* D3DFILLMODE */
    D3DRS_CULLMODE                   = 56,   /* D3DCULL */
    D3DRS_ALPHABLENDENABLE           = 60,   /* TRUE to enable alpha blending */
    D3DRS_SEPARATEALPHABLENDENABLE   = 64,   /* TRUE to enable a separate blending function for the alpha channel */
    D3DRS_BLENDFACTOR                = 68,   /* D3DCOLOR used for a constant blend factor during alpha blending for D3DBLEND_BLENDFACTOR, etc. */
    D3DRS_SRCBLEND                   = 72,   /* D3DBLEND */
    D3DRS_DESTBLEND                  = 76,   /* D3DBLEND */
    D3DRS_BLENDOP                    = 80,   /* D3DBLENDOP setting */
    D3DRS_SRCBLENDALPHA              = 84,   /* SRC blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */
    D3DRS_DESTBLENDALPHA             = 88,   /* DST blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */
    D3DRS_BLENDOPALPHA               = 92,   /* Blending operation for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE */
    D3DRS_ALPHATESTENABLE            = 96,   /* TRUE to enable alpha tests */
    D3DRS_ALPHAREF                   = 100,  /* BYTE */
    D3DRS_ALPHAFUNC                  = 104,  /* D3DCMPFUNC */
    D3DRS_STENCILENABLE              = 108,  /* TRUE to enable stenciling */
    D3DRS_TWOSIDEDSTENCILMODE        = 112,  /* TRUE to enable 2 sided stenciling */
    D3DRS_STENCILFAIL                = 116,  /* D3DSTENCILOP to do if stencil test fails */
    D3DRS_STENCILZFAIL               = 120,  /* D3DSTENCILOP to do if stencil test passes and Z test fails */
    D3DRS_STENCILPASS                = 124,  /* D3DSTENCILOP to do if both stencil and Z tests pass */
    D3DRS_STENCILFUNC                = 128,  /* D3DCMPFUNC stencilfn - stencil test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
    D3DRS_STENCILREF                 = 132,  /* BYTE reference value used in stencil test */
    D3DRS_STENCILMASK                = 136,  /* BYTE mask value used in stencil test */
    D3DRS_STENCILWRITEMASK           = 140,  /* BYTE write mask applied to values written to stencil buffer */
    D3DRS_CCW_STENCILFAIL            = 144,  /* D3DSTENCILOP to do if CCW stencil test fails */
    D3DRS_CCW_STENCILZFAIL           = 148,  /* D3DSTENCILOP to do if CCW stencil test passes and Z test fails */
    D3DRS_CCW_STENCILPASS            = 152,  /* D3DSTENCILOP to do if both CCW stencil and Z tests pass */
    D3DRS_CCW_STENCILFUNC            = 156,  /* D3DCMPFUNC stencilfn - CCW stencil test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
    D3DRS_CCW_STENCILREF             = 160,  /* BYTE reference value used in CCW stencil test */
    D3DRS_CCW_STENCILMASK            = 164,  /* BYTE mask value used in CCW stencil test */
    D3DRS_CCW_STENCILWRITEMASK       = 168,  /* BYTE write mask applied to CCW values written to stencil buffer */
    D3DRS_CLIPPLANEENABLE            = 172,  /* TRUE to enable SetClipPlane */
    D3DRS_POINTSIZE                  = 176,  /* FLOAT point size */
    D3DRS_POINTSIZE_MIN              = 180,  /* FLOAT point size min threshold */
    D3DRS_POINTSPRITEENABLE          = 184,  /* TRUE to enable point sprites */
    D3DRS_POINTSIZE_MAX              = 188,  /* FLOAT point size max threshold */
    D3DRS_MULTISAMPLEANTIALIAS       = 192,  /* TRUE to enable multisample antialiasing */
    D3DRS_MULTISAMPLEMASK            = 196,  /* DWORD per-pixel and per-sample enable/disable */
    D3DRS_SCISSORTESTENABLE          = 200,  /* TRUE to enable SetScissorRect */
    D3DRS_SLOPESCALEDEPTHBIAS        = 204,  /* FLOAT depth-slope scaling bias */
    D3DRS_DEPTHBIAS                  = 208,  /* FLOAT depth bias */
    D3DRS_COLORWRITEENABLE           = 212,  /* D3DCOLORWRITEENABLE_ALPHA, etc. per-channel write enable */
    D3DRS_COLORWRITEENABLE1          = 216,
    D3DRS_COLORWRITEENABLE2          = 220,
    D3DRS_COLORWRITEENABLE3          = 224,
    D3DRS_SRGBWRITEENABLE            = 228,  /* Enable rendertarget writes to be DE-linearized to SRGB  */
    D3DRS_TESSELLATIONMODE           = 232,  /* D3DTESSELLATIONMODE */
    D3DRS_MINTESSELLATIONLEVEL       = 236,  /* FLOAT */
    D3DRS_MAXTESSELLATIONLEVEL       = 240,  /* FLOAT */
    D3DRS_WRAP0                      = 244,  /* D3DWRAPCOORD_0, etc. for 1st texture coord. set */
    D3DRS_WRAP1                      = 248,
    D3DRS_WRAP2                      = 252,
    D3DRS_WRAP3                      = 256,
    D3DRS_WRAP4                      = 260,
    D3DRS_WRAP5                      = 264,
    D3DRS_WRAP6                      = 268,
    D3DRS_WRAP7                      = 272,
    D3DRS_WRAP8                      = 276,
    D3DRS_WRAP9                      = 280,
    D3DRS_WRAP10                     = 284,
    D3DRS_WRAP11                     = 288,
    D3DRS_WRAP12                     = 292,
    D3DRS_WRAP13                     = 296,
    D3DRS_WRAP14                     = 300,
    D3DRS_WRAP15                     = 304,

    // The following are Xbox 360 extensions:

    D3DRS_VIEWPORTENABLE             = 308,  /* TRUE to enable viewport transformation */
    D3DRS_HIGHPRECISIONBLENDENABLE   = 312,  /* TRUE to enable higher precision blending operations for 2_10_10_10 and 2_10_10_10_FLOAT */
    D3DRS_HIGHPRECISIONBLENDENABLE1  = 316,  /*   render targets at the expense of running at half rate.  2_10_10_10 surfaces are expanded */
    D3DRS_HIGHPRECISIONBLENDENABLE2  = 320,  /*   out to 10:10:10:10 before the blend and 2_10_10_10_FLOAT surfaces are expanded out to */
    D3DRS_HIGHPRECISIONBLENDENABLE3  = 324,  /*   16:16:16:16 before the blend.  The default value is FALSE. */
    D3DRS_HALFPIXELOFFSET            = 328,  /* TRUE to enable (0.5, 0.5) screen-space offset */
    D3DRS_PRIMITIVERESETENABLE       = 332,  /* TRUE to enable primitive resets in indexed drawing.  The default value is FALSE. */
    D3DRS_PRIMITIVERESETINDEX        = 336,  /* WORD Index reference value to trigger a primitive reset.  The default value is 0xFFFF. */
    D3DRS_ALPHATOMASKENABLE          = 340,  /* TRUE to enable alpha to mask.  The default value is FALSE. */
    D3DRS_ALPHATOMASKOFFSETS         = 344,  /* BYTE Packed offsets (2:2:2:2) to apply to the alpha value for each pixel in quad before it is converted to a mask. */
    D3DRS_GUARDBAND_X                = 348,  /* FLOAT horizontal guard band factor */
    D3DRS_GUARDBAND_Y                = 352,  /* FLOAT vertical guard band factor */
    D3DRS_DISCARDBAND_X              = 356,  /* FLOAT horizontal discard band factor */
    D3DRS_DISCARDBAND_Y              = 360,  /* FLOAT vertical discard band factor */
    D3DRS_HISTENCILENABLE            = 364,  /* TRUE to enable early culling based on hi-stencil bit */
    D3DRS_HISTENCILWRITEENABLE       = 368,  /* TRUE to enable update of hi-stencil bit based on hi-stencil test */
    D3DRS_HISTENCILFUNC              = 372,  /* D3DHISTENCILCMPFUNC - bit is set to cull if (ref histencilfn stencil) is true */
    D3DRS_HISTENCILREF               = 376,  /* BYTE reference value used in hi-stencil test */
    D3DRS_PRESENTINTERVAL            = 380,  /* D3DPRESENT_INTERVAL_ONE, etc. */
    D3DRS_PRESENTIMMEDIATETHRESHOLD  = 384,  /* BYTE percentage of DAC's progress in frame where a non-D3DPRESENT_INTERVAL_IMMEDIATE Present/Swap will be considered for immediate Present/Swap */

    D3DRS_MAX                        = 388,

    // The following render states are not supported on Xbox 360:
    //
    //  D3DRS_SHADEMODE
    //  D3DRS_LASTPIXEL
    //  D3DRS_CLIPPING

    D3DRS_FORCE_DWORD                = 0x7fffffff, /* force 32-bit size enum */
} D3DRENDERSTATETYPE;

// Maximum number of simultaneous render targets D3D supports
#define D3D_MAX_SIMULTANEOUS_RENDERTARGETS 4

// Bias to apply to the texture coordinate set to apply a wrap to.
#define D3DRENDERSTATE_WRAPBIAS                 128UL

/* Flags to construct the WRAP render states */
#define D3DWRAP_U   0x00000001L
#define D3DWRAP_V   0x00000002L
#define D3DWRAP_W   0x00000004L

/* Flags to construct the WRAP render states for 1D thru 4D texture coordinates */
#define D3DWRAPCOORD_0   0x00000001L    // same as D3DWRAP_U
#define D3DWRAPCOORD_1   0x00000002L    // same as D3DWRAP_V
#define D3DWRAPCOORD_2   0x00000004L    // same as D3DWRAP_W
#define D3DWRAPCOORD_3   0x00000008L

/* Flags to construct D3DRS_COLORWRITEENABLE */
#define D3DCOLORWRITEENABLE_RED     (1L<<0)
#define D3DCOLORWRITEENABLE_GREEN   (1L<<1)
#define D3DCOLORWRITEENABLE_BLUE    (1L<<2)
#define D3DCOLORWRITEENABLE_ALPHA   (1L<<3)
#define D3DCOLORWRITEENABLE_ALL     0xf     // Xbox 360 extension

/* Predefined values for D3DRS_ALPHATOMASKOFFSETS */
#define D3DALPHATOMASK_DITHERED   0x00000087
#define D3DALPHATOMASK_SOLID      0x000000AA

/*
 * State enumerants for per-sampler texture processing.
 */
typedef enum _D3DSAMPLERSTATETYPE
{
    D3DSAMP_ADDRESSU                = 0,  /* D3DTEXTUREADDRESS for U coordinate */
    D3DSAMP_ADDRESSV                = 4,  /* D3DTEXTUREADDRESS for V coordinate */
    D3DSAMP_ADDRESSW                = 8,  /* D3DTEXTUREADDRESS for W coordinate */
    D3DSAMP_BORDERCOLOR             = 12, /* D3DCOLOR */
    D3DSAMP_MAGFILTER               = 16, /* D3DTEXTUREFILTERTYPE filter to use for magnification */
    D3DSAMP_MINFILTER               = 20, /* D3DTEXTUREFILTERTYPE filter to use for minification */
    D3DSAMP_MIPFILTER               = 24, /* D3DTEXTUREFILTERTYPE filter to use between mipmaps during minification */
    D3DSAMP_MIPMAPLODBIAS           = 28, /* Float Mipmap LOD bias */
    D3DSAMP_MAXMIPLEVEL             = 32, /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
    D3DSAMP_MAXANISOTROPY           = 36, /* DWORD maximum anisotropy */

    // The following are Xbox 360 extensions:

    D3DSAMP_MAGFILTERZ              = 40, /* D3DTEXTUREFILTERTYPE filter to use for volume/array slice magnification */
    D3DSAMP_MINFILTERZ              = 44, /* D3DTEXTUREFILTERTYPE filter to use for volume/array slice minification */
    D3DSAMP_SEPARATEZFILTERENABLE   = 48, /* TRUE to use MIN/MAGFILTERZ for slice filtering. FALSE to use MIN/MAGFILTER */
    D3DSAMP_MINMIPLEVEL             = 52, /* DWORD 0..(n-1) LOD index of smallest map to use (0 == highest resolution map) */
    D3DSAMP_TRILINEARTHRESHOLD      = 56, /* D3DTRILINEARTHRESHOLD trilinear filtering range threshold */
    D3DSAMP_ANISOTROPYBIAS          = 60, /* Float bias to add to anisotropy ratio ranging from -1.875 to 0.0 */
    D3DSAMP_HGRADIENTEXPBIAS        = 64, /* Signed integer bias to add to the horizontal LOD gradient ranging from -16 to 15 */
    D3DSAMP_VGRADIENTEXPBIAS        = 68, /* Signed integer bias to add to the vertical LOD gradient ranging from -16 to 15 */
    D3DSAMP_WHITEBORDERCOLORW       = 72, /* TRUE to override the w component of the border color with white */
    D3DSAMP_POINTBORDERENABLE       = 76, /* FALSE to disable border addressing when using point filtering */

    D3DSAMP_MAX                     = 80,

    // The following sampler states are not supported by Xbox 360:
    //
    //  D3DSAMP_ELEMENTINDEX
    //  D3DSAMP_DMAPOFFSET

    D3DSAMP_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */

} D3DSAMPLERSTATETYPE;

/* Special sampler which is used in the tesselator */
#define D3DDMAPSAMPLER (16)

// Samplers used in vertex shaders
#define D3DVERTEXTEXTURESAMPLER0 ((DWORD) (D3DDMAPSAMPLER+0))
#define D3DVERTEXTEXTURESAMPLER1 ((DWORD) (D3DDMAPSAMPLER+1))
#define D3DVERTEXTEXTURESAMPLER2 ((DWORD) (D3DDMAPSAMPLER+2))
#define D3DVERTEXTEXTURESAMPLER3 ((DWORD) (D3DDMAPSAMPLER+3))

//
// Values for D3DSAMP_***FILTER texture stage states
//
typedef enum _D3DTEXTUREFILTERTYPE
{
    D3DTEXF_NONE            = 2,    // filtering disabled (valid for mip filter only)
    D3DTEXF_POINT           = 0,    // nearest
    D3DTEXF_LINEAR          = 1,    // linear interpolation
    D3DTEXF_ANISOTROPIC     = 4,    // anisotropic

    D3DTEXF_FORCE_DWORD     = 0x7fffffff,   // force 32-bit size enum

    // The following are not supported on Xbox 360:
    //
    // D3DTEXF_PYRAMIDALQUAD
    // D3DTEXF_GAUSSIANQUAD

} D3DTEXTUREFILTERTYPE;

//
// Values for D3DSAMP_TRILINEARTHRESHOLD sampler state
//

typedef enum _D3DTRILINEARTHRESHOLD
{
    D3DTRILINEAR_IMMEDIATE      = 0,
    D3DTRILINEAR_ONESIXTH       = 1,
    D3DTRILINEAR_ONEFOURTH      = 2,
    D3DTRILINEAR_THREEEIGHTHS   = 3,

    D3DTRILINEAR_FORCE_DWORD    = 0x7fffffff,   // force 32-bit size enum

} D3DTRILINEARTHRESHOLD;

/* Bits for Flags in ProcessVertices call */

#define D3DPV_DONOTCOPYDATA     (1 << 0)

//-------------------------------------------------------------------
// Flexible vertex format bits
//
#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x400E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002

#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080

#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800

#define D3DFVF_LASTBETA_UBYTE4   0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000

#define D3DFVF_RESERVED2         0x6000  // 2 reserved bits

//---------------------------------------------------------------------
// Vertex Shaders
//

// Vertex shader declaration

// Vertex element semantics
//
typedef enum _D3DDECLUSAGE
{
    D3DDECLUSAGE_POSITION = 0,
    D3DDECLUSAGE_BLENDWEIGHT,   // 1
    D3DDECLUSAGE_BLENDINDICES,  // 2
    D3DDECLUSAGE_NORMAL,        // 3
    D3DDECLUSAGE_PSIZE,         // 4
    D3DDECLUSAGE_TEXCOORD,      // 5
    D3DDECLUSAGE_TANGENT,       // 6
    D3DDECLUSAGE_BINORMAL,      // 7
    D3DDECLUSAGE_TESSFACTOR,    // 8
    D3DDECLUSAGE_COLOR=10,      // 10
    D3DDECLUSAGE_FOG,           // 11
    D3DDECLUSAGE_DEPTH,         // 12
    D3DDECLUSAGE_SAMPLE,        // 13
} D3DDECLUSAGE;

#define MAXD3DDECLUSAGE         D3DDECLUSAGE_SAMPLE
#define MAXD3DDECLUSAGEINDEX    15
#define MAXD3DDECLLENGTH        64 // does not include "end" marker vertex element

typedef enum _D3DDECLMETHOD
{
    D3DDECLMETHOD_DEFAULT = 0,
    D3DDECLMETHOD_PARTIALU,
    D3DDECLMETHOD_PARTIALV,
    D3DDECLMETHOD_CROSSUV,              // Normal
    D3DDECLMETHOD_UV,
    D3DDECLMETHOD_LOOKUP,               // Lookup a displacement map
    D3DDECLMETHOD_LOOKUPPRESAMPLED,     // Lookup a pre-sampled displacement map
} D3DDECLMETHOD;

#define MAXD3DDECLMETHOD D3DDECLMETHOD_LOOKUPPRESAMPLED

// A D3DDECLTYPE constant is an identifier that uniquely identifies
// a vertex format.  It contains the following fields:

#define D3DDECLTYPE_VERTEXFORMAT_SHIFT      0       // GPUVERTEXFORMAT
#define D3DDECLTYPE_ENDIAN_SHIFT            6       // GPUENDIAN
#define D3DDECLTYPE_VERTEXSIGN_SHIFT        8       // GPUVERTEXSIGN
#define D3DDECLTYPE_NUMFORMAT_SHIFT         9       // GPUNUMFORMAT
#define D3DDECLTYPE_SWIZZLEX_SHIFT          10      // GPUSWIZZLE
#define D3DDECLTYPE_SWIZZLEY_SHIFT          13      // GPUSWIZZLE
#define D3DDECLTYPE_SWIZZLEZ_SHIFT          16      // GPUSWIZZLE
#define D3DDECLTYPE_SWIZZLEW_SHIFT          19      // GPUSWIZZLE

#define D3DDECLTYPE_VERTEXFORMAT_MASK       0x0000003f
#define D3DDECLTYPE_ENDIAN_MASK             0x000000c0
#define D3DDECLTYPE_VERTEXSIGN_MASK         0x00000100
#define D3DDECLTYPE_NUMFORMAT_MASK          0x00000200
#define D3DDECLTYPE_SWIZZLEX_MASK           0x00001c00
#define D3DDECLTYPE_SWIZZLEY_MASK           0x0000e000
#define D3DDECLTYPE_SWIZZLEZ_MASK           0x00070000
#define D3DDECLTYPE_SWIZZLEW_MASK           0x00380000

#define MAKED3DDECLTYPE(VertexFormat, Endian, VertexSign, NumFormat, Swizzle) \
            ((VertexFormat) << D3DDECLTYPE_VERTEXFORMAT_SHIFT | \
             (Endian) << D3DDECLTYPE_ENDIAN_SHIFT | \
             (VertexSign) << D3DDECLTYPE_VERTEXSIGN_SHIFT | \
             (NumFormat) << D3DDECLTYPE_NUMFORMAT_SHIFT | \
             (Swizzle) << D3DDECLTYPE_SWIZZLEX_SHIFT)

#define MAKED3DDECLTYPE2(VertexFormat, Endian, VertexSign, NumFormat, SwizzleX, SwizzleY, SwizzleZ, SwizzleW) \
            ((VertexFormat) << D3DDECLTYPE_VERTEXFORMAT_SHIFT | \
             (Endian) << D3DDECLTYPE_ENDIAN_SHIFT | \
             (VertexSign) << D3DDECLTYPE_VERTEXSIGN_SHIFT | \
             (NumFormat) << D3DDECLTYPE_NUMFORMAT_SHIFT | \
             (SwizzleX) << D3DDECLTYPE_SWIZZLEX_SHIFT | \
             (SwizzleY) << D3DDECLTYPE_SWIZZLEY_SHIFT | \
             (SwizzleZ) << D3DDECLTYPE_SWIZZLEZ_SHIFT | \
             (SwizzleW) << D3DDECLTYPE_SWIZZLEW_SHIFT)

typedef enum _D3DDECLTYPE
{
    // 1D float expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_FLOAT1    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_FLOAT, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_0, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 2D float expanded to (value, value, 0., 1.)
    D3DDECLTYPE_FLOAT2    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32_FLOAT, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 3D float expanded to (value, value, value, 1.)
    D3DDECLTYPE_FLOAT3    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32_32_FLOAT, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 4D float
    D3DDECLTYPE_FLOAT4    = MAKED3DDECLTYPE(GPUVERTEXFORMAT_32_32_32_32_FLOAT, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 1D signed int expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_INT1      = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_0, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 2D signed int expanded to (value, value, 0., 1.)
    D3DDECLTYPE_INT2      = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D signed int
    D3DDECLTYPE_INT4      = MAKED3DDECLTYPE(GPUVERTEXFORMAT_32_32_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 1D unsigned int expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_UINT1     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_0, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 2D unsigned int expanded to (value, value, 0., 1.)
    D3DDECLTYPE_UINT2     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D unsigned int
    D3DDECLTYPE_UINT4     = MAKED3DDECLTYPE(GPUVERTEXFORMAT_32_32_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 1D signed int normalized
    D3DDECLTYPE_INT1N     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_0, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 2D signed int normalized
    D3DDECLTYPE_INT2N     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D signed int normalized
    D3DDECLTYPE_INT4N     = MAKED3DDECLTYPE(GPUVERTEXFORMAT_32_32_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // 1D unsigned int normalized
    D3DDECLTYPE_UINT1N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_0, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 2D unsigned int normalized
    D3DDECLTYPE_UINT2N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D unsigned int normalized
    D3DDECLTYPE_UINT4N    = MAKED3DDECLTYPE(GPUVERTEXFORMAT_32_32_32_32, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // 4D packed unsigned bytes mapped to 0. to 1. range
    // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    D3DDECLTYPE_D3DCOLOR  = MAKED3DDECLTYPE(GPUVERTEXFORMAT_8_8_8_8, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),

    // 4D unsigned byte
    D3DDECLTYPE_UBYTE4    = MAKED3DDECLTYPE(GPUVERTEXFORMAT_8_8_8_8, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 4D signed byte
    D3DDECLTYPE_BYTE4     = MAKED3DDECLTYPE(GPUVERTEXFORMAT_8_8_8_8, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // Each of 4 bytes is normalized by dividing to 255.0
    D3DDECLTYPE_UBYTE4N   = MAKED3DDECLTYPE(GPUVERTEXFORMAT_8_8_8_8, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // Each of 4 bytes is normalized by dividing to 127.0
    D3DDECLTYPE_BYTE4N    = MAKED3DDECLTYPE(GPUVERTEXFORMAT_8_8_8_8, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // 2D signed short expanded to (value, value, 0., 1.)
    D3DDECLTYPE_SHORT2    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D signed short
    D3DDECLTYPE_SHORT4    = MAKED3DDECLTYPE(GPUVERTEXFORMAT_16_16_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 2D unsigned short expanded to (value, value, 0., 1.)
    D3DDECLTYPE_USHORT2   = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D unsigned short
    D3DDECLTYPE_USHORT4   = MAKED3DDECLTYPE(GPUVERTEXFORMAT_16_16_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    D3DDECLTYPE_SHORT2N   = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    D3DDECLTYPE_SHORT4N   = MAKED3DDECLTYPE(GPUVERTEXFORMAT_16_16_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    D3DDECLTYPE_USHORT2N  = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    D3DDECLTYPE_USHORT4N  = MAKED3DDECLTYPE(GPUVERTEXFORMAT_16_16_16_16, GPUENDIAN_8IN16, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_UDEC3     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 10 10 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_DEC3      = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D unsigned 10 10 10 format normalized and expanded to (v[0]/1023.0, v[1]/1023.0, v[2]/1023.0, 1)
    D3DDECLTYPE_UDEC3N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    D3DDECLTYPE_DEC3N     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 4D unsigned 10 10 10 2 format expanded to (value, value, value, value)
    D3DDECLTYPE_UDEC4     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W),

    // 4D signed 10 10 10 2 format expanded to (value, value, value, value)
    D3DDECLTYPE_DEC4      = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W),

    // 4D unsigned 10 10 10 2 format normalized and expanded to (v[0]/1023.0, v[1]/1023.0, v[2]/1023.0, v[3]/3.0)
    D3DDECLTYPE_UDEC4N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W),

    // 4D signed 10 10 10 2 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, v[3]/1.0)
    D3DDECLTYPE_DEC4N     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_2_10_10_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W),

    // 3D unsigned 11 11 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_UHEND3    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_10_11_11, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 11 11 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_HEND3     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_10_11_11, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D unsigned 11 11 10 format normalized and expanded to (v[0]/2047.0, v[1]/2047.0, v[2]/1023.0, 1)
    D3DDECLTYPE_UHEND3N   = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_10_11_11, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 11 11 10 format normalized and expanded to (v[0]/1023.0, v[1]/1023.0, v[2]/511.0, 1)
    D3DDECLTYPE_HEND3N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_10_11_11, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D unsigned 10 11 11 format expanded to (value, value, value, 1)
    D3DDECLTYPE_UDHEN3    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_11_11_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 10 11 11 format expanded to (value, value, value, 1)
    D3DDECLTYPE_DHEN3     = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_11_11_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D unsigned 10 11 11 format normalized and expanded to (v[0]/1023.0, v[1]/2047.0, v[2]/2047.0, 1)
    D3DDECLTYPE_UDHEN3N   = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_11_11_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // 3D signed 10 11 11 format normalized and expanded to (v[0]/511.0, v[1]/1023.0, v[2]/1023.0, 1)
    D3DDECLTYPE_DHEN3N    = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_11_11_10, GPUENDIAN_8IN32, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),

    // Two 16-bit floating point values, expanded to (value, value, 0, 1)
    D3DDECLTYPE_FLOAT16_2 = MAKED3DDECLTYPE2(GPUVERTEXFORMAT_16_16_FLOAT, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_0, GPUSWIZZLE_1),

    // Four 16-bit floating point values
    D3DDECLTYPE_FLOAT16_4 = MAKED3DDECLTYPE(GPUVERTEXFORMAT_16_16_16_16_FLOAT, GPUENDIAN_8IN16, GPUVERTEXSIGN_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),

    // When the type field in a decl is unused.
    D3DDECLTYPE_UNUSED    = 0xffffffff,

} D3DDECLTYPE;

#define MAXD3DDECLTYPE      D3DDECLTYPE_UNUSED

typedef struct _D3DVERTEXELEMENT9
{
    WORD    Stream;     // Stream index
    WORD    Offset;     // Offset in the stream in bytes
    DWORD   Type;       // Data type, e.g. D3DDECLTYPE_FLOAT3
    BYTE    Method;     // Processing method, e.g. D3DDECLMETHOD_DEFAULT
    BYTE    Usage;      // Semantics, e.g. D3DDECLUSAGE_POSITION
    BYTE    UsageIndex; // Semantic index
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;

// This is used to initialize the last vertex element in a vertex declaration
// array
//
#define D3DDECL_END() {0xFF,0,(DWORD)D3DDECLTYPE_UNUSED,0,0,0}

// Maximum supported number of texture coordinate sets
#define D3DDP_MAXTEXCOORD   8

//---------------------------------------------------------------------
//
// Pixel Shader (PS) & Vertex Shader (VS) Instruction Token Definition
//
//---------------------------------------------------------------------
// Version token format
//
// [07:00] minor version number
// [15:08] major version number
// [31:16]
//         PS 0xFFFF
//         VS 0xFFFE
//
//---------------------------------------------------------------------
// OpCode token format
//
// [15:00] Opcode (D3DSIO_*)
// [23:16] Opcode-Specific Controls
// [27:24] Instruction length in DWORDs excluding the opcode token
//         (version >= 2_0)
// [28]    ps/vs <  2_x: Reserved 0x0
//         ps/vs >= 2_x: 1 means instruction is predicated
//                       (extra predicate source token at end)
// [29]    Reserved 0x0
// [30]    ps_1_x: Co-issue flag (this inst and prev are parallel)
//         ps > 1_x and all vs: Reserved 0x0
// [31]    0x0
//
//---------------------------------------------------------------------
// End Token format
//
// [31:00] 0x0000FFFF
//
//---------------------------------------------------------------------
// Comment Token format
//
// [15:00] 0xFFFE
// [30:16] DWORD Length (up to 2^15 DWORDS = 128KB)
// [31] 0x0
//
//---------------------------------------------------------------------
// Destination token format
//
// [10:00] Register Number (offset in register file)
// [11-12] Register type bits [3-4]
// [13]    VS (version 3_x) Relative Addressing mode
//         VS others Reserved 0x0
//         PS Reserved 0x0
// [15:14] Reserved 0x0
// [19:16] Write Mask
// [16]    Component 0 (X;Red)
// [17]    Component 1 (Y;Green)
// [18]    Component 2 (Z;Blue)
// [19]    Component 3 (W;Alpha)
// [23:20]
//         Result Modifier
// [27:24]
//         PS (version < 2_0) Result Shift Scale (signed )
//         VS Reserved 0x0
// [30:28] Register type bits [0-2]
// [31]    0x1
//
//---------------------------------------------------------------------
// Source token format
//
// [10:00] Register Number (offset in register file)
// [11-12] Register type bits [3-4]
// [13]
//         VS Relative Addressing mode
//         PS (version < 3_0) Reserved 0x0
//         PS (version 3_0) Relative Addressing Mode
// [14:15]
//         VS Reserved 0x0
//         PS Reserved 0x0
// [23:16] Source Component Swizzle
// [17:16] Component 0 Swizzle
// [19:18] Component 1 Swizzle
// [21:20] Component 2 Swizzle
// [23:22] Component 3 Swizzle
// [27:24] Source Modifier
// [30:28] Register type bits [0-2]
// [31]    0x1
//
// Swizzle defines which input component should be used as output
// component:
//  0   - component X is used
//  1   - component y is used
//  2   - component Z is used
//  3   - component W is used
//
//---------------------------------------------------------------------
// Vertex shader version >= 2_0 only:
//
// When bit [13] (relative addressing) is set in a src or dest token,
// (where supported in the version) an additional DWORD token follows.
// The token has the same format as the source operand token, with the
// following rules:
//       a. only D3DSPR_ADDR or D3DSPR_LOOP could be used as register
//          types.
//       b. swizzle bits are used to determine a register component
//       c. bit[31] is 1
//       d. register offset is used
//       e. all other bits are not used
//
//---------------------------------------------------------------------
// Label token format
//
// (only certain instructions use this token -> for example callnz)
//
// [10:00] Register Number (offset in register file)
// [11:12] Register type bits [3-4]
// [27:13] Reserved 0x0
// [30:28] Register type bits [0-2]
// [31]    0x1
//
// Note:
// 1. The format is the same as the source token format,except that only
//    register type and offset are used.
// 2. The register type must be D3DSPR_LABEL
//
//---------------------------------------------------------------------
// DEF instruction format (constant definition)
//
//                      Token 1:    D3DSIO_DEF (with instruction length = 5)
//                      Token 2:    Destination parameter token: c# or i#
//                      Tokens 3-6: Four 32 bit values. For c# registers,
//                                  these are 32 bit floats, and for i#
//                                  registers, these are 32 bit signed
//                                  integers.
//
//---------------------------------------------------------------------
// DCL instruction format
//
// ps_2_0+:
// --------
// s# dcl Format:       Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-26]  = Reserved 0x0
//                               [27-30] = D3DSAMPLER_TEXTURE_TYPE (1D, 2D,
//                                         cube, etc.)
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating sampler
//                               "register" (s#).
//                               Register type == D3DSPR_SAMPLER, and register
//                               # are only fields used.
//
// v# or t# dcl Format: Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-30]  = Reserved 0x0
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating v# or
//                               t# register, and writemask field indicating
//                               declared components.
//
// vs_2_0+:
// --------
// v# dcl Format:       Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-4]   = D3DDECLUSAGE (i.e.
//                                         D3DDECLUSAGE_TEXCOORD, _NORMAL etc)
//                               [5-15]  = Reserved 0x0
//                               [16-19] = Usage Index
//                               [20-30] = Reserved 0x0
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating v#
//                               register, and writemask field indicating
//                               declared components.
//
// ps_3_0+:
// --------
// t# dcl Format:       Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-4]   = D3DDECLUSAGE (MUST be
//                                         D3DDECLUSAGE_TEXCOORD
//                                         or D3DDECLUSAGE_COLOR)
//                               [5-15]  = Reserved 0x0
//                               [16-19] = Usage Index (for _TEXCOORD must be
//                                         0-7, else must be 0)
//                               [20-30] = Reserved 0x0
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating t#
//                               register, and writemask field indicating
//                               declared components.
//
// vFace dcl Format:    Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-30]  = Reserved 0x0
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating bf
//                               (vFace) register.
//                               Writemask field must be full (although it is
//                               unused), and result mod/ scale fields all
//                               must be 0 (also unused).
//
// pos dcl Format       Token 1: D3DSIO_DCL (with instruction length = 2)
//                      Token 2: [0-30]  = Reserved 0x0
//                               [31]    = 1
//                      Token 3: Destination parameter token indicating pos
//                               register, and writemask field indicating
//                               declared components.
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//
// The internal format of Pixel Shader (PS) & Vertex Shader (VS)
// Instruction Tokens is defined in the Direct3D Device Driver Kit
//
//---------------------------------------------------------------------

//
// Instruction Token Bit Definitions
//
#define D3DSI_OPCODE_MASK       0x0000FFFF

#define D3DSI_INSTLENGTH_MASK   0x0F000000
#define D3DSI_INSTLENGTH_SHIFT  24

typedef enum _D3DSHADER_INSTRUCTION_OPCODE_TYPE
{
    D3DSIO_NOP          = 0,
    D3DSIO_MOV          ,
    D3DSIO_ADD          ,
    D3DSIO_SUB          ,
    D3DSIO_MAD          ,
    D3DSIO_MUL          ,
    D3DSIO_RCP          ,
    D3DSIO_RSQ          ,
    D3DSIO_DP3          ,
    D3DSIO_DP4          ,
    D3DSIO_MIN          ,
    D3DSIO_MAX          ,
    D3DSIO_SLT          ,
    D3DSIO_SGE          ,
    D3DSIO_EXP          ,
    D3DSIO_LOG          ,
    D3DSIO_LIT          ,
    D3DSIO_DST          ,
    D3DSIO_LRP          ,
    D3DSIO_FRC          ,
    D3DSIO_M4x4         ,
    D3DSIO_M4x3         ,
    D3DSIO_M3x4         ,
    D3DSIO_M3x3         ,
    D3DSIO_M3x2         ,
    D3DSIO_CALL         ,
    D3DSIO_CALLNZ       ,
    D3DSIO_LOOP         ,
    D3DSIO_RET          ,
    D3DSIO_ENDLOOP      ,
    D3DSIO_LABEL        ,
    D3DSIO_DCL          ,
    D3DSIO_POW          ,
    D3DSIO_CRS          ,
    D3DSIO_SGN          ,
    D3DSIO_ABS          ,
    D3DSIO_NRM          ,
    D3DSIO_SINCOS       ,
    D3DSIO_REP          ,
    D3DSIO_ENDREP       ,
    D3DSIO_IF           ,
    D3DSIO_IFC          ,
    D3DSIO_ELSE         ,
    D3DSIO_ENDIF        ,
    D3DSIO_BREAK        ,
    D3DSIO_BREAKC       ,
    D3DSIO_MOVA         ,
    D3DSIO_DEFB         ,
    D3DSIO_DEFI         ,

    D3DSIO_TEXCOORD     = 64,
    D3DSIO_TEXKILL      ,
    D3DSIO_TEX          ,
    D3DSIO_TEXBEM       ,
    D3DSIO_TEXBEML      ,
    D3DSIO_TEXREG2AR    ,
    D3DSIO_TEXREG2GB    ,
    D3DSIO_TEXM3x2PAD   ,
    D3DSIO_TEXM3x2TEX   ,
    D3DSIO_TEXM3x3PAD   ,
    D3DSIO_TEXM3x3TEX   ,
    D3DSIO_RESERVED0    ,
    D3DSIO_TEXM3x3SPEC  ,
    D3DSIO_TEXM3x3VSPEC ,
    D3DSIO_EXPP         ,
    D3DSIO_LOGP         ,
    D3DSIO_CND          ,
    D3DSIO_DEF          ,
    D3DSIO_TEXREG2RGB   ,
    D3DSIO_TEXDP3TEX    ,
    D3DSIO_TEXM3x2DEPTH ,
    D3DSIO_TEXDP3       ,
    D3DSIO_TEXM3x3      ,
    D3DSIO_TEXDEPTH     ,
    D3DSIO_CMP          ,
    D3DSIO_BEM          ,
    D3DSIO_DP2ADD       ,
    D3DSIO_DSX          ,
    D3DSIO_DSY          ,
    D3DSIO_TEXLDD       ,
    D3DSIO_SETP         ,
    D3DSIO_TEXLDL       ,
    D3DSIO_BREAKP       ,

    D3DSIO_PHASE        = 0xFFFD,
    D3DSIO_COMMENT      = 0xFFFE,
    D3DSIO_END          = 0xFFFF,

    D3DSIO_FORCE_DWORD  = 0x7fffffff,   // force 32-bit size enum
} D3DSHADER_INSTRUCTION_OPCODE_TYPE;

//---------------------------------------------------------------------
// Use these constants with D3DSIO_SINCOS macro as SRC2, SRC3
//
#define D3DSINCOSCONST1 -1.5500992e-006f, -2.1701389e-005f,  0.0026041667f, 0.00026041668f
#define D3DSINCOSCONST2 -0.020833334f, -0.12500000f, 1.0f, 0.50000000f

//---------------------------------------------------------------------
// Co-Issue Instruction Modifier - if set then this instruction is to be
// issued in parallel with the previous instruction(s) for which this bit
// is not set.
//
#define D3DSI_COISSUE           0x40000000

//---------------------------------------------------------------------
// Opcode specific controls

#define D3DSP_OPCODESPECIFICCONTROL_MASK  0x00ff0000
#define D3DSP_OPCODESPECIFICCONTROL_SHIFT 16

// ps_2_0 texld controls
#define D3DSI_TEXLD_PROJECT (0x01 << D3DSP_OPCODESPECIFICCONTROL_SHIFT)
#define D3DSI_TEXLD_BIAS    (0x02 << D3DSP_OPCODESPECIFICCONTROL_SHIFT)

// Comparison for dynamic conditional instruction opcodes (i.e. if, breakc)
typedef enum _D3DSHADER_COMPARISON
{
                         // < = >
    D3DSPC_RESERVED0= 0, // 0 0 0
    D3DSPC_GT       = 1, // 0 0 1
    D3DSPC_EQ       = 2, // 0 1 0
    D3DSPC_GE       = 3, // 0 1 1
    D3DSPC_LT       = 4, // 1 0 0
    D3DSPC_NE       = 5, // 1 0 1
    D3DSPC_LE       = 6, // 1 1 0
    D3DSPC_RESERVED1= 7  // 1 1 1
} D3DSHADER_COMPARISON;

// Comparison is part of instruction opcode token:
#define D3DSHADER_COMPARISON_SHIFT D3DSP_OPCODESPECIFICCONTROL_SHIFT
#define D3DSHADER_COMPARISON_MASK  (0x7<<D3DSHADER_COMPARISON_SHIFT)

//---------------------------------------------------------------------
// Predication flags on instruction token
#define D3DSHADER_INSTRUCTION_PREDICATED    (0x1 << 28)

//---------------------------------------------------------------------
// DCL Info Token Controls

// For dcl info tokens requiring a semantic (usage + index)
#define D3DSP_DCL_USAGE_SHIFT 0
#define D3DSP_DCL_USAGE_MASK  0x0000000f

#define D3DSP_DCL_USAGEINDEX_SHIFT 16
#define D3DSP_DCL_USAGEINDEX_MASK  0x000f0000

// DCL pixel shader sampler info token.
#define D3DSP_TEXTURETYPE_SHIFT 27
#define D3DSP_TEXTURETYPE_MASK  0x78000000

typedef enum _D3DSAMPLER_TEXTURE_TYPE
{
    D3DSTT_UNKNOWN = 0<<D3DSP_TEXTURETYPE_SHIFT, // uninitialized value
    D3DSTT_2D      = 2<<D3DSP_TEXTURETYPE_SHIFT, // dcl_2d s# (for declaring a 2-D texture)
    D3DSTT_CUBE    = 3<<D3DSP_TEXTURETYPE_SHIFT, // dcl_cube s# (for declaring a cube texture)
    D3DSTT_VOLUME  = 4<<D3DSP_TEXTURETYPE_SHIFT, // dcl_volume s# (for declaring a volume texture)
    D3DSTT_FORCE_DWORD  = 0x7fffffff,      // force 32-bit size enum
} D3DSAMPLER_TEXTURE_TYPE;

//---------------------------------------------------------------------
// Parameter Token Bit Definitions
//
#define D3DSP_REGNUM_MASK       0x000007FF

// destination parameter write mask
#define D3DSP_WRITEMASK_0       0x00010000  // Component 0 (X;Red)
#define D3DSP_WRITEMASK_1       0x00020000  // Component 1 (Y;Green)
#define D3DSP_WRITEMASK_2       0x00040000  // Component 2 (Z;Blue)
#define D3DSP_WRITEMASK_3       0x00080000  // Component 3 (W;Alpha)
#define D3DSP_WRITEMASK_ALL     0x000F0000  // All Components

// destination parameter modifiers
#define D3DSP_DSTMOD_SHIFT      20
#define D3DSP_DSTMOD_MASK       0x00F00000

// Bit masks for destination parameter modifiers
#define    D3DSPDM_NONE                 (0<<D3DSP_DSTMOD_SHIFT) // nop
#define    D3DSPDM_SATURATE             (1<<D3DSP_DSTMOD_SHIFT) // clamp to 0. to 1. range
#define    D3DSPDM_PARTIALPRECISION     (2<<D3DSP_DSTMOD_SHIFT) // Partial precision hint
#define    D3DSPDM_MSAMPCENTROID        (4<<D3DSP_DSTMOD_SHIFT) // Relevant to multisampling only:
                                                                //      When the pixel center is not covered, sample
                                                                //      attribute or compute gradients/LOD
                                                                //      using multisample "centroid" location.
                                                                //      "Centroid" is some location within the covered
                                                                //      region of the pixel.

// destination parameter
#define D3DSP_DSTSHIFT_SHIFT    24
#define D3DSP_DSTSHIFT_MASK     0x0F000000

// destination/source parameter register type
#define D3DSP_REGTYPE_SHIFT     28
#define D3DSP_REGTYPE_SHIFT2    8
#define D3DSP_REGTYPE_MASK      0x70000000
#define D3DSP_REGTYPE_MASK2     0x00001800

typedef enum _D3DSHADER_PARAM_REGISTER_TYPE
{
    D3DSPR_TEMP           =  0, // Temporary Register File
    D3DSPR_INPUT          =  1, // Input Register File
    D3DSPR_CONST          =  2, // Constant Register File
    D3DSPR_ADDR           =  3, // Address Register (VS)
    D3DSPR_TEXTURE        =  3, // Texture Register File (PS)
    D3DSPR_RASTOUT        =  4, // Rasterizer Register File
    D3DSPR_ATTROUT        =  5, // Attribute Output Register File
    D3DSPR_TEXCRDOUT      =  6, // Texture Coordinate Output Register File
    D3DSPR_OUTPUT         =  6, // Output register file for VS3.0+
    D3DSPR_CONSTINT       =  7, // Constant Integer Vector Register File
    D3DSPR_COLOROUT       =  8, // Color Output Register File
    D3DSPR_DEPTHOUT       =  9, // Depth Output Register File
    D3DSPR_SAMPLER        = 10, // Sampler State Register File
    D3DSPR_CONST2         = 11, // Constant Register File  2048 - 4095
    D3DSPR_CONST3         = 12, // Constant Register File  4096 - 6143
    D3DSPR_CONST4         = 13, // Constant Register File  6144 - 8191
    D3DSPR_CONSTBOOL      = 14, // Constant Boolean register file
    D3DSPR_LOOP           = 15, // Loop counter register file
    D3DSPR_TEMPFLOAT16    = 16, // 16-bit float temp register file
    D3DSPR_MISCTYPE       = 17, // Miscellaneous (single) registers.
    D3DSPR_LABEL          = 18, // Label
    D3DSPR_PREDICATE      = 19, // Predicate register
    D3DSPR_FORCE_DWORD  = 0x7fffffff,         // force 32-bit size enum
} D3DSHADER_PARAM_REGISTER_TYPE;

// The miscellaneous register file (D3DSPR_MISCTYPES)
// contains register types for which there is only ever one
// register (i.e. the register # is not needed).
// Rather than use up additional register types for such
// registers, they are defined
// as particular offsets into the misc. register file:
typedef enum _D3DSHADER_MISCTYPE_OFFSETS
{
    D3DSMO_POSITION   = 0, // Input position x,y,z,rhw (PS)
    D3DSMO_FACE   = 1, // Floating point primitive area (PS)
} D3DSHADER_MISCTYPE_OFFSETS;

// Register offsets in the Rasterizer Register File
//
typedef enum _D3DVS_RASTOUT_OFFSETS
{
    D3DSRO_POSITION = 0,
    D3DSRO_FOG,
    D3DSRO_POINT_SIZE,
    D3DSRO_FORCE_DWORD  = 0x7fffffff,         // force 32-bit size enum
} D3DVS_RASTOUT_OFFSETS;

// Source operand addressing modes

#define D3DVS_ADDRESSMODE_SHIFT 13
#define D3DVS_ADDRESSMODE_MASK  (1 << D3DVS_ADDRESSMODE_SHIFT)

typedef enum _D3DVS_ADDRESSMODE_TYPE
{
    D3DVS_ADDRMODE_ABSOLUTE  = (0 << D3DVS_ADDRESSMODE_SHIFT),
    D3DVS_ADDRMODE_RELATIVE  = (1 << D3DVS_ADDRESSMODE_SHIFT),
    D3DVS_ADDRMODE_FORCE_DWORD = 0x7fffffff, // force 32-bit size enum
} D3DVS_ADDRESSMODE_TYPE;

#define D3DSHADER_ADDRESSMODE_SHIFT 13
#define D3DSHADER_ADDRESSMODE_MASK  (1 << D3DSHADER_ADDRESSMODE_SHIFT)

typedef enum _D3DSHADER_ADDRESSMODE_TYPE
{
    D3DSHADER_ADDRMODE_ABSOLUTE  = (0 << D3DSHADER_ADDRESSMODE_SHIFT),
    D3DSHADER_ADDRMODE_RELATIVE  = (1 << D3DSHADER_ADDRESSMODE_SHIFT),
    D3DSHADER_ADDRMODE_FORCE_DWORD = 0x7fffffff, // force 32-bit size enum
} D3DSHADER_ADDRESSMODE_TYPE;

// Source operand swizzle definitions
//
#define D3DVS_SWIZZLE_SHIFT     16
#define D3DVS_SWIZZLE_MASK      0x00FF0000

// The following bits define where to take component X from:

#define D3DVS_X_X       (0 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_Y       (1 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_Z       (2 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_W       (3 << D3DVS_SWIZZLE_SHIFT)

// The following bits define where to take component Y from:

#define D3DVS_Y_X       (0 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_W       (3 << (D3DVS_SWIZZLE_SHIFT + 2))

// The following bits define where to take component Z from:

#define D3DVS_Z_X       (0 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_W       (3 << (D3DVS_SWIZZLE_SHIFT + 4))

// The following bits define where to take component W from:

#define D3DVS_W_X       (0 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_W       (3 << (D3DVS_SWIZZLE_SHIFT + 6))

// Value when there is no swizzle (X is taken from X, Y is taken from Y,
// Z is taken from Z, W is taken from W
//
#define D3DVS_NOSWIZZLE (D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W)

// source parameter swizzle
#define D3DSP_SWIZZLE_SHIFT     16
#define D3DSP_SWIZZLE_MASK      0x00FF0000

#define D3DSP_NOSWIZZLE \
    ( (0 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 6)) )

// pixel-shader swizzle ops
#define D3DSP_REPLICATERED \
    ( (0 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEGREEN \
    ( (1 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEBLUE \
    ( (2 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEALPHA \
    ( (3 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 6)) )

// source parameter modifiers
#define D3DSP_SRCMOD_SHIFT      24
#define D3DSP_SRCMOD_MASK       0x0F000000

typedef enum _D3DSHADER_PARAM_SRCMOD_TYPE
{
    D3DSPSM_NONE    = 0<<D3DSP_SRCMOD_SHIFT, // nop
    D3DSPSM_NEG     = 1<<D3DSP_SRCMOD_SHIFT, // negate
    D3DSPSM_BIAS    = 2<<D3DSP_SRCMOD_SHIFT, // bias
    D3DSPSM_BIASNEG = 3<<D3DSP_SRCMOD_SHIFT, // bias and negate
    D3DSPSM_SIGN    = 4<<D3DSP_SRCMOD_SHIFT, // sign
    D3DSPSM_SIGNNEG = 5<<D3DSP_SRCMOD_SHIFT, // sign and negate
    D3DSPSM_COMP    = 6<<D3DSP_SRCMOD_SHIFT, // complement
    D3DSPSM_X2      = 7<<D3DSP_SRCMOD_SHIFT, // *2
    D3DSPSM_X2NEG   = 8<<D3DSP_SRCMOD_SHIFT, // *2 and negate
    D3DSPSM_DZ      = 9<<D3DSP_SRCMOD_SHIFT, // divide through by z component
    D3DSPSM_DW      = 10<<D3DSP_SRCMOD_SHIFT, // divide through by w component
    D3DSPSM_ABS     = 11<<D3DSP_SRCMOD_SHIFT, // abs()
    D3DSPSM_ABSNEG  = 12<<D3DSP_SRCMOD_SHIFT, // -abs()
    D3DSPSM_NOT     = 13<<D3DSP_SRCMOD_SHIFT, // for predicate register: "!p0"
    D3DSPSM_FORCE_DWORD = 0x7fffffff,        // force 32-bit size enum
} D3DSHADER_PARAM_SRCMOD_TYPE;

// pixel shader version token
#define D3DPS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))

// vertex shader version token
#define D3DVS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))

// extract major/minor from version cap
#define D3DSHADER_VERSION_MAJOR(_Version) (((_Version)>>8)&0xFF)
#define D3DSHADER_VERSION_MINOR(_Version) (((_Version)>>0)&0xFF)

// destination/source parameter register type
#define D3DSI_COMMENTSIZE_SHIFT     16
#define D3DSI_COMMENTSIZE_MASK      0x7FFF0000
#define D3DSHADER_COMMENT(_DWordSize) \
    ((((_DWordSize)<<D3DSI_COMMENTSIZE_SHIFT)&D3DSI_COMMENTSIZE_MASK)|D3DSIO_COMMENT)

// pixel/vertex shader end token
#define D3DPS_END()  0x0000FFFF
#define D3DVS_END()  0x0000FFFF

// begin internal
// Shader Validator Error Type Enum
// Used by IDirect3DShaderValidator9
typedef enum _D3DSVERROR_TYPE
{
    D3DSVE_GENERIC_ERROR = 0,
    D3DSVE_INSTRUCTION_WARNING,
    D3DSVE_INSTRUCTION_ERROR,
    D3DSVE_ELEMENT_WARNING, // for vertex decl validation
    D3DSVE_ELEMENT_ERROR,   // for vertex decl validation
    D3DSVE_GLOBAL_WARNING,
    D3DSVE_GLOBAL_ERROR,
    D3DSVE_GENERIC_WARNING,
} D3DSVERROR_TYPE;
#define D3DSVE_TYPE_MASK 0x0000ffff
#define D3DSVE_FLAGS_MASK 0xffff0000
#define D3DSVE_FLAGS_CUSTOM_HLSL_MESSAGE 0x00010000
// end internal

//---------------------------------------------------------------------

// High order surfaces
//
typedef enum _D3DBASISTYPE
{
   D3DBASIS_BEZIER      = 0,
   D3DBASIS_BSPLINE     = 1,
   D3DBASIS_CATMULL_ROM = 2, /* In D3D8 this used to be D3DBASIS_INTERPOLATE */
   D3DBASIS_FORCE_DWORD = 0x7fffffff,
} D3DBASISTYPE;

typedef enum _D3DDEGREETYPE
{
   D3DDEGREE_LINEAR      = 1,
   D3DDEGREE_QUADRATIC   = 2,
   D3DDEGREE_CUBIC       = 3,
   D3DDEGREE_QUINTIC     = 5,
   D3DDEGREE_FORCE_DWORD = 0x7fffffff,
} D3DDEGREETYPE;

typedef enum _D3DPATCHEDGESTYLE
{
   D3DPATCHEDGE_DISCRETE    = 0,
   D3DPATCHEDGE_CONTINUOUS  = 1,
   D3DPATCHEDGE_FORCE_DWORD = 0x7fffffff,
} D3DPATCHEDGESTYLE;

typedef enum _D3DSTATEBLOCKTYPE
{
    D3DSBT_ALL           = 1, // capture all state
    D3DSBT_PIXELSTATE    = 2, // capture pixel state
    D3DSBT_VERTEXSTATE   = 3, // capture vertex state
    D3DSBT_FORCE_DWORD   = 0x7fffffff,
} D3DSTATEBLOCKTYPE;

// Macros to set texture coordinate format bits in the FVF id

#define D3DFVF_TEXTUREFORMAT2 0         // Two floating point values
#define D3DFVF_TEXTUREFORMAT1 3         // One floating point value
#define D3DFVF_TEXTUREFORMAT3 1         // Three floating point values
#define D3DFVF_TEXTUREFORMAT4 2         // Four floating point values

#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))

//---------------------------------------------------------------------

/* Direct3D9 Device types */
typedef enum _D3DDEVTYPE
{
    D3DDEVTYPE_HAL = 1,

    // The following are not supported on Xbox 360:
    //
    // D3DDEVTYPE_REF
    // D3DDEVTYPE_SW

    D3DDEVTYPE_FORCE_DWORD  = 0x7fffffff
} D3DDEVTYPE;

/* Multi-Sample buffer types */
typedef enum _D3DMULTISAMPLE_TYPE
{
    D3DMULTISAMPLE_NONE            =  0,
    D3DMULTISAMPLE_2_SAMPLES       =  1,
    D3DMULTISAMPLE_4_SAMPLES       =  2,

    // Other multisample types are not supported on Xbox 360

    D3DMULTISAMPLE_FORCE_DWORD     = 0x7fffffff
} D3DMULTISAMPLE_TYPE;

/* Formats
 * Most of these names have the following convention:
 *      A = Alpha
 *      R = Red
 *      G = Green
 *      B = Blue
 *      X = Unused Bits
 *      P = Palette
 *      L = Luminance
 *      U = dU coordinate for BumpMap
 *      V = dV coordinate for BumpMap
 *      S = Stencil
 *      D = Depth (e.g. Z or W buffer)
 *      C = Computed from other channels (typically on certain read operations)
 *
 *      Further, the order of the pieces are from MSB first; hence
 *      D3DFMT_A8L8 indicates that the high byte of this two byte
 *      format is alpha.
 *
 *      D16 indicates:
 *           - An integer 16-bit value.
 *           - An app-lockable surface.
 *
 *      All Depth/Stencil formats except D3DFMT_D16_LOCKABLE indicate:
 *          - no particular bit ordering per pixel, and
 *          - are not app lockable, and
 *          - the driver is allowed to consume more than the indicated
 *            number of bits per Depth channel (but not Stencil channel).
 */
#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

// A D3DFORMAT constant is an identifier that uniquely identifies
// a texture or surface format.  It contains the following fields:

#define D3DFORMAT_TEXTUREFORMAT_SHIFT   0       // GPUTEXTUREFORMAT
#define D3DFORMAT_ENDIAN_SHIFT          6       // GPUENDIAN
#define D3DFORMAT_TILED_SHIFT           8       // BOOL
#define D3DFORMAT_SIGNX_SHIFT           9       // GPUSIGN
#define D3DFORMAT_SIGNY_SHIFT           11      // GPUSIGN
#define D3DFORMAT_SIGNZ_SHIFT           13      // GPUSIGN
#define D3DFORMAT_SIGNW_SHIFT           15      // GPUSIGN
#define D3DFORMAT_NUMFORMAT_SHIFT       17      // GPUNUMFORMAT
#define D3DFORMAT_SWIZZLEX_SHIFT        18      // GPUSWIZZLE
#define D3DFORMAT_SWIZZLEY_SHIFT        21      // GPUSWIZZLE
#define D3DFORMAT_SWIZZLEZ_SHIFT        24      // GPUSWIZZLE
#define D3DFORMAT_SWIZZLEW_SHIFT        27      // GPUSWIZZLE
#define D3DFORMAT_SWIZZLE_SHIFT         D3DFORMAT_SWIZZLEX_SHIFT

#define D3DFORMAT_TEXTUREFORMAT_MASK    0x0000003f
#define D3DFORMAT_ENDIAN_MASK           0x000000c0
#define D3DFORMAT_TILED_MASK            0x00000100
#define D3DFORMAT_SIGNX_MASK            0x00000600
#define D3DFORMAT_SIGNY_MASK            0x00001800
#define D3DFORMAT_SIGNZ_MASK            0x00006000
#define D3DFORMAT_SIGNW_MASK            0x00018000
#define D3DFORMAT_NUMFORMAT_MASK        0x00020000
#define D3DFORMAT_SWIZZLEX_MASK         0x001c0000
#define D3DFORMAT_SWIZZLEY_MASK         0x00e00000
#define D3DFORMAT_SWIZZLEZ_MASK         0x07000000
#define D3DFORMAT_SWIZZLEW_MASK         0x38000000
#define D3DFORMAT_SWIZZLE_MASK          (D3DFORMAT_SWIZZLEX_MASK | D3DFORMAT_SWIZZLEY_MASK | D3DFORMAT_SWIZZLEZ_MASK | D3DFORMAT_SWIZZLEW_MASK)

#define MAKED3DFMT(TextureFormat, Endian, Tiled, TextureSign, NumFormat, Swizzle) \
            ((TextureFormat) << D3DFORMAT_TEXTUREFORMAT_SHIFT | \
             (Endian) << D3DFORMAT_ENDIAN_SHIFT | \
             (Tiled) << D3DFORMAT_TILED_SHIFT | \
             (TextureSign) << D3DFORMAT_SIGNX_SHIFT | \
             (NumFormat) << D3DFORMAT_NUMFORMAT_SHIFT | \
             (Swizzle) << D3DFORMAT_SWIZZLEX_SHIFT)

#define MAKED3DFMT2(TextureFormat, Endian, Tiled, TextureSignX, TextureSignY, TextureSignZ, TextureSignW, NumFormat, SwizzleX, SwizzleY, SwizzleZ, SwizzleW) \
            ((TextureFormat) << D3DFORMAT_TEXTUREFORMAT_SHIFT | \
             (Endian) << D3DFORMAT_ENDIAN_SHIFT | \
             (Tiled) << D3DFORMAT_TILED_SHIFT | \
             (TextureSignX) << D3DFORMAT_SIGNX_SHIFT | \
             (TextureSignY) << D3DFORMAT_SIGNY_SHIFT | \
             (TextureSignZ) << D3DFORMAT_SIGNZ_SHIFT | \
             (TextureSignW) << D3DFORMAT_SIGNW_SHIFT | \
             (NumFormat) << D3DFORMAT_NUMFORMAT_SHIFT | \
             (SwizzleX) << D3DFORMAT_SWIZZLEX_SHIFT | \
             (SwizzleY) << D3DFORMAT_SWIZZLEY_SHIFT | \
             (SwizzleZ) << D3DFORMAT_SWIZZLEZ_SHIFT | \
             (SwizzleW) << D3DFORMAT_SWIZZLEW_SHIFT)

#define MAKELINFMT(D3dFmt) ((D3dFmt) & ~D3DFORMAT_TILED_MASK)

#define MAKESRGBFMT(D3dFmt) (((D3dFmt) & ~(D3DFORMAT_SIGNX_MASK | D3DFORMAT_SIGNY_MASK | D3DFORMAT_SIGNZ_MASK)) | \
             ((GPUSIGN_GAMMA << D3DFORMAT_SIGNX_SHIFT) | \
              (GPUSIGN_GAMMA << D3DFORMAT_SIGNY_SHIFT) | \
              (GPUSIGN_GAMMA << D3DFORMAT_SIGNZ_SHIFT)))

#define MAKELEFMT(D3dFmt) (((D3dFmt) & ~D3DFORMAT_ENDIAN_MASK) | (GPUENDIAN_NONE << D3DFORMAT_ENDIAN_SHIFT))

typedef enum _D3DFORMAT
{
    // DXT:

    D3DFMT_DXT1                 = MAKED3DFMT(GPUTEXTUREFORMAT_DXT1, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT1             = MAKELINFMT(D3DFMT_DXT1),

    D3DFMT_DXT2                 = MAKED3DFMT(GPUTEXTUREFORMAT_DXT2_3, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT2             = MAKELINFMT(D3DFMT_DXT2),

    D3DFMT_DXT3                 = D3DFMT_DXT2,
    D3DFMT_LIN_DXT3             = MAKELINFMT(D3DFMT_DXT3),

    D3DFMT_DXT4                 = MAKED3DFMT(GPUTEXTUREFORMAT_DXT4_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT4             = MAKELINFMT(D3DFMT_DXT4),

    D3DFMT_DXT5                 = D3DFMT_DXT4,
    D3DFMT_LIN_DXT5             = MAKELINFMT(D3DFMT_DXT5),

    D3DFMT_DXN                  = MAKED3DFMT(GPUTEXTUREFORMAT_DXN, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXN              = MAKELINFMT(D3DFMT_DXN),

    // 8bpp:

    D3DFMT_A8                   = MAKED3DFMT(GPUTEXTUREFORMAT_8, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_RZZZ),
    D3DFMT_LIN_A8               = MAKELINFMT(D3DFMT_A8),

    D3DFMT_L8                   = MAKED3DFMT(GPUTEXTUREFORMAT_8, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORRR),
    D3DFMT_LIN_L8               = MAKELINFMT(D3DFMT_L8),

    // 16bpp:

    D3DFMT_R5G6B5               = MAKED3DFMT(GPUTEXTUREFORMAT_5_6_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_R5G6B5           = MAKELINFMT(D3DFMT_R5G6B5),

    D3DFMT_R6G5B5               = MAKED3DFMT(GPUTEXTUREFORMAT_6_5_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_R6G5B5           = MAKELINFMT(D3DFMT_R6G5B5),

    D3DFMT_L6V5U5               = MAKED3DFMT2(GPUTEXTUREFORMAT_6_5_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_UNSIGNED, GPUSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),
    D3DFMT_LIN_L6V5U5           = MAKELINFMT(D3DFMT_L6V5U5),

    D3DFMT_X1R5G5B5             = MAKED3DFMT(GPUTEXTUREFORMAT_1_5_5_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_X1R5G5B5         = MAKELINFMT(D3DFMT_X1R5G5B5),

    D3DFMT_A1R5G5B5             = MAKED3DFMT(GPUTEXTUREFORMAT_1_5_5_5, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_A1R5G5B5         = MAKELINFMT(D3DFMT_A1R5G5B5),

    D3DFMT_A4R4G4B4             = MAKED3DFMT(GPUTEXTUREFORMAT_4_4_4_4, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_A4R4G4B4         = MAKELINFMT(D3DFMT_A4R4G4B4),

    D3DFMT_X4R4G4B4             = MAKED3DFMT(GPUTEXTUREFORMAT_4_4_4_4, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_X4R4G4B4         = MAKELINFMT(D3DFMT_X4R4G4B4),

    D3DFMT_Q4W4V4U4             = MAKED3DFMT(GPUTEXTUREFORMAT_4_4_4_4, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_Q4W4V4U4         = MAKELINFMT(D3DFMT_Q4W4V4U4),

    D3DFMT_A8L8                 = MAKED3DFMT(GPUTEXTUREFORMAT_8_8, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_GRRR),
    D3DFMT_LIN_A8L8             = MAKELINFMT(D3DFMT_A8L8),

    D3DFMT_G8R8                 = MAKED3DFMT(GPUTEXTUREFORMAT_8_8, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G8R8             = MAKELINFMT(D3DFMT_G8R8),

    D3DFMT_V8U8                 = MAKED3DFMT(GPUTEXTUREFORMAT_8_8, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_V8U8             = MAKELINFMT(D3DFMT_V8U8),

    D3DFMT_D16                  = MAKED3DFMT(GPUTEXTUREFORMAT_16, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_D16              = MAKELINFMT(D3DFMT_D16),

    D3DFMT_L16                  = MAKED3DFMT(GPUTEXTUREFORMAT_16, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORRR),
    D3DFMT_LIN_L16              = MAKELINFMT(D3DFMT_L16),

    D3DFMT_R16F                 = MAKED3DFMT(GPUTEXTUREFORMAT_16_FLOAT, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOOR),
    D3DFMT_LIN_R16F             = MAKELINFMT(D3DFMT_R16F),

    D3DFMT_R16F_EXPAND          = MAKED3DFMT(GPUTEXTUREFORMAT_16_EXPAND, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOOR),
    D3DFMT_LIN_R16F_EXPAND      = MAKELINFMT(D3DFMT_R16F_EXPAND),

    D3DFMT_UYVY                 = MAKED3DFMT(GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_UYVY             = MAKELINFMT(D3DFMT_UYVY),

    D3DFMT_LE_UYVY              = MAKED3DFMT(GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LE_LIN_UYVY          = MAKELINFMT(D3DFMT_LE_UYVY),

    D3DFMT_G8R8_G8B8            = MAKED3DFMT(GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_G8R8_G8B8        = MAKELINFMT(D3DFMT_G8R8_G8B8),

    D3DFMT_R8G8_B8G8            = MAKED3DFMT(GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_R8G8_B8G8        = MAKELINFMT(D3DFMT_R8G8_B8G8),

    D3DFMT_YUY2                 = MAKED3DFMT(GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_YUY2             = MAKELINFMT(D3DFMT_YUY2),

    D3DFMT_LE_YUY2              = MAKED3DFMT(GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LE_LIN_YUY2          = MAKELINFMT(D3DFMT_LE_YUY2),

    // 32bpp:

    D3DFMT_A8R8G8B8             = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_A8R8G8B8         = MAKELINFMT(D3DFMT_A8R8G8B8),

    D3DFMT_X8R8G8B8             = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_X8R8G8B8         = MAKELINFMT(D3DFMT_X8R8G8B8),

    D3DFMT_A8B8G8R8             = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A8B8G8R8         = MAKELINFMT(D3DFMT_A8B8G8R8),

    D3DFMT_X8B8G8R8             = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OBGR),
    D3DFMT_LIN_X8B8G8R8         = MAKELINFMT(D3DFMT_X8B8G8R8),

    D3DFMT_X8L8V8U8             = MAKED3DFMT2(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_UNSIGNED, GPUSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_1),
    D3DFMT_LIN_X8L8V8U8         = MAKELINFMT(D3DFMT_X8L8V8U8),

    D3DFMT_Q8W8V8U8             = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_Q8W8V8U8         = MAKELINFMT(D3DFMT_Q8W8V8U8),

    D3DFMT_A2R10G10B10          = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LIN_A2R10G10B10      = MAKELINFMT(D3DFMT_A2R10G10B10),

    D3DFMT_X2R10G10B10          = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_X2R10G10B10      = MAKELINFMT(D3DFMT_X2R10G10B10),

    D3DFMT_A2B10G10R10          = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A2B10G10R10      = MAKELINFMT(D3DFMT_A2B10G10R10),

    D3DFMT_A2W10V10U10          = MAKED3DFMT2(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W),
    D3DFMT_LIN_A2W10V10U10      = MAKELINFMT(D3DFMT_A2W10V10U10),

    D3DFMT_A16L16               = MAKED3DFMT(GPUTEXTUREFORMAT_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_GRRR),
    D3DFMT_LIN_A16L16           = MAKELINFMT(D3DFMT_A16L16),

    D3DFMT_G16R16               = MAKED3DFMT(GPUTEXTUREFORMAT_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G16R16           = MAKELINFMT(D3DFMT_G16R16),

    D3DFMT_V16U16               = MAKED3DFMT(GPUTEXTUREFORMAT_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_V16U16           = MAKELINFMT(D3DFMT_V16U16),

    D3DFMT_R10G11B11            = MAKED3DFMT(GPUTEXTUREFORMAT_10_11_11_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_R10G11B11        = MAKELINFMT(D3DFMT_R10G11B11),

    D3DFMT_R11G11B10            = MAKED3DFMT(GPUTEXTUREFORMAT_11_11_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LIN_R11G11B10        = MAKELINFMT(D3DFMT_R11G11B10),

    D3DFMT_W10V11U11            = MAKED3DFMT(GPUTEXTUREFORMAT_10_11_11_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OBGR),
    D3DFMT_LIN_W10V11U11        = MAKELINFMT(D3DFMT_W10V11U11),

    D3DFMT_W11V11U10            = MAKED3DFMT(GPUTEXTUREFORMAT_11_11_10_AS_16_16_16_16, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OBGR),
    D3DFMT_LIN_W11V11U10        = MAKELINFMT(D3DFMT_W11V11U10),

    D3DFMT_G16R16F              = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_FLOAT, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G16R16F          = MAKELINFMT(D3DFMT_G16R16F),

    D3DFMT_G16R16F_EXPAND       = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_EXPAND, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G16R16F_EXPAND   = MAKELINFMT(D3DFMT_G16R16F_EXPAND),

    D3DFMT_L32                  = MAKED3DFMT(GPUTEXTUREFORMAT_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORRR),
    D3DFMT_LIN_L32              = MAKELINFMT(D3DFMT_L32),

    D3DFMT_R32F                 = MAKED3DFMT(GPUTEXTUREFORMAT_32_FLOAT, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOOR),
    D3DFMT_LIN_R32F             = MAKELINFMT(D3DFMT_R32F),

    // 64bpp:

    D3DFMT_A16B16G16R16         = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_16_16, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A16B16G16R16     = MAKELINFMT(D3DFMT_A16B16G16R16),

    D3DFMT_Q16W16V16U16         = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_16_16, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_Q16W16V16U16     = MAKELINFMT(D3DFMT_Q16W16V16U16),

    D3DFMT_A16B16G16R16F        = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_16_16_FLOAT, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A16B16G16R16F    = MAKELINFMT(D3DFMT_A16B16G16R16F),

    D3DFMT_A16B16G16R16F_EXPAND = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_16_16_EXPAND, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A16B16G16R16F_EXPAND = MAKELINFMT(D3DFMT_A16B16G16R16F_EXPAND),

    D3DFMT_A32L32               = MAKED3DFMT(GPUTEXTUREFORMAT_32_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_GRRR),
    D3DFMT_LIN_A32L32           = MAKELINFMT(D3DFMT_A32L32),

    D3DFMT_G32R32               = MAKED3DFMT(GPUTEXTUREFORMAT_32_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G32R32           = MAKELINFMT(D3DFMT_G32R32),

    D3DFMT_V32U32               = MAKED3DFMT(GPUTEXTUREFORMAT_32_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_V32U32           = MAKELINFMT(D3DFMT_V32U32),

    D3DFMT_G32R32F              = MAKED3DFMT(GPUTEXTUREFORMAT_32_32_FLOAT, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_G32R32F          = MAKELINFMT(D3DFMT_G32R32F),

    // 128bpp:

    D3DFMT_A32B32G32R32         = MAKED3DFMT(GPUTEXTUREFORMAT_32_32_32_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A32B32G32R32     = MAKELINFMT(D3DFMT_A32B32G32R32),

    D3DFMT_Q32W32V32U32         = MAKED3DFMT(GPUTEXTUREFORMAT_32_32_32_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_Q32W32V32U32     = MAKELINFMT(D3DFMT_Q32W32V32U32),

    D3DFMT_A32B32G32R32F        = MAKED3DFMT(GPUTEXTUREFORMAT_32_32_32_32_FLOAT, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_A32B32G32R32F    = MAKELINFMT(D3DFMT_A32B32G32R32F),

    // EDRAM only:

    D3DFMT_A2B10G10R10F_EDRAM   = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_FLOAT_EDRAM, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_SIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_G16R16_EDRAM         = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_EDRAM, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_A16B16G16R16_EDRAM   = MAKED3DFMT(GPUTEXTUREFORMAT_16_16_16_16_EDRAM, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),

    // Front buffer formats have to be little endian:

    D3DFMT_LE_X8R8G8B8          = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LE_A8R8G8B8          = MAKED3DFMT(GPUTEXTUREFORMAT_8_8_8_8, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),
    D3DFMT_LE_X2R10G10B10       = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ORGB),
    D3DFMT_LE_A2R10G10B10       = MAKED3DFMT(GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16, GPUENDIAN_NONE, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ARGB),

    // Other:

    D3DFMT_INDEX16              = 0,
    D3DFMT_INDEX32              = 1,
    D3DFMT_VERTEXDATA           = 2,
    D3DFMT_UNKNOWN              = 0xffffffff,

    /*
     * The following are not supported as textures in realtime mode on the Alpha:
     */

    D3DFMT_DXT3A                = MAKED3DFMT(GPUTEXTUREFORMAT_DXT3A, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT3A            = MAKELINFMT(D3DFMT_DXT3A),

    D3DFMT_DXT3A_1111           = MAKED3DFMT(GPUTEXTUREFORMAT_DXT3A_AS_1_1_1_1, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT3A_1111       = MAKELINFMT(D3DFMT_DXT3A_1111),

    D3DFMT_DXT5A                = MAKED3DFMT(GPUTEXTUREFORMAT_DXT5A, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_DXT5A            = MAKELINFMT(D3DFMT_DXT5A),

    D3DFMT_CTX1                 = MAKED3DFMT(GPUTEXTUREFORMAT_CTX1, GPUENDIAN_8IN16, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_CTX1             = MAKELINFMT(D3DFMT_CTX1),

    D3DFMT_D24S8                = MAKED3DFMT(GPUTEXTUREFORMAT_24_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOGR),
    D3DFMT_LIN_D24S8            = MAKELINFMT(D3DFMT_D24S8),

    D3DFMT_D24X8                = MAKED3DFMT(GPUTEXTUREFORMAT_24_8, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, GPUSWIZZLE_OOOR),
    D3DFMT_LIN_D24X8            = MAKELINFMT(D3DFMT_D24X8),

    D3DFMT_D24FS8               = MAKED3DFMT(GPUTEXTUREFORMAT_24_8_FLOAT, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_D24FS8           = MAKELINFMT(D3DFMT_D24FS8),

    D3DFMT_D32                  = MAKED3DFMT(GPUTEXTUREFORMAT_32, GPUENDIAN_8IN32, TRUE, GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_INTEGER, GPUSWIZZLE_ABGR),
    D3DFMT_LIN_D32              = MAKELINFMT(D3DFMT_D32),

    // The following are not supported on Xbox 360:
    //
    // D3DFMT_A8R3G3B2
    // D3DFMT_R3G3B2
    // D3DFMT_S1D15
    // D3DFMT_S8D24
    // D3DFMT_X8D24
    // D3DFMT_X4S4D24
    // D3DFMT_A8P8
    // D3DFMT_P8
    // D3DFMT_A4L4
    // D3DFMT_R8G8B8
    // D3DFMT_D16_LOCKABLE
    // D3DFMT_D15S1
    // D3DFMT_D24X4S4
    // D3DFMT_D32F_LOCKABLE

    D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;

/* Display Modes */
typedef struct _D3DDISPLAYMODE
{
    UINT            Width;
    UINT            Height;
    UINT            RefreshRate;
    D3DFORMAT       Format;
} D3DDISPLAYMODE;

/* Creation Parameters */
typedef struct _D3DDEVICE_CREATION_PARAMETERS
{
    UINT            AdapterOrdinal;
    D3DDEVTYPE      DeviceType;
    HWND            hFocusWindow;
    DWORD           BehaviorFlags;
} D3DDEVICE_CREATION_PARAMETERS;


/* SwapEffects */
typedef enum _D3DSWAPEFFECT
{
    D3DSWAPEFFECT_DISCARD           = 1,            // Ignored on Xbox 360
    D3DSWAPEFFECT_FLIP              = 2,            // Ignored on Xbox 360
    D3DSWAPEFFECT_COPY              = 3,

    D3DSWAPEFFECT_FORCE_DWORD       = 0x7fffffff
} D3DSWAPEFFECT;

/* Swap flags (Xbox 360 extension) */

#define D3DSWAP_DEFAULT             0x00000000L

/* Insert Callback (Xbox 360 extension) */
typedef DWORD D3DCALLBACKTYPE;

#define D3DCALLBACK_IDLE            0x00000000
#define D3DCALLBACK_IMMEDIATE       0x00000001
#define D3DCALLBACK_CPU0            0x01000000
#define D3DCALLBACK_CPU1            0x02000000
#define D3DCALLBACK_CPU2            0x04000000
#define D3DCALLBACK_CPU3            0x08000000
#define D3DCALLBACK_CPU4            0x10000000
#define D3DCALLBACK_CPU5            0x20000000
#define D3DCALLBACK_CPU_MASK        0x3F000000
#define D3DCALLBACK_CPU_SHIFT       24

/* VBlank data (Xbox 360 extension) */
#define D3DVBLANK_SWAPDONE          0x00000001L
#define D3DVBLANK_SWAPMISSED        0x00000002L

typedef struct _D3DVBLANKDATA
{
    DWORD           VBlank;
    DWORD           Swap;
    DWORD           Flags;
} D3DVBLANKDATA;

/* Swap data (Xbox 360 extension) */
typedef struct _D3DSWAPDATA
{
    DWORD           Swap;
    DWORD           SwapVBlank;
    DWORD           MissedVBlanks;
    DWORD           TimeUntilSwapVBlank;
    DWORD           TimeBetweenSwapVBlanks;
} D3DSWAPDATA;

#ifndef D3DVECTOR4_DEFINED
#ifdef _XBOX
typedef __vector4 D3DVECTOR4;
typedef D3DVECTOR4 *LPD3DVECTOR4;
#else
typedef struct _D3DVECTOR4 {
    float x;
    float y;
    float z;
    float w;
} D3DVECTOR4;
typedef struct _D3DVECTOR4 *LPD3DVECTOR4;
#endif // _XBOX
#define D3DVECTOR4_DEFINED
#endif // D3DVECTOR4_DEFINED

// Enumeration for GPU major version
//
typedef enum _D3DGPUMAJORVERSION
{
    D3DGPUMAJORVERSION_BETAPREVIEW  = 2,
    D3DGPUMAJORVERSION_FINAL        = 3,
} D3DGPUMAJORVERSION;

typedef struct _D3DGPUVERSION
{
    D3DGPUMAJORVERSION  MajorVersion;
    USHORT              MinorVersion;
} D3DGPUVERSION, *PD3DGPUVERSION;

// --- end of Xbox 360 extensions ---

/* Pool types */
typedef DWORD D3DPOOL;

#define D3DPOOL_DEFAULT             0
#define D3DPOOL_MANAGED             1
#define D3DPOOL_SYSTEMMEM           2
#define D3DPOOL_SCRATCH             3


/* RefreshRate pre-defines */
#define D3DPRESENT_RATE_DEFAULT     0x00000000

/* SetRingBuffer parameters */
typedef struct _D3DRING_BUFFER_PARAMETERS
{
    DWORD                           Flags;
    DWORD                           PrimarySize;
    void*                           pPrimary;
    DWORD                           SecondarySize;
    void*                           pSecondary;
    DWORD                           SegmentCount;
} D3DRING_BUFFER_PARAMETERS;

/* Front buffer color space */
typedef enum _D3DCOLORSPACE
{
    D3DCOLORSPACE_RGB               = 0,
    D3DCOLORSPACE_YCbCr601          = 1,
    D3DCOLORSPACE_YCbCr709          = 2,
    D3DCOLORSPACE_FORCE_DWORD       = 0x7fffffff
} D3DCOLORSPACE;

/* SetScaler parameters */
typedef struct _D3DVIDEO_SCALER_PARAMETERS
{
    D3DRECT                         ScalerSourceRect;
    DWORD                           ScaledOutputWidth;
    DWORD                           ScaledOutputHeight;
    DWORD                           FilterProfile;
} D3DVIDEO_SCALER_PARAMETERS;

/* CreateDevice parameters */
typedef struct _D3DPRESENT_PARAMETERS_
{
    UINT                            BackBufferWidth;
    UINT                            BackBufferHeight;
    D3DFORMAT                       BackBufferFormat;
    UINT                            BackBufferCount;            // Unused on Xbox 360
    D3DMULTISAMPLE_TYPE             MultiSampleType;
    DWORD                           MultiSampleQuality;         // Unused on Xbox 360
    D3DSWAPEFFECT                   SwapEffect;                 // Unused on Xbox 360
    HWND                            hDeviceWindow;              // Unused on Xbox 360
    BOOL                            Windowed;                   // Unused on Xbox 360
    BOOL                            EnableAutoDepthStencil;
    D3DFORMAT                       AutoDepthStencilFormat;
    DWORD                           Flags;
    UINT                            FullScreen_RefreshRateInHz;
    UINT                            PresentationInterval;
    BOOL                            DisableAutoBackBuffer;      // Xbox 360 extension
    BOOL                            DisableAutoFrontBuffer;     // Xbox 360 extension
    D3DFORMAT                       FrontBufferFormat;          // Xbox 360 extension
    D3DCOLORSPACE                   FrontBufferColorSpace;      // Xbox 360 extension
    D3DRING_BUFFER_PARAMETERS       RingBufferParameters;       // Xbox 360 extension
    D3DVIDEO_SCALER_PARAMETERS      VideoScalerParameters;      // Xbox 360 extension

} D3DPRESENT_PARAMETERS;

/* CreateRenderTarget/CreateDepthStencilSurface optional parameters */
typedef struct _D3DSURFACE_PARAMETERS
{
    DWORD                           Base;
    DWORD                           HierarchicalZBase;
    INT                             ColorExpBias;

} D3DSURFACE_PARAMETERS;

// Values for D3DPRESENT_PARAMETERS.Flags

#define D3DPRESENTFLAG_NO_LETTERBOX             0x00000001      // Xbox 360 extension

// The following are ignored on Xbox 360:

#define D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL     0x00000000

// The following are not supported on Xbox 360:
//
//  D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
//  D3DPRESENTFLAG_DEVICECLIP
//  D3DPRESENTFLAG_VIDEO

/* Gamma Ramp: Same as DX7 */

typedef struct _D3DGAMMARAMP
{
    WORD                red  [256];
    WORD                green[256];
    WORD                blue [256];
} D3DGAMMARAMP;

// Piece-wise linear Gamma: Xbox 360 extension

typedef struct _D3DPWLGAMMAENTRY
{
    WORD Base;
    WORD Delta;
} D3DPWLGAMMAENTRY;

typedef struct _D3DPWLGAMMA
{
    D3DPWLGAMMAENTRY    red  [128];
    D3DPWLGAMMAENTRY    green[128];
    D3DPWLGAMMAENTRY    blue [128];
} D3DPWLGAMMA;

/* Back buffer types */
typedef enum _D3DBACKBUFFER_TYPE
{
    D3DBACKBUFFER_TYPE_MONO         = 0,
    D3DBACKBUFFER_TYPE_LEFT         = 1,
    D3DBACKBUFFER_TYPE_RIGHT        = 2,

    D3DBACKBUFFER_TYPE_FORCE_DWORD  = 0x7fffffff
} D3DBACKBUFFER_TYPE;


/* Types */
typedef enum _D3DRESOURCETYPE {
    D3DRTYPE_NONE                   =  0,
    D3DRTYPE_SURFACE                =  1,
    D3DRTYPE_VOLUME                 =  2,
    D3DRTYPE_TEXTURE                =  3,
    D3DRTYPE_VOLUMETEXTURE          =  4,
    D3DRTYPE_CUBETEXTURE            =  5,
    D3DRTYPE_VERTEXBUFFER           =  6,
    D3DRTYPE_INDEXBUFFER            =  7,
    D3DRTYPE_ARRAYTEXTURE           =  8,

    D3DRTYPE_FORCE_DWORD            = 0x7fffffff
} D3DRESOURCETYPE;

/* Usages */
#define D3DUSAGE_CPU_CACHED_MEMORY              (0x00000004L)

// All Xbox 360 Direct3D APIs except CheckDeviceFormat ignore the following:

#define D3DUSAGE_RENDERTARGET                   (0x00000001L)
#define D3DUSAGE_DEPTHSTENCIL                   (0x00000002L)
#define D3DUSAGE_DYNAMIC                        (0x00000200L)
#define D3DUSAGE_DMAP                           (0x00004000L)
#define D3DUSAGE_QUERY_LEGACYBUMPMAP            (0x00008000L)
#define D3DUSAGE_QUERY_SRGBREAD                 (0x00010000L)
#define D3DUSAGE_QUERY_FILTER                   (0x00020000L)
#define D3DUSAGE_QUERY_SRGBWRITE                (0x00040000L)
#define D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING (0x00080000L)
#define D3DUSAGE_QUERY_VERTEXTEXTURE            (0x00100000L)
#define D3DUSAGE_WRITEONLY                      (0x00000008L)
#define D3DUSAGE_SOFTWAREPROCESSING             (0x00000010L)
#define D3DUSAGE_DONOTCLIP                      (0x00000020L)
#define D3DUSAGE_POINTS                         (0x00000040L)
#define D3DUSAGE_RTPATCHES                      (0x00000080L)
#define D3DUSAGE_NPATCHES                       (0x00000100L)

// The following are not supported on Xbox 360:
//
//  D3DUSAGE_AUTOGENMIPMAP


/* CubeMap Face identifiers */
typedef enum _D3DCUBEMAP_FACES
{
    D3DCUBEMAP_FACE_POSITIVE_X     = 0,
    D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
    D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
    D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
    D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
    D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,

    D3DCUBEMAP_FACE_FORCE_DWORD    = 0x7fffffff
} D3DCUBEMAP_FACES;


/* Lock flags */

#define D3DLOCK_READONLY                0x00000010L
#define D3DLOCK_NOOVERWRITE             0x00001000L
#define D3DLOCK_NO_CPU_CACHE_FLUSH      0x00000001L      // Xbox 360 extension
#define D3DLOCK_NO_GPU_CACHE_INVALIDATE 0x00000002L      // Xbox 360 extension

#define D3DLOCK_DISCARD                 0x00002000L      // Ignored on Xbox 360
#define D3DLOCK_NOSYSLOCK               0x00000000L      // Ignored on Xbox 360
#define D3DLOCK_NO_DIRTY_UPDATE         0x00000000L      // Ignored on Xbox 360

// The following flags are not supported on Xbox 360:
//
//  D3DLOCK_DONOTWAIT

/* Vertex Buffer Description */
typedef struct _D3DVERTEXBUFFER_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;

    DWORD               FVF;

} D3DVERTEXBUFFER_DESC;

/* Index Buffer Description */
typedef struct _D3DINDEXBUFFER_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;
    UINT                Size;
} D3DINDEXBUFFER_DESC;


/* Surface Description */
typedef struct _D3DSURFACE_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;

    D3DMULTISAMPLE_TYPE MultiSampleType;
    DWORD               MultiSampleQuality;
    UINT                Width;
    UINT                Height;
} D3DSURFACE_DESC;

typedef struct _D3DVOLUME_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;

    UINT                Width;
    UINT                Height;
    UINT                Depth;
} D3DVOLUME_DESC;

typedef struct _D3DMIPTAIL_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    D3DPOOL             Pool;

    UINT                Width;
    UINT                Height;
    UINT                Depth;
    UINT                BaseLevel;
    UINT                Size;
} D3DMIPTAIL_DESC;

/* Structure for LockRect */
typedef struct _D3DLOCKED_RECT
{
    INT                 Pitch;
    void*               pBits;
} D3DLOCKED_RECT;

/* Structures for LockBox */
typedef struct _D3DBOX
{
    UINT                Left;
    UINT                Top;
    UINT                Right;
    UINT                Bottom;
    UINT                Front;
    UINT                Back;
} D3DBOX;

typedef struct _D3DLOCKED_BOX
{
    INT                 RowPitch;
    INT                 SlicePitch;
    void*               pBits;
} D3DLOCKED_BOX;

/* Structures for LockTail */
typedef struct _D3DLOCKED_TAIL
{
    INT                 RowPitch;
    INT                 SlicePitch;
    void*               pBits;
} D3DLOCKED_TAIL;

/*
 * Optional Resolve clear format overrides
 */

typedef struct _D3DRESOLVE_PARAMETERS {
    D3DFORMAT ColorFormat;
    INT       ColorExpBias;
    D3DFORMAT DepthFormat;
} D3DRESOLVE_PARAMETERS;

/* Structures for high order primitives */
typedef struct _D3DRECTPATCH_INFO
{
    UINT                StartVertexOffsetWidth;
    UINT                StartVertexOffsetHeight;
    UINT                Width;
    UINT                Height;
    UINT                Stride;
    D3DBASISTYPE        Basis;
    D3DDEGREETYPE       Degree;
} D3DRECTPATCH_INFO;

typedef struct _D3DTRIPATCH_INFO
{
    UINT                StartVertexOffset;
    UINT                NumVertices;
    D3DBASISTYPE        Basis;
    D3DDEGREETYPE       Degree;
} D3DTRIPATCH_INFO;

/* Adapter Identifier */

#define MAX_DEVICE_IDENTIFIER_STRING        512
typedef struct _D3DADAPTER_IDENTIFIER9
{
    char            Driver[MAX_DEVICE_IDENTIFIER_STRING];
    char            Description[MAX_DEVICE_IDENTIFIER_STRING];
    char            DeviceName[32];         /* Device name for GDI (ex. \\.\DISPLAY1) */

#ifdef _WIN32
    LARGE_INTEGER   DriverVersion;          /* Defined for 32 bit components */
#else
    DWORD           DriverVersionLowPart;   /* Defined for 16 bit driver components */
    DWORD           DriverVersionHighPart;
#endif

    DWORD           VendorId;
    DWORD           DeviceId;
    DWORD           SubSysId;
    DWORD           Revision;

    GUID            DeviceIdentifier;

    DWORD           WHQLLevel;

} D3DADAPTER_IDENTIFIER9;


/* Raster Status structure returned by GetRasterStatus */
typedef struct _D3DRASTER_STATUS
{
    BOOL            InVBlank;
    UINT            ScanLine;
} D3DRASTER_STATUS;

// Async feedback

typedef enum _D3DQUERYTYPE {
    D3DQUERYTYPE_VCACHE                 = 4, /* D3DISSUE_END */
    D3DQUERYTYPE_RESOURCEMANAGER        = 5, /* D3DISSUE_END */
    D3DQUERYTYPE_VERTEXSTATS            = 6, /* D3DISSUE_END */
    D3DQUERYTYPE_EVENT                  = 8, /* D3DISSUE_END */
    D3DQUERYTYPE_OCCLUSION              = 9, /* D3DISSUE_BEGIN, D3DISSUE_END */
    D3DQUERYTYPE_SCREENEXTENT           = 10, /* D3DISSUE_BEGIN, D3DISSUE_END, Xbox 360 Extension */
} D3DQUERYTYPE;

// Flags field for Issue
#define D3DISSUE_END (1 << 0) // Tells the runtime to issue the end of a query, changing it's state to "non-signaled".
#define D3DISSUE_BEGIN (1 << 1) // Tells the runtime to issue the beginng of a query.


// Flags field for GetData
#define D3DGETDATA_FLUSH (1 << 0) // Tells the runtime to flush if the query is outstanding.

typedef struct _D3DSCREENEXTENTSTATS
{
    DWORD MinX;
    DWORD MaxX;
    DWORD MinY;
    DWORD MaxY;
    float MinZ;
    float MaxZ;
} D3DSCREENEXTENTSTATS;

typedef struct _D3DOCCLUSIONSTATS
{
    DWORD ZPass;
    DWORD ZFail;
    DWORD StencilFail;
    DWORD Total;
} D3DOCCLUSIONSTATS;

typedef struct _D3DRESOURCESTATS
{
// Data collected since last Present()
    BOOL    bThrashing;             /* indicates if thrashing */
    DWORD   ApproxBytesDownloaded;  /* Approximate number of bytes downloaded by resource manager */
    DWORD   NumEvicts;              /* number of objects evicted */
    DWORD   NumVidCreates;          /* number of objects created in video memory */
    DWORD   LastPri;                /* priority of last object evicted */
    DWORD   NumUsed;                /* number of objects set to the device */
    DWORD   NumUsedInVidMem;        /* number of objects set to the device, which are already in video memory */
// Persistent data
    DWORD   WorkingSet;             /* number of objects in video memory */
    DWORD   WorkingSetBytes;        /* number of bytes in video memory */
    DWORD   TotalManaged;           /* total number of managed objects */
    DWORD   TotalBytes;             /* total number of bytes of managed objects */
} D3DRESOURCESTATS;

#define D3DRTYPECOUNT (D3DRTYPE_INDEXBUFFER+1)

typedef struct _D3DDEVINFO_RESOURCEMANAGER
{
    D3DRESOURCESTATS    stats[D3DRTYPECOUNT];
} D3DDEVINFO_RESOURCEMANAGER, *LPD3DDEVINFO_RESOURCEMANAGER;

typedef struct _D3DDEVINFO_D3DVERTEXSTATS
{
    DWORD   NumRenderedTriangles;       /* total number of triangles that are not clipped in this frame */
    DWORD   NumExtraClippingTriangles;  /* Number of new triangles generated by clipping */
} D3DDEVINFO_D3DVERTEXSTATS, *LPD3DDEVINFO_D3DVERTEXSTATS;

typedef struct _D3DDEVINFO_VCACHE
{
    DWORD   Pattern;                    /* bit pattern, return value must be FOUR_CC('C', 'A', 'C', 'H') */
    DWORD   OptMethod;                  /* optimization method 0 means longest strips, 1 means vertex cache based */
    DWORD   CacheSize;                  /* cache size to optimize for  (only required if type is 1) */
    DWORD   MagicNumber;                /* used to determine when to restart strips (only required if type is 1)*/
} D3DDEVINFO_VCACHE, *LPD3DDEVINFO_VCACHE;

typedef enum _D3DSCREENEXTENTQUERYMODE
{
    D3DSEQM_NONCULLED                      = 0,
    D3DSEQM_CULLED                         = 1,
    D3DSEQM_PRECLIP                        = 2,
} D3DSCREENEXTENTQUERYMODE;

typedef enum _D3DCOUNTER
{
    D3DCOUNTER_FRAMETIME                   = 0,
    D3DCOUNTER_FRAMESPERSECOND             = 1,
    D3DCOUNTER_FRAMECOUNT                  = 2,
    D3DCOUNTER_FRAME_BLOCKED_PERCENT       = 3,
    D3DCOUNTER_FRAME_THROTTLED_PERCENT     = 4,
    D3DCOUNTER_FRAME_ACTIVE_PERCENT        = 5,
    D3DCOUNTER_XAM_GPU_PERCENT             = 6,
    D3DCOUNTER_FRAME_GPU_IDLE_PERCENT      = 7,
    D3DCOUNTER_PRIMITIVES_CULLED_PERCENT   = 8,
    D3DCOUNTER_PRIMITIVES_CLIPPED_PERCENT  = 9,
    D3DCOUNTER_PRIMITIVES_VISIBLE_PERCENT  = 10,
    D3DCOUNTER_BANDWIDTH_SYSTEM            = 11,
    D3DCOUNTER_BANDWIDTH_INDICES           = 12,
    D3DCOUNTER_BANDWIDTH_TEXTURE           = 13,
    D3DCOUNTER_BANDWIDTH_VERTEX            = 14,
    D3DCOUNTER_BANDWIDTH_RESOLVE           = 15,
    D3DCOUNTER_BANDWIDTH_MEMEXPORT         = 16,
    D3DCOUNTER_XAM_COMMAND_BUFFER_SIZE     = 17,
    D3DCOUNTER_MAX                      // number of D3D Counters
} D3DCOUNTER;


//------------------------------------------------------------------------------
// GPU performance counters

// Flags for options
#define D3DPERFQUERY_WAITGPUIDLE 0x00000001L   // for GPU to wait unti idle before writing out counter values

// struct to hold perf counter selects
typedef struct _D3DPERFCOUNTER_EVENTS
{
    GPUPERFEVENT_CP     CP[1];
    GPUPERFEVENT_RBBM   RBBM[2];
    GPUPERFEVENT_SQ     SQ[4];
    GPUPERFEVENT_VGT    VGT[4];
    GPUPERFEVENT_VC     VC[4];
    GPUPERFEVENT_PA_SU  PA_SU[4];
    GPUPERFEVENT_PA_SC  PA_SC[4];
    GPUPERFEVENT_HZ     HZ[2];
    GPUPERFEVENT_TCR    TCR[2];  // texture cache return
    GPUPERFEVENT_TCM    TCM[2];  // texture cache memory
    GPUPERFEVENT_TCF    TCF[12]; // texture cache fetch
    GPUPERFEVENT_TP     TP0[2];
    GPUPERFEVENT_TP     TP1[2];
    GPUPERFEVENT_TP     TP2[2];
    GPUPERFEVENT_TP     TP3[2];
    GPUPERFEVENT_SX     SX[1];
    GPUPERFEVENT_BC     BC[4];
    GPUPERFEVENT_MC     MC0[1];
    GPUPERFEVENT_MC     MC1[1];
    GPUPERFEVENT_MH     MH[3];
    GPUPERFEVENT_BIF    BIF[1];
} D3DPERFCOUNTER_EVENTS;

// struct to hold timer values.
typedef struct _D3DPERFCOUNTER_VALUES
{
    ULARGE_INTEGER      CP[1];
    ULARGE_INTEGER      RBBM[2];
    ULARGE_INTEGER      SQ[4];
    ULARGE_INTEGER      VGT[4];
    ULARGE_INTEGER      VC[4];
    ULARGE_INTEGER      PA_SU[4];
    ULARGE_INTEGER      PA_SC[4];
    ULARGE_INTEGER      HZ[2];
    ULARGE_INTEGER      TCR[2];  // texture cache return
    ULARGE_INTEGER      TCM[2];  // texture cache memory
    ULARGE_INTEGER      TCF[12]; // texture cache fetch
    ULARGE_INTEGER      TP0[2];
    ULARGE_INTEGER      TP1[2];
    ULARGE_INTEGER      TP2[2];
    ULARGE_INTEGER      TP3[2];
    ULARGE_INTEGER      SX[1];
    ULARGE_INTEGER      BC[4];
    ULARGE_INTEGER      MC0[1];
    ULARGE_INTEGER      MC1[1];
    ULARGE_INTEGER      MH[3];
    ULARGE_INTEGER      BIF[1];
} D3DPERFCOUNTER_VALUES;

//------------------------------------------------------------------------------
// D3D Unbuffered Graphics API support
//
// The following macros and enums are used to reference GPU registers
// when interacting with the D3D Unbuffered Graphics APIs

#define D3DTAG_GROUP(Index, StartBit, Count) (((Index) << 16) | ((Count) << 8) | (StartBit))
#define D3DTAG_BIT(Index, Bit) ((Index << 16) | (1 << 8) | (Bit))
#define D3DTAG_INDEX(Tag) ((DWORD) (((Tag) >> 16)))
#define D3DTAG_COUNT(Tag) ((DWORD) ((Tag) >> 8) & 0xff)
#define D3DTAG_START(Tag) ((DWORD) ((Tag) & 0xff))

// EndBit is inclusive:

#define D3DTAG_MASKENCODE(StartBit, EndBit) \
    ((UINT64) ((INT64) 0x8000000000000000i64 >> ((EndBit) - (StartBit))) >> (StartBit))

// Enums that describe individual GPU registers or groups of GPU registers:

typedef enum _D3DTAG {

    D3DTAG_VERTEXSHADERCONSTANTS            = D3DTAG_GROUP(0, 0, 64),
    D3DTAG_PIXELSHADERCONSTANTS             = D3DTAG_GROUP(1, 0, 64),

    // The first 32 bits are used for fetch constants (note that
    // 'D3DTAG_FETCHCONSTANTS' is a union of the vertex and texture constant
    // groups):

    D3DTAG_FETCHCONSTANTS                   = D3DTAG_GROUP(2, 0, 32),
    D3DTAG_VERTEXFETCHCONSTANTS             = D3DTAG_GROUP(2, GPU_D3D_VERTEX_FETCH_CONSTANT_BASE, GPU_D3D_VERTEX_FETCH_CONSTANT_COUNT),
    D3DTAG_TEXTUREFETCHCONSTANTS            = D3DTAG_GROUP(2, GPU_D3D_TEXTURE_FETCH_CONSTANT_BASE, GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT),

    // The hardware requires all flow control constants to be set at once
    // so they only require one bit:

    D3DTAG_MISCCONSTANTS                    = D3DTAG_GROUP(2, 32, 2),
    D3DTAG_FLOWCONSTANTS                    = D3DTAG_BIT(2, 32),
    D3DTAG_RESERVED0,

    // The following groups must match the ordering of the corresponding hardware
    // state packets.

    // Packet 0, Destination:

    D3DTAG_DESTINATIONPACKET                = D3DTAG_GROUP(2, 34, 16),
    D3DTAG_SURFACEINFO                      = D3DTAG_BIT(2, 34),  // GPUREG_SURFACEINFO
    D3DTAG_COLOR0INFO,                                            // GPUREG_COLOR0INFO
    D3DTAG_DEPTHINFO,                                             // GPUREG_DEPTHINFO
    D3DTAG_COLOR1INFO,                                            // GPUREG_COLOR1INFO
    D3DTAG_COLOR2INFO,                                            // GPUREG_COLOR2INFO
    D3DTAG_COLOR3INFO,                                            // GPUREG_COLOR3INFO
    D3DTAG_COHERDESTBASE0,                                        // GPUREG_COHERDESTBASE0
    D3DTAG_COHERDESTBASE1,                                        // GPUREG_COHERDESTBASE1
    D3DTAG_COHERDESTBASE2,                                        // GPUREG_COHERDESTBASE2
    D3DTAG_COHERDESTBASE3,                                        // GPUREG_COHERDESTBASE3
    D3DTAG_COHERDESTBASE4,                                        // GPUREG_COHERDESTBASE4
    D3DTAG_COHERDESTBASE5,                                        // GPUREG_COHERDESTBASE5
    D3DTAG_COHERDESTBASE6,                                        // GPUREG_COHERDESTBASE6
    D3DTAG_COHERDESTBASE7,                                        // GPUREG_COHERDESTBASE7
    D3DTAG_SCREENSCISSORTL,                                       // GPUREG_SCREENSCISSORTL
    D3DTAG_SCREENSCISSORBR,                                       // GPUREG_SCREENSCISSORBR

    // Packet 1, Window:

    D3DTAG_WINDOWPACKET                     = D3DTAG_GROUP(2, 50, 3),
    D3DTAG_WINDOWOFFSET                     = D3DTAG_BIT(2, 50),  // GPUREG_WINDOWOFFSET
    D3DTAG_WINDOWSCISSORTL,                                       // GPUREG_WINDOWSCISSORTL
    D3DTAG_WINDOWSCISSORBR,                                       // GPUREG_WINDOWSCISSORBR

    // Packet 2, Values:

    D3DTAG_VALUESPACKET                     = D3DTAG_GROUP(3, 0, 21),
    D3DTAG_MAXVTXINDX                       = D3DTAG_BIT(3, 0),   // GPUREG_MAXVTXINDX
    D3DTAG_MINVTXINDX,                                            // GPUREG_MINVTXINDX
    D3DTAG_INDXOFFSET,                                            // GPUREG_INDXOFFSET
    D3DTAG_MULTIPRIMIBRESETINDX,                                  // GPUREG_MULTIPRIMIBRESETINDX
    D3DTAG_COLORMASK,                                             // GPUREG_COLORMASK
    D3DTAG_BLENDRED,                                              // GPUREG_BLENDRED
    D3DTAG_BLENDGREEN,                                            // GPUREG_BLENDGREEN
    D3DTAG_BLENDBLUE,                                             // GPUREG_BLENDBLUE
    D3DTAG_BLENDALPHA,                                            // GPUREG_BLENDALPHA
    D3DTAG_UNUSED_VALUES0,                                        // GPUREG_UNUSED0
    D3DTAG_UNUSED_VALUES1,                                        // GPUREG_UNUSED1
    D3DTAG_UNUSED_VALUES2,                                        // GPUREG_UNUSED2
    D3DTAG_STENCILREFMASKBF,                                      // GPUREG_STENCILREFMASKBF
    D3DTAG_STENCILREFMASK,                                        // GPUREG_STENCILREFMASK
    D3DTAG_ALPHAREF,                                              // GPUREG_ALPHAREF
    D3DTAG_VPORTXSCALE,                                           // GPUREG_VPORTXSCALE
    D3DTAG_VPORTXOFFSET,                                          // GPUREG_VPORTXOFFSET
    D3DTAG_VPORTYSCALE,                                           // GPUREG_VPORTYSCALE
    D3DTAG_VPORTYOFFSET,                                          // GPUREG_VPORTYOFFSET
    D3DTAG_VPORTZSCALE,                                           // GPUREG_VPORTZSCALE
    D3DTAG_VPORTZOFFSET,                                          // GPUREG_VPORTZOFFSET

    // Packet 3, Program:

    D3DTAG_PROGRAMPACKET                  = D3DTAG_GROUP(3, 21, 5),
    D3DTAG_PROGRAMCONTROL                 = D3DTAG_BIT(3, 21),    // GPUREG_PROGRAMCONTROL
    D3DTAG_CONTEXTMISC,                                           // GPUREG_CONTEXTMISC
    D3DTAG_INTERPOLATORCONTROL,                                   // GPUREG_INTERPOLATORCONTROL
    D3DTAG_WRAPPING0,                                             // GPUREG_WRAPPING0
    D3DTAG_WRAPPING1,                                             // GPUREG_WRAPPING1

    // Packet 4, Control:

    D3DTAG_CONTROLPACKET                  = D3DTAG_GROUP(3, 26, 12),
    D3DTAG_DEPTHCONTROL                   = D3DTAG_BIT(3, 26),    // GPUREG_DEPTHCONTROL
    D3DTAG_BLENDCONTROL0,                                         // GPUREG_BLENDCONTROL0
    D3DTAG_COLORCONTROL,                                          // GPUREG_COLORCONTROL
    D3DTAG_HICONTROL,                                             // GPUREG_HICONTROL
    D3DTAG_CLIPCONTROL,                                           // GPUREG_CLIPCONTROL
    D3DTAG_MODECONTROL,                                           // GPUREG_MODECONTROL
    D3DTAG_VTECONTROL,                                            // GPUREG_VTECONTROL
    D3DTAG_UNUSED_CONTROL0,                                       // GPUREG_UNUSED3
    D3DTAG_EDRAMMODECONTROL,                                      // GPUREG_EDRAMMODECONTROL
    D3DTAG_BLENDCONTROL1,                                         // GPUREG_BLENDCONTROL1
    D3DTAG_BLENDCONTROL2,                                         // GPUREG_BLENDCONTROL2
    D3DTAG_BLENDCONTROL3,                                         // GPUREG_BLENDCONTROL3

    // Packet 5, Tessellator:

    D3DTAG_TESSELLATORPACKET              = D3DTAG_GROUP(3, 38, 21),
    D3DTAG_POINTSIZE                      = D3DTAG_BIT(3, 38),    // GPUREG_POINTSIZE
    D3DTAG_POINTMINMAX,                                           // GPUREG_POINTMINMAX
    D3DTAG_LINECONTROL,                                           // GPUREG_LINECONTROL
    D3DTAG_LINESTIPPLE,                                           // GPUREG_UNUSED4
    D3DTAG_OUTPUTPATHCONTROL,                                     // GPUREG_OUTPUTPATHCONTROL
    D3DTAG_HOSCONTROL,                                            // GPUREG_HOSCONTROL
    D3DTAG_HOSMAXTESSLEVEL,                                       // GPUREG_HOSMAXTESSLEVEL
    D3DTAG_HOSMINTESSLEVEL,                                       // GPUREG_HOSMINTESSLEVEL
    D3DTAG_HOSREUSEDEPTH,                                         // GPUREG_HOSREUSEDEPTH
    D3DTAG_GROUPPRIMTYPE,                                         // GPUREG_GROUPPRIMTYPE
    D3DTAG_GROUPFIRSTDECR,                                        // GPUREG_GROUPFIRSTDECR
    D3DTAG_GROUPDECR,                                             // GPUREG_GROUPDECR
    D3DTAG_GROUPVECT0CONTROL,                                     // GPUREG_GROUPVECT0CONTROL
    D3DTAG_GROUPVECT1CONTROL,                                     // GPUREG_GROUPVECT1CONTROL
    D3DTAG_GROUPVECT0FMTCONTROL,                                  // GPUREG_GROUPVECT0FMTCONTROL
    D3DTAG_GROUPVECT1FMTCONTROL,                                  // GPUREG_GROUPVECT1FMTCONTROL
    D3DTAG_UNUSED_TESSELLATOR0,                                   // GPUREG_UNUSED5
    D3DTAG_UNUSED_TESSELLATOR1,                                   // GPUREG_UNUSED6
    D3DTAG_MPASSPSCONTROL,                                        // GPUREG_MPASSPSCONTROL
    D3DTAG_VIZQUERY,                                              // GPUREG_VIZQUERY
    D3DTAG_ENHANCE,                                               // GPUREG_ENHANCE

    // Packet 6, Misc:

    D3DTAG_MISCPACKET                     = D3DTAG_GROUP(4, 0, 38),
    D3DTAG_SCLINECONTROL                  = D3DTAG_BIT(4, 0) ,    // GPUREG_SCLINECONTROL
    D3DTAG_AACONFIG,                                              // GPUREG_AACONFIG
    D3DTAG_VTXCONTROL,                                            // GPUREG_VTXCONTROL
    D3DTAG_GBVERTCLIPADJ,                                         // GPUREG_GBVERTCLIPADJ
    D3DTAG_GBVERTDISCADJ,                                         // GPUREG_GBVERTDISCADJ
    D3DTAG_GBHORZCLIPADJ,                                         // GPUREG_GBHORZCLIPADJ
    D3DTAG_GBHORZDISCADJ,                                         // GPUREG_GBHORZDISCADJ
    D3DTAG_VSCONST,                                               // GPUREG_VSCONST
    D3DTAG_PSCONST,                                               // GPUREG_PSCONST
    D3DTAG_DEBUGMISC0,                                            // GPUREG_DEBUGMISC0
    D3DTAG_DEBUGMISC1,                                            // GPUREG_DEBUGMISC1
    D3DTAG_UNUSED_MISC0,                                          // GPUREG_UNUSED7
    D3DTAG_UNUSED_MISC1,                                          // GPUREG_UNUSED8
    D3DTAG_UNUSED_MISC2,                                          // GPUREG_UNUSED9
    D3DTAG_UNUSED_MISC3,                                          // GPUREG_UNUSED10
    D3DTAG_UNUSED_MISC4,                                          // GPUREG_UNUSED11
    D3DTAG_UNUSED_MISC5,                                          // GPUREG_UNUSED12
    D3DTAG_UNUSED_MISC6,                                          // GPUREG_UNUSED13
    D3DTAG_AAMASK,                                                // GPUREG_AAMASK
    D3DTAG_UNUSED_MISC7,                                          // GPUREG_UNUSED14
    D3DTAG_UNUSED_MISC8,                                          // GPUREG_UNUSED15
    D3DTAG_UNUSED_MISC9,                                          // GPUREG_UNUSED16
    D3DTAG_VERTEXREUSEBLOCKCONTROL,                               // GPUREG_VERTEXREUSEBLOCKCONTROL
    D3DTAG_OUTDEALLOCCONTROL,                                     // GPUREG_OUTDEALLOCCONTROL
    D3DTAG_COPYCONTROL,                                           // GPUREG_COPYCONTROL
    D3DTAG_COPYDESTBASE,                                          // GPUREG_COPYDESTBASE
    D3DTAG_COPYDESTPITCH,                                         // GPUREG_COPYDESTPITCH
    D3DTAG_COPYDESTINFO,                                          // GPUREG_COPYDESTINFO
    D3DTAG_HICLEAR,                                               // GPUREG_HICLEAR
    D3DTAG_DEPTHCLEAR,                                            // GPUREG_DEPTHCLEAR
    D3DTAG_COLORCLEAR,                                            // GPUREG_COLORCLEAR
    D3DTAG_COLORCLEARLO,                                          // GPUREG_COLORCLEARLO
    D3DTAG_COPYFUNC,                                              // GPUREG_COPYFUNC
    D3DTAG_COPYREF,                                               // GPUREG_COPYREF
    D3DTAG_COPYMASK,                                              // GPUREG_COPYMASK
    D3DTAG_COPYSURFACESLICE,                                      // GPUREG_COPYSURFACESLICE
    D3DTAG_SAMPLECOUNTCONTROL,                                    // GPUREG_SAMPLECOUNTCONTROL
    D3DTAG_SAMPLECOUNTADDRESS,                                    // GPUREG_SAMPLECOUNTADDRESS

    // Packet 7, Point:

    D3DTAG_POINTPACKET                    = D3DTAG_GROUP(4, 38, 8),
    D3DTAG_POLYOFFSETFRONTSCALE           = D3DTAG_BIT(4, 38),    // GPUREG_POLYOFFSETFRONTSCALE
    D3DTAG_POLYOFFSETFRONTOFFSET,                                 // GPUREG_POLYOFFSETFRONTOFFSET
    D3DTAG_POLYOFFSETBACKSCALE,                                   // GPUREG_POLYOFFSETBACKSCALE
    D3DTAG_POLYOFFSETBACKOFFSET,                                  // GPUREG_POLYOFFSETBACKOFFSET
    D3DTAG_POINTXRAD,                                             // GPUREG_POINTXRAD
    D3DTAG_POINTYRAD,                                             // GPUREG_POINTYRAD
    D3DTAG_POINTCONSTANTSIZE,                                     // GPUREG_POINTCONSTANTSIZE
    D3DTAG_POINTCULLRAD,                                          // GPUREG_POINTCULLRAD

    // Non-context state:

    D3DTAG_USERCLIPPLANES                 = D3DTAG_GROUP(4, 46, 6),
    D3DTAG_USERCLIPPLANE0                 = D3DTAG_BIT(4, 46),    // GPUREG_USERCLIPPLANE0
    D3DTAG_USERCLIPPLANE1,                                        // GPUREG_USERCLIPPLANE1
    D3DTAG_USERCLIPPLANE2,                                        // GPUREG_USERCLIPPLANE2
    D3DTAG_USERCLIPPLANE3,                                        // GPUREG_USERCLIPPLANE3
    D3DTAG_USERCLIPPLANE4,                                        // GPUREG_USERCLIPPLANE4
    D3DTAG_USERCLIPPLANE5,                                        // GPUREG_USERCLIPPLANE5

    // The following group of bits is reserved for internal use by D3D:

    D3DTAG_PRIVATE                        = D3DTAG_GROUP(4, 52, 12),

    // This terminator must always be at the end:

    D3DTAG_TERMINATOR,

} D3DTAG;

// Flags for flushing the Hi-Z/Hi-Stencil test results:

typedef enum _D3DFHZS_FLUSHTYPE
{
    D3DFHZS_ASYNCHRONOUS,   // Faster, but requires conservative Hi-Z/Hi-Stencil tests
    D3DFHZS_SYNCHRONOUS     // Slower, but can be used with non-conservative tests
} D3DFHZS_FLUSHTYPE;

#pragma pack()
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif

#if defined(_M_PPCBE)
#pragma bitfield_order(pop)
#endif

#endif /* (DIRECT3D_VERSION >= 0x0900) */

#endif /* _d3d9TYPES(P)_H_ */

