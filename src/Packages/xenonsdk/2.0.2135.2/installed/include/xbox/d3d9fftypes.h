/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d9fftypes.h
 *  Content:    Legacy Direct3D types used by the unsupported fixed function
 *              pipeline
 *
 ***************************************************************************/

#ifndef _d3d9FFTYPES_H_
#define _d3d9FFTYPES_H_

// Only define these items if explicitly asked for.
// (This is done to prevent the casual reintroduction
// of these types, when all the program wants is some
// other D3D fixed function data type.

#ifdef D3DCOMPILE_DEFINE_POSITIONT_AND_XYZRHW

#define D3DDECLUSAGE_POSITIONT ((D3DDECLUSAGE) 9)
#define D3DFVF_XYZRHW           0x004

#endif // D3DCOMPILE_DEFINE_POSITIONT_AND_XYZRHW

typedef struct _D3DMATERIAL9 {
    D3DCOLORVALUE   Diffuse;        /* Diffuse color RGBA */
    D3DCOLORVALUE   Ambient;        /* Ambient color RGB */
    D3DCOLORVALUE   Specular;       /* Specular 'shininess' */
    D3DCOLORVALUE   Emissive;       /* Emissive color RGB */
    float           Power;          /* Sharpness if specular highlight */
} D3DMATERIAL9;

typedef enum _D3DLIGHTTYPE {
    D3DLIGHT_POINT          = 1,
    D3DLIGHT_SPOT           = 2,
    D3DLIGHT_DIRECTIONAL    = 3,
    D3DLIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DLIGHTTYPE;

typedef struct _D3DLIGHT9 {
    D3DLIGHTTYPE    Type;            /* Type of light source */
    D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
    D3DCOLORVALUE   Specular;        /* Specular color of light */
    D3DCOLORVALUE   Ambient;         /* Ambient color of light */
    D3DVECTOR       Position;         /* Position in world space */
    D3DVECTOR       Direction;        /* Direction in world space */
    float           Range;            /* Cutoff range */
    float           Falloff;          /* Falloff */
    float           Attenuation0;     /* Constant attenuation */
    float           Attenuation1;     /* Linear attenuation */
    float           Attenuation2;     /* Quadratic attenuation */
    float           Theta;            /* Inner angle of spotlight cone */
    float           Phi;              /* Outer angle of spotlight cone */
} D3DLIGHT9;

typedef enum _D3DFOGMODE {
    D3DFOG_NONE                 = 0,
    D3DFOG_EXP                  = 1,
    D3DFOG_EXP2                 = 2,
    D3DFOG_LINEAR               = 3,
    D3DFOG_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} D3DFOGMODE;

typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTS_WORLD         = 0,
    D3DTS_VIEW          = 1,
    D3DTS_PROJECTION    = 2,
    D3DTS_TEXTURE0      = 3,
    D3DTS_TEXTURE1      = 4,
    D3DTS_TEXTURE2      = 5,
    D3DTS_TEXTURE3      = 6,
    D3DTS_TEXTURE4      = 7,
    D3DTS_TEXTURE5      = 8,
    D3DTS_TEXTURE6      = 9,
    D3DTS_TEXTURE7      = 10,

    D3DTS_MAX           = 11,
    D3DTS_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
} D3DTRANSFORMSTATETYPE;

#define D3DTS_WORLDMATRIX(index) (D3DTS_WORLD)

// Values for material source
typedef enum _D3DMATERIALCOLORSOURCE
{
    D3DMCS_MATERIAL         = 0,            // Color from material is used
    D3DMCS_COLOR1           = 1,            // Diffuse vertex color is used
    D3DMCS_COLOR2           = 2,            // Specular vertex color is used
    D3DMCS_FORCE_DWORD      = 0x7fffffff,   // force 32-bit size enum
} D3DMATERIALCOLORSOURCE;

/*
 * State enumerants for per-stage processing of fixed function pixel processing
 * Two of these affect fixed function vertex processing as well: TEXTURETRANSFORMFLAGS and TEXCOORDINDEX.
 */
typedef enum _D3DTEXTURESTAGESTATETYPE
{
    D3DTSS_COLOROP        =  1, /* D3DTEXTUREOP - per-stage blending controls for color channels */
    D3DTSS_COLORARG1      =  2, /* D3DTA_* (texture arg) */
    D3DTSS_COLORARG2      =  3, /* D3DTA_* (texture arg) */
    D3DTSS_ALPHAOP        =  4, /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
    D3DTSS_ALPHAARG1      =  5, /* D3DTA_* (texture arg) */
    D3DTSS_ALPHAARG2      =  6, /* D3DTA_* (texture arg) */
    D3DTSS_BUMPENVMAT00   =  7, /* float (bump mapping matrix) */
    D3DTSS_BUMPENVMAT01   =  8, /* float (bump mapping matrix) */
    D3DTSS_BUMPENVMAT10   =  9, /* float (bump mapping matrix) */
    D3DTSS_BUMPENVMAT11   = 10, /* float (bump mapping matrix) */
    D3DTSS_TEXCOORDINDEX  = 11, /* identifies which set of texture coordinates index this texture */
    D3DTSS_BUMPENVLSCALE  = 22, /* float scale for bump map luminance */
    D3DTSS_BUMPENVLOFFSET = 23, /* float offset for bump map luminance */
    D3DTSS_TEXTURETRANSFORMFLAGS = 24, /* D3DTEXTURETRANSFORMFLAGS controls texture transform */
    D3DTSS_COLORARG0      = 26, /* D3DTA_* third arg for triadic ops */
    D3DTSS_ALPHAARG0      = 27, /* D3DTA_* third arg for triadic ops */
    D3DTSS_RESULTARG      = 28, /* D3DTA_* arg for result (CURRENT or TEMP) */
    D3DTSS_CONSTANT       = 32, /* Per-stage constant D3DTA_CONSTANT */

    D3DTSS_MAX            = 33, /* Total number of texture stage states */

    D3DTSS_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
} D3DTEXTURESTAGESTATETYPE;

// Values, used with D3DTSS_TEXCOORDINDEX, to specify that the vertex data(position
// and normal in the camera space) should be taken as texture coordinates
// Low 16 bits are used to specify texture coordinate index, to take the WRAP mode from
//
#define D3DTSS_TCI_PASSTHRU                             0x00000000
#define D3DTSS_TCI_CAMERASPACENORMAL                    0x00010000
#define D3DTSS_TCI_CAMERASPACEPOSITION                  0x00020000
#define D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR          0x00030000
#define D3DTSS_TCI_SPHEREMAP                            0x00040000

/*
 * Enumerations for COLOROP and ALPHAOP texture blending operations set in
 * texture processing stage controls in D3DTSS.
 */
typedef enum _D3DTEXTUREOP
{
    // Control
    D3DTOP_DISABLE              = 1,      // disables stage
    D3DTOP_SELECTARG1           = 2,      // the default
    D3DTOP_SELECTARG2           = 3,

    // Modulate
    D3DTOP_MODULATE             = 4,      // multiply args together
    D3DTOP_MODULATE2X           = 5,      // multiply and  1 bit
    D3DTOP_MODULATE4X           = 6,      // multiply and  2 bits

    // Add
    D3DTOP_ADD                  =  7,   // add arguments together
    D3DTOP_ADDSIGNED            =  8,   // add with -0.5 bias
    D3DTOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
    D3DTOP_SUBTRACT             = 10,   // Arg1 - Arg2, with no saturation
    D3DTOP_ADDSMOOTH            = 11,   // add 2 args, subtract product
                                        // Arg1 + Arg2 - Arg1*Arg2
                                        // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    D3DTOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
    D3DTOP_BLENDTEXTUREALPHA    = 13, // texture alpha
    D3DTOP_BLENDFACTORALPHA     = 14, // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    D3DTOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
    D3DTOP_BLENDCURRENTALPHA    = 16, // by alpha of current color

    // Specular mapping
    D3DTOP_PREMODULATE            = 17,     // modulate with next texture before use
    D3DTOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
                                            // COLOROP only
    D3DTOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
                                            // COLOROP only
    D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
                                            // COLOROP only
    D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
                                            // COLOROP only

    // Bump mapping
    D3DTOP_BUMPENVMAP           = 22, // per pixel env map perturbation
    D3DTOP_BUMPENVMAPLUMINANCE  = 23, // with luminance channel

    // This can do either diffuse or specular bump mapping with correct input.
    // Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
    // where each component has been scaled and offset to make it signed.
    // The result is replicated into all four (including alpha) channels.
    // This is a valid COLOROP only.
    D3DTOP_DOTPRODUCT3          = 24,

    // Triadic ops
    D3DTOP_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
    D3DTOP_LERP                 = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

    D3DTOP_MAX                  = 27,   /* Total number of texture operations */

    D3DTOP_FORCE_DWORD = 0x7fffffff,
} D3DTEXTUREOP;

/*
 * Values for COLORARG0,1,2, ALPHAARG0,1,2, and RESULTARG texture blending
 * operations set in texture processing stage controls in D3DRENDERSTATE.
 */
#define D3DTA_SELECTMASK        0x0000000f  // mask for arg selector
#define D3DTA_DIFFUSE           0x00000000  // select diffuse color (read only)
#define D3DTA_CURRENT           0x00000001  // select stage destination register (read/write)
#define D3DTA_TEXTURE           0x00000002  // select texture color (read only)
#define D3DTA_TFACTOR           0x00000003  // select D3DRS_TEXTUREFACTOR (read only)
#define D3DTA_SPECULAR          0x00000004  // select specular color (read only)
#define D3DTA_TEMP              0x00000005  // select temporary register color (read/write)
#define D3DTA_CONSTANT          0x00000006  // select texture stage constant
#define D3DTA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define D3DTA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)

#define D3DTA_SELECTMAX         0x00000007
#define D3DTA_MODIFIERMAX       0x00000040

// The D3DVERTEXBLENDFLAGS type is used with D3DRS_VERTEXBLEND state.
//
typedef enum _D3DVERTEXBLENDFLAGS
{
    D3DVBF_DISABLE  = 0,     // Disable vertex blending
    D3DVBF_1WEIGHTS = 1,     // 2 matrix blending
    D3DVBF_2WEIGHTS = 2,     // 3 matrix blending
    D3DVBF_3WEIGHTS = 3,     // 4 matrix blending
    D3DVBF_TWEENING = 255,   // blending using D3DRS_TWEENFACTOR
    D3DVBF_0WEIGHTS = 256,   // one matrix is used with weight 1.0
    D3DVBF_FORCE_DWORD = 0x7fffffff, // force 32-bit size enum
} D3DVERTEXBLENDFLAGS;

typedef enum _D3DTEXTURETRANSFORMFLAGS {
    D3DTTFF_DISABLE         = 0,    // texture coordinates are passed directly
    D3DTTFF_COUNT1          = 1,    // rasterizer should expect 1-D texture coords
    D3DTTFF_COUNT2          = 2,    // rasterizer should expect 2-D texture coords
    D3DTTFF_COUNT3          = 3,    // rasterizer should expect 3-D texture coords
    D3DTTFF_COUNT4          = 4,    // rasterizer should expect 4-D texture coords
    D3DTTFF_PROJECTED       = 256,  // texcoords to be divided by COUNTth element
    D3DTTFF_FORCE_DWORD     = 0x7fffffff,
} D3DTEXTURETRANSFORMFLAGS;

#endif /* _d3d9FFTYPES(P)_H_ */
