/*++

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    XMADecoder.h

Abstract:

    XMA decoder public interfaces, functions and data types

--*/

#ifndef _XMADECODER_H_
#define _XMADECODER_H_

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#   pragma once
#endif // #if defined (_MSC_VER) && (_MSC_VER >= 1020)

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include <xtl.h>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

typedef struct IXMAContext IXMAContext;
typedef IXMAContext *LPXMACONTEXT;
typedef IXMAContext *PXMACONTEXT;

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

static const DWORD XMADECODER_SAMPLE_SIZE_IN_BYTES                  = 2;
static const DWORD XMADECODER_SUBFRAME_SIZE_IN_SAMPLES              = 128;
static const DWORD XMADECODER_SUBFRAME_SIZE_IN_BYTES                = XMADECODER_SUBFRAME_SIZE_IN_SAMPLES * XMADECODER_SAMPLE_SIZE_IN_BYTES;
static const DWORD XMADECODER_INPUT_BUFFER_ALIGNMENT                = 2048;
static const DWORD XMADECODER_MAX_INPUT_BUFFER_SIZE_IN_BLOCKS       = 0xfff;
static const DWORD XMADECODER_MAX_INPUT_BUFFER_SIZE_IN_BYTES        = XMADECODER_MAX_INPUT_BUFFER_SIZE_IN_BLOCKS * XMADECODER_INPUT_BUFFER_ALIGNMENT;
static const DWORD XMADECODER_MAX_OUTPUT_BUFFER_SIZE_IN_SUBFRAMES   = 0x1f;
static const DWORD XMADECODER_MAX_OUTPUT_BUFFER_SIZE_IN_BYTES       = XMADECODER_MAX_OUTPUT_BUFFER_SIZE_IN_SUBFRAMES * XMADECODER_SUBFRAME_SIZE_IN_BYTES;

//------------------------------------------------------------------------------
// XMALOOPDATA
//------------------------------------------------------------------------------

typedef struct XMALOOPDATA
{
    DWORD   LoopStart;          // [0, 67108863] Offset in bits
    DWORD   LoopEnd;            // [0, 67108863] Offset in bits
    BYTE    LoopCount;          // [0, 254], 255 = Infinite loop
    BYTE    LoopSubframeEnd;    // Subframe number of loop end point, inclusive
    BYTE    LoopSubframeSkip;   // [1, 4]: Number of sub-frames to skip before decoding and outputting data at the loop start point

} XMALOOPDATA, *LPXMALOOPDATA, *PXMALOOPDATA;
typedef const XMALOOPDATA *LPCXMALOOPDATA;

//------------------------------------------------------------------------------
// XMACONTEXTINIT
//------------------------------------------------------------------------------

typedef struct XMACONTEXTINIT
{
    VOID*       pInputBuffer0;
    DWORD       InputBuffer0Size;
    VOID*       pInputBuffer1;
    DWORD       InputBuffer1Size;
    DWORD       InputBufferReadOffset;

    VOID*       pOutputBuffer;
    DWORD       OutputBufferSize;

    VOID*       pWorkBuffer;

    DWORD       NumSubframesToDecode;

    DWORD       NumChannels;
    DWORD       DecoderTable;

    XMALOOPDATA LoopData;

} XMACONTEXTINIT, *LPXMACONTEXTINIT, *PXMACONTEXTINIT;
typedef const XMACONTEXTINIT *LPCXMACONTEXTINIT;

//------------------------------------------------------------------------------
// XMA Decoder Functions
//------------------------------------------------------------------------------

STDAPI XMACreateContext(PXMACONTEXT* ppContext);
STDAPI_(VOID) XMAInitializeContext(PXMACONTEXT pContext, PXMACONTEXTINIT pInit);
STDAPI_(VOID) XMAReleaseContext(PXMACONTEXT pContext);
STDAPI_(VOID) XMAEnableContext(PXMACONTEXT pContext);
STDAPI XMADisableContext(PXMACONTEXT pContext, BOOL BlockIfBusy);
STDAPI_(DWORD) XMAGetOutputBufferWriteOffset(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetOutputBufferReadOffset(PXMACONTEXT pContext, DWORD NumSubframes);
STDAPI_(DWORD) XMAGetOutputBufferReadOffset(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetOutputBufferValid(PXMACONTEXT pContext);
STDAPI_(BOOL) XMAIsOutputBufferValid(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetInputBuffer0Valid(PXMACONTEXT pContext);
STDAPI_(BOOL) XMAIsInputBuffer0Valid(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetInputBuffer1Valid(PXMACONTEXT pContext);
STDAPI_(BOOL) XMAIsInputBuffer1Valid(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetInputBuffer0(PXMACONTEXT pContext, VOID* pBuffer, DWORD NumBlocks);
STDAPI_(VOID) XMASetInputBuffer1(PXMACONTEXT pContext, VOID* pBuffer, DWORD NumBlocks);
STDAPI_(DWORD) XMAGetPacketMetadata(PXMACONTEXT pContext);
STDAPI_(VOID) XMABlockWhileInUse(PXMACONTEXT pContext);
STDAPI_(VOID) XMASetLoopData(PXMACONTEXT pContext, LPCXMALOOPDATA pLoopData);
STDAPI_(VOID) XMASetInputBufferReadOffset(PXMACONTEXT pContext, DWORD OffsetInBits);
STDAPI_(DWORD) XMAGetInputBufferReadOffset(PXMACONTEXT pContext);

#endif // #ifndef _XMADECODER_H_
