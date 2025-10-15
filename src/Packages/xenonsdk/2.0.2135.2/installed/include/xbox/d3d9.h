/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d9.h
 *  Content:    Xbox 360 Direct3D include file
 *
 ****************************************************************************/

#ifndef _D3D9_H_
#define _D3D9_H_

#pragma warning( push )

// Disable unreferenced parameter, nameless struct/union, zero-sized
// array, structure padded due to __declspec(align()) warnings for this header.

#pragma warning( disable : 4100 4201 4200 4324 )

#define DIRECT3D_VERSION 0x0900
#define D3D_SDK_VERSION 0

/****************************************************************************
 *
 * D3DCOMPILE_USEVOIDS
 *
 * On Xbox 360, all D3D APIs (except Create* calls and some other exceptions)
 * always return S_OK.  D3D parameter checking is only done (on debug builds)
 * via assertions, so the API functions could really be declared 'void'
 * and have no return value, resulting in better clarity and potentially
 * slightly better performance.  But the PC version of DirectX returns
 * HRESULT values for all APIs, so for compatibility we declare them the
 * same way, by default.
 *
 * If you prefer to have the D3D APIs declared as void when there is no
 * return value other than S_OK, simply #define D3DCOMPILE_USEVOIDS before
 * including d3d9.h.
 *
 * Note that Create* APIs do memory allocations and so can return either
 * S_OK or E_OUTOFMEMORY.
 *
 ****************************************************************************/

#ifdef D3DCOMPILE_USEVOIDS
    #define D3DVOID void
    #define D3DVOIDRETURN return
#else
    #define D3DVOID HRESULT
    #define D3DVOIDRETURN return S_OK
#endif

#define D3DINLINE static __forceinline
#define D3DMINLINE __forceinline

#define D3DFASTCALL __fastcall
#define DECLSPEC_SELECTANY __declspec(selectany)

#ifdef _XBOX
    #define D3DRESTRICT __restrict
#else
    #define D3DRESTRICT
#endif

#include <stdlib.h>
#ifdef _XBOX
#include <PPCIntrinsics.h>
#endif

// Xbox 360 Direct3D internal native types.

typedef struct Direct3D                  Direct3D;
typedef struct D3DDevice                 D3DDevice;
typedef struct D3DStateBlock             D3DStateBlock;
typedef struct D3DVertexDeclaration      D3DVertexDeclaration;
typedef struct D3DVertexShader           D3DVertexShader;
typedef struct D3DPixelShader            D3DPixelShader;
typedef struct D3DResource               D3DResource;
typedef struct D3DBaseTexture            D3DBaseTexture;
typedef struct D3DTexture                D3DTexture;
typedef struct D3DVolumeTexture          D3DVolumeTexture;
typedef struct D3DCubeTexture            D3DCubeTexture;
typedef struct D3DArrayTexture           D3DArrayTexture;
typedef struct D3DVertexBuffer           D3DVertexBuffer;
typedef struct D3DIndexBuffer            D3DIndexBuffer;
typedef struct D3DSurface                D3DSurface;
typedef struct D3DVolume                 D3DVolume;
typedef struct D3DQuery                  D3DQuery;
typedef struct D3DPerfCounters           D3DPerfCounters;

// Compatibility typedefs.

#define IDirect3D9                       Direct3D
#define IDirect3DDevice9                 D3DDevice
#define IDirect3DStateBlock9             D3DStateBlock
#define IDirect3DVertexDeclaration9      D3DVertexDeclaration
#define IDirect3DVertexShader9           D3DVertexShader
#define IDirect3DPixelShader9            D3DPixelShader
#define IDirect3DResource9               D3DResource
#define IDirect3DBaseTexture9            D3DBaseTexture
#define IDirect3DTexture9                D3DTexture
#define IDirect3DVolumeTexture9          D3DVolumeTexture
#define IDirect3DCubeTexture9            D3DCubeTexture
#define IDirect3DArrayTexture9           D3DArrayTexture
#define IDirect3DVertexBuffer9           D3DVertexBuffer
#define IDirect3DIndexBuffer9            D3DIndexBuffer
#define IDirect3DSurface9                D3DSurface
#define IDirect3DVolume9                 D3DVolume
#define IDirect3DQuery9                  D3DQuery
#define IDirect3DPerfCounter9            D3DPerfCounters;

// Pointer typedefs.

typedef struct Direct3D                  *LPDIRECT3D9,                  *PDIRECT3D9;
typedef struct D3DDevice                 *LPDIRECT3DDEVICE9,            *PDIRECT3DDEVICE9;
typedef struct D3DStateBlock             *LPDIRECT3DSTATEBLOCK9,        *PDIRECT3DSTATEBLOCK9;
typedef struct D3DVertexDeclaration      *LPDIRECT3DVERTEXDECLARATION9, *PDIRECT3DVERTEXDECLARATION9;
typedef struct D3DVertexShader           *LPDIRECT3DVERTEXSHADER9,      *PDIRECT3DVERTEXSHADER9;
typedef struct D3DPixelShader            *LPDIRECT3DPIXELSHADER9,       *PDIRECT3DPIXELSHADER9;
typedef struct D3DResource               *LPDIRECT3DRESOURCE9,          *PDIRECT3DRESOURCE9;
typedef struct D3DBaseTexture            *LPDIRECT3DBASETEXTURE9,       *PDIRECT3DBASETEXTURE9;
typedef struct D3DTexture                *LPDIRECT3DTEXTURE9,           *PDIRECT3DTEXTURE9;
typedef struct D3DVolumeTexture          *LPDIRECT3DVOLUMETEXTURE9,     *PDIRECT3DVOLUMETEXTURE9;
typedef struct D3DCubeTexture            *LPDIRECT3DCUBETEXTURE9,       *PDIRECT3DCUBETEXTURE9;
typedef struct D3DArrayTexture           *LPDIRECT3DARRAYTEXTURE9,      *PDIRECT3DARRAYTEXTURE9;
typedef struct D3DVertexBuffer           *LPDIRECT3DVERTEXBUFFER9,      *PDIRECT3DVERTEXBUFFER9;
typedef struct D3DIndexBuffer            *LPDIRECT3DINDEXBUFFER9,       *PDIRECT3DINDEXBUFFER9;
typedef struct D3DSurface                *LPDIRECT3DSURFACE9,           *PDIRECT3DSURFACE9;
typedef struct D3DVolume                 *LPDIRECT3DVOLUME9,            *PDIRECT3DVOLUME9;
typedef struct D3DQuery                  *LPDIRECT3DQUERY9,             *PDIRECT3DQUERY9;
typedef struct D3DPerfCounters           *LPDIRECT3DPERFCOUNTERS9,      *PDIRECT3DPERFCOUNTERS9;

// IDirect3DSwapChain9 is not supported on Xbox 360


#include "d3d9types.h"
#include "d3d9caps.h"
#include "d3d9xps.h"


#ifdef __cplusplus
extern "C" {
#endif

D3DINLINE Direct3D* WINAPI Direct3DCreate9(UINT SDKVersion) { return (Direct3D*) 1; }

/****************************************************************************
 *
 * Parameter for Direct3D Enum and GetCaps9 functions to get the info for
 * the current mode only.
 *
 ****************************************************************************/

#define D3DCURRENT_DISPLAY_MODE                 0x00EFFFFFL

/****************************************************************************
 *
 * Flags for Direct3D::CreateDevice's BehaviorFlags
 *
 ****************************************************************************/

#define D3DCREATE_PUREDEVICE                    0x00000000L     // Ignored on Xbox 360
#define D3DCREATE_FPU_PRESERVE                  0x00000000L     // Ignored on Xbox 360
#define D3DCREATE_HARDWARE_VERTEXPROCESSING     0x00000000L     // Ignored on Xbox 360
#define D3DCREATE_BUFFER_2_FRAMES               0x00000001L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_0            0x01000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_1            0x02000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_2            0x04000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_3            0x08000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_4            0x10000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_THREAD_ON_5            0x20000000L     // Xbox 360 extension
#define D3DCREATE_CREATE_NO_THREADS             0x00000100L     // Xbox 360 extension

// The following flags are not supported on Xbox 360:
//
//  D3DCREATE_MULTITHREADED
//  D3DCREATE_SOFTWARE_VERTEXPROCESSING
//  D3DCREATE_MIXED_VERTEXPROCESSING
//  D3DCREATE_DISABLE_DRIVER_MANAGEMENT
//  D3DCREATE_ADAPTERGROUP_DEVICE

/****************************************************************************
 *
 * Parameter for Direct3D::CreateDevice's Adapter
 *
 ****************************************************************************/

#define D3DADAPTER_DEFAULT                     0

/****************************************************************************
 *
 * Flags for Direct3D::EnumAdapters
 *
 ****************************************************************************/

#define D3DENUM_NO_WHQL_LEVEL                   0x00000002L

/****************************************************************************
 *
 * Maximum number of back-buffers supported in DX9
 *
 ****************************************************************************/

#define D3DPRESENT_BACK_BUFFERS_MAX             2L

/****************************************************************************
 *
 * Flags for IDirect3DDevice9::SetGammaRamp
 *
 ****************************************************************************/

#define D3DSGR_NO_CALIBRATION                   0x00000000L
#define D3DSGR_IMMEDIATE                        0x00000002L

/*
 *  DirectDraw error codes
 */

#define _FACD3D  0x876
#define MAKE_D3DHRESULT( code )  MAKE_HRESULT( 1, _FACD3D, code )
#define MAKE_D3DSTATUS( code )  MAKE_HRESULT( 0, _FACD3D, code )

/*
 * Direct3D Errors
 */

#define D3D_OK                                  S_OK

#define D3DERR_WRONGTEXTUREFORMAT               MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION        MAKE_D3DHRESULT(2073)
#define D3DERR_UNSUPPORTEDCOLORARG              MAKE_D3DHRESULT(2074)
#define D3DERR_UNSUPPORTEDALPHAOPERATION        MAKE_D3DHRESULT(2075)
#define D3DERR_UNSUPPORTEDALPHAARG              MAKE_D3DHRESULT(2076)
#define D3DERR_TOOMANYOPERATIONS                MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGTEXTUREFILTER         MAKE_D3DHRESULT(2078)
#define D3DERR_UNSUPPORTEDFACTORVALUE           MAKE_D3DHRESULT(2079)
#define D3DERR_CONFLICTINGRENDERSTATE           MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER         MAKE_D3DHRESULT(2082)
#define D3DERR_DRIVERINTERNALERROR              MAKE_D3DHRESULT(2087)

#define D3DERR_NOTFOUND                         MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA                         MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST                       MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET                   MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE                     MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY                 MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE                    MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL                      MAKE_D3DHRESULT(2156)
#define D3DOK_NOAUTOGEN                         MAKE_D3DSTATUS(2159)

/****************************************************************************
 *
 * __declspec(selectany) has the lovely attribute that it allows the linker
 * to remove duplicate instantiations of global declarations, and to remove
 * the instantiation entirely if unreferenced.
 *
 ****************************************************************************/

#define D3DCONST extern CONST DECLSPEC_SELECTANY

/****************************************************************************
 *
 * Private internal data - Please don't access these directly, as they're
 *                         subject to change.
 *
 ****************************************************************************/

D3DCONST UINT D3DPRIMITIVEVERTEXCOUNT[][2] =
{
    {0, 0},         // Illegal             = 0
    {1, 0},         // D3DPT_POINTLIST     = 1,
    {2, 0},         // D3DPT_LINELIST      = 2,
    {1, 1},         // D3DPT_LINESTRIP     = 3,
    {3, 0},         // D3DPT_TRIANGLELIST  = 4,
    {1, 2},         // D3DPT_TRIANGLEFAN   = 5,
    {1, 2},         // D3DPT_TRIANGLESTRIP = 6,
    {0, 0},         // Illegal             = 7
    {3, 0},         // D3DPT_RECTLIST      = 8
    {0, 0},         // Illegal             = 9
    {0, 0},         // Illegal             = 10
    {0, 0},         // Illegal             = 11
    {0, 0},         // Illegal             = 12
    {4, 0},         // D3DPT_QUADLIST      = 13,
};

// Macro for converting from primitive count to number of vertices.  When
// 'PrimitiveType' is specified as a constant, the table lookups are done
// at compile time.
//
#define D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount) \
    ((PrimitiveCount) * D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][0] \
     + D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][1])

// Macro for converting from vertex count to number of primitives.  When
// 'PrimitiveType' is specified as a constant, the table lookups are done
// at compile time.
//
#define D3DPRIMCOUNT(PrimitiveType, VertexCount) \
    (((VertexCount) - D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][1]) \
    / D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][0])

// The equivalent table for tessellated primitives
D3DCONST UINT D3DTESSPRIMITIVEVERTEXCOUNT[][2] =
{
    {0, 0},         // Illegal              = 0
    {0, 0},         // Illegal              = 1,
    {2, 0},         // D3DTPT_LINELIST      = 2,
    {1, 1},         // D3DTPT_LINESTRIP     = 3,
    {3, 0},         // D3DTPT_TRIANGLELIST  = 4,
    {1, 2},         // D3DTPT_TRIANGLEFAN   = 5,
    {1, 2},         // D3DTPT_TRIANGLESTRIP = 6,
    {0, 0},         // Illegal              = 7
    {0, 0},         // Illegal              = 8
    {0, 0},         // Illegal              = 9
    {0, 0},         // Illegal              = 10
    {0, 0},         // Illegal              = 11
    {0, 0},         // Illegal              = 12
    {4, 0},         // D3DTPT_QUADLIST      = 13,
    {0, 0},         // Illegal              = 14
    {0, 0},         // Illegal              = 15
    {1, 0},         // D3DTPT_LINEPATCH     = 16,
    {1, 0},         // D3DTPT_TRIPATCH      = 17,
    {1, 0},         // D3DTPT_RECTPATCH     = 18,
};

// Macro for converting from primitive count to number of vertices.  When
// 'PrimitiveType' is specified as a constant, the table lookups are done
// at compile time.
//
#define D3DTESSVERTEXCOUNT(PrimitiveType, PrimitiveCount) \
    ((PrimitiveCount) * D3DTESSPRIMITIVEVERTEXCOUNT[PrimitiveType][0] + \
     + D3DTESSPRIMITIVEVERTEXCOUNT[PrimitiveType][1])

// Macro for converting from vertex count to number of primitives.  When
// 'PrimitiveType' is specified as a constant, the table lookups are done
// at compile time.
//
#define D3DTESSPRIMCOUNT(PrimitiveType, VertexCount) \
    (((VertexCount) - D3DTESSPRIMITIVEVERTEXCOUNT[PrimitiveType][1]) \
    / D3DTESSPRIMITIVEVERTEXCOUNT[PrimitiveType][0])

// 'Extern' declaration that is both C and C++ friendly.
//
#ifdef __cplusplus
    #define D3DEXTERN extern "C"
#else
    #define D3DEXTERN extern
#endif

/****************************************************************************
 *
 * Definitions for public fields in Xbox 360's D3D objects
 *
 ****************************************************************************/

// Structure for maintaining D3D's shadow of all GPU constants.
//
typedef struct _D3DConstants
{
    union {
        GPUFETCH_CONSTANT Fetch[GPU_FETCH_CONSTANTS];
        struct {
            GPUTEXTURE_FETCH_CONSTANT TextureFetch[GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT];
            GPUVERTEX_FETCH_CONSTANT VertexFetch[3*GPU_D3D_VERTEX_FETCH_CONSTANT_COUNT];
        };
    };
    union {
        D3DVECTOR4 Alu[GPU_ALU_CONSTANTS];
        struct {
            D3DVECTOR4 VertexShaderF[GPU_D3D_VERTEX_CONSTANTF_COUNT];
            D3DVECTOR4 PixelShaderF[GPU_D3D_PIXEL_CONSTANTF_COUNT];
        };
    };
    union {
        DWORD Flow[GPU_FLOW_CONSTANTS];
        struct {
            DWORD VertexShaderB[GPU_D3D_VERTEX_CONSTANTB_COUNT/32];
            DWORD PixelShaderB[GPU_D3D_PIXEL_CONSTANTB_COUNT/32];
            DWORD VertexShaderI[GPU_D3D_VERTEX_CONSTANTI_COUNT];
            DWORD PixelShaderI[GPU_D3D_PIXEL_CONSTANTI_COUNT];
        };
    };
} D3DConstants;

// Function prototypes
//
typedef void (WINAPI *D3DSETRENDERSTATECALL)(D3DDevice* pDevice, DWORD Value);
typedef void (WINAPI *D3DSETSAMPLERSTATECALL)(D3DDevice* pDevice, DWORD Sampler, DWORD Value);

typedef DWORD (WINAPI *D3DGETRENDERSTATECALL)(D3DDevice* pDevice);
typedef DWORD (WINAPI *D3DGETSAMPLERSTATECALL)(D3DDevice* pDevice, DWORD Sampler);

// Helpers for decoding ownership of GPU registers
//
D3DINLINE DWORD D3DTag_Index(D3DTAG Tag)
{
    return D3DTAG_INDEX(Tag);
}
D3DINLINE DWORD D3DTag_Count(D3DTAG Tag)
{
    return D3DTAG_COUNT(Tag);
}
D3DINLINE UINT64 D3DTag_SubsetMask(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex)
{
    return D3DTAG_MASKENCODE(D3DTAG_START(Tag) + StartIndex, D3DTAG_START(Tag) + EndIndex);
}
D3DINLINE UINT64 D3DTag_Mask(D3DTAG Tag)
{
    return D3DTag_SubsetMask(Tag, 0, D3DTAG_COUNT(Tag) - 1);
}
D3DINLINE UINT64 D3DTag_ShaderConstantMask(DWORD StartConstant, DWORD ConstantCount)
{
    // D3DTAG_PIXELSHADERCONSTANTS vs D3DTAG_VERTEXSHADERCONSTANTS doesn't matter here
    return D3DTag_SubsetMask(D3DTAG_VERTEXSHADERCONSTANTS, StartConstant / 4, (StartConstant + ConstantCount - 1) / 4);
}
D3DINLINE UINT64 D3DTag_SamplerMask(DWORD Sampler)
{
    return D3DTag_SubsetMask(D3DTAG_TEXTUREFETCHCONSTANTS, Sampler, Sampler);
}

// Structure representing every context register on the GPU
typedef struct _D3DTAGCOLLECTION
{
    UINT64 m_Mask[D3DTAG_INDEX(D3DTAG_TERMINATOR) + 1];

} D3DTAGCOLLECTION;

D3DINLINE void D3DTagCollection_Set(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    pCollection->m_Mask[Index] |= Mask;
}
D3DINLINE void D3DTagCollection_Clear(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    pCollection->m_Mask[Index] &= ~Mask;
}
D3DINLINE BOOL D3DTagCollection_TestAny(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    return (pCollection->m_Mask[Index] & Mask) != 0;
}
D3DINLINE BOOL D3DTagCollection_TestAll(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    return (pCollection->m_Mask[Index] & Mask) == Mask;
}

D3DINLINE void D3DTagCollection_SetTag(D3DTAGCOLLECTION* pCollection, D3DTAG Tag)
{
    D3DTagCollection_Set(pCollection, D3DTag_Index(Tag), D3DTag_Mask(Tag));
}

/****************************************************************************
 *
 * Publicly accessible globals - Feel free to modify the values.
 *
 ****************************************************************************/

// Set D3D__NullHardware to TRUE to enable infinitely fast hardware (so fast
// you can't even see the results).  This is useful for determining how CPU-
// bound your program is.
//
D3DEXTERN BOOL D3D__NullHardware;

// D3D__SingleStepper is no longer used.
//
D3DEXTERN BOOL D3D__SingleStepper;

// Set D3D__BreakOnDeprecation to TRUE on debug versions of D3D to cause D3D
// to break into the debugger when warning about deprecated D3D functionality.
//
D3DEXTERN BOOL D3D__BreakOnDeprecation;

/****************************************************************************
 *
 * Miscellaneous public defines
 *
 ****************************************************************************/

// Types of our various callback functions.
//
typedef void (*D3DCALLBACK)(DWORD Context);
typedef void (*D3DVBLANKCALLBACK)(D3DVBLANKDATA *pData);
typedef void (*D3DHANGCALLBACK)(const char* pDescription);

/*
 * D3DQuery, IDirect3DQuery9 interface
 *
 * There are no public fields
 */

#ifdef __cplusplus

struct D3DQuery
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DQUERYTYPE    WINAPI GetType();
    DWORD           WINAPI GetDataSize();
    D3DVOID         WINAPI Issue(DWORD IssueFlags);
    HRESULT         WINAPI GetData(void* pData, DWORD Size, DWORD GetDataFlags);
};

#endif __cplusplus


/*
 * D3DPerfCounters, IDirect3DPerfCounter9 interface
 *
 */

#ifdef __cplusplus

struct D3DPerfCounters
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    BOOL            WINAPI IsBusy();
    void            WINAPI BlockUntilNotBusy();
    UINT            WINAPI GetNumPasses();
    HRESULT         WINAPI GetValues(D3DPERFCOUNTER_VALUES* pValues, UINT InstanceIndex, DWORD* pMask);
};

#endif __cplusplus



/*
 * D3DStateBlock, IDirect3DStateBlock9 interface
 *
 * There are no public fields
 */

#ifdef __cplusplus

struct D3DStateBlock
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DVOID         WINAPI Capture();
    D3DVOID         WINAPI Apply();
};

#endif __cplusplus

/*
 * D3DVertexDeclaration, IDirect3DVertexDeclaration9 interface
 *
 * There are no public fields
 */

#ifdef __cplusplus

struct D3DVertexDeclaration
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DVOID         WINAPI GetDeclaration(D3DVERTEXELEMENT9 *pDecl, UINT *pNumElements);
};

#endif __cplusplus

/*
 * D3DVertexShader, IDirect3DVertexShader9 interface
 *
 * There are no public fields
 */

#ifdef __cplusplus

struct D3DVertexShader
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DVOID         WINAPI GetFunction(void *pData, UINT* pSizeOfData);
};

#endif __cplusplus

/*
 * D3DPixelShader, IDirect3DPixelShader9 interface
 *
 * There are no public fields
 */

#ifdef __cplusplus

struct D3DPixelShader
{
    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DVOID         WINAPI GetFunction(void *pData, UINT* pSizeOfData);
};

#endif __cplusplus

/*
 * D3DResource, IDirect3DResource9 interface
 *
 * The root structure of all D3D 'resources' such as textures and vertex buffers.
 */

#define D3DCOMMON_REFCOUNT_MASK             0x000000FF

#define D3DCOMMON_LOCKID_MASK               0x00000F00
#define D3DCOMMON_LOCKID_SHIFT              8

#define D3DCOMMON_LOCKCOUNT_MASK            0x0000F000
#define D3DCOMMON_LOCKCOUNT_SHIFT           12

#define D3DCOMMON_TYPE_MASK                 0x00070000
#define D3DCOMMON_TYPE_SHIFT                16
#define D3DCOMMON_TYPE_VERTEXBUFFER         0x00010000
#define D3DCOMMON_TYPE_INDEXBUFFER          0x00020000
#define D3DCOMMON_TYPE_TEXTURE              0x00030000
#define D3DCOMMON_TYPE_SURFACE              0x00040000
#define D3DCOMMON_TYPE_RESERVED             0x00070000

#define D3DCOMMON_INTREFCOUNT_MASK          0x00F80000
#define D3DCOMMON_INTREFCOUNT_SHIFT         19

// Internal flag to indicate that this resource was created by Direct3D.
//
#define D3DCOMMON_D3DCREATED                0x01000000

// This internal flag indicates that this resource is a surface that
// was derived from a texture (via something like GetSurfaceLevel).
// As such, it doesn't own the memory to which it's pointing, and
// it can't be set as a render target.
//
#define D3DCOMMON_SURFACEFROMTEXTURE        0x02000000

// When accessing this resource via the CPU, the CPU uses a cached memory
// view.  D3D ensures coherency with the GPU by flushing the modified
// range at Unlock time.
//
#define D3DCOMMON_CPU_CACHED_MEMORY         0x40000000

// The rest of the bits may be used by derived classes.
//
#define D3DCOMMON_UNUSED_MASK               0x80000000
#define D3DCOMMON_UNUSED_SHIFT              31

// Initialize the 'BaseFlush' and 'MipFlush' fields to this value.
//
#define D3DFLUSH_INITIAL_VALUE              0xffff0000

struct D3DResource
{

#ifdef __cplusplus

    ULONG           WINAPI AddRef();
    ULONG           WINAPI Release();

    D3DVOID         WINAPI GetDevice(D3DDevice **ppDevice);
    D3DRESOURCETYPE WINAPI GetType();
    BOOL            WINAPI IsBusy();
    void            WINAPI BlockUntilNotBusy();

    // The following methods are not supported on Xbox 360:
    //
    //  SetPrivateData
    //  GetPrivateData
    //  FreePrivateData
    //  SetPriority
    //  GetPriority
    //  PreLoad

#endif __cplusplus

    // All resources need these fields.  Inherit them in C++.

    DWORD Common;           // Reference count and flags common to all resources
    DWORD Fence;            // This is the fence number of the last ring buffer
                            //   reference to this resource.  (This field was
                            //   known as 'Lock' on the original Xbox.)
                            //   Initialize it to zero.
    DWORD BaseFlush;        // Encodes the memory range to be flushed by D3D
                            //   via 'dcbf' at 'Unlock' time.  Initialize it
                            //   to D3DFLUSH_INITIAL_VALUE.
};

/*
 * D3DBaseTexture interface
 *
 * The root structure of all D3D textures.  Inherits all of the methods
 * from D3DResource.
 */

#define D3DTEXTURE_ALIGNMENT 4096

struct D3DBaseTexture
    #ifdef __cplusplus
        : public D3DResource
    #endif
{

#ifdef __cplusplus
    DWORD           WINAPI GetLevelCount();
    D3DVOID         WINAPI GetTailDesc(D3DMIPTAIL_DESC *pDesc);
    D3DVOID         WINAPI LockTail(UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
    D3DVOID         WINAPI UnlockTail(UINT ArrayIndex);
#endif

    // The following methods are not supported on Xbox 360:
    //
    // SetLOD
    // GetLOD
    // SetAutoGenFilterType
    // GetAutoGenFilterType
    // GenerateMipSubLevels

#ifndef __cplusplus
    // Manually inherit these from D3DResource
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
#endif

    DWORD MipFlush;         // Encodes the mip's memory range to be flushed by D3D
                            //   via 'dcbf' at 'Unlock' time.  Initialize it
                            //   to D3DFLUSH_INITIAL_VALUE.
    GPUTEXTURE_FETCH_CONSTANT Format;
};


/*
 * D3DTexture, IDirect3DTexture9 interface
 *
 * A normal texture.  Inherits from D3DBaseTexture
 */

struct D3DTexture
    #ifdef __cplusplus
        : public D3DBaseTexture
    #endif
{

#ifdef __cplusplus
    D3DVOID         WINAPI GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);
    HRESULT         WINAPI GetSurfaceLevel(UINT Level, D3DSurface **ppSurfaceLevel);
    D3DVOID         WINAPI LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
    D3DVOID         WINAPI UnlockRect(UINT Level);
    D3DVOID         WINAPI LockTail(D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
    D3DVOID         WINAPI UnlockTail();

    // The following methods are not supported on Xbox 360:
    //
    // AddDirtyRect

#endif

#ifndef __cplusplus
    // Manually inherit these from D3DBaseTexture
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
    DWORD MipFlush;
    GPUTEXTURE_FETCH_CONSTANT Format;
#endif

};


/*
 * D3DVolumeTexture, IDirect3DVolumeTexture9 interface
 *
 * A volume texture.  Inherits from D3DBaseTexture
 */

struct D3DVolumeTexture
    #ifdef __cplusplus
        : public D3DBaseTexture
    #endif
{

#ifdef __cplusplus

    D3DVOID         WINAPI GetLevelDesc(UINT Level, D3DVOLUME_DESC *pDesc);
    HRESULT         WINAPI GetVolumeLevel(UINT Level, D3DVolume **ppVolumeLevel);
    D3DVOID         WINAPI LockBox(UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags);
    D3DVOID         WINAPI UnlockBox(UINT Level);
    D3DVOID         WINAPI LockTail(D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
    D3DVOID         WINAPI UnlockTail();

    // The following methods are not supported on Xbox 360:
    //
    // AddDirtyBox

#endif

#ifndef __cplusplus
    // Manually inherit these from D3DBaseTexture
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
    DWORD MipFlush;
    GPUTEXTURE_FETCH_CONSTANT Format;
#endif

};


/*
 * D3DCubeTexture, IDirect3DCubeTexture9 interface
 *
 * A cube texture.  Inherits from D3DBaseTexture
 */

struct D3DCubeTexture
    #ifdef __cplusplus
        : public D3DBaseTexture
    #endif
{

#ifdef __cplusplus
    D3DVOID         WINAPI GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);
    HRESULT         WINAPI GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, D3DSurface **ppCubeMapSurface);
    D3DVOID         WINAPI LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
    D3DVOID         WINAPI UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level);
    D3DVOID         WINAPI LockTail(D3DCUBEMAP_FACES FaceType, D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
    D3DVOID         WINAPI UnlockTail(D3DCUBEMAP_FACES FaceType);

    // The following methods are not supported on Xbox 360:
    //
    // AddDirtyRect

#endif

#ifndef __cplusplus
    // Manually inherit these from D3DBaseTexture
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
    DWORD MipFlush;
    GPUTEXTURE_FETCH_CONSTANT Format;
#endif

};


/*
 * D3DArrayTexture, IDirect3DArrayTexture9 interface
 *
 * An array texture.  Inherits from D3DBaseTexture
 */

struct D3DArrayTexture
    #ifdef __cplusplus
        : public D3DBaseTexture
    #endif
{

#ifdef __cplusplus
    DWORD           WINAPI GetArraySize();
    D3DVOID         WINAPI GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);
    HRESULT         WINAPI GetArraySurface(UINT ArrayIndex, UINT Level, D3DSurface **ppArraySurface);
    D3DVOID         WINAPI LockRect(UINT ArrayIndex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
    D3DVOID         WINAPI UnlockRect(UINT ArrayIndex, UINT Level);
    D3DVOID         WINAPI LockTail(UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
    D3DVOID         WINAPI UnlockTail(UINT ArrayIndex);

    // The following methods are not supported on Xbox 360:
    //
    // AddDirtyRect

#endif

#ifndef __cplusplus
    // Manually inherit these from D3DBaseTexture
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
    DWORD MipFlush;
    GPUTEXTURE_FETCH_CONSTANT Format;
#endif

};


/*
 * D3DVertexBuffer, IDirect3DVertexBuffer9 interface
 *
 * A vertex buffer.
 *
 * The data for the vertex buffer must be aligned on a D3DVERTEXBUFFER_ALIGNMENT
 * byte multiple.
 */

#define D3DVERTEXBUFFER_ALIGNMENT 4

struct D3DVertexBuffer
    #ifdef __cplusplus
        : public D3DResource
    #endif
{
#ifdef __cplusplus

    D3DVOID         WINAPI Lock(UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags);
    D3DVOID         WINAPI Unlock();
    D3DVOID         WINAPI GetDesc(D3DVERTEXBUFFER_DESC *pDesc);

#endif __cplusplus

#ifndef __cplusplus
    // Manually inherit these from D3DResource
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
#endif

    GPUVERTEX_FETCH_CONSTANT Format;
};


/*
 * D3DIndexBuffer, IDirect3DIndexBuffer9 interface
 *
 * An index buffer.
 *
 * The data for the vertex buffer must be aligned on a D3DINDEXBUFFER_ALIGNMENT
 * byte multiple.
 */

#define D3DINDEXBUFFER_ALIGNMENT 4

// If set in the 'Common' field, this bit indicates that the indices are
// 32-bit values instead of 16-bit:

#define D3DINDEXBUFFER_INDEX32 0x80000000

struct D3DIndexBuffer
    #ifdef __cplusplus
        : public D3DResource
    #endif
{
#ifdef __cplusplus

    D3DVOID         WINAPI Lock(UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags);
    D3DVOID         WINAPI Unlock();
    D3DVOID         WINAPI GetDesc(D3DINDEXBUFFER_DESC *pDesc);

#endif __cplusplus

#ifndef __cplusplus
    // Manually inherit these from D3DResource
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
#endif

    DWORD Address;
    DWORD Size;
};


/*
 * D3DSurface, IDirect3DSurface9 interface
 *
 * Abstracts a chunk of data that can be drawn to.  The Common and Format
 * fields use the D3DCOMMON and D3DFORMAT constants defined for
 * textures.
 */

#define D3DSURFACE_ALIGNMENT 4096

// This internal flag indicates whether D3D allocated the EDRAM memory,
// or whether it was specified by the game via D3DSURFACE_PARAMETERS.

#define D3DCOMMON_SURFACE_D3D_EDRAM_ALLOCATED       0x80000000

struct D3DSurface
    #if defined(__cplusplus)
        : public D3DResource
    #endif
{
#ifdef __cplusplus

    HRESULT         WINAPI GetContainer(REFIID UnusedRiid, void **ppContainer);
    D3DVOID         WINAPI GetDesc(D3DSURFACE_DESC *pDesc);

    D3DVOID         WINAPI LockRect(D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
    D3DVOID         WINAPI UnlockRect();

    // The following methods are not supported on Xbox 360:
    //
    // GetDC
    // ReleaseDC

#endif __cplusplus

#ifndef __cplusplus
    // Manually inherit these from D3DResource
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
#endif

    DWORD Unused;

    GPUTEXTURE_FETCH_CONSTANT Format;
    DWORD Size;
    D3DBaseTexture *Parent;

    // The following fields are valid only if D3DCOMMON_SURFACEFROMTEXTURE
    // is not set:

    GPU_SURFACEINFO SurfaceInfo;
    union {
        GPU_DEPTHINFO DepthInfo;
        GPU_COLORINFO ColorInfo;
    };
    GPU_HICONTROL HiControl;
    DWORD DepthStencilClearValue; // This field to be remove with beta hardware
};

/*
 * D3DVolume, IDirect3DVolume9 interface
 */

#define D3DVOLUME_ALIGNMENT 4096

struct D3DVolume
    #if defined(__cplusplus)
        : public D3DResource
    #endif
{
#ifdef __cplusplus

    HRESULT         WINAPI GetContainer(REFIID UnusedRiid, void **ppContainer);
    D3DVOID         WINAPI GetDesc(D3DVOLUME_DESC *pDesc);
    D3DVOID         WINAPI LockBox(D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags);
    D3DVOID         WINAPI UnlockBox();

#endif

#ifndef __cplusplus
    // Manually inherit these from D3DResource
    DWORD Common;
    DWORD Fence;
    DWORD BaseFlush;
#endif

    DWORD Unused;

    GPUTEXTURE_FETCH_CONSTANT Format;
    DWORD Size;
    D3DBaseTexture *Parent;
};



/*
 * Direct3D, IDirect3D9 interface
 *
 */

#ifdef __cplusplus

struct Direct3D
{

    static ULONG    WINAPI AddRef();
    static ULONG    WINAPI Release();

    static UINT     WINAPI GetAdapterCount();
    static HRESULT  WINAPI GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier);
    static HRESULT  WINAPI CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed);
    static HRESULT  WINAPI CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
    static HRESULT  WINAPI CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
    static HRESULT  WINAPI CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
    static HRESULT  WINAPI CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
    static HRESULT  WINAPI GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
    static HRESULT  WINAPI CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void *hUnusedFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface);

    // The following APIs are all Xbox 360 extensions:

    static VOID     WINAPI QueryGpuVersion(D3DGPUVERSION* pGpuVersion);

    // The following methods are not supported on Xbox 360:
    //
    //  RegisterSoftwareDevice
    //  GetAdapterMonitor
};

#endif __cplusplus

/*
 * D3DDevice, IDirect3DDevice9 interface
 */

struct D3DDevice
{
#ifdef __cplusplus

    // Standard D3D APIs:

    ULONG WINAPI AddRef();
    ULONG WINAPI Release();
    D3DVOID WINAPI GetDirect3D(Direct3D **ppD3D9);
    D3DVOID WINAPI GetDeviceCaps(D3DCAPS9 *pCaps);
    D3DVOID WINAPI GetDisplayMode(UINT UnusedSwapChain, D3DDISPLAYMODE *pMode);
    D3DVOID WINAPI GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
    HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
    D3DVOID WINAPI Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, void *hUnusedDestWindowOverride, void *pUnusedDirtyRegion);
    D3DVOID WINAPI GetRasterStatus(UINT iUnusedSwapChain, D3DRASTER_STATUS *pRasterStatus);
    void    WINAPI SetGammaRamp(UINT iUnusedSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp);
    void    WINAPI GetGammaRamp(UINT iUnusedSwapChain, D3DGAMMARAMP *pRamp);
    void    WINAPI SetPWLGamma(DWORD Flags, CONST D3DPWLGAMMA *pRamp);
    void    WINAPI GetPWLGamma(D3DPWLGAMMA *pRamp);
    HRESULT WINAPI CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DTexture **ppTexture, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DVolumeTexture **ppVolumeTexture, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DCubeTexture **ppCubeTexture, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateArrayTexture(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DArrayTexture **ppArrayTexture, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateVertexBuffer(UINT Length, DWORD Usage, DWORD UnusedFVF, D3DPOOL UnusedPool, D3DVertexBuffer **ppVertexBuffer, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DIndexBuffer **ppIndexBuffer, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedLockable, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters);
    HRESULT WINAPI CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedDiscard, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters);
    D3DVOID WINAPI SetRenderTarget(DWORD RenderTargetIndex, D3DSurface *pRenderTarget);
    HRESULT WINAPI GetRenderTarget(DWORD RenderTargetIndex, D3DSurface **ppRenderTarget);
    D3DVOID WINAPI SetDepthStencilSurface(D3DSurface *pZStencilSurface);
    HRESULT WINAPI GetDepthStencilSurface(D3DSurface **ppZStencilSurface);
    HRESULT WINAPI GetBackBuffer(UINT UnusedSwapChain, UINT iUnusedBackBuffer, UINT UnusedType, D3DSurface **ppBackBuffer);
    HRESULT WINAPI GetFrontBuffer(D3DBaseTexture **ppFrontBuffer);
    D3DVOID WINAPI BeginScene();
    D3DVOID WINAPI EndScene();
    D3DVOID WINAPI Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    D3DVOID WINAPI ClearF(DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil);
    D3DVOID WINAPI SetViewport(CONST D3DVIEWPORT9 *pViewport);
    D3DVOID WINAPI GetViewport(D3DVIEWPORT9 *pViewport);
    D3DVOID WINAPI SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    D3DVOID WINAPI SetRenderState_Inline(D3DRENDERSTATETYPE State, DWORD Value);
    D3DVOID WINAPI GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue);
    HRESULT WINAPI CreateStateBlock(D3DSTATEBLOCKTYPE Type, D3DStateBlock** ppSB);
    D3DVOID WINAPI GetTexture(DWORD Sampler, D3DBaseTexture **ppTexture);
    D3DVOID WINAPI SetTexture(DWORD Sampler, D3DBaseTexture *pTexture);
    D3DVOID WINAPI GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
    D3DVOID WINAPI SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    D3DVOID WINAPI SetSamplerState_Inline(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    D3DVOID WINAPI DrawVertices(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount);
    D3DVOID WINAPI DrawIndexedVertices(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT VertexCount);
    D3DVOID WINAPI DrawVerticesUP(D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    D3DVOID WINAPI DrawIndexedVerticesUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT IndexCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    D3DVOID WINAPI DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    D3DVOID WINAPI DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT UnusedMinIndex, UINT UnusedNumIndices, UINT StartIndex, UINT PrimitiveCount);
    D3DVOID WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    D3DVOID WINAPI DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    D3DVOID WINAPI SetFVF(DWORD FVF);
    D3DVOID WINAPI GetFVF(DWORD* pFVF);
    HRESULT WINAPI CreateVertexShader(CONST DWORD *pFunction, D3DVertexShader** ppShader);
    D3DVOID WINAPI SetVertexShader(D3DVertexShader *pShader);
    D3DVOID WINAPI GetVertexShader(D3DVertexShader **ppShader);
    D3DVOID WINAPI SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    D3DVOID WINAPI SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI GetVertexShaderConstantB(UINT StartRegister, BOOL *pConstantData, DWORD BoolCount);
    D3DVOID WINAPI GetVertexShaderConstantF(UINT StartRegister, float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI GetVertexShaderConstantI(UINT StartRegister, int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI SetVertexShaderConstantF_NotInline(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI SetStreamSource(UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride);
    D3DVOID WINAPI GetStreamSource(UINT StreamNumber, D3DVertexBuffer **ppStreamData, UINT *pOffsetInBytes, UINT *pStride);
    D3DVOID WINAPI SetIndices(D3DIndexBuffer *pIndexData);
    D3DVOID WINAPI GetIndices(D3DIndexBuffer **ppIndexData);
    HRESULT WINAPI CreatePixelShader(CONST DWORD *pFunction, D3DPixelShader** ppShader);
    D3DVOID WINAPI SetPixelShader(D3DPixelShader* pShader);
    D3DVOID WINAPI GetPixelShader(D3DPixelShader** ppShader);
    D3DVOID WINAPI SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    D3DVOID WINAPI SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, DWORD BoolCount);
    D3DVOID WINAPI GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI SetPixelShaderConstantF_NotInline(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    HRESULT WINAPI CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, D3DVertexDeclaration **ppVertexDeclaration);
    D3DVOID WINAPI SetVertexDeclaration(D3DVertexDeclaration *pDecl);
    D3DVOID WINAPI GetVertexDeclaration(D3DVertexDeclaration **ppDecl);
    D3DVOID WINAPI SetScissorRect(CONST RECT* pRect);
    D3DVOID WINAPI GetScissorRect(RECT* pRect);
    D3DVOID WINAPI DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo);
    D3DVOID WINAPI DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo);
    D3DVOID WINAPI DeletePatch(UINT Handle);
    D3DVOID WINAPI SetClipPlane(DWORD Index, CONST float* pPlane);
    D3DVOID WINAPI GetClipPlane(DWORD Index, float* pPlane);
    D3DVOID WINAPI SetNPatchMode(float nSegments);
    float   WINAPI GetNPatchMode();
    HRESULT WINAPI CreateQuery(D3DQUERYTYPE Type, D3DQuery** ppQuery);
    HRESULT WINAPI CreateQueryTiled(D3DQUERYTYPE Type, DWORD TileCount, D3DQuery** ppQuery);

    // The following APIs are all Xbox 360 extensions:

    D3DVOID WINAPI Resolve(DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
    D3DVOID WINAPI AcquireThreadOwnership();
    D3DVOID WINAPI ReleaseThreadOwnership();
    D3DVOID WINAPI SetThreadOwnership(DWORD ThreadID);
    DWORD   WINAPI QueryThreadOwnership();
    BOOL    WINAPI IsBusy();
    D3DVOID WINAPI BlockUntilIdle();
    D3DVOID WINAPI InsertCallback(D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD Context);
    D3DVOID WINAPI SetVerticalBlankCallback(D3DVBLANKCALLBACK pCallback);
    D3DVOID WINAPI SynchronizeToPresentationInterval();
    D3DVOID WINAPI Swap(D3DBaseTexture* pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS* pParameters);
    D3DVOID WINAPI RenderSystemUI();
    D3DVOID WINAPI QueryBufferSpace(DWORD* pUsed, DWORD* pRemaining);
    D3DVOID WINAPI SetPredication(DWORD PredicationMask);
    D3DVOID WINAPI SetPatchablePredication(DWORD PredicationMask, DWORD Identifier);
    D3DVOID WINAPI BeginTiling(DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil);
    HRESULT WINAPI EndTiling(DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
    D3DVOID WINAPI BeginZPass(DWORD Flags);
    HRESULT WINAPI EndZPass();
    HRESULT WINAPI InvokeRenderPass();
    D3DVOID WINAPI BeginExport(DWORD Index, D3DResource* pResource);
    D3DVOID WINAPI EndExport(DWORD Index, D3DResource* pResource, DWORD Flags);
    D3DVOID WINAPI SynchronizeExport(D3DResource* pResource);
    D3DVOID WINAPI DrawTessellatedPrimitive(D3DTESSPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    D3DVOID WINAPI DrawIndexedTessellatedPrimitive(D3DTESSPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT PrimitiveCount);
    HRESULT WINAPI SetRingBufferParameters(CONST D3DRING_BUFFER_PARAMETERS *pParameters);
    D3DVOID WINAPI XpsBegin(DWORD Flags);
    HRESULT WINAPI XpsEnd();
    D3DVOID WINAPI XpsSetCallback(XpsCallback pCallback, void* pContext, DWORD Flags);
    D3DVOID WINAPI XpsSubmit(DWORD InstanceCount, CONST void* pData, DWORD Size);
    HRESULT WINAPI BeginVertices(D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, UINT VertexStreamZeroStride, void**ppVertexData);
    D3DVOID WINAPI EndVertices();
    HRESULT WINAPI BeginIndexedVertices(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT NumVertices, UINT IndexCount, D3DFORMAT IndexDataFormat, UINT VertexStreamZeroStride, void** ppIndexData, void** ppVertexData);
    D3DVOID WINAPI EndIndexedVertices();
    DWORD   WINAPI InsertFence();
    D3DVOID WINAPI BlockOnFence(DWORD Fence);
    BOOL    WINAPI IsFencePending(DWORD Fence);
    D3DVOID WINAPI SetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE BlendState);
    D3DVOID WINAPI GetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE* pBlendState);
    D3DVOID WINAPI SetVertexFetchConstant(UINT VertexFetchRegister, D3DVertexBuffer* pVertexBuffer, UINT Offset);
    D3DVOID WINAPI SetTextureFetchConstant(UINT TextureFetchRegister, D3DBaseTexture* pTexture);
    float   WINAPI GetCounter(D3DCOUNTER CounterID);
    D3DVOID WINAPI SetSafeLevel(DWORD Flags, DWORD Level);
    D3DVOID WINAPI GetSafeLevel(DWORD* pFlags, DWORD* pLevel);
    D3DVOID WINAPI SetHangCallback(D3DHANGCALLBACK pCallback);
    D3DVOID WINAPI BeginConditionalSurvey(DWORD Identifier, DWORD Flags);
    D3DVOID WINAPI EndConditionalSurvey(DWORD Flags);
    D3DVOID WINAPI BeginConditionalRendering(DWORD Identifier);
    D3DVOID WINAPI EndConditionalRendering();
    HRESULT WINAPI PersistDisplay(D3DTexture* pFrontBuffer, const D3DVIDEO_SCALER_PARAMETERS* pParameters);
    HRESULT WINAPI GetPersistedTexture(D3DTexture** pFrontBuffer);
    D3DVOID WINAPI Suspend();
    D3DVOID WINAPI Resume();
    HRESULT WINAPI CreatePerfCounters(D3DPerfCounters** ppCounters, UINT NumInstances);
    D3DVOID WINAPI EnablePerfCounters(BOOL Enable);
    D3DVOID WINAPI SetPerfCounterEvents(CONST D3DPERFCOUNTER_EVENTS* pEvents, DWORD Flags);
    D3DVOID WINAPI QueryPerfCounters(D3DPerfCounters* pCounters, DWORD Flags);
    D3DVOID WINAPI SetShaderGPRAllocation(DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount);
    D3DVOID WINAPI GetShaderGPRAllocation(DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount);
    D3DVOID WINAPI SetScreenExtentQueryMode(D3DSCREENEXTENTQUERYMODE Mode);
    D3DVOID WINAPI GetScreenExtentQueryMode(D3DSCREENEXTENTQUERYMODE* pMode);
    D3DVOID WINAPI BeginPixelShaderConstantF1(UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount);
    D3DVOID WINAPI EndPixelShaderConstantF1();
    D3DVOID WINAPI BeginVertexShaderConstantF1(UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount);
    D3DVOID WINAPI EndVertexShaderConstantF1();
    HRESULT WINAPI BeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount);
    D3DVOID WINAPI EndPixelShaderConstantF4();
    HRESULT WINAPI BeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount);
    D3DVOID WINAPI EndVertexShaderConstantF4();
    DWORD   WINAPI GetCurrentFence();
    D3DVOID WINAPI InvalidateGpuCache(void* pBaseAddress, DWORD Size, DWORD Flags);
    D3DVOID WINAPI FlushHiZStencil(D3DFHZS_FLUSHTYPE FlushType);


    // Methods from the original Xbox that may be re-implemented in the future:
    //
    //  GetDisplayFieldStatus
    //  Nop
    //  SetSwapCallback
    //  SetWaitCallback
    //  BlockUntilVerticalBlank
    //  SetFlickerFilter
    //  BeginCommand
    //  EndCommand
    //  RunCommandBuffer
    //  SetDebugMarker
    //  GetDebugMarker
    //
    // The following methods are not supported on Xbox 360:
    //
    //  TestCooperativeLevel
    //  GetAvailableTextureMemory
    //  EvictManagedResources
    //  SetCursorProperties
    //  SetCursorPosition
    //  ShowCursor
    //  CreateAdditionalSwapChain
    //  GetSwapChain
    //  GetNumberOfSwapChains
    //  SetDialogBoxMode
    //  UpdateSurface
    //  UpdateTexture
    //  GetRenderTargetData
    //  GetFrontBufferData
    //  StretchRect
    //  ColorFill
    //  CreateOffscreenPlainSurface
    //  ValidateDevice
    //  SetPaletteEntries
    //  GetPaletteEntries
    //  SetCurrentTexturePalette
    //  GetCurrentTexturePalette
    //  SetSoftwareVertexProcessing
    //  GetSoftwareVertexProcessing
    //  ProcessVertices
    //  BeginStateBlock
    //  EndStateBlock
    //  SetClipStatus
    //  GetClipStatus

    // Unbuffered 'Gpu' APIs:
    //
    // Most Xbox 360 D3D APIs defer the sending of modified state to the GPU
    // until a Draw API is called.  The following APIs are different in that
    // they are unbuffered and so immediately send modified state to the GPU
    // when the API is called.

    void    WINAPI GpuOwn(D3DTAG Tag);
    void    WINAPI GpuOwnSubset(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex);
    void    WINAPI GpuOwnVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuOwnPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuDisown(D3DTAG Tag);
    void    WINAPI GpuDisownSubset(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex);
    void    WINAPI GpuDisownVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuDisownPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuDisownAll();
    void    WINAPI GpuLoadPixelShaderConstantF4(UINT StartRegister, const void* pConstantData, DWORD Vector4fCount);
    void    WINAPI GpuLoadVertexShaderConstantF4(UINT StartRegister, const void* pConstantData, DWORD Vector4fCount);
    HRESULT WINAPI GpuBeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount);
    void    WINAPI GpuEndVertexShaderConstantF4();
    HRESULT WINAPI GpuBeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount);
    void    WINAPI GpuEndPixelShaderConstantF4();

#endif __cplusplus

    // 'm_pRing' points one dword BEFORE the location in the ring-buffer
    // or command buffer where the next GPU command will be added.  It's one
    // dword before so that we can use 'storewordupdate(dword, 4, pCommand)'/
    // 'storefloatupdate(float, 4, pCommand)' for most of our buffer writes,
    // which is a nifty PPC trick that does a store and auto-increment in one
    // instruction.  The only caveat is that it's a pre-increment...
    //
    // (The 'storewordupdate/storefloatupdate' trick has the other nice
    // attribute in that it forces the compiler to strongly order all writes
    // to the ring-buffer, which is important since all writes to write-combined
    // have to be perfectly consecutive in order to write-combine.)
    //
    PRING m_pRing;

    // 'm_pRingLimit' points to the end of the current secondary ring buffer
    // segment.
    //
    PRING m_pRingLimit;

    // If 'm_pRing' is less than 'm_pRingGuarantee', it's guaranteed that
    // there are at least 32 dwords of room in the segment.
    //
    PRING m_pRingGuarantee;

    // Reference count on the current device.
    //
    DWORD m_ReferenceCount;

    // Tracks all GPU registers that are pending to be set at the next Draw
    // calls.  These are essentially 'Lazy' flags for all GPU context state.
    // Only registers that are not 'owned' by the 'Gpu' APIs can be pending.
    //
    D3DTAGCOLLECTION m_Pending;

    // Tracks what GPU registers have been used 'so far'.  This is used on
    // retail for recording command buffers and on debug for using the
    // InvalidateConstants API.
    //
    D3DTAGCOLLECTION m_Used;

    // Table of function pointers to specific state APIs.
    //
    D3DSETRENDERSTATECALL m_SetRenderStateCall[D3DRS_MAX/4];
    D3DSETSAMPLERSTATECALL m_SetSamplerStateCall[D3DSAMP_MAX/4];
    D3DGETRENDERSTATECALL m_GetRenderStateCall[D3DRS_MAX/4];
    D3DGETSAMPLERSTATECALL m_GetSamplerStateCall[D3DSAMP_MAX/4];

    // Shadow of all GPU constants.  Ensure that the beginning of the ALU
    // constant array is 128-byte aligned.
    //
    __declspec(align(128)) D3DConstants m_Constants;
};

/*
 * C exported method definitions for the class methods defined above and the C++
 * thunks that defer to them.
 */

/* Direct3D */

D3DINLINE ULONG     WINAPI Direct3D_AddRef() { return 1; }
D3DINLINE ULONG     WINAPI Direct3D_Release() { return 1; }
D3DINLINE UINT      WINAPI Direct3D_GetAdapterCount() { return 1; }
HRESULT             WINAPI Direct3D_GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier);
HRESULT             WINAPI Direct3D_CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed);
HRESULT             WINAPI Direct3D_CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
HRESULT             WINAPI Direct3D_CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
HRESULT             WINAPI Direct3D_CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
HRESULT             WINAPI Direct3D_CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
HRESULT             WINAPI Direct3D_GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
HRESULT             WINAPI Direct3D_CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void *hUnusedFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface);
VOID                WINAPI Direct3D_QueryGpuVersion(D3DGPUVERSION *pGpuVersion);


// Compatibility wrappers.

D3DINLINE ULONG     WINAPI IDirect3D9_AddRef(Direct3D *pThis) { return Direct3D_AddRef(); }
D3DINLINE ULONG     WINAPI IDirect3D9_Release(Direct3D *pThis) { return Direct3D_Release(); }
D3DINLINE UINT      WINAPI IDirect3D9_GetAdapterCount(Direct3D *pThis) { return Direct3D_GetAdapterCount(); }
D3DINLINE HRESULT   WINAPI IDirect3D9_GetAdapterIdentifier(Direct3D *pThis, UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) { return Direct3D_GetAdapterIdentifier(Adapter, Flags, pIdentifier); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CheckDeviceType(Direct3D *pThis, UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed) { return Direct3D_CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CheckDeviceFormat(Direct3D *pThis, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) { return Direct3D_CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CheckDeviceMultiSampleType(Direct3D *pThis, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) { return Direct3D_CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CheckDepthStencilMatch(Direct3D *pThis, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return Direct3D_CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return Direct3D_CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat); }
D3DINLINE HRESULT   WINAPI IDirect3D9_GetDeviceCaps(Direct3D *pThis, UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps) { return Direct3D_GetDeviceCaps(Adapter, DeviceType, pCaps); }
D3DINLINE HRESULT   WINAPI IDirect3D9_CreateDevice(Direct3D *pThis, UINT Adapter, D3DDEVTYPE DeviceType, void *hUnusedFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface) { return Direct3D_CreateDevice(Adapter, DeviceType, hUnusedFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface); }
D3DINLINE VOID      WINAPI IDirect3D9_QueryGpuVersion(Direct3D *pThis, D3DGPUVERSION *pGpuVersion) { Direct3D_QueryGpuVersion(pGpuVersion); }

#ifdef __cplusplus

D3DMINLINE ULONG    WINAPI Direct3D::AddRef() { return Direct3D_AddRef(); }
D3DMINLINE ULONG    WINAPI Direct3D::Release() { return Direct3D_Release(); }
D3DMINLINE UINT     WINAPI Direct3D::GetAdapterCount() { return Direct3D_GetAdapterCount(); }
D3DMINLINE HRESULT  WINAPI Direct3D::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) { return Direct3D_GetAdapterIdentifier(Adapter, Flags, pIdentifier); }
D3DMINLINE HRESULT  WINAPI Direct3D::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed) { return Direct3D_CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed); }
D3DMINLINE HRESULT  WINAPI Direct3D::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) { return Direct3D_CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat); }
D3DMINLINE HRESULT  WINAPI Direct3D::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) { return Direct3D_CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels); }
D3DMINLINE HRESULT  WINAPI Direct3D::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return Direct3D_CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat); }
D3DMINLINE HRESULT  WINAPI Direct3D::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return Direct3D_CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat); }
D3DMINLINE HRESULT  WINAPI Direct3D::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps) { return Direct3D_GetDeviceCaps(Adapter, DeviceType, pCaps); }
D3DMINLINE HRESULT  WINAPI Direct3D::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void *hUnusedFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface) { return Direct3D_CreateDevice(Adapter, DeviceType, hUnusedFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface); }
D3DMINLINE VOID     WINAPI Direct3D::QueryGpuVersion(D3DGPUVERSION *pGpuVersion) { Direct3D_QueryGpuVersion(pGpuVersion); }

#endif __cplusplus

/****************************************************************************
 *
 * D3DDevice_*
 *
 * Private internal interfaces - Please don't access these directly, as they're
 *                               subject to change.
 *
 ****************************************************************************/

D3DVertexShader*      WINAPI D3DDevice_CreateVertexShader(CONST DWORD *pFunction);
D3DPixelShader*       WINAPI D3DDevice_CreatePixelShader(CONST DWORD *pFunction);
D3DBaseTexture*       WINAPI D3DDevice_CreateTexture(DWORD Width, DWORD Height, DWORD Depth, DWORD Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DRESOURCETYPE D3DType);
D3DSurface*           WINAPI D3DDevice_CreateSurface(DWORD Width, DWORD Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, CONST D3DSURFACE_PARAMETERS* pParameters);
D3DVertexBuffer*      WINAPI D3DDevice_CreateVertexBuffer(UINT Length, DWORD Usage, D3DPOOL UnusedPool);
D3DIndexBuffer*       WINAPI D3DDevice_CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool);
D3DVertexDeclaration* WINAPI D3DDevice_CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements);
D3DVertexDeclaration* WINAPI D3DDevice_GetVertexDeclaration(D3DDevice *pDevice);
D3DSurface*           WINAPI D3DDevice_GetRenderTarget(D3DDevice *pDevice, DWORD RenderTargetIndex);
D3DSurface*           WINAPI D3DDevice_GetDepthStencilSurface(D3DDevice *pDevice);
D3DSurface*           WINAPI D3DDevice_GetBackBuffer(D3DDevice *pDevice);
D3DBaseTexture*       WINAPI D3DDevice_GetFrontBuffer(D3DDevice *pDevice);
D3DBaseTexture*       WINAPI D3DDevice_GetTexture(D3DDevice *pDevice, DWORD Sampler);
D3DVertexBuffer*      WINAPI D3DDevice_GetStreamSource(D3DDevice *pDevice, UINT StreamNumber, UINT *pOffsetInBytes, UINT *pStride);
D3DIndexBuffer*       WINAPI D3DDevice_GetIndices(D3DDevice *pDevice);
D3DQuery*             WINAPI D3DDevice_CreateQuery(D3DDevice *pDevice, D3DQUERYTYPE Type);
D3DQuery*             WINAPI D3DDevice_CreateQueryTiled(D3DDevice *pDevice, D3DQUERYTYPE Type, DWORD TileCount);
D3DStateBlock*        WINAPI D3DDevice_CreateStateBlock(D3DDevice* pDevice, D3DSTATEBLOCKTYPE Type);
D3DPerfCounters*      WINAPI D3DDevice_CreatePerfCounters(D3DDevice *pDevice, UINT NumInstances);

ULONG   WINAPI D3DDevice_AddRef(D3DDevice *pDevice);
ULONG   WINAPI D3DDevice_Release(D3DDevice *pDevice);
void    WINAPI D3DDevice_GetDirect3D(D3DDevice *pDevice, Direct3D **ppD3D9);
void    WINAPI D3DDevice_GetDeviceCaps(D3DDevice *pDevice, D3DCAPS9 *pCaps);
void    WINAPI D3DDevice_GetDisplayMode(D3DDevice *pDevice, UINT UnusedSwapChain, D3DDISPLAYMODE *pMode);
void    WINAPI D3DDevice_GetCreationParameters(D3DDevice *pDevice, D3DDEVICE_CREATION_PARAMETERS *pParameters);
HRESULT WINAPI D3DDevice_Reset(D3DDevice *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters);
void    WINAPI D3DDevice_GetRasterStatus(D3DDevice *pDevice, D3DRASTER_STATUS *pRasterStatus);
void    WINAPI D3DDevice_SetGammaRamp(D3DDevice *pDevice, DWORD Flags, CONST D3DGAMMARAMP *pRamp);
void    WINAPI D3DDevice_GetGammaRamp(D3DDevice *pDevice, D3DGAMMARAMP *pRamp);
void    WINAPI D3DDevice_SetPWLGamma(D3DDevice *pDevice, DWORD Flags, CONST D3DPWLGAMMA *pRamp);
void    WINAPI D3DDevice_GetPWLGamma(D3DDevice *pDevice, D3DPWLGAMMA *pRamp);
void    WINAPI D3DDevice_SetRenderTarget(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DSurface *pRenderTarget);
void    WINAPI D3DDevice_SetDepthStencilSurface(D3DDevice *pDevice, D3DSurface *pZStencilSurface);
D3DINLINE void    WINAPI D3DDevice_BeginScene(D3DDevice *pDevice) { }
D3DINLINE void    WINAPI D3DDevice_EndScene(D3DDevice *pDevice) { }
void    WINAPI D3DDevice_Clear(D3DDevice *pDevice, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil, BOOL EDRAMClear);
void    WINAPI D3DDevice_ClearF(D3DDevice *pDevice, DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil);
void    WINAPI D3DDevice_SetViewport(D3DDevice *pDevice, CONST D3DVIEWPORT9 *pViewport);
void    WINAPI D3DDevice_GetViewport(D3DDevice *pDevice, D3DVIEWPORT9 *pViewport);
void    WINAPI D3DDevice_SetRenderState_ParameterCheck(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD Value);
void    WINAPI D3DDevice_GetRenderState_ParameterCheck(D3DDevice *pDevice, D3DRENDERSTATETYPE State);
void    WINAPI D3DDevice_SetSamplerState_ParameterCheck(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
void    WINAPI D3DDevice_GetSamplerState_ParameterCheck(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type);
void    WINAPI D3DDevice_SetTexture(D3DDevice *pDevice, DWORD Sampler, D3DBaseTexture *pTexture);
void    WINAPI D3DDevice_DrawVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount);
void    WINAPI D3DDevice_DrawIndexedVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT VertexCount);
void    WINAPI D3DDevice_DrawVerticesUP(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
void    WINAPI D3DDevice_DrawIndexedVerticesUP(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT IndexCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
void    WINAPI D3DDevice_SetVertexDeclaration(D3DDevice* pDevice, D3DVertexDeclaration *pDecl);
void    WINAPI D3DDevice_SetScissorRect(D3DDevice* pDevice, CONST RECT* pRect);
void    WINAPI D3DDevice_GetScissorRect(D3DDevice* pDevice, RECT* pRect);
void    WINAPI D3DDevice_SetFVF(D3DDevice *pDevice, DWORD FVF);
DWORD   WINAPI D3DDevice_GetFVF(D3DDevice *pDevice);
void    WINAPI D3DDevice_SetVertexShader(D3DDevice *pDevice, D3DVertexShader *pShader);
void    WINAPI D3DDevice_GetVertexShader(D3DDevice *pDevice, D3DVertexShader **ppShader);
void    WINAPI D3DDevice_SetVertexShaderConstantB(D3DDevice *pDevice, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
void    WINAPI D3DDevice_SetVertexShaderConstantFN(D3DDevice *pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_SetVertexShaderConstantI(D3DDevice *pDevice, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
void    WINAPI D3DDevice_GetVertexShaderConstantB(D3DDevice *pDevice, UINT StartRegister, BOOL *pConstantData, DWORD BoolCount);
void    WINAPI D3DDevice_GetVertexShaderConstantF(D3DDevice *pDevice, UINT StartRegister, float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_GetVertexShaderConstantI(D3DDevice *pDevice, UINT StartRegister, int *pConstantData, DWORD Vector4iCount);
void    WINAPI D3DDevice_SetVertexShaderConstantF_ParameterCheck(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_SetStreamSource(D3DDevice *pDevice, UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride);
void    WINAPI D3DDevice_SetIndices(D3DDevice *pDevice, D3DIndexBuffer* pIndexData);
void    WINAPI D3DDevice_SetPixelShader(D3DDevice *pDevice, D3DPixelShader* pShader);
void    WINAPI D3DDevice_GetPixelShader(D3DDevice *pDevice, D3DPixelShader** ppShader);
void    WINAPI D3DDevice_SetPixelShaderConstantB(D3DDevice *pDevice, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
void    WINAPI D3DDevice_SetPixelShaderConstantFN(D3DDevice *pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_SetPixelShaderConstantI(D3DDevice *pDevice, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
void    WINAPI D3DDevice_GetPixelShaderConstantB(D3DDevice *pDevice, UINT StartRegister, BOOL *pConstantData, DWORD BoolCount);
void    WINAPI D3DDevice_GetPixelShaderConstantF(D3DDevice *pDevice, UINT StartRegister, float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_GetPixelShaderConstantI(D3DDevice *pDevice, UINT StartRegister, int *pConstantData, DWORD Vector4iCount);
void    WINAPI D3DDevice_SetPixelShaderConstantF_ParameterCheck(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_DrawRectPatch(D3DDevice *pDevice, UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo);
void    WINAPI D3DDevice_DrawTriPatch(D3DDevice *pDevice, UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo);
void    WINAPI D3DDevice_DeletePatch(D3DDevice *pDevice, UINT Handle);
void    WINAPI D3DDevice_SetClipPlane(D3DDevice *pDevice, DWORD Index, CONST float* pPlane);
void    WINAPI D3DDevice_GetClipPlane(D3DDevice *pDevice, DWORD Index, float* pPlane);
void    WINAPI D3DDevice_SetNPatchMode(D3DDevice *pDevice, float nSegments);
float   WINAPI D3DDevice_GetNPatchMode(D3DDevice *pDevice);

void    WINAPI D3DDevice_Resolve(D3DDevice* pDevice, DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
void    WINAPI D3DDevice_AcquireThreadOwnership(D3DDevice* pDevice);
void    WINAPI D3DDevice_ReleaseThreadOwnership(D3DDevice* pDevice);
void    WINAPI D3DDevice_SetThreadOwnership(D3DDevice* pDevice, DWORD ThreadID);
DWORD   WINAPI D3DDevice_QueryThreadOwnership(D3DDevice* pDevice);
void    WINAPI D3DDevice_Present(D3DDevice *pDevice);
BOOL    WINAPI D3DDevice_IsBusy(D3DDevice *pDevice);
void    WINAPI D3DDevice_BlockUntilIdle(D3DDevice *pDevice);
void    WINAPI D3DDevice_InsertCallback(D3DDevice *pDevice, D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD Context);
void    WINAPI D3DDevice_SetVerticalBlankCallback(D3DDevice *pDevice, D3DVBLANKCALLBACK pCallback);
void    WINAPI D3DDevice_SynchronizeToPresentationInterval(D3DDevice *pDevice);
void    WINAPI D3DDevice_Swap(D3DDevice *pDevice, D3DBaseTexture *pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS *pParameters);
void    WINAPI D3DDevice_RenderSystemUI(D3DDevice *pDevice);
void    WINAPI D3DDevice_QueryBufferSpace(D3DDevice* pDevice, DWORD* pUsed, DWORD* pRemaining);
void    WINAPI D3DDevice_SetPredication(D3DDevice* pDevice, DWORD PredicationMask);
void    WINAPI D3DDevice_SetPatchablePredication(D3DDevice* pDevice, DWORD PredicationMask, DWORD Identifier);
void    WINAPI D3DDevice_BeginTiling(D3DDevice* pDevice, DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil);
HRESULT WINAPI D3DDevice_EndTiling(D3DDevice* pDevice, DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
void    WINAPI D3DDevice_BeginZPass(D3DDevice* pDevice, DWORD Flags);
HRESULT WINAPI D3DDevice_EndZPass(D3DDevice* pDevice);
HRESULT WINAPI D3DDevice_InvokeRenderPass(D3DDevice* pDevice);
void    WINAPI D3DDevice_BeginExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource);
void    WINAPI D3DDevice_EndExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource, DWORD Flags);
void    WINAPI D3DDevice_SynchronizeExport(D3DDevice* pDevice, D3DResource* pResource);
HRESULT WINAPI D3DDevice_CreateCommandBuffer(DWORD Size, DWORD Flags, D3DDevice** ppCommandBuffer);
void    WINAPI D3DDevice_SetCommandBufferMemory(D3DDevice* pDevice, void* pMemory, DWORD Size);
void    WINAPI D3DDevice_BeginCommandBuffer(D3DDevice* pDevice, DWORD Flags);
HRESULT WINAPI D3DDevice_EndCommandBuffer(D3DDevice* pDevice);
void    WINAPI D3DDevice_DrawTessellatedVertices(D3DDevice *pDevice, D3DTESSPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount);
void    WINAPI D3DDevice_DrawIndexedTessellatedVertices(D3DDevice *pDevice, D3DTESSPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT VertexCount);
HRESULT WINAPI D3DDevice_SetRingBufferParameters(D3DDevice* pDevice, CONST D3DRING_BUFFER_PARAMETERS* pParameters);
void    WINAPI D3DDevice_XpsBegin(D3DDevice* pDevice, DWORD Flags);
HRESULT WINAPI D3DDevice_XpsEnd(D3DDevice* pDevice);
void    WINAPI D3DDevice_XpsSetCallback(D3DDevice* pDevice, XpsCallback pCallback, void* pContext, DWORD Flags);
void    WINAPI D3DDevice_XpsSubmit(D3DDevice* pDevice, DWORD InstanceCount, CONST void* pData, DWORD Size);
void*   WINAPI D3DDevice_BeginVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, UINT VertexStreamZeroStride);
void    WINAPI D3DDevice_EndVertices(D3DDevice *pDevice);
HRESULT WINAPI D3DDevice_BeginIndexedVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT NumVertices, UINT IndexCount, D3DFORMAT IndexDataFormat, UINT VertexStreamZeroStride, void** ppIndexData, void** ppVertexData);
void    WINAPI D3DDevice_EndIndexedVertices(D3DDevice *pDevice);
DWORD   WINAPI D3DDevice_InsertFence(D3DDevice* pDevice);
void    WINAPI D3DDevice_BlockOnFence(DWORD Fence);
BOOL    WINAPI D3DDevice_IsFencePending(DWORD Fence);
void    WINAPI D3DDevice_SetBlendState(D3DDevice* pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE BlendState);
void    WINAPI D3DDevice_GetBlendState(D3DDevice* pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE* pBlendState);
void    WINAPI D3DDevice_SetVertexFetchConstant(D3DDevice* pDevice, UINT VertexFetchRegister, D3DVertexBuffer* pVertexBuffer, UINT Offset);
void    WINAPI D3DDevice_SetTextureFetchConstant(D3DDevice* pDevice, UINT TextureFetchRegister, D3DBaseTexture* pTexture);
float   WINAPI D3DDevice_GetCounter(D3DDevice* pDevice, D3DCOUNTER CounterID);
void    WINAPI D3DDevice_SetSafeLevel(D3DDevice* pDevice, DWORD Flags, DWORD Level);
void    WINAPI D3DDevice_GetSafeLevel(D3DDevice* pDevice, DWORD* pFlags, DWORD* pLevel);
void    WINAPI D3DDevice_SetHangCallback(D3DDevice* pDevice, D3DHANGCALLBACK pCallback);
void    WINAPI D3DDevice_BeginConditionalSurvey(D3DDevice* pDevice, DWORD Identifier, DWORD Flags);
void    WINAPI D3DDevice_EndConditionalSurvey(D3DDevice* pDevice, DWORD Flags);
void    WINAPI D3DDevice_BeginConditionalRendering(D3DDevice* pDevice, DWORD Identifier);
void    WINAPI D3DDevice_EndConditionalRendering(D3DDevice* pDevice);
HRESULT WINAPI D3DDevice_PersistDisplay(D3DDevice* pDevice, D3DTexture* pFrontBuffer, const D3DVIDEO_SCALER_PARAMETERS* pParameters);
HRESULT WINAPI D3DDevice_GetPersistedTexture(D3DDevice* pDevice, D3DTexture** pFrontBuffer);
void    WINAPI D3DDevice_Suspend(D3DDevice* pDevice);
void    WINAPI D3DDevice_Resume(D3DDevice* pDevice);
void    WINAPI D3DDevice_EnablePerfCounters(D3DDevice* pDevice, BOOL Enable);
void    WINAPI D3DDevice_SetPerfCounterEvents(D3DDevice* pDevice, CONST D3DPERFCOUNTER_EVENTS* pEvents, DWORD Flags);
void    WINAPI D3DDevice_QueryPerfCounters(D3DDevice* pDevice, D3DPerfCounters* pCounters, DWORD Flags);
void    WINAPI D3DDevice_SetShaderGPRAllocation(D3DDevice* pDevice, DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount);
void    WINAPI D3DDevice_GetShaderGPRAllocation(D3DDevice* pDevice, DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount);
void    WINAPI D3DDevice_SetScreenExtentQueryMode(D3DDevice* pDevice, D3DSCREENEXTENTQUERYMODE Mode);
void    WINAPI D3DDevice_GetScreenExtentQueryMode(D3DDevice* pDevice, D3DSCREENEXTENTQUERYMODE* pMode);
void    WINAPI D3DDevice_BeginShaderConstantF1_ParameterCheck(D3DDevice *pDevice, BOOL PixelShader, UINT StartRegister, DWORD Vector4fCount);
void    WINAPI D3DDevice_EndShaderConstantF1_ParameterCheck(D3DDevice *pDevice);
HRESULT WINAPI D3DDevice_BeginShaderConstantF4(D3DDevice *pDevice, BOOL PixelShader, UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, DWORD Vector4fCount);
void    WINAPI D3DDevice_EndShaderConstantF4_ParameterCheck(D3DDevice *pDevice);
DWORD   WINAPI D3DDevice_GetCurrentFence(D3DDevice *pDevice);
void    WINAPI D3DDevice_InvalidateGpuCache(D3DDevice* pDevice, void* pBaseAddress, DWORD Size, DWORD Flags);
void    WINAPI D3DDevice_FlushHiZStencil(D3DDevice* pDevice, D3DFHZS_FLUSHTYPE FlushType);

void WINAPI D3DDevice_SetRenderState_CullMode(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_FillMode(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaTestEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaBlendEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_BlendOp(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_SrcBlend(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_DestBlend(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_BlendOpAlpha(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_SrcBlendAlpha(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_DestBlendAlpha(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_SeparateAlphaBlendEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaRef(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaFunc(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_BlendFactor(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ZEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ZWriteEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ZFunc(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_TwoSidedStencilMode(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilFunc(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilFail(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilZFail(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilPass(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilFunc(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilFail(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilZFail(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilPass(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilRef(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilMask(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_StencilWriteMask(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilRef(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilMask(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_CCWStencilWriteMask(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ClipPlaneEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ScissorTestEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_SlopeScaleDepthBias(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_DepthBias(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_MultiSampleAntiAlias(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_MultiSampleMask(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ColorWriteEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ColorWriteEnable1(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ColorWriteEnable2(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ColorWriteEnable3(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PointSpriteEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PointSize(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_PointSizeMin(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_PointSizeMax(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_Wrap0(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap1(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap2(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap3(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap4(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap5(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap6(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap7(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap8(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap9(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap10(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap11(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap12(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap13(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap14(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_Wrap15(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_SRGBWriteEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_ViewportEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HighPrecisionBlendEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HighPrecisionBlendEnable1(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HighPrecisionBlendEnable2(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HighPrecisionBlendEnable3(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_MinTessellationLevel(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_MaxTessellationLevel(D3DDevice* pDevice, DWORD FloatAsDword);
void WINAPI D3DDevice_SetRenderState_TessellationMode(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HalfPixelOffset(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PrimitiveResetEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PrimitiveResetIndex(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaToMaskEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_AlphaToMaskOffsets(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_GuardBand_X(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_GuardBand_Y(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_DiscardBand_X(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_DiscardBand_Y(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HiStencilEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HiStencilWriteEnable(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HiStencilFunc(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_HiStencilRef(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PresentInterval(D3DDevice* pDevice, DWORD Value);
void WINAPI D3DDevice_SetRenderState_PresentImmediateThreshold(D3DDevice* pDevice, DWORD Value);

void WINAPI D3DDevice_SetSamplerState_MinFilter(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MinFilterZ(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MagFilter(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MagFilterZ(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MipFilter(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_SeparateZFilterEnable(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MaxAnisotropy(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_AnisotropyBias(D3DDevice* pDevice, DWORD Sampler, DWORD FloatAsDword);
void WINAPI D3DDevice_SetSamplerState_MipMapLodBias(D3DDevice* pDevice, DWORD Sampler, DWORD FloatAsDword);
void WINAPI D3DDevice_SetSamplerState_MaxMipLevel(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_MinMipLevel(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_BorderColor(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_AddressU(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_AddressV(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_AddressW(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_TrilinearThreshold(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_HGradientExpBias(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_VGradientExpBias(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_WhiteBorderColorW(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
void WINAPI D3DDevice_SetSamplerState_PointBorderEnable(D3DDevice* pDevice, DWORD Sampler, DWORD Value);

D3DVECTOR4* WINAPI D3DDevice_GpuBeginShaderConstantF4(D3DDevice* pDevice, BOOL PixelShader, UINT StartRegister, DWORD Vector4fCount);

void    WINAPI D3DDevice_GpuOwn_ParameterCheck(D3DDevice *pDevice, DWORD Index, UINT64 Mask);
void    WINAPI D3DDevice_GpuDisown_ParameterCheck(D3DDevice *pDevice, DWORD Index, UINT64 Mask);
void    WINAPI D3DDevice_GpuDisownAll(D3DDevice *pDevice);
void    WINAPI D3DDevice_GpuEndShaderConstantF4_ParameterCheck(D3DDevice* pDevice);
void    WINAPI D3DDevice_GpuLoadShaderConstantF4(D3DDevice* pDevice, BOOL PixelShader, UINT StartRegister, const void* pConstantData, DWORD Vector4fCount);

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressU_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampX = (GPUCLAMP)Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressV_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampY = (GPUCLAMP)Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressW_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampZ = (GPUCLAMP)Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_MipFilter_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].MipFilter = Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_BorderColor_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].BorderColor = (Value == 0) ? GPUBORDERCOLOR_ABGR_BLACK : GPUBORDERCOLOR_ABGR_WHITE;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_TrilinearThreshold_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].TriClamp = (GPUTRICLAMP)Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_HGradientExpBias_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].GradExpAdjustH = (INT) Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_VGradientExpBias_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].GradExpAdjustV = (INT) Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_WhiteBorderColorW_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ForceBCWToMax = Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_PointBorderEnable_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampPolicy = (GPUCLAMPPOLICY)!Value;
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler));
}

// When SetRenderState or SetSamplerState are called with a constant
// state type we can avoid an indirect and directly call the function
// courtesy of the optimizer.  We can also often inline the directly called
// function.

D3DINLINE void D3DDevice_SetRenderState_Inline(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD Value)
{
    #if defined(_DEBUG)
    D3DDevice_SetRenderState_ParameterCheck(pDevice, State, Value);
    #endif

    switch (State)
    {
    case D3DRS_PATCHEDGESTYLE:             break;
    case D3DRS_POSITIONDEGREE:             break;
    case D3DRS_NORMALDEGREE:               break;
    case D3DRS_ANTIALIASEDLINEENABLE:      break;
    case D3DRS_ADAPTIVETESS_X:             break;
    case D3DRS_ADAPTIVETESS_Y:             break;
    case D3DRS_ADAPTIVETESS_Z:             break;
    case D3DRS_ADAPTIVETESS_W:             break;
    case D3DRS_ENABLEADAPTIVETESSELLATION: break;
    case D3DRS_DITHERENABLE:               break;

    case D3DRS_ZENABLE:                   D3DDevice_SetRenderState_ZEnable(pDevice, Value);                   break;
    case D3DRS_ZFUNC:                     D3DDevice_SetRenderState_ZFunc(pDevice, Value);                     break;
    case D3DRS_ZWRITEENABLE:              D3DDevice_SetRenderState_ZWriteEnable(pDevice, Value);              break;
    case D3DRS_FILLMODE:                  D3DDevice_SetRenderState_FillMode(pDevice, Value);                  break;
    case D3DRS_CULLMODE:                  D3DDevice_SetRenderState_CullMode(pDevice, Value);                  break;
    case D3DRS_ALPHABLENDENABLE:          D3DDevice_SetRenderState_AlphaBlendEnable(pDevice, Value);          break;
    case D3DRS_SEPARATEALPHABLENDENABLE:  D3DDevice_SetRenderState_SeparateAlphaBlendEnable(pDevice, Value);  break;
    case D3DRS_BLENDFACTOR:               D3DDevice_SetRenderState_BlendFactor(pDevice, Value);               break;
    case D3DRS_SRCBLEND:                  D3DDevice_SetRenderState_SrcBlend(pDevice, Value);                  break;
    case D3DRS_DESTBLEND:                 D3DDevice_SetRenderState_DestBlend(pDevice, Value);                 break;
    case D3DRS_BLENDOP:                   D3DDevice_SetRenderState_BlendOp(pDevice, Value);                   break;
    case D3DRS_SRCBLENDALPHA:             D3DDevice_SetRenderState_SrcBlendAlpha(pDevice, Value);             break;
    case D3DRS_DESTBLENDALPHA:            D3DDevice_SetRenderState_DestBlendAlpha(pDevice, Value);            break;
    case D3DRS_BLENDOPALPHA:              D3DDevice_SetRenderState_BlendOpAlpha(pDevice, Value);              break;
    case D3DRS_ALPHATESTENABLE:           D3DDevice_SetRenderState_AlphaTestEnable(pDevice, Value);           break;
    case D3DRS_ALPHAREF:                  D3DDevice_SetRenderState_AlphaRef(pDevice, Value);                  break;
    case D3DRS_ALPHAFUNC:                 D3DDevice_SetRenderState_AlphaFunc(pDevice, Value);                 break;
    case D3DRS_STENCILENABLE:             D3DDevice_SetRenderState_StencilEnable(pDevice, Value);             break;
    case D3DRS_TWOSIDEDSTENCILMODE:       D3DDevice_SetRenderState_TwoSidedStencilMode(pDevice, Value);       break;
    case D3DRS_STENCILFAIL:               D3DDevice_SetRenderState_StencilFail(pDevice, Value);               break;
    case D3DRS_STENCILZFAIL:              D3DDevice_SetRenderState_StencilZFail(pDevice, Value);              break;
    case D3DRS_STENCILPASS:               D3DDevice_SetRenderState_StencilPass(pDevice, Value);               break;
    case D3DRS_STENCILFUNC:               D3DDevice_SetRenderState_StencilFunc(pDevice, Value);               break;
    case D3DRS_STENCILREF:                D3DDevice_SetRenderState_StencilRef(pDevice, Value);                break;
    case D3DRS_STENCILMASK:               D3DDevice_SetRenderState_StencilMask(pDevice, Value);               break;
    case D3DRS_STENCILWRITEMASK:          D3DDevice_SetRenderState_StencilWriteMask(pDevice, Value);          break;
    case D3DRS_CCW_STENCILFAIL:           D3DDevice_SetRenderState_CCWStencilFail(pDevice, Value);            break;
    case D3DRS_CCW_STENCILZFAIL:          D3DDevice_SetRenderState_CCWStencilZFail(pDevice, Value);           break;
    case D3DRS_CCW_STENCILPASS:           D3DDevice_SetRenderState_CCWStencilPass(pDevice, Value);            break;
    case D3DRS_CCW_STENCILFUNC:           D3DDevice_SetRenderState_CCWStencilFunc(pDevice, Value);            break;
    case D3DRS_CCW_STENCILREF:            D3DDevice_SetRenderState_CCWStencilRef(pDevice, Value);             break;
    case D3DRS_CCW_STENCILMASK:           D3DDevice_SetRenderState_CCWStencilMask(pDevice, Value);            break;
    case D3DRS_CCW_STENCILWRITEMASK:      D3DDevice_SetRenderState_CCWStencilWriteMask(pDevice, Value);       break;
    case D3DRS_CLIPPLANEENABLE:           D3DDevice_SetRenderState_ClipPlaneEnable(pDevice, Value);           break;
    case D3DRS_POINTSIZE:                 D3DDevice_SetRenderState_PointSize(pDevice, Value);                 break;
    case D3DRS_POINTSIZE_MIN:             D3DDevice_SetRenderState_PointSizeMin(pDevice, Value);              break;
    case D3DRS_POINTSPRITEENABLE:         D3DDevice_SetRenderState_PointSpriteEnable(pDevice, Value);         break;
    case D3DRS_POINTSIZE_MAX:             D3DDevice_SetRenderState_PointSizeMax(pDevice, Value);              break;
    case D3DRS_MULTISAMPLEANTIALIAS:      D3DDevice_SetRenderState_MultiSampleAntiAlias(pDevice, Value);      break;
    case D3DRS_MULTISAMPLEMASK:           D3DDevice_SetRenderState_MultiSampleMask(pDevice, Value);           break;
    case D3DRS_SCISSORTESTENABLE:         D3DDevice_SetRenderState_ScissorTestEnable(pDevice, Value);         break;
    case D3DRS_SLOPESCALEDEPTHBIAS:       D3DDevice_SetRenderState_SlopeScaleDepthBias(pDevice, Value);       break;
    case D3DRS_DEPTHBIAS:                 D3DDevice_SetRenderState_DepthBias(pDevice, Value);                 break;
    case D3DRS_COLORWRITEENABLE:          D3DDevice_SetRenderState_ColorWriteEnable(pDevice, Value);          break;
    case D3DRS_COLORWRITEENABLE1:         D3DDevice_SetRenderState_ColorWriteEnable1(pDevice, Value);         break;
    case D3DRS_COLORWRITEENABLE2:         D3DDevice_SetRenderState_ColorWriteEnable2(pDevice, Value);         break;
    case D3DRS_COLORWRITEENABLE3:         D3DDevice_SetRenderState_ColorWriteEnable3(pDevice, Value);         break;
    case D3DRS_SRGBWRITEENABLE:           D3DDevice_SetRenderState_SRGBWriteEnable(pDevice, Value);           break;
    case D3DRS_TESSELLATIONMODE:          D3DDevice_SetRenderState_TessellationMode(pDevice, Value);          break;
    case D3DRS_MINTESSELLATIONLEVEL:      D3DDevice_SetRenderState_MinTessellationLevel(pDevice, Value);      break;
    case D3DRS_MAXTESSELLATIONLEVEL:      D3DDevice_SetRenderState_MaxTessellationLevel(pDevice, Value);      break;
    case D3DRS_WRAP0:                     D3DDevice_SetRenderState_Wrap0(pDevice, Value);                     break;
    case D3DRS_WRAP1:                     D3DDevice_SetRenderState_Wrap1(pDevice, Value);                     break;
    case D3DRS_WRAP2:                     D3DDevice_SetRenderState_Wrap2(pDevice, Value);                     break;
    case D3DRS_WRAP3:                     D3DDevice_SetRenderState_Wrap3(pDevice, Value);                     break;
    case D3DRS_WRAP4:                     D3DDevice_SetRenderState_Wrap4(pDevice, Value);                     break;
    case D3DRS_WRAP5:                     D3DDevice_SetRenderState_Wrap5(pDevice, Value);                     break;
    case D3DRS_WRAP6:                     D3DDevice_SetRenderState_Wrap6(pDevice, Value);                     break;
    case D3DRS_WRAP7:                     D3DDevice_SetRenderState_Wrap7(pDevice, Value);                     break;
    case D3DRS_WRAP8:                     D3DDevice_SetRenderState_Wrap8(pDevice, Value);                     break;
    case D3DRS_WRAP9:                     D3DDevice_SetRenderState_Wrap9(pDevice, Value);                     break;
    case D3DRS_WRAP10:                    D3DDevice_SetRenderState_Wrap10(pDevice, Value);                    break;
    case D3DRS_WRAP11:                    D3DDevice_SetRenderState_Wrap11(pDevice, Value);                    break;
    case D3DRS_WRAP12:                    D3DDevice_SetRenderState_Wrap12(pDevice, Value);                    break;
    case D3DRS_WRAP13:                    D3DDevice_SetRenderState_Wrap13(pDevice, Value);                    break;
    case D3DRS_WRAP14:                    D3DDevice_SetRenderState_Wrap14(pDevice, Value);                    break;
    case D3DRS_WRAP15:                    D3DDevice_SetRenderState_Wrap15(pDevice, Value);                    break;
    case D3DRS_VIEWPORTENABLE:            D3DDevice_SetRenderState_ViewportEnable(pDevice, Value);            break;
    case D3DRS_HIGHPRECISIONBLENDENABLE:  D3DDevice_SetRenderState_HighPrecisionBlendEnable(pDevice, Value);  break;
    case D3DRS_HIGHPRECISIONBLENDENABLE1: D3DDevice_SetRenderState_HighPrecisionBlendEnable1(pDevice, Value); break;
    case D3DRS_HIGHPRECISIONBLENDENABLE2: D3DDevice_SetRenderState_HighPrecisionBlendEnable2(pDevice, Value); break;
    case D3DRS_HIGHPRECISIONBLENDENABLE3: D3DDevice_SetRenderState_HighPrecisionBlendEnable3(pDevice, Value); break;
    case D3DRS_HALFPIXELOFFSET:           D3DDevice_SetRenderState_HalfPixelOffset(pDevice, Value);           break;
    case D3DRS_PRIMITIVERESETENABLE:      D3DDevice_SetRenderState_PrimitiveResetEnable(pDevice, Value);      break;
    case D3DRS_PRIMITIVERESETINDEX:       D3DDevice_SetRenderState_PrimitiveResetIndex(pDevice, Value);       break;
    case D3DRS_ALPHATOMASKENABLE:         D3DDevice_SetRenderState_AlphaToMaskEnable(pDevice, Value);         break;
    case D3DRS_ALPHATOMASKOFFSETS:        D3DDevice_SetRenderState_AlphaToMaskOffsets(pDevice, Value);        break;
    case D3DRS_GUARDBAND_X:               D3DDevice_SetRenderState_GuardBand_X(pDevice, Value);               break;
    case D3DRS_GUARDBAND_Y:               D3DDevice_SetRenderState_GuardBand_Y(pDevice, Value);               break;
    case D3DRS_DISCARDBAND_X:             D3DDevice_SetRenderState_DiscardBand_X(pDevice, Value);             break;
    case D3DRS_DISCARDBAND_Y:             D3DDevice_SetRenderState_DiscardBand_Y(pDevice, Value);             break;
    case D3DRS_HISTENCILENABLE:           D3DDevice_SetRenderState_HiStencilEnable(pDevice, Value);           break;
    case D3DRS_HISTENCILWRITEENABLE:      D3DDevice_SetRenderState_HiStencilWriteEnable(pDevice, Value);      break;
    case D3DRS_HISTENCILFUNC:             D3DDevice_SetRenderState_HiStencilFunc(pDevice, Value);             break;
    case D3DRS_HISTENCILREF:              D3DDevice_SetRenderState_HiStencilRef(pDevice, Value);              break;
    case D3DRS_PRESENTINTERVAL:           D3DDevice_SetRenderState_PresentInterval(pDevice, Value);           break;
    case D3DRS_PRESENTIMMEDIATETHRESHOLD: D3DDevice_SetRenderState_PresentImmediateThreshold(pDevice, Value); break;

    #if defined(_DEBUG)
    default: __debugbreak();
    #endif
    }
}

D3DINLINE void D3DDevice_SetSamplerState_Inline(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    #if defined(_DEBUG)
    D3DDevice_SetSamplerState_ParameterCheck(pDevice, Sampler, Type, Value);
    #endif

    switch (Type)
    {
    case D3DSAMP_ADDRESSU:              D3DDevice_SetSamplerState_AddressU_Inline(pDevice, Sampler, Value);           break;
    case D3DSAMP_ADDRESSV:              D3DDevice_SetSamplerState_AddressV_Inline(pDevice, Sampler, Value);           break;
    case D3DSAMP_ADDRESSW:              D3DDevice_SetSamplerState_AddressW_Inline(pDevice, Sampler, Value);           break;
    case D3DSAMP_BORDERCOLOR:           D3DDevice_SetSamplerState_BorderColor_Inline(pDevice, Sampler, Value);        break;
    case D3DSAMP_MAGFILTER:             D3DDevice_SetSamplerState_MagFilter(pDevice, Sampler, Value);                 break;
    case D3DSAMP_MINFILTER:             D3DDevice_SetSamplerState_MinFilter(pDevice, Sampler, Value);                 break;
    case D3DSAMP_MIPFILTER:             D3DDevice_SetSamplerState_MipFilter_Inline(pDevice, Sampler, Value);          break;
    case D3DSAMP_MIPMAPLODBIAS:         D3DDevice_SetSamplerState_MipMapLodBias(pDevice, Sampler, Value);             break;
    case D3DSAMP_MAXMIPLEVEL:           D3DDevice_SetSamplerState_MaxMipLevel(pDevice, Sampler, Value);               break;
    case D3DSAMP_MAXANISOTROPY:         D3DDevice_SetSamplerState_MaxAnisotropy(pDevice, Sampler, Value);             break;
    case D3DSAMP_MAGFILTERZ:            D3DDevice_SetSamplerState_MagFilterZ(pDevice, Sampler, Value);                break;
    case D3DSAMP_MINFILTERZ:            D3DDevice_SetSamplerState_MinFilterZ(pDevice, Sampler, Value);                break;
    case D3DSAMP_SEPARATEZFILTERENABLE: D3DDevice_SetSamplerState_SeparateZFilterEnable(pDevice, Sampler, Value);     break;
    case D3DSAMP_MINMIPLEVEL:           D3DDevice_SetSamplerState_MinMipLevel(pDevice, Sampler, Value);               break;
    case D3DSAMP_TRILINEARTHRESHOLD:    D3DDevice_SetSamplerState_TrilinearThreshold_Inline(pDevice, Sampler, Value); break;
    case D3DSAMP_ANISOTROPYBIAS:        D3DDevice_SetSamplerState_AnisotropyBias(pDevice, Sampler, Value);            break;
    case D3DSAMP_HGRADIENTEXPBIAS:      D3DDevice_SetSamplerState_HGradientExpBias_Inline(pDevice, Sampler, Value);   break;
    case D3DSAMP_VGRADIENTEXPBIAS:      D3DDevice_SetSamplerState_VGradientExpBias_Inline(pDevice, Sampler, Value);   break;
    case D3DSAMP_WHITEBORDERCOLORW:     D3DDevice_SetSamplerState_WhiteBorderColorW_Inline(pDevice, Sampler, Value);  break;
    case D3DSAMP_POINTBORDERENABLE:     D3DDevice_SetSamplerState_PointBorderEnable_Inline(pDevice, Sampler, Value);  break;

    #if defined(_DEBUG)
    default: __debugbreak();
    #endif
    }
}

// When SetRenderState or SetSamplerState are called with a non-constant
// state type we indirect through a function-table which hangs off the device.
//
// In order to save a shift instruction we've pre-scaled the constants
// by 4 (the stride of the table) and do an ugly bit of casting.

D3DINLINE void D3DDevice_SetRenderState(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD Value)
{
    #if defined(_DEBUG)
    D3DDevice_SetRenderState_ParameterCheck(pDevice, State, Value);
    #endif
    (*(D3DSETRENDERSTATECALL*)(((BYTE*) &pDevice->m_SetRenderStateCall[0] + State)))(pDevice, Value);
}
D3DINLINE void D3DDevice_GetRenderState(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD* pValue)
{
    #if defined(_DEBUG)
    D3DDevice_GetRenderState_ParameterCheck(pDevice, State);
    #endif
    *pValue = (*(D3DGETRENDERSTATECALL*)(((BYTE*) &pDevice->m_GetRenderStateCall[0] + State)))(pDevice);
}
D3DINLINE void D3DDevice_SetSamplerState(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    #if defined(_DEBUG)
    D3DDevice_SetSamplerState_ParameterCheck(pDevice, Sampler, Type, Value);
    #endif
    (*(D3DSETSAMPLERSTATECALL*)(((BYTE*) &pDevice->m_SetSamplerStateCall[0] + Type)))(pDevice, Sampler, Value);
}
D3DINLINE void D3DDevice_GetSamplerState(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD *pValue)
{
    #if defined(_DEBUG)
    D3DDevice_GetSamplerState_ParameterCheck(pDevice, Sampler, Type);
    #endif
    *pValue = (*(D3DGETSAMPLERSTATECALL*)(((BYTE*) &pDevice->m_GetSamplerStateCall[0] + Type)))(pDevice, Sampler);
}

// Fast deferred shader constant setting

D3DINLINE void D3DDevice_BeginVertexShaderConstantF1(D3DDevice *pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount)
{
    #if defined(_DEBUG)
    D3DDevice_BeginShaderConstantF1_ParameterCheck(pDevice, FALSE, StartRegister, Vector4fCount);
    #endif
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    *ppConstantData = &pDevice->m_Constants.VertexShaderF[StartRegister];
}
D3DINLINE void D3DDevice_BeginPixelShaderConstantF1(D3DDevice *pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount)
{
    #if defined(_DEBUG)
    D3DDevice_BeginShaderConstantF1_ParameterCheck(pDevice, TRUE, StartRegister, Vector4fCount);
    #endif
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    *ppConstantData = &pDevice->m_Constants.PixelShaderF[StartRegister];
}
D3DINLINE void D3DDevice_EndShaderConstantF1(D3DDevice *pDevice)
{
    #if defined(_DEBUG)
    D3DDevice_EndShaderConstantF1_ParameterCheck(pDevice);
    #endif
}
D3DINLINE void D3DDevice_EndShaderConstantF4(D3DDevice *pDevice)
{
    #if defined(_DEBUG)
    D3DDevice_EndShaderConstantF4_ParameterCheck(pDevice);
    #endif
}

// The following two inline routines are automatically called by the corresponding
// SetVertexShaderConstantF/SetPixelShaderConstantF APIs.

D3DINLINE void D3DDevice_SetVertexShaderConstantF1(D3DDevice* pDevice, UINT StartRegister, CONST float* D3DRESTRICT pConstantData)
{
#if defined(_DEBUG)
    D3DDevice_SetVertexShaderConstantF_ParameterCheck(pDevice, StartRegister, pConstantData, 1);
#endif
    // Copy into D3D's shadow state.  Note that by copying a dword at a
    // time, we let the optimizer recognize and remove redundant copies.
    // That wouldn't be true if we used a vector copy.  For example,
    // this lets the compiler optimize the following so that the temporary
    // store to the stack is completely eliminated from a sequence such
    // as the following:
    //
    //      float vector[4] = { x, y, z, w };
    //      pDevice->SetVertexShaderConstantF(Register, vector, 1);

    pDevice->m_Constants.VertexShaderF[StartRegister].x = pConstantData[0];
    pDevice->m_Constants.VertexShaderF[StartRegister].y = pConstantData[1];
    pDevice->m_Constants.VertexShaderF[StartRegister].z = pConstantData[2];
    pDevice->m_Constants.VertexShaderF[StartRegister].w = pConstantData[3];

    // Let D3D know that the constants were set:

    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, 1));
}
D3DINLINE void D3DDevice_SetPixelShaderConstantF1(D3DDevice* D3DRESTRICT pDevice, UINT StartRegister, CONST float* D3DRESTRICT pConstantData)
{
#if defined(_DEBUG)
    D3DDevice_SetPixelShaderConstantF_ParameterCheck(pDevice, StartRegister, pConstantData, 1);
#endif

    pDevice->m_Constants.PixelShaderF[StartRegister].x = pConstantData[0];
    pDevice->m_Constants.PixelShaderF[StartRegister].y = pConstantData[1];
    pDevice->m_Constants.PixelShaderF[StartRegister].z = pConstantData[2];
    pDevice->m_Constants.PixelShaderF[StartRegister].w = pConstantData[3];

    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, 1));
}

// The following two inline routines are automatically called by the
// corresponding SetVertexShaderConstantF/SetPixelShaderConstantF routines.
// They special-case a count of "1" since that's typically the most frequent
// count specified, and inlining can reduce copies and avoid load-hit-store
// stalls.
//
// Note that the conditionals here get compiled away by the optimizer
// when the vector count is constant.  If you call with a variable vector count,
// please use the SetVertexShaderConstantF_NotInline/
// SetPixelShaderConstantF_NotInline API variants.

D3DINLINE void D3DDevice_SetVertexShaderConstantF(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount)
{
    if (Vector4fCount == 1)
    {
        D3DDevice_SetVertexShaderConstantF1(pDevice, StartRegister, pConstantData);
    }
    else
    {
        D3DDevice_SetVertexShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount);
    }
}

D3DINLINE void D3DDevice_SetPixelShaderConstantF(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount)
{
    if (Vector4fCount == 1)
    {
        D3DDevice_SetPixelShaderConstantF1(pDevice, StartRegister, pConstantData);
    }
    else
    {
        D3DDevice_SetPixelShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount);
    }
}

#if defined(_DEBUG)

// Set D3D__DisableConstantOverwriteCheck to disable the shader constant
// overwrite checks. These checks are only performed for debug builds.) This is
// useful when D3D is warning you about constants being overwritten, but you
// know that it's OK. (Perhaps you know that for some draw calls the shader
// doesn't actually use the constants that were overwritten.)

D3DEXTERN BOOL D3D__DisableConstantOverwriteCheck;

#endif

// Unbuffered Graphics support

D3DINLINE void D3DDevice_GpuOwn(D3DDevice *pDevice, D3DTAG Tag)
{
    DWORD index = D3DTag_Index(Tag);
    UINT64 mask = D3DTag_Mask(Tag);

    #if defined(_DEBUG)
    D3DDevice_GpuOwn_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Used, index, mask);
    D3DTagCollection_Clear(&pDevice->m_Pending, index, mask);

}
D3DINLINE void D3DDevice_GpuOwnSubset(D3DDevice *pDevice, D3DTAG Tag, DWORD StartIndex, DWORD EndIndex)
{
    DWORD index = D3DTag_Index(Tag);
    UINT64 mask = D3DTag_SubsetMask(Tag, StartIndex, EndIndex);

    #if defined(_DEBUG)
    D3DDevice_GpuOwn_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Used, index, mask);
    D3DTagCollection_Clear(&pDevice->m_Pending, index, mask);
}
D3DINLINE void D3DDevice_GpuOwnVertexShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    DWORD index = D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS);
    UINT64 mask = D3DTag_ShaderConstantMask(StartRegister, Vector4fCount);

    #if defined(_DEBUG)
    D3DDevice_GpuOwn_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Used, index, mask);
    D3DTagCollection_Clear(&pDevice->m_Pending, index, mask);
}
D3DINLINE void D3DDevice_GpuOwnPixelShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    DWORD index = D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS);
    UINT64 mask = D3DTag_ShaderConstantMask(StartRegister, Vector4fCount);

    #if defined(_DEBUG)
    D3DDevice_GpuOwn_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Used, index, mask);
    D3DTagCollection_Clear(&pDevice->m_Pending, index, mask);
}


D3DINLINE void D3DDevice_GpuDisown(D3DDevice *pDevice, D3DTAG Tag)
{
    DWORD index = D3DTag_Index(Tag);
    UINT64 mask = D3DTag_Mask(Tag);
    #if defined(_DEBUG)
    D3DDevice_GpuDisown_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Pending, index, mask);
}
D3DINLINE void D3DDevice_GpuDisownSubset(D3DDevice *pDevice, D3DTAG Tag, DWORD StartIndex, DWORD EndIndex)
{
    DWORD index = D3DTag_Index(Tag);
    UINT64 mask = D3DTag_SubsetMask(Tag, StartIndex, EndIndex);

    #if defined(_DEBUG)
    D3DDevice_GpuDisown_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Pending, index, mask);
}
D3DINLINE void D3DDevice_GpuDisownVertexShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    DWORD index = D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS);
    UINT64 mask = D3DTag_ShaderConstantMask(StartRegister, Vector4fCount);

    #if defined(_DEBUG)
    D3DDevice_GpuDisown_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Pending, index, mask);
}
D3DINLINE void D3DDevice_GpuDisownPixelShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    DWORD index = D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS);
    UINT64 mask = D3DTag_ShaderConstantMask(StartRegister, Vector4fCount);

    #if defined(_DEBUG)
    D3DDevice_GpuDisown_ParameterCheck(pDevice, index, mask);
    #endif

    D3DTagCollection_Set(&pDevice->m_Pending, index, mask);
}

D3DINLINE void D3DDevice_GpuEndShaderConstantF4(D3DDevice *pDevice)
{
    #if defined(_DEBUG)
    D3DDevice_GpuEndShaderConstantF4_ParameterCheck(pDevice);
    #endif
}

// Compatibility wrappers.

D3DINLINE ULONG   WINAPI IDirect3DDevice9_AddRef(D3DDevice *pThis) { return D3DDevice_AddRef(pThis); }
D3DINLINE ULONG   WINAPI IDirect3DDevice9_Release(D3DDevice *pThis) { return D3DDevice_Release(pThis); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetDirect3D(D3DDevice *pThis, Direct3D **ppD3D9) { D3DDevice_GetDirect3D(pThis, ppD3D9); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetDeviceCaps(D3DDevice *pThis, D3DCAPS9 *pCaps) { D3DDevice_GetDeviceCaps(pThis, pCaps); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetDisplayMode(D3DDevice *pThis, UINT UnusedSwapChain, D3DDISPLAYMODE *pMode) { D3DDevice_GetDisplayMode(pThis, UnusedSwapChain, pMode); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetCreationParameters(D3DDevice *pThis, D3DDEVICE_CREATION_PARAMETERS *pParameters) { D3DDevice_GetCreationParameters(pThis, pParameters); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_Reset(D3DDevice *pThis, D3DPRESENT_PARAMETERS *pPresentationParameters) { return D3DDevice_Reset(pThis, pPresentationParameters); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Present(D3DDevice *pThis, CONST RECT *pSourceRect, CONST RECT *pDestRect, void *hUnusedDestWindowOverride, void *pUnusedDirtyRegion) { D3DDevice_Present(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetRasterStatus(D3DDevice *pThis, UINT iUnusedSwapChain, D3DRASTER_STATUS *pRasterStatus) { D3DDevice_GetRasterStatus(pThis, pRasterStatus); D3DVOIDRETURN; }
D3DINLINE void    WINAPI IDirect3DDevice9_SetGammaRamp(D3DDevice *pThis, UINT iUnusedSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp) { D3DDevice_SetGammaRamp(pThis, Flags, pRamp); }
D3DINLINE void    WINAPI IDirect3DDevice9_GetGammaRamp(D3DDevice *pThis, UINT iUnusedSwapChain, D3DGAMMARAMP *pRamp) { D3DDevice_GetGammaRamp(pThis, pRamp); }
D3DINLINE void    WINAPI IDirect3DDevice9_SetPWLGamma(D3DDevice *pThis, DWORD Flags, CONST D3DPWLGAMMA *pRamp) { D3DDevice_SetPWLGamma(pThis, Flags, pRamp); }
D3DINLINE void    WINAPI IDirect3DDevice9_GetPWLGamma(D3DDevice *pThis, D3DPWLGAMMA *pRamp) { D3DDevice_GetPWLGamma(pThis, pRamp); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateTexture(D3DDevice *pThis, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DTexture **ppTexture, HANDLE* pUnusedSharedHandle) { return (*ppTexture = (D3DTexture*) D3DDevice_CreateTexture(Width, Height, 1, Levels, Usage, Format, UnusedPool, D3DRTYPE_TEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateVolumeTexture(D3DDevice *pThis, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DVolumeTexture **ppVolumeTexture, HANDLE* pUnusedSharedHandle) { return (*ppVolumeTexture = (D3DVolumeTexture*) D3DDevice_CreateTexture(Width, Height, Depth, Levels, Usage, Format, UnusedPool, D3DRTYPE_VOLUMETEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateCubeTexture(D3DDevice *pThis, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DCubeTexture **ppCubeTexture, HANDLE* pUnusedSharedHandle) { return (*ppCubeTexture = (D3DCubeTexture*) D3DDevice_CreateTexture(EdgeLength, EdgeLength, 6, Levels, Usage, Format, UnusedPool, D3DRTYPE_CUBETEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateArrayTexture(D3DDevice *pThis, UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DArrayTexture **ppArrayTexture, HANDLE* pUnusedSharedHandle) { return (*ppArrayTexture = (D3DArrayTexture*) D3DDevice_CreateTexture(Width, Height, ArraySize, Levels, Usage, Format, UnusedPool, D3DRTYPE_ARRAYTEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateVertexBuffer(D3DDevice *pThis, UINT Length, DWORD Usage, DWORD UnusedFVF, D3DPOOL UnusedPool, D3DVertexBuffer **ppVertexBuffer, HANDLE* pUnusedSharedHandle) { return (*ppVertexBuffer = D3DDevice_CreateVertexBuffer(Length, Usage, UnusedPool)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateIndexBuffer(D3DDevice *pThis, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DIndexBuffer **ppIndexBuffer, HANDLE* pUnusedSharedHandle) { return (*ppIndexBuffer = D3DDevice_CreateIndexBuffer(Length, Usage, Format, UnusedPool)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateVertexDeclaration(D3DDevice* pThis, CONST D3DVERTEXELEMENT9 *pVertexElements, D3DVertexDeclaration **ppVertexDeclaration) { return (*ppVertexDeclaration = D3DDevice_CreateVertexDeclaration(pVertexElements)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateRenderTarget(D3DDevice *pThis, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedLockable, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return (*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateDepthStencilSurface(D3DDevice *pThis, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedDiscard, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return (*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetRenderTarget(D3DDevice *pThis, DWORD RenderTargetIndex, D3DSurface *pRenderTarget) { D3DDevice_SetRenderTarget(pThis, RenderTargetIndex, pRenderTarget); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GetRenderTarget(D3DDevice *pThis, DWORD RenderTargetIndex, D3DSurface **ppRenderTarget) { return (*ppRenderTarget = D3DDevice_GetRenderTarget(pThis, RenderTargetIndex)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetDepthStencilSurface(D3DDevice *pThis, D3DSurface *pZStencilSurface) { D3DDevice_SetDepthStencilSurface(pThis, pZStencilSurface); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GetDepthStencilSurface(D3DDevice *pThis, D3DSurface **ppZStencilSurface) { return (*ppZStencilSurface = D3DDevice_GetDepthStencilSurface(pThis)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GetBackBuffer(D3DDevice *pThis, UINT UnusedSwapChain, UINT iUnusedBackBuffer, UINT UnusedType, D3DSurface **ppBackBuffer) { return (*ppBackBuffer = D3DDevice_GetBackBuffer(pThis)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GetFrontBuffer(D3DDevice *pThis, D3DBaseTexture **ppFrontBuffer) { return (*ppFrontBuffer = D3DDevice_GetFrontBuffer(pThis)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginScene(D3DDevice *pThis) { D3DDevice_BeginScene(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndScene(D3DDevice *pThis) { D3DDevice_EndScene(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Clear(D3DDevice *pThis, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { D3DDevice_Clear(pThis, Count, pRects, Flags, Color, Z, Stencil, FALSE); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_ClearF(D3DDevice *pThis, DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil) { D3DDevice_ClearF(pThis, Flags, pRect, pColor, Z, Stencil); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SynchronizeToPresentationInterval(D3DDevice *pThis) { D3DDevice_SynchronizeToPresentationInterval(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Swap(D3DDevice *pThis, D3DBaseTexture *pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS *pParameters) { D3DDevice_Swap(pThis, pFrontBuffer, pParameters); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_RenderSystemUI(D3DDevice *pThis) { D3DDevice_RenderSystemUI(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_QueryBufferSpace(D3DDevice* pDevice, DWORD* pUsed, DWORD* pRemaining) { D3DDevice_QueryBufferSpace(pDevice, pUsed, pRemaining); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPredication(D3DDevice* pDevice, DWORD PredicationMask) { D3DDevice_SetPredication(pDevice, PredicationMask); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPatchablePredication(D3DDevice* pDevice, DWORD PredicationMask, DWORD Identifier) { D3DDevice_SetPatchablePredication(pDevice, PredicationMask, Identifier); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginTiling(D3DDevice* pDevice, DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil) { D3DDevice_BeginTiling(pDevice, Flags, Count, pTileRects, pClearColor, ClearZ, ClearStencil); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_EndTiling(D3DDevice* pDevice, DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { return D3DDevice_EndTiling(pDevice, ResolveFlags, pResolveRects, pDestTexture, pClearColor, ClearZ, ClearStencil, pParameters); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginZPass(D3DDevice* pDevice, DWORD Flags) { D3DDevice_BeginZPass(pDevice, Flags); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_EndZPass(D3DDevice* pDevice) { return D3DDevice_EndZPass(pDevice); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_InvokeRenderPass(D3DDevice* pDevice) { return D3DDevice_InvokeRenderPass(pDevice); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource) { D3DDevice_BeginExport(pDevice, Index, pResource); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource, DWORD Flags) { D3DDevice_EndExport(pDevice, Index, pResource, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SynchronizeExport(D3DDevice* pDevice, D3DResource* pResource) { D3DDevice_SynchronizeExport(pDevice, pResource); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetViewport(D3DDevice *pThis, CONST D3DVIEWPORT9 *pViewport) { D3DDevice_SetViewport(pThis, pViewport); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetViewport(D3DDevice *pThis, D3DVIEWPORT9 *pViewport) { D3DDevice_GetViewport(pThis, pViewport); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetRenderState(D3DDevice *pThis, D3DRENDERSTATETYPE State, DWORD Value) { D3DDevice_SetRenderState(pThis, State, Value); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetRenderState_Inline(D3DDevice *pThis, D3DRENDERSTATETYPE State, DWORD Value) { D3DDevice_SetRenderState_Inline(pThis, State, Value); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetRenderState(D3DDevice *pThis, D3DRENDERSTATETYPE State, DWORD *pValue) { D3DDevice_GetRenderState(pThis, State, pValue); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateStateBlock(D3DDevice *pThis, D3DSTATEBLOCKTYPE Type, D3DStateBlock** ppSB) { return (*ppSB = D3DDevice_CreateStateBlock(pThis, Type)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetTexture(D3DDevice *pThis, DWORD Sampler, D3DBaseTexture **ppTexture) { *ppTexture = D3DDevice_GetTexture(pThis, Sampler); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetTexture(D3DDevice *pThis, DWORD Sampler, D3DBaseTexture *pTexture) { D3DDevice_SetTexture(pThis, Sampler, pTexture); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetSamplerState(D3DDevice *pThis, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD *pValue) { D3DDevice_GetSamplerState(pThis, Sampler, Type, pValue); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetSamplerState(D3DDevice *pThis, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { D3DDevice_SetSamplerState(pThis, Sampler, Type, Value); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetSamplerState_Inline(D3DDevice *pThis, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { D3DDevice_SetSamplerState_Inline(pThis, Sampler, Type, Value); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawVertices(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount) { D3DDevice_DrawVertices(pThis, PrimitiveType, StartVertex, VertexCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawIndexedVertices(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT VertexCount) { D3DDevice_DrawIndexedVertices(pThis, PrimitiveType, BaseVertexIndex, StartIndex, VertexCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawVerticesUP(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawVerticesUP(pThis, PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawIndexedVerticesUP(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT IndexCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawIndexedVerticesUP(pThis, PrimitiveType, MinVertexIndex, NumVertices, IndexCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawPrimitive(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { D3DDevice_DrawVertices(pThis, PrimitiveType, StartVertex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawIndexedPrimitive(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT UnusedMinIndex, UINT UnusedNumIndices, UINT StartIndex, UINT PrimitiveCount) { D3DDevice_DrawIndexedVertices(pThis, PrimitiveType, BaseVertexIndex, StartIndex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawPrimitiveUP(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawVerticesUP(pThis, PrimitiveType, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount), pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawIndexedPrimitiveUP(D3DDevice *pThis, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawIndexedVerticesUP(pThis, PrimitiveType, MinVertexIndex, NumVertices, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount), pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexDeclaration(D3DDevice *pThis, D3DVertexDeclaration *pDecl) { D3DDevice_SetVertexDeclaration(pThis, pDecl); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetVertexDeclaration(D3DDevice *pThis, D3DVertexDeclaration **ppDecl) { *ppDecl = D3DDevice_GetVertexDeclaration(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetScissorRect(D3DDevice *pThis, CONST RECT* pRect) { D3DDevice_SetScissorRect(pThis, pRect); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetScissorRect(D3DDevice *pThis, RECT* pRect) { D3DDevice_GetScissorRect(pThis, pRect); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetFVF(D3DDevice *pThis, DWORD FVF) { D3DDevice_SetFVF(pThis, FVF); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetFVF(D3DDevice *pThis, DWORD* pFVF) { *pFVF = D3DDevice_GetFVF(pThis); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateVertexShader(D3DDevice *pThis, CONST DWORD *pFunction, D3DVertexShader** ppShader) { return (*ppShader = D3DDevice_CreateVertexShader(pFunction)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexShader(D3DDevice *pThis, D3DVertexShader *pShader) { D3DDevice_SetVertexShader(pThis, pShader); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetVertexShader(D3DDevice *pThis, D3DVertexShader **ppShader) { D3DDevice_GetVertexShader(pThis, ppShader); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexShaderConstantB(D3DDevice *pThis, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount) { D3DDevice_SetVertexShaderConstantB(pThis, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexShaderConstantF(D3DDevice *pThis, UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount) { D3DDevice_SetVertexShaderConstantF(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexShaderConstantI(D3DDevice *pThis, UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount) { D3DDevice_SetVertexShaderConstantI(pThis, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetVertexShaderConstantB(D3DDevice *pThis, UINT StartRegister, BOOL *pConstantData, DWORD BoolCount) { D3DDevice_GetVertexShaderConstantB(pThis, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetVertexShaderConstantF(D3DDevice *pThis, UINT StartRegister, float *pConstantData, DWORD Vector4fCount) { D3DDevice_GetVertexShaderConstantF(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetVertexShaderConstantI(D3DDevice *pThis, UINT StartRegister, int *pConstantData, DWORD Vector4iCount) { D3DDevice_GetVertexShaderConstantI(pThis, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexShaderConstantF_NotInline(D3DDevice *pThis, UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount) { D3DDevice_SetVertexShaderConstantFN(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetStreamSource(D3DDevice *pThis, UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride) { D3DDevice_SetStreamSource(pThis, StreamNumber, pStreamData, OffsetInBytes, Stride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetStreamSource(D3DDevice *pThis, UINT StreamNumber, D3DVertexBuffer **ppStreamData, UINT *pOffsetInBytes, UINT *pStride) { *ppStreamData = D3DDevice_GetStreamSource(pThis, StreamNumber, pOffsetInBytes, pStride); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetIndices(D3DDevice *pThis, D3DIndexBuffer* pIndexData) { D3DDevice_SetIndices(pThis, pIndexData); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetIndices(D3DDevice *pThis, D3DIndexBuffer** ppIndexData) { *ppIndexData = D3DDevice_GetIndices(pThis); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreatePixelShader(D3DDevice *pThis, CONST DWORD *pFunction, IDirect3DPixelShader9** ppShader) { return (*ppShader = D3DDevice_CreatePixelShader(pFunction)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPixelShader(D3DDevice *pThis, D3DPixelShader* pShader) { D3DDevice_SetPixelShader(pThis, pShader); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetPixelShader(D3DDevice *pThis, D3DPixelShader** ppShader) { D3DDevice_GetPixelShader(pThis, ppShader); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPixelShaderConstantB(D3DDevice *pThis, UINT StartRegister, CONST BOOL *pConstantData, DWORD BoolCount) { D3DDevice_SetPixelShaderConstantB(pThis, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPixelShaderConstantF(D3DDevice *pThis, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetPixelShaderConstantF(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPixelShaderConstantI(D3DDevice *pThis, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount) { D3DDevice_SetPixelShaderConstantI(pThis, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetPixelShaderConstantB(D3DDevice *pThis, UINT StartRegister, BOOL *pConstantData, DWORD BoolCount) { D3DDevice_GetPixelShaderConstantB(pThis, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetPixelShaderConstantF(D3DDevice *pThis, UINT StartRegister, float *pConstantData, DWORD Vector4fCount) { D3DDevice_GetPixelShaderConstantF(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetPixelShaderConstantI(D3DDevice *pThis, UINT StartRegister, int *pConstantData, DWORD Vector4iCount) { D3DDevice_GetPixelShaderConstantI(pThis, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPixelShaderConstantF_NotInline(D3DDevice *pThis, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetPixelShaderConstantFN(pThis, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawRectPatch(D3DDevice *pThis, UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo) { D3DDevice_DrawRectPatch(pThis, Handle, pNumSegs, pRectPatchInfo); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawTriPatch(D3DDevice *pThis, UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo) { D3DDevice_DrawTriPatch(pThis, Handle, pNumSegs, pTriPatchInfo); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DeletePatch(D3DDevice *pThis, UINT Handle) { D3DDevice_DeletePatch(pThis, Handle); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetClipPlane(D3DDevice *pThis, DWORD Index, CONST float* pPlane) { D3DDevice_SetClipPlane(pThis, Index, pPlane); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetClipPlane(D3DDevice *pThis, DWORD Index, float* pPlane) { D3DDevice_GetClipPlane(pThis, Index, pPlane); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetNPatchMode(D3DDevice *pThis, float nSegments) { D3DDevice_SetNPatchMode(pThis, nSegments); D3DVOIDRETURN; }
D3DINLINE float   WINAPI IDirect3DDevice9_GetNPatchMode(D3DDevice *pThis) { return D3DDevice_GetNPatchMode(pThis); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateQuery(D3DDevice *pThis, D3DQUERYTYPE Type, D3DQuery** ppQuery) { if (ppQuery == NULL) { return ((Type == D3DQUERYTYPE_EVENT) || (Type == D3DQUERYTYPE_OCCLUSION)) ? S_OK : D3DERR_NOTAVAILABLE; } else { return (*ppQuery = D3DDevice_CreateQueryTiled(pThis, Type, 1)) != NULL ? S_OK : E_OUTOFMEMORY; } }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreateQueryTiled(D3DDevice *pThis, D3DQUERYTYPE Type, DWORD TileCount, D3DQuery** ppQuery) { if (ppQuery == NULL) { return ((Type == D3DQUERYTYPE_EVENT) || (Type == D3DQUERYTYPE_OCCLUSION)) ? S_OK : D3DERR_NOTAVAILABLE; } else { return (*ppQuery = D3DDevice_CreateQueryTiled(pThis, Type, TileCount)) != NULL ? S_OK : E_OUTOFMEMORY; } }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Resolve(D3DDevice* pThis, DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { D3DDevice_Resolve(pThis, Flags, pSourceRect, pDestTexture, pDestPoint, DestLevel, DestSliceOrFace, pClearColor, ClearZ, ClearStencil, pParameters); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_AcquireThreadOwnership(D3DDevice* pThis) { D3DDevice_AcquireThreadOwnership(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_ReleaseThreadOwnership(D3DDevice* pThis) { D3DDevice_ReleaseThreadOwnership(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetThreadOwnership(D3DDevice* pThis, DWORD ThreadID) { D3DDevice_SetThreadOwnership(pThis, ThreadID); D3DVOIDRETURN; }
D3DINLINE DWORD   WINAPI IDirect3DDevice9_QueryThreadOwnership(D3DDevice* pThis) { return D3DDevice_QueryThreadOwnership(pThis); }
D3DINLINE BOOL    WINAPI IDirect3DDevice9_IsBusy(D3DDevice *pThis) { return D3DDevice_IsBusy(pThis); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BlockUntilIdle(D3DDevice *pThis) { D3DDevice_BlockUntilIdle(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_InsertCallback(D3DDevice *pThis, D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD Context) { D3DDevice_InsertCallback(pThis, Type, pCallback, Context); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVerticalBlankCallback(D3DDevice *pThis, D3DVBLANKCALLBACK pCallback) { D3DDevice_SetVerticalBlankCallback(pThis, pCallback); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawTessellatedPrimitive(D3DDevice *pThis, D3DTESSPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { D3DDevice_DrawTessellatedVertices(pThis, PrimitiveType, StartVertex, D3DTESSVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_DrawIndexedTessellatedPrimitive(D3DDevice *pThis, D3DTESSPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT PrimitiveCount) { D3DDevice_DrawIndexedTessellatedVertices(pThis, PrimitiveType, BaseVertexIndex, StartIndex, D3DTESSVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_SetRingBufferParameters(D3DDevice *pThis, CONST D3DRING_BUFFER_PARAMETERS* pParameters) { return D3DDevice_SetRingBufferParameters(pThis, pParameters); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_XpsBegin(D3DDevice* pDevice, DWORD Flags) { D3DDevice_XpsBegin(pDevice, Flags); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_XpsEnd(D3DDevice* pDevice) { return D3DDevice_XpsEnd(pDevice); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_XpsSetCallback(D3DDevice* pDevice, XpsCallback pCallback, void* pContext, DWORD Flags) { D3DDevice_XpsSetCallback(pDevice, pCallback, pContext, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_XpsSubmit(D3DDevice* pDevice, DWORD InstanceCount, CONST void* pData, DWORD Size) { D3DDevice_XpsSubmit(pDevice, InstanceCount, pData, Size); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_BeginVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, UINT VertexStreamZeroStride, void** ppVertexData) { return (*ppVertexData = D3DDevice_BeginVertices(pDevice, PrimitiveType, VertexCount, VertexStreamZeroStride)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndVertices(D3DDevice *pDevice) { D3DDevice_EndVertices(pDevice); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_BeginIndexedVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT NumVertices, UINT IndexCount, D3DFORMAT IndexDataFormat, UINT VertexStreamZeroStride, void** ppIndexData, void** ppVertexData) { return D3DDevice_BeginIndexedVertices(pDevice, PrimitiveType, BaseVertexIndex, NumVertices, IndexCount, IndexDataFormat, VertexStreamZeroStride, ppIndexData, ppVertexData); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndIndexedVertices(D3DDevice *pDevice) { D3DDevice_EndIndexedVertices(pDevice); D3DVOIDRETURN; }
D3DINLINE DWORD   WINAPI IDirect3DDevice9_InsertFence(D3DDevice *pDevice) { return D3DDevice_InsertFence(pDevice); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BlockOnFence(D3DDevice *pDevice, DWORD Fence) { D3DDevice_BlockOnFence(Fence); D3DVOIDRETURN; }
D3DINLINE BOOL    WINAPI IDirect3DDevice9_IsFencePending(D3DDevice *pDevice, DWORD Fence) { return D3DDevice_IsFencePending(Fence); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetBlendState(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE BlendState) { D3DDevice_SetBlendState(pDevice, RenderTargetIndex, BlendState); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetBlendState(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE* pBlendState) { D3DDevice_GetBlendState(pDevice, RenderTargetIndex, pBlendState); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetVertexFetchConstant(D3DDevice *pDevice, UINT VertexFetchRegister, D3DVertexBuffer* pVertexBuffer, UINT Offset) { D3DDevice_SetVertexFetchConstant(pDevice, VertexFetchRegister, pVertexBuffer, Offset); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetTextureFetchConstant(D3DDevice *pDevice, UINT TextureFetchRegister, D3DBaseTexture* pTexture) { D3DDevice_SetTextureFetchConstant(pDevice, TextureFetchRegister, pTexture); D3DVOIDRETURN; }
D3DINLINE float   WINAPI IDirect3DDevice9_GetCounter(D3DDevice* pDevice, D3DCOUNTER CounterID) { return D3DDevice_GetCounter(pDevice, CounterID);}
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetSafeLevel(D3DDevice* pDevice, DWORD Flags, DWORD Level) { D3DDevice_SetSafeLevel(pDevice, Flags, Level); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetSafeLevel(D3DDevice* pDevice, DWORD* pFlags, DWORD* pLevel) { D3DDevice_GetSafeLevel(pDevice, pFlags, pLevel); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetHangCallback(D3DDevice* pDevice, D3DHANGCALLBACK pCallback) { D3DDevice_SetHangCallback(pDevice, pCallback); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginConditionalSurvey(D3DDevice* pDevice, DWORD Identifier, DWORD Flags) { D3DDevice_BeginConditionalSurvey(pDevice, Identifier, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndConditionalSurvey(D3DDevice* pDevice, DWORD Flags) {D3DDevice_EndConditionalSurvey(pDevice, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginConditionalRendering(D3DDevice* pDevice, DWORD Identifier) { D3DDevice_BeginConditionalRendering(pDevice, Identifier); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndConditionalRendering(D3DDevice* pDevice) { D3DDevice_EndConditionalRendering(pDevice); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_PersistDisplay(D3DDevice* pDevice, D3DTexture* pFrontBuffer, const D3DVIDEO_SCALER_PARAMETERS* pParameters) { return D3DDevice_PersistDisplay(pDevice, pFrontBuffer, pParameters); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GetPersistedTexture(D3DDevice* pDevice, D3DTexture** pFrontBuffer) { return D3DDevice_GetPersistedTexture(pDevice, pFrontBuffer); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Suspend(D3DDevice* pDevice) { D3DDevice_Suspend(pDevice); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_Resume(D3DDevice* pDevice) { D3DDevice_Resume(pDevice); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_CreatePerfCounters(D3DDevice *pDevice, D3DPerfCounters** ppCounters, UINT NumPasses) { return (*ppCounters = D3DDevice_CreatePerfCounters(pDevice, NumPasses)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EnablePerfCounters(D3DDevice* pDevice, BOOL Enable) { D3DDevice_EnablePerfCounters(pDevice, Enable); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetPerfCounterEvents(D3DDevice* pDevice, CONST D3DPERFCOUNTER_EVENTS* pEvents, DWORD Flags) { D3DDevice_SetPerfCounterEvents(pDevice, pEvents, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_QueryPerfCounters(D3DDevice* pDevice, D3DPerfCounters* pCounters, DWORD Flags) { D3DDevice_QueryPerfCounters(pDevice, pCounters, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetShaderGPRAllocation(D3DDevice* pDevice, DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount) { D3DDevice_SetShaderGPRAllocation(pDevice, Flags, VertexShaderCount, PixelShaderCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetShaderGPRAllocation(D3DDevice* pDevice, DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount) { D3DDevice_GetShaderGPRAllocation(pDevice, pFlags, pVertexShaderCount, pPixelShaderCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_SetScreenExtentQueryMode(D3DDevice* pDevice, D3DSCREENEXTENTQUERYMODE Mode) { D3DDevice_SetScreenExtentQueryMode(pDevice, Mode); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_GetScreenExtentQueryMode(D3DDevice* pDevice, D3DSCREENEXTENTQUERYMODE* pMode) { D3DDevice_GetScreenExtentQueryMode(pDevice, pMode); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginVertexShaderConstantF1(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount) { D3DDevice_BeginVertexShaderConstantF1(pDevice, StartRegister, ppConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndVertexShaderConstantF1(D3DDevice* pDevice) { D3DDevice_EndShaderConstantF1(pDevice); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_BeginPixelShaderConstantF1(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount) { D3DDevice_BeginPixelShaderConstantF1(pDevice, StartRegister, ppConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndPixelShaderConstantF1(D3DDevice* pDevice) { D3DDevice_EndShaderConstantF1(pDevice); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_BeginVertexShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount) { return D3DDevice_BeginShaderConstantF4(pDevice, FALSE, StartRegister, ppCachedConstantData, ppWriteCombinedConstantData, Vector4fCount); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndVertexShaderConstantF4(D3DDevice* pDevice) { D3DDevice_EndShaderConstantF4(pDevice); D3DVOIDRETURN; }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_BeginPixelShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount) { return D3DDevice_BeginShaderConstantF4(pDevice, TRUE, StartRegister, ppCachedConstantData, ppWriteCombinedConstantData, Vector4fCount); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_EndPixelShaderConstantF4(D3DDevice* pDevice) { D3DDevice_EndShaderConstantF4(pDevice); D3DVOIDRETURN; }
D3DINLINE DWORD   WINAPI IDirect3DDevice9_GetCurrentFence(D3DDevice* pDevice) { return D3DDevice_GetCurrentFence(pDevice); }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_InvalidateGpuCache(D3DDevice* pDevice, void* pBaseAddress, DWORD Size, DWORD Flags) { D3DDevice_InvalidateGpuCache(pDevice, pBaseAddress, Size, Flags ); D3DVOIDRETURN; }
D3DINLINE D3DVOID WINAPI IDirect3DDevice9_FlushHiZStencil(D3DDevice* pDevice, D3DFHZS_FLUSHTYPE FlushType) { D3DDevice_FlushHiZStencil(pDevice, FlushType); D3DVOIDRETURN; }

D3DINLINE void    WINAPI IDirect3DDevice9_GpuOwn(D3DDevice* pDevice, D3DTAG Tag) { D3DDevice_GpuOwn(pDevice, Tag); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuOwnSubset(D3DDevice* pDevice, D3DTAG Tag, DWORD StartIndex, DWORD EndIndex) { D3DDevice_GpuOwnSubset(pDevice, Tag, StartIndex, EndIndex); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuOwnVertexShaderConstantF(D3DDevice* pDevice, DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnVertexShaderConstantF(pDevice, StartRegister, Vector4fCount); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuOwnPixelShaderConstantF(D3DDevice* pDevice, DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnPixelShaderConstantF(pDevice, StartRegister, Vector4fCount); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuDisown(D3DDevice* pDevice, D3DTAG Tag) { D3DDevice_GpuDisown(pDevice, Tag); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuDisownSubset(D3DDevice* pDevice, D3DTAG Tag, DWORD StartIndex, DWORD EndIndex) { D3DDevice_GpuDisownSubset(pDevice, Tag, StartIndex, EndIndex); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuDisownVertexShaderConstantF(D3DDevice* pDevice, DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownVertexShaderConstantF(pDevice, StartRegister, Vector4fCount); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuDisownPixelShaderConstantF(D3DDevice* pDevice, DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownPixelShaderConstantF(pDevice, StartRegister, Vector4fCount); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuDisownAll(D3DDevice* pDevice) { D3DDevice_GpuDisownAll(pDevice); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GpuBeginVertexShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return (*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(pDevice, FALSE, StartRegister, Vector4fCount)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuEndVertexShaderConstantF4(D3DDevice* pDevice) { D3DDevice_GpuEndShaderConstantF4(pDevice); }
D3DINLINE HRESULT WINAPI IDirect3DDevice9_GpuBeginPixelShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return (*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(pDevice, TRUE, StartRegister, Vector4fCount)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuEndPixelShaderConstantF4(D3DDevice* pDevice) { D3DDevice_GpuEndShaderConstantF4(pDevice); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuLoadVertexShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, const void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4(pDevice, FALSE, StartRegister, pConstantData, Vector4fCount); }
D3DINLINE void    WINAPI IDirect3DDevice9_GpuLoadPixelShaderConstantF4(D3DDevice* pDevice, UINT StartRegister, const void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4(pDevice, TRUE, StartRegister, pConstantData, Vector4fCount); }

#ifdef __cplusplus

D3DMINLINE ULONG   WINAPI D3DDevice::AddRef() { return D3DDevice_AddRef(this); }
D3DMINLINE ULONG   WINAPI D3DDevice::Release() { return D3DDevice_Release(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetDirect3D(Direct3D **ppD3D9) { D3DDevice_GetDirect3D(this, ppD3D9); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetDeviceCaps(D3DCAPS9 *pCaps) { D3DDevice_GetDeviceCaps(this, pCaps); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetDisplayMode(UINT UnusedSwapChain, D3DDISPLAYMODE *pMode) { D3DDevice_GetDisplayMode(this, UnusedSwapChain, pMode); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) { D3DDevice_GetCreationParameters(this, pParameters); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters) { return D3DDevice_Reset(this, pPresentationParameters); }
D3DMINLINE D3DVOID WINAPI D3DDevice::Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, void *hUnusedDestWindowOverride, void *pUnusedDirtyRegion) { D3DDevice_Present(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetRasterStatus(UINT iUnusedSwapChain, D3DRASTER_STATUS *pRasterStatus) { D3DDevice_GetRasterStatus(this, pRasterStatus); D3DVOIDRETURN; }
D3DMINLINE void    WINAPI D3DDevice::SetGammaRamp(UINT iUnusedSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp) { D3DDevice_SetGammaRamp(this, Flags, pRamp); }
D3DMINLINE void    WINAPI D3DDevice::GetGammaRamp(UINT iUnusedSwapChain, D3DGAMMARAMP *pRamp) { D3DDevice_GetGammaRamp(this, pRamp); }
D3DMINLINE void    WINAPI D3DDevice::SetPWLGamma(DWORD Flags, CONST D3DPWLGAMMA *pRamp) { D3DDevice_SetPWLGamma(this, Flags, pRamp); }
D3DMINLINE void    WINAPI D3DDevice::GetPWLGamma(D3DPWLGAMMA *pRamp) { D3DDevice_GetPWLGamma(this, pRamp); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DTexture **ppTexture, HANDLE* pUnusedSharedHandle) { return (*ppTexture = (D3DTexture*) D3DDevice_CreateTexture(Width, Height, 1, Levels, Usage, Format, UnusedPool, D3DRTYPE_TEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DVolumeTexture **ppVolumeTexture, HANDLE* pUnusedSharedHandle) { return (*ppVolumeTexture = (D3DVolumeTexture*) D3DDevice_CreateTexture(Width, Height, Depth, Levels, Usage, Format, UnusedPool, D3DRTYPE_VOLUMETEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DCubeTexture **ppCubeTexture, HANDLE* pUnusedSharedHandle) { return (*ppCubeTexture = (D3DCubeTexture*) D3DDevice_CreateTexture(EdgeLength, EdgeLength, 6, Levels, Usage, Format, UnusedPool, D3DRTYPE_CUBETEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateArrayTexture(UINT Width, UINT Height, UINT ArraySize, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DArrayTexture **ppArrayTexture, HANDLE* pUnusedSharedHandle) { return (*ppArrayTexture = (D3DArrayTexture*) D3DDevice_CreateTexture(Width, Height, ArraySize, Levels, Usage, Format, UnusedPool, D3DRTYPE_ARRAYTEXTURE)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD UnusedFVF, D3DPOOL UnusedPool, D3DVertexBuffer **ppVertexBuffer, HANDLE* pUnusedSharedHandle) { return (*ppVertexBuffer = D3DDevice_CreateVertexBuffer(Length, Usage, UnusedPool)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DIndexBuffer **ppIndexBuffer, HANDLE* pUnusedSharedHandle) { return (*ppIndexBuffer = D3DDevice_CreateIndexBuffer(Length, Usage, Format, UnusedPool)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements, D3DVertexDeclaration **ppVertexDeclaration) { return (*ppVertexDeclaration = D3DDevice_CreateVertexDeclaration(pVertexElements)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedLockable, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return (*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedDiscard, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return (*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetRenderTarget(DWORD RenderTargetIndex, D3DSurface *pRenderTarget) { D3DDevice_SetRenderTarget(this, RenderTargetIndex, pRenderTarget); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::GetRenderTarget(DWORD RenderTargetIndex, D3DSurface **ppRenderTarget) { return (*ppRenderTarget = D3DDevice_GetRenderTarget(this, RenderTargetIndex)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetDepthStencilSurface(D3DSurface *pZStencilSurface) { D3DDevice_SetDepthStencilSurface(this, pZStencilSurface); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::GetDepthStencilSurface(D3DSurface **ppZStencilSurface) { return (*ppZStencilSurface = D3DDevice_GetDepthStencilSurface(this)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DMINLINE HRESULT WINAPI D3DDevice::GetBackBuffer(UINT UnusedSwapChain, UINT iUnusedBackBuffer, UINT UnusedType, D3DSurface **ppBackBuffer) { return (*ppBackBuffer = D3DDevice_GetBackBuffer(this)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DMINLINE HRESULT WINAPI D3DDevice::GetFrontBuffer(D3DBaseTexture **ppFrontBuffer) { return (*ppFrontBuffer = D3DDevice_GetFrontBuffer(this)) != NULL ? S_OK : D3DERR_NOTFOUND; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginScene() { D3DDevice_BeginScene(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndScene() { D3DDevice_EndScene(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { D3DDevice_Clear(this, Count, pRects, Flags, Color, Z, Stencil, FALSE); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::ClearF(DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil) { D3DDevice_ClearF(this, Flags, pRect, pColor, Z, Stencil); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SynchronizeToPresentationInterval() { D3DDevice_SynchronizeToPresentationInterval(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Swap(D3DBaseTexture *pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS *pParameters) { D3DDevice_Swap(this, pFrontBuffer, pParameters); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::RenderSystemUI() { D3DDevice_RenderSystemUI(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::QueryBufferSpace(DWORD* pUsed, DWORD* pRemaining) { D3DDevice_QueryBufferSpace(this, pUsed, pRemaining); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPredication(DWORD PredicationMask) { D3DDevice_SetPredication(this, PredicationMask); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPatchablePredication(DWORD PredicationMask, DWORD Identifier) { D3DDevice_SetPatchablePredication(this, PredicationMask, Identifier); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginTiling(DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil) { D3DDevice_BeginTiling(this, Flags, Count, pTileRects, pClearColor, ClearZ, ClearStencil); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::EndTiling(DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { return D3DDevice_EndTiling(this, ResolveFlags, pResolveRects, pDestTexture, pClearColor, ClearZ, ClearStencil, pParameters); }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginZPass(DWORD Flags) { D3DDevice_BeginZPass(this, Flags); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::EndZPass() { return D3DDevice_EndZPass(this); }
D3DMINLINE HRESULT WINAPI D3DDevice::InvokeRenderPass() { return D3DDevice_InvokeRenderPass(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginExport(DWORD Index, D3DResource* pResource) { D3DDevice_BeginExport(this, Index, pResource); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndExport(DWORD Index, D3DResource* pResource, DWORD Flags) { D3DDevice_EndExport(this, Index, pResource, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SynchronizeExport(D3DResource* pResource) { D3DDevice_SynchronizeExport(this, pResource); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetViewport(CONST D3DVIEWPORT9 *pViewport) { D3DDevice_SetViewport(this, pViewport); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetViewport(D3DVIEWPORT9 *pViewport) { D3DDevice_GetViewport(this, pViewport); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { D3DDevice_SetRenderState(this, State, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetRenderState_Inline(D3DRENDERSTATETYPE State, DWORD Value) { D3DDevice_SetRenderState_Inline(this, State, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue) { D3DDevice_GetRenderState(this, State, pValue); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateStateBlock(D3DSTATEBLOCKTYPE Type, D3DStateBlock **ppSB) { return (*ppSB = D3DDevice_CreateStateBlock(this, Type)) != NULL ? S_OK : E_OUTOFMEMORY;}
D3DMINLINE D3DVOID WINAPI D3DDevice::GetTexture(DWORD Sampler, D3DBaseTexture **ppTexture) { *ppTexture = D3DDevice_GetTexture(this, Sampler); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetTexture(DWORD Sampler, D3DBaseTexture *pTexture) { D3DDevice_SetTexture(this, Sampler, pTexture); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { D3DDevice_GetSamplerState(this, Sampler, Type, pValue); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { D3DDevice_SetSamplerState(this, Sampler, Type, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetSamplerState_Inline(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { D3DDevice_SetSamplerState_Inline(this, Sampler, Type, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawVertices(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount) { D3DDevice_DrawVertices(this, PrimitiveType, StartVertex, VertexCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedVertices(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT VertexCount) { D3DDevice_DrawIndexedVertices(this, PrimitiveType, BaseVertexIndex, StartIndex, VertexCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawVerticesUP(D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawVerticesUP(this, PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedVerticesUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT IndexCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawIndexedVerticesUP(this, PrimitiveType, MinVertexIndex, NumVertices, IndexCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { D3DDevice_DrawVertices(this, PrimitiveType, StartVertex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT UnusedMinIndex, UINT UnusedNumIndices, UINT StartIndex, UINT PrimitiveCount) { D3DDevice_DrawIndexedVertices(this, PrimitiveType, BaseVertexIndex, StartIndex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawVerticesUP(this, PrimitiveType, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount), pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawIndexedVerticesUP(this, PrimitiveType, MinVertexIndex, NumVertices, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount), pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexDeclaration(D3DVertexDeclaration *pDecl) { D3DDevice_SetVertexDeclaration(this, pDecl); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetVertexDeclaration(D3DVertexDeclaration **ppDecl) { *ppDecl = D3DDevice_GetVertexDeclaration(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetScissorRect(CONST RECT* pRect) { D3DDevice_SetScissorRect(this, pRect); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetScissorRect(RECT* pRect) { D3DDevice_GetScissorRect(this, pRect); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetFVF(DWORD FVF) { D3DDevice_SetFVF(this, FVF); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetFVF(DWORD* pFVF) { *pFVF = D3DDevice_GetFVF(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVertexShader(CONST DWORD *pFunction, IDirect3DVertexShader9** ppShader) { return (*ppShader = D3DDevice_CreateVertexShader(pFunction)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShader(D3DVertexShader *pShader) { D3DDevice_SetVertexShader(this, pShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetVertexShader(D3DVertexShader **ppShader) { D3DDevice_GetVertexShader(this, ppShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetVertexShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetVertexShaderConstantF(UINT StartRegister, float *pConstantData, DWORD Vector4fCount) { D3DDevice_GetVertexShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount) { D3DDevice_SetVertexShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetVertexShaderConstantI(UINT StartRegister, int *pConstantData, DWORD Vector4iCount) { D3DDevice_GetVertexShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount) { D3DDevice_SetVertexShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetVertexShaderConstantB(UINT StartRegister, BOOL *pConstantData, DWORD BoolCount) { D3DDevice_GetVertexShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantF_NotInline(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetVertexShaderConstantFN(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetStreamSource(UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride) { D3DDevice_SetStreamSource(this, StreamNumber, pStreamData, OffsetInBytes, Stride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetStreamSource(UINT StreamNumber, D3DVertexBuffer **ppStreamData, UINT *pOffsetInBytes, UINT *pStride) { *ppStreamData = D3DDevice_GetStreamSource(this, StreamNumber, pOffsetInBytes, pStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetIndices(D3DIndexBuffer* pIndexData) { D3DDevice_SetIndices(this, pIndexData); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetIndices(D3DIndexBuffer** ppIndexData) { *ppIndexData = D3DDevice_GetIndices(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreatePixelShader(CONST DWORD *pFunction, IDirect3DPixelShader9** ppShader) { return (*ppShader = D3DDevice_CreatePixelShader(pFunction)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShader(D3DPixelShader* pShader) { D3DDevice_SetPixelShader(this, pShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetPixelShader(D3DPixelShader** ppShader) { D3DDevice_GetPixelShader(this, ppShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetPixelShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, DWORD Vector4fCount) { D3DDevice_GetPixelShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount) { D3DDevice_SetPixelShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, DWORD Vector4iCount) { D3DDevice_GetPixelShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount) { D3DDevice_SetPixelShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, DWORD BoolCount) { D3DDevice_GetPixelShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantF_NotInline(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetPixelShaderConstantFN(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo) { D3DDevice_DrawRectPatch(this, Handle, pNumSegs, pRectPatchInfo); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo) { D3DDevice_DrawTriPatch(this, Handle, pNumSegs, pTriPatchInfo); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DeletePatch(UINT Handle) { D3DDevice_DeletePatch(this, Handle); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetClipPlane(DWORD Index, CONST float* pPlane) { D3DDevice_SetClipPlane(this, Index, pPlane); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetClipPlane(DWORD Index, float* pPlane) { D3DDevice_GetClipPlane(this, Index, pPlane); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetNPatchMode(float nSegments) { D3DDevice_SetNPatchMode(this, nSegments); D3DVOIDRETURN; }
D3DMINLINE float   WINAPI D3DDevice::GetNPatchMode() { return D3DDevice_GetNPatchMode(this); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { if (ppQuery == NULL) { return ((Type == D3DQUERYTYPE_EVENT) || (Type == D3DQUERYTYPE_OCCLUSION)) ? S_OK : D3DERR_NOTAVAILABLE; } else { return (*ppQuery = D3DDevice_CreateQueryTiled(this, Type, 1)) != NULL ? S_OK : E_OUTOFMEMORY; } }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateQueryTiled(D3DQUERYTYPE Type, DWORD TileCount, IDirect3DQuery9** ppQuery) { if (ppQuery == NULL) { return ((Type == D3DQUERYTYPE_EVENT) || (Type == D3DQUERYTYPE_OCCLUSION)) ? S_OK : D3DERR_NOTAVAILABLE; } else { return (*ppQuery = D3DDevice_CreateQueryTiled(this, Type, TileCount)) != NULL ? S_OK : E_OUTOFMEMORY; } }
D3DMINLINE D3DVOID WINAPI D3DDevice::Resolve(DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { D3DDevice_Resolve(this, Flags, pSourceRect, pDestTexture, pDestPoint, DestLevel, DestSliceOrFace, pClearColor, ClearZ, ClearStencil, pParameters); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::AcquireThreadOwnership() { D3DDevice_AcquireThreadOwnership(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::ReleaseThreadOwnership() { D3DDevice_ReleaseThreadOwnership(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetThreadOwnership(DWORD ThreadID) { D3DDevice_SetThreadOwnership(this, ThreadID); D3DVOIDRETURN; }
D3DMINLINE DWORD   WINAPI D3DDevice::QueryThreadOwnership() { return D3DDevice_QueryThreadOwnership(this); }
D3DMINLINE BOOL    WINAPI D3DDevice::IsBusy() { return D3DDevice_IsBusy(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::BlockUntilIdle() { D3DDevice_BlockUntilIdle(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::InsertCallback(D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD Context) { D3DDevice_InsertCallback(this, Type, pCallback, Context); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVerticalBlankCallback(D3DVBLANKCALLBACK pCallback) { D3DDevice_SetVerticalBlankCallback(this, pCallback); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawTessellatedPrimitive(D3DTESSPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { D3DDevice_DrawTessellatedVertices(this, PrimitiveType, StartVertex, D3DTESSVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedTessellatedPrimitive(D3DTESSPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT PrimitiveCount) { D3DDevice_DrawIndexedTessellatedVertices(this, PrimitiveType, BaseVertexIndex, StartIndex, D3DTESSVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::SetRingBufferParameters(CONST D3DRING_BUFFER_PARAMETERS* pParameters) { return D3DDevice_SetRingBufferParameters(this, pParameters); }
D3DMINLINE D3DVOID WINAPI D3DDevice::XpsBegin(DWORD Flags) { D3DDevice_XpsBegin(this, Flags); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::XpsEnd() { return D3DDevice_XpsEnd(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::XpsSetCallback(XpsCallback pCallback, void* pContext, DWORD Flags) { D3DDevice_XpsSetCallback(this, pCallback, pContext, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::XpsSubmit(DWORD InstanceCount, CONST void* pData, DWORD Size) { D3DDevice_XpsSubmit(this, InstanceCount, pData, Size); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::BeginVertices(D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, UINT VertexStreamZeroStride, void** ppVertexData) { return (*ppVertexData = D3DDevice_BeginVertices(this, PrimitiveType, VertexCount, VertexStreamZeroStride)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndVertices() { D3DDevice_EndVertices(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::BeginIndexedVertices(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT NumVertices, UINT IndexCount, D3DFORMAT IndexDataFormat, UINT VertexStreamZeroStride, void** ppIndexData, void** ppVertexData) { return D3DDevice_BeginIndexedVertices(this, PrimitiveType, BaseVertexIndex, NumVertices, IndexCount, IndexDataFormat, VertexStreamZeroStride, ppIndexData, ppVertexData); }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndIndexedVertices() { D3DDevice_EndIndexedVertices(this); D3DVOIDRETURN; }
D3DMINLINE DWORD   WINAPI D3DDevice::InsertFence() { return D3DDevice_InsertFence(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::BlockOnFence(DWORD Fence) { D3DDevice_BlockOnFence(Fence); D3DVOIDRETURN; }
D3DMINLINE BOOL    WINAPI D3DDevice::IsFencePending(DWORD Fence) { return D3DDevice_IsFencePending(Fence); }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE BlendState) { D3DDevice_SetBlendState(this, RenderTargetIndex, BlendState); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE* pBlendState) { D3DDevice_GetBlendState(this, RenderTargetIndex, pBlendState); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexFetchConstant(UINT VertexFetchRegister, D3DVertexBuffer* pVertexBuffer, UINT Offset) { D3DDevice_SetVertexFetchConstant(this, VertexFetchRegister, pVertexBuffer, Offset); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetTextureFetchConstant(UINT TextureFetchRegister, D3DBaseTexture* pTexture) { D3DDevice_SetTextureFetchConstant(this, TextureFetchRegister, pTexture); D3DVOIDRETURN; }
D3DMINLINE float   WINAPI D3DDevice::GetCounter(D3DCOUNTER CounterID) { return D3DDevice_GetCounter(this, CounterID);}
D3DMINLINE D3DVOID WINAPI D3DDevice::SetSafeLevel(DWORD Flags, DWORD Level) { D3DDevice_SetSafeLevel(this, Flags, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetSafeLevel(DWORD* pFlags, DWORD* pLevel) { D3DDevice_GetSafeLevel(this, pFlags, pLevel); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetHangCallback(D3DHANGCALLBACK pCallback) { D3DDevice_SetHangCallback(this, pCallback); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginConditionalSurvey(DWORD Identifier, DWORD Flags) { D3DDevice_BeginConditionalSurvey(this, Identifier, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndConditionalSurvey(DWORD Flags) {D3DDevice_EndConditionalSurvey(this, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginConditionalRendering(DWORD Identifier) { D3DDevice_BeginConditionalRendering(this, Identifier); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndConditionalRendering() { D3DDevice_EndConditionalRendering(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::PersistDisplay(D3DTexture* pFrontBuffer, const D3DVIDEO_SCALER_PARAMETERS* pParameters) { return D3DDevice_PersistDisplay(this, pFrontBuffer, pParameters); }
D3DMINLINE HRESULT WINAPI D3DDevice::GetPersistedTexture(D3DTexture** pFrontBuffer) { return D3DDevice_GetPersistedTexture(this, pFrontBuffer); }
D3DMINLINE D3DVOID WINAPI D3DDevice::Suspend() { D3DDevice_Suspend(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Resume() { D3DDevice_Resume(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::CreatePerfCounters(D3DPerfCounters** ppCounters, UINT NumPasses) { return (*ppCounters = D3DDevice_CreatePerfCounters(this, NumPasses)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EnablePerfCounters( BOOL Enable) { D3DDevice_EnablePerfCounters(this, Enable); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPerfCounterEvents(CONST D3DPERFCOUNTER_EVENTS* pEvents, DWORD Flags) { D3DDevice_SetPerfCounterEvents(this, pEvents, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::QueryPerfCounters(D3DPerfCounters* pCounters, DWORD Flags) { D3DDevice_QueryPerfCounters(this, pCounters, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetShaderGPRAllocation(DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount) { D3DDevice_SetShaderGPRAllocation(this, Flags, VertexShaderCount, PixelShaderCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetShaderGPRAllocation(DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount) { D3DDevice_GetShaderGPRAllocation(this, pFlags, pVertexShaderCount, pPixelShaderCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetScreenExtentQueryMode(D3DSCREENEXTENTQUERYMODE Mode) { D3DDevice_SetScreenExtentQueryMode(this, Mode); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetScreenExtentQueryMode(D3DSCREENEXTENTQUERYMODE* pMode) { D3DDevice_GetScreenExtentQueryMode(this, pMode); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginVertexShaderConstantF1(UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount) { D3DDevice_BeginVertexShaderConstantF1(this, StartRegister, ppConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndVertexShaderConstantF1() { D3DDevice_EndShaderConstantF1(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginPixelShaderConstantF1(UINT StartRegister, D3DVECTOR4** ppConstantData, UINT Vector4fCount) { D3DDevice_BeginPixelShaderConstantF1(this, StartRegister, ppConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndPixelShaderConstantF1() { D3DDevice_EndShaderConstantF1(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::BeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount) { return D3DDevice_BeginShaderConstantF4(this, FALSE, StartRegister, ppCachedConstantData, ppWriteCombinedConstantData, Vector4fCount); }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndVertexShaderConstantF4() { D3DDevice_EndShaderConstantF4(this); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::BeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppCachedConstantData, D3DVECTOR4** ppWriteCombinedConstantData, UINT Vector4fCount) { return D3DDevice_BeginShaderConstantF4(this, TRUE, StartRegister, ppCachedConstantData, ppWriteCombinedConstantData, Vector4fCount); }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndPixelShaderConstantF4() { D3DDevice_EndShaderConstantF4(this); D3DVOIDRETURN; }
D3DMINLINE DWORD   WINAPI D3DDevice::GetCurrentFence() { return D3DDevice_GetCurrentFence(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::InvalidateGpuCache(void* pBaseAddress, DWORD Size, DWORD Flags) { D3DDevice_InvalidateGpuCache(this, pBaseAddress, Size, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::FlushHiZStencil(D3DFHZS_FLUSHTYPE FlushType) { D3DDevice_FlushHiZStencil(this, FlushType); D3DVOIDRETURN; }

D3DMINLINE void    WINAPI D3DDevice::GpuOwn(D3DTAG Tag) { D3DDevice_GpuOwn(this, Tag); }
D3DMINLINE void    WINAPI D3DDevice::GpuOwnSubset(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex) { D3DDevice_GpuOwnSubset(this, Tag, StartIndex, EndIndex); }
D3DMINLINE void    WINAPI D3DDevice::GpuOwnVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnVertexShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuOwnPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnPixelShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisown(D3DTAG Tag) { D3DDevice_GpuDisown(this, Tag); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownSubset(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex) { D3DDevice_GpuDisownSubset(this, Tag, StartIndex, EndIndex); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownVertexShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownPixelShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownAll() { D3DDevice_GpuDisownAll(this); }
D3DMINLINE HRESULT WINAPI D3DDevice::GpuBeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return (*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(this, FALSE, StartRegister, Vector4fCount)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE void    WINAPI D3DDevice::GpuEndVertexShaderConstantF4() { D3DDevice_GpuEndShaderConstantF4(this); }
D3DMINLINE HRESULT WINAPI D3DDevice::GpuBeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return (*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(this, TRUE, StartRegister, Vector4fCount)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE void    WINAPI D3DDevice::GpuEndPixelShaderConstantF4() { D3DDevice_GpuEndShaderConstantF4(this); }
D3DMINLINE void    WINAPI D3DDevice::GpuLoadVertexShaderConstantF4(UINT StartRegister, const void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4(this, FALSE, StartRegister, pConstantData, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuLoadPixelShaderConstantF4(UINT StartRegister, const void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4(this, TRUE, StartRegister, pConstantData, Vector4fCount); }

#endif __cplusplus

/* D3DPerfCounters */

ULONG                       WINAPI D3DPerfCounters_AddRef(D3DPerfCounters *pThis);
ULONG                       WINAPI D3DPerfCounters_Release(D3DPerfCounters *pThis);
void                        WINAPI D3DPerfCounters_GetDevice(D3DPerfCounters *pThis, D3DDevice **ppDevice);
BOOL                        WINAPI D3DPerfCounters_IsBusy(D3DPerfCounters *pThis);
void                        WINAPI D3DPerfCounters_BlockUntilNotBusy(D3DPerfCounters *pThis);
UINT                        WINAPI D3DPerfCounters_GetNumPasses(D3DPerfCounters* pThis);
HRESULT                     WINAPI D3DPerfCounters_GetValues(D3DPerfCounters *pThis, D3DPERFCOUNTER_VALUES* pValues, UINT PassIndex, DWORD* pPassType);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DPerfCounters9_AddRef(D3DPerfCounters *pThis) { return D3DPerfCounters_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DPerfCounters9_Release(D3DPerfCounters *pThis) { return D3DPerfCounters_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DPerfCounters9_GetDevice(D3DPerfCounters *pThis, D3DDevice **ppDevice) { D3DPerfCounters_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE BOOL              WINAPI IDirect3DPerfCounters9_IsBusy(D3DPerfCounters *pThis) { return D3DPerfCounters_IsBusy(pThis); }
D3DINLINE VOID              WINAPI IDirect3DPerfCounters9_BlockUntilNotBusy(D3DPerfCounters *pThis) { D3DPerfCounters_BlockUntilNotBusy(pThis); }
D3DINLINE UINT              WINAPI IDirect3DPerfCounters9_GetNumPasses(D3DPerfCounters* pThis) { return D3DPerfCounters_GetNumPasses(pThis); }
D3DINLINE HRESULT           WINAPI IDirect3DPerfCounters9_GetValues(D3DPerfCounters *pThis, D3DPERFCOUNTER_VALUES* pValues, UINT PassIndex, DWORD* pPassType) { return D3DPerfCounters_GetValues(pThis, pValues, PassIndex, pPassType); }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DPerfCounters::AddRef() { return D3DPerfCounters_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DPerfCounters::Release() { return D3DPerfCounters_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DPerfCounters::GetDevice(D3DDevice **ppDevice) { D3DPerfCounters_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE BOOL             WINAPI D3DPerfCounters::IsBusy() { return D3DPerfCounters_IsBusy(this); }
D3DMINLINE VOID             WINAPI D3DPerfCounters::BlockUntilNotBusy() { D3DPerfCounters_BlockUntilNotBusy(this); }
D3DMINLINE UINT             WINAPI D3DPerfCounters::GetNumPasses() {return D3DPerfCounters_GetNumPasses(this); }
D3DMINLINE HRESULT          WINAPI D3DPerfCounters::GetValues( D3DPERFCOUNTER_VALUES* pValues, UINT PassIndex, DWORD* pPassType) { return D3DPerfCounters_GetValues(this, pValues, PassIndex, pPassType); }

#endif


/* D3DQuery */

ULONG                       WINAPI D3DQuery_AddRef(D3DQuery *pThis);
ULONG                       WINAPI D3DQuery_Release(D3DQuery *pThis);
void                        WINAPI D3DQuery_GetDevice(D3DQuery *pThis, D3DDevice **ppDevice);
D3DQUERYTYPE                WINAPI D3DQuery_GetType(D3DQuery *pThis);
DWORD                       WINAPI D3DQuery_GetDataSize(D3DQuery *pThis);
void                        WINAPI D3DQuery_Issue(D3DQuery *pThis, DWORD IssueFlags);
HRESULT                     WINAPI D3DQuery_GetData(D3DQuery *pThis, void* pData, DWORD Size, DWORD GetDataFlags);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DQuery9_AddRef(D3DQuery *pThis) { return D3DQuery_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DQuery9_Release(D3DQuery *pThis) { return D3DQuery_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DQuery9_GetDevice(D3DQuery *pThis, D3DDevice **ppDevice) { D3DQuery_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DQUERYTYPE      WINAPI IDirect3DQuery9_GetType(D3DQuery *pThis) { return D3DQuery_GetType(pThis); }
D3DINLINE DWORD             WINAPI IDirect3DQuery9_GetDataSize(D3DQuery *pThis) { return D3DQuery_GetDataSize(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DQuery9_Issue(D3DQuery *pThis, DWORD IssueFlags) { D3DQuery_Issue(pThis, IssueFlags); D3DVOIDRETURN; }
D3DINLINE HRESULT           WINAPI IDirect3DQuery9_GetData(D3DQuery *pThis, void* pData, DWORD Size, DWORD GetDataFlags) { return D3DQuery_GetData(pThis, pData, Size, GetDataFlags); }


#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DQuery::AddRef() { return D3DQuery_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DQuery::Release() { return D3DQuery_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DQuery::GetDevice(D3DDevice **ppDevice) { D3DQuery_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE D3DQUERYTYPE     WINAPI D3DQuery::GetType() { return D3DQuery_GetType(this); }
D3DMINLINE DWORD            WINAPI D3DQuery::GetDataSize() { return D3DQuery_GetDataSize(this); }
D3DMINLINE D3DVOID          WINAPI D3DQuery::Issue(DWORD IssueFlags) { D3DQuery_Issue(this, IssueFlags); D3DVOIDRETURN; }
D3DMINLINE HRESULT          WINAPI D3DQuery::GetData(void* pData, DWORD Size, DWORD GetDataFlags) { return D3DQuery_GetData(this, pData, Size, GetDataFlags); }

#endif

/* D3DStateBlock */

ULONG                       WINAPI D3DStateBlock_AddRef(D3DStateBlock *pThis);
ULONG                       WINAPI D3DStateBlock_Release(D3DStateBlock *pThis);
void                        WINAPI D3DStateBlock_GetDevice(D3DStateBlock *pThis, D3DDevice **ppDevice);
void                        WINAPI D3DStateBlock_Capture(D3DStateBlock *pThis);
void                        WINAPI D3DStateBlock_Apply(D3DStateBlock *pThis);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DStateBlock9_AddRef(D3DStateBlock *pThis) { return D3DStateBlock_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DStateBlock9_Release(D3DStateBlock *pThis) { return D3DStateBlock_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DStateBlock9_GetDevice(D3DStateBlock *pThis, D3DDevice **ppDevice) { D3DStateBlock_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DStateBlock9_Capture(D3DStateBlock *pThis) { D3DStateBlock_Capture(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DStateBlock9_Apply(D3DStateBlock *pThis) { D3DStateBlock_Apply(pThis); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DStateBlock::AddRef() { return D3DStateBlock_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DStateBlock::Release() { return D3DStateBlock_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DStateBlock::GetDevice(D3DDevice **ppDevice) { D3DStateBlock_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DStateBlock::Capture() { D3DStateBlock_Capture(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DStateBlock::Apply() { D3DStateBlock_Apply(this); D3DVOIDRETURN; }

#endif

/* D3DVertexDeclaration */

ULONG                       WINAPI D3DVertexDeclaration_AddRef(D3DVertexDeclaration *pThis);
ULONG                       WINAPI D3DVertexDeclaration_Release(D3DVertexDeclaration *pThis);
void                        WINAPI D3DVertexDeclaration_GetDeclaration(D3DVertexDeclaration *pThis, D3DVERTEXELEMENT9 *pDecl, UINT *pNumElements);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DVertexDeclaration9_AddRef(D3DVertexDeclaration *pThis) { return D3DVertexDeclaration_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DVertexDeclaration9_Release(D3DVertexDeclaration *pThis) { return D3DVertexDeclaration_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexDeclaration9_GetDeclaration(D3DVertexDeclaration *pThis, D3DVERTEXELEMENT9 *pDecl, UINT *pNumElements) { D3DVertexDeclaration_GetDeclaration(pThis, pDecl, pNumElements); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DVertexDeclaration::AddRef() { return D3DVertexDeclaration_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DVertexDeclaration::Release() { return D3DVertexDeclaration_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DVertexDeclaration::GetDeclaration(D3DVERTEXELEMENT9 *pDecl, UINT *pNumElements) { D3DVertexDeclaration_GetDeclaration(this, pDecl, pNumElements); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DVertexShader */

ULONG                       WINAPI D3DVertexShader_AddRef(D3DVertexShader *pThis);
ULONG                       WINAPI D3DVertexShader_Release(D3DVertexShader *pThis);
void                        WINAPI D3DVertexShader_GetDevice(D3DVertexShader *pThis, D3DDevice **ppDevice);
void                        WINAPI D3DVertexShader_GetFunction(D3DVertexShader *pThis, void* pData, UINT* pSizeOfData);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DVertexShader9_AddRef(D3DVertexShader *pThis) { return D3DVertexShader_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DVertexShader9_Release(D3DVertexShader *pThis) { return D3DVertexShader_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexShader9_GetDevice(D3DVertexShader *pThis, D3DDevice **ppDevice) { D3DVertexShader_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexShader9_GetFunction(D3DVertexShader *pThis, void* pData, UINT* pSizeOfData) { D3DVertexShader_GetFunction(pThis, pData, pSizeOfData); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DVertexShader::AddRef() { return D3DVertexShader_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DVertexShader::Release() { return D3DVertexShader_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DVertexShader::GetDevice(D3DDevice **ppDevice) { D3DVertexShader_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVertexShader::GetFunction(void* pData, UINT* pSizeOfData) { D3DVertexShader_GetFunction(this, pData, pSizeOfData); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DPixelShader */

ULONG                       WINAPI D3DPixelShader_AddRef(D3DPixelShader *pThis);
ULONG                       WINAPI D3DPixelShader_Release(D3DPixelShader *pThis);
void                        WINAPI D3DPixelShader_GetDevice(D3DPixelShader *pThis, D3DDevice **ppDevice);
void                        WINAPI D3DPixelShader_GetFunction(D3DPixelShader *pThis, void* pData, UINT* pSizeOfData);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DPixelShader9_AddRef(D3DPixelShader *pThis) { return D3DPixelShader_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DPixelShader9_Release(D3DPixelShader *pThis) { return D3DPixelShader_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DPixelShader9_GetDevice(D3DPixelShader *pThis, D3DDevice **ppDevice) { D3DPixelShader_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DPixelShader9_GetFunction(D3DPixelShader *pThis, void* pData, UINT* pSizeOfData) { D3DPixelShader_GetFunction(pThis, pData, pSizeOfData); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DPixelShader::AddRef() { return D3DPixelShader_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DPixelShader::Release() { return D3DPixelShader_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DPixelShader::GetDevice(D3DDevice **ppDevice) { D3DPixelShader_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DPixelShader::GetFunction(void* pData, UINT* pSizeOfData) { D3DPixelShader_GetFunction(this, pData, pSizeOfData); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DResource */

ULONG                       WINAPI D3DResource_AddRef(D3DResource *pThis);
ULONG                       WINAPI D3DResource_Release(D3DResource *pThis);
void                        WINAPI D3DResource_GetDevice(D3DResource *pThis, D3DDevice **ppDevice);
D3DRESOURCETYPE             WINAPI D3DResource_GetType(D3DResource *pThis);
BOOL                        WINAPI D3DResource_IsBusy(D3DResource *pThis);
void                        WINAPI D3DResource_BlockUntilNotBusy(D3DResource *pThis);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DResource9_AddRef(D3DResource *pThis) { return D3DResource_AddRef(pThis); }
D3DINLINE ULONG             WINAPI IDirect3DResource9_Release(D3DResource *pThis) { return D3DResource_Release(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DResource9_GetDevice(D3DResource *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice(pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DResource9_GetType(D3DResource *pThis) { return D3DResource_GetType(pThis); }
D3DINLINE BOOL              WINAPI IDirect3DResource9_IsBusy(D3DResource *pThis) { return D3DResource_IsBusy(pThis); }
D3DINLINE void              WINAPI IDirect3DResource9_BlockUntilNotBusy(D3DResource *pThis) { D3DResource_BlockUntilNotBusy(pThis); }

#ifdef __cplusplus

D3DMINLINE ULONG            WINAPI D3DResource::AddRef() { return D3DResource_AddRef(this); }
D3DMINLINE ULONG            WINAPI D3DResource::Release() { return D3DResource_Release(this); }
D3DMINLINE D3DVOID          WINAPI D3DResource::GetDevice(D3DDevice **ppDevice) { D3DResource_GetDevice(this, ppDevice); D3DVOIDRETURN; }
D3DMINLINE D3DRESOURCETYPE  WINAPI D3DResource::GetType() { return D3DResource_GetType(this); }
D3DMINLINE BOOL             WINAPI D3DResource::IsBusy() { return D3DResource_IsBusy(this); }
D3DMINLINE void             WINAPI D3DResource::BlockUntilNotBusy() { D3DResource_BlockUntilNotBusy(this); }

#endif __cplusplus

/* D3DBaseTexture */

D3DINLINE ULONG             WINAPI D3DBaseTexture_AddRef(D3DBaseTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DBaseTexture_Release(D3DBaseTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DBaseTexture_GetDevice(D3DBaseTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DBaseTexture_GetType(D3DBaseTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DBaseTexture_IsBusy(D3DBaseTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DBaseTexture_BlockUntilNotBusy(D3DBaseTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }

DWORD                       WINAPI D3DBaseTexture_GetLevelCount(D3DBaseTexture *pThis);
void                        WINAPI D3DBaseTexture_GetTailDesc(D3DBaseTexture *pThis, D3DMIPTAIL_DESC *pDesc);
void                        WINAPI D3DBaseTexture_LockTail(D3DBaseTexture *pThis, UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags);
void                        WINAPI D3DBaseTexture_UnlockTail(D3DBaseTexture *pThis, UINT ArrayIndex);

// Compatibility wrappers.

D3DINLINE ULONG             WINAPI IDirect3DBaseTexture9_AddRef(D3DBaseTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DBaseTexture9_Release(D3DBaseTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DBaseTexture9_GetDevice(D3DBaseTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DBaseTexture9_GetType(D3DBaseTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DBaseTexture9_IsBusy(D3DBaseTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DBaseTexture9_BlockUntilNotBusy(D3DBaseTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DBaseTexture9_GetLevelCount(D3DBaseTexture *pThis) { return D3DBaseTexture_GetLevelCount(pThis); }
D3DINLINE void              WINAPI IDirect3DBaseTexture9_GetTailDesc(D3DBaseTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc(pThis, pDesc); }
D3DINLINE void              WINAPI IDirect3DBaseTexture9_LockTail(D3DBaseTexture *pThis, UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail(pThis, ArrayIndex, pLockedTail, Flags); }
D3DINLINE void              WINAPI IDirect3DBaseTexture9_UnlockTail(D3DBaseTexture *pThis, UINT ArrayIndex) { D3DBaseTexture_UnlockTail(pThis, ArrayIndex); }

#ifdef __cplusplus

D3DMINLINE DWORD            WINAPI D3DBaseTexture::GetLevelCount() { return D3DBaseTexture_GetLevelCount(this); }
D3DMINLINE D3DVOID          WINAPI D3DBaseTexture::GetTailDesc(D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc(this, pDesc); }
D3DMINLINE D3DVOID          WINAPI D3DBaseTexture::LockTail(UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail(this, ArrayIndex, pLockedTail, Flags); }
D3DMINLINE D3DVOID          WINAPI D3DBaseTexture::UnlockTail(UINT ArrayIndex) { D3DBaseTexture_UnlockTail(this, ArrayIndex); }

#endif __cplusplus

/* D3DTexture */

D3DINLINE ULONG             WINAPI D3DTexture_AddRef(D3DTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DTexture_Release(D3DTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DTexture_GetDevice(D3DTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DTexture_GetType(D3DTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DTexture_IsBusy(D3DTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DTexture_BlockUntilNotBusy(D3DTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI D3DTexture_GetLevelCount(D3DTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE void              WINAPI D3DTexture_GetTailDesc(D3DTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc((D3DBaseTexture *)pThis, pDesc); }
D3DINLINE void              WINAPI D3DTexture_LockTail(D3DTexture *pThis, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail((D3DBaseTexture *)pThis, D3DCUBEMAP_FACE_POSITIVE_X, pLockedTail, Flags); }
D3DINLINE void              WINAPI D3DTexture_UnlockTail(D3DTexture *pThis) { D3DBaseTexture_UnlockTail((D3DBaseTexture *)pThis, D3DCUBEMAP_FACE_POSITIVE_X); }

void                        WINAPI D3DTexture_GetLevelDesc(D3DTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc);
D3DSurface*                 WINAPI D3DTexture_GetSurfaceLevel(D3DTexture *pThis, UINT Level);
void                        WINAPI D3DTexture_LockRect(D3DTexture *pThis, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
void                        WINAPI D3DTexture_UnlockRect(D3DTexture *pThis, UINT Level);

// Compatibilty wrappers.

D3DINLINE ULONG             WINAPI IDirect3DTexture9_AddRef(D3DTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DTexture9_Release(D3DTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_GetDevice(D3DTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DTexture9_GetType(D3DTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DTexture9_IsBusy(D3DTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DTexture9_BlockUntilNotBusy(D3DTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DTexture9_GetLevelCount(D3DTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_GetLevelDesc(D3DTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc) { D3DTexture_GetLevelDesc(pThis, Level, pDesc); D3DVOIDRETURN; }
D3DINLINE HRESULT           WINAPI IDirect3DTexture9_GetSurfaceLevel(D3DTexture *pThis, UINT Level, D3DSurface **ppSurfaceLevel) { return (*ppSurfaceLevel = D3DTexture_GetSurfaceLevel(pThis, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_LockRect(D3DTexture *pThis, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DTexture_LockRect(pThis, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_UnlockRect(D3DTexture *pThis, UINT Level) { D3DTexture_UnlockRect(pThis, Level); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_GetTailDesc(D3DTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DTexture_GetTailDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_LockTail(D3DTexture *pThis, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DTexture_LockTail(pThis, pLockedTail, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DTexture9_UnlockTail(D3DTexture *pThis) { D3DTexture_UnlockTail(pThis); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE D3DVOID          WINAPI D3DTexture::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) { D3DTexture_GetLevelDesc(this, Level, pDesc); D3DVOIDRETURN; }
D3DMINLINE HRESULT          WINAPI D3DTexture::GetSurfaceLevel(UINT Level, D3DSurface **ppSurfaceLevel) { return (*ppSurfaceLevel = D3DTexture_GetSurfaceLevel(this, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID          WINAPI D3DTexture::LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DTexture_LockRect(this, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DTexture::UnlockRect(UINT Level) { D3DTexture_UnlockRect(this, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DTexture::LockTail(D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DTexture_LockTail(this, pLockedTail, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DTexture::UnlockTail() { D3DTexture_UnlockTail(this); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DVolumeTexture */

D3DINLINE ULONG             WINAPI D3DVolumeTexture_AddRef(D3DVolumeTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DVolumeTexture_Release(D3DVolumeTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVolumeTexture_GetDevice(D3DVolumeTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DVolumeTexture_GetType(D3DVolumeTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DVolumeTexture_IsBusy(D3DVolumeTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVolumeTexture_BlockUntilNotBusy(D3DVolumeTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI D3DVolumeTexture_GetLevelCount(D3DVolumeTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE void              WINAPI D3DVolumeTexture_GetTailDesc(D3DVolumeTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc((D3DBaseTexture *)pThis, pDesc); }
D3DINLINE void              WINAPI D3DVolumeTexture_LockTail(D3DVolumeTexture *pThis, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail((D3DBaseTexture *)pThis, D3DCUBEMAP_FACE_POSITIVE_X, pLockedTail, Flags); }
D3DINLINE void              WINAPI D3DVolumeTexture_UnlockTail(D3DVolumeTexture *pThis) { D3DBaseTexture_UnlockTail((D3DBaseTexture *)pThis, D3DCUBEMAP_FACE_POSITIVE_X); }

void                        WINAPI D3DVolumeTexture_GetLevelDesc(D3DVolumeTexture *pThis, UINT Level, D3DVOLUME_DESC *pDesc);
D3DVolume*                  WINAPI D3DVolumeTexture_GetVolumeLevel(D3DVolumeTexture *pThis, UINT Level);
void                        WINAPI D3DVolumeTexture_LockBox(D3DVolumeTexture *pThis, UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags);
void                        WINAPI D3DVolumeTexture_UnlockBox(D3DVolumeTexture *pThis, UINT Level);

// Compatibilty wrappers.

D3DINLINE ULONG             WINAPI IDirect3DVolumeTexture9_AddRef(D3DVolumeTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DVolumeTexture9_Release(D3DVolumeTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_GetDevice(D3DVolumeTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DVolumeTexture9_GetType(D3DVolumeTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DVolumeTexture9_IsBusy(D3DVolumeTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DVolumeTexture9_BlockUntilNotBusy(D3DVolumeTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DVolumeTexture9_GetLevelCount(D3DVolumeTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_GetLevelDesc(D3DVolumeTexture *pThis, UINT Level, D3DVOLUME_DESC *pDesc) { D3DVolumeTexture_GetLevelDesc(pThis, Level, pDesc); D3DVOIDRETURN; }
D3DINLINE HRESULT           WINAPI IDirect3DVolumeTexture9_GetVolumeLevel(D3DVolumeTexture *pThis, UINT Level, D3DVolume **ppVolumeLevel) { return (*ppVolumeLevel = D3DVolumeTexture_GetVolumeLevel(pThis, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_LockBox(D3DVolumeTexture *pThis, UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags) { D3DVolumeTexture_LockBox(pThis, Level, pLockedVolume, pBox, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_UnlockBox(D3DVolumeTexture *pThis, UINT Level) { D3DVolumeTexture_UnlockBox(pThis, Level); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_GetTailDesc(D3DVolumeTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DVolumeTexture_GetTailDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_LockTail(D3DVolumeTexture *pThis, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DVolumeTexture_LockTail(pThis, pLockedTail, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolumeTexture9_UnlockTail(D3DVolumeTexture *pThis) { D3DVolumeTexture_UnlockTail(pThis); D3DVOIDRETURN; }


#ifdef __cplusplus

D3DMINLINE D3DVOID          WINAPI D3DVolumeTexture::GetLevelDesc(UINT Level, D3DVOLUME_DESC *pDesc) { D3DVolumeTexture_GetLevelDesc(this, Level, pDesc); D3DVOIDRETURN; }
D3DMINLINE HRESULT          WINAPI D3DVolumeTexture::GetVolumeLevel(UINT Level, D3DVolume **ppVolumeLevel) { return (*ppVolumeLevel = D3DVolumeTexture_GetVolumeLevel(this, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID          WINAPI D3DVolumeTexture::LockBox(UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags) { D3DVolumeTexture_LockBox(this, Level, pLockedVolume, pBox, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVolumeTexture::UnlockBox(UINT Level) { D3DVolumeTexture_UnlockBox(this, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVolumeTexture::LockTail(D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DVolumeTexture_LockTail(this, pLockedTail, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVolumeTexture::UnlockTail() { D3DVolumeTexture_UnlockTail(this); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DCubeTexture */

D3DINLINE ULONG             WINAPI D3DCubeTexture_AddRef(D3DCubeTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DCubeTexture_Release(D3DCubeTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DCubeTexture_GetDevice(D3DCubeTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DCubeTexture_GetType(D3DCubeTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DCubeTexture_IsBusy(D3DCubeTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DCubeTexture_BlockUntilNotBusy(D3DCubeTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI D3DCubeTexture_GetLevelCount(D3DCubeTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE void              WINAPI D3DCubeTexture_GetTailDesc(D3DCubeTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc((D3DBaseTexture *)pThis, pDesc); }
D3DINLINE void              WINAPI D3DCubeTexture_LockTail(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail((D3DBaseTexture *)pThis, FaceType, pLockedTail, Flags); }
D3DINLINE void              WINAPI D3DCubeTexture_UnlockTail(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType) { D3DBaseTexture_UnlockTail((D3DBaseTexture *)pThis, FaceType); }

void                        WINAPI D3DCubeTexture_GetLevelDesc(D3DCubeTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc);
D3DSurface*                 WINAPI D3DCubeTexture_GetCubeMapSurface(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level);
void                        WINAPI D3DCubeTexture_LockRect(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
void                        WINAPI D3DCubeTexture_UnlockRect(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level);

D3DINLINE ULONG             WINAPI IDirect3DCubeTexture9_AddRef(D3DCubeTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DCubeTexture9_Release(D3DCubeTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_GetDevice(D3DCubeTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DCubeTexture9_GetType(D3DCubeTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DCubeTexture9_IsBusy(D3DCubeTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DCubeTexture9_BlockUntilNotBusy(D3DCubeTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DCubeTexture9_GetLevelCount(D3DCubeTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_GetLevelDesc(D3DCubeTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc) { D3DCubeTexture_GetLevelDesc(pThis, Level, pDesc); D3DVOIDRETURN; }
D3DINLINE HRESULT           WINAPI IDirect3DCubeTexture9_GetCubeMapSurface(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level, D3DSurface **ppCubeMapSurface) { return (*ppCubeMapSurface = D3DCubeTexture_GetCubeMapSurface(pThis, FaceType, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_LockRect(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DCubeTexture_LockRect(pThis, FaceType, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_UnlockRect(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level) { D3DCubeTexture_UnlockRect(pThis, FaceType, Level); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_GetTailDesc(D3DCubeTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DCubeTexture_GetTailDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_LockTail(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DCubeTexture_LockTail(pThis, FaceType, pLockedTail, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DCubeTexture9_UnlockTail(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType) { D3DCubeTexture_UnlockTail(pThis, FaceType); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE D3DVOID          WINAPI D3DCubeTexture::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) { D3DCubeTexture_GetLevelDesc(this, Level, pDesc); D3DVOIDRETURN; }
D3DMINLINE HRESULT          WINAPI D3DCubeTexture::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, D3DSurface **ppCubeMapSurface) { return (*ppCubeMapSurface = D3DCubeTexture_GetCubeMapSurface(this, FaceType, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID          WINAPI D3DCubeTexture::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DCubeTexture_LockRect(this, FaceType, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DCubeTexture::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level) { D3DCubeTexture_UnlockRect(this, FaceType, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DCubeTexture::LockTail(D3DCUBEMAP_FACES FaceType, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DCubeTexture_LockTail(this, FaceType, pLockedTail, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DCubeTexture::UnlockTail(D3DCUBEMAP_FACES FaceType) { D3DCubeTexture_UnlockTail(this, FaceType); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DArrayTexture */

D3DINLINE ULONG             WINAPI D3DArrayTexture_AddRef(D3DArrayTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DArrayTexture_Release(D3DArrayTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DArrayTexture_GetDevice(D3DArrayTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DArrayTexture_GetType(D3DArrayTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DArrayTexture_IsBusy(D3DArrayTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DArrayTexture_BlockUntilNotBusy(D3DArrayTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI D3DArrayTexture_GetLevelCount(D3DArrayTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE void              WINAPI D3DArrayTexture_GetTailDesc(D3DArrayTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DBaseTexture_GetTailDesc((D3DBaseTexture *)pThis, pDesc); }
D3DINLINE void              WINAPI D3DArrayTexture_LockTail(D3DArrayTexture *pThis, UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DBaseTexture_LockTail((D3DBaseTexture *)pThis, ArrayIndex, pLockedTail, Flags); }
D3DINLINE void              WINAPI D3DArrayTexture_UnlockTail(D3DArrayTexture *pThis, UINT ArrayIndex) { D3DBaseTexture_UnlockTail((D3DBaseTexture *)pThis, ArrayIndex); }

DWORD                       WINAPI D3DArrayTexture_GetArraySize(D3DArrayTexture *pThis);
void                        WINAPI D3DArrayTexture_GetLevelDesc(D3DArrayTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc);
D3DSurface*                 WINAPI D3DArrayTexture_GetArraySurface(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level);
void                        WINAPI D3DArrayTexture_LockRect(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
void                        WINAPI D3DArrayTexture_UnlockRect(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level);

D3DINLINE ULONG             WINAPI IDirect3DArrayTexture9_AddRef(D3DArrayTexture *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DArrayTexture9_Release(D3DArrayTexture *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_GetDevice(D3DArrayTexture *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DArrayTexture9_GetType(D3DArrayTexture *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DArrayTexture9_IsBusy(D3DArrayTexture *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DArrayTexture9_BlockUntilNotBusy(D3DArrayTexture *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DArrayTexture9_GetLevelCount(D3DArrayTexture *pThis) { return D3DBaseTexture_GetLevelCount((D3DBaseTexture *)pThis); }
D3DINLINE DWORD             WINAPI IDirect3DArrayTexture9_GetArraySize(D3DArrayTexture *pThis) { return D3DArrayTexture_GetArraySize(pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_GetLevelDesc(D3DArrayTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc) { D3DArrayTexture_GetLevelDesc(pThis, Level, pDesc); D3DVOIDRETURN; }
D3DINLINE HRESULT           WINAPI IDirect3DArrayTexture9_GetArraySurface(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level, D3DSurface **ppArraySurface) { return (*ppArraySurface = D3DArrayTexture_GetArraySurface(pThis, ArrayIndex, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_LockRect(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DArrayTexture_LockRect(pThis, ArrayIndex, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_UnlockRect(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level) { D3DArrayTexture_UnlockRect(pThis, ArrayIndex, Level); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_GetTailDesc(D3DArrayTexture *pThis, D3DMIPTAIL_DESC *pDesc) { D3DArrayTexture_GetTailDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_LockTail(D3DArrayTexture *pThis, UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DArrayTexture_LockTail(pThis, ArrayIndex, pLockedTail, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DArrayTexture9_UnlockTail(D3DArrayTexture *pThis, UINT ArrayIndex) { D3DArrayTexture_UnlockTail(pThis, ArrayIndex); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE DWORD            WINAPI D3DArrayTexture::GetArraySize() { return D3DArrayTexture_GetArraySize(this); }
D3DMINLINE D3DVOID          WINAPI D3DArrayTexture::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) { D3DArrayTexture_GetLevelDesc(this, Level, pDesc); D3DVOIDRETURN; }
D3DMINLINE HRESULT          WINAPI D3DArrayTexture::GetArraySurface(UINT ArrayIndex, UINT Level, D3DSurface **ppArraySurface) { return (*ppArraySurface = D3DArrayTexture_GetArraySurface(this, ArrayIndex, Level)) != NULL ? S_OK : E_OUTOFMEMORY; }
D3DMINLINE D3DVOID          WINAPI D3DArrayTexture::LockRect(UINT ArrayIndex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DArrayTexture_LockRect(this, ArrayIndex, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DArrayTexture::UnlockRect(UINT ArrayIndex, UINT Level) { D3DArrayTexture_UnlockRect(this, ArrayIndex, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DArrayTexture::LockTail(UINT ArrayIndex, D3DLOCKED_TAIL *pLockedTail, DWORD Flags) { D3DArrayTexture_LockTail(this, ArrayIndex, pLockedTail, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DArrayTexture::UnlockTail(UINT ArrayIndex) { D3DArrayTexture_UnlockTail(this, ArrayIndex); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DVertexBuffer */

D3DINLINE ULONG             WINAPI D3DVertexBuffer_AddRef(D3DVertexBuffer *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DVertexBuffer_Release(D3DVertexBuffer *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVertexBuffer_GetDevice(D3DVertexBuffer *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DVertexBuffer_GetType(D3DVertexBuffer *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DVertexBuffer_IsBusy(D3DVertexBuffer *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVertexBuffer_BlockUntilNotBusy(D3DVertexBuffer *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }

void*                       WINAPI D3DVertexBuffer_Lock(D3DVertexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
void                        WINAPI D3DVertexBuffer_GetDesc(D3DVertexBuffer *pThis, D3DVERTEXBUFFER_DESC *pDesc);
void                        WINAPI D3DVertexBuffer_Unlock(D3DVertexBuffer *pThis);

D3DINLINE ULONG             WINAPI IDirect3DVertexBuffer9_AddRef(D3DVertexBuffer *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DVertexBuffer9_Release(D3DVertexBuffer *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexBuffer9_GetDevice(D3DVertexBuffer *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DVertexBuffer9_GetType(D3DVertexBuffer *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DVertexBuffer9_IsBusy(D3DVertexBuffer *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DVertexBuffer9_BlockUntilNotBusy(D3DVertexBuffer *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexBuffer9_Lock(D3DVertexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags) { *ppbData = (BYTE*)D3DVertexBuffer_Lock(pThis, OffsetToLock, SizeToLock, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexBuffer9_Unlock(D3DVertexBuffer *pThis) { D3DVertexBuffer_Unlock(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVertexBuffer9_GetDesc(D3DVertexBuffer *pThis, D3DVERTEXBUFFER_DESC *pDesc) { D3DVertexBuffer_GetDesc(pThis, pDesc); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE D3DVOID          WINAPI D3DVertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags) { *ppbData = (BYTE*)D3DVertexBuffer_Lock(this, OffsetToLock, SizeToLock, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVertexBuffer::Unlock() { D3DVertexBuffer_Unlock(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVertexBuffer::GetDesc(D3DVERTEXBUFFER_DESC *pDesc) { D3DVertexBuffer_GetDesc(this, pDesc); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DIndexBuffer */

D3DINLINE ULONG             WINAPI D3DIndexBuffer_AddRef(D3DIndexBuffer *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DIndexBuffer_Release(D3DIndexBuffer *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DIndexBuffer_GetDevice(D3DIndexBuffer *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DIndexBuffer_GetType(D3DIndexBuffer *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DIndexBuffer_IsBusy(D3DIndexBuffer *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DIndexBuffer_BlockUntilNotBusy(D3DIndexBuffer *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }

void*                       WINAPI D3DIndexBuffer_Lock(D3DIndexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
void                        WINAPI D3DIndexBuffer_Unlock(D3DIndexBuffer *pThis);
void                        WINAPI D3DIndexBuffer_GetDesc(D3DIndexBuffer *pThis, D3DINDEXBUFFER_DESC *pDesc);

// Compatibilty wrappers.

D3DINLINE ULONG             WINAPI IDirect3DIndexBuffer9_AddRef(D3DIndexBuffer *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DIndexBuffer9_Release(D3DIndexBuffer *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DIndexBuffer9_GetDevice(D3DIndexBuffer *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DIndexBuffer9_GetType(D3DIndexBuffer *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DIndexBuffer9_IsBusy(D3DIndexBuffer *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DIndexBuffer9_BlockUntilNotBusy(D3DIndexBuffer *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DIndexBuffer9_Lock(D3DIndexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags) { *ppbData = D3DIndexBuffer_Lock(pThis, OffsetToLock, SizeToLock, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DIndexBuffer9_Unlock(D3DIndexBuffer *pThis) { D3DIndexBuffer_Unlock(pThis); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DIndexBuffer9_GetDesc(D3DIndexBuffer *pThis, D3DINDEXBUFFER_DESC *pDesc) { D3DIndexBuffer_GetDesc(pThis, pDesc); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE D3DVOID          WINAPI D3DIndexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void **ppbData, DWORD Flags) { *ppbData = D3DIndexBuffer_Lock(this, OffsetToLock, SizeToLock, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DIndexBuffer::Unlock() { D3DIndexBuffer_Unlock(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DIndexBuffer::GetDesc(D3DINDEXBUFFER_DESC *pDesc) { D3DIndexBuffer_GetDesc(this, pDesc); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DSurface */

D3DINLINE ULONG             WINAPI D3DSurface_AddRef(D3DSurface *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DSurface_Release(D3DSurface *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DSurface_GetDevice(D3DSurface *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DSurface_GetType(D3DSurface *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DSurface_IsBusy(D3DSurface *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DSurface_BlockUntilNotBusy(D3DSurface *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }

void*                       WINAPI D3DSurface_GetContainer(D3DSurface *pThis, REFIID UnusedRiid);
void                        WINAPI D3DSurface_GetDesc(D3DSurface *pThis, D3DSURFACE_DESC *pDesc);
void                        WINAPI D3DSurface_LockRect(D3DSurface *pThis, D3DLOCKED_RECT *pLockedRect,CONST RECT *pRect, DWORD Flags);
void                        WINAPI D3DSurface_UnlockRect(D3DSurface *pThis);

D3DINLINE ULONG             WINAPI IDirect3DSurface9_AddRef(D3DSurface *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DSurface9_Release(D3DSurface *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DSurface9_GetDevice(D3DSurface *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DSurface9_GetType(D3DSurface *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DSurface9_IsBusy(D3DSurface *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DSurface9_BlockUntilNotBusy(D3DSurface *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE HRESULT           WINAPI IDirect3DSurface9_GetContainer(D3DSurface *pThis, REFIID UnusedRiid, void **ppContainer) { return (*ppContainer = D3DSurface_GetContainer(pThis, UnusedRiid)) != NULL ? S_OK : E_FAIL; }
D3DINLINE D3DVOID           WINAPI IDirect3DSurface9_GetDesc(D3DSurface *pThis, D3DSURFACE_DESC *pDesc) { D3DSurface_GetDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DSurface9_LockRect(D3DSurface *pThis, D3DLOCKED_RECT *pLockedRect,CONST RECT *pRect, DWORD Flags) { D3DSurface_LockRect(pThis, pLockedRect,pRect, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DSurface9_UnlockRect(D3DSurface *pThis) { D3DSurface_UnlockRect(pThis); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE HRESULT          WINAPI D3DSurface::GetContainer(REFIID UnusedRiid, void **ppContainer) { return (*ppContainer = D3DSurface_GetContainer(this, UnusedRiid)) != NULL ? S_OK : E_FAIL; }
D3DMINLINE D3DVOID          WINAPI D3DSurface::GetDesc(D3DSURFACE_DESC *pDesc) { D3DSurface_GetDesc(this, pDesc); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DSurface::LockRect(D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DSurface_LockRect(this, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DSurface::UnlockRect() { D3DSurface_UnlockRect(this); D3DVOIDRETURN; }

#endif __cplusplus

/* D3DVolume */

D3DINLINE ULONG             WINAPI D3DVolume_AddRef(D3DVolume *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI D3DVolume_Release(D3DVolume *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVolume_GetDevice(D3DVolume *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
D3DINLINE D3DRESOURCETYPE   WINAPI D3DVolume_GetType(D3DVolume *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI D3DVolume_IsBusy(D3DVolume *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI D3DVolume_BlockUntilNotBusy(D3DVolume *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }

void*                       WINAPI D3DVolume_GetContainer(D3DVolume *pThis, REFIID UnusedRiid);
void                        WINAPI D3DVolume_GetDesc(D3DVolume *pThis, D3DVOLUME_DESC *pDesc);
void                        WINAPI D3DVolume_LockBox(D3DVolume *pThis, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags);
void                        WINAPI D3DVolume_UnlockBox(D3DVolume *pThis);

D3DINLINE ULONG             WINAPI IDirect3DVolume9_AddRef(D3DVolume *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DINLINE ULONG             WINAPI IDirect3DVolume9_Release(D3DVolume *pThis) { return D3DResource_Release((D3DResource *)pThis); }
D3DINLINE D3DVOID           WINAPI IDirect3DVolume9_GetDevice(D3DVolume *pThis, D3DDevice **ppDevice) { D3DResource_GetDevice((D3DResource *)pThis, ppDevice); D3DVOIDRETURN; }
D3DINLINE D3DRESOURCETYPE   WINAPI IDirect3DVolume9_GetType(D3DVolume *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DINLINE BOOL              WINAPI IDirect3DVolume9_IsBusy(D3DVolume *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DINLINE void              WINAPI IDirect3DVolume9_BlockUntilNotBusy(D3DVolume *pThis) { D3DResource_BlockUntilNotBusy((D3DResource *)pThis); }
D3DINLINE HRESULT           WINAPI IDirect3DVolume9_GetContainer(D3DVolume *pThis, REFIID UnusedRiid, void **ppContainer) { return (*ppContainer = D3DVolume_GetContainer(pThis, UnusedRiid)) != NULL ? S_OK : E_FAIL; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolume9_GetDesc(D3DVolume *pThis, D3DVOLUME_DESC *pDesc) { D3DVolume_GetDesc(pThis, pDesc); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolume9_LockBox(D3DVolume *pThis, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags) { D3DVolume_LockBox(pThis, pLockedVolume, pBox, Flags); D3DVOIDRETURN; }
D3DINLINE D3DVOID           WINAPI IDirect3DVolume9_UnlockBox(D3DVolume *pThis) { D3DVolume_UnlockBox(pThis); D3DVOIDRETURN; }

#ifdef __cplusplus

D3DMINLINE HRESULT          WINAPI D3DVolume::GetContainer(REFIID UnusedRiid, void **ppContainer) { return (*ppContainer = D3DVolume_GetContainer(this, UnusedRiid)) != NULL ? S_OK : E_FAIL; }
D3DMINLINE D3DVOID          WINAPI D3DVolume::GetDesc(D3DVOLUME_DESC *pDesc) { D3DVolume_GetDesc(this, pDesc); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVolume::LockBox(D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags) { D3DVolume_LockBox(this, pLockedVolume, pBox, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID          WINAPI D3DVolume::UnlockBox() { D3DVolume_UnlockBox(this); D3DVOIDRETURN; }

#endif __cplusplus

#ifdef __cplusplus
};
#endif

/* PIX Utility Functions */

#ifdef __cplusplus
extern "C" {
#endif

HRESULT WINAPI PIXCaptureAlphaTimingFrame(char *pFilename);
HRESULT WINAPI PIXCaptureGpuFrame(char *pFilename);

#ifdef __cplusplus
};
#endif


#pragma warning( pop )

#endif /* _D3D9_H_ */
