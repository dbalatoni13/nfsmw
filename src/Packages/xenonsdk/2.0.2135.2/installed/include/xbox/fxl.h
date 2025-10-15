/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fxl.h
 *  Content:    Xenon Effects include file
 *
 ****************************************************************************/

#ifndef _FXL_H_
#define _FXL_H_

#include <xboxmath.h>

#pragma warning(push)

// Disable nameless struct/union, zero-sized array, constant conditional
// expression, and padded structures from FXLALIGN16 warnings for this header.

#pragma warning(disable:4201 4200 4127 4324)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _X86_
#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
#endif // _X86_

#define FXLRESTRICT //__restrict
#define FXLINLINE  __inline
#define FXLFINLINE __forceinline
#define FXLALIGN16 __declspec(align(16))
#define FXLALIGN128 __declspec(align(128))
#ifdef __cplusplus
#define FXLEXTERN extern "C"
#else
#define FXLEXTERN extern
#endif

/*
 * Begin technique flags
 */

#define FXL_RESTORE_DEFAULT_RENDER_STATE            0x1
#define FXL_RESTORE_DEFAULT_SAMPLER_STATE           0x2
#define FXL_RESTORE_DEFAULT_STATE                   (FXL_RESTORE_DEFAULT_RENDER_STATE | FXL_RESTORE_DEFAULT_SAMPLER_STATE)

/*
 * Parameter desc flags
 */

#define FXLPFLAG_SHARED                             0x1

/*
 * Compile effect flags
 */

#define D3DXSHADER_FXLANNOTATE_VARIABLE_NAMES       (1 << 26)
#define D3DXSHADER_FXLANNOTATE_SEMANTIC_NAMES       (1 << 27)
#define D3DXSHADER_FXLPARAMETERS_AS_VARIABLE_NAMES  (1 << 28)
#define D3DXSHADER_FXLEFFECT_NATIVE_ENDIAN          (1 << 29)

/*
 * Binary effect data version
 */

#define FXLDATA_IDENTIFIER                          0xA3D7
#define FXLDATA_VERSION_MAJOR                       1
#define FXLDATA_VERSION_MINOR                       4

/*
 * Publicly accessible globals - Feel free to modify the values.
 */

// Set FXL__EnforceSharedCorrelation to TRUE to enforce a correlation of
// shared parameter register assignments across all effects in a single
// global pool.  When this global is set to TRUE, FXL will RIP under any
// of the following conditions:
//
//   - More than one effect pool is created
//
//   - The shader register assignment(s) for any shared parameter added 
//     to the global pool differ in base indices from the register 
//     assignment(s) of the matching parameter already present in the pool
//     (but the register counts for a matching parameter are allowed to 
//     fluctuate)
//
//   - The shader register assignment(s) for any shared parameter added
//     to the global pool interfere with the assignment(s) of any non-
//     matching parameter already present in the pool
//
//   - The register assignment(s) for a local parameter of any effect (or 
//     shared parameter of any effect not placed in the global pool) 
//     conflict with the assignments of any shared parameters in the 
//     global pool
//
//   - The register assignments for literal constants used by any shader
//     in any effect interfere with the assignments of any shared
//     parameters in the global pool
//
//   - The integer literal constants used in conjuction with a shared loop 
//     parameter added to the global pool are not identical in value to the 
//     integer literal constants associated with a matching parameter 
//     already present in the pool
//
//
// When the correlation of shared parameters is preserved, Commit will only 
// send shared parameter data to the hardware when explicitly modified in value 
// through a parameter 'Set' call.  Without correlation, all shared parameters
// must be updated in the hardware at the first Commit of every pass.

FXLEXTERN BOOL FXL__EnforceSharedCorrelation;

/*
 * Data types
 */

typedef DWORD                                       FXLHANDLE;

typedef FXLALIGN16 FLOAT                            FXLFLOATA;
typedef FXLALIGN16 INT                              FXLINTA;
typedef FXLALIGN16 BOOL                             FXLBOOLA;

typedef struct FXLEffect                            FXLEffect;
typedef struct FXLEffect*                           PFXLEFFECT, *LPFXLEFFECT;

typedef struct FXLEffectPool                        FXLEffectPool;
typedef struct FXLEffectPool*                       PFXLEFFECTPOOL, *LPFXLEFFECTPOOL;

/*
 * FXLPA_CONTENT
 */

typedef enum _FXLPA_CONTENT
{
    FXLPACONTENT_DATA                               = 0,
    FXLPACONTENT_STRUCT                             = 1,
    FXLPACONTENT_ARRAY                              = 2,
} FXLPA_CONTENT;

/*
 * FXLDATA_TYPE
 */

typedef enum _FXLDATA_TYPE
{
    FXLDTYPE_FLOAT                                  = 0,
    FXLDTYPE_INT                                    = 1,
    FXLDTYPE_BOOL                                   = 2,
    FXLDTYPE_SAMPLER                                = 3,
    FXLDTYPE_STRING                                 = 4,
    FXLDTYPE_CONTAINER                              = 5,
} FXLDATA_TYPE;

/*
 * FXLDATA_CLASS
 */

typedef enum _FXLDATA_CLASS
{
    FXLDCLASS_SCALAR                                = 0,
    FXLDCLASS_VECTOR                                = 1,
    FXLDCLASS_RMATRIX                               = 2,
    FXLDCLASS_CMATRIX                               = 3,
    FXLDCLASS_SAMPLER                               = 4,
    FXLDCLASS_SAMPLER1D                             = 5,
    FXLDCLASS_SAMPLER2D                             = 6,
    FXLDCLASS_SAMPLER3D                             = 7,
    FXLDCLASS_SAMPLERCUBE                           = 8,
    FXLDCLASS_CONTAINER                             = 9,
} FXLDATA_CLASS;

/*
 * FXLPARAMETER_CONTEXT
 */

#define FXLPCONTEXT_VERTEXSHADERCONSTANTF           0x0001
#define FXLPCONTEXT_PIXELSHADERCONSTANTF            0x0002
#define FXLPCONTEXT_VERTEXSHADERCONSTANTI           0x0004
#define FXLPCONTEXT_PIXELSHADERCONSTANTI            0x0008
#define FXLPCONTEXT_VERTEXSHADERCONSTANTB           0x0010
#define FXLPCONTEXT_PIXELSHADERCONSTANTB            0x0020
#define FXLPCONTEXT_VERTEXSHADERSAMPLER             0x0040
#define FXLPCONTEXT_PIXELSHADERSAMPLER              0x0080

typedef DWORD FXLPARAMETER_CONTEXT;

/*
 * FXLREGISTER_UPDATE
 */

typedef enum _FXLREGISTER_UPDATE
{
    FXLREGUPDATE_MANUAL                             = 0,
    FXLREGUPDATE_AUTOMATIC                          = 1,
} FXLREGISTER_UPDATE;

/*
 * FXLEFFECT_DESC
 */

typedef struct _FXLEFFECT_DESC
{
    CONST CHAR*             pCreator;
    UINT                    Parameters;
    UINT                    Techniques;
    UINT                    Functions;
    UINT                    Annotations;
} FXLEFFECT_DESC;

/*
 * FXLTECHNIQUE_DESC
 */

typedef struct _FXLTECHNIQUE_DESC
{
    CONST CHAR*             pName;
    UINT                    Passes;
    UINT                    Annotations;
} FXLTECHNIQUE_DESC;

/*
 * FXLPASS_DESC
 */

typedef struct _FXLPASS_DESC
{
    CONST CHAR*             pName;
    UINT                    Annotations;
    UINT                    RenderStates;
    UINT                    SamplerStates;
    DWORD*                  pVertexShaderFunction;
    UINT                    VertexShaderFunctionSize;
    DWORD*                  pPixelShaderFunction;
    UINT                    PixelShaderFunctionSize;
} FXLPASS_DESC;

/*
 * FXLPARAMETER_DESC
 */

typedef struct _FXLPARAMETER_DESC
{
    CONST CHAR*             pName;
    FXLPA_CONTENT           Content;
    FXLDATA_TYPE            Type;
    FXLDATA_CLASS           Class;
    UINT                    Rows;
    UINT                    Columns;
    UINT                    Elements;
    UINT                    Annotations;
    DWORD                   Flags;
    UINT                    Size;
} FXLPARAMETER_DESC;

/*
 * FXLANNOTATION_DESC
 */

typedef struct _FXLANNOTATION_DESC
{
    CONST CHAR*             pName;
    FXLPA_CONTENT           Content;
    FXLDATA_TYPE            Type;
    FXLDATA_CLASS           Class;
    UINT                    Rows;
    UINT                    Columns;
    UINT                    Elements;
    UINT                    Size;
} FXLANNOTATION_DESC;

/*
 * Render state type and value pair
 */

typedef struct _FXLRENDERSTATE
{
    D3DRENDERSTATETYPE Type;
    DWORD Value;
} FXLRENDERSTATE;

/*
 * Sampler state type and value pair
 */

typedef struct _FXLSAMPLERSTATE
{
    D3DSAMPLERSTATETYPE Type;
    DWORD Value;
} FXLSAMPLERSTATE;

/****************************************************************************
 *
 * Begin: Private internal data - Please don't access these directly, as 
 *                                they're subject to change.
 *
 ****************************************************************************/

#define FXL_MAX_PARAMETER_FLAG_GROUPS               16

/*
 * Parameter content
 */

#define FXLPACONTENTI_DATA                          0
#define FXLPACONTENTI_STRUCT                        1
#define FXLPACONTENTI_ARRAY                         2

/*
 * Parameter type
 */

#define FXLDTYPEI_FLOAT                             0
#define FXLDTYPEI_INT                               1
#define FXLDTYPEI_BOOL                              2
#define FXLDTYPEI_SAMPLER                           3   // Samplers are supported as parameters, but not as annotations
#define FXLDTYPEI_STRING                            3   // Strings are supported as annotations, but not as parameters

/*
 * Parameter class
 */

#define FXLDCLASSI_SCALAR                           0
#define FXLDCLASSI_VECTOR                           1
#define FXLDCLASSI_RMATRIX                          2
#define FXLDCLASSI_CMATRIX                          3
#define FXLDCLASSI_SAMPLER                          0
#define FXLDCLASSI_SAMPLER1D                        1
#define FXLDCLASSI_SAMPLER2D                        2
#define FXLDCLASSI_SAMPLER3D                        3
#define FXLDCLASSI_SAMPLERCUBE                      4

/*
 * FXLDIRTYFLAGS
 */

typedef FXLALIGN128 UINT64 FXLDIRTYFLAGS[FXL_MAX_PARAMETER_FLAG_GROUPS];

/*
 * Data parameter
 */

typedef union _FXLPARAMETERDATA
{
    DWORD DWord[2];

    struct
    {
        // DWord[0]

        DWORD       Content         : 2;
        DWORD       Type            : 2;
        DWORD       Class           : 3;
        DWORD       Rows            : 2;
        DWORD       Columns         : 2;
        DWORD                       : 5;
        DWORD       Flags           : 5;
        DWORD       Annotations     : 11;

        // DWord[1]

        DWORD       DataOffset      : 16;
        DWORD       Size            : 16;
    };
} FXLPARAMETERDATA;

/*
 * Struct parameter
 */

typedef union _FXLPARAMETERSTRUCT
{
    DWORD DWord[2];

    struct
    {
        // DWord[0]

        DWORD       Content         : 2;
        DWORD       Members         : 14;
        DWORD       Flags           : 5;
        DWORD       Annotations     : 11;

        // DWord[1]

        DWORD       NextOffset      : 16;
        DWORD       Size            : 16;
    };
} FXLPARAMETERSTRUCT;

/*
 * Array parameter
 */

typedef union _FXLPARAMETERARRAY
{
    DWORD DWord[2];

    struct
    {
        // DWord[0]

        DWORD       Content         : 2;
        DWORD       Elements        : 14;
        DWORD       Flags           : 5;
        DWORD       Annotations     : 11;

        // DWord[1]

        DWORD       NextOffset      : 16;
        DWORD       Size            : 16;
    };
} FXLPARAMETERARRAY;

/*
 * Generic parameter entry.  The specific contents of the parameter depend
 * on the 'Content' field.
 */

typedef union _FXLPARAMETERENTRY
{
    FXLPARAMETERDATA   Data;
    FXLPARAMETERSTRUCT Struct;
    FXLPARAMETERARRAY  Array;

    DWORD DWord[2];

    struct
    {
        // DWord[0]

        DWORD       Content         : 2;
        DWORD                       : 14;
        DWORD       Flags           : 5;
        DWORD       Annotations     : 11;

        // DWord[1]

        DWORD       Offset          : 16;
        DWORD       Size            : 16;
    };
} FXLPARAMETERENTRY;

/*
 * Parameter reference in a pass.  The reference is used to map
 * parameter data to shader registers and any literal constant
 * data packed into the registers with the parameter data.
 */

typedef FXLALIGN16 union _FXLPASSPARAMETERREF
{
    DWORD DWord[4];

    struct
    {
        // DWord[0]

        FXLHANDLE ParameterHandle;

        // DWord[1]

        DWORD VSFRegisterIndex      : 10;
        DWORD VSFRegisterCount      : 2;
        DWORD VSIRegisterIndex      : 8;
        DWORD VSIRegisterCount      : 2;
        DWORD VSSRegisterIndex      : 8;
        DWORD VSBRegisterCount      : 2;

        // DWord[2]

        DWORD PSFRegisterIndex      : 10;
        DWORD PSFRegisterCount      : 2;
        DWORD PSIRegisterIndex      : 8;
        DWORD PSIRegisterCount      : 2;
        DWORD PSSRegisterIndex      : 8;
        DWORD PSBRegisterCount      : 2;

        // DWord[3]

        DWORD VSBRegisterIndex      : 8;
        DWORD PSBRegisterIndex      : 8;
        DWORD VSILiteralOffset      : 8;    // 16 byte multiple
        DWORD PSILiteralOffset      : 8;    // 16 byte multiple
    };
} FXLPASSPARAMETERREF;

/*
 * Vertex shader
 */

typedef struct _FXLVERTEXSHADERENTRY
{
    IDirect3DVertexShader9* pObject;
    DWORD FunctionSize;
    DWORD pFunction[0];
} FXLVERTEXSHADERENTRY;

/*
 * Pixel shader
 */

typedef struct _FXLPIXELSHADERENTRY
{
    IDirect3DPixelShader9* pObject;
    DWORD FunctionSize;
    DWORD pFunction[0];
} FXLPIXELSHADERENTRY;

/*
 * Shader state entry - Vertex shader, pixel shader, literal constant data, 
 * shader constant reference lists, and usage flags.
 */

typedef struct _FXLSHADERSTATEENTRY
{
    UINT64* pVSFUsageFlagsLocal;
    UINT64* pPSFUsageFlagsLocal;
    UINT64* pVSIUsageFlagsLocal;
    UINT64* pPSIUsageFlagsLocal;
    UINT64* pVSBUsageFlagsLocal;
    UINT64* pPSBUsageFlagsLocal;
    UINT64* pVSSUsageFlagsLocal;
    UINT64* pPSSUsageFlagsLocal;
    UINT64* pVSFUsageFlagsShared;
    UINT64* pPSFUsageFlagsShared;
    UINT64* pVSIUsageFlagsShared;
    UINT64* pPSIUsageFlagsShared;
    UINT64* pVSBUsageFlagsShared;
    UINT64* pPSBUsageFlagsShared;
    UINT64* pVSSUsageFlagsShared;
    UINT64* pPSSUsageFlagsShared;

    FXLPASSPARAMETERREF* pReferenceListLocal;
    FXLPASSPARAMETERREF* pReferenceListShared;

    FXLVERTEXSHADERENTRY* pVertexShader;
    FXLPIXELSHADERENTRY* pPixelShader;

    XMFLOAT4A16* pIntLiteralData;   // Storage of integer flow control literal data placed into components of parameter registers
    UINT IntLiteralDataSize;
} FXLSHADERSTATEENTRY;

/*
 * Render state data
 */

typedef struct _FXLRENDERSTATEDATA
{
    DWORD Type;
    DWORD Value;
} FXLRENDERSTATEDATA;

/*
 * Render state entry
 */

typedef struct _FXLRENDERSTATEENTRY
{
    FXLALIGN16 UINT64 RenderStateUsageFlags[2];
    UINT RenderStateCount;
    FXLRENDERSTATEDATA pRenderStateList[0];
} FXLRENDERSTATEENTRY;

/*
 * Sampler state data
 */

typedef struct _FXLSAMPLERSTATEDATA
{
    WORD Index;
    WORD Type;
    DWORD Value;
} FXLSAMPLERSTATEDATA;

/*
 * Sampler state entry
 */

typedef struct _FXLSAMPLERSTATEENTRY
{
    FXLALIGN16 UINT64 SamplerStateUsageFlags[16];
    UINT SamplerStateCount;
    FXLSAMPLERSTATEDATA pSamplerStateList[0];
} FXLSAMPLERSTATEENTRY;

/*
 * Pass entry
 */

typedef struct _FXLPASSENTRY
{
    CHAR* pName;
    UINT AnnotationCount;
    FXLSHADERSTATEENTRY* pShaderStates;
    FXLRENDERSTATEENTRY* pRenderStates;
    FXLSAMPLERSTATEENTRY* pSamplerStates;
} FXLPASSENTRY;

/*
 * Technique entry
 */

typedef struct _FXLTECHNIQUEENTRY
{
    CHAR* pName;
    UINT PassCount;
    UINT AnnotationCount;
    FXLPASSENTRY* pPassList[0];
} FXLTECHNIQUEENTRY;

/*
 * Globals
 */

FXLEXTERN BOOL FXL__SharedCorrelation;

/****************************************************************************
 *
 * End: Private internal data.
 *
 ****************************************************************************/

/*
 * Effect Pool
 */

struct FXLEffectPool
{
#ifdef __cplusplus

    ULONG                WINAPI AddRef();
    ULONG                WINAPI Release();

    VOID                 WINAPI SetScalarF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI GetScalarF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetScalarArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetScalarI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI GetScalarI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetScalarArrayI(FXLHANDLE Parameter, CONST INT* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayI(FXLHANDLE Parameter, INT* pData, UINT Count);
    VOID                 WINAPI SetScalarB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI GetScalarB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetScalarArrayB(FXLHANDLE Parameter, CONST BOOL* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayB(FXLHANDLE Parameter, BOOL* pData, UINT Count);
    VOID                 WINAPI SetVectorF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetVectorFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetVectorF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetVectorArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetVectorArrayF4A(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI GetVectorArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetVectorI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI SetVectorIA(FXLHANDLE Parameter, CONST FXLINTA* pData);
    VOID                 WINAPI GetVectorI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetVectorB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI SetVectorBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData);
    VOID                 WINAPI GetVectorB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetMatrixF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetMatrixFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetMatrixF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetMatrixF4x4(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetMatrixF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetMatrixF4x4(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetMatrixArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixArrayF4x4(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixArrayF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
    VOID                 WINAPI GetMatrixArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI GetMatrixArrayF4x4(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI SetMatrixIA(FXLHANDLE Parameter, CONST FXLINTA* pData);
    VOID                 WINAPI GetMatrixI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetMatrixB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI SetMatrixBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData);
    VOID                 WINAPI GetMatrixB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetSampler(FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture);
    VOID                 WINAPI GetSampler(FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture);
    VOID                 WINAPI SetParameter(FXLHANDLE Parameter, CONST VOID* pData);
    VOID                 WINAPI GetParameter(FXLHANDLE Parameter, VOID* pData);

    FXLHANDLE            WINAPI GetParameterHandle(CONST CHAR* pParameterName);
    FXLHANDLE            WINAPI GetMemberHandle(FXLHANDLE Struct, CONST CHAR* pMemberName);
    FXLHANDLE            WINAPI GetMemberHandleFromIndex(FXLHANDLE Struct, UINT MemberIndex);
    FXLHANDLE            WINAPI GetElementHandle(FXLHANDLE Array, UINT ElementIndex);

    VOID                 WINAPI GetParameterDesc(FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc);

    VOID                 WINAPI SetParameterRegisterUpdate(FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode);

#endif __cplusplus

    // Private internal data (please do not access directly):

    FXLDIRTYFLAGS m_DirtyFlags;             // Shared parameter dirty flags
    FXLALIGN128 UINT64 m_UpdateFlags[FXL_MAX_PARAMETER_FLAG_GROUPS];
    FXLPARAMETERENTRY* m_pParameterList;    // Pointer to the list of parameters in the pool    
    UINT m_ParameterListEntries;            // Number of entries in the parameter list
    XMFLOAT4A16* m_pParameterData;          // Pointer to the list of shared parameter data in the pool
    UINT m_ParameterDataEntries;            // Number of parameter data groups in the pool
    UINT m_ParameterDataSize;               // Size of the shared parameter data in bytes (inclusive of all shared data present in the pool at the time the effect was created)
    UINT m_ParameterBitGroups;              // Count of QWords used in the shared parameter bit groups

    UINT m_ParameterCount;                  // Number of top-level parameters in the the pool

    CHAR* m_pNameList;                      // Ordered list of all top level parameter names in the pool
    UINT m_NameListSize;

    CHAR** m_pParameterNameMap;             // Map of parameter names based on parameter index
};

/*
 * Effect
 */

struct FXLEffect
{
#ifdef __cplusplus

    ULONG                WINAPI AddRef();
    ULONG                WINAPI Release();

    VOID                 WINAPI BeginTechnique(FXLHANDLE Technique, DWORD Flags);
    VOID                 WINAPI BeginTechniqueFromIndex(UINT TechniqueIndex, DWORD Flags);
    VOID                 WINAPI EndTechnique();
    VOID                 WINAPI BeginPass(FXLHANDLE Pass);
    VOID                 WINAPI BeginPassFromIndex(UINT PassIndex);
    VOID                 WINAPI EndPass();

    VOID                 WINAPI SetScalarF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI GetScalarF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetScalarArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetScalarI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI GetScalarI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetScalarArrayI(FXLHANDLE Parameter, CONST INT* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayI(FXLHANDLE Parameter, INT* pData, UINT Count);
    VOID                 WINAPI SetScalarB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI GetScalarB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetScalarArrayB(FXLHANDLE Parameter, CONST BOOL* pData, UINT Count);
    VOID                 WINAPI GetScalarArrayB(FXLHANDLE Parameter, BOOL* pData, UINT Count);
    VOID                 WINAPI SetVectorF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetVectorFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetVectorF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetVectorArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetVectorArrayF4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
    VOID                 WINAPI GetVectorArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetVectorI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI SetVectorIA(FXLHANDLE Parameter, CONST FXLINTA* pData);
    VOID                 WINAPI GetVectorI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetVectorB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI SetVectorBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData);
    VOID                 WINAPI GetVectorB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetMatrixF(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetMatrixFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetMatrixF(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetMatrixF4x4(FXLHANDLE Parameter, CONST FLOAT* pData);
    VOID                 WINAPI SetMatrixF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData);
    VOID                 WINAPI GetMatrixF4x4(FXLHANDLE Parameter, FLOAT* pData);
    VOID                 WINAPI SetMatrixArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixArrayF4x4(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixArrayF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
    VOID                 WINAPI GetMatrixArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI GetMatrixArrayF4x4(FXLHANDLE Parameter, FLOAT* pData, UINT Count);
    VOID                 WINAPI SetMatrixI(FXLHANDLE Parameter, CONST INT* pData);
    VOID                 WINAPI SetMatrixIA(FXLHANDLE Parameter, CONST FXLINTA* pData);
    VOID                 WINAPI GetMatrixI(FXLHANDLE Parameter, INT* pData);
    VOID                 WINAPI SetMatrixB(FXLHANDLE Parameter, CONST BOOL* pData);
    VOID                 WINAPI SetMatrixBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData);
    VOID                 WINAPI GetMatrixB(FXLHANDLE Parameter, BOOL* pData);
    VOID                 WINAPI SetSampler(FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture);
    VOID                 WINAPI GetSampler(FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture);
    VOID                 WINAPI SetParameter(FXLHANDLE Parameter, CONST VOID* pData);
    VOID                 WINAPI GetParameter(FXLHANDLE Parameter, VOID* pData);

    VOID                 WINAPI Commit();
    VOID                 WINAPI CommitU();

    FXLHANDLE            WINAPI GetParameterHandle(CONST CHAR* pParameterName);
    FXLHANDLE            WINAPI GetParameterHandleFromIndex(UINT ParameterIndex);
    FXLHANDLE            WINAPI GetAnnotationHandle(FXLHANDLE Anchor, CONST CHAR* pAnnotationName);
    FXLHANDLE            WINAPI GetAnnotationHandleFromIndex(FXLHANDLE Anchor, UINT AnnotationIndex);
    FXLHANDLE            WINAPI GetMemberHandle(FXLHANDLE Struct, CONST CHAR* pMemberName);
    FXLHANDLE            WINAPI GetMemberHandleFromIndex(FXLHANDLE Struct, UINT MemberIndex);
    FXLHANDLE            WINAPI GetElementHandle(FXLHANDLE Array, UINT ElementIndex);
    FXLHANDLE            WINAPI GetTechniqueHandle(CONST CHAR* pTechniqueName);
    FXLHANDLE            WINAPI GetTechniqueHandleFromIndex(UINT TechniqueIndex);
    FXLHANDLE            WINAPI GetPassHandle(FXLHANDLE Technique, CONST CHAR* pPassName);
    FXLHANDLE            WINAPI GetPassHandleFromIndex(FXLHANDLE Technique, UINT PassIndex);

    VOID                 WINAPI GetEffectDesc(FXLEFFECT_DESC* pDesc);
    VOID                 WINAPI GetTechniqueDesc(FXLHANDLE Technique, FXLTECHNIQUE_DESC* pDesc);
    VOID                 WINAPI GetPassDesc(FXLHANDLE Pass, FXLPASS_DESC* pDesc);
    VOID                 WINAPI GetParameterDesc(FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc);
    VOID                 WINAPI GetAnnotationDesc(FXLHANDLE Annotation, FXLANNOTATION_DESC* pDesc);

    FXLHANDLE            WINAPI GetActiveTechnique();
    FXLHANDLE            WINAPI GetActivePass();
    VOID                 WINAPI GetRenderStateList(FXLHANDLE Pass, FXLRENDERSTATE* pList, UINT* pCount);
    VOID                 WINAPI GetSamplerStateList(FXLHANDLE Sampler, FXLSAMPLERSTATE* pList, UINT* pCount);
    VOID                 WINAPI GetRenderState(FXLHANDLE Pass, UINT StateIndex, D3DRENDERSTATETYPE* pStateType, DWORD* pStateValue);
    VOID                 WINAPI GetSamplerState(FXLHANDLE Pass, UINT StateIndex, UINT* pSamplerIndex, D3DSAMPLERSTATETYPE* pStateType, DWORD* pStateValue);
    FXLPARAMETER_CONTEXT WINAPI GetParameterContext(FXLHANDLE Pass, FXLHANDLE Parameter);
    VOID                 WINAPI GetParameterRegister(FXLHANDLE Pass, FXLHANDLE Parameter, FXLPARAMETER_CONTEXT Context, UINT* pRegisterIndex, UINT* pRegisterCount);
    VOID                 WINAPI GetAnnotation(FXLHANDLE Annotation, VOID* pData);

    VOID                 WINAPI SetParameterRegisterUpdate(FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode);

    HRESULT              WINAPI CreateInstance(FXLEffect** pEffectInstance);
    VOID                 WINAPI GetPool(FXLEffectPool** pPool);

    HRESULT              WINAPI ChangeDevice(IDirect3DDevice9* pDevice);
    HRESULT              WINAPI OnLostDevice();
    HRESULT              WINAPI OnResetDevice();

#endif __cplusplus

    // Private internal data (please do not access directly):

    FXLDIRTYFLAGS m_DirtyFlagsLocal;            // Local parameter dirty flags
    FXLALIGN128 UINT64 m_UpdateFlagsLocal[FXL_MAX_PARAMETER_FLAG_GROUPS];
    UINT64* m_pDirtyFlagsShared;                // Pointer to the dirty flags contained in the shared pool
    UINT64* m_pUpdateFlagsShared;

    FXLPARAMETERENTRY* m_pParameterListLocal;   // Pointer to the list of local parameters in the effect
    FXLPARAMETERENTRY** m_pParameterListShared; // Pointer to the list of shared parameters in the pool
    UINT m_ParameterCountLocal;                 // Number of local top-level parameters in the effect
    UINT m_ParameterCountShared;                // Number of shared top-level parameters in the effect
    UINT m_ParameterListEntriesLocal;           // Number of entries in the local parameter list
    UINT m_ParameterListEntriesShared;          // Number of entries in the shared parameter list
    UINT m_ParameterBitGroupsLocal;             // Count of QWords used in the local parameter bit groups (dirty and usage flags)
    UINT m_ParameterBitGroupsShared;            // Count of QWords used in the shared parameter bit groups

    XMFLOAT4A16* m_pParameterDataLocal;         // Pointer to the list of local parameter data in the effect
    XMFLOAT4A16** m_pParameterDataShared;       // Pointer to the list of shared parameter data in the pool
    UINT m_ParameterDataEntriesLocal;           // Number of local parameter data groups in the effect (i.e. parameter list leaf nodes)
    UINT m_ParameterDataEntriesShared;          // Number of shared parameter data groups in the effect (total number of groups in the pool at a time just after the effect was created)
    UINT m_ParameterDataSizeLocal;              // Size of the local parameter data in bytes
    UINT m_ParameterDataSizeShared;             // Size of the shared parameter data in bytes (inclusive of all shared data present in the pool at the time the effect was created)

    FXLALIGN16 UINT64 m_RenderStateDirtyFlags[2];    // Flags to track render state changes made by a technique
    FXLALIGN128 UINT64 m_SamplerStateDirtyFlags[16]; // Flags to track sampler state changes made by a technique

    FXLTECHNIQUEENTRY** m_pTechniqueList;       // Pointer to the list of techniques in the effect
    FXLTECHNIQUEENTRY* m_pActiveTechnique;      // Pointer to the currently active technique in the effect
    FXLHANDLE m_ActiveTechniqueHandle;          // Handle to the currently active technique in the effect
    UINT m_TechniqueCount;                      // Number of techniques present in the effect

    FXLPASSENTRY* m_pPassList;                  // Pointer to the list of passes in the effect
    FXLPASSENTRY* m_pActivePass;                // Pointer to the currently active pass in the effect
    FXLSHADERSTATEENTRY* m_pActiveShader;       // Pointer to the FXLSHADERSTATEENTRY used by the active pass
    FXLPASSENTRY* m_pPreviousPass;              // Pointer to the last active pass in the effect
    UINT m_PassCount;                           // Number of unique passes in the effect

    FXLSHADERSTATEENTRY* m_pShaderStateList;    // List of unique groups of shader state (based on vertex and pixel shader pairs) used by passes in the effect
    UINT m_ShaderStateCount;                    // Number of unique groups of shader state in the effect
    UINT m_ShaderStateListSize;                 // Size of the shader state list

    FXLVERTEXSHADERENTRY* m_pVertexShaderList;  // List of unique vertex shaders used by the passes in the effect
    UINT m_VertexShaderCount;                   // Number of unique vertex shaders in the effect
    UINT m_VertexShaderListSize;                // Size of the vertex shader list

    FXLPIXELSHADERENTRY* m_pPixelShaderList;    // List of unique pixel shaders used by the passes in the effect
    UINT m_PixelShaderCount;                    // Number of unique pixel shaders in the effect
    UINT m_PixelShaderListSize;                 // Size of the pixel shader list

    FXLRENDERSTATEENTRY* m_pRenderStateList;    // List of unique groups of render state used by passes in the effect
    UINT m_RenderStateCount;                    // Number of unique groups of render state in the effect
    UINT m_RenderStateListSize;                 // Size of the render state list

    FXLSAMPLERSTATEENTRY* m_pSamplerStateList;  // List of unique groups of sampler state used by passes in the effect
    UINT m_SamplerStateCount;                   // Number of unique groups of sampler state in the effect
    UINT m_SamplerStateListSize;                // Size of the sampler state list

    FXLPARAMETERENTRY* m_pAnnotationList;       // List of global annotations (strings) followed by all annotations attached to techniques, passes, and parameters (both local and shared)
    UINT m_AnnotationListEntries;               // Number of entries in the annotation list
    UINT m_AnnotationListGlobals;               // Number of global annotations (those not attached to any anchor) in the list

    DWORD* m_pAnnotationData;                   // Annotation data
    UINT m_AnnotationDataSize;                  // Size of the annotation data

    WORD* m_pAnnotationEntryMapTechnique;       // Map from a technique list offset to an offset in the annotation list to the set of annotations belonging to the technique
    WORD* m_pAnnotationEntryMapPass;            // Map from a pass list offset to an offset in the annotation list to the set of annotations belonging to the pass
    WORD* m_pAnnotationEntryMapLocal;           // Map from a local parameter list offset to an offset in the annotation list to the set of annotations belonging to the parameter
    WORD* m_pAnnotationEntryMapShared;          // Map from a shared parameter list offset to an offset in the annotation list to the set of annotations belonging to the parameter

    WORD* m_pAnnotationCountListShared;         // List of annotation counts for shared parameters (which cannot be stored in the Annotations field of an FXLPARAMETERENTRY since the count can vary across effects)

    CHAR* m_pNameListLocal;                     // Ordered list of all top level local parameter names followed after by a collection of member names, technique names, pass names, and annotation names
    UINT m_NameListSizeLocal;                   // Size of the local parameter name list in bytes
    CHAR** m_pNameListShared;                   // Ordered list of all top level shared parameter names in the effect/pool followed after by member names
    UINT m_NameListSizeShared;                  // Size of the shared parameter name list in bytes

    CHAR** m_pParameterNameMapLocal;
    CHAR*** m_pParameterNameMapShared;
    CHAR** m_pAnnotationNameMap;

    CHAR* m_pCreatorName;                       // Name of the effect creator

    FXLHANDLE* m_pParameterHandleIndexMap;
    UINT m_ParameterHandleIndexMapSize;         // Number of entries in the index map.

    DWORD m_CompilationFlags;                   // Copy of the flags given when the effect was compiled

    DWORD m_TechniqueFlags;                     // Copy of the flags given to BeginTechnique

    FXLEffectPool* m_pEffectPool;               // Pointer to the pool used by the effect

    D3DDevice* m_pDevice;
};

/*
 * Effect Pool functions and methods.
 */

ULONG                           WINAPI FXLEffectPool_AddRef(FXLEffectPool* pEffectPool);
ULONG                           WINAPI FXLEffectPool_Release(FXLEffectPool* pEffectPool);

VOID                            WINAPI FXLEffectPool_SetScalarF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffectPool_GetScalarF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetScalarArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetScalarArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetScalarI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffectPool_GetScalarI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffectPool_SetScalarArrayI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST INT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetScalarArrayI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, INT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetScalarB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffectPool_GetScalarB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffectPool_SetScalarArrayB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST BOOL* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetScalarArrayB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, BOOL* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetVectorF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetVectorFA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffectPool_GetVectorF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetVectorArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetVectorArrayF4A(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetVectorArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetVectorI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffectPool_SetVectorIA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLINTA* pData);
VOID                            WINAPI FXLEffectPool_GetVectorI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffectPool_SetVectorB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffectPool_SetVectorBA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLBOOLA* pData);
VOID                            WINAPI FXLEffectPool_GetVectorB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixFA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffectPool_GetMatrixF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixF4x4(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixF4x4A(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffectPool_GetMatrixF4x4(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetMatrixArrayF(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetMatrixArrayF4x4(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetMatrixArrayF4x4A(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_GetMatrixArrayF4x4(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffectPool_SetMatrixI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixIA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLINTA* pData);
VOID                            WINAPI FXLEffectPool_GetMatrixI(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffectPool_SetMatrixBA(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST FXLBOOLA* pData);
VOID                            WINAPI FXLEffectPool_GetMatrixB(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffectPool_SetSampler(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture);
VOID                            WINAPI FXLEffectPool_GetSampler(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture);
VOID                            WINAPI FXLEffectPool_SetParameter(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, CONST VOID* pData);
VOID                            WINAPI FXLEffectPool_GetParameter(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, VOID* pData);

FXLHANDLE                       WINAPI FXLEffectPool_GetParameterHandle(FXLEffectPool* pEffectPool, CONST CHAR* pParameterName);
FXLHANDLE                       WINAPI FXLEffectPool_GetMemberHandle(FXLEffectPool* pEffectPool, FXLHANDLE Struct, CONST CHAR* pMemberName);
FXLHANDLE                       WINAPI FXLEffectPool_GetMemberHandleFromIndex(FXLEffectPool* pEffectPool, FXLHANDLE Struct, UINT MemberIndex);
FXLHANDLE                       WINAPI FXLEffectPool_GetElementHandle(FXLEffectPool* pEffectPool, FXLHANDLE Array, UINT ElementIndex);

VOID                            WINAPI FXLEffectPool_GetParameterDesc(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc);

VOID                            WINAPI FXLEffectPool_SetParameterRegisterUpdate(FXLEffectPool* pEffectPool, FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode);

HRESULT                         WINAPI FXLEffectPool_Dump(FXLEffectPool* pPool, ID3DXBuffer** pOutput);

#ifdef __cplusplus

FXLFINLINE ULONG                WINAPI FXLEffectPool::AddRef() { return FXLEffectPool_AddRef(this); }
FXLFINLINE ULONG                WINAPI FXLEffectPool::Release() { return FXLEffectPool_Release(this); }

FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffectPool_SetScalarF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffectPool_GetScalarF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffectPool_SetScalarArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffectPool_GetScalarArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffectPool_SetScalarI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarI(FXLHANDLE Parameter, INT* pData) { FXLEffectPool_GetScalarI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarArrayI(FXLHANDLE Parameter, CONST INT* pData, UINT Count) { FXLEffectPool_SetScalarArrayI(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarArrayI(FXLHANDLE Parameter, INT* pData, UINT Count) { FXLEffectPool_GetScalarArrayI(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffectPool_SetScalarB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarB(FXLHANDLE Parameter, BOOL* pData) { FXLEffectPool_GetScalarB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetScalarArrayB(FXLHANDLE Parameter, CONST BOOL* pData, UINT Count) { FXLEffectPool_SetScalarArrayB(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetScalarArrayB(FXLHANDLE Parameter, BOOL* pData, UINT Count) { FXLEffectPool_GetScalarArrayB(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffectPool_SetVectorF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffectPool_SetVectorFA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetVectorF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffectPool_GetVectorF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffectPool_SetVectorArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorArrayF4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count) { FXLEffectPool_SetVectorArrayF4A(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetVectorArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffectPool_GetVectorArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffectPool_SetVectorI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorIA(FXLHANDLE Parameter, CONST FXLINTA* pData) { FXLEffectPool_SetVectorIA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetVectorI(FXLHANDLE Parameter, INT* pData) { FXLEffectPool_GetVectorI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffectPool_SetVectorB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetVectorBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData) { FXLEffectPool_SetVectorBA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetVectorB(FXLHANDLE Parameter, BOOL* pData) { FXLEffectPool_GetVectorB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffectPool_SetMatrixF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffectPool_SetMatrixFA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffectPool_GetMatrixF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixF4x4(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffectPool_SetMatrixF4x4(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffectPool_SetMatrixF4x4A(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixF4x4(FXLHANDLE Parameter, FLOAT* pData) { FXLEffectPool_GetMatrixF4x4(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffectPool_SetMatrixArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffectPool_GetMatrixArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixArrayF4x4(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffectPool_SetMatrixArrayF4x4(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixArrayF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count) { FXLEffectPool_SetMatrixArrayF4x4A(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixArrayF4x4(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffectPool_GetMatrixArrayF4x4(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffectPool_SetMatrixI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixIA(FXLHANDLE Parameter, CONST FXLINTA* pData) { FXLEffectPool_SetMatrixIA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixI(FXLHANDLE Parameter, INT* pData) { FXLEffectPool_GetMatrixI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffectPool_SetMatrixB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetMatrixBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData) { FXLEffectPool_SetMatrixBA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetMatrixB(FXLHANDLE Parameter, BOOL* pData) { FXLEffectPool_GetMatrixB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetSampler(FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture) { FXLEffectPool_SetSampler(this, Parameter, pTexture); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetSampler(FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture) { FXLEffectPool_GetSampler(this, Parameter, pTexture); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::SetParameter(FXLHANDLE Parameter, CONST VOID* pData) { FXLEffectPool_SetParameter(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffectPool::GetParameter(FXLHANDLE Parameter, VOID* pData) { FXLEffectPool_GetParameter(this, Parameter, pData); }

FXLFINLINE FXLHANDLE            WINAPI FXLEffectPool::GetParameterHandle(CONST CHAR* pParameterName) { return FXLEffectPool_GetParameterHandle(this, pParameterName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffectPool::GetMemberHandle(FXLHANDLE Struct, CONST CHAR* pMemberName) { return FXLEffectPool_GetMemberHandle(this, Struct, pMemberName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffectPool::GetMemberHandleFromIndex(FXLHANDLE Struct, UINT MemberIndex) { return FXLEffectPool_GetMemberHandleFromIndex(this, Struct, MemberIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffectPool::GetElementHandle(FXLHANDLE Array, UINT ElementIndex) { return FXLEffectPool_GetElementHandle(this, Array, ElementIndex); }

FXLFINLINE VOID                 WINAPI FXLEffectPool::GetParameterDesc(FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc) { FXLEffectPool_GetParameterDesc(this, Parameter, pDesc); }

FXLFINLINE VOID                 WINAPI FXLEffectPool::SetParameterRegisterUpdate(FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode) { FXLEffectPool_SetParameterRegisterUpdate(this, Parameter, UpdateMode); }

#endif __cplusplus

/*
 * Effect functions and methods.
 */

ULONG                           WINAPI FXLEffect_AddRef(FXLEffect* pEffect);
ULONG                           WINAPI FXLEffect_Release(FXLEffect* pEffect);

VOID                            WINAPI FXLEffect_BeginTechnique(FXLEffect* pEffect, FXLHANDLE Technique, DWORD Flags);
VOID                            WINAPI FXLEffect_BeginTechniqueFromIndex(FXLEffect* pEffect, UINT TechniqueIndex, DWORD Flags);
VOID                            WINAPI FXLEffect_EndTechnique(FXLEffect* pEffect);
VOID                            WINAPI FXLEffect_BeginPass(FXLEffect* pEffect, FXLHANDLE Pass);
VOID                            WINAPI FXLEffect_BeginPassFromIndex(FXLEffect* pEffect, UINT PassIndex);
VOID                            WINAPI FXLEffect_EndPass(FXLEffect* pEffect);

VOID                            WINAPI FXLEffect_SetScalarF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffect_GetScalarF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffect_SetScalarArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetScalarArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetScalarI(FXLEffect* pEffect, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffect_GetScalarI(FXLEffect* pEffect, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffect_SetScalarArrayI(FXLEffect* pEffect, FXLHANDLE Parameter, CONST INT* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetScalarArrayI(FXLEffect* pEffect, FXLHANDLE Parameter, INT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetScalarB(FXLEffect* pEffect, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffect_GetScalarB(FXLEffect* pEffect, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffect_SetScalarArrayB(FXLEffect* pEffect, FXLHANDLE Parameter, CONST BOOL* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetScalarArrayB(FXLEffect* pEffect, FXLHANDLE Parameter, BOOL* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetVectorF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffect_SetVectorFA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffect_GetVectorF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffect_SetVectorArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetVectorArrayF4A(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetVectorArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetVectorI(FXLEffect* pEffect, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffect_SetVectorIA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLINTA* pData);
VOID                            WINAPI FXLEffect_GetVectorI(FXLEffect* pEffect, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffect_SetVectorB(FXLEffect* pEffect, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffect_SetVectorBA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLBOOLA* pData);
VOID                            WINAPI FXLEffect_GetVectorB(FXLEffect* pEffect, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffect_SetMatrixF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffect_SetMatrixFA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffect_GetMatrixF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffect_SetMatrixF4x4(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData);
VOID                            WINAPI FXLEffect_SetMatrixF4x4A(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLFLOATA* pData);
VOID                            WINAPI FXLEffect_GetMatrixF4x4(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData);
VOID                            WINAPI FXLEffect_SetMatrixArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetMatrixArrayF(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetMatrixArrayF4x4(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetMatrixArrayF4x4A(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count);
VOID                            WINAPI FXLEffect_GetMatrixArrayF4x4(FXLEffect* pEffect, FXLHANDLE Parameter, FLOAT* pData, UINT Count);
VOID                            WINAPI FXLEffect_SetMatrixI(FXLEffect* pEffect, FXLHANDLE Parameter, CONST INT* pData);
VOID                            WINAPI FXLEffect_SetMatrixIA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLINTA* pData);
VOID                            WINAPI FXLEffect_GetMatrixI(FXLEffect* pEffect, FXLHANDLE Parameter, INT* pData);
VOID                            WINAPI FXLEffect_SetMatrixB(FXLEffect* pEffect, FXLHANDLE Parameter, CONST BOOL* pData);
VOID                            WINAPI FXLEffect_SetMatrixBA(FXLEffect* pEffect, FXLHANDLE Parameter, CONST FXLBOOLA* pData);
VOID                            WINAPI FXLEffect_GetMatrixB(FXLEffect* pEffect, FXLHANDLE Parameter, BOOL* pData);
VOID                            WINAPI FXLEffect_SetSampler(FXLEffect* pEffect, FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture);
VOID                            WINAPI FXLEffect_GetSampler(FXLEffect* pEffect, FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture);
VOID                            WINAPI FXLEffect_SetParameter(FXLEffect* pEffect, FXLHANDLE Parameter, CONST VOID* pData);
VOID                            WINAPI FXLEffect_GetParameter(FXLEffect* pEffect, FXLHANDLE Parameter, VOID* pData);

VOID                            WINAPI FXLEffect_Commit(FXLEffect* pEffect);
VOID                            WINAPI FXLEffect_CommitU(FXLEffect* pEffect);

FXLHANDLE                       WINAPI FXLEffect_GetParameterHandle(FXLEffect* pEffect, CONST CHAR* pParameterName);
FXLHANDLE                       WINAPI FXLEffect_GetParameterHandleFromIndex(FXLEffect* pEffect, UINT ParameterIndex);
FXLHANDLE                       WINAPI FXLEffect_GetAnnotationHandle(FXLEffect* pEffect, FXLHANDLE Anchor, CONST CHAR* pAnnotationName);
FXLHANDLE                       WINAPI FXLEffect_GetAnnotationHandleFromIndex(FXLEffect* pEffect, FXLHANDLE Anchor, UINT AnnotationIndex);
FXLHANDLE                       WINAPI FXLEffect_GetMemberHandle(FXLEffect* pEffect, FXLHANDLE Struct, CONST CHAR* pMemberName);
FXLHANDLE                       WINAPI FXLEffect_GetMemberHandleFromIndex(FXLEffect* pEffect, FXLHANDLE Struct, UINT MemberIndex);
FXLHANDLE                       WINAPI FXLEffect_GetElementHandle(FXLEffect* pEffect, FXLHANDLE Array, UINT ElementIndex);
FXLHANDLE                       WINAPI FXLEffect_GetTechniqueHandle(FXLEffect* pEffect, CONST CHAR* pTechniqueName);
FXLHANDLE                       WINAPI FXLEffect_GetTechniqueHandleFromIndex(FXLEffect* pEffect, UINT TechniqueIndex);
FXLHANDLE                       WINAPI FXLEffect_GetPassHandle(FXLEffect* pEffect, FXLHANDLE Technique, CONST CHAR* pPassName);
FXLHANDLE                       WINAPI FXLEffect_GetPassHandleFromIndex(FXLEffect* pEffect, FXLHANDLE Technique, UINT PassIndex);

VOID                            WINAPI FXLEffect_GetEffectDesc(FXLEffect* pEffect, FXLEFFECT_DESC* pDesc);
VOID                            WINAPI FXLEffect_GetTechniqueDesc(FXLEffect* pEffect, FXLHANDLE Technique, FXLTECHNIQUE_DESC* pDesc);
VOID                            WINAPI FXLEffect_GetPassDesc(FXLEffect* pEffect, FXLHANDLE Pass, FXLPASS_DESC* pDesc);
VOID                            WINAPI FXLEffect_GetParameterDesc(FXLEffect* pEffect, FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc);
VOID                            WINAPI FXLEffect_GetAnnotationDesc(FXLEffect* pEffect, FXLHANDLE Annotation, FXLANNOTATION_DESC* pDesc);

FXLHANDLE                       WINAPI FXLEffect_GetActiveTechnique(FXLEffect* pEffect);
FXLHANDLE                       WINAPI FXLEffect_GetActivePass(FXLEffect* pEffect);
VOID                            WINAPI FXLEffect_GetRenderStateList(FXLEffect* pEffect, FXLHANDLE Pass, FXLRENDERSTATE* pList, UINT* pCount);
VOID                            WINAPI FXLEffect_GetSamplerStateList(FXLEffect* pEffect, FXLHANDLE Sampler, FXLSAMPLERSTATE* pList, UINT* pCount);
VOID                            WINAPI FXLEffect_GetRenderState(FXLEffect* pEffect, FXLHANDLE Pass, UINT StateIndex, D3DRENDERSTATETYPE* pStateType, DWORD* pStateValue);
VOID                            WINAPI FXLEffect_GetSamplerState(FXLEffect* pEffect, FXLHANDLE Pass, UINT StateIndex, UINT* pSamplerIndex, D3DSAMPLERSTATETYPE* pStateType, DWORD* pStateValue);
FXLPARAMETER_CONTEXT            WINAPI FXLEffect_GetParameterContext(FXLEffect* pEffect, FXLHANDLE Pass, FXLHANDLE Parameter);
VOID                            WINAPI FXLEffect_GetParameterRegister(FXLEffect* pEffect, FXLHANDLE Pass, FXLHANDLE Parameter, FXLPARAMETER_CONTEXT Context, UINT* pRegisterIndex, UINT* pRegisterCount);
VOID                            WINAPI FXLEffect_GetAnnotation(FXLEffect* pEffect, FXLHANDLE Annotation, VOID* pData);

VOID                            WINAPI FXLEffect_SetParameterRegisterUpdate(FXLEffect* pEffect, FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode);

HRESULT                         WINAPI FXLEffect_CreateInstance(FXLEffect* pEffect, FXLEffect** pEffectInstance);
VOID                            WINAPI FXLEffect_GetPool(FXLEffect* pEffect, FXLEffectPool** pPool);

HRESULT                         WINAPI FXLEffect_ChangeDevice(FXLEffect* pEffect, IDirect3DDevice9* pDevice);
HRESULT                         WINAPI FXLEffect_OnLostDevice(FXLEffect* pEffect);
HRESULT                         WINAPI FXLEffect_OnResetDevice(FXLEffect* pEffect);

HRESULT                         WINAPI FXLEffect_Dump(FXLEffect* pEffect, ID3DXBuffer** pOutput);

#ifdef __cplusplus

FXLFINLINE ULONG                WINAPI FXLEffect::AddRef() { return FXLEffect_AddRef(this); }
FXLFINLINE ULONG                WINAPI FXLEffect::Release() { return FXLEffect_Release(this); }

FXLFINLINE VOID                 WINAPI FXLEffect::BeginTechnique(FXLHANDLE Technique, DWORD Flags) { FXLEffect_BeginTechnique(this, Technique, Flags); }
FXLFINLINE VOID                 WINAPI FXLEffect::BeginTechniqueFromIndex(UINT TechniqueIndex, DWORD Flags) { FXLEffect_BeginTechniqueFromIndex(this, TechniqueIndex, Flags); }
FXLFINLINE VOID                 WINAPI FXLEffect::EndTechnique() { FXLEffect_EndTechnique(this); }
FXLFINLINE VOID                 WINAPI FXLEffect::BeginPass(FXLHANDLE Pass) { FXLEffect_BeginPass(this, Pass); }
FXLFINLINE VOID                 WINAPI FXLEffect::BeginPassFromIndex(UINT PassIndex) { FXLEffect_BeginPassFromIndex(this, PassIndex); }
FXLFINLINE VOID                 WINAPI FXLEffect::EndPass() { FXLEffect_EndPass(this); }

FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffect_SetScalarF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffect_GetScalarF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffect_SetScalarArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffect_GetScalarArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffect_SetScalarI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarI(FXLHANDLE Parameter, INT* pData) { FXLEffect_GetScalarI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarArrayI(FXLHANDLE Parameter, CONST INT* pData, UINT Count) { FXLEffect_SetScalarArrayI(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarArrayI(FXLHANDLE Parameter, INT* pData, UINT Count) { FXLEffect_GetScalarArrayI(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffect_SetScalarB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarB(FXLHANDLE Parameter, BOOL* pData) { FXLEffect_GetScalarB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetScalarArrayB(FXLHANDLE Parameter, CONST BOOL* pData, UINT Count) { FXLEffect_SetScalarArrayB(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetScalarArrayB(FXLHANDLE Parameter, BOOL* pData, UINT Count) { FXLEffect_GetScalarArrayB(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffect_SetVectorF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffect_SetVectorFA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetVectorF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffect_GetVectorF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffect_SetVectorArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorArrayF4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count) { FXLEffect_SetVectorArrayF4A(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetVectorArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffect_GetVectorArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffect_SetVectorI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorIA(FXLHANDLE Parameter, CONST FXLINTA* pData) { FXLEffect_SetVectorIA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetVectorI(FXLHANDLE Parameter, INT* pData) { FXLEffect_GetVectorI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffect_SetVectorB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetVectorBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData) { FXLEffect_SetVectorBA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetVectorB(FXLHANDLE Parameter, BOOL* pData) { FXLEffect_GetVectorB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixF(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffect_SetMatrixF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixFA(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffect_SetMatrixFA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixF(FXLHANDLE Parameter, FLOAT* pData) { FXLEffect_GetMatrixF(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixF4x4(FXLHANDLE Parameter, CONST FLOAT* pData) { FXLEffect_SetMatrixF4x4(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData) { FXLEffect_SetMatrixF4x4A(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixF4x4(FXLHANDLE Parameter, FLOAT* pData) { FXLEffect_GetMatrixF4x4(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixArrayF(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffect_SetMatrixArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixArrayF(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffect_GetMatrixArrayF(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixArrayF4x4(FXLHANDLE Parameter, CONST FLOAT* pData, UINT Count) { FXLEffect_SetMatrixArrayF4x4(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixArrayF4x4A(FXLHANDLE Parameter, CONST FXLFLOATA* pData, UINT Count) { FXLEffect_SetMatrixArrayF4x4A(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixArrayF4x4(FXLHANDLE Parameter, FLOAT* pData, UINT Count) { FXLEffect_GetMatrixArrayF4x4(this, Parameter, pData, Count); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixI(FXLHANDLE Parameter, CONST INT* pData) { FXLEffect_SetMatrixI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixIA(FXLHANDLE Parameter, CONST FXLINTA* pData) { FXLEffect_SetMatrixIA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixI(FXLHANDLE Parameter, INT* pData) { FXLEffect_GetMatrixI(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixB(FXLHANDLE Parameter, CONST BOOL* pData) { FXLEffect_SetMatrixB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetMatrixBA(FXLHANDLE Parameter, CONST FXLBOOLA* pData) { FXLEffect_SetMatrixBA(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetMatrixB(FXLHANDLE Parameter, BOOL* pData) { FXLEffect_GetMatrixB(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetSampler(FXLHANDLE Parameter, CONST IDirect3DBaseTexture9* pTexture) { FXLEffect_SetSampler(this, Parameter, pTexture); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetSampler(FXLHANDLE Parameter, IDirect3DBaseTexture9** pTexture) { FXLEffect_GetSampler(this, Parameter, pTexture); }
FXLFINLINE VOID                 WINAPI FXLEffect::SetParameter(FXLHANDLE Parameter, CONST VOID* pData) { FXLEffect_SetParameter(this, Parameter, pData); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetParameter(FXLHANDLE Parameter, VOID* pData) { FXLEffect_GetParameter(this, Parameter, pData); }

FXLFINLINE VOID                 WINAPI FXLEffect::Commit() { FXLEffect_Commit(this); }
FXLFINLINE VOID                 WINAPI FXLEffect::CommitU() { FXLEffect_CommitU(this); }

FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetParameterHandle(CONST CHAR* pParameterName) { return FXLEffect_GetParameterHandle(this, pParameterName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetParameterHandleFromIndex(UINT ParameterIndex) { return FXLEffect_GetParameterHandleFromIndex(this, ParameterIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetAnnotationHandle(FXLHANDLE Anchor, CONST CHAR* pAnnotationName) { return FXLEffect_GetAnnotationHandle(this, Anchor, pAnnotationName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetAnnotationHandleFromIndex(FXLHANDLE Anchor, UINT AnnotationIndex) { return FXLEffect_GetAnnotationHandleFromIndex(this, Anchor, AnnotationIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetMemberHandle(FXLHANDLE Struct, CONST CHAR* pMemberName) { return FXLEffect_GetMemberHandle(this, Struct, pMemberName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetMemberHandleFromIndex(FXLHANDLE Struct, UINT MemberIndex) { return FXLEffect_GetMemberHandleFromIndex(this, Struct, MemberIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetElementHandle(FXLHANDLE Array, UINT ElementIndex) { return FXLEffect_GetElementHandle(this, Array, ElementIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetTechniqueHandle(CONST CHAR* pTechniqueName) { return FXLEffect_GetTechniqueHandle(this, pTechniqueName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetTechniqueHandleFromIndex(UINT TechniqueIndex) { return FXLEffect_GetTechniqueHandleFromIndex(this, TechniqueIndex); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetPassHandle(FXLHANDLE Technique, CONST CHAR* pPassName) { return FXLEffect_GetPassHandle(this, Technique, pPassName); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetPassHandleFromIndex(FXLHANDLE Technique, UINT PassIndex) { return FXLEffect_GetPassHandleFromIndex(this, Technique, PassIndex); }

FXLFINLINE VOID                 WINAPI FXLEffect::GetEffectDesc(FXLEFFECT_DESC* pDesc) { FXLEffect_GetEffectDesc(this, pDesc); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetTechniqueDesc(FXLHANDLE Technique, FXLTECHNIQUE_DESC* pDesc) { FXLEffect_GetTechniqueDesc(this, Technique, pDesc); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetPassDesc(FXLHANDLE Pass, FXLPASS_DESC* pDesc) { FXLEffect_GetPassDesc(this, Pass, pDesc); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetParameterDesc(FXLHANDLE Parameter, FXLPARAMETER_DESC* pDesc) { FXLEffect_GetParameterDesc(this, Parameter, pDesc); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetAnnotationDesc(FXLHANDLE Annotation, FXLANNOTATION_DESC* pDesc) { FXLEffect_GetAnnotationDesc(this, Annotation, pDesc); }

FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetActiveTechnique() { return FXLEffect_GetActiveTechnique(this); }
FXLFINLINE FXLHANDLE            WINAPI FXLEffect::GetActivePass() { return FXLEffect_GetActivePass(this); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetRenderStateList(FXLHANDLE Pass, FXLRENDERSTATE* pList, UINT* pCount) { FXLEffect_GetRenderStateList(this, Pass, pList, pCount); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetSamplerStateList(FXLHANDLE Sampler, FXLSAMPLERSTATE* pList, UINT* pCount) { FXLEffect_GetSamplerStateList(this, Sampler, pList, pCount); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetRenderState(FXLHANDLE Pass, UINT StateIndex, D3DRENDERSTATETYPE* pStateType, DWORD* pStateValue) { FXLEffect_GetRenderState(this, Pass, StateIndex, pStateType, pStateValue); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetSamplerState(FXLHANDLE Pass, UINT StateIndex, UINT* pSamplerIndex, D3DSAMPLERSTATETYPE* pStateType, DWORD* pStateValue) { FXLEffect_GetSamplerState(this, Pass, StateIndex, pSamplerIndex, pStateType, pStateValue); }
FXLFINLINE FXLPARAMETER_CONTEXT WINAPI FXLEffect::GetParameterContext(FXLHANDLE Pass, FXLHANDLE Parameter) { return FXLEffect_GetParameterContext(this, Pass, Parameter); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetParameterRegister(FXLHANDLE Pass, FXLHANDLE Parameter, FXLPARAMETER_CONTEXT Context, UINT* pRegisterIndex, UINT* pRegisterCount) { FXLEffect_GetParameterRegister(this, Pass, Parameter, Context, pRegisterIndex, pRegisterCount); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetAnnotation(FXLHANDLE Annotation, VOID* pData) { FXLEffect_GetAnnotation(this, Annotation, pData); }

FXLFINLINE VOID                 WINAPI FXLEffect::SetParameterRegisterUpdate(FXLHANDLE Parameter, FXLREGISTER_UPDATE UpdateMode) { FXLEffect_SetParameterRegisterUpdate(this, Parameter, UpdateMode); }

FXLFINLINE HRESULT              WINAPI FXLEffect::CreateInstance(FXLEffect** pEffectInstance) { return FXLEffect_CreateInstance(this, pEffectInstance); }
FXLFINLINE VOID                 WINAPI FXLEffect::GetPool(FXLEffectPool** pPool) { FXLEffect_GetPool(this, pPool); }

FXLFINLINE HRESULT              WINAPI FXLEffect::ChangeDevice(IDirect3DDevice9* pDevice) { return FXLEffect_ChangeDevice(this, pDevice); }
FXLFINLINE HRESULT              WINAPI FXLEffect::OnLostDevice() { return FXLEffect_OnLostDevice(this); }
FXLFINLINE HRESULT              WINAPI FXLEffect::OnResetDevice() { return FXLEffect_OnResetDevice(this); }

#endif __cplusplus

/*
 * Compile and create functions.
 */

HRESULT WINAPI FXLCompileEffect(
    CONST CHAR*         pSourceData,
    UINT                SourceDataLength,
    CONST D3DXMACRO*    pDefines,
    LPD3DXINCLUDE       pInclude,
    DWORD               Flags,
    ID3DXBuffer**       pCompiledData,
    ID3DXBuffer**       pErrorMessages
    );

HRESULT WINAPI FXLCreateEffect(
    IDirect3DDevice9*   pDevice,        // May be NULL.
    CONST VOID*         pCompiledData,
    FXLEffectPool*      pPool,
    FXLEffect**         pEffect
    );

HRESULT WINAPI FXLCreateEffectPool(
    FXLEffectPool**     pEffectPool
    );

HRESULT WINAPI FXLDisassembleEffect(
    CONST VOID*         pCompiledData,
    DWORD               Flags,
    LPCSTR              pComments,
    ID3DXBuffer**       pDisassembly
    );

/*
 * Default state management functions.
 */

VOID WINAPI FXLSetDefaultRenderState(
    D3DRENDERSTATETYPE StateType, 
    DWORD StateValue
    );

VOID WINAPI FXLGetDefaultRenderState(
    D3DRENDERSTATETYPE StateType, 
    DWORD* pStateValue
    );

VOID WINAPI FXLSetDefaultSamplerState(
    D3DSAMPLERSTATETYPE StateType, 
    DWORD StateValue
    );

VOID WINAPI FXLGetDefaultSamplerState(
    D3DSAMPLERSTATETYPE StateType, 
    DWORD* pStateValue
    );

#if defined(_M_PPCBE)
#pragma bitfield_order(pop)
#endif

#ifdef __cplusplus
}; // extern "C"
#endif

#include <fxl.inl>

#pragma warning(pop)

#endif /* _FXL_H_ */
