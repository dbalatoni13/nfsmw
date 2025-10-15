/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fxl.inl
 *  Content:    Xenon Effects inline function file
 *
 ****************************************************************************/

#pragma once

#ifndef __FXL_INL__
#define __FXL_INL__

#ifndef _X86_
#include <PPCIntrinsics.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 *
 * Begin: Private internal interfaces and data - Please don't access these 
 *                              directly, as they're subject to change.
 *
 ****************************************************************************/

#define FXLCONST extern CONST __declspec(selectany)

#define FXLHANDLE_PARAMETER_OFFSET_SHIFT    18
#define FXLHANDLE_PARAMETER_OFFSET_MASK     0xFFFC0000
#define FXLHANDLE_PARAMETER_INDEX_SHIFT     1
#define FXLHANDLE_PARAMETER_INDEX_MASK      0x0003FFFE

#define FXLHANDLE_PASS_OFFSET_SHIFT         FXLHANDLE_PARAMETER_OFFSET_SHIFT
#define FXLHANDLE_PASS_OFFSET_MASK          FXLHANDLE_PARAMETER_OFFSET_MASK
#define FXLHANDLE_PASS_IDENTIFIER           FXLHANDLE_PARAMETER_INDEX_MASK

#define FXLHANDLE_TECHNIQUE_OFFSET_SHIFT    FXLHANDLE_PARAMETER_OFFSET_SHIFT
#define FXLHANDLE_TECHNIQUE_OFFSET_MASK     FXLHANDLE_PARAMETER_OFFSET_MASK
#define FXLHANDLE_TECHNIQUE_IDENTIFIER      (((FXLHANDLE_PARAMETER_INDEX_MASK >> FXLHANDLE_PARAMETER_INDEX_SHIFT) - 1) << FXLHANDLE_PARAMETER_INDEX_SHIFT)

#define FXLHANDLE_ANNOTATION_OFFSET_SHIFT   FXLHANDLE_PARAMETER_OFFSET_SHIFT
#define FXLHANDLE_ANNOTATION_OFFSET_MASK    FXLHANDLE_PARAMETER_OFFSET_MASK
#define FXLHANDLE_ANNOTATION_IDENTIFIER     (((FXLHANDLE_PARAMETER_INDEX_MASK >> FXLHANDLE_PARAMETER_INDEX_SHIFT) - 2) << FXLHANDLE_PARAMETER_INDEX_SHIFT)

#define FXL_MAX_PARAMETER_DATA_ENTRIES      ((FXLHANDLE_PARAMETER_INDEX_MASK >> FXLHANDLE_PARAMETER_INDEX_SHIFT) - 2)

#define FXL_DEFAULT_RENDER_STATE_ENTRIES    75
#define FXL_DEFAULT_SAMPLER_STATE_ENTRIES   20

//------------------------------------------------------------------------------

FXLEXTERN FXLRENDERSTATEDATA  g_FXLIDefaultRenderStateList[FXL_DEFAULT_RENDER_STATE_ENTRIES];
FXLEXTERN FXLSAMPLERSTATEDATA g_FXLIDefaultSamplerStateList[FXL_DEFAULT_SAMPLER_STATE_ENTRIES];

//------------------------------------------------------------------------------

FXLCONST XMVECTORI g_FXLITypeSelect[2] = {{0, 0, 0, 0}, {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterIndexOffset[] = 
{
    1,
    2,
    1,
    2,
    3,
    3,
    1,
    2,
};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterIndexMask[] = 
{
    0x000003FF,
    0x000003FF,
    0x000FF000,
    0x000FF000,
    0x000000FF,
    0x0000FF00,
    0x3FC00000,
    0x3FC00000,
};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterIndexShift[] = 
{
    0,
    0,
    12,
    12,
    0,
    8,
    22,
    22,
};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterCountOffset[] = 
{
    1,
    2,
    1,
    2,
    1,
    2,
    1, // Don't care
    1, // Don't care
};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterCountMask[] = 
{
    0x00000C00,
    0x00000C00,
    0x00300000,
    0x00300000,
    0xC0000000,
    0xC0000000,
    0x00000000,
    0x00000000,
};

//------------------------------------------------------------------------------

FXLCONST DWORD g_FXLIRegisterCountShift[] = 
{
    10,
    10,
    20,
    20,
    30,
    30,
    0,
    0,
};

/****************************************************************************
 *
 * Debugging routines.
 *
 ****************************************************************************/

#if DBG

    #define PARAMETER_CHECK 1
    
    VOID FXLPRINT(PCHAR Format, ...);
    VOID FXLRIP(PCHAR Format, ...);
    VOID FXLWARNING(PCHAR Format, ...);
    VOID FXLASSERTMSG(BOOL cond, PCHAR Format, ...);

    #define FXLASSERT(cond) { if (!(cond)) FXLRIP("Assertion failure: %s\n", #cond); }

    #if i386
        #define FXLBREAK() { __asm { int 3 } }
    #else
        #define FXLBREAK() { __debugbreak(); }
    #endif

#else

    #define PARAMETER_CHECK 0
    
    VOID FXLPRINT(PCHAR Format, ...);

    #define FXLRIP 1 ? (void)0 : (void)
    #define FXLWARNING 1 ? (void)0 : (void)
    #define FXLASSERTMSG 1 ? (void)0 : (void)
    
    #define FXLASSERT(cond) {}
    #define FXLBREAK()

#endif

/****************************************************************************
 *
 * Helper routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_CountLeadingZeros(
    UINT Value)
{
#ifdef _X86_
    UINT Count;

    for (Count = 0; (Value < (1 << 31)) && (Count < 32); Count++)
    {
        Value <<= 1;
    }

    return Count;
#else
    return _CountLeadingZeros(Value);
#endif
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_CountLeadingZeros64(
    UINT64 Value)
{
#ifdef _X86_
    UINT Count;

    for (Count = 0; (Value < (1ull << 63)) && (Count < 64); Count++)
    {
        Value <<= 1;
    }

    return Count;
#else
    return _CountLeadingZeros64(Value);
#endif
}

//------------------------------------------------------------------------------

FXLFINLINE BOOL FXLI_IsPow2(
    DWORD Value) 
{ 
    return (((Value - 1) & Value) == 0); 
}

//------------------------------------------------------------------------------

FXLFINLINE DWORD FXLI_Log2(
    DWORD Value)
{
    FXLASSERT((Value != 0) && (FXLI_IsPow2(Value)));

    return 31 - FXLI_CountLeadingZeros(Value);
}

/****************************************************************************
 *
 * Handle management routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLFINLINE DWORD FXLI_SelectValue(
    FXLHANDLE Parameter,
    DWORD LocalValue,
    DWORD SharedValue)
{
    DWORD Select = (Parameter & 1) - 1;
    DWORD Local = LocalValue & Select;
    DWORD Shared = SharedValue & ~Select;
    return Local + Shared;
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetParameterOffset(
    FXLHANDLE Parameter)
{
    return (Parameter >> FXLHANDLE_PARAMETER_OFFSET_SHIFT);
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetParameterIndex(
    FXLHANDLE Parameter)
{
    return (Parameter & FXLHANDLE_PARAMETER_INDEX_MASK) >> FXLHANDLE_PARAMETER_INDEX_SHIFT;
}

//------------------------------------------------------------------------------

FXLFINLINE DWORD FXLI_GetHandleIdentifier(
    FXLHANDLE Handle)
{
    return (Handle & FXLHANDLE_PARAMETER_INDEX_MASK);
}

//------------------------------------------------------------------------------

FXLFINLINE BOOL FXLI_IsParameterShared(
    FXLHANDLE Parameter)
{
    return (BOOL)(Parameter & 1);
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectI_GetParameterEntry(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    DWORD Entry = FXLI_SelectValue(Parameter, (DWORD)(UINT_PTR)pEffect->m_pParameterListLocal, (DWORD)(UINT_PTR)*pEffect->m_pParameterListShared);
    return (FXLPARAMETERENTRY*)(UINT_PTR)Entry + Offset;
}

//------------------------------------------------------------------------------

FXLFINLINE XMFLOAT4A16* FXLEffectI_GetParameterData(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    return (XMFLOAT4A16*)(UINT_PTR)FXLI_SelectValue(Parameter, (DWORD)(UINT_PTR)pEffect->m_pParameterDataLocal, (DWORD)(UINT_PTR)*pEffect->m_pParameterDataShared);
}

//------------------------------------------------------------------------------

FXLFINLINE UINT64* FXLEffectI_GetDirtyFlags(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    return (UINT64*)(UINT_PTR)FXLI_SelectValue(Parameter, (DWORD)(UINT_PTR)pEffect->m_DirtyFlagsLocal, (DWORD)(UINT_PTR)pEffect->m_pDirtyFlagsShared);
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_EncodeParameterHandle(
    DWORD ListOffset,
    DWORD DataIndex,
    DWORD Shared)
{
    FXLASSERT(DataIndex < FXL_MAX_PARAMETER_DATA_ENTRIES);
    FXLASSERT((DataIndex & ~(FXLHANDLE_PARAMETER_INDEX_MASK >> FXLHANDLE_PARAMETER_INDEX_SHIFT)) == 0);
    FXLASSERT((ListOffset & ~(FXLHANDLE_PARAMETER_OFFSET_MASK >> FXLHANDLE_PARAMETER_OFFSET_SHIFT)) == 0);

    return (ListOffset << FXLHANDLE_PARAMETER_OFFSET_SHIFT) | (DataIndex << FXLHANDLE_PARAMETER_INDEX_SHIFT) | Shared;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_EncodeTechniqueHandle(
    UINT TechniqueIndex)
{
    FXLASSERT((TechniqueIndex & ~(FXLHANDLE_TECHNIQUE_OFFSET_MASK >> FXLHANDLE_TECHNIQUE_OFFSET_SHIFT)) == 0);

    return (TechniqueIndex << FXLHANDLE_TECHNIQUE_OFFSET_SHIFT) | FXLHANDLE_TECHNIQUE_IDENTIFIER;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_EncodePassHandle(
    UINT PassIndex)
{
    FXLASSERT((PassIndex & ~(FXLHANDLE_PASS_OFFSET_MASK >> FXLHANDLE_PASS_OFFSET_SHIFT)) == 0);

    return (PassIndex << FXLHANDLE_PASS_OFFSET_SHIFT) | FXLHANDLE_PASS_IDENTIFIER;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_EncodeAnnotationHandle(
    UINT AnnotationOffset)
{
    FXLASSERT((AnnotationOffset & ~(FXLHANDLE_ANNOTATION_OFFSET_MASK >> FXLHANDLE_ANNOTATION_OFFSET_SHIFT)) == 0);

    return (AnnotationOffset << FXLHANDLE_ANNOTATION_OFFSET_SHIFT) | FXLHANDLE_ANNOTATION_IDENTIFIER;
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLEffectI_ValidateParameterHandle(
    FXLEffect* pEffect,
    FXLHANDLE Handle)
{
    if (Handle == 0)
    {
        FXLRIP("ValidateParameterHandle - The given handle is NULL.\n");
    }

    if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) >= FXL_MAX_PARAMETER_DATA_ENTRIES)
    {
        if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to a technique.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_PASS_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to a pass.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to an annotation.\n");
        }
    }

    DWORD Index = FXLI_GetParameterIndex(Handle);

    if (Index >= FXLI_SelectValue(Handle, pEffect->m_ParameterDataEntriesLocal, pEffect->m_ParameterDataEntriesShared))
    {
        FXLRIP("ValidateParameterHandle - Invalid parameter index.\n");
    }

    DWORD Offset = FXLI_GetParameterOffset(Handle);

    if (Offset >= FXLI_SelectValue(Handle, pEffect->m_ParameterListEntriesLocal, pEffect->m_ParameterListEntriesShared))
    {
        FXLRIP("ValidateParameterHandle - Invalid parameter offset.\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLEffectI_ValidateTechniqueHandle(
    FXLEffect* pEffect,
    FXLHANDLE Handle)
{
    if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) != FXLHANDLE_TECHNIQUE_IDENTIFIER)
    {
        if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_PASS_IDENTIFIER)
        {
            FXLRIP("ValidateTechniqueHandle - The given handle corresponds to a pass.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLRIP("ValidateTechniqueHandle - The given handle corresponds to an annotation.\n");
        }
        else
        {
            FXLRIP("ValidateTechniqueHandle - The given handle corresponds to a parameter.\n");
        }
    }

    if ((Handle >> FXLHANDLE_TECHNIQUE_OFFSET_SHIFT) >= pEffect->m_TechniqueCount)
    {
        FXLRIP("ValidateTechniqueHandle - Invalid technique index.\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLEffectI_ValidatePassHandle(
    FXLEffect* pEffect,
    FXLHANDLE Handle)
{
    if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) != FXLHANDLE_PASS_IDENTIFIER)
    {
        if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            FXLRIP("ValidatePassHandle - The given handle corresponds to a technique.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLRIP("ValidatePassHandle - The given handle corresponds to an annotation.\n");
        }
        else
        {
            FXLRIP("ValidatePassHandle - The given handle corresponds to a parameter.\n");
        }
    }

    if ((Handle >> FXLHANDLE_PASS_OFFSET_SHIFT) >= pEffect->m_PassCount)
    {
        FXLRIP("ValidatePassHandle - Invalid pass index.\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLEffectI_ValidateAnnotationHandle(
    FXLEffect* pEffect,
    FXLHANDLE Handle)
{
    if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) != FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            FXLRIP("ValidateAnnotationHandle - The given handle corresponds to a technique.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_PASS_IDENTIFIER)
        {
            FXLRIP("ValidateAnnotationHandle - The given handle corresponds to a pass.\n");
        }
        else
        {
            FXLRIP("ValidateAnnotationHandle - The given handle corresponds to a parameter.\n");
        }
    }

    if ((Handle >> FXLHANDLE_ANNOTATION_OFFSET_SHIFT) >= pEffect->m_AnnotationListEntries)
    {
        FXLRIP("ValidateAnnotationHandle - Invalid annotation index.\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectPoolI_GetParameterEntry(
    FXLEffectPool* pPool,
    FXLHANDLE Parameter)
{
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    return pPool->m_pParameterList + Offset;
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLEffectPoolI_ValidateParameterHandle(
    FXLEffectPool* pPool,
    FXLHANDLE Handle)
{
    if (Handle == 0)
    {
        FXLRIP("ValidateParameterHandle - The given handle is NULL.\n");
    }

    if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) >= FXL_MAX_PARAMETER_DATA_ENTRIES)
    {
        if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to a technique.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_PASS_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to a pass.\n");
        }
        else if ((Handle & FXLHANDLE_PARAMETER_INDEX_MASK) == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLRIP("ValidateParameterHandle - The given handle corresponds to an annotation.\n");
        }
    }

    DWORD Index = FXLI_GetParameterIndex(Handle);

    if (Index >= pPool->m_ParameterDataEntries)
    {
        FXLRIP("ValidateParameterHandle - Invalid parameter index.\n");
    }

    DWORD Offset = FXLI_GetParameterOffset(Handle);

    if (Offset >= pPool->m_ParameterListEntries)
    {
        FXLRIP("ValidateParameterHandle - Invalid parameter offset.\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_ValidateParameterDataArray(
    FXLPARAMETERENTRY* pParameter,
    DWORD Type,
    UINT Count,
    CONST CHAR* pFunction)
{
    if (pParameter->Content != FXLPACONTENTI_ARRAY)
    {
        FXLRIP("%s - The given parameter is not an array (content %d).\n", pFunction, pParameter->Content);
    }

    if (pParameter[1].Content != FXLPACONTENTI_DATA)
    {
        FXLRIP("%s - The given parameter is not an array of data elements.\n", pFunction);
    }

    if (pParameter[1].Data.Type != Type)
    {
        FXLRIP("%s - Invalid array element data type %d.\n", pFunction, pParameter[1].Data.Type);
    }

    if (Count > pParameter->Array.Elements)
    {
        FXLRIP("%s - The given count exceeds the number of elements in the array.\n", pFunction);
    }
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_SelectArrayCount(
    UINT Count, 
    UINT Elements)
{
    DWORD Select = (FXLI_CountLeadingZeros((DWORD)Count) >> 5) - 1;
    return (UINT)((Count & Select) + (Elements & ~Select));
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetDirtyFlag(
    DWORD ParameterIndex,
    UINT64* pDirtyFlags)
{
    // Set the dirty bit corresponding to the given parameter index.

    // sld is expensive. Let's not do it.
    //pDirtyFlags[ParameterIndex >> 6] |= (1ull << (63 - (ParameterIndex & 63)));

    static unsigned char bitMasks[] =
    {
        128, 64, 32, 16, 8, 4, 2, 1
    };
    // Do a table lookup instead of the variable amount shift
    ((unsigned char*)pDirtyFlags)[ParameterIndex >> 3] |= bitMasks[ParameterIndex & 7];
}

FXLFINLINE VOID FXLI_Set4DirtyFlags(
    DWORD ParameterIndex,
    UINT64* pDirtyFlags)
{
    // Set four dirty bits at a time, for the specified index and the next three.
    // This reduces the total number of operations, and avoids a lot of load-hit-stores.

    static unsigned short bit4Masks[] =
    {
        0xF << 12, 0xF << 11, 0xF << 10, 0xF << 9,
        0xF << 8,  0xF << 7,  0xF << 6,  0xF << 5,
    };

    // This horrible expression identifes the two bytes that contain the flags we
    // are interested in. Occasionally (one out of 32 random accesses, less in reality)
    // this will cause a read/write that crosses a 32-byte boundary, giving two ~50
    // cycle penalties, but the savings in load-hit-stores makes it a good tradeoff.
    *((unsigned short*)(((unsigned char *)pDirtyFlags) + (ParameterIndex >> 3))) |=
                bit4Masks[ParameterIndex & 7];
}

/****************************************************************************
 *
 * Routines to obtain effect information.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetSiblingParameterEntryOffset(
    FXLPARAMETERENTRY* pEntry)
{
    return (pEntry->Content == FXLPACONTENTI_DATA) ? 1 : pEntry->Offset;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectI_GetParameterEntryLocal(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    return pEffect->m_pParameterListLocal + Offset;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectI_GetParameterEntryShared(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    return *pEffect->m_pParameterListShared + Offset;
}

//------------------------------------------------------------------------------

FXLFINLINE XMFLOAT4A16* FXLEffectI_GetParameterValue(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    FXLPARAMETERDATA* pData = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);
    DWORD DataList = FXLI_SelectValue(Parameter, (DWORD)pEffect->m_pParameterDataLocal, (DWORD)*pEffect->m_pParameterDataShared);
    return (XMFLOAT4A16*)DataList + pData->DataOffset;
}

//------------------------------------------------------------------------------

FXLFINLINE XMFLOAT4A16* FXLEffectI_GetParameterValueLocal(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    FXLPARAMETERDATA* pData = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntryLocal(pEffect, Parameter);
    return pEffect->m_pParameterDataLocal + pData->DataOffset;
}

//------------------------------------------------------------------------------

FXLFINLINE XMFLOAT4A16* FXLEffectI_GetParameterValueShared(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    FXLPARAMETERDATA* pData = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntryShared(pEffect, Parameter);
    return *pEffect->m_pParameterDataShared + pData->DataOffset;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectI_GetParameterList(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    return (FXLPARAMETERENTRY*)FXLI_SelectValue(Parameter, (DWORD)pEffect->m_pParameterListLocal, (DWORD)*pEffect->m_pParameterListShared);
}

//------------------------------------------------------------------------------

FXLFINLINE CHAR* FXLEffectI_GetNameList(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    return (CHAR*)FXLI_SelectValue(Parameter, (DWORD)pEffect->m_pNameListLocal, (DWORD)*pEffect->m_pNameListShared);
}

//------------------------------------------------------------------------------

FXLFINLINE CHAR** FXLEffectI_GetNameMap(
    FXLEffect* pEffect,
    FXLHANDLE Parameter)
{
    return (CHAR**)FXLI_SelectValue(Parameter, (DWORD)pEffect->m_pParameterNameMapLocal, (DWORD)*pEffect->m_pParameterNameMapShared);
}

//------------------------------------------------------------------------------

FXLFINLINE FXLTECHNIQUEENTRY* FXLEffectI_GetTechnique(
    FXLEffect* pEffect,
    FXLHANDLE Technique)
{
    return pEffect->m_pTechniqueList[Technique >> FXLHANDLE_TECHNIQUE_OFFSET_SHIFT];
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPASSENTRY* FXLEffectI_GetPass(
    FXLEffect* pEffect,
    FXLHANDLE Pass)
{
    return &pEffect->m_pPassList[Pass >> FXLHANDLE_PASS_OFFSET_SHIFT];
}

//------------------------------------------------------------------------------

FXLFINLINE FXLPARAMETERENTRY* FXLEffectI_GetAnnotationEntry(
    FXLEffect* pEffect,
    FXLHANDLE Annotation)
{
    return &pEffect->m_pAnnotationList[Annotation >> FXLHANDLE_ANNOTATION_OFFSET_SHIFT];
}

//------------------------------------------------------------------------------

FXLFINLINE DWORD* FXLEffectI_GetAnnotationData(
    FXLEffect* pEffect,
    FXLPARAMETERDATA* pAnnotation)
{
    return &pEffect->m_pAnnotationData[pAnnotation->DataOffset];
}

//------------------------------------------------------------------------------

FXLFINLINE XMFLOAT4A16* FXLEffectPoolI_GetParameterValue(
    FXLEffectPool* pEffectPool,
    FXLHANDLE Parameter)
{
    FXLPARAMETERDATA* pData = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);
    return pEffectPool->m_pParameterData + pData->DataOffset;
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetPassParameterRefIndex(
    FXLPASSPARAMETERREF* pRef, 
    UINT ContextIndex)
{
    return (pRef->DWord[g_FXLIRegisterIndexOffset[ContextIndex]] & g_FXLIRegisterIndexMask[ContextIndex]) >> g_FXLIRegisterIndexShift[ContextIndex];
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetPassParameterRefIndex(
    FXLPASSPARAMETERREF* pRef, 
    UINT ContextIndex,
    UINT RegisterIndex)
{
    FXLASSERT((RegisterIndex & ~(g_FXLIRegisterIndexMask[ContextIndex] >> g_FXLIRegisterIndexShift[ContextIndex])) == 0);
    DWORD* pDWord = &pRef->DWord[g_FXLIRegisterIndexOffset[ContextIndex]];
    *pDWord = (*pDWord & ~g_FXLIRegisterIndexMask[ContextIndex]) | (RegisterIndex << g_FXLIRegisterIndexShift[ContextIndex]);
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetPassParameterRefCount(
    FXLPASSPARAMETERREF* pRef, 
    UINT ContextIndex)
{
    return ((pRef->DWord[g_FXLIRegisterCountOffset[ContextIndex]] & g_FXLIRegisterCountMask[ContextIndex]) >> g_FXLIRegisterCountShift[ContextIndex]) + 1;
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetPassParameterRefCount(
    FXLPASSPARAMETERREF* pRef, 
    UINT ContextIndex,
    UINT RegisterCount)
{
    FXLASSERT(RegisterCount > 0);
    FXLASSERT((ContextIndex < 6) || RegisterCount == 1);
    FXLASSERT(((RegisterCount - 1) & ~(g_FXLIRegisterCountMask[ContextIndex] >> g_FXLIRegisterCountShift[ContextIndex])) == 0);
    DWORD* pDWord = &pRef->DWord[g_FXLIRegisterCountOffset[ContextIndex]];
    *pDWord = (*pDWord & ~g_FXLIRegisterCountMask[ContextIndex]) | ((RegisterCount - 1) << g_FXLIRegisterCountShift[ContextIndex]);
}

/****************************************************************************
 *
 * Routines to obtain FXLHANDLE values.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_GetParameterHandle(
    CONST CHAR* pParameterName,
    CHAR* pEntryName,
    FXLPARAMETERENTRY* pParameterList,
    UINT ParameterCount,
    DWORD Shared)
{
    CONST CHAR* pParameterChar;
    CHAR* pEntryChar;
    DWORD ParameterIndex;

    // Find the index of the parameter in the name list.  The name list is an ordered list of all parameter names.

    for (ParameterIndex = 0; ParameterIndex < ParameterCount; ParameterIndex++)
    {
        for (pEntryChar = pEntryName, pParameterChar = pParameterName;
            *pEntryChar != '\0' && *pParameterChar != '\0' && *pEntryChar == *pParameterChar;
            pEntryChar++, pParameterChar++);

        if (*pEntryChar == '\0' && 
            *pParameterChar == '\0')
        {
            // The parameter was found in the list.

            FXLPARAMETERENTRY* pEntry;
            UINT EntryIndex;
            DWORD EntryOffset;

            for (EntryIndex = 0, pEntry = pParameterList + 1; // The first entry is a guard entry and prevents NULL handle values.
                EntryIndex < ParameterIndex;
                EntryIndex++, pEntry += EntryOffset)
            {
                EntryOffset = FXLI_GetSiblingParameterEntryOffset(pEntry);
            }

            DWORD ParameterOffset = pEntry - pParameterList;
            ParameterIndex = 0;

            for (EntryIndex = 1, pEntry = pParameterList + 1;
                EntryIndex < ParameterOffset;
                EntryIndex++, pEntry++)
            {
                if (pEntry->Content == FXLPACONTENTI_DATA)
                {
                    ParameterIndex++;
                }
            }

            return FXLI_EncodeParameterHandle(ParameterOffset, ParameterIndex, Shared);
        }
        else
        {
            for (; *pEntryChar != '\0'; pEntryChar++);
            pEntryName = pEntryChar + 1;
        }
    }

    // The parameter was not found.

    return 0;
}

//------------------------------------------------------------------------------

FXLFINLINE UINT FXLI_GetMemberHandleIndex(
    CONST CHAR* pMemberName,
    FXLPARAMETERSTRUCT* pStruct,
    FXLPARAMETERENTRY* pParameterList,
    CHAR** pNameMap)
{
    if (PARAMETER_CHECK)
    {
        if (pStruct->Content != FXLPACONTENTI_STRUCT)
        {
            FXLRIP("GetMemberHandle - The given handle does not correspond to a structure.\n");
            return 0;
        }

        if (pMemberName == NULL ||
            *pMemberName == '\0')
        {
            FXLRIP("GetMemberHandle - The given member name is NULL.\n");
        }
    }

    DWORD EntryIndex;
    DWORD EntryOffset;

    FXLPARAMETERENTRY* pEntry = (FXLPARAMETERENTRY*)pStruct;

    for (EntryIndex = 0, pEntry++; 
         EntryIndex < pStruct->Members; 
         EntryIndex++, pEntry += EntryOffset)
    {
        if (strcmp(pNameMap[pEntry - pParameterList], pMemberName) == 0)
        {
            break;
        }

        EntryOffset = FXLI_GetSiblingParameterEntryOffset(pEntry);
    }

    return EntryIndex;
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_GetMemberHandleFromIndex(
    FXLHANDLE Struct,
    UINT MemberIndex,
    FXLPARAMETERSTRUCT* pStruct,
    FXLPARAMETERENTRY* pParameterList)
{
    DWORD GroupIndex;
    DWORD EntryIndex;
    DWORD EntryOffset;
    DWORD ParameterIndex = FXLI_GetParameterIndex(Struct);

    if (PARAMETER_CHECK)
    {
        if (pStruct->Content != FXLPACONTENTI_STRUCT)
        {
            FXLRIP("GetMemberHandleFromIndex - The given handle does not correspond to a structure.\n");
            return 0;
        }

        if (MemberIndex >= pStruct->Members)
        {
            FXLRIP("GetMemberHandleFromIndex - The given index exceeds the number of members in the structure.\n");
            return 0;
        }
    }

    FXLPARAMETERENTRY* pEntry = (FXLPARAMETERENTRY*)pStruct;

    pEntry++;
    for (EntryIndex = 0; EntryIndex < MemberIndex; EntryIndex++)
    {
        if (pEntry->Content == FXLPACONTENTI_DATA)
        {
            ParameterIndex++;
            pEntry++;
        }
        else
        {
            EntryOffset = pEntry->Offset;
            for (GroupIndex = 0; GroupIndex < EntryOffset; GroupIndex++, pEntry++)
            {
                if (pEntry->Content == FXLPACONTENTI_DATA)
                {
                    ParameterIndex++;
                }
            }
        }
    }

    DWORD ParameterOffset = pEntry - pParameterList;

    if (FXLI_GetHandleIdentifier(Struct) == FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        return FXLI_EncodeAnnotationHandle(ParameterOffset);
    }
    else
    {
        return FXLI_EncodeParameterHandle(ParameterOffset, ParameterIndex, (Struct & 1));
    }
}

//------------------------------------------------------------------------------

FXLFINLINE FXLHANDLE FXLI_GetElementHandle(
    FXLHANDLE Array,
    UINT ElementIndex,
    FXLPARAMETERARRAY* pArray,
    FXLPARAMETERENTRY* pParameterList)
{
    DWORD ParameterIndex = FXLI_GetParameterIndex(Array);
    DWORD GroupIndex;
    DWORD EntryIndex;
    DWORD EntryOffset;

    if (PARAMETER_CHECK)
    {
        if (pArray->Content != FXLPACONTENTI_ARRAY)
        {
            FXLRIP("GetElementHandle - The given handle does not correspond to an array.\n");
            return 0;
        }

        if (ElementIndex >= pArray->Elements)
        {
            FXLRIP("GetElementHandle - The given index exceeds the number of elements in the array.\n");
            return 0;
        }
    }

    FXLPARAMETERENTRY* pEntry = (FXLPARAMETERENTRY*)pArray;

    pEntry++;
    for (EntryIndex = 0; EntryIndex < ElementIndex; EntryIndex++)
    {
        if (pEntry->Content == FXLPACONTENTI_DATA)
        {
            ParameterIndex++;
            pEntry++;
        }
        else
        {
            EntryOffset = pEntry->Offset;
            for (GroupIndex = 0; GroupIndex < EntryOffset; GroupIndex++, pEntry++)
            {
                if (pEntry->Content == FXLPACONTENTI_DATA)
                {
                    ParameterIndex++;
                }
            }
        }
    }

    DWORD ParameterOffset = pEntry - pParameterList;

    if (FXLI_GetHandleIdentifier(Array) == FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        return FXLI_EncodeAnnotationHandle(ParameterOffset);
    }
    else
    {
        return FXLI_EncodeParameterHandle(ParameterOffset, ParameterIndex, (Array & 1));
    }
}

/****************************************************************************
 *
 * Routines to set and get parameter data.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetScalar(
    DWORD ParameterIndex,
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    UINT64* pDirtyFlags,
    XMVECTOR Value)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("SetScalar - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Class != FXLDCLASSI_SCALAR)
        {
            FXLRIP("SetScalar - Invalid parameter class %d.\n", pParameter->Class);
        }
        if (pParameter->Type == FXLDTYPEI_SAMPLER)
        {
            FXLRIP("SetScalar - Invalid parameter type of 'sampler.'\n");
        }
    }

    FXLI_SetDirtyFlag(ParameterIndex, pDirtyFlags);

#ifdef _X86_
    XMStoreScalar(pDataList + pParameter->DataOffset, Value);
#else
    __stvewx(Value, pDataList + pParameter->DataOffset, 0);
#endif
}

//------------------------------------------------------------------------------

// If you use this function you are responsible for setting the dirty bits.
FXLFINLINE VOID FXLI_SetScalarNoDirty(
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    XMVECTOR Value)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("SetScalar - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Class != FXLDCLASSI_SCALAR)
        {
            FXLRIP("SetScalar - Invalid parameter class %d.\n", pParameter->Class);
        }
        if (pParameter->Type == FXLDTYPEI_SAMPLER)
        {
            FXLRIP("SetScalar - Invalid parameter type of 'sampler.'\n");
        }
    }

#ifdef _X86_
    XMStoreScalar(pDataList + pParameter->DataOffset, Value);
#else
    __stvewx(Value, pDataList + pParameter->DataOffset, 0);
#endif
}
//------------------------------------------------------------------------------

FXLFINLINE XMVECTOR FXLI_GetScalar(
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("GetScalar - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Class != FXLDCLASSI_SCALAR)
        {
            FXLRIP("GetScalar - Invalid parameter class %d.\n", pParameter->Class);
        }
        if (pParameter->Type == FXLDTYPEI_SAMPLER)
        {
            FXLRIP("GetScalar - Invalid parameter type of 'sampler.'\n");
        }
    }

    return XMLoadVector4A(pDataList + pParameter->DataOffset);
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_ValidateSetVectorParameter(
    FXLPARAMETERDATA* pParameter)
{
    if (pParameter->Content != FXLPACONTENTI_DATA)
    {
        FXLRIP("SetVector - Invalid parameter content %d.\n", pParameter->Content);
    }
    if (pParameter->Class != FXLDCLASSI_VECTOR)
    {
        FXLRIP("SetVector - Invalid parameter class %d.\n", pParameter->Class);
    }
    if (pParameter->Type == FXLDTYPEI_SAMPLER)
    {
        FXLRIP("SetVector - Invalid parameter type of 'sampler.'\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE XMVECTOR FXLI_LoadVectorData(
    FXLPARAMETERDATA* pParameter,
    CONST VOID* pData)
{
    XMVECTOR Data;

#ifdef _X86_
    if (pParameter->Columns == 3)
    {
        Data = XMLoadFloat4((XMFLOAT4*)pData);
    }
    else if (pParameter->Columns == 2)
    {
        Data = XMLoadFloat3((XMFLOAT3*)pData);
    }
    else if (pParameter->Columns == 1)
    {
        Data = XMLoadFloat2((XMFLOAT2*)pData);
    }
    else
    {
        Data = XMLoadScalar(pData);
    }
#else
    XMVECTOR VL, VR, VS;
    DWORD Columns = pParameter->Columns;
    INT Offset = 4 * (Columns + 1);

    VS.u[3] = (3 - Columns) << 5;

    VR = __lvrx(pData, Offset);
    VL = __lvlx(pData, 0);

    VR = __vslo(VR, VS);
    Data = __vor(VL, VR);
#endif

    return Data;
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_StoreVectorData(
    FXLPARAMETERDATA* pParameter,
    XMVECTOR Value,
    VOID* pData)
{
    if (pParameter->Columns == 3)
    {
        XMStoreFloat4((XMFLOAT4*)pData, Value);
    }
    else if (pParameter->Columns == 2)
    {
        XMStoreFloat3((XMFLOAT3*)pData, Value);
    }
    else if (pParameter->Columns == 1)
    {
        XMStoreFloat2((XMFLOAT2*)pData, Value);
    }
    else
    {
        XMStoreScalar(pData, Value);
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetVector(
    DWORD ParameterIndex,
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    UINT64* pDirtyFlags,
    XMVECTOR Value)
{
    FXLI_SetDirtyFlag(ParameterIndex, pDirtyFlags);

    XMStoreVector4A(pDataList + pParameter->DataOffset, Value);
}

//------------------------------------------------------------------------------

FXLFINLINE XMVECTOR FXLI_GetVector(
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("GetVector - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Class != FXLDCLASSI_VECTOR)
        {
            FXLRIP("GetVector - Invalid parameter class %d.\n", pParameter->Class);
        }
        if (pParameter->Type == FXLDTYPEI_SAMPLER)
        {
            FXLRIP("GetVector - Invalid parameter type of 'sampler.'\n");
        }
    }

    return XMLoadVector4A(pDataList + pParameter->DataOffset);
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_ValidateSetMatrixParameter(
    FXLPARAMETERDATA* pParameter)
{
    if (pParameter->Content != FXLPACONTENTI_DATA)
    {
        FXLRIP("SetMatrix - Invalid parameter content %d.\n", pParameter->Content);
    }
    if (pParameter->Class != FXLDCLASSI_RMATRIX &&
        pParameter->Class != FXLDCLASSI_CMATRIX)
    {
        FXLRIP("SetMatrix - Invalid parameter class %d.\n", pParameter->Class);
    }
    if (pParameter->Type == FXLDTYPEI_SAMPLER)
    {
        FXLRIP("SetMatrix - Invalid parameter type of 'sampler.'\n");
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_LoadMatrixData(
    FXLPARAMETERDATA* pParameter,
    CONST VOID* pData,
    XMMATRIX* pMatrix)
{
    DWORD* pSource = (DWORD*)pData;

#ifdef _X86_

    UINT Rows = pParameter->Rows + 1;
    UINT Columns = pParameter->Columns + 1;
    UINT Row;
    if (pParameter->Columns == 3)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat4((XMFLOAT4*)(pSource + Columns * Row));
        }
    }
    else if (pParameter->Columns == 2)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat3((XMFLOAT3*)(pSource + Columns * Row));
        }
    }
    else if (pParameter->Columns == 1)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat2((XMFLOAT2*)(pSource + Columns * Row));
        }
    }
    else
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadScalar(pSource + Columns * Row);
        }
    }

#else

    XMVECTOR VL0, VL1, VL2, VL3, VR0, VR1, VR2, VR3, VS;
    DWORD* pSource1, *pSource2, *pSource3;
    DWORD Rows = pParameter->Rows;
    DWORD Columns = pParameter->Columns + 1;
    INT Offset = 4 * Columns;
    VS.u[3] = (4 - Columns) << 5;

    pSource3 = pSource + Columns * Rows;
    pSource2 = pSource + Columns * (Rows & 2);
    pSource1 = pSource + Columns * ((Rows & 1) | (Rows >> 1));

    VR0 = __lvrx(pSource, Offset);
    VL0 = __lvlx(pSource, 0);
    VR3 = __lvrx(pSource3, Offset);
    VL3 = __lvlx(pSource3, 0);

    VR2 = __lvrx(pSource2, Offset);
    VL2 = __lvlx(pSource2, 0);
    VR1 = __lvrx(pSource1, Offset);
    VL1 = __lvlx(pSource1, 0);

    VR0 = __vslo(VR0, VS);
    VR3 = __vslo(VR3, VS);
    VR2 = __vslo(VR2, VS);
    VR1 = __vslo(VR1, VS);

    pMatrix->r[0] = __vor(VL0, VR0);
    pMatrix->r[3] = __vor(VL3, VR3);
    pMatrix->r[2] = __vor(VL2, VR2);
    pMatrix->r[1] = __vor(VL1, VR1);

#endif
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_LoadMatrixDataA(
    FXLPARAMETERDATA* pParameter,
    CONST VOID* pData,
    XMMATRIX* pMatrix)
{
    DWORD* pSource = (DWORD*)pData;

#ifdef _X86_

    UINT Rows = pParameter->Rows + 1;
    UINT Columns = pParameter->Columns + 1;
    UINT Row;
    if (pParameter->Columns == 3)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat4((XMFLOAT4*)(pSource + Columns * Row));
        }
    }
    else if (pParameter->Columns == 2)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat3((XMFLOAT3*)(pSource + Columns * Row));
        }
    }
    else if (pParameter->Columns == 1)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadFloat2((XMFLOAT2*)(pSource + Columns * Row));
        }
    }
    else
    {
        for (Row = 0; Row < Rows; Row++)
        {
            pMatrix->r[Row] = XMLoadScalar(pSource + Columns * Row);
        }
    }

#else

    XMVECTOR VL1, VL2, VL3, VR1, VR2, VR3, VS;
    DWORD* pSource1, *pSource2, *pSource3;
    DWORD Rows = pParameter->Rows;
    DWORD Columns = pParameter->Columns + 1;
    INT Offset = 4 * Columns;
    VS.u[3] = (4 - Columns) << 5;

    pSource3 = pSource + Columns * Rows;
    pSource2 = pSource + Columns * (Rows & 2);
    pSource1 = pSource + Columns * ((Rows & 1) | (Rows >> 1));

    pMatrix->r[0] = __lvx(pSource, 0);
    VR3 = __lvrx(pSource3, Offset);
    VL3 = __lvlx(pSource3, 0);

    VR2 = __lvrx(pSource2, Offset);
    VL2 = __lvlx(pSource2, 0);
    VR1 = __lvrx(pSource1, Offset);
    VL1 = __lvlx(pSource1, 0);

    VR3 = __vslo(VR3, VS);
    VR2 = __vslo(VR2, VS);
    VR1 = __vslo(VR1, VS);

    pMatrix->r[3] = __vor(VL3, VR3);
    pMatrix->r[2] = __vor(VL2, VR2);
    pMatrix->r[1] = __vor(VL1, VR1);

#endif
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_StoreMatrixData(
    FXLPARAMETERDATA* pParameter,
    XMMATRIX Value,
    CONST VOID* pData)
{
    UINT Row;
    UINT Rows = pParameter->Rows + 1;

    if (pParameter->Columns == 3)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            XMStoreFloat4((XMFLOAT4*)pData + Row, Value.r[Row]);
        }
    }
    else if (pParameter->Columns == 2)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            XMStoreFloat3((XMFLOAT3*)pData + Row, Value.r[Row]);
        }
    }
    else if (pParameter->Columns == 1)
    {
        for (Row = 0; Row < Rows; Row++)
        {
            XMStoreFloat2((XMFLOAT2*)pData + Row, Value.r[Row]);
        }
    }
    else
    {
        for (Row = 0; Row < Rows; Row++)
        {
#ifdef _XBOX
            XMVECTOR Scalar = XMVectorSplatX(Value.r[Row]);
            __stvewx(Scalar, (FLOAT*)pData + Row, 0);
#else
            *((DWORD*)pData + Row) = Value.r[Row].u[0];
#endif
        }
    }
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetMatrix(
    DWORD ParameterIndex,
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    UINT64* pDirtyFlags,
    XMMATRIX Value)
{
    XMVECTOR SelectOrder = *(XMVECTOR*)g_FXLITypeSelect[(pParameter->Class + 1) >> 2];

    XMMATRIX ValueT = XMMatrixTranspose(Value);

    Value.r[0] = XMVectorSelect(Value.r[0], ValueT.r[0], SelectOrder);
    Value.r[1] = XMVectorSelect(Value.r[1], ValueT.r[1], SelectOrder);
    Value.r[2] = XMVectorSelect(Value.r[2], ValueT.r[2], SelectOrder);
    Value.r[3] = XMVectorSelect(Value.r[3], ValueT.r[3], SelectOrder);

    XMFLOAT4X4A16* pDestination = (XMFLOAT4X4A16*)(pDataList + pParameter->DataOffset);

    FXLI_SetDirtyFlag(ParameterIndex, pDirtyFlags);

    XMStoreFloat4x4A16(pDestination, Value);
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_GetMatrix(
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    XMMATRIX* pMatrix)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("GetMatrix - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Class != FXLDCLASSI_RMATRIX &&
            pParameter->Class != FXLDCLASSI_CMATRIX)
        {
            FXLRIP("GetMatrix - Invalid parameter class %d.\n", pParameter->Class);
        }
        if (pParameter->Type == FXLDTYPEI_SAMPLER)
        {
            FXLRIP("GetMatrix - Invalid parameter type of 'sampler.'\n");
        }
    }

    XMMATRIX Value = XMLoadFloat4x4A16((XMFLOAT4X4A16*)(pDataList + pParameter->DataOffset));

    XMVECTOR SelectOrder = *(XMVECTOR*)g_FXLITypeSelect[(pParameter->Class + 1) >> 2];

    XMMATRIX ValueT = XMMatrixTranspose(Value);

    Value.r[0] = XMVectorSelect(Value.r[0], ValueT.r[0], SelectOrder);
    Value.r[1] = XMVectorSelect(Value.r[1], ValueT.r[1], SelectOrder);
    Value.r[2] = XMVectorSelect(Value.r[2], ValueT.r[2], SelectOrder);
    Value.r[3] = XMVectorSelect(Value.r[3], ValueT.r[3], SelectOrder);

    *pMatrix = Value;
}

//------------------------------------------------------------------------------

FXLFINLINE VOID FXLI_SetSampler(
    DWORD ParameterIndex,
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList,
    UINT64* pDirtyFlags,
    CONST IDirect3DBaseTexture9* pTexture)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("SetSampler - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Type != FXLDTYPEI_SAMPLER)
        {
            FXLRIP("SetSampler - Invalid parameter type %d.'\n", pParameter->Type);
        }
    }

    CONST IDirect3DBaseTexture9** pDestination = (CONST IDirect3DBaseTexture9**)(pDataList + pParameter->DataOffset);

    FXLI_SetDirtyFlag(ParameterIndex, pDirtyFlags);

    *pDestination = pTexture;
}

//------------------------------------------------------------------------------

FXLFINLINE IDirect3DBaseTexture9* FXLI_GetSampler(
    FXLPARAMETERDATA* pParameter,
    XMFLOAT4A16* pDataList)
{
    if (PARAMETER_CHECK)
    {
        if (pParameter->Content != FXLPACONTENTI_DATA)
        {
            FXLRIP("GetSampler - Invalid parameter content %d.\n", pParameter->Content);
        }
        if (pParameter->Type != FXLDTYPEI_SAMPLER)
        {
            FXLRIP("GetSampler - Invalid parameter type of %d.\n", pParameter->Type);
        }
    }

    return *(IDirect3DBaseTexture9**)(pDataList + pParameter->DataOffset);
}

/****************************************************************************
 *
 * Description routines.
 *
 ****************************************************************************/

FXLFINLINE VOID FXLI_InitializeParameterDesc(
    FXLHANDLE Parameter,
    FXLPARAMETERENTRY* pParameter,
    CHAR** pNameMap,
    DWORD Offset,
    FXLPARAMETER_DESC* pDesc)
{
    pDesc->pName = pNameMap[Offset];
    pDesc->Content = (FXLPA_CONTENT)pParameter->Content;
    pDesc->Flags = pParameter->Flags | (Parameter & 1); // Add the shared flag as appropriate
    pDesc->Size = pParameter->Size << 2;

    if (pParameter->Content == FXLPACONTENT_DATA)
    {
        pDesc->Type = (FXLDATA_TYPE)pParameter->Data.Type;
        pDesc->Class = (FXLDATA_CLASS)(pParameter->Data.Class + ((pParameter->Data.Type + 1) & 4));
        pDesc->Rows = pParameter->Data.Rows + 1;
        pDesc->Columns = pParameter->Data.Columns + 1;
        pDesc->Elements = 0;
    }
    else
    {
        pDesc->Type = FXLDTYPE_CONTAINER;
        pDesc->Class = FXLDCLASS_CONTAINER;
        pDesc->Rows = 1;
        pDesc->Columns = 1;
        pDesc->Elements = pParameter->Struct.Members; // Same bit-field location as pParameter->Array.Elements.
    }
}

/****************************************************************************
 *
 * Technique and pass routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE VOID FXLI_BeginTechnique(
    FXLEffect* pEffect, 
    FXLTECHNIQUEENTRY* pTechnique, 
    DWORD Flags)
{
    if (PARAMETER_CHECK)
    {
#if DBG
        if (FXL__EnforceSharedCorrelation && 
            (FXL__SharedCorrelation == FALSE))
        {
            FXLRIP("BeginTechnique - Shared parameter correlation was lost before the \n"
                    "FXL__EnforceSharedCorrelation global was set to TRUE (or execution \n"
                    "proceeded after the lost correlation RIP was encountered).  Set the global \n"
                    "to TRUE before creating any effects or pools in order to determine what \n"
                    "condition caused correlation to be lost.\n");
        }
#endif
    }

    // Save the flags.

    pEffect->m_TechniqueFlags = Flags;

    // Ensure that all dirty flags are updated on the first pass.

    pEffect->m_pPreviousPass = NULL;

    if (pEffect->m_TechniqueFlags & FXL_RESTORE_DEFAULT_STATE)
    {
        // Clear render and sampler state dirty flags.

        XMVECTOR Zero = XMVectorZero();
        XMStoreFloat4A16((XMFLOAT4A16*)pEffect->m_RenderStateDirtyFlags, Zero);
#ifndef _X86_
        __dcbz128(0, pEffect->m_SamplerStateDirtyFlags);
#else
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 2), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 4), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 6), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 8), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 10), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 12), Zero);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 14), Zero);
#endif
    }

    pEffect->m_pActiveTechnique = pTechnique;
}

//------------------------------------------------------------------------------

FXLINLINE VOID FXLI_BeginPass(
    FXLEffect* pEffect, 
    FXLPASSENTRY* pPass)
{
    // Dirty the parameter flags if either of the shaders has changed from the previous pass.

    FXLSHADERSTATEENTRY* pShaderStateData = pPass->pShaderStates;
    FXLRENDERSTATEENTRY* pRenderStateData = pPass->pRenderStates;
    FXLSAMPLERSTATEENTRY* pSamplerStateData = pPass->pSamplerStates;

    if ((pEffect->m_pPreviousPass == NULL) ||
        ((pShaderStateData != NULL) && 
         (pShaderStateData != pEffect->m_pPreviousPass->pShaderStates)))
    {
        XMVECTOR Flags = XMVectorTrueInt();
        UINT Group;

        // Dirty all local parameters.

#ifndef _X86_
        C_ASSERT((FXL_MAX_PARAMETER_FLAG_GROUPS * sizeof(UINT64)) == 128);
        __dcbz128(0, pEffect->m_DirtyFlagsLocal);
#endif
        for (Group = 0; Group < pEffect->m_ParameterBitGroupsLocal; Group+=2)
        {
            XMStoreFloat4A16((XMFLOAT4A16*)&pEffect->m_DirtyFlagsLocal[Group], Flags);
        }

        // Dirty all shared parameters if shared parameter correlation has not been preserved in the system.

        if ((FXL__SharedCorrelation == FALSE) ||
            (pEffect->m_pEffectPool == NULL))
        {
#ifndef _X86_
            __dcbz128(0, pEffect->m_pDirtyFlagsShared);
#endif
            for (Group = 0; Group < pEffect->m_ParameterBitGroupsShared; Group+=2)
            {
                XMStoreFloat4A16((XMFLOAT4A16*)&pEffect->m_pDirtyFlagsShared[Group], Flags);
            }
        }
    }

    // Set the shaders.

    IDirect3DDevice9* pDevice = pEffect->m_pDevice;

    pDevice->SetVertexShader(pShaderStateData->pVertexShader->pObject);
    pDevice->SetPixelShader(pShaderStateData->pPixelShader->pObject);

    // Set the render states.

    UINT StateIndex;

    FXLRENDERSTATEDATA* pRenderStateList = pRenderStateData->pRenderStateList;

    for (StateIndex = 0; StateIndex < pRenderStateData->RenderStateCount; StateIndex++)
    {
        pDevice->SetRenderState((D3DRENDERSTATETYPE)pRenderStateList[StateIndex].Type, pRenderStateList[StateIndex].Value);
    }

    // Set the sampler states.

    FXLSAMPLERSTATEDATA* pSamplerStateList = pSamplerStateData->pSamplerStateList;

    for (StateIndex = 0; StateIndex < pSamplerStateData->SamplerStateCount; StateIndex++)
    {
        pDevice->SetSamplerState(pSamplerStateList[StateIndex].Index, (D3DSAMPLERSTATETYPE)pSamplerStateList[StateIndex].Type, pSamplerStateList[StateIndex].Value);
    }

    // Update the render and sampler state dirty flags

    if (pEffect->m_TechniqueFlags & FXL_RESTORE_DEFAULT_STATE)
    {
        XMVECTOR StateDirtyFlags[9];
        XMVECTOR StateUsageFlags[9];

        StateDirtyFlags[0] = XMLoadFloat4A16((XMFLOAT4A16*)pEffect->m_RenderStateDirtyFlags);
        StateUsageFlags[0] = XMLoadFloat4A16((XMFLOAT4A16*)pRenderStateData->RenderStateUsageFlags);
        StateDirtyFlags[1] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags));
        StateUsageFlags[1] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags));
        StateDirtyFlags[2] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 2));
        StateUsageFlags[2] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 2));
        StateDirtyFlags[3] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 4));
        StateUsageFlags[3] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 4));
        StateDirtyFlags[4] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 6));
        StateUsageFlags[4] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 6));
        StateDirtyFlags[5] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 8));
        StateUsageFlags[5] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 8));
        StateDirtyFlags[6] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 10));
        StateUsageFlags[6] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 10));
        StateDirtyFlags[7] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 12));
        StateUsageFlags[7] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 12));
        StateDirtyFlags[8] = XMLoadFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 14));
        StateUsageFlags[8] = XMLoadFloat4A16((XMFLOAT4A16*)(pSamplerStateData->SamplerStateUsageFlags + 14));

        StateDirtyFlags[0] = XMVectorOrInt(StateDirtyFlags[0], StateUsageFlags[0]);
        StateDirtyFlags[1] = XMVectorOrInt(StateDirtyFlags[1], StateUsageFlags[1]);
        StateDirtyFlags[2] = XMVectorOrInt(StateDirtyFlags[2], StateUsageFlags[2]);
        StateDirtyFlags[3] = XMVectorOrInt(StateDirtyFlags[3], StateUsageFlags[3]);
        StateDirtyFlags[4] = XMVectorOrInt(StateDirtyFlags[4], StateUsageFlags[4]);
        StateDirtyFlags[5] = XMVectorOrInt(StateDirtyFlags[5], StateUsageFlags[5]);
        StateDirtyFlags[6] = XMVectorOrInt(StateDirtyFlags[6], StateUsageFlags[6]);
        StateDirtyFlags[7] = XMVectorOrInt(StateDirtyFlags[7], StateUsageFlags[7]);
        StateDirtyFlags[8] = XMVectorOrInt(StateDirtyFlags[8], StateUsageFlags[8]);

        XMStoreFloat4A16((XMFLOAT4A16*)pEffect->m_RenderStateDirtyFlags, StateDirtyFlags[0]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags), StateDirtyFlags[1]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 2), StateDirtyFlags[2]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 4), StateDirtyFlags[3]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 6), StateDirtyFlags[4]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 8), StateDirtyFlags[5]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 10), StateDirtyFlags[6]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 12), StateDirtyFlags[7]);
        XMStoreFloat4A16((XMFLOAT4A16*)(pEffect->m_SamplerStateDirtyFlags + 14), StateDirtyFlags[8]);
    }

    pEffect->m_pActivePass = pPass;
    pEffect->m_pActiveShader = pShaderStateData;
}

/****************************************************************************
 *
 * End: Private internal interfaces and data.
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Public routines to obtain FXLHANDLE values.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetParameterHandle(
    FXLEffect* pEffect, 
    CONST CHAR* pParameterName)
{
    CHAR* pEntryName;
    DWORD ParameterCount;
    FXLPARAMETERENTRY* pParameterList;
    DWORD ListIndex;
    FXLHANDLE Handle;

    if (PARAMETER_CHECK)
    {
        if (pParameterName == NULL ||
            *pParameterName == '\0')
        {
            FXLRIP("GetParameterHandle - The given parameter name is NULL.\n");
        }
    }

    // Find the index of the parameter in the name list.  The name list is an ordered list of all parameter names.

    ParameterCount = pEffect->m_ParameterCountLocal;
    pParameterList = pEffect->m_pParameterListLocal;

    pEntryName = pEffect->m_pNameListLocal;

    for (ListIndex = 0; ListIndex < 2; ListIndex++)
    {
        Handle = FXLI_GetParameterHandle(pParameterName, pEntryName, pParameterList, ParameterCount, ListIndex);
        if (Handle != 0)
        {
            return Handle;
        }

        ParameterCount = pEffect->m_ParameterCountShared;
        pParameterList = *pEffect->m_pParameterListShared;
        pEntryName = *pEffect->m_pNameListShared;
    }

    // The parameter was not found.

    return 0;
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetParameterHandleFromIndex(
    FXLEffect* pEffect, 
    UINT ParameterIndex)
{
    if (PARAMETER_CHECK)
    {
        if (ParameterIndex >= (pEffect->m_ParameterHandleIndexMapSize))
        {
            FXLRIP("GetParameterHandleFromIndex - The given index exceeds the number of parameters in the effect.\n");
        }
    }

    // The original index cannot be inferred from the position of a parameter in a list since
    // local and shared parameters have been split into separate lists.  The handle is instead
    // retreived from a map created during compilation.

    return pEffect->m_pParameterHandleIndexMap[ParameterIndex];
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetAnnotationHandle(
    FXLEffect* pEffect,
    FXLHANDLE Anchor, 
    CONST CHAR* pAnnotationName)
{
    DWORD AnnotationEntryOffset;

    if (PARAMETER_CHECK)
    {
        if (pAnnotationName == NULL ||
            *pAnnotationName == '\0')
        {
            FXLRIP("GetAnnotationHandle - The given annotation name is NULL.\n");
        }
    }

    if (Anchor == NULL)
    {
        if (PARAMETER_CHECK)
        {
            if (pEffect->m_AnnotationListGlobals == 0)
            {
                FXLRIP("GetAnnotationHandle - The effect has no global string annotations.\n");
                return 0;
            }
        }

        FXLPARAMETERENTRY* pEntry = pEffect->m_pAnnotationList;
        UINT Index;

        for (Index = 0; Index < pEffect->m_AnnotationListGlobals; Index++)
        {
            AnnotationEntryOffset = pEntry - pEffect->m_pAnnotationList;

            if (strcmp(pEffect->m_pAnnotationNameMap[AnnotationEntryOffset], pAnnotationName) == 0)
            {
                break;
            }

            pEntry += FXLI_GetSiblingParameterEntryOffset(pEntry);
        }

        if (Index == pEffect->m_AnnotationListGlobals)
        {
            // The annotation was not found.

            return 0;
        }
    }
    else
    {
        DWORD AnchorType = FXLI_GetHandleIdentifier(Anchor);

        if (PARAMETER_CHECK)
        {
            if (AnchorType == FXLHANDLE_ANNOTATION_IDENTIFIER)
            {
                FXLRIP("GetAnnotationHandleFromIndex - The given anchor is itself an annotation.\n");
                return 0;
            }
            if (AnchorType == FXLHANDLE_TECHNIQUE_IDENTIFIER)
            {
                FXLEffectI_ValidateTechniqueHandle(pEffect, Anchor);
            }
            else if (AnchorType == FXLHANDLE_PASS_IDENTIFIER)
            {
                FXLEffectI_ValidatePassHandle(pEffect, Anchor);
            }
            else
            {
                FXLEffectI_ValidateParameterHandle(pEffect, Anchor);
            }
        }

        DWORD AnchorOffset = Anchor >> FXLHANDLE_PARAMETER_OFFSET_SHIFT;
        UINT AnnotationCount;

        if (AnchorType == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Anchor);
            AnnotationCount = pTechnique->AnnotationCount;

            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapTechnique[AnchorOffset];
        }
        else if (AnchorType == FXLHANDLE_PASS_IDENTIFIER)
        {
            FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Anchor);
            AnnotationCount = pPass->AnnotationCount;

            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapPass[AnchorOffset];
        }
        else if (FXLI_IsParameterShared(Anchor) == FALSE)
        {
            FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntryLocal(pEffect, Anchor);
            AnnotationCount = pParameter->Annotations;

            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapLocal[AnchorOffset];
        }
        else
        {
            FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntryShared(pEffect, Anchor);
            AnnotationCount = (pEffect->m_AnnotationListEntries > 0) ? pEffect->m_pAnnotationCountListShared[pParameter - *pEffect->m_pParameterListShared] : 0;

            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapShared[AnchorOffset];
        }

        if (PARAMETER_CHECK)
        {
            if (AnnotationCount == 0)
            {
                FXLRIP("GetAnnotationHandle - The given anchor has no attached annotations.\n");
            }
        }

        FXLPARAMETERENTRY* pAnnotation = pEffect->m_pAnnotationList + AnnotationEntryOffset;
        UINT Index;

        for (Index = 0; Index < AnnotationCount; Index++)
        {
            if (strcmp(pEffect->m_pAnnotationNameMap[pAnnotation - pEffect->m_pAnnotationList], pAnnotationName) == 0)
            {
                break;
            }

            pAnnotation += FXLI_GetSiblingParameterEntryOffset(pAnnotation);
        }

        if (Index < AnnotationCount)
        {
            AnnotationEntryOffset = pAnnotation - pEffect->m_pAnnotationList;
        }
        else
        {
            // The annotation was not found.

            return 0;
        }
    }

    return FXLI_EncodeAnnotationHandle(AnnotationEntryOffset);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetAnnotationHandleFromIndex(
    FXLEffect* pEffect,
    FXLHANDLE Anchor, 
    UINT AnnotationIndex)
{
    DWORD AnnotationEntryOffset;

    if (Anchor == NULL)
    {
        if (PARAMETER_CHECK)
        {
            if (AnnotationIndex >= (pEffect->m_AnnotationListGlobals))
            {
                FXLRIP("GetAnnotationHandleFromIndex - The given index exceeds the number of global string annotations in the effect.\n");
                return 0;
            }
        }

        FXLPARAMETERENTRY* pEntry = pEffect->m_pAnnotationList;

        for (UINT Index = 0; Index < AnnotationIndex; Index++)
        {
            pEntry += FXLI_GetSiblingParameterEntryOffset(pEntry);
        }

        AnnotationEntryOffset = pEntry - pEffect->m_pAnnotationList;
    }
    else
    {
        DWORD AnchorType = FXLI_GetHandleIdentifier(Anchor);

        if (PARAMETER_CHECK)
        {
            UINT AnnotationCount;

            if (AnchorType == FXLHANDLE_ANNOTATION_IDENTIFIER)
            {
                FXLRIP("GetAnnotationHandleFromIndex - The given anchor is itself an annotation.\n");
                return 0;
            }
            if (AnchorType == FXLHANDLE_TECHNIQUE_IDENTIFIER)
            {
                FXLEffectI_ValidateTechniqueHandle(pEffect, Anchor);

                FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Anchor);
                AnnotationCount = pTechnique->AnnotationCount;
            }
            else if (AnchorType == FXLHANDLE_PASS_IDENTIFIER)
            {
                FXLEffectI_ValidatePassHandle(pEffect, Anchor);

                FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Anchor);
                AnnotationCount = pPass->AnnotationCount;
            }
            else if (FXLI_IsParameterShared(Anchor) == FALSE)
            {
                FXLEffectI_ValidateParameterHandle(pEffect, Anchor);

                FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntryLocal(pEffect, Anchor);
                AnnotationCount = pParameter->Annotations;
            }
            else
            {
                FXLEffectI_ValidateParameterHandle(pEffect, Anchor);

                FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntryShared(pEffect, Anchor);
                AnnotationCount = (pEffect->m_AnnotationListEntries > 0) ? pEffect->m_pAnnotationCountListShared[pParameter - *pEffect->m_pParameterListShared] : 0;
            }

            if (AnnotationCount == 0)
            {
                FXLRIP("GetAnnotationHandleFromIndex - The given anchor has no attached annotations.\n");
            }
            if (AnnotationIndex >= AnnotationCount)
            {
                FXLRIP("GetAnnotationHandleFromIndex - The given index exceeds the number of annotations attached to the given anchor.\n");
            }
        }

        DWORD AnchorOffset = Anchor >> FXLHANDLE_PARAMETER_OFFSET_SHIFT;

        if (AnchorType == FXLHANDLE_TECHNIQUE_IDENTIFIER)
        {
            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapTechnique[AnchorOffset];
        }
        else if (AnchorType == FXLHANDLE_PASS_IDENTIFIER)
        {
            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapPass[AnchorOffset];
        }
        else if (FXLI_IsParameterShared(Anchor) == FALSE)
        {
            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapLocal[AnchorOffset];
        }
        else
        {
            AnnotationEntryOffset = (DWORD)pEffect->m_pAnnotationEntryMapShared[AnchorOffset];
        }

        FXLPARAMETERENTRY* pAnnotation = pEffect->m_pAnnotationList + AnnotationEntryOffset;
        for (UINT Index = 0; Index < AnnotationIndex; Index++)
        {
            pAnnotation += FXLI_GetSiblingParameterEntryOffset(pAnnotation);
        }

        AnnotationEntryOffset = pAnnotation - pEffect->m_pAnnotationList;
    }

    return FXLI_EncodeAnnotationHandle(AnnotationEntryOffset);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetMemberHandle(
    FXLEffect* pEffect, 
    FXLHANDLE Struct, 
    CONST CHAR* pMemberName)
{
    DWORD StructType = FXLI_GetHandleIdentifier(Struct);

    if (PARAMETER_CHECK)
    {
        if (StructType == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLEffectI_ValidateAnnotationHandle(pEffect, Struct);
        }
        else
        {
            FXLEffectI_ValidateParameterHandle(pEffect, Struct);
        }
    }

    FXLPARAMETERSTRUCT* pStruct;
    FXLPARAMETERENTRY* pParameterList;
    CHAR** pParameterNameMap;

    if (StructType == FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        pStruct = (FXLPARAMETERSTRUCT*)FXLEffectI_GetAnnotationEntry(pEffect, Struct);
        pParameterList = pEffect->m_pAnnotationList;
        pParameterNameMap = pEffect->m_pAnnotationNameMap;
    }
    else
    {
        pStruct = (FXLPARAMETERSTRUCT*)FXLEffectI_GetParameterEntry(pEffect, Struct);
        pParameterList = FXLEffectI_GetParameterList(pEffect, Struct);
        pParameterNameMap = (CHAR**)FXLI_SelectValue(Struct, (DWORD)pEffect->m_pParameterNameMapLocal, (DWORD)*pEffect->m_pParameterNameMapShared);
    }

    UINT Index = FXLI_GetMemberHandleIndex(pMemberName, pStruct, pParameterList, pParameterNameMap);

    if (Index < pStruct->Members)
    {
        return FXLEffect_GetMemberHandleFromIndex(pEffect, Struct, Index);
    }
    else
    {
        // The parameter was not found.

        return 0;
    }
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetMemberHandleFromIndex(
    FXLEffect* pEffect, 
    FXLHANDLE Struct, 
    UINT MemberIndex)
{
    DWORD StructType = FXLI_GetHandleIdentifier(Struct);

    if (PARAMETER_CHECK)
    {
        if (StructType == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLEffectI_ValidateAnnotationHandle(pEffect, Struct);
        }
        else
        {
            FXLEffectI_ValidateParameterHandle(pEffect, Struct);
        }
    }

    FXLPARAMETERSTRUCT* pStruct;
    FXLPARAMETERENTRY* pParameterList;

    if (StructType == FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        pStruct = (FXLPARAMETERSTRUCT*)FXLEffectI_GetAnnotationEntry(pEffect, Struct);
        pParameterList = pEffect->m_pAnnotationList;
    }
    else
    {
        pStruct = (FXLPARAMETERSTRUCT*)FXLEffectI_GetParameterEntry(pEffect, Struct);
        pParameterList = FXLEffectI_GetParameterList(pEffect, Struct);
    }

    return FXLI_GetMemberHandleFromIndex(Struct, MemberIndex, pStruct, pParameterList);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetElementHandle(
    FXLEffect* pEffect, 
    FXLHANDLE Array, 
    UINT ElementIndex)
{
    DWORD ArrayType = FXLI_GetHandleIdentifier(Array);

    if (PARAMETER_CHECK)
    {
        if (ArrayType == FXLHANDLE_ANNOTATION_IDENTIFIER)
        {
            FXLEffectI_ValidateAnnotationHandle(pEffect, Array);
        }
        else
        {
            FXLEffectI_ValidateParameterHandle(pEffect, Array);
        }
    }

    FXLPARAMETERARRAY* pArray;
    FXLPARAMETERENTRY* pParameterList;

    if (ArrayType == FXLHANDLE_ANNOTATION_IDENTIFIER)
    {
        pArray = (FXLPARAMETERARRAY*)FXLEffectI_GetAnnotationEntry(pEffect, Array);
        pParameterList = pEffect->m_pAnnotationList;
    }
    else
    {
        pArray = (FXLPARAMETERARRAY*)FXLEffectI_GetParameterEntry(pEffect, Array);
        pParameterList = FXLEffectI_GetParameterList(pEffect, Array);
    }

    return FXLI_GetElementHandle(Array, ElementIndex, pArray, pParameterList);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetTechniqueHandle(
    FXLEffect* pEffect, 
    CONST CHAR* pTechniqueName)
{
    if (PARAMETER_CHECK)
    {
        if (pTechniqueName == NULL ||
            *pTechniqueName == '\0')
        {
            FXLRIP("GetTechniqueHandle - The given technique name is NULL.\n");
        }
    }

    for (UINT TechniqueIndex = 0; TechniqueIndex < pEffect->m_TechniqueCount; TechniqueIndex++)
    {
        if (strcmp(pEffect->m_pTechniqueList[TechniqueIndex]->pName, pTechniqueName) == 0)
        {
            return FXLI_EncodeTechniqueHandle(TechniqueIndex);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetTechniqueHandleFromIndex(
    FXLEffect* pEffect, 
    UINT TechniqueIndex)
{
    if (PARAMETER_CHECK)
    {
        if (TechniqueIndex >= pEffect->m_TechniqueCount)
        {
            FXLRIP("GetTechniqueHandleFromIndex - The given technique index exceeds the number of techniques in the effect.\n");
        }
    }

    return FXLI_EncodeTechniqueHandle(TechniqueIndex);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetPassHandle(
    FXLEffect* pEffect, 
    FXLHANDLE Technique, 
    CONST CHAR* pPassName)
{
    if (PARAMETER_CHECK)
    {
        if (pPassName == NULL ||
            *pPassName == '\0')
        {
            FXLRIP("GetPassHandle - The given pass name is NULL.\n");
        }

        FXLEffectI_ValidateTechniqueHandle(pEffect, Technique);
    }

    FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Technique);

    for (UINT PassIndex = 0; PassIndex < pTechnique->PassCount; PassIndex++)
    {
        if (strcmp(pTechnique->pPassList[PassIndex]->pName, pPassName) == 0)
        {
            return FXLI_EncodePassHandle(pTechnique->pPassList[PassIndex] - pEffect->m_pPassList);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetPassHandleFromIndex(
    FXLEffect* pEffect, 
    FXLHANDLE Technique, 
    UINT PassIndex)
{
    FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Technique);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateTechniqueHandle(pEffect, Technique);

        if (PassIndex >= pTechnique->PassCount)
        {
            FXLRIP("GetPassHandleFromIndex - The given pass index exceeds the number of passes in the technique.\n");
        }
    }

    return FXLI_EncodePassHandle(pTechnique->pPassList[PassIndex] - pEffect->m_pPassList);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffectPool_GetParameterHandle(
    FXLEffectPool* pEffectPool, 
    CONST CHAR* pParameterName)
{
    if (PARAMETER_CHECK)
    {
        if (pParameterName == NULL ||
            *pParameterName == '\0')
        {
            FXLRIP("GetParameterHandle - NULL parameter name given.\n");
        }
    }

    return FXLI_GetParameterHandle(pParameterName, pEffectPool->m_pNameList, pEffectPool->m_pParameterList, pEffectPool->m_ParameterCount, 1);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffectPool_GetMemberHandle(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Struct, 
    CONST CHAR* pMemberName)
{
    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Struct);
    }

    FXLPARAMETERSTRUCT* pStruct = (FXLPARAMETERSTRUCT*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Struct);

    UINT Index = FXLI_GetMemberHandleIndex(pMemberName, pStruct, pEffectPool->m_pParameterList, pEffectPool->m_pParameterNameMap);

    if (Index < pStruct->Members)
    {
        return FXLEffectPool_GetMemberHandleFromIndex(pEffectPool, Struct, Index);
    }
    else
    {
        // The parameter was not found.

        return 0;
    }
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffectPool_GetMemberHandleFromIndex(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Struct, 
    UINT MemberIndex)
{
    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Struct);
    }

    FXLPARAMETERSTRUCT* pStruct = (FXLPARAMETERSTRUCT*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Struct);

    return FXLI_GetMemberHandleFromIndex(Struct, MemberIndex, pStruct, pEffectPool->m_pParameterList);
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffectPool_GetElementHandle(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Array, 
    UINT ElementIndex)
{
    FXLPARAMETERARRAY* pArray = (FXLPARAMETERARRAY*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Array);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Array);
    }

    return FXLI_GetElementHandle(Array, ElementIndex, pArray, pEffectPool->m_pParameterList);
}

/****************************************************************************
 *
 * Public routines to set and get parameter data.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetScalarF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    XMVECTOR DataV = XMLoadScalar(pData);

    FXLI_SetScalar(ParameterIndex, pParameter, pDataList, pDirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetScalarF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetScalar(pParameter, pDataList);

    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetScalarArrayF");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        FXLI_Set4DirtyFlags(ParameterIndex, pDirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pDataList, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetScalarArrayF");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    FLOAT* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pDataList);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetScalarI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR DataV = XMLoadScalar(pData);
    DataV = XMConvertVectorIntToFloat(DataV, 0);
    FXLI_SetScalar(ParameterIndex, pParameter, pDataList, pDirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetScalarI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetScalar(pParameter, pDataList);

    Value = XMConvertVectorFloatToInt(Value, 0);
    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarArrayI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST INT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_INT, Count, "SetScalarArrayI");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST INT* pSource = pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        DataV = XMConvertVectorIntToFloat(DataV, 0);
        FXLI_Set4DirtyFlags(ParameterIndex, pDirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pDataList, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        DataV = XMConvertVectorIntToFloat(DataV, 0);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarArrayI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    INT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_INT, Count, "GetScalarArrayI");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    INT* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pDataList);
        Value = XMConvertVectorFloatToInt(Value, 0);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetScalarB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR DataV = XMLoadScalar(pData);
    DataV = XMConvertVectorUIntToFloat(DataV, 0);
    FXLI_SetScalar(ParameterIndex, pParameter, pDataList, pDirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetScalarB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetScalar(pParameter, pDataList);

    Value = XMConvertVectorFloatToUInt(Value, 0);
    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetScalarArrayB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_BOOL, Count, "SetScalarArrayB");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST BOOL* pSource = pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        DataV = XMConvertVectorUIntToFloat(DataV, 0);
        FXLI_Set4DirtyFlags(ParameterIndex, pDirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pDataList, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pDataList, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        DataV = XMConvertVectorUIntToFloat(DataV, 0);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetScalarArrayB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    BOOL* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_BOOL, Count, "GetScalarArrayB");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    BOOL* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pDataList);
        Value = XMConvertVectorFloatToUInt(Value, 0);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetVectorF - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR Data = FXLI_LoadVectorData(pParameter, pData);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorFA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetVectorFA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR Data = XMLoadFloat4A16((XMFLOAT4A16*)pData);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetVectorF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetVectorF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetVector(pParameter, pDataList);

    FXLI_StoreVectorData(pParameter, Value, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetVectorArrayF");
        FXLI_ValidateSetVectorParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMVECTOR Data = FXLI_LoadVectorData(&pParameter->Data, pSource);
        FXLI_SetVector(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, Data);

        pSource += (pParameter->Data.Columns + 1);
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorArrayF4A(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetVectorArrayF4A");
        FXLI_ValidateSetVectorParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMVECTOR Data = XMLoadFloat4A16((XMFLOAT4A16*)pSource);
        FXLI_SetVector(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, Data);

        pSource += 4;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetVectorArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetVectorArrayF");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetVector(&pParameter->Data, pDataList);
        FXLI_StoreVectorData(&pParameter->Data, Value, pDestination);

        pDestination += (pParameter->Data.Columns + 1);
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetVectorI - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR IntData = FXLI_LoadVectorData(pParameter, pData);
    XMVECTOR Data = XMConvertVectorIntToFloat(IntData, 0);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorIA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLINTA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetVectorIA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR IntData = XMLoadFloat4A16((XMFLOAT4A16*)pData);
    XMVECTOR Data = XMConvertVectorIntToFloat(IntData, 0);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetVectorI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetVectorI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetVector(pParameter, pDataList);
    XMVECTOR IntValue = XMConvertVectorFloatToInt(Value, 0);

    FXLI_StoreVectorData(pParameter, IntValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetVectorB - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR BoolData = FXLI_LoadVectorData(pParameter, pData);
    XMVECTOR Data = XMConvertVectorUIntToFloat(BoolData, 0);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetVectorBA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLBOOLA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetVectorBA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR BoolData = XMLoadFloat4A16((XMFLOAT4A16*)pData);
    XMVECTOR Data = XMConvertVectorUIntToFloat(BoolData, 0);

    FXLI_SetVector(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetVectorB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetVectorB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMVECTOR Value = FXLI_GetVector(pParameter, pDataList);
    XMVECTOR BoolValue = XMConvertVectorFloatToUInt(Value, 0);

    FXLI_StoreVectorData(pParameter, BoolValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data;
    FXLI_LoadMatrixData(pParameter, pData, &Data);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixFA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixFA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data;
    FXLI_LoadMatrixDataA(pParameter, pData, &Data);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetMatrixF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMMATRIX Value;
    FXLI_GetMatrix(pParameter, pDataList, &Value);

    FXLI_StoreMatrixData(pParameter, Value, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixF4x4(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF4x4 - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data = XMLoadFloat4x4((XMFLOAT4X4*)pData);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixF4x4A(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF4x4A - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data = XMLoadFloat4x4A16((XMFLOAT4X4A16*)pData);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixF4x4(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter,
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetMatrixF4x4 - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMMATRIX Value;
    FXLI_GetMatrix(pParameter, pDataList, &Value);

    XMStoreFloat4x4((XMFLOAT4X4*)pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data;
        FXLI_LoadMatrixData(&pParameter->Data, pSource, &Data);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, Data);

        pSource += ((pParameter->Data.Rows + 1) * (pParameter->Data.Columns + 1));
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixArrayF4x4(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF4x4");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data = XMLoadFloat4x4((XMFLOAT4X4*)pSource);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, Data);

        pSource += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixArrayF4x4A(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF4x4A");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data = XMLoadFloat4x4A16((XMFLOAT4X4A16*)pSource);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pDataList, pDirtyFlags, Data);

        pSource += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixArrayF(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetMatrixArrayF");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMMATRIX Value;
        FXLI_GetMatrix(&pParameter->Data, pDataList, &Value);
        FXLI_StoreMatrixData(&pParameter->Data, Value, pDestination);

        pDestination += ((pParameter->Data.Rows + 1) * (pParameter->Data.Columns + 1));
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixArrayF4x4(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetMatrixArrayF4x4");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMMATRIX Value;
        FXLI_GetMatrix(&pParameter->Data, pDataList, &Value);
        XMStoreFloat4x4((XMFLOAT4X4*)pDestination, Value);

        pDestination += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetMatrixI - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX IntData;
    XMMATRIX Data;

    FXLI_LoadMatrixData(pParameter, pData, &IntData);

    Data.r[0] = XMConvertVectorIntToFloat(IntData.r[0], 0);
    Data.r[1] = XMConvertVectorIntToFloat(IntData.r[1], 0);
    Data.r[2] = XMConvertVectorIntToFloat(IntData.r[2], 0);
    Data.r[3] = XMConvertVectorIntToFloat(IntData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixIA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLINTA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetMatrixIA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX IntData;
    XMMATRIX Data;

    FXLI_LoadMatrixDataA(pParameter, pData, &IntData);

    Data.r[0] = XMConvertVectorIntToFloat(IntData.r[0], 0);
    Data.r[1] = XMConvertVectorIntToFloat(IntData.r[1], 0);
    Data.r[2] = XMConvertVectorIntToFloat(IntData.r[2], 0);
    Data.r[3] = XMConvertVectorIntToFloat(IntData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixI(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetMatrixI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMMATRIX Value;
    XMMATRIX IntValue;

    FXLI_GetMatrix(pParameter, pDataList, &Value);

    IntValue.r[0] = XMConvertVectorFloatToInt(Value.r[0], 0);
    IntValue.r[1] = XMConvertVectorFloatToInt(Value.r[1], 0);
    IntValue.r[2] = XMConvertVectorFloatToInt(Value.r[2], 0);
    IntValue.r[3] = XMConvertVectorFloatToInt(Value.r[3], 0);

    FXLI_StoreMatrixData(pParameter, IntValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetMatrixB - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX BoolData;
    XMMATRIX Data;

    FXLI_LoadMatrixData(pParameter, pData, &BoolData);

    Data.r[0] = XMConvertVectorUIntToFloat(BoolData.r[0], 0);
    Data.r[1] = XMConvertVectorUIntToFloat(BoolData.r[1], 0);
    Data.r[2] = XMConvertVectorUIntToFloat(BoolData.r[2], 0);
    Data.r[3] = XMConvertVectorUIntToFloat(BoolData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetMatrixBA(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST FXLBOOLA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetMatrixBA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX BoolData;
    XMMATRIX Data;

    FXLI_LoadMatrixDataA(pParameter, pData, &BoolData);

    Data.r[0] = XMConvertVectorUIntToFloat(BoolData.r[0], 0);
    Data.r[1] = XMConvertVectorUIntToFloat(BoolData.r[1], 0);
    Data.r[2] = XMConvertVectorUIntToFloat(BoolData.r[2], 0);
    Data.r[3] = XMConvertVectorUIntToFloat(BoolData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pDataList, pDirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetMatrixB(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetMatrixB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    XMMATRIX Value;
    XMMATRIX BoolValue;

    FXLI_GetMatrix(pParameter, pDataList, &Value);

    BoolValue.r[0] = XMConvertVectorFloatToUInt(Value.r[0], 0);
    BoolValue.r[1] = XMConvertVectorFloatToUInt(Value.r[1], 0);
    BoolValue.r[2] = XMConvertVectorFloatToUInt(Value.r[2], 0);
    BoolValue.r[3] = XMConvertVectorFloatToUInt(Value.r[3], 0);

    FXLI_StoreMatrixData(pParameter, BoolValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_SetSampler(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    CONST IDirect3DBaseTexture9* pTexture)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);
    UINT64* pDirtyFlags = FXLEffectI_GetDirtyFlags(pEffect, Parameter);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetSampler(ParameterIndex, pParameter, pDataList, pDirtyFlags, pTexture);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetSampler(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    IDirect3DBaseTexture9** pTexture)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);

        FXLASSERT(pTexture != NULL);
    }

    XMFLOAT4A16* pDataList = FXLEffectI_GetParameterData(pEffect, Parameter);

    *pTexture = FXLI_GetSampler(pParameter, pDataList);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetScalarF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);
    XMVECTOR DataV = XMLoadScalar(pData);

    FXLI_SetScalar(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetScalarF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetScalar(pParameter, pEffectPool->m_pParameterData);

    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetScalarArrayF");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        FXLI_Set4DirtyFlags(ParameterIndex, pEffectPool->m_DirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pEffectPool->m_pParameterData, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetScalarArrayF");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    FLOAT* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pEffectPool->m_pParameterData);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetScalarI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR DataV = XMLoadScalar(pData);
    DataV = XMConvertVectorIntToFloat(DataV, 0);
    FXLI_SetScalar(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetScalarI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetScalar(pParameter, pEffectPool->m_pParameterData);

    Value = XMConvertVectorFloatToInt(Value, 0);
    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarArrayI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST INT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_INT, Count, "SetScalarArrayI");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST INT* pSource = pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        DataV = XMConvertVectorIntToFloat(DataV, 0);
        FXLI_Set4DirtyFlags(ParameterIndex, pEffectPool->m_DirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pEffectPool->m_pParameterData, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        DataV = XMConvertVectorIntToFloat(DataV, 0);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarArrayI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    INT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_INT, Count, "GetScalarArrayI");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    INT* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pEffectPool->m_pParameterData);
        Value = XMConvertVectorFloatToInt(Value, 0);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetScalarB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMVECTOR DataV = XMLoadScalar(pData);
    DataV = XMConvertVectorUIntToFloat(DataV, 0);
    FXLI_SetScalar(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetScalarB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetScalar(pParameter, pEffectPool->m_pParameterData);

    Value = XMConvertVectorFloatToUInt(Value, 0);
    XMStoreScalar(pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetScalarArrayB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_BOOL, Count, "SetScalarArrayB");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST BOOL* pSource = pData;
    XMVECTOR DataV;

    for (ElementCount += ParameterIndex; (ParameterIndex + 3) < ElementCount; ParameterIndex += 4)
    {
        DataV = XMLoadVector4(pSource);
        DataV = XMConvertVectorUIntToFloat(DataV, 0);
        FXLI_Set4DirtyFlags(ParameterIndex, pEffectPool->m_DirtyFlags);
        FXLI_SetScalarNoDirty(&pParameter[1].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[2].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[3].Data, pEffectPool->m_pParameterData, DataV);
        DataV = XMVectorRotateLeft(DataV, 1);
        FXLI_SetScalarNoDirty(&pParameter[4].Data, pEffectPool->m_pParameterData, DataV);
        pParameter += 4;
        pSource += 4;
    }
    for (; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;
        DataV = XMLoadScalar(pSource);
        DataV = XMConvertVectorUIntToFloat(DataV, 0);
        FXLI_SetScalar(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, DataV);
        ++pSource;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetScalarArrayB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    BOOL* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_BOOL, Count, "GetScalarArrayB");
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    BOOL* pDestination = pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetScalar(&pParameter->Data, pEffectPool->m_pParameterData);
        Value = XMConvertVectorFloatToUInt(Value, 0);
        XMStoreScalar(pDestination, Value);

        ++pDestination;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetVectorF - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR Data = FXLI_LoadVectorData(pParameter, pData);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorFA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetVectorFA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR Data = XMLoadFloat4A16((XMFLOAT4A16*)pData);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetVectorF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetVectorF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetVector(pParameter, pEffectPool->m_pParameterData);

    FXLI_StoreVectorData(pParameter, Value, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetVectorArrayF");
        FXLI_ValidateSetVectorParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMVECTOR Data = FXLI_LoadVectorData(&pParameter->Data, pSource);
        FXLI_SetVector(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);

        pSource += (pParameter->Data.Columns + 1);
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorArrayF4A(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetVectorArrayF4A");
        FXLI_ValidateSetVectorParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMVECTOR Data = XMLoadFloat4A16((XMFLOAT4A16*)pSource);
        FXLI_SetVector(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);

        pSource += 4;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetVectorArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetVectorArrayF");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMVECTOR Value = FXLI_GetVector(&pParameter->Data, pEffectPool->m_pParameterData);
        FXLI_StoreVectorData(&pParameter->Data, Value, pDestination);

        pDestination += (pParameter->Data.Columns + 1);
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetVectorI - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR IntData = FXLI_LoadVectorData(pParameter, pData);
    XMVECTOR Data = XMConvertVectorIntToFloat(IntData, 0);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorIA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLINTA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetVectorIA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR IntData = XMLoadFloat4A16((XMFLOAT4A16*)pData);
    XMVECTOR Data = XMConvertVectorIntToFloat(IntData, 0);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetVectorI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetVectorI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetVector(pParameter, pEffectPool->m_pParameterData);
    XMVECTOR IntValue = XMConvertVectorFloatToInt(Value, 0);

    FXLI_StoreVectorData(pParameter, IntValue, pData);
}


//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetVectorB - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR BoolData = FXLI_LoadVectorData(pParameter, pData);
    XMVECTOR Data = XMConvertVectorUIntToFloat(BoolData, 0);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetVectorBA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLBOOLA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetVectorBA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetVectorParameter(pParameter);
    }

    XMVECTOR BoolData = XMLoadFloat4A16((XMFLOAT4A16*)pData);
    XMVECTOR Data = XMConvertVectorUIntToFloat(BoolData, 0);
    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetVector(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetVectorB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetVectorB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMVECTOR Value = FXLI_GetVector(pParameter, pEffectPool->m_pParameterData);
    XMVECTOR BoolValue = XMConvertVectorFloatToUInt(Value, 0);

    FXLI_StoreVectorData(pParameter, BoolValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data;
    FXLI_LoadMatrixData(pParameter, pData, &Data);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixFA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixFA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data;
    FXLI_LoadMatrixDataA(pParameter, pData, &Data);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetMatrixF - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMMATRIX Value;
    FXLI_GetMatrix(pParameter, pEffectPool->m_pParameterData, &Value);

    FXLI_StoreMatrixData(pParameter, Value, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixF4x4(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("GetMatrixF4x4 - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMMATRIX Value;
    FXLI_GetMatrix(pParameter, pEffectPool->m_pParameterData, &Value);

    XMStoreFloat4x4((XMFLOAT4X4*)pData, Value);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixF4x4(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF4x4 - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data = XMLoadFloat4x4((XMFLOAT4X4*)pData);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixF4x4A(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_FLOAT)
        {
            FXLRIP("SetMatrixF4x4A - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX Data = XMLoadFloat4x4A16((XMFLOAT4X4A16*)pData);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data;
        FXLI_LoadMatrixData(&pParameter->Data, pSource, &Data);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);

        pSource += ((pParameter->Data.Rows + 1) * (pParameter->Data.Columns + 1));
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixArrayF4x4(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF4x4");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data = XMLoadFloat4x4((XMFLOAT4X4*)pSource);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);

        pSource += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixArrayF4x4A(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLFLOATA* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "SetMatrixArrayF4x4A");
        FXLI_ValidateSetMatrixParameter(&pParameter[1].Data);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    UINT ParameterIndex = FXLI_GetParameterIndex(Parameter);
    CONST DWORD* pSource = (CONST DWORD*)pData;

    for (ElementCount += ParameterIndex; ParameterIndex < ElementCount; ++ParameterIndex)
    {
        ++pParameter;

        XMMATRIX Data = XMLoadFloat4x4A16((XMFLOAT4X4A16*)pSource);
        FXLI_SetMatrix(ParameterIndex, &pParameter->Data, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);

        pSource += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixArrayF(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetMatrixArrayF");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMMATRIX Value;
        FXLI_GetMatrix(&pParameter->Data, pEffectPool->m_pParameterData, &Value);
        FXLI_StoreMatrixData(&pParameter->Data, Value, pDestination);

        pDestination += ((pParameter->Data.Rows + 1) * (pParameter->Data.Columns + 1));
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixArrayF4x4(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FLOAT* pData,
    UINT Count)
{
    FXLPARAMETERENTRY* pParameter = FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLI_ValidateParameterDataArray(pParameter, FXLDTYPEI_FLOAT, Count, "GetMatrixArrayF4x4");
        FXLASSERT(pData != NULL);
    }

    UINT ElementCount = FXLI_SelectArrayCount(Count, pParameter->Array.Elements);
    DWORD* pDestination = (DWORD*)pData;

    for (UINT EntryIndex = 0; EntryIndex < ElementCount; ++EntryIndex)
    {
        ++pParameter;

        XMMATRIX Value;
        FXLI_GetMatrix(&pParameter->Data, pEffectPool->m_pParameterData, &Value);
        XMStoreFloat4x4((XMFLOAT4X4*)pDestination, Value);

        pDestination += 16;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetMatrixI - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX IntData;
    XMMATRIX Data;

    FXLI_LoadMatrixData(pParameter, pData, &IntData);

    Data.r[0] = XMConvertVectorIntToFloat(IntData.r[0], 0);
    Data.r[1] = XMConvertVectorIntToFloat(IntData.r[1], 0);
    Data.r[2] = XMConvertVectorIntToFloat(IntData.r[2], 0);
    Data.r[3] = XMConvertVectorIntToFloat(IntData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixIA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLINTA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("SetMatrixIA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX IntData;
    XMMATRIX Data;

    FXLI_LoadMatrixDataA(pParameter, pData, &IntData);

    Data.r[0] = XMConvertVectorIntToFloat(IntData.r[0], 0);
    Data.r[1] = XMConvertVectorIntToFloat(IntData.r[1], 0);
    Data.r[2] = XMConvertVectorIntToFloat(IntData.r[2], 0);
    Data.r[3] = XMConvertVectorIntToFloat(IntData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixI(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    INT* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_INT)
        {
            FXLRIP("GetMatrixI - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMMATRIX Value;
    XMMATRIX IntValue;

    FXLI_GetMatrix(pParameter, pEffectPool->m_pParameterData, &Value);

    IntValue.r[0] = XMConvertVectorFloatToInt(Value.r[0], 0);
    IntValue.r[1] = XMConvertVectorFloatToInt(Value.r[1], 0);
    IntValue.r[2] = XMConvertVectorFloatToInt(Value.r[2], 0);
    IntValue.r[3] = XMConvertVectorFloatToInt(Value.r[3], 0);

    FXLI_StoreMatrixData(pParameter, IntValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetMatrixB - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX BoolData;
    XMMATRIX Data;

    FXLI_LoadMatrixData(pParameter, pData, &BoolData);

    Data.r[0] = XMConvertVectorUIntToFloat(BoolData.r[0], 0);
    Data.r[1] = XMConvertVectorUIntToFloat(BoolData.r[1], 0);
    Data.r[2] = XMConvertVectorUIntToFloat(BoolData.r[2], 0);
    Data.r[3] = XMConvertVectorUIntToFloat(BoolData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetMatrixBA(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST FXLBOOLA* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("SetMatrixBA - Invalid parameter type %d.\n", pParameter->Type);
        }

        FXLI_ValidateSetMatrixParameter(pParameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    XMMATRIX BoolData;
    XMMATRIX Data;

    FXLI_LoadMatrixDataA(pParameter, pData, &BoolData);

    Data.r[0] = XMConvertVectorUIntToFloat(BoolData.r[0], 0);
    Data.r[1] = XMConvertVectorUIntToFloat(BoolData.r[1], 0);
    Data.r[2] = XMConvertVectorUIntToFloat(BoolData.r[2], 0);
    Data.r[3] = XMConvertVectorUIntToFloat(BoolData.r[3], 0);

    FXLI_SetMatrix(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, Data);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetMatrixB(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    BOOL* pData)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pData != NULL);

        if (pParameter->Type != FXLDTYPEI_BOOL)
        {
            FXLRIP("GetMatrixB - Invalid parameter type %d.\n", pParameter->Type);
        }
    }

    XMMATRIX Value;
    XMMATRIX BoolValue;

    FXLI_GetMatrix(pParameter, pEffectPool->m_pParameterData, &Value);

    BoolValue.r[0] = XMConvertVectorFloatToUInt(Value.r[0], 0);
    BoolValue.r[1] = XMConvertVectorFloatToUInt(Value.r[1], 0);
    BoolValue.r[2] = XMConvertVectorFloatToUInt(Value.r[2], 0);
    BoolValue.r[3] = XMConvertVectorFloatToUInt(Value.r[3], 0);

    FXLI_StoreMatrixData(pParameter, BoolValue, pData);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_SetSampler(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    CONST IDirect3DBaseTexture9* pTexture)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
    }

    DWORD ParameterIndex = FXLI_GetParameterIndex(Parameter);

    FXLI_SetSampler(ParameterIndex, pParameter, pEffectPool->m_pParameterData, pEffectPool->m_DirtyFlags, pTexture);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetSampler(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    IDirect3DBaseTexture9** pTexture)
{
    FXLPARAMETERDATA* pParameter = (FXLPARAMETERDATA*)FXLEffectPoolI_GetParameterEntry(pEffectPool, Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);

        FXLASSERT(pTexture != NULL);
    }

    *pTexture = FXLI_GetSampler(pParameter, pEffectPool->m_pParameterData);
}

/****************************************************************************
 *
 * Public description routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetEffectDesc(
    FXLEffect* pEffect, 
    FXLEFFECT_DESC* pDesc)
{
    FXLASSERT(pDesc);

    pDesc->pCreator = pEffect->m_pCreatorName;
    pDesc->Parameters = pEffect->m_ParameterHandleIndexMapSize;
    pDesc->Techniques = pEffect->m_TechniqueCount;
    pDesc->Functions = pEffect->m_ShaderStateCount;
    pDesc->Annotations = pEffect->m_AnnotationListGlobals;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetTechniqueDesc(
    FXLEffect* pEffect, 
    FXLHANDLE Technique, 
    FXLTECHNIQUE_DESC* pDesc)
{
    FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Technique);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateTechniqueHandle(pEffect, Technique);
        FXLASSERT(pDesc != NULL);
    }

    pDesc->pName = pTechnique->pName;
    pDesc->Passes = pTechnique->PassCount;
    pDesc->Annotations = pTechnique->AnnotationCount;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetPassDesc(
    FXLEffect* pEffect, 
    FXLHANDLE Pass, 
    FXLPASS_DESC* pDesc)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);
        FXLASSERT(pDesc != NULL);
    }

    pDesc->pName = pPass->pName;
    pDesc->Annotations = pPass->AnnotationCount;
    pDesc->RenderStates = pPass->pRenderStates->RenderStateCount;
    pDesc->SamplerStates = pPass->pSamplerStates->SamplerStateCount;
    pDesc->pVertexShaderFunction = (pPass->pShaderStates->pVertexShader->FunctionSize > 0) ?
                                    pPass->pShaderStates->pVertexShader->pFunction : NULL;
    pDesc->VertexShaderFunctionSize = pPass->pShaderStates->pVertexShader->FunctionSize;
    pDesc->pPixelShaderFunction = (pPass->pShaderStates->pPixelShader->FunctionSize > 0) ?
                                    pPass->pShaderStates->pPixelShader->pFunction : NULL;
    pDesc->PixelShaderFunctionSize = pPass->pShaderStates->pPixelShader->FunctionSize;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetParameterDesc(
    FXLEffect* pEffect, 
    FXLHANDLE Parameter, 
    FXLPARAMETER_DESC* pDesc)
{
    FXLPARAMETERENTRY* pParameterList = FXLEffectI_GetParameterList(pEffect, Parameter);
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    FXLPARAMETERENTRY* pParameter = pParameterList + Offset;

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateParameterHandle(pEffect, Parameter);
        FXLASSERT(pDesc != NULL);
    }

    CHAR** pNameMap = FXLEffectI_GetNameMap(pEffect, Parameter);

    FXLI_InitializeParameterDesc(Parameter, pParameter, pNameMap, Offset, pDesc);

    if (pEffect->m_AnnotationListEntries > 0)
    {
        pDesc->Annotations = FXLI_IsParameterShared(Parameter) ? pEffect->m_pAnnotationCountListShared[Offset] : pParameter->Annotations;
    }
    else
    {
        pDesc->Annotations = 0;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetAnnotationDesc(
    FXLEffect* pEffect, 
    FXLHANDLE Annotation, 
    FXLANNOTATION_DESC* pDesc)
{
    FXLPARAMETERENTRY* pAnnotation = FXLEffectI_GetAnnotationEntry(pEffect, Annotation);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateAnnotationHandle(pEffect, Annotation);
        FXLASSERT(pDesc != NULL);
    }

    pDesc->pName = pEffect->m_pAnnotationNameMap[Annotation >> FXLHANDLE_ANNOTATION_OFFSET_SHIFT];
    pDesc->Content = (FXLPA_CONTENT)pAnnotation->Content;

    if (pAnnotation->Content == FXLPACONTENT_DATA)
    {
        if (pAnnotation->Data.Type == FXLDTYPEI_STRING)
        {
            pDesc->Type = FXLDTYPE_STRING;
            pDesc->Class = FXLDCLASS_SCALAR;

            CHAR* pString = (CHAR*)(pEffect->m_pAnnotationData + pAnnotation->Data.DataOffset);
            pDesc->Size = strlen(pString) + 1;
        }
        else
        {
            pDesc->Type = (FXLDATA_TYPE)pAnnotation->Data.Type;
            pDesc->Class = (FXLDATA_CLASS)pAnnotation->Data.Class;
            pDesc->Size = pAnnotation->Size << 2;
        }
        pDesc->Rows = pAnnotation->Data.Rows + 1;
        pDesc->Columns = pAnnotation->Data.Columns + 1;
        pDesc->Elements = 0;
    }
    else
    {
        pDesc->Type = FXLDTYPE_CONTAINER;
        pDesc->Class = FXLDCLASS_CONTAINER;
        pDesc->Rows = 1;
        pDesc->Columns = 1;
        pDesc->Elements = pAnnotation->Struct.Members; // Same bit-field location as pAnnotation->Array.Elements.
        pDesc->Size = pAnnotation->Size << 2;
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffectPool_GetParameterDesc(
    FXLEffectPool* pEffectPool, 
    FXLHANDLE Parameter, 
    FXLPARAMETER_DESC* pDesc)
{
    DWORD Offset = FXLI_GetParameterOffset(Parameter);
    FXLPARAMETERENTRY* pParameter = pEffectPool->m_pParameterList + Offset;

    if (PARAMETER_CHECK)
    {
        FXLEffectPoolI_ValidateParameterHandle(pEffectPool, Parameter);
        FXLASSERT(pDesc != NULL);
    }

    FXLI_InitializeParameterDesc(Parameter, pParameter, pEffectPool->m_pParameterNameMap, Offset, pDesc);

    pDesc->Annotations = 0;
}

/****************************************************************************
 *
 * Public query routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetActiveTechnique(
    FXLEffect* pEffect)
{
    return pEffect->m_ActiveTechniqueHandle;
}

//------------------------------------------------------------------------------

FXLINLINE FXLHANDLE WINAPI FXLEffect_GetActivePass(
    FXLEffect* pEffect)
{
    return (pEffect->m_pActivePass != NULL) ? FXLI_EncodePassHandle(pEffect->m_pActivePass - pEffect->m_pPassList) : 0;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetRenderStateList(
    FXLEffect* pEffect, 
    FXLHANDLE Pass, 
    FXLRENDERSTATE* pList, 
    UINT* pCount)
{
    UINT RenderStateCount;
    UINT RenderStateIndex;
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        FXLASSERT(pCount != NULL);
    }

    RenderStateCount = *pCount;
    *pCount = 0;

    if (pPass->pRenderStates != NULL)
    {
        for (RenderStateIndex = 0; RenderStateIndex < pPass->pRenderStates->RenderStateCount; RenderStateIndex++)
        {
            if ((pList != NULL) &&
                (*pCount < RenderStateCount))
            {
                pList[*pCount].Type = (D3DRENDERSTATETYPE)pPass->pRenderStates->pRenderStateList[RenderStateIndex].Type;
                pList[*pCount].Value = pPass->pRenderStates->pRenderStateList[RenderStateIndex].Value;
            }

            (*pCount)++;
        }
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetSamplerStateList(
    FXLEffect* pEffect, 
    FXLHANDLE Sampler, 
    FXLSAMPLERSTATE* pList, 
    UINT* pCount)
{
    UINT SamplerOffset;
    UINT SamplerStateCount;
    UINT64** pUsageFlags;
    FXLPASSPARAMETERREF* pRef;
    FXLPASSPARAMETERREF* pRefEnd;
    UINT64 UpdateFlags;
    UINT SkipCount;
    UINT CheckCount;
    FXLPARAMETERDATA* pParameterData;
    UINT ParameterBitGroups;
    UINT EntryIndex;
    UINT RegisterIndex;
    UINT PassIndex;
    UINT StateIndex;
    UINT Scope;
    UINT Group;
    UINT SamplerStateIndex;

    SamplerOffset = FXLI_GetParameterOffset(Sampler);

    if (PARAMETER_CHECK)
    {
        FXLPARAMETERENTRY* pParameterList;
        FXLPARAMETERENTRY* pParameter;

        FXLEffectI_ValidateParameterHandle(pEffect, Sampler);
        FXLASSERT(pCount != NULL);

        pParameterList = FXLEffectI_GetParameterList(pEffect, Sampler);
        pParameter = pParameterList + SamplerOffset;
        if (pParameter->Data.Type != FXLDTYPEI_SAMPLER)
        {
            FXLRIP("GetSamplerStateList - The given parameter is not a sampler.\n");
        }
    }

    SamplerStateCount = *pCount;
    *pCount = 0;

    // Find a reference to the sampler in a shader of a pass to obtain any sampler state settings.

    for (PassIndex = 0; PassIndex < pEffect->m_PassCount; PassIndex++)
    {
        if (pEffect->m_pPassList[PassIndex].pShaderStates->pVertexShader->FunctionSize > 0 ||
            pEffect->m_pPassList[PassIndex].pShaderStates->pPixelShader->FunctionSize > 0)
        {
            pUsageFlags = &pEffect->m_pPassList[PassIndex].pShaderStates->pVSFUsageFlagsLocal;

            for (StateIndex = 6; StateIndex < 8; StateIndex++)
            {
                ParameterBitGroups = pEffect->m_ParameterBitGroupsLocal;

                for (Scope = 0; Scope < 2; Scope++)
                {
                    for (Group = 0; Group < ParameterBitGroups; Group++)
                    {
                        pRef = (Scope == 0) ? pEffect->m_pPassList[PassIndex].pShaderStates->pReferenceListLocal : 
                                              pEffect->m_pPassList[PassIndex].pShaderStates->pReferenceListShared;
                        pRef += (Group << 6);

                        UpdateFlags = pUsageFlags[StateIndex + Scope * 8][Group];

                        while (UpdateFlags != 0)
                        {
                            SkipCount = FXLI_CountLeadingZeros64(UpdateFlags);
                            UpdateFlags <<= SkipCount;
                            pRef += SkipCount;

                            CheckCount = FXLI_CountLeadingZeros64(~UpdateFlags);
                            UpdateFlags <<= CheckCount;
                            pRefEnd = pRef + CheckCount;

                            do
                            {
                                pParameterData = (FXLPARAMETERDATA*)FXLEffectI_GetParameterEntry(pEffect, pRef->ParameterHandle);
                                EntryIndex = (FXLPARAMETERENTRY*)pParameterData - FXLEffectI_GetParameterList(pEffect, pRef->ParameterHandle);
                                RegisterIndex = (StateIndex == 6) ? pRef->VSSRegisterIndex : pRef->PSSRegisterIndex;

                                if (EntryIndex == SamplerOffset)
                                {
                                    if (pEffect->m_pPassList[PassIndex].pSamplerStates != NULL &&
                                        pEffect->m_pPassList[PassIndex].pSamplerStates->SamplerStateCount > 0)
                                    {
                                        for (SamplerStateIndex = 0; SamplerStateIndex < pEffect->m_pPassList[PassIndex].pSamplerStates->SamplerStateCount; SamplerStateIndex++)
                                        {
                                            if (pEffect->m_pPassList[PassIndex].pSamplerStates->pSamplerStateList[SamplerStateIndex].Index == RegisterIndex)
                                            {
                                                if ((pList != NULL) &&
                                                    (*pCount < SamplerStateCount))
                                                {
                                                    pList[*pCount].Type = (D3DSAMPLERSTATETYPE)pEffect->m_pPassList[PassIndex].pSamplerStates->pSamplerStateList[SamplerStateIndex].Type;
                                                    pList[*pCount].Value = pEffect->m_pPassList[PassIndex].pSamplerStates->pSamplerStateList[SamplerStateIndex].Value;
                                                }

                                                (*pCount)++;
                                            }
                                        }
                                    }

                                    return;
                                }
                                                                                                                             
                                ++pRef;                                                                                      
                                                                                                                             
                            } while (pRef < pRefEnd);                                                                        
                        }                                                                                                    
                    }

                    ParameterBitGroups = pEffect->m_ParameterBitGroupsShared;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetRenderState(
    FXLEffect* pEffect,
    FXLHANDLE Pass, 
    UINT StateIndex, 
    D3DRENDERSTATETYPE* pStateType, 
    DWORD* pStateValue)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        FXLASSERT(pStateType != NULL);
        FXLASSERT(pStateValue != NULL);

        if (pPass->pRenderStates == NULL)
        {
            FXLRIP("FXL: GetRenderState - The given pass does not contain any render states.\n");
        }

        if (StateIndex >= pPass->pRenderStates->RenderStateCount)
        {
            FXLRIP("FXL: GetRenderState - The given state index exceeds the number of render states contained in the given pass.\n");
        }
    }

    FXLRENDERSTATEDATA* pData = &pPass->pRenderStates->pRenderStateList[StateIndex];

    *pStateType = (D3DRENDERSTATETYPE)pData->Type;
    *pStateValue = pData->Value;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetSamplerState(
    FXLEffect* pEffect,
    FXLHANDLE Pass, 
    UINT StateIndex, 
    UINT* pSamplerIndex,
    D3DSAMPLERSTATETYPE* pStateType, 
    DWORD* pStateValue)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        FXLASSERT(pStateType != NULL);
        FXLASSERT(pStateValue != NULL);

        if (pPass->pSamplerStates == NULL)
        {
            FXLRIP("FXL: GetSamplerState - The given pass does not contain any sampler states.\n");
        }

        if (StateIndex >= pPass->pSamplerStates->SamplerStateCount)
        {
            FXLRIP("FXL: GetSamplerState - The given state index exceeds the number of sampler states contained in the given pass.\n");
        }
    }

    FXLSAMPLERSTATEDATA* pData = &pPass->pSamplerStates->pSamplerStateList[StateIndex];

    *pSamplerIndex = pData->Index;
    *pStateType = (D3DSAMPLERSTATETYPE)pData->Type;
    *pStateValue = pData->Value;
}

//------------------------------------------------------------------------------

FXLINLINE FXLPARAMETER_CONTEXT WINAPI FXLEffect_GetParameterContext(
    FXLEffect* pEffect, 
    FXLHANDLE Pass, 
    FXLHANDLE Parameter)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);
    DWORD Index = FXLI_GetParameterIndex(Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        FXLPARAMETERENTRY* pEntry = FXLEffectI_GetParameterEntry(pEffect, Parameter);
        if (pEntry->Content != FXLPACONTENT_DATA)
        {
            FXLRIP("GetParameterContext - Context information cannot be given for a struct or array parameter.\n");
        }
    }

    FXLSHADERSTATEENTRY* pState = pPass->pShaderStates;
    UINT64** pUsageFlags = (&pState->pVSFUsageFlagsLocal) + ((Parameter & 1) << 3);
    UINT Group = (Index >> 6);
    UINT Shift = 63 - (Index & 63);

    return (FXLPARAMETER_CONTEXT)((((pUsageFlags[0][Group] >> Shift) & 1) << 0) |
                                  (((pUsageFlags[1][Group] >> Shift) & 1) << 1) |
                                  (((pUsageFlags[2][Group] >> Shift) & 1) << 2) |
                                  (((pUsageFlags[3][Group] >> Shift) & 1) << 3) |
                                  (((pUsageFlags[4][Group] >> Shift) & 1) << 4) |
                                  (((pUsageFlags[5][Group] >> Shift) & 1) << 5) |
                                  (((pUsageFlags[6][Group] >> Shift) & 1) << 6) |
                                  (((pUsageFlags[7][Group] >> Shift) & 1) << 7));
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_GetParameterRegister(
    FXLEffect* pEffect, 
    FXLHANDLE Pass, 
    FXLHANDLE Parameter, 
    FXLPARAMETER_CONTEXT Context, 
    UINT* pRegisterIndex, 
    UINT* pRegisterCount)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);
    DWORD Index = FXLI_GetParameterIndex(Parameter);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        FXLASSERT(pRegisterIndex != NULL);
        FXLASSERT(pRegisterCount != NULL);

        FXLPARAMETERENTRY* pEntry = FXLEffectI_GetParameterEntry(pEffect, Parameter);
        if (pEntry->Content != FXLPACONTENT_DATA)
        {
            FXLRIP("GetParameterRegister - Register information cannot be given for a struct or array parameter.\n");
        }

        if ((((DWORD)Context - 1) & (DWORD)Context) != 0)
        {
            FXLRIP("GetParameterRegister - A single context must be specified to obtain specific register information.\n");
        }

        FXLPARAMETER_CONTEXT TotalContext = FXLEffect_GetParameterContext(pEffect, Pass, Parameter);
        if ((TotalContext & Context) == 0)
        {
            FXLRIP("GetParameterRegister - The given parameter does not possess the given context for the given pass.\n");
        }
    }

    FXLSHADERSTATEENTRY* pState = pPass->pShaderStates;
    FXLPASSPARAMETERREF* pRefList = (FXLPASSPARAMETERREF*)FXLI_SelectValue(Parameter, (DWORD)pState->pReferenceListLocal, (DWORD)pState->pReferenceListShared);
    FXLPASSPARAMETERREF* pRef = pRefList + Index;

    UINT ContextIndex = FXLI_Log2(Context);

    *pRegisterIndex = FXLI_GetPassParameterRefIndex(pRef, ContextIndex);
    *pRegisterCount = FXLI_GetPassParameterRefCount(pRef, ContextIndex);
}

/****************************************************************************
 *
 * Public technique and pass routines.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_BeginTechnique(
    FXLEffect* pEffect, 
    FXLHANDLE Technique, 
    DWORD Flags)
{
    FXLTECHNIQUEENTRY* pTechnique = FXLEffectI_GetTechnique(pEffect, Technique);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidateTechniqueHandle(pEffect, Technique);

        if (pEffect->m_pActiveTechnique != NULL)
        {
            FXLRIP("BeginTechnique - The nesting of techniques is unsupported.\n");
        }

        if (pEffect->m_pDevice == NULL)
        {
            FXLRIP("BeginTechnique - A Direct3DDevice object must be set for the effect "
                   "through FXLCreateEffect or FXLEffect_ChangeDevice before a technique may be begun.\n");
        }
    }

    FXLI_BeginTechnique(pEffect, pTechnique, Flags);

    pEffect->m_ActiveTechniqueHandle = Technique;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_BeginTechniqueFromIndex(
    FXLEffect* pEffect, 
    UINT TechniqueIndex, 
    DWORD Flags)
{
    if (PARAMETER_CHECK)
    {
        if (pEffect->m_pActiveTechnique != NULL)
        {
            FXLRIP("BeginTechniqueFromIndex - The nesting of techniques is unsupported.\n");
        }

        if (pEffect->m_pDevice == NULL)
        {
            FXLRIP("BeginTechniqueFromIndex - A Direct3DDevice object must be set for the effect "
                   "through FXLCreateEffect or FXLEffect_ChangeDevice before a technique may be begun.\n");
        }

        if (TechniqueIndex >= pEffect->m_TechniqueCount)
        {
            FXLRIP("BeginTechniqueFromIndex - The given technique index exceeds the number of techniques in the effect.\n");
        }
    }

    FXLI_BeginTechnique(pEffect, pEffect->m_pTechniqueList[TechniqueIndex], Flags);

    pEffect->m_ActiveTechniqueHandle = FXLI_EncodeTechniqueHandle(TechniqueIndex);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_EndTechnique(
    FXLEffect* pEffect)
{
    if (PARAMETER_CHECK)
    {
        if (pEffect->m_pActiveTechnique == NULL)
        {
            FXLRIP("EndTechnique - A technique cannot be ended before it has begun.\n");
        }
    }

    if (pEffect->m_TechniqueFlags & FXL_RESTORE_DEFAULT_STATE)
    {
        IDirect3DDevice9* pDevice = pEffect->m_pDevice;

        if (pEffect->m_TechniqueFlags & FXL_RESTORE_DEFAULT_RENDER_STATE)
        {
            // Restore all render states altered by the passes used in the technique to D3D defaults.

            UINT Group;
            UINT64 RestoreFlags;
            DWORD SkipCount;
            DWORD SetCount;
            DWORD StateIndex;
            DWORD StateLimit;

            for (Group = 0; Group < 2; Group++)
            {
                StateIndex = Group << 6;

                RestoreFlags = pEffect->m_RenderStateDirtyFlags[Group];

                while (RestoreFlags != 0)
                {
                    SkipCount = FXLI_CountLeadingZeros64(RestoreFlags);
                    RestoreFlags <<= SkipCount;
                    StateIndex += SkipCount;

                    SetCount = FXLI_CountLeadingZeros64(~RestoreFlags);
                    RestoreFlags <<= SetCount;
                    StateLimit = StateIndex + SetCount;

                    do
                    {
                        pDevice->SetRenderState((D3DRENDERSTATETYPE)g_FXLIDefaultRenderStateList[StateIndex].Type, 
                                                    g_FXLIDefaultRenderStateList[StateIndex].Value);
                        StateIndex++;
                    } while (StateIndex != StateLimit);
                }
            }
        }

        if (pEffect->m_TechniqueFlags & FXL_RESTORE_DEFAULT_SAMPLER_STATE)
        {
            // Restore all sampler states altered by the passes used in the technique to D3D defaults.

            UINT Group;
            UINT64 RestoreFlags;
            DWORD SkipCount;
            DWORD SetCount;
            DWORD StateIndex;
            DWORD StateLimit;
            DWORD SamplerIndex;
            DWORD TypeIndex;

            for (Group = 0; Group < 32; Group += 2)
            {
                StateIndex = 0;

                RestoreFlags = pEffect->m_SamplerStateDirtyFlags[Group >> 1];

                while (RestoreFlags != 0)
                {
                    SkipCount = FXLI_CountLeadingZeros64(RestoreFlags);
                    RestoreFlags <<= SkipCount;
                    StateIndex += SkipCount;

                    SetCount = FXLI_CountLeadingZeros64(~RestoreFlags);
                    RestoreFlags <<= SetCount;
                    StateLimit = StateIndex + SetCount;

                    do
                    {
                        SamplerIndex = Group + (StateIndex >> 5);
                        TypeIndex = StateIndex & 31;

                        pDevice->SetSamplerState(SamplerIndex,
                                                    (D3DSAMPLERSTATETYPE)g_FXLIDefaultSamplerStateList[TypeIndex].Type, 
                                                    g_FXLIDefaultSamplerStateList[TypeIndex].Value);
                        StateIndex++;
                    } while (StateIndex != StateLimit);
                }
            }
        }
    }

    pEffect->m_pActiveTechnique = NULL;
    pEffect->m_ActiveTechniqueHandle = 0;
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_BeginPass(
    FXLEffect* pEffect, 
    FXLHANDLE Pass)
{
    FXLPASSENTRY* pPass = FXLEffectI_GetPass(pEffect, Pass);

    if (PARAMETER_CHECK)
    {
        FXLEffectI_ValidatePassHandle(pEffect, Pass);

        if (pEffect->m_pActiveTechnique == NULL)
        {
            FXLRIP("BeginPass - A technique must be started before beginning a pass.\n");
        }

        if (pEffect->m_pActivePass != NULL)
        {
            FXLRIP("BeginPass - The nesting of passes is unsupported.\n");
        }

        UINT PassIndex;
        for (PassIndex = 0; PassIndex < pEffect->m_pActiveTechnique->PassCount; PassIndex++)
        {
            if (pPass == pEffect->m_pActiveTechnique->pPassList[PassIndex])
            {
                break;
            }
        }
        if (PassIndex == pEffect->m_pActiveTechnique->PassCount)
        {
            FXLRIP("BeginPass - The given pass does not belong to the currently active technique.\n");
        }
    }

    FXLI_BeginPass(pEffect, pPass);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_BeginPassFromIndex(
    FXLEffect* pEffect, 
    UINT PassIndex)
{
    if (PARAMETER_CHECK)
    {
        if (pEffect->m_pActiveTechnique == NULL)
        {
            FXLRIP("BeginPassFromIndex - A technique must be started before beginning a pass.\n");
        }

        if (pEffect->m_pActivePass != NULL)
        {
            FXLRIP("BeginPassFromIndex - The nesting of passes is unsupported.\n");
        }

        if (PassIndex >= pEffect->m_pActiveTechnique->PassCount)
        {
            FXLRIP("BeginPassFromIndex - The given index exceeds the number of passes in the active technique.\n");
        }
    }

    FXLI_BeginPass(pEffect, pEffect->m_pActiveTechnique->pPassList[PassIndex]);
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLEffect_EndPass(
    FXLEffect* pEffect)
{
    if (PARAMETER_CHECK)
    {
        if (pEffect->m_pActivePass == NULL)
        {
            FXLRIP("EndPass - A pass cannot be ended before it has begun.\n");
        }
    }

    pEffect->m_pPreviousPass = pEffect->m_pActivePass;
    pEffect->m_pActivePass = NULL;
}

/****************************************************************************
 *
 * Filtered commit routine.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// General Commit operations will update shader constant registers with parameter
// values (i.e. "send them to the device") under the following conditions:
//
// - Parameters will only be sent to the device if they are referenced by either
//   the vertex shader or pixel shader in the current pass.
//
// - All referenced local parameters will be sent to the device (even if
//   they have not been modified) on the first Commit after a BeginPass,
//   unless a matching vertex and pixel shader were used on the previous
//   pass.  After the first Commit (or when the shaders do match), only local 
//   parameters which have been modified will be sent to the device.
//
// - All shared parameters will be sent to the device using the same conditions 
//   as local parameters unless the shared parameters are in a state of 
//   correlation.
//
// - If the effects system is in a state of shared parameter correlation, no 
//   shared parameter will be sent to the device unless it has been
//   modified in value (even on the first Commit in a pass).  Once modified, 
//   a shared parameter will be sent to the device on the next Commit, even 
//   if it is not referenced by either of the shaders in the associated pass.
//
// These general conditions for parameter register updates by Commit are further 
// qualified for the CommitU routine to take into account the register update mode
// of individual parameters.  CommitU will only send a parameter to the device
// if the parameter is set to use automatic register updates (the default).
// A parameter using manual register updates will be disregarded by CommitU
// and must be explicitly set through D3DDevice SetShaderConstant operations.

FXLINLINE VOID WINAPI FXLEffect_CommitU(
    FXLEffect* pEffect)
{
    XMVECTOR DirtyFlags[2];
    XMVECTOR UpdateFlags[2];
    UINT Group;

    for (Group = 0; Group < pEffect->m_ParameterBitGroupsLocal; Group += 4)
    {
        DirtyFlags[0]  = XMLoadVector4A(&pEffect->m_DirtyFlagsLocal[Group]);
        UpdateFlags[0] = XMLoadVector4A(&pEffect->m_UpdateFlagsLocal[Group]);
        DirtyFlags[1]  = XMLoadVector4A(&pEffect->m_DirtyFlagsLocal[Group + 2]);
        UpdateFlags[1] = XMLoadVector4A(&pEffect->m_UpdateFlagsLocal[Group + 2]);

        DirtyFlags[0] = XMVectorAndInt(DirtyFlags[0], UpdateFlags[0]);
        DirtyFlags[1] = XMVectorAndInt(DirtyFlags[1], UpdateFlags[1]);

        XMStoreVector4A(&pEffect->m_DirtyFlagsLocal[Group], DirtyFlags[0]);
        XMStoreVector4A(&pEffect->m_DirtyFlagsLocal[Group + 2], DirtyFlags[1]);
    }

    for (Group = 0; Group < pEffect->m_ParameterBitGroupsShared; Group += 4)
    {
        DirtyFlags[0]  = XMLoadVector4A(&pEffect->m_pDirtyFlagsShared[Group]);
        UpdateFlags[0] = XMLoadVector4A(&pEffect->m_pUpdateFlagsShared[Group]);
        DirtyFlags[1]  = XMLoadVector4A(&pEffect->m_pDirtyFlagsShared[Group + 2]);
        UpdateFlags[1] = XMLoadVector4A(&pEffect->m_pUpdateFlagsShared[Group + 2]);

        DirtyFlags[0] = XMVectorAndInt(DirtyFlags[0], UpdateFlags[0]);
        DirtyFlags[1] = XMVectorAndInt(DirtyFlags[1], UpdateFlags[1]);

        XMStoreVector4A(&pEffect->m_pDirtyFlagsShared[Group], DirtyFlags[0]);
        XMStoreVector4A(&pEffect->m_pDirtyFlagsShared[Group + 2], DirtyFlags[1]);
    }

    FXLEffect_Commit(pEffect);
}

/****************************************************************************
 *
 * Public default state management functions.
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLSetDefaultRenderState(
    D3DRENDERSTATETYPE StateType, 
    DWORD StateValue)
{
    UINT StateIndex;

    for (StateIndex = 0; StateIndex < FXL_DEFAULT_RENDER_STATE_ENTRIES; StateIndex++)
    {
        if (g_FXLIDefaultRenderStateList[StateIndex].Type == (DWORD)StateType)
        {
            g_FXLIDefaultRenderStateList[StateIndex].Value = StateValue;
            return;
        }
    }

    FXLRIP("FXLSetDefaultRenderState - The given state type was not found in the default render state list.\n");
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLGetDefaultRenderState(
    D3DRENDERSTATETYPE StateType, 
    DWORD* pStateValue)
{
    UINT StateIndex;

    FXLASSERT(pStateValue != NULL);

    for (StateIndex = 0; StateIndex < FXL_DEFAULT_RENDER_STATE_ENTRIES; StateIndex++)
    {
        if (g_FXLIDefaultRenderStateList[StateIndex].Type == (DWORD)StateType)
        {
            *pStateValue = g_FXLIDefaultRenderStateList[StateIndex].Value;
            return;
        }
    }

    FXLRIP("FXLGetDefaultRenderState - The given state type was not found in the default render state list.\n");
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLSetDefaultSamplerState(
    D3DSAMPLERSTATETYPE StateType, 
    DWORD StateValue)
{
    UINT StateIndex;

    for (StateIndex = 0; StateIndex < FXL_DEFAULT_SAMPLER_STATE_ENTRIES; StateIndex++)
    {
        if (g_FXLIDefaultSamplerStateList[StateIndex].Type == (DWORD)StateType)
        {
            g_FXLIDefaultSamplerStateList[StateIndex].Value = StateValue;
            return;
        }
    }

    FXLRIP("FXLSetDefaultSamplerState - The given state type was not found in the default sampler state list.\n");
}

//------------------------------------------------------------------------------

FXLINLINE VOID WINAPI FXLGetDefaultSamplerState(
    D3DSAMPLERSTATETYPE StateType, 
    DWORD* pStateValue)
{
    UINT StateIndex;

    FXLASSERT(pStateValue != NULL);

    for (StateIndex = 0; StateIndex < FXL_DEFAULT_SAMPLER_STATE_ENTRIES; StateIndex++)
    {
        if (g_FXLIDefaultSamplerStateList[StateIndex].Type == (DWORD)StateType)
        {
            *pStateValue = g_FXLIDefaultSamplerStateList[StateIndex].Value;
            return;
        }
    }

    FXLRIP("FXLGetDefaultSamplerState - The given state type was not found in the default sampler state list.\n");
}

#ifdef __cplusplus
}; // extern "C"
#endif

#endif // __FXL_INL__
