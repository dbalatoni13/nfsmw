/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d9gpu.h
 *  Content:    Direct3D hardware register definitions include file
 *
 ****************************************************************************/

#ifndef _D3D9GPU_H_
#define _D3D9GPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma warning(push)

// Disable nameless struct/union and zero-sized array warnings for this header.

#pragma warning(disable:4201 4200)

// The PPC back-end of the C compiler by default defines bitfields to be
// ordered from the MSB to the LSB, which is opposite the convention on
// the X86 platform.  Use the 'bitfield_order' pragma to switch the
// ordering.  Note that this does not affect endianness in any way.

#if defined(_M_PPCBE)
#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
#endif

// Total amount of EDRAM memory, in bytes:

#define GPU_EDRAM_SIZE                              (10*1024*1024)

// Total amount of system memory addressable by GPU, in bytes:

#define GPU_MEMORY_SIZE                             (512*1024*1024)

// The GPU's core clock is 500 MHz:

#define GPU_CLOCK_SPEED                             (500*1000*1000)

// AND this value with any physical memory virtual address in order to
// convert to an address that can be used by the GPU:

#define GPU_ADDRESS_MASK                            (GPU_MEMORY_SIZE - 1)

// The CPU's virtual addresses for 'physical memory' allocations are divided
// into three sections:
//
// 0xA0000000 - 0xBFFFFFF 64 KB Pages
// 0xC0000000 - 0xDFFFFFF 16 MB Pages (Address range also used for cached-read-only pages, see below)
// 0xE0000000 - 0xFFFFFFF  4 KB Pages

#define GPU_CPU_VIRTUAL_ADDRESS_64KB_START          ((DWORD) 0xA0000000)
#define GPU_CPU_VIRTUAL_ADDRESS_64KB_END            ((DWORD) 0xBFFFFFFF)
#define GPU_CPU_VIRTUAL_ADDRESS_16MB_START          ((DWORD) 0xC0000000)
#define GPU_CPU_VIRTUAL_ADDRESS_16MB_END            ((DWORD) 0xDFFFFFFF)
#define GPU_CPU_VIRTUAL_ADDRESS_4KB_START           ((DWORD) 0xDFFFF000)
#define GPU_CPU_VIRTUAL_ADDRESS_4KB_END             ((DWORD) 0xFFFFFFFF)

#define GPU_CPU_VIRTUAL_ADDRESS                     ((DWORD) 0xA0000000)

// The 64KB and 4 KB physical memory pages can be accessed as
// cached, read-only memory by offseting their addresses from their
// native range to the corresponding 0xC0000000 to 0xDFFFFFFF range.
// 16 MB pages are always write-combined. They don't have a corresponding
// cached, read-only representation.

#define GPU_CPU_CACHED_READONLY_VIRTUAL_ADDRESS     ((DWORD) 0xC0000000)

// There are sixteen 32-bit 'tail write-back' registers on the CPU.  The
// GPU's command processor can write directly to these registers.  They're
// beneficial over GPU write-backs to normal cacheable memory because they
// reduce the latency, which is particularly important for XPS: the CPU
// doesn't have to go all the way to memory to read a new write-back value.
// There is one register per 128 bytes at the following physical address
// range.  The remainder of the range is unused.

#define GPU_TAIL_WRITEBACKS                         16
#define GPU_TAIL_WRITEBACK_DELTA                    128

#define GPU_TAIL_WRITEBACK_RANGE_VIRTUAL_ADDRESS    ((DWORD) 0x7F000000)
#define GPU_TAIL_WRITEBACK_RANGE_PHYSICAL_ADDRESS   ((DWORD) 0xC0000000)
#define GPU_TAIL_WRITEBACK_RANGE_SIZE               0x00100000

// There is a particular physical memory range where the GPU knows to request
// memory from the CPU's L2 instead of from physical memory.  This is used
// solely by XPS.  The L2 set has to be 'locked' and each cache-line in the
// set has to be pre-initialized using 'dcbz128' to a virtual addresses
// which corresponds to a consecutive subset of this physical memory range.
// (In other words, unless the cache is pre-populated, GPU requests from this
// memory range will be lost.)

#define GPU_XPS_LOCKED_L2_RANGE_VIRTUAL_ADDRESS     ((DWORD) 0x7F100000)
#define GPU_XPS_LOCKED_L2_RANGE_PHYSICAL_ADDRESS    ((DWORD) 0xC0100000)
#define GPU_XPS_LOCKED_L2_RANGE_SIZE                0x07F00000

// Convert a CPU address into a GPU usable address.  Only 'physical memory'
// (as allocated by XPhysicalAlloc or XMemAlloc(XALLOC_MEMTYPE_PHYSICAL)) can
// be converted in this way.

static __forceinline DWORD GPU_CONVERT_CPU_TO_GPU_ADDRESS(CONST void* CpuAddress)
{
    return (((DWORD) (UINT_PTR) CpuAddress) & GPU_ADDRESS_MASK) +
        (DWORD)(((((UINT_PTR) CpuAddress) >> 20) + 0x200) & 0x1000);
}

// Convert a GPU address into a CPU usable address.  The resulting address
// is usable only if the memory was originally allocated as 'physical
// memory' by XPhysicalAlloc or XMemAlloc(XALLOC_MEMTYPE_PHYSICAL).  The
// memory will be accessed by the CPU using the memory-protection type (e.g.,
// cached or write-combining) of the original XPhysicalAlloc or XMemAlloc
// allocation.

static __forceinline void* GPU_CONVERT_GPU_TO_CPU_ADDRESS_64KB(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress + GPU_CPU_VIRTUAL_ADDRESS_64KB_START);
}

static __forceinline void* GPU_CONVERT_GPU_TO_CPU_ADDRESS_16MB(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress + GPU_CPU_VIRTUAL_ADDRESS_16MB_START);
}

static __forceinline void* GPU_CONVERT_GPU_TO_CPU_ADDRESS_4KB(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress + GPU_CPU_VIRTUAL_ADDRESS_4KB_START);
}

// Convert a GPU address into a CPU usable address.  All of memory can be
// accessed regardless of how the memory was originally allocated.  The
// memory will be accessed by the CPU as cached and readonly.  The hardware
// has no automatic cache coherency between write-combining and cached views
// so care must be taken to avoid stale data (e.g., if the CPU does a cached
// read of this memory, then modifies it using write-combining, then does a
// cached read again, the second read may get invalid stale cached data
// unless CPU cache control instructions are used).

static __forceinline void* GPU_CONVERT_GPU_TO_CPU_CACHED_READONLY_ADDRESS(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress + GPU_CPU_CACHED_READONLY_VIRTUAL_ADDRESS);
}

// Convert a CPU address into a CPU cached, readonly address.  Only 'physical
// memory' (as allocated by XPhysicalAlloc or XMemAlloc(XALLOC_MEMTYPE_PHYSICAL))
// can be converted in this way.

static __forceinline void* GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS(CONST void* CpuAddress)
{
    return GPU_CONVERT_GPU_TO_CPU_CACHED_READONLY_ADDRESS(GPU_CONVERT_CPU_TO_GPU_ADDRESS(CpuAddress));
}

// Convert a CPU address pointing into the XPS locked L2 cache into a GPU
// physical address.

static __forceinline DWORD GPU_CONVERT_XPS_CPU_TO_GPU_ADDRESS(CONST void* CpuAddress)
{
    return ((DWORD) (UINT_PTR) CpuAddress)
            - GPU_XPS_LOCKED_L2_RANGE_VIRTUAL_ADDRESS
            + GPU_XPS_LOCKED_L2_RANGE_PHYSICAL_ADDRESS;
}

// Convert a GPU address pointing into the XPS locked L2 cache into a CPU
// virtual address.

static __forceinline void* GPU_CONVERT_XPS_GPU_TO_CPU_ADDRESS(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress
            - GPU_XPS_LOCKED_L2_RANGE_PHYSICAL_ADDRESS
            + GPU_XPS_LOCKED_L2_RANGE_VIRTUAL_ADDRESS);
}

// Total size of the shader program store, in instructions:

#define GPU_INSTRUCTIONS                            4096

// Total number of constants support by a single context:

#define GPU_ALU_CONSTANTS                           512
#define GPU_BOOLEAN_CONSTANTS                       256
#define GPU_INTEGER_CONSTANTS                       32
#define GPU_FLOW_CONSTANTS                          (GPU_BOOLEAN_CONSTANTS/32 + GPU_INTEGER_CONSTANTS) // Actual register count
#define GPU_FETCH_CONSTANTS                         32
#define GPU_VERTEX_FETCH_CONSTANTS                  (3*GPU_FETCH_CONSTANTS)

// How D3D divides up the constants:

#define GPU_D3D_VERTEX_CONSTANTF_BASE               0
#define GPU_D3D_VERTEX_CONSTANTF_COUNT              256
#define GPU_D3D_VERTEX_CONSTANTI_BASE               0
#define GPU_D3D_VERTEX_CONSTANTI_COUNT              16
#define GPU_D3D_VERTEX_CONSTANTB_BASE               0
#define GPU_D3D_VERTEX_CONSTANTB_COUNT              128
#define GPU_D3D_VERTEX_FETCH_CONSTANT_BASE          26
#define GPU_D3D_VERTEX_FETCH_CONSTANT_COUNT         6 // Good for 18 streams
#define GPU_D3D_VERTEX_TEXTURE_FETCH_CONSTANT_BASE  16
#define GPU_D3D_VERTEX_TEXTURE_FETCH_CONSTANT_COUNT 10

#define GPU_D3D_PIXEL_CONSTANTF_BASE                GPU_D3D_VERTEX_CONSTANTF_COUNT
#define GPU_D3D_PIXEL_CONSTANTF_COUNT               256
#define GPU_D3D_PIXEL_CONSTANTI_BASE                GPU_D3D_VERTEX_CONSTANTI_COUNT
#define GPU_D3D_PIXEL_CONSTANTI_COUNT               16
#define GPU_D3D_PIXEL_CONSTANTB_BASE                GPU_D3D_VERTEX_CONSTANTB_COUNT
#define GPU_D3D_PIXEL_CONSTANTB_COUNT               128
#define GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_BASE   0
#define GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_COUNT  26

// Total texture fetch constants tracked by D3D runtime
// for both vertex and pixel shaders:

#define GPU_D3D_TEXTURE_FETCH_CONSTANT_BASE         0
#define GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT        26

// Fetch constant allocation strategy:
// pixel shader samplers start at texture constant 0, and proceed upwards.
// vertex shader samplers start at texture constant 16 and proceed upwards.
// The constant D3DDMAPSAMPLER is set to 16. Typically vertex shaders will use
// textures D3DDMAPSAMPLER..D3DDMAPSAMPLER+3, while pixel shaders will use
// textures 0..15. But it's up to the shader authors to sort things out.

//  0..15 : samplers 0..15 for pixel shaders
// 16..19 : samplers 0..3 for vertex shaders
//        : also samplers 16..19 for pixel shaders
// 20..25 : samplers 20..25 for pixel shaders
//          also samplers 4..9 for vertex shaders
//          also vertex streams 18..35
// 26..31 : Vertex stream fetch constants 0..17(31.High is stream 0, 31.Middle is stream 1, and so on.)

#define GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(X) ((X) + GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_BASE)
#define GPU_CONVERT_D3D_TO_HARDWARE_VERTEXFETCHCONSTANT(X) ((GPU_VERTEX_FETCH_CONSTANTS - 1) - (X))

// GPU Temporary register count (that can be used by our runtime)

#define GPU_SHADER_TEMPORARY_REGISTER_COUNT         32

// GPU Temporary register count (that the hardware supports)

#define GPU_SHADER_TEMPORARY_REGISTER_COUNT_PHYSICAL 64

// 2D textures have a maximum dimension of 8K by 8K:

#define GPU_MAX_TEXTURE_DIMENSION                   8192

// Volume textures can have up to 1024 layers in the Z direction:

#define GPU_MAX_TEXTURE_DEPTH                       1024

// Textures have to have 4K alignment both for the base and mip levels:

#define GPU_TEXTURE_ALIGNMENT                       4096

// Textures have to have a pitch that is a multiple of 32 texels:

#define GPU_TEXTURE_TEXEL_PITCH_ALIGNMENT           32

// Linear textures have to have a pitch that is a multiple of 256 bytes:

#define GPU_LINEAR_TEXTURE_PITCH_BYTE_ALIGNMENT 256

// Texture tiles are 32x32x4 texels:

#define GPU_TEXTURE_TILE_DIMENSION                  32

#define GPU_TEXTURE_TILE_DEPTH_DIMENSION            4

// Resolve rectangles must always be 8x8 pixel aligned:

#define GPU_RESOLVE_ALIGNMENT                       8

// Maximum vertex fetch stride in dwords:

#define GPU_MAX_VERTEX_STRIDE                       255

// There's 16 interpolators for the pixel shader:

#define GPU_INTERPOLATORS                           16

// There is a pool of 128 sets of GPRs:

#define GPU_GPRS                                    128

// There's 4 channels per interpolator:

#define GPU_INTERPOLATOR_CHANNELS                   4

// Size of the post-transform cache.  VtxReuseDepth should always be
// set to this value:

#define GPU_VERTEX_REUSE_DEPTH                      14

// Distance (in indices) which the vertex vector slot assignment leads the
// deallocation. DeallocDist should always be set to this value:

#define GPU_DEALLOC_DIST                            16

// EDRAM tiles are allocated in units of 80x16 pixels at 1X multisampling,
// 80x8 at 2X and 40x8 at 4X:

#define GPU_EDRAM_TILE_WIDTH_1X                     80
#define GPU_EDRAM_TILE_HEIGHT_1X                    16

#define GPU_EDRAM_TILE_WIDTH_2X                     80
#define GPU_EDRAM_TILE_HEIGHT_2X                    8

#define GPU_EDRAM_TILE_WIDTH_4X                     40
#define GPU_EDRAM_TILE_HEIGHT_4X                    8

// EDRAM tile size in fragments instead of pixels:

#define GPU_EDRAM_TILE_WIDTH_IN_FRAGMENTS           GPU_EDRAM_TILE_WIDTH_1X
#define GPU_EDRAM_TILE_HEIGHT_IN_FRAGMENTS          GPU_EDRAM_TILE_HEIGHT_1X

// An EDRAM tile size, in bytes.  Note that 64-bit surfaces have an
// allocation granularity of twice this (10240 bytes) but can have a 5120
// byte start alignment:

#define GPU_EDRAM_TILE_SIZE                         5120

// Total number of usable EDRAM tiles:

#define GPU_EDRAM_TILES                             (GPU_EDRAM_SIZE / GPU_EDRAM_TILE_SIZE)

// Hierarchical Z tiles are allocated in units of 32x16 at 1X multisampilng,
// 32x8 at 2X and 16x8 at 4X:

#define GPU_HIERARCHICAL_Z_TILE_WIDTH_1X            32
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_1X           16

#define GPU_HIERARCHICAL_Z_TILE_WIDTH_2X            32
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_2X           8

#define GPU_HIERARCHICAL_Z_TILE_WIDTH_4X            16
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_4X           8

// Hierarchical Z tile size in fragments instead of pixels:

#define GPU_HIERARCHICAL_Z_TILE_WIDTH_IN_FRAGMENTS  GPU_HIERARCHICAL_Z_TILE_WIDTH_1X
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_IN_FRAGMENTS GPU_HIERARCHICAL_Z_TILE_HEIGHT_1X

// A hierarchical Z tile size, in fragments:

#define GPU_HIERARCHICAL_Z_TILE_SIZE                512

// Total number of usable hierarchical Z tiles.  This is enough for 1280x720x2X:

#define GPU_HIERARCHICAL_Z_TILES                    3600

// Resource address shift amounts for storage in header fields:

#define GPU_VERTEXBUFFER_ADDRESS_SHIFT              2
#define GPU_VERTEXBUFFER_SIZE_SHIFT                 2
#define GPU_TEXTURE_ADDRESS_SHIFT                   12

// GPU callable command buffers require a 32 byte alignment:

#define GPU_COMMAND_BUFFER_ALIGNMENT                32

// Masks and shifts for GPU control flow exec instruction
// TypeAndSerialize field

#define GPUEXEC_TYPE_SHIFT                          0
#define GPUEXEC_TYPE_MASK                           1 // Set if Fetch instruction
#define GPUEXEC_SERIALIZE_SHIFT                     1
#define GPUEXEC_SERIALIZE_MASK                      2 // Set if serialized

// Maximum number of ALU/Fetch instructions in an Exec:

#define GPUFLOW_MAX_EXEC_COUNT                      6

// Size of the guard band:

#define GPU_GUARDBAND                               8192

// Helper for calculating guard band factors for a given number of pixels
// and a given scale. (Scale is 1/2 the height or width of the screen.):

#define GPU_GUARDBANDFACTOR(GUARDBAND_PIXELS, SCALE) (1.0f + ((float) GUARDBAND_PIXELS) / ((float) SCALE))

// Maximum size of a sprite:

#define GPU_MAX_POINT_SIZE                         256.0f

//------------------------------------------------------------------------------

typedef enum
{
    GPUENDIAN_NONE                                  = 0,
    GPUENDIAN_8IN16                                 = 1,
    GPUENDIAN_8IN32                                 = 2,
    GPUENDIAN_16IN32                                = 3,
} GPUENDIAN;

typedef enum
{
    GPUENDIAN128_NONE                               = 0,
    GPUENDIAN128_8IN16                              = 1,
    GPUENDIAN128_8IN32                              = 2,
    GPUENDIAN128_16IN32                             = 3,
    GPUENDIAN128_8IN64                              = 4,
    GPUENDIAN128_8IN128                             = 5,
} GPUENDIAN128;

typedef enum
{
    GPUCOLORARRAY_2D_COLOR                          = 0,
    GPUCOLORARRAY_3D_SLICE_COLOR                    = 1,
} GPUCOLORARRAY;

typedef enum
{
    GPUDEPTHARRAY_2D_ALT_DEPTH                      = 0,
    GPUDEPTHARRAY_2D_DEPTH                          = 1,
} GPUDEPTHARRAY;

typedef enum
{
    GPUCOLORFORMAT_8                                = 2,
    GPUCOLORFORMAT_1_5_5_5                          = 3,
    GPUCOLORFORMAT_5_6_5                            = 4,
    GPUCOLORFORMAT_6_5_5                            = 5,
    GPUCOLORFORMAT_8_8_8_8                          = 6,
    GPUCOLORFORMAT_2_10_10_10                       = 7,
    GPUCOLORFORMAT_8_A                              = 8,
    GPUCOLORFORMAT_8_B                              = 9,
    GPUCOLORFORMAT_8_8                              = 10,
    GPUCOLORFORMAT_8_8_8_8_A                        = 14,
    GPUCOLORFORMAT_4_4_4_4                          = 15,
    GPUCOLORFORMAT_10_11_11                         = 16,
    GPUCOLORFORMAT_11_11_10                         = 17,
    GPUCOLORFORMAT_16                               = 24,
    GPUCOLORFORMAT_16_16                            = 25,
    GPUCOLORFORMAT_16_16_16_16                      = 26,
    GPUCOLORFORMAT_16_FLOAT                         = 30,
    GPUCOLORFORMAT_16_16_FLOAT                      = 31,
    GPUCOLORFORMAT_16_16_16_16_FLOAT                = 32,
    GPUCOLORFORMAT_32_FLOAT                         = 36,
    GPUCOLORFORMAT_32_32_FLOAT                      = 37,
    GPUCOLORFORMAT_32_32_32_32_FLOAT                = 38,
    GPUCOLORFORMAT_2_10_10_10_FLOAT                 = 62, // EDRAM render target only
} GPUCOLORFORMAT;

typedef enum
{
    GPUTEXTUREFORMAT_1_REVERSE                      = 0,
    GPUTEXTUREFORMAT_1                              = 1,
    GPUTEXTUREFORMAT_8                              = 2,
    GPUTEXTUREFORMAT_1_5_5_5                        = 3,
    GPUTEXTUREFORMAT_5_6_5                          = 4,
    GPUTEXTUREFORMAT_6_5_5                          = 5,
    GPUTEXTUREFORMAT_8_8_8_8                        = 6,
    GPUTEXTUREFORMAT_2_10_10_10                     = 7,
    GPUTEXTUREFORMAT_8_A                            = 8,
    GPUTEXTUREFORMAT_8_B                            = 9,
    GPUTEXTUREFORMAT_8_8                            = 10,
    GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP                = 11,
    GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP                = 12,
    GPUTEXTUREFORMAT_16_16_EDRAM                    = 13, // EDRAM render target only
    GPUTEXTUREFORMAT_8_8_8_8_A                      = 14,
    GPUTEXTUREFORMAT_4_4_4_4                        = 15,
    GPUTEXTUREFORMAT_10_11_11                       = 16,
    GPUTEXTUREFORMAT_11_11_10                       = 17,
    GPUTEXTUREFORMAT_DXT1                           = 18,
    GPUTEXTUREFORMAT_DXT2_3                         = 19,
    GPUTEXTUREFORMAT_DXT4_5                         = 20,
    GPUTEXTUREFORMAT_16_16_16_16_EDRAM              = 21, // EDRAM render target only
    GPUTEXTUREFORMAT_24_8                           = 22,
    GPUTEXTUREFORMAT_24_8_FLOAT                     = 23,
    GPUTEXTUREFORMAT_16                             = 24,
    GPUTEXTUREFORMAT_16_16                          = 25,
    GPUTEXTUREFORMAT_16_16_16_16                    = 26,
    GPUTEXTUREFORMAT_16_EXPAND                      = 27,
    GPUTEXTUREFORMAT_16_16_EXPAND                   = 28,
    GPUTEXTUREFORMAT_16_16_16_16_EXPAND             = 29,
    GPUTEXTUREFORMAT_16_FLOAT                       = 30,
    GPUTEXTUREFORMAT_16_16_FLOAT                    = 31,
    GPUTEXTUREFORMAT_16_16_16_16_FLOAT              = 32,
    GPUTEXTUREFORMAT_32                             = 33,
    GPUTEXTUREFORMAT_32_32                          = 34,
    GPUTEXTUREFORMAT_32_32_32_32                    = 35,
    GPUTEXTUREFORMAT_32_FLOAT                       = 36,
    GPUTEXTUREFORMAT_32_32_FLOAT                    = 37,
    GPUTEXTUREFORMAT_32_32_32_32_FLOAT              = 38,
    GPUTEXTUREFORMAT_32_AS_8                        = 39,
    GPUTEXTUREFORMAT_32_AS_8_8                      = 40,
    GPUTEXTUREFORMAT_16_MPEG                        = 41,
    GPUTEXTUREFORMAT_16_16_MPEG                     = 42,
    GPUTEXTUREFORMAT_8_INTERLACED                   = 43,
    GPUTEXTUREFORMAT_32_AS_8_INTERLACED             = 44,
    GPUTEXTUREFORMAT_32_AS_8_8_INTERLACED           = 45,
    GPUTEXTUREFORMAT_16_INTERLACED                  = 46,
    GPUTEXTUREFORMAT_16_MPEG_INTERLACED             = 47,
    GPUTEXTUREFORMAT_16_16_MPEG_INTERLACED          = 48,
    GPUTEXTUREFORMAT_DXN                            = 49,
    GPUTEXTUREFORMAT_8_8_8_8_AS_16_16_16_16         = 50,
    GPUTEXTUREFORMAT_DXT1_AS_16_16_16_16            = 51,
    GPUTEXTUREFORMAT_DXT2_3_AS_16_16_16_16          = 52,
    GPUTEXTUREFORMAT_DXT4_5_AS_16_16_16_16          = 53,
    GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16      = 54,
    GPUTEXTUREFORMAT_10_11_11_AS_16_16_16_16        = 55,
    GPUTEXTUREFORMAT_11_11_10_AS_16_16_16_16        = 56,
    GPUTEXTUREFORMAT_32_32_32_FLOAT                 = 57,
    GPUTEXTUREFORMAT_DXT3A                          = 58,
    GPUTEXTUREFORMAT_DXT5A                          = 59,
    GPUTEXTUREFORMAT_CTX1                           = 60,
    GPUTEXTUREFORMAT_DXT3A_AS_1_1_1_1               = 61,
    GPUTEXTUREFORMAT_8_8_8_8_GAMMA_EDRAM            = 62, // EDRAM render target only
    GPUTEXTUREFORMAT_2_10_10_10_FLOAT_EDRAM         = 63, // EDRAM render target only
} GPUTEXTUREFORMAT;

typedef enum
{
    GPUVERTEXFORMAT_8_8_8_8                         = 6,
    GPUVERTEXFORMAT_2_10_10_10                      = 7,
    GPUVERTEXFORMAT_10_11_11                        = 16,
    GPUVERTEXFORMAT_11_11_10                        = 17,
    GPUVERTEXFORMAT_16_16                           = 25,
    GPUVERTEXFORMAT_16_16_16_16                     = 26,
    GPUVERTEXFORMAT_16_16_FLOAT                     = 31,
    GPUVERTEXFORMAT_16_16_16_16_FLOAT               = 32,
    GPUVERTEXFORMAT_32                              = 33,
    GPUVERTEXFORMAT_32_32                           = 34,
    GPUVERTEXFORMAT_32_32_32_32                     = 35,
    GPUVERTEXFORMAT_32_FLOAT                        = 36,
    GPUVERTEXFORMAT_32_32_FLOAT                     = 37,
    GPUVERTEXFORMAT_32_32_32_32_FLOAT               = 38,
    GPUVERTEXFORMAT_32_32_32_FLOAT                  = 57,
} GPUVERTEXFORMAT;

typedef enum
{
    GPUEDRAMCOLORFORMAT_8_8_8_8                     = 0,
    GPUEDRAMCOLORFORMAT_8_8_8_8_GAMMA               = 1,
    GPUEDRAMCOLORFORMAT_2_10_10_10                  = 2,
    GPUEDRAMCOLORFORMAT_2_10_10_10_FLOAT            = 3,
    GPUEDRAMCOLORFORMAT_16_16                       = 4,
    GPUEDRAMCOLORFORMAT_16_16_16_16                 = 5,
    GPUEDRAMCOLORFORMAT_16_16_FLOAT                 = 6,
    GPUEDRAMCOLORFORMAT_16_16_16_16_FLOAT           = 7,
    GPUEDRAMCOLORFORMAT_2_10_10_10_AS_10_10_10_10   = 10,
    GPUEDRAMCOLORFORMAT_2_10_10_10_FLOAT_AS_16_16_16_16 = 12,
    GPUEDRAMCOLORFORMAT_32_FLOAT                    = 14,
    GPUEDRAMCOLORFORMAT_32_32_FLOAT                 = 15,
} GPUEDRAMCOLORFORMAT;

typedef enum
{
    GPUEDRAMDEPTHFORMAT_24_8                        = 0,
    GPUEDRAMDEPTHFORMAT_24_8_FLOAT                  = 1,
} GPUEDRAMDEPTHFORMAT;

typedef enum
{
    GPUSIGN_UNSIGNED                                = 0,
    GPUSIGN_SIGNED                                  = 1,
    GPUSIGN_BIAS                                    = 2,
    GPUSIGN_GAMMA                                   = 3,
} GPUSIGN;

#define GPUSIGN_ALL_UNSIGNED (GPUSIGN_UNSIGNED | GPUSIGN_UNSIGNED<<2 | GPUSIGN_UNSIGNED<<4 | GPUSIGN_UNSIGNED<<6)
#define GPUSIGN_ALL_SIGNED (GPUSIGN_SIGNED | GPUSIGN_SIGNED<<2 | GPUSIGN_SIGNED<<4 | GPUSIGN_SIGNED<<6)

typedef enum
{
    GPUSWIZZLE_X                                    = 0,
    GPUSWIZZLE_Y                                    = 1,
    GPUSWIZZLE_Z                                    = 2,
    GPUSWIZZLE_W                                    = 3,
    GPUSWIZZLE_0                                    = 4,
    GPUSWIZZLE_1                                    = 5,
    GPUSWIZZLE_KEEP                                 = 7, // Fetch instructions only
} GPUSWIZZLE;

#define GPUSWIZZLE_ARGB (GPUSWIZZLE_Z | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_X<<6 | GPUSWIZZLE_W<<9)
#define GPUSWIZZLE_ORGB (GPUSWIZZLE_Z | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_X<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_ABGR (GPUSWIZZLE_X | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_Z<<6 | GPUSWIZZLE_W<<9)
#define GPUSWIZZLE_OBGR (GPUSWIZZLE_X | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_Z<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_OOGR (GPUSWIZZLE_X | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_1<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_OZGR (GPUSWIZZLE_X | GPUSWIZZLE_Y<<3 | GPUSWIZZLE_0<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_RZZZ (GPUSWIZZLE_0 | GPUSWIZZLE_0<<3 | GPUSWIZZLE_0<<6 | GPUSWIZZLE_X<<9)
#define GPUSWIZZLE_OOOR (GPUSWIZZLE_X | GPUSWIZZLE_1<<3 | GPUSWIZZLE_1<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_ORRR (GPUSWIZZLE_X | GPUSWIZZLE_X<<3 | GPUSWIZZLE_X<<6 | GPUSWIZZLE_1<<9)
#define GPUSWIZZLE_GRRR (GPUSWIZZLE_X | GPUSWIZZLE_X<<3 | GPUSWIZZLE_X<<6 | GPUSWIZZLE_Y<<9)
#define GPUSWIZZLE_RGBA (GPUSWIZZLE_W | GPUSWIZZLE_Z<<3 | GPUSWIZZLE_Y<<6 | GPUSWIZZLE_X<<9)

typedef enum
{
    GPUNUMFORMAT_FRACTION                           = 0,
    GPUNUMFORMAT_INTEGER                            = 1,
} GPUNUMFORMAT;

typedef enum
{
    GPUCONSTANTTYPE_INVALID_TEXTURE                 = 0,
    GPUCONSTANTTYPE_INVALID_VERTEX                  = 1,
    GPUCONSTANTTYPE_TEXTURE                         = 2,
    GPUCONSTANTTYPE_VERTEX                          = 3,
} GPUCONSTANTTYPE;

typedef enum
{
    GPUCLAMP_WRAP                                   = 0,
    GPUCLAMP_MIRROR                                 = 1,
    GPUCLAMP_CLAMP_TO_LAST                          = 2,
    GPUCLAMP_MIRROR_ONCE_TO_LAST                    = 3,
    GPUCLAMP_CLAMP_HALFWAY                          = 4,
    GPUCLAMP_MIRROR_ONCE_HALFWAY                    = 5,
    GPUCLAMP_CLAMP_TO_BORDER                        = 6,
    GPUCLAMP_MIRROR_TO_BORDER                       = 7,
} GPUCLAMP;

typedef enum
{
    GPUDIMENSION_1D                                 = 0,
    GPUDIMENSION_2D                                 = 1,
    GPUDIMENSION_3D                                 = 2,
    GPUDIMENSION_CUBEMAP                            = 3,
} GPUDIMENSION;

typedef enum
{
    GPUREQUESTSIZE_256BIT                           = 0,
    GPUREQUESTSIZE_512BIT                           = 1,
} GPUREQUESTSIZE;

typedef enum
{
    GPUCLAMPPOLICY_D3D                              = 0,
    GPUCLAMPPOLICY_OGL                              = 1,
} GPUCLAMPPOLICY;

typedef enum
{
    GPUMINMAGFILTER_POINT                           = 0,
    GPUMINMAGFILTER_LINEAR                          = 1,
    GPUMINMAGFILTER_KEEP                            = 3, // Texture fetch instructions only
} GPUMINMAGFILTER;

typedef enum
{
    GPUMIPFILTER_POINT                              = 0,
    GPUMIPFILTER_LINEAR                             = 1,
    GPUMIPFILTER_BASEMAP                            = 2,
    GPUMIPFILTER_KEEP                               = 3, // Texture fetch instructions only
} GPUMIPFILTER;

typedef enum
{
    GPUANISOFILTER_DISABLED                         = 0,
    GPUANISOFILTER_MAX1TO1                          = 1,
    GPUANISOFILTER_MAX2TO1                          = 2,
    GPUANISOFILTER_MAX4TO1                          = 3,
    GPUANISOFILTER_MAX8TO1                          = 4,
    GPUANISOFILTER_MAX16TO1                         = 5 ,
    GPUANISOFILTER_KEEP                             = 7, // Texture fetch instructions only
} GPUANISOFILTER;

typedef enum
{
    GPUBORDERCOLOR_ABGR_BLACK                       = 0,
    GPUBORDERCOLOR_ABGR_WHITE                       = 1,
    GPUBORDERCOLOR_ACBYCR_BLACK                     = 2,
    GPUBORDERCOLOR_ACBCRY_BLACK                     = 3,
} GPUBORDERCOLOR;

typedef enum
{
    GPUTRICLAMP_NORMAL                              = 0,
    GPUTRICLAMP_ONE_SIXTH                           = 1,
    GPUTRICLAMP_ONE_FOURTH                          = 2,
    GPUTRICLAMP_THREE_EIGHTHS                       = 3,
} GPUTRICLAMP;

typedef enum
{
    GPUADDRESSCLAMP_CLAMP_TO_LAST                   = 0,
    GPUADDRESSCLAMP_CLAMP_TO_CONSTANT               = 1,
} GPUADDRESSCLAMP;

typedef enum
{
    GPUSURFACENUMBER_UREPEAT                        = 0,
    GPUSURFACENUMBER_SREPEAT                        = 1,
    GPUSURFACENUMBER_UINTEGER                       = 2,
    GPUSURFACENUMBER_SINTEGER                       = 3,
    GPUSURFACENUMBER_FLOAT                          = 7,
} GPUSURFACENUMBER;

typedef enum
{
    SURFACESWAP_LOW_RED                             = 0,
    SURFACESWAP_LOW_BLUE                            = 1,
} GPUSURFACESWAP;

typedef enum
{
    GPUPRIMTYPE_NONE                                = 0,
    GPUPRIMTYPE_POINTLIST                           = 1,
    GPUPRIMTYPE_LINELIST                            = 2,
    GPUPRIMTYPE_LINESTRIP                           = 3,
    GPUPRIMTYPE_TRILIST                             = 4,
    GPUPRIMTYPE_TRIFAN                              = 5,
    GPUPRIMTYPE_TRISTRIP                            = 6,
    GPUPRIMTYPE_TRI_WITH_WFLAGS                     = 7,
    GPUPRIMTYPE_RECTLIST                            = 8,
    GPUPRIMTYPE_LINELOOP                            = 12,
    GPUPRIMTYPE_QUADLIST                            = 13,
    GPUPRIMTYPE_QUADSTRIP                           = 14,
    GPUPRIMTYPE_POLYGON                             = 15,
    GPUPRIMTYPE_2D_COPY_RECT_LIST_V0                = 16,
    GPUPRIMTYPE_2D_COPY_RECT_LIST_V1                = 17,
    GPUPRIMTYPE_2D_COPY_RECT_LIST_V2                = 18,
    GPUPRIMTYPE_2D_COPY_RECT_LIST_V3                = 19,
    GPUPRIMTYPE_2D_FILL_RECT_LIST                   = 20,
    GPUPRIMTYPE_2D_LINE_STRIP                       = 21,
    GPUPRIMTYPE_2D_TRI_STRIP                        = 22,
} GPUPRIMTYPE;

typedef enum
{
    GPUGROUPPRIMTYPE_3D_POINT                       = 0,
    GPUGROUPPRIMTYPE_3D_LINE                        = 1,
    GPUGROUPPRIMTYPE_3D_TRI                         = 2,
    GPUGROUPPRIMTYPE_3D_RECT                        = 3,
    GPUGROUPPRIMTYPE_3D_QUAD                        = 4,
    GPUGROUPPRIMTYPE_2D_COPY_RECT_V0                = 5,
    GPUGROUPPRIMTYPE_2D_COPY_RECT_V1                = 6,
    GPUGROUPPRIMTYPE_2D_COPY_RECT_V2                = 7,
    GPUGROUPPRIMTYPE_2D_COPY_RECT_V3                = 8,
    GPUGROUPPRIMTYPE_2D_FILL_RECT                   = 9,
    GPUGROUPPRIMTYPE_2D_LINE                        = 10,
    GPUGROUPPRIMTYPE_2D_TRI                         = 11,
    GPUGROUPPRIMTYPE_PRIM_INDEX_LINE                = 12,
    GPUGROUPPRIMTYPE_PRIM_INDEX_TRI                 = 13,
    GPUGROUPPRIMTYPE_PRIM_INDEX_QUAD                = 14,
} GPUGROUPPRIMTYPE;

typedef enum
{
    GPUGROUPPRIMORDER_LIST                          = 0,
    GPUGROUPPRIMORDER_STRIP                         = 1,
    GPUGROUPPRIMORDER_FAN                           = 2,
    GPUGROUPPRIMORDER_LOOP                          = 3,
    GPUGROUPPRIMORDER_POLYGON                       = 4,
} GPUGROUPPRIMORDER;

typedef enum
{
    GPUGROUPCONV_INDEX_16                           = 0,
    GPUGROUPCONV_INDEX_32                           = 1,
    GPUGROUPCONV_UINT_16                            = 2,
    GPUGROUPCONV_UINT_32                            = 3,
    GPUGROUPCONV_SINT_16                            = 4,
    GPUGROUPCONV_SINT_32                            = 5,
    GPUGROUPCONV_FLOAT_32                           = 6,
    GPUGROUPCONV_AUTO_PRIM                          = 7,
    GPUGROUPCONV_FIX_1_23_TO_FLOAT                  = 8,
} GPUGROUPCONV;

typedef enum
{
    GPUCMP_NEVER                                    = 0,
    GPUCMP_LESS                                     = 1,
    GPUCMP_EQUAL                                    = 2,
    GPUCMP_LESS_EQUAL                               = 3,
    GPUCMP_GREATER                                  = 4,
    GPUCMP_NOT_EQUAL                                = 5,
    GPUCMP_GREATER_EQUAL                            = 6,
    GPUCMP_ALWAYS                                   = 7,
} GPUCMPFUNC;

typedef enum
{
    GPUSTENCILOP_KEEP                               = 0,
    GPUSTENCILOP_ZERO                               = 1,
    GPUSTENCILOP_REPLACE                            = 2,
    GPUSTENCILOP_INCRSAT                            = 3,
    GPUSTENCILOP_DECRSAT                            = 4,
    GPUSTENCILOP_INVERT                             = 5,
    GPUSTENCILOP_INCR                               = 6,
    GPUSTENCILOP_DECR                               = 7,
} GPUSTENCILOP;

typedef enum
{
    GPUBLEND_ZERO                                   = 0,
    GPUBLEND_ONE                                    = 1,
    GPUBLEND_SRCCOLOR                               = 4,
    GPUBLEND_INVSRCCOLOR                            = 5,
    GPUBLEND_SRCALPHA                               = 6,
    GPUBLEND_INVSRCALPHA                            = 7,
    GPUBLEND_DESTCOLOR                              = 8,
    GPUBLEND_INVDESTCOLOR                           = 9,
    GPUBLEND_DESTALPHA                              = 10,
    GPUBLEND_INVDESTALPHA                           = 11,
    GPUBLEND_BLENDFACTOR                            = 12,
    GPUBLEND_INVBLENDFACTOR                         = 13,
    GPUBLEND_CONSTANTALPHA                          = 14,
    GPUBLEND_INVCONSTANTALPHA                       = 15,
    GPUBLEND_SRCALPHASAT                            = 16,
} GPUBLEND;

typedef enum
{
    GPUBLENDOP_ADD                                  = 0,
    GPUBLENDOP_SUBTRACT                             = 1,
    GPUBLENDOP_MIN                                  = 2,
    GPUBLENDOP_MAX                                  = 3,
    GPUBLENDOP_REVSUBTRACT                          = 4,
} GPUBLENDOP;

typedef enum
{
    GPUCULL_NONE_FRONTFACE_CCW                      = 0x0,
    GPUCULL_FRONT_FRONTFACE_CCW                     = 0x1,
    GPUCULL_BACK_FRONTFACE_CCW                      = 0x2,
    GPUCULL_NONE_FRONTFACE_CW                       = 0x4,
    GPUCULL_FRONT_FRONTFACE_CW                      = 0x5,
    GPUCULL_BACK_FRONTFACE_CW                       = 0x6,
} GPUCULL;

typedef enum
{
    GPUFILL_POINT                                   = 0,
    GPUFILL_WIREFRAME                               = 1,
    GPUFILL_SOLID                                   = 2,
} GPUFILLMODE;

typedef enum
{
    GPUVERTEXSIGN_UNSIGNED                          = 0,
    GPUVERTEXSIGN_SIGNED                            = 1
} GPUVERTEXSIGN;

typedef enum
{
    GPUVERTEXFETCHOP_FETCH_VERTEX                   = 0,
} GPUVERTEXFETCHOP;

typedef enum
{
    GPUTEXTUREFETCHOP_FETCH_TEXTURE_MAP             = 1,
    GPUTEXTUREFETCHOP_GET_BORDER_COLOR_FRACTION     = 16,
    GPUTEXTUREFETCHOP_GET_COMPUTED_TEX_LOD          = 17,
    GPUTEXTUREFETCHOP_GET_GRADIENTS                 = 18,
    GPUTEXTUREFETCHOP_GET_WEIGHTS                   = 19,
    GPUTEXTUREFETCHOP_SET_TEX_LOD                   = 24,
    GPUTEXTUREFETCHOP_SET_GRADIENTS_H               = 25,
    GPUTEXTUREFETCHOP_SET_GRADIENTS_V               = 26,
} GPUTEXTUREFETCHOP;

typedef enum
{
    GPUFLOWOP_NOP                                   = 0,
    GPUFLOWOP_EXEC                                  = 1,
    GPUFLOWOP_EXEC_END                              = 2,
    GPUFLOWOP_COND_EXEC                             = 3,
    GPUFLOWOP_COND_EXEC_END                         = 4,
    GPUFLOWOP_COND_EXEC_PRED                        = 5,
    GPUFLOWOP_COND_EXEC_PRED_END                    = 6,
    GPUFLOWOP_LOOP_START                            = 7,
    GPUFLOWOP_LOOP_END                              = 8,
    GPUFLOWOP_COND_CALL                             = 9,
    GPUFLOWOP_RETURN                                = 10,
    GPUFLOWOP_COND_JUMP                             = 11,
    GPUFLOWOP_ALLOC                                 = 12,
    GPUFLOWOP_COND_EXEC_PRED_CLEAN                  = 13,
    GPUFLOWOP_COND_EXEC_PRED_CLEAN_END              = 14,
    GPUFLOWOP_VFETCH_END                            = 15,
} GPUFLOWOP;

typedef enum
{
    GPUALUSCALAROP_ADD                              = 0,
    GPUALUSCALAROP_ADDPREV                          = 1,
    GPUALUSCALAROP_MUL                              = 2,
    GPUALUSCALAROP_MULPREV                          = 3,
    GPUALUSCALAROP_MULPREV2                         = 4,
    GPUALUSCALAROP_MAX                              = 5,
    GPUALUSCALAROP_MIN                              = 6,
    GPUALUSCALAROP_SEQ                              = 7,
    GPUALUSCALAROP_SGT                              = 8,
    GPUALUSCALAROP_SGE                              = 9,
    GPUALUSCALAROP_SNE                              = 10,
    GPUALUSCALAROP_FRC                              = 11,
    GPUALUSCALAROP_TRUNC                            = 12,
    GPUALUSCALAROP_FLOOR                            = 13,
    GPUALUSCALAROP_EXP                              = 14,
    GPUALUSCALAROP_LOGC                             = 15,
    GPUALUSCALAROP_LOG                              = 16,
    GPUALUSCALAROP_RCPC                             = 17,
    GPUALUSCALAROP_RCPF                             = 18,
    GPUALUSCALAROP_RCP                              = 19,
    GPUALUSCALAROP_RSQC                             = 20,
    GPUALUSCALAROP_RSQF                             = 21,
    GPUALUSCALAROP_RSQ                              = 22,
    GPUALUSCALAROP_MAXA                             = 23,
    GPUALUSCALAROP_MAXAF                            = 24,
    GPUALUSCALAROP_SUB                              = 25,
    GPUALUSCALAROP_SUBPREV                          = 26,
    GPUALUSCALAROP_SETPEQ                           = 27,
    GPUALUSCALAROP_SETPNE                           = 28,
    GPUALUSCALAROP_SETPGT                           = 29,
    GPUALUSCALAROP_SETPGE                           = 30,
    GPUALUSCALAROP_SETPINV                          = 31,
    GPUALUSCALAROP_SETPPOP                          = 32,
    GPUALUSCALAROP_SETPCLR                          = 33,
    GPUALUSCALAROP_SETPRSTR                         = 34,
    GPUALUSCALAROP_KILLEQ                           = 35,
    GPUALUSCALAROP_KILLGT                           = 36,
    GPUALUSCALAROP_KILLGE                           = 37,
    GPUALUSCALAROP_KILLNE                           = 38,
    GPUALUSCALAROP_KILLONE                          = 39,
    GPUALUSCALAROP_SQRT                             = 40,
    GPUALUSCALAROP_MULC0                            = 42,
    GPUALUSCALAROP_MULC1                            = 43,
    GPUALUSCALAROP_ADDC0                            = 44,
    GPUALUSCALAROP_ADDC1                            = 45,
    GPUALUSCALAROP_SUBC0                            = 46,
    GPUALUSCALAROP_SUBC1                            = 47,
    GPUALUSCALAROP_SIN                              = 48,
    GPUALUSCALAROP_COS                              = 49,
    GPUALUSCALAROP_RETAINPREV                       = 50
} GPUALUSCALAROP;

typedef enum
{
    GPUALUVECTOROP_ADD                              = 0,
    GPUALUVECTOROP_MUL                              = 1,
    GPUALUVECTOROP_MAX                              = 2,
    GPUALUVECTOROP_MIN                              = 3,
    GPUALUVECTOROP_SEQ                              = 4,
    GPUALUVECTOROP_SGT                              = 5,
    GPUALUVECTOROP_SGE                              = 6,
    GPUALUVECTOROP_SNE                              = 7,
    GPUALUVECTOROP_FRC                              = 8,
    GPUALUVECTOROP_TRUNC                            = 9,
    GPUALUVECTOROP_FLOOR                            = 10,
    GPUALUVECTOROP_MAD                              = 11,
    GPUALUVECTOROP_CNDEQ                            = 12,
    GPUALUVECTOROP_CNDGE                            = 13,
    GPUALUVECTOROP_CNDGT                            = 14,
    GPUALUVECTOROP_DP4                              = 15,
    GPUALUVECTOROP_DP3                              = 16,
    GPUALUVECTOROP_DP2ADD                           = 17,
    GPUALUVECTOROP_CUBE                             = 18,
    GPUALUVECTOROP_MAX4                             = 19,
    GPUALUVECTOROP_SETPEQP                          = 20,
    GPUALUVECTOROP_SETPNEP                          = 21,
    GPUALUVECTOROP_SETPGTP                          = 22,
    GPUALUVECTOROP_SETPGEP                          = 23,
    GPUALUVECTOROP_KILLEQ                           = 24,
    GPUALUVECTOROP_KILLGT                           = 25,
    GPUALUVECTOROP_KILLGE                           = 26,
    GPUALUVECTOROP_KILLNE                           = 27,
    GPUALUVECTOROP_DST                              = 28,
    GPUALUVECTOROP_MAXA                             = 29
} GPUALUVECTOROP;

typedef enum
{
    GPUALUSRCSELECT_C                               = 0,
    GPUALUSRCSELECT_R                               = 1
} GPUALUSRCSELECT;

typedef enum
{
    GPUEXPORTREGISTER_PS_COLOR_0                    = 0,
    GPUEXPORTREGISTER_PS_COLOR_1                    = 1,
    GPUEXPORTREGISTER_PS_COLOR_2                    = 2,
    GPUEXPORTREGISTER_PS_COLOR_3                    = 3,

    GPUEXPORTREGISTER_PS_EXPORT_ADDRESS             = 32,
    GPUEXPORTREGISTER_PS_EXPORT_DATA_0              = 33,
    GPUEXPORTREGISTER_PS_EXPORT_DATA_1              = 34,
    GPUEXPORTREGISTER_PS_EXPORT_DATA_2              = 35,
    GPUEXPORTREGISTER_PS_EXPORT_DATA_3              = 36,
    GPUEXPORTREGISTER_PS_EXPORT_DATA_4              = 37,

    GPUEXPORTREGISTER_PS_DEPTH                      = 61,
} GPUEXPORTREGISTER_PS;

typedef enum
{
    GPUEXPORTREGISTER_VS_INTERPOLATOR_0             = 0,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_1             = 1,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_2             = 2,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_3             = 3,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_4             = 4,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_5             = 5,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_6             = 6,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_7             = 7,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_8             = 8,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_9             = 9,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_10            = 10,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_11            = 11,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_12            = 12,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_13            = 13,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_14            = 14,
    GPUEXPORTREGISTER_VS_INTERPOLATOR_15            = 15,

    GPUEXPORTREGISTER_VS_EXPORT_ADDRESS             = 32,
    GPUEXPORTREGISTER_VS_EXPORT_DATA_0              = 33,
    GPUEXPORTREGISTER_VS_EXPORT_DATA_1              = 34,
    GPUEXPORTREGISTER_VS_EXPORT_DATA_2              = 35,
    GPUEXPORTREGISTER_VS_EXPORT_DATA_3              = 36,
    GPUEXPORTREGISTER_VS_EXPORT_DATA_4              = 37,

    GPUEXPORTREGISTER_VS_POSITION                   = 62,
    GPUEXPORTREGISTER_VS_SPRITE_EDGE_KILL           = 63,
} GPUEXPORTREGISTER_VS;

typedef enum
{
    GPUCOMMANDOP_NOP                                = 0x10,
    GPUCOMMANDOP_REG_RMW                            = 0x21,
    GPUCOMMANDOP_DRAW                               = 0x22,
    GPUCOMMANDOP_VIZ_QUERY                          = 0x23,
    GPUCOMMANDOP_SET_STATE                          = 0x25,
    GPUCOMMANDOP_WAIT_FOR_IDLE                      = 0x26,
    GPUCOMMANDOP_LOAD_SHADER                        = 0x27,
    GPUCOMMANDOP_LOAD_SHADER_IMMEDIATE              = 0x2b,
    GPUCOMMANDOP_SET_CONSTANT                       = 0x2d,
    GPUCOMMANDOP_LOAD_ALU_CONSTANT                  = 0x2f,
    GPUCOMMANDOP_DRAW_IMMEDIATE                     = 0x36,
    GPUCOMMANDOP_MPEG_INDEX                         = 0x3a,
    GPUCOMMANDOP_INVALIDATE_STATE                   = 0x3b,
    GPUCOMMANDOP_WAIT_REG_MEM                       = 0x3c,
    GPUCOMMANDOP_MEM_WRITE                          = 0x3d,
    GPUCOMMANDOP_REG_TO_MEM                         = 0x3e,
    GPUCOMMANDOP_INDIRECT_BUFFER                    = 0x3f,
    GPUCOMMANDOP_COND_WRITE                         = 0x45,
    GPUCOMMANDOP_EVENT_WRITE                        = 0x46,
    GPUCOMMANDOP_FIX_2_FLT_REG                      = 0x4d,
    GPUCOMMANDOP_MEM_WRITE_COUNTER                  = 0x4f,
    GPUCOMMANDOP_SET_BIN_MASK                       = 0x50,
    GPUCOMMANDOP_SET_BIN_SELECT                     = 0x51,
    GPUCOMMANDOP_WAIT_REG_EQ                        = 0x52,
    GPUCOMMANDOP_WAIT_REG_GTE                       = 0x53,
    GPUCOMMANDOP_CPU_INTERRUPT                      = 0x54,
    GPUCOMMANDOP_EVENT_WRITE_SHADER                 = 0x58,
    GPUCOMMANDOP_EVENT_WRITE_CACHE_FLUSH            = 0x59,
    GPUCOMMANDOP_EVENT_WRITE_SCREEN_EXTENT          = 0x5a,
    GPUCOMMANDOP_EVENT_WRITE_ZPASS_DONE             = 0x5b,
    GPUCOMMANDOP_CONTEXT_UPDATE                     = 0x5e,
} GPUCOMMANDOP;

typedef enum
{
    GPULOADTYPE_VERTEX                              = 0,
    GPULOADTYPE_PIXEL                               = 1,
    GPULOADTYPE_SHARED                              = 2,
} GPULOADTYPE;

typedef enum
{
    GPUHIZFUNC_LESS_EQUAL                           = 0,
    GPUHIZFUNC_GREATER_EQUAL                        = 1,
} GPUHIZFUNC;

typedef enum
{
    GPUHISTENCILFUNC_EQUAL                          = 0,
    GPUHISTENCILFUNC_NOT_EQUAL                      = 1,
} GPUHISTENCILFUNC;

typedef enum
{
    GPUEDRAMMODE_NOP                                = 0,
    GPUEDRAMMODE_COLOR_DEPTH                        = 4,
    GPUEDRAMMODE_DOUBLE_DEPTH                       = 5,
    GPUEDRAMMODE_COPY                               = 6,
} GPUEDRAMMODE;

typedef enum
{
    GPUCLIPPLANEMODE_CULL_CENTER_NO_BIAS            = 0,
    GPUCLIPPLANEMODE_CULL_CENTER_RADIUS_BIAS        = 1,
    GPUCLIPPLANEMODE_CULL_CENTER_RADIUS_BIAS_EXPAND = 2,
    GPUCLIPPLANEMODE_ALWAYS_EXPAND                  = 3,
} GPUCLIPPLANEMODE;

typedef enum
{
    GPUSAMPLECONTROL_CENTROIDS_ONLY                 = 0,
    GPUSAMPLECONTROL_CENTERS_ONLY                   = 1,
    GPUSAMPLECONTROL_CENTROIDS_AND_CENTERS          = 2,
} GPUSAMPLECONTROL;

typedef enum
{
    GPUPATHSELECT_VERTEX_REUSE                      = 0,
    GPUPATHSELECT_TESS_ENABLE                       = 1,
    GPUPATHSELECT_PASSTHRU                          = 2,
} GPUPATHSELECT;

typedef enum
{
    GPUTESSMODE_DISCRETE                            = 0,
    GPUTESSMODE_CONTINUOUS                          = 1,
    GPUTESSMODE_ADAPTIVE                            = 2,
} GPUTESSMODE;

typedef enum
{
    GPUPIXCENTER_ZERO                               = 0,
    GPUPIXCENTER_HALF                               = 1,
} GPUPIXCENTER;

typedef enum
{
    GPUROUNDMODE_TRUNCATE                           = 0,
    GPUROUNDMODE_ROUND                              = 1,
    GPUROUNDMODE_ROUND_TO_EVEN                      = 2,
    GPUROUNDMODE_ROUND_TO_ODD                       = 3,
} GPUROUNDMODE;

typedef enum
{
    GPUQUANTMODE_16TH                               = 0,
    GPUQUANTMODE_8TH                                = 1,
    GPUQUANTMODE_4TH                                = 2,
    GPUQUANTMODE_HALF                               = 3,
    GPUQUANTMODE_ONE                                = 4,
} GPUQUANTMODE;

typedef enum
{
    GPUCOPYSRCSELECT_RENDER_TARGET_0                = 0,
    GPUCOPYSRCSELECT_RENDER_TARGET_1                = 1,
    GPUCOPYSRCSELECT_RENDER_TARGET_2                = 2,
    GPUCOPYSRCSELECT_RENDER_TARGET_3                = 3,
    GPUCOPYSRCSELECT_DEPTH_STENCIL                  = 4,
} GPUCOPYSRCSELECT;

typedef enum
{
    GPUCOPYSAMPLESELECT_SAMPLE_0                    = 0,
    GPUCOPYSAMPLESELECT_SAMPLE_1                    = 1,
    GPUCOPYSAMPLESELECT_SAMPLE_2                    = 2,
    GPUCOPYSAMPLESELECT_SAMPLE_3                    = 3,
    GPUCOPYSAMPLESELECT_SAMPLES_0_1                 = 4,
    GPUCOPYSAMPLESELECT_SAMPLES_2_3                 = 5,
    GPUCOPYSAMPLESELECT_SAMPLES_0_1_2_3             = 6,
} GPUCOPYSAMPLESELECT;

typedef enum
{
    GPUCOPYCOMMAND_RAW                              = 0,
    GPUCOPYCOMMAND_CONVERT                          = 1,
    GPUCOPYCOMMAND_1_1_1_1                          = 2,
    GPUCOPYCOMMAND_NULL                             = 3,
} GPUCOPYCOMMAND;

typedef enum
{
    GPUVSEXPORTMODE_POSITION_ONLY                   = 0,
    GPUVSEXPORTMODE_SPRITE                          = 2,
    GPUVSEXPORTMODE_EDGE                            = 3,
    GPUVSEXPORTMODE_KILL                            = 4,
    GPUVSEXPORTMODE_SPRITE_KILL                     = 5,
    GPUVSEXPORTMODE_EDGE_KILL                       = 6,
    GPUVSEXPORTMODE_MULTIPASS                       = 7,
} GPUVSEXPORTMODE;

typedef enum
{
    GPUSAMPLES_1X                                   = 0,
    GPUSAMPLES_2X                                   = 1,
    GPUSAMPLES_4X                                   = 2,
} GPUSAMPLES;

typedef enum
{
    GPUEXECINSTRUCTIONTYPE_ALU                      = 0,
    GPUEXECINSTRUCTIONTYPE_FETCH                    = 1,
} GPUEXECINSTRUCTIONTYPE;

typedef enum
{
    GPUEXECSERIALIZEMODE_UNSERIALIZED               = 0,
    GPUEXECSERIALIZEMODE_SERIALIZED                 = 1,
} GPUEXECSERIALIZEMODE;

typedef enum
{
    GPUEXECFETCHCACHETYPE_TEXTURE                   = 0,
    GPUEXECFETCHCACHETYPE_VERTEX                    = 1,
} GPUEXECFETCHCACHETYPE;

typedef enum
{
    GPUALLOCBUFFERSELECT_POSITION                   = 1,
    GPUALLOCBUFFERSELECT_INTERPOLATORS              = 2, // for vertex shaders
    GPUALLOCBUFFERSELECT_COLORS                     = 2, // for pixel shaders
    GPUALLOCBUFFERSELECT_EXPORT                     = 3
} GPUALLOCBUFFERSELECT;

typedef enum
{
    GPUSCREENEXTENTS_NONCULLED                      = 0,
    GPUSCREENEXTENTS_ALLTILES                       = 1,
    GPUSCREENEXTENTS_PRIMEXTENTS                    = 2,
} GPUSCREENEXTENTS;

typedef enum
{
    GPUINITIATOR_VS_DEALLOC                         = 0,
    GPUINITIATOR_PS_DEALLOC                         = 1,
    GPUINITIATOR_VS_DONE_WRITE_BACK                 = 2,
    GPUINITIATOR_PS_DONE_WRITE_BACK                 = 3,
    GPUINITIATOR_CACHE_FLUSH_WRITE_BACK             = 4,
    GPUINITIATOR_CONTEXT_DONE                       = 5,
    GPUINITIATOR_CACHE_FLUSH                        = 6,
    GPUINITIATOR_VIZQUERY_START                     = 7,
    GPUINITIATOR_VIZQUERY_END                       = 8,
    GPUINITIATOR_SC_WAIT_WC                         = 9,
    GPUINITIATOR_MPASS_PS_CP_REFETCH                = 10,
    GPUINITIATOR_MPASS_PS_RST_START                 = 11,
    GPUINITIATOR_MPASS_PS_INCR_START                = 12,
    GPUINITIATOR_RST_PIX_CNT                        = 13,
    GPUINITIATOR_RST_VTX_CNT                        = 14,
    GPUINITIATOR_TILE_FLUSH                         = 15,
    GPUINITIATOR_CACHE_FLUSH_AND_INV_WRITE_BACK_EVENT = 20,
    GPUINITIATOR_ZPASS_DONE                         = 21,
    GPUINITIATOR_CACHE_FLUSH_AND_INV_EVENT          = 22,
    GPUINITIATOR_PERFCOUNTER_START                  = 23,
    GPUINITIATOR_PERFCOUNTER_STOP                   = 24,
    GPUINITIATOR_SCREEN_EXT_INIT                    = 25,
    GPUINITIATOR_SCREEN_EXT_RPT                     = 26,
    GPUINITIATOR_VS_FETCH_DONE_WRITE_BACK           = 27,
} GPUINITIATOR;

typedef enum
{
    GPUSYNCFUNCTION_NEVER                           = 0,
    GPUSYNCFUNCTION_LESS                            = 1,
    GPUSYNCFUNCTION_LEQUAL                          = 2,
    GPUSYNCFUNCTION_EQUAL                           = 3,
    GPUSYNCFUNCTION_NOTEQUAL                        = 4,
    GPUSYNCFUNCTION_GEQUAL                          = 5,
    GPUSYNCFUNCTION_GREATER                         = 6,
    GPUSYNCFUNCTION_ALWAYS                          = 7,
} GPUSYNCFUNCTION;

typedef enum
{
    GPUSYNCSPACE_REGISTER                           = 0,
    GPUSYNCSPACE_MEMORY                             = 1,
} GPUSYNCSPACE;

typedef enum
{
    GPUCONSTANTID_ALU                               = 0,
    GPUCONSTANTID_FETCH                             = 1,
    GPUCONSTANTID_BOOLEAN                           = 2,
    GPUCONSTANTID_INTEGER                           = 3,
    GPUCONSTANTID_REGISTER                          = 4,
} GPUCONSTANTID;

typedef enum
{
    GPUINDEXSELECT_DMA                              = 0,
    GPUINDEXSELECT_IMMEDIATE                        = 1,
    GPUINDEXSELECT_AUTO                             = 2,
} GPUINDEXSELECT;

typedef enum
{
    GPUINDEXTYPE_16BIT                              = 0,
    GPUINDEXTYPE_32BIT                              = 1,
} GPUINDEXTYPE;

typedef enum
{
    GPUDESTBASE7_DOUBLE_DEPTH_DISABLED              = 0,
    GPUDESTBASE7_DOUBLE_DEPTH_ENABLED               = 0X1000,
} GPUDESTBASE7;

typedef enum
{
    GPUDESTBASE6_MEMEXPORT_DISABLED                 = 0,
    GPUDESTBASE6_MEMEXPORT_ENABLED                  = 0X1000,
} GPUDESTBASE6;

//------------------------------------------------------------------------------

typedef struct
{
    DWORD Total[2];         // Odd/even pair, little-endian
    DWORD ZFail[2];         // Odd/even pair, little-endian
    DWORD ZPass[2];         // Odd/even pair, little-endian
    DWORD StencilFail[2];   // Odd/even pair, little-endian
} GPU_SAMPLECOUNT;

typedef struct
{
    WORD MinX;              // In tile coordinates, multiply by 8 for pixel coordinates
    WORD MaxX;              // In tile coordinates, multiply by 8 for pixel coordinates
    WORD MinY;              // In tile coordinates, multiply by 8 for pixel coordinates
    WORD MaxY;              // In tile coordinates, multiply by 8 for pixel coordinates
    WORD MinZ;              // 11 MSBs of Z
    WORD MaxZ;              // 11 MSBs of Z
} GPU_SCREENEXTENT;

//------------------------------------------------------------------------------

typedef union {
    struct {
        DWORD SurfacePitch                          : 14;
        DWORD                                       : 2;
        DWORD MsaaSamples                           : 2;    // GPUSAMPLES
        DWORD HiZPitch                              : 14;
    };
    DWORD dword;
} GPU_SURFACEINFO;

typedef union {
    struct {
        DWORD ColorBase                             : 12;
        DWORD                                       : 4;
        DWORD ColorFormat                           : 4;    // GPUEDRAMCOLORFORMAT
        DWORD ColorExpBias                          : 6;
        DWORD                                       : 6;
    };
    DWORD dword;
} GPU_COLORINFO;

typedef union {
    struct {
        DWORD DepthBase                             : 12;
        DWORD                                       : 4;
        DWORD DepthFormat                           : 1;    // GPUEDRAMDEPTHFORMAT
        DWORD DisableHZClamp                        : 1;
        DWORD                                       : 14;
    };
    DWORD dword;
} GPU_DEPTHINFO;

typedef union {
    struct {
        int X                                       : 15;
        DWORD                                       : 1;
        int Y                                       : 15;
        DWORD                                       : 1;
    };
    DWORD dword;
} GPU_POINT;

typedef union {
    struct {
        DWORD Write0                                : 4;
        DWORD Write1                                : 4;
        DWORD Write2                                : 4;
        DWORD Write3                                : 4;
        DWORD                                       : 16;
    };
    DWORD dword;
} GPU_COLORMASK;

typedef union {
    struct {
        DWORD Ref                                   : 8;
        DWORD Mask                                  : 8;
        DWORD WriteMask                             : 8;
        DWORD                                       : 8;
    };
    DWORD dword;
} GPU_STENCILREFMASK;

typedef union {
    struct {
        DWORD VsMaxReg                              : 6;
        DWORD                                       : 2;
        DWORD PsMaxReg                              : 6;
        DWORD                                       : 2;
        DWORD VsResource                            : 1;
        DWORD PsResource                            : 1;
        DWORD ParamGen                              : 1;
        DWORD GenIndexPix                           : 1;
        DWORD VsExportCount                         : 4;
        DWORD VsExportMode                          : 3;    // GPUVSEXPORTMODE
        DWORD PsExportZ                             : 1;
        DWORD PsExportColorCount                    : 3;
        DWORD GenIndexVtx                           : 1;
    };
    DWORD dword;
} GPU_PROGRAMCONTROL;

typedef union {
    struct {
        DWORD InstPredOptimize                      : 1;
        DWORD OutputScreenXY                        : 1;
        DWORD SampleControl                         : 2;    // GPUSAMPLECONTROL
        DWORD                                       : 4;
        DWORD ParamGenPos                           : 8;
        DWORD PerfCounterRef                        : 1;
        DWORD YieldOptimize                         : 1;
        DWORD TxCacheSelect                         : 1;
        DWORD                                       : 13;
    };
    DWORD dword;
} GPU_CONTEXTMISC;

typedef union {
    struct {
        DWORD ParamShade                            : 16;
        DWORD SamplingPattern                       : 16;
    };
    DWORD dword;
} GPU_INTERPOLATORCONTROL;

typedef union {
    struct {
        DWORD Wrap0                                 : 4;
        DWORD Wrap1                                 : 4;
        DWORD Wrap2                                 : 4;
        DWORD Wrap3                                 : 4;
        DWORD Wrap4                                 : 4;
        DWORD Wrap5                                 : 4;
        DWORD Wrap6                                 : 4;
        DWORD Wrap7                                 : 4;
    };
    DWORD dword;
} GPU_WRAPPING1;

typedef union {
    struct {
        DWORD Wrap8                                 : 4;
        DWORD Wrap9                                 : 4;
        DWORD Wrap10                                : 4;
        DWORD Wrap11                                : 4;
        DWORD Wrap12                                : 4;
        DWORD Wrap13                                : 4;
        DWORD Wrap14                                : 4;
        DWORD Wrap15                                : 4;
    };
    DWORD dword;
} GPU_WRAPPING0;

typedef union {
    struct {
        DWORD StencilEnable                         : 1;
        DWORD ZEnable                               : 1;
        DWORD ZWriteEnable                          : 1;
        DWORD                                       : 1;
        DWORD ZFunc                                 : 3;
        DWORD BackFaceEnable                        : 1;
        DWORD StencilFunc                           : 3;    // GPUCMPFUNC
        DWORD StencilFail                           : 3;    // GPUSTENCILOP
        DWORD StencilZPass                          : 3;    // GPUSTENCILOP
        DWORD StencilZFail                          : 3;    // GPUSTENCILOP
        DWORD StencilFuncBF                         : 3;    // GPUCMPFUNC
        DWORD StencilFailBF                         : 3;    // GPUSTENCILOP
        DWORD StencilZPassBF                        : 3;    // GPUSTENCILOP
        DWORD StencilZFailBF                        : 3;    // GPUSTENCILOP
    };
    DWORD dword;
} GPU_DEPTHCONTROL;

typedef union {
    struct {
        DWORD ColorSrcBlend                         : 5;    // GPUBLEND
        DWORD ColorBlendOp                          : 3;    // GPUBLENDOP
        DWORD ColorDestBlend                        : 5;    // GPUBLEND
        DWORD                                       : 3;
        DWORD AlphaSrcBlend                         : 5;    // GPUBLEND
        DWORD AlphaBlendOp                          : 3;    // GPUBLENDOP
        DWORD AlphaDestBlend                        : 5;    // GPUBLEND
        DWORD                                       : 3;
    };
    DWORD dword;
} GPU_BLENDCONTROL;

typedef union {
    struct {
        DWORD AlphaFunc                             : 3;    // GPUCMPFUNC
        DWORD AlphaTestEnable                       : 1;
        DWORD AlphaToMaskEnable                     : 1;
        DWORD                                       : 19;
        DWORD AlphaToMaskOffset0                    : 2;
        DWORD AlphaToMaskOffset1                    : 2;
        DWORD AlphaToMaskOffset2                    : 2;
        DWORD AlphaToMaskOffset3                    : 2;
    };
    DWORD dword;
} GPU_COLORCONTROL;

typedef union {
    struct {
        DWORD HiZWriteEnable                        : 1;
        DWORD HiZEnable                             : 1;
        DWORD HiStencilWriteEnable                  : 1;
        DWORD HiStencilEnable                       : 1;
        DWORD HiZFunc                               : 1;    // GPUHIZFUNC
        DWORD HiStencilFunc                         : 1;    // GPUHISTENCILFUNC
        DWORD                                       : 2;
        DWORD HiStencilRef                          : 8;
        DWORD                                       : 1;
        DWORD HiBaseAddr                            : 15;
    };
    DWORD dword;
} GPU_HICONTROL;

typedef union {
    struct {
        DWORD ClipPlaneEnable0                      : 1;
        DWORD ClipPlaneEnable1                      : 1;
        DWORD ClipPlaneEnable2                      : 1;
        DWORD ClipPlaneEnable3                      : 1;
        DWORD ClipPlaneEnable4                      : 1;
        DWORD ClipPlaneEnable5                      : 1;
        DWORD                                       : 8;
        DWORD ClipPlaneMode                         : 2;    // GPUCLIPPLANEMODE
        DWORD ClipDisable                           : 1;
        DWORD ClipPlaneCullOnlyEnable               : 1;
        DWORD BoundaryEdgeFlagEnable                : 1;
        DWORD DxClipSpaceDef                        : 1;
        DWORD DisableClipErrDetect                  : 1;
        DWORD VtxKillOr                             : 1;
        DWORD XyNanRetain                           : 1;
        DWORD ZNanRetain                            : 1;
        DWORD WNanRetain                            : 1;
        DWORD                                       : 7;
    };
    DWORD dword;
} GPU_CLIPCONTROL;

typedef union {
    struct {
        DWORD CullMode                              : 3;    // GPUCULL
        DWORD PolyMode                              : 2;    // BOOL
        DWORD PolyModeFrontPType                    : 3;    // GPUFILLMODE
        DWORD PolyModeBackPType                     : 3;    // GPUFILLMODE
        DWORD PolyOffsetFrontEnable                 : 1;
        DWORD PolyOffsetBackEnable                  : 1;
        DWORD PolyOffsetParaEnable                  : 1;
        DWORD                                       : 1;
        DWORD MsaaEnable                            : 1;
        DWORD VtxWindowOffsetEnable                 : 1;
        DWORD                                       : 2;
        DWORD ProvokingVtxLast                      : 1;
        DWORD PerspCorrDisable                      : 1;
        DWORD MultiPrimIbEnable                     : 1;
        DWORD                                       : 1;
        DWORD QuadOrderEnable                       : 1;
        DWORD ScOneQuadPerClock                     : 1;
        DWORD                                       : 7;
    };
    DWORD dword;
} GPU_MODECONTROL;

typedef union {
    struct {
        DWORD VportXScaleEnable                     : 1;
        DWORD VportXOffsetEnable                    : 1;
        DWORD VportYScaleEnable                     : 1;
        DWORD VportYOffsetEnable                    : 1;
        DWORD VportZScaleEnable                     : 1;
        DWORD VportZOffsetEnable                    : 1;
        DWORD                                       : 2;
        DWORD VtxXyFmt                              : 1;
        DWORD VtxZFmt                               : 1;
        DWORD VtxW0Fmt                              : 1;
        DWORD PerfCounterRef                        : 1;
        DWORD                                       : 20;
    };
    DWORD dword;
} GPU_VTECONTROL;

typedef union {
    struct {
        DWORD EdramMode                             : 3;    // GPUEDRAMMODE
        DWORD ColorDepthMacro                       : 1;
        DWORD                                       : 28;
    };
    DWORD dword;
} GPU_EDRAMMODECONTROL;

typedef union {
    struct {
        DWORD Height                                : 16;
        DWORD Width                                 : 16;
    };
    DWORD dword;
} GPU_POINTSIZE;

typedef union {
    struct {
        DWORD MinSize                               : 16;
        DWORD MaxSize                               : 16;
    };
    DWORD dword;
} GPU_POINTMINMAX;

typedef union {
    struct {
        DWORD Width                                 : 16;
        DWORD                                       : 16;
    };
    DWORD dword;
} GPU_LINECONTROL;

typedef union {
    struct {
        DWORD PathSelect                            : 2;    // GPUPATHSELECT
        DWORD                                       : 30;
    };
    DWORD dword;
} GPU_OUTPUTPATHCONTROL;

typedef union {
    struct {
        DWORD TessMode                              : 2;    // GPUTESSMODE
        DWORD                                       : 30;
    };
    DWORD dword;
} GPU_HOSCONTROL;

typedef union {
    struct {
        DWORD GroupPrimType                         : 4;    // GPUGROUPPRIMTYPE
        DWORD                                       : 10;
        DWORD GroupRetainOrder                      : 1;
        DWORD GroupRetainQuads                      : 1;
        DWORD GroupPrimOrder                        : 3;    // GPUGROUPPRIMORDER
        DWORD                                       : 13;
    };
    DWORD dword;
} GPU_GROUPPRIMTYPE;

typedef union {
    struct {
        DWORD CompXEnable                           : 1;
        DWORD CompYEnable                           : 1;
        DWORD CompZEnable                           : 1;
        DWORD CompWEnable                           : 1;
        DWORD                                       : 4;
        DWORD Stride                                : 8;
        DWORD Shift                                 : 8;
        DWORD                                       : 8;
    };
    DWORD dword;
} GPU_GROUPVECTCONTROL;

typedef union {
    struct {
        DWORD XConv                                 : 4;    // GPUGROUPCONV
        DWORD XOffset                               : 4;
        DWORD YConv                                 : 4;    // GPUGROUPCONV
        DWORD YOffset                               : 4;
        DWORD ZConv                                 : 4;    // GPUGROUPCONV
        DWORD ZOffset                               : 4;
        DWORD WConv                                 : 4;    // GPUGROUPCONV
        DWORD WOffset                               : 4;
    };
    DWORD dword;
} GPU_GROUPVECTFMTCONTROL;

typedef union {
    struct {
        DWORD MPassPixVecPerPass                    : 20;
        DWORD                                       : 11;
        DWORD MPassPsEnable                         : 1;
    };
    DWORD dword;
} GPU_MPASSPSCONTROL;

typedef union {
    struct {
        DWORD VizQueryEnable                        : 1;
        DWORD VizQueryId                            : 6;
        DWORD KillPixPostHiZ                        : 1;
        DWORD KillPixPostDetailMask                 : 1;
        DWORD                                       : 23;
    };
    DWORD dword;
} GPU_VIZQUERY;

typedef union {
    struct {
        DWORD Misc                                  : 16;
        DWORD                                       : 16;
    };
    DWORD dword;
} GPU_ENHANCE;

typedef union {
    struct {
        DWORD BresControl                           : 8;
        DWORD UseBresControl                        : 1;
        DWORD ExpandLineWidth                       : 1;
        DWORD LastPixel                             : 1;
        DWORD                                       : 21;
    };
    DWORD dword;
} GPU_SCLINECONTROL;

typedef union {
    struct {
        DWORD MsaaNumSamples                        : 3;
        DWORD                                       : 10;
        DWORD MaxSampleDist                         : 4;
        DWORD                                       : 15;
    };
    DWORD dword;
} GPU_AACONFIG;

typedef union {
    struct {
        DWORD PixCenter                             : 1;    // GPUPIXCENTER
        DWORD RoundMode                             : 2;    // GPUROUNDMODE
        DWORD QuantMode                             : 3;    // GPUQUANTMODE
        DWORD                                       : 26;
    };
    DWORD dword;
} GPU_VTXCONTROL;

typedef union {
    struct {
        DWORD Base                                  : 9;
        DWORD                                       : 3;
        DWORD Size                                  : 9;
        DWORD                                       : 11;
    };
    DWORD dword;
} GPU_CONST;

typedef union {
    struct {
        DWORD DbProgOn                              : 1;
        DWORD                                       : 3;
        DWORD DbProbBreak                           : 1;
        DWORD                                       : 3;
        DWORD DbProbAddr                            : 11;
        DWORD                                       : 5;
        DWORD DbProbCount                           : 8;
    };
    DWORD dword;
} GPU_DEBUGMISC0;

typedef union {
    struct {
        DWORD DbOnPix                               : 1;
        DWORD DbOnVtx                               : 1;
        DWORD                                       : 6;
        DWORD DbInstCount                           : 8;
        DWORD DbBreakAddr                           : 11;
        DWORD                                       : 5;
    };
    DWORD dword;
} GPU_DEBUGMISC1;

typedef union {
    struct {
        DWORD VtxReuseDepth                         : 8;
        DWORD                                       : 24;
    };
    DWORD dword;
} GPU_VERTEXREUSEBLOCKCONTROL;

typedef union {
    struct {
        DWORD DeallocDist                           : 7;
        DWORD                                       : 25;
    };
    DWORD dword;
} GPU_OUTDEALLOCCONTROL;

typedef union {
    struct {
        DWORD CopySrcSelect                         : 3;    // GPUCOPYSRCSELECT
        DWORD                                       : 1;
        DWORD CopySampleSelect                      : 3;    // GPUCOPYSAMPLESELECT
        DWORD                                       : 1;
        DWORD ColorClearEnable                      : 1;
        DWORD DepthClearEnable                      : 1;
        DWORD                                       : 10;
        DWORD CopyCommand                           : 2;    // GPUCOPYCOMMAND
        DWORD                                       : 10;
    };
    DWORD dword;
} GPU_COPYCONTROL;

typedef union {
    struct {
        DWORD CopyDestPitch                         : 14;
        DWORD                                       : 2;
        DWORD CopyDestHeight                        : 14;
        DWORD                                       : 2;
    };
    DWORD dword;
} GPU_COPYDESTPITCH;

typedef union {
    struct {
        DWORD CopyDestEndian                        : 3;    // GPUENDIAN
        DWORD CopyDestArray                         : 1;
        DWORD CopyDestSlice                         : 3;
        DWORD CopyDestFormat                        : 6;
        DWORD CopyDestNumber                        : 3;
        DWORD CopyDestExpBias                       : 6;
        DWORD                                       : 2;
        DWORD CopyDestSwap                          : 1;
        DWORD                                       : 7;
    };
    DWORD dword;
} GPU_COPYDESTINFO;

typedef union {
    struct {
        DWORD CopyFuncRed                           : 3;
        DWORD                                       : 1;
        DWORD CopyFuncGreen                         : 3;
        DWORD                                       : 1;
        DWORD CopyFuncBlue                          : 3;
        DWORD                                       : 1;
        DWORD CopyFuncAlpha                         : 3;
        DWORD                                       : 17;
    };
    DWORD dword;
} GPU_COPYFUNC;

typedef union {
    struct {
        DWORD CopyRefRed                            : 8;
        DWORD CopyRefGreen                          : 8;
        DWORD CopyRefBlue                           : 8;
        DWORD CopyRefAlpha                          : 8;
    };
    DWORD dword;
} GPU_COPYREF;

typedef union {
    struct {
        DWORD CopyMaskRed                           : 8;
        DWORD CopyMaskGreen                         : 8;
        DWORD CopyMaskBlue                          : 8;
        DWORD CopyMaskAlpha                         : 8;
    };
    DWORD dword;
} GPU_COPYMASK;

typedef union {
    struct {
        DWORD ResetSampleCount                      : 1;
        DWORD CopySampleCount                       : 1;
        DWORD                                       : 30;
    };
    DWORD dword;
} GPU_SAMPLECOUNTCONTROL;

typedef union {
    struct {
        DWORD BankActToActSClk                      : 6;
        DWORD                                       : 2;
        DWORD DisableFragCombine                    : 1;
        DWORD DisableReOrder                        : 1;
        DWORD HzFudgeShift                          : 2;
        DWORD ScreenExtMethod                       : 2;    // GPUSCREENEXTENTS
        DWORD                                       : 18;
    };
    DWORD dword;
} GPU_BCCONTROL;

typedef union {
    struct {
        DWORD MatchingContexts                      : 8;
        DWORD CopyDestBaseEnable                    : 1;
        DWORD DestBase0Enable                       : 1;
        DWORD DestBase1Enable                       : 1;
        DWORD DestBase2Enable                       : 1;
        DWORD DestBase3Enable                       : 1;
        DWORD DestBase4Enable                       : 1;
        DWORD DestBase5Enable                       : 1;
        DWORD DestBase6Enable                       : 1;
        DWORD DestBase7Enable                       : 1;
        DWORD                                       : 7;
        DWORD VcActionEnable                        : 1;
        DWORD TcActionEnable                        : 1;
        DWORD PglbActionEnable                      : 1;
        DWORD                                       : 4;
        DWORD Status                                : 1;
    };
    DWORD dword;
} GPU_COHERSTATUS;

typedef union {
    struct {
        DWORD WaitCrtcPFlip                         : 1;
        DWORD WaitReCrtcVLine                       : 1;
        DWORD WaitFeCrtcVLine                       : 1;
        DWORD WaitCrtcVLine                         : 1;
        DWORD                                       : 4;
        DWORD WaitCpDmaIdle                         : 1;
        DWORD                                       : 1;
        DWORD WaitCmdFifo                           : 1;
        DWORD WaitOvFlip                            : 1;
        DWORD                                       : 3;
        DWORD WaitIdle                              : 1;
        DWORD                                       : 1;
        DWORD WaitIdleClean                         : 1;
        DWORD                                       : 2;
        DWORD CmdFifoEntries                        : 4;
        DWORD                                       : 8;
    };
    DWORD dword;
} GPU_WAITUNTIL;

typedef union {
    struct {
        DWORD Cpu0Ack                               : 1;
        DWORD Cpu1Ack                               : 1;
        DWORD Cpu2Ack                               : 1;
        DWORD Cpu3Ack                               : 1;
        DWORD Cpu4Ack                               : 1;
        DWORD Cpu5Ack                               : 1;
        DWORD                                       : 26;
    };
    DWORD dword;
} GPU_CPUINTERRUPTACK;

typedef union {
    struct {
        DWORD Dynamic                               : 1;
        DWORD                                       : 3;
        DWORD PixelSize                             : 7;
        DWORD                                       : 1;
        DWORD VertexSize                            : 7;
        DWORD                                       : 13;
    };
    DWORD dword;
} GPU_GPRMANAGEMENT;

//------------------------------------------------------------------------------

// Packet 0:

typedef struct
{
    GPU_SURFACEINFO             SurfaceInfo;            // RB_SURFACE_INFO
    GPU_COLORINFO               Color0Info;             // RB_COLOR0_INFO
    GPU_DEPTHINFO               DepthInfo;              // RB_DEPTH_INFO
    GPU_COLORINFO               Color1Info;             // RB_COLOR1_INFO
    GPU_COLORINFO               Color2Info;             // RB_COLOR2_INFO
    GPU_COLORINFO               Color3Info;             // RB_COLOR3_INFO
    DWORD                       CoherDestBase0;         // COHER_DEST_BASE_0
    DWORD                       CoherDestBase1;         // COHER_DEST_BASE_1

    DWORD                       CoherDestBase2;         // COHER_DEST_BASE_2
    DWORD                       CoherDestBase3;         // COHER_DEST_BASE_3
    DWORD                       CoherDestBase4;         // COHER_DEST_BASE_4
    DWORD                       CoherDestBase5;         // COHER_DEST_BASE_5
    DWORD                       CoherDestBase6;         // COHER_DEST_BASE_6
    DWORD                       CoherDestBase7;         // COHER_DEST_BASE_7
    GPU_POINT                   ScreenScissorTL;        // PA_SC_SCREEN_SCISSOR_TL
    GPU_POINT                   ScreenScissorBR;        // PA_SC_SCREEN_SCISSOR_BR
} GPU_DESTINATIONPACKET;

// Packet 1:

typedef struct
{
    GPU_POINT                   WindowOffset;           // PA_SC_WINDOW_OFFSET
    GPU_POINT                   WindowScissorTL;        // PA_SC_WINDOW_SCISSOR_TL
    GPU_POINT                   WindowScissorBR;        // PA_SC_WINDOW_SCISSOR_BR
} GPU_WINDOWPACKET;

// Packet 2:

typedef struct
{
    DWORD                       MaxVtxIndx;             // VGT_MAX_VTX_INDX
    DWORD                       MinVtxIndx;             // VGT_MIN_VTX_INDX
    DWORD                       IndxOffset;             // VGT_INDX_OFFSET
    DWORD                       MultiPrimIbResetIndx;   // VGT_MULTI_PRIM_IB_RESET_INDX
    GPU_COLORMASK               ColorMask;              // RB_COLOR_MASK
    float                       BlendRed;               // RB_BLEND_RED
    float                       BlendGreen;             // RB_BLEND_GREEN
    float                       BlendBlue;              // RB_BLEND_BLUE

    float                       BlendAlpha;             // RB_BLEND_ALPHA
    DWORD                       Unused[3];
    GPU_STENCILREFMASK          StencilRefMaskBF;       // RB_STENCILREFMASK_BF
    GPU_STENCILREFMASK          StencilRefMask;         // RB_STENCILREFMASK
    float                       AlphaRef;               // RB_ALPHA_REF
    float                       VportXScale;            // PA_CL_VPORT_XSCALE

    float                       VportXOffset;           // PA_CL_VPORT_XOFFSET
    float                       VportYScale;            // PA_CL_VPORT_YSCALE
    float                       VportYOffset;           // PA_CL_VPORT_YOFFSET
    float                       VportZScale;            // PA_CL_VPORT_ZSCALE
    float                       VportZOffset;           // PA_CL_VPORT_ZOFFSET
} GPU_VALUESPACKET;

// Packet 3:

typedef struct
{
    GPU_PROGRAMCONTROL          ProgramControl;         // SQ_PROGRAM_CNTL
    GPU_CONTEXTMISC             ContextMisc;            // SQ_CONTEXT_MISC
    GPU_INTERPOLATORCONTROL     InterpolatorControl;    // SQ_INTERPOLATOR_CNTL
    GPU_WRAPPING0               Wrapping0;              // SQ_WRAPPING_0
    GPU_WRAPPING1               Wrapping1;              // SQ_WRAPPING_1
} GPU_PROGRAMPACKET;

// Packet 4:

typedef struct
{
    GPU_DEPTHCONTROL            DepthControl;           // RB_DEPTHCONTROL
    GPU_BLENDCONTROL            BlendControl0;          // RB_BLENDCONTROL0
    GPU_COLORCONTROL            ColorControl;           // RB_COLORCONTROL
    GPU_HICONTROL               HiControl;              // RB_TILECONTROL
    GPU_CLIPCONTROL             ClipControl;            // PA_CL_CLIP_CNTL
    GPU_MODECONTROL             ModeControl;            // PA_SU_SC_MODE_CNTL
    GPU_VTECONTROL              VteControl;             // PA_CL_VTE_CNTL
    DWORD                       Unused;

    GPU_EDRAMMODECONTROL        EdramModeControl;       // RB_MODECONTROL
    GPU_BLENDCONTROL            BlendControl1;          // RB_BLENDCONTROL1
    GPU_BLENDCONTROL            BlendControl2;          // RB_BLENDCONTROL2
    GPU_BLENDCONTROL            BlendControl3;          // RB_BLENDCONTROL3
} GPU_CONTROLPACKET;

// Packet 5:

typedef struct
{
    GPU_POINTSIZE               PointSize;              // PA_SU_POINT_SIZE
    GPU_POINTMINMAX             PointMinMax;            // PA_SU_POINT_MINMAX
    GPU_LINECONTROL             LineControl;            // PA_SU_LINE_CNTL
    DWORD                       Unused1;
    GPU_OUTPUTPATHCONTROL       OutputPathControl;      // VGT_OUTPUT_PATH_CNTL
    GPU_HOSCONTROL              HosControl;             // VGT_HOS_CNTL
    float                       HosMaxTessLevel;        // VGT_HOS_MAX_TESS_LEVEL
    float                       HosMinTessLevel;        // VGT_HOS_MIN_TESS_LEVEL

    DWORD                       HosReuseDepth;          // VGT_HOS_REUSE_DEPTH
    GPU_GROUPPRIMTYPE           GroupPrimType;          // VGT_GROUP_PRIM_TYPE
    DWORD                       GroupFirstDecr;         // VGT_GROUP_FIRST_DECR
    DWORD                       GroupDecr;              // VGT_GROUP_DECR
    GPU_GROUPVECTCONTROL        GroupVect0Control;      // VGT_GROUP_VECT_0_CNTL
    GPU_GROUPVECTCONTROL        GroupVect1Control;      // VGT_GROUP_VECT_1_CNTL
    GPU_GROUPVECTFMTCONTROL     GroupVect0FmtControl;   // VGT_GROUP_VECT_0_FMT_CNTL
    GPU_GROUPVECTFMTCONTROL     GroupVect1FmtControl;   // VGT_GROUP_VECT_1_FMT_CNTL

    DWORD                       Unused2[2];
    GPU_MPASSPSCONTROL          MPassPsControl;         // PA_SC_MPASS_PS_CNTL
    GPU_VIZQUERY                VizQuery;               // PA_SC_VIZ_QUERY
    GPU_ENHANCE                 Enhance;                // VGT_ENHANCE
} GPU_TESSELLATORPACKET;

// Packet 6:

typedef struct
{
    GPU_SCLINECONTROL           ScLineControl;          // PA_SC_LINE_CNTL
    GPU_AACONFIG                AaConfig;               // PA_SC_AA_CONFIG
    GPU_VTXCONTROL              VtxControl;             // PA_SU_VTX_CNTL
    float                       GbVertClipAdj;          // PA_CL_GB_VERT_CLIP_ADJ
    float                       GbVertDiscAdj;          // PA_CL_GB_VERT_DISC_ADJ
    float                       GbHorzClipAdj;          // PA_CL_GB_HORZ_CLIP_ADJ
    float                       GbHorzDiscAdj;          // PA_CL_GB_HORZ_DISC_ADJ
    GPU_CONST                   VsConst;                // SQ_VS_CONST

    GPU_CONST                   PsConst;                // SQ_PS_CONST
    GPU_DEBUGMISC0              DebugMisc0;             // SQ_DEBUG_MISC_0
    GPU_DEBUGMISC1              DebugMisc1;             // SQ_DEBUG_MISC_1
    DWORD                       Unused1[5];

    DWORD                       Unused2[2];
    DWORD                       AaMask;                 // PA_SC_AA_MASK
    DWORD                       Unused3[3];
    GPU_VERTEXREUSEBLOCKCONTROL VertexReuseBlockControl;// VGT_VERTEX_REUSE_BLOCK_CNTL
    GPU_OUTDEALLOCCONTROL       OutDeallocControl;      // VGT_OUT_DEALLOC_CNTL

    GPU_COPYCONTROL             CopyControl;            // RB_COPY_CONTROL
    DWORD                       CopyDestBase;           // RB_COPY_DEST_BASE
    GPU_COPYDESTPITCH           CopyDestPitch;          // RB_COPY_DEST_PITCH
    GPU_COPYDESTINFO            CopyDestInfo;           // RB_COPY_DEST_INFO
    DWORD                       HiClear;                // RB_TILE_CLEAR
    DWORD                       DepthClear;             // RB_DEPTH_CLEAR
    DWORD                       ColorClear;             // RB_COLOR_CLEAR
    DWORD                       ColorClearLo;           // RB_COLOR_CLEAR_LO

    GPU_COPYFUNC                CopyFunc;               // RB_COPY_FUNC
    GPU_COPYREF                 CopyRef;                // RB_COPY_REF
    GPU_COPYMASK                CopyMask;               // RB_COPY_MASK
    DWORD                       CopySurfaceSlice;       // RB_COPY_SURFACE_SLICE
    GPU_SAMPLECOUNTCONTROL      SampleCountControl;     // RB_SAMPLE_COUNT_CTL
    DWORD                       SampleCountAddress;     // RB_SAMPLE_COUNT_ADDR
} GPU_MISCPACKET;

// Packet 7:

typedef struct
{
    float                       PolyOffsetFrontScale;   // PA_SU_POLY_OFFSET_FRONT_SCALE
    float                       PolyOffsetFrontOffset;  // PA_SU_POLY_OFFSET_FRONT_OFFSET
    float                       PolyOffsetBackScale;    // PA_SU_POLY_OFFSET_BACK_SCALE
    float                       PolyOffsetBackOffset;   // PA_SU_POLY_OFFSET_BACK_OFFSET
    float                       PointXRad;              // PA_CL_POINT_X_RAD
    float                       PointYRad;              // PA_CL_POINT_Y_RAD
    float                       PointConstantSize;      // PA_CL_POINT_SIZE
    float                       PointCullRad;           // PA_CL_POINT_CULL_RAD
} GPU_POINTPACKET;

//------------------------------------------------------------------------------

// Number of packets:

#define GPU_PACKET_COUNT                8

// Maximum size of every packet, in registers:

#define GPU_PACKET_SIZE                 0x80

// Actual size of each packet:

#define GPU_DESTINATIONPACKET_SIZE      (sizeof(GPU_DESTINATIONPACKET) / sizeof(DWORD))
#define GPU_WINDOWPACKET_SIZE           (sizeof(GPU_WINDOWPACKET) / sizeof(DWORD))
#define GPU_VALUESPACKET_SIZE           (sizeof(GPU_VALUESPACKET) / sizeof(DWORD))
#define GPU_PROGRAMPACKET_SIZE          (sizeof(GPU_PROGRAMPACKET) / sizeof(DWORD))
#define GPU_CONTROLPACKET_SIZE          (sizeof(GPU_CONTROLPACKET) / sizeof(DWORD))
#define GPU_TESSELLATORPACKET_SIZE      (sizeof(GPU_TESSELLATORPACKET) / sizeof(DWORD))
#define GPU_MISCPACKET_SIZE             (sizeof(GPU_MISCPACKET) / sizeof(DWORD))
#define GPU_POINTPACKET_SIZE            (sizeof(GPU_POINTPACKET) / sizeof(DWORD))

//------------------------------------------------------------------------------

typedef enum
{
    GPUREG_WAITUNTIL                    = 0x05C8,       // GPU_WAITUNTIL

    GPUREG_COHERSIZEHOST                = 0x0A2F,       // DWORD
    GPUREG_COHERBASEHOST,                               // DWORD
    GPUREG_COHERSTATUSHOST,                             // GPU_COHERSTATUS

    GPUREG_CPUINTERRUPTACK              = 0x045E,       // GPU_CPUINTERRUPTACK

    GPUREG_GPRMANAGEMENT                = 0x0D00,       // GPU_GPRMANAGEMENT

    GPUREG_INVALIDATECONSTANTS          = 0x0D04,       // DWORD

    GPUREG_BCCONTROL                    = 0x0F01,       // GPU_BCCONTROL

    // Packet 0:

    GPUREG_DESTINATIONPACKET            = 0x2000,       // GPU_DESTINATIONPACKET
    GPUREG_PACKET0                      = 0x2000,

    GPUREG_SURFACEINFO                  = 0x2000,       // GPU_SURFACEINFO
    GPUREG_COLOR0INFO,                                  // GPU_COLORINFO
    GPUREG_DEPTHINFO,                                   // GPU_DEPTHINFO
    GPUREG_COLOR1INFO,                                  // GPU_COLORINFO
    GPUREG_COLOR2INFO,                                  // GPU_COLORINFO
    GPUREG_COLOR3INFO,                                  // GPU_COLORINFO
    GPUREG_COHERDESTBASE0,                              // DWORD
    GPUREG_COHERDESTBASE1,                              // DWORD

    GPUREG_COHERDESTBASE2,                              // DWORD
    GPUREG_COHERDESTBASE3,                              // DWORD
    GPUREG_COHERDESTBASE4,                              // DWORD
    GPUREG_COHERDESTBASE5,                              // DWORD
    GPUREG_COHERDESTBASE6,                              // DWORD
    GPUREG_COHERDESTBASE7,                              // DWORD
    GPUREG_SCREENSCISSORTL,                             // GPU_POINT
    GPUREG_SCREENSCISSORBR,                             // GPU_POINT

    // Packet 1:

    GPUREG_WINDOWPACKET                 = 0x2080,       // GPU_WINDOWPACKET
    GPUREG_PACKET1                      = 0x2080,

    GPUREG_WINDOWOFFSET                 = 0x2080,       // GPU_POINT
    GPUREG_WINDOWSCISSORTL,                             // GPU_POINT
    GPUREG_WINDOWSCISSORBR,                             // GPU_POINT

    // Packet 2:

    GPUREG_VALUESPACKET                 = 0x2100,       // GPU_VALUESPACKET
    GPUREG_PACKET2                      = 0x2100,

    GPUREG_MAXVTXINDX                   = 0x2100,       // DWORD
    GPUREG_MINVTXINDX,                                  // DWORD
    GPUREG_INDXOFFSET,                                  // DWORD
    GPUREG_MULTIPRIMIBRESETINDX,                        // DWORD
    GPUREG_COLORMASK,                                   // GPUREG_COLORMASK
    GPUREG_BLENDRED,                                    // float
    GPUREG_BLENDGREEN,                                  // float
    GPUREG_BLENDBLUE,                                   // float

    GPUREG_BLENDALPHA,                                  // float
    GPUREG_UNUSED0,
    GPUREG_UNUSED1,
    GPUREG_UNUSED2,
    GPUREG_STENCILREFMASKBF,                            // GPU_STENCILREFMASK
    GPUREG_STENCILREFMASK,                              // GPU_STENCILREFMASK
    GPUREG_ALPHAREF,                                    // float
    GPUREG_VPORTXSCALE,                                 // float

    GPUREG_VPORTXOFFSET,                                // float
    GPUREG_VPORTYSCALE,                                 // float
    GPUREG_VPORTYOFFSET,                                // float
    GPUREG_VPORTZSCALE,                                 // float
    GPUREG_VPORTZOFFSET,                                // float

    // Packet 3:

    GPUREG_PROGRAMPACKET                = 0x2180,       // GPU_PROGRAMPACKET
    GPUREG_PACKET3                      = 0x2180,

    GPUREG_PROGRAMCONTROL               = 0x2180,       // GPU_PROGRAMCONTROL
    GPUREG_CONTEXTMISC,                                 // GPU_CONTEXTMISC
    GPUREG_INTERPOLATORCONTROL,                         // GPU_INTERPOLATORCONTROL
    GPUREG_WRAPPING0,                                   // GPU_WRAPPING0
    GPUREG_WRAPPING1,                                   // GPU_WRAPPING1

    // Packet 4:

    GPUREG_CONTROLPACKET                = 0x2200,       // GPU_CONTROLPACKET
    GPUREG_PACKET4                      = 0x2200,

    GPUREG_DEPTHCONTROL                 = 0x2200,       // GPU_DEPTHCONTROL
    GPUREG_BLENDCONTROL0,                               // GPU_BLENDCONTROL
    GPUREG_COLORCONTROL,                                // GPU_COLORCONTROL
    GPUREG_HICONTROL,                                   // GPU_HICONTROL
    GPUREG_CLIPCONTROL,                                 // GPU_CLIPCONTROL
    GPUREG_MODECONTROL,                                 // GPU_MODECONTROL
    GPUREG_VTECONTROL,                                  // GPU_VTECONTROL
    GPUREG_UNUSED3,

    GPUREG_EDRAMMODECONTROL,                            // GPU_EDRAMMODECONTROL
    GPUREG_BLENDCONTROL1,                               // GPU_BLENDCONTROL
    GPUREG_BLENDCONTROL2,                               // GPU_BLENDCONTROL
    GPUREG_BLENDCONTROL3,                               // GPU_BLENDCONTROL

    // Packet 5:

    GPUREG_TESSELLATORPACKET            = 0x2280,       // GPU_TESSELLATORPACKET
    GPUREG_PACKET5                      = 0x2280,

    GPUREG_POINTSIZE                    = 0x2280,       // GPU_POINTSIZE
    GPUREG_POINTMINMAX,                                 // GPU_POINTMINMAX
    GPUREG_LINECONTROL,                                 // GPU_LINECONTROL
    GPUREG_UNUSED4,
    GPUREG_OUTPUTPATHCONTROL,                           // GPU_OUTPUTPATHCONTROL
    GPUREG_HOSCONTROL,                                  // GPU_HOSCONTROL
    GPUREG_HOSMAXTESSLEVEL,                             // float
    GPUREG_HOSMINTESSLEVEL,                             // float

    GPUREG_HOSREUSEDEPTH,                               // DWORD
    GPUREG_GROUPPRIMTYPE,                               // GPU_GROUPPRIMTYPE
    GPUREG_GROUPFIRSTDECR,                              // DWORD
    GPUREG_GROUPDECR,                                   // DWORD
    GPUREG_GROUPVECT0CONTROL,                           // GPU_GROUPVECTCONTROL
    GPUREG_GROUPVECT1CONTROL,                           // GPU_GROUPVECTCONTROL
    GPUREG_GROUPVECT0FMTCONTROL,                        // GPU_GROUPVECTFMTCONTROL
    GPUREG_GROUPVECT1FMTCONTROL,                        // GPU_GROUPVECTFMTCONTROL

    GPUREG_UNUSED5,
    GPUREG_UNUSED6,
    GPUREG_MPASSPSCONTROL,                              // GPU_MPASSPSCONTROL
    GPUREG_VIZQUERY,                                    // GPU_VIZQUERY
    GPUREG_ENHANCE,                                     // GPU_ENHANCE

    // Packet 6:

    GPUREG_MISCPACKET                   = 0x2300,       // GPU_MISCPACKET
    GPUREG_PACKET6                      = 0x2300,

    GPUREG_SCLINECONTROL                = 0x2300,       // GPU_SCLINECONTROL
    GPUREG_AACONFIG,                                    // GPU_AACONFIG
    GPUREG_VTXCONTROL,                                  // GPU_VTXCONTROL
    GPUREG_GBVERTCLIPADJ,                               // float
    GPUREG_GBVERTDISCADJ,                               // float
    GPUREG_GBHORZCLIPADJ,                               // float
    GPUREG_GBHORZDISCADJ,                               // float
    GPUREG_VSCONST,                                     // GPU_CONST

    GPUREG_PSCONST,                                     // GPU_CONST
    GPUREG_DEBUGMISC0,                                  // GPU_DEBUGMISC0
    GPUREG_DEBUGMISC1,                                  // GPU_DEBUGMISC1
    GPUREG_UNUSED7,
    GPUREG_UNUSED8,
    GPUREG_UNUSED9,
    GPUREG_UNUSED10,
    GPUREG_UNUSED11,

    GPUREG_UNUSED12,
    GPUREG_UNUSED13,
    GPUREG_AAMASK,                                      // DWORD
    GPUREG_UNUSED14,
    GPUREG_UNUSED15,
    GPUREG_UNUSED16,
    GPUREG_VERTEXREUSEBLOCKCONTROL,                     // GPU_VERTEXREUSEBLOCKCONTROL
    GPUREG_OUTDEALLOCCONTROL,                           // GPU_OUTDEALLOCCONTROL

    GPUREG_COPYCONTROL,                                 // GPU_COPYCONTROL
    GPUREG_COPYDESTBASE,                                // GPU_COPYDESTBASE
    GPUREG_COPYDESTPITCH,                               // GPU_COPYDESTPITCH
    GPUREG_COPYDESTINFO,                                // GPU_COPYDESTINFO
    GPUREG_HICLEAR,                                     // DWORD
    GPUREG_DEPTHCLEAR,                                  // DWORD
    GPUREG_COLORCLEAR,                                  // DWORD
    GPUREG_COLORCLEARLO,                                // DWORD

    GPUREG_COPYFUNC,                                    // GPU_COPYFUNC
    GPUREG_COPYREF,                                     // GPU_COPYREF
    GPUREG_COPYMASK,                                    // GPU_COPYMASK
    GPUREG_COPYSURFACESLICE,                            // DWORD
    GPUREG_SAMPLECOUNTCONTROL,                          // GPU_SAMPLECOUNTCONTROL
    GPUREG_SAMPLECOUNTADDRESS,                          // DWORD

    // Packet 7:

    GPUREG_POINTPACKET                  = 0x2380,       // GPU_POINTPACKET
    GPUREG_PACKET7                      = 0x2380,

    GPUREG_POLYOFFSETFRONTSCALE         = 0x2380,       // float
    GPUREG_POLYOFFSETFRONTOFFSET,                       // float
    GPUREG_POLYOFFSETBACKSCALE,                         // float
    GPUREG_POLYOFFSETBACKOFFSET,                        // float
    GPUREG_POINTXRAD,                                   // float
    GPUREG_POINTYRAD,                                   // float
    GPUREG_POINTCONSTANTSIZE,                           // float
    GPUREG_POINTCULLRAD,                                // float

    // Other:

    GPUREG_USERCLIPPLANE0               = 0x2388,       // float[4]
    GPUREG_USERCLIPPLANE1               = 0x238C,       // float[4]
    GPUREG_USERCLIPPLANE2               = 0x2390,       // float[4]
    GPUREG_USERCLIPPLANE3               = 0x2394,       // float[4]
    GPUREG_USERCLIPPLANE4               = 0x2398,       // float[4]
    GPUREG_USERCLIPPLANE5               = 0x239C,       // float[4]

    GPUREG_ALUCONSTANTS                 = 0x4000,       // float[GPU_ALU_CONSTANTS][4]

    GPUREG_FETCHCONSTANTS               = 0x4800,       // GPUFETCH_CONSTANT[GPU_FETCH_CONSTANTS]

    GPUREG_FLOWCONSTANTS                = 0x4900,       // DWORD[GPU_FLOW_CONSTANTS]
    GPUREG_BOOLEANCONSTANTS             = 0x4900,       // DWORD[GPU_BOOLEAN_CONSTANTS/32]
    GPUREG_INTEGERCONSTANTS             = 0x4908,       // DWORD[GPU_INTEGER_CONSTANTS]

} GPUREGISTER;

// When using GPUCOMMANDOP_SET_CONSTANT to set a register, subtract this value
// from the register address:

#define GPU_SET_CONSTANT_REGISTER_OFFSET  0x2000

//------------------------------------------------------------------------------

typedef struct
{
    DWORD Width                         : 24;   // DWORD
    DWORD                               : 8;
} GPUTEXTURESIZE_1D;

typedef struct
{
    DWORD Width                         : 13;   // DWORD
    DWORD Height                        : 13;   // DWORD
    DWORD                               : 6;
} GPUTEXTURESIZE_2D;

typedef struct
{
    DWORD Width                         : 13;   // DWORD
    DWORD Height                        : 13;   // DWORD
    DWORD Depth                         : 6;    // DWORD
} GPUTEXTURESIZE_STACK;

typedef struct
{
    DWORD Width                         : 11;   // DWORD
    DWORD Height                        : 11;   // DWORD
    DWORD Depth                         : 10;   // DWORD
} GPUTEXTURESIZE_3D;

typedef union {
    struct {
        // DWORD 0:

        DWORD Type                      : 2;    // GPUCONSTANTTYPE
        DWORD SignX                     : 2;    // GPUSIGN
        DWORD SignY                     : 2;    // GPUSIGN
        DWORD SignZ                     : 2;    // GPUSIGN
        DWORD SignW                     : 2;    // GPUSIGN
        DWORD ClampX                    : 3;    // GPUCLAMP
        DWORD ClampY                    : 3;    // GPUCLAMP
        DWORD ClampZ                    : 3;    // GPUCLAMP
        DWORD                           : 2;
        DWORD                           : 1;
        DWORD Pitch                     : 9;    // DWORD
        DWORD Tiled                     : 1;    // BOOL

        // DWORD 1:

        DWORD DataFormat                : 6;    // GPUTEXTUREFORMAT
        DWORD Endian                    : 2;    // GPUENDIAN
        DWORD RequestSize               : 2;    // GPUREQUESTSIZE
        DWORD Stacked                   : 1;    // BOOL
        DWORD ClampPolicy               : 1;    // GPUCLAMPPOLICY
        DWORD BaseAddress               : 20;   // DWORD

        // DWORD 2:

        union
        {
            GPUTEXTURESIZE_1D OneD;
            GPUTEXTURESIZE_2D TwoD;
            GPUTEXTURESIZE_3D ThreeD;
            GPUTEXTURESIZE_STACK Stack;
        } Size;

        // DWORD 3:

        DWORD NumFormat                 : 1;    // GPUNUMFORMAT
        DWORD SwizzleX                  : 3;    // GPUSWIZZLE
        DWORD SwizzleY                  : 3;    // GPUSWIZZLE
        DWORD SwizzleZ                  : 3;    // GPUSWIZZLE
        DWORD SwizzleW                  : 3;    // GPUSWIZZLE
        INT   ExpAdjust                 : 6;    // int
        DWORD MagFilter                 : 2;    // GPUMINMAGFILTER
        DWORD MinFilter                 : 2;    // GPUMINMAGFILTER
        DWORD MipFilter                 : 2;    // GPUMIPFILTER
        DWORD AnisoFilter               : 3;    // GPUANISOFILTER
        DWORD                           : 3;
        DWORD BorderSize                : 1;    // DWORD

        // DWORD 4:

        DWORD VolMagFilter              : 1;    // GPUMINMAGFILTER
        DWORD VolMinFilter              : 1;    // GPUMINMAGFILTER
        DWORD MinMipLevel               : 4;    // DWORD
        DWORD MaxMipLevel               : 4;    // DWORD
        DWORD MagAnisoWalk              : 1;    // BOOL
        DWORD MinAnisoWalk              : 1;    // BOOL
        INT   LODBias                   : 10;   // int
        INT   GradExpAdjustH            : 5;    // int
        INT   GradExpAdjustV            : 5;    // int

        // DWORD 5:

        DWORD BorderColor               : 2;    // GPUBORDERCOLOR
        DWORD ForceBCWToMax             : 1;    // BOOL
        DWORD TriClamp                  : 2;    // GPUTRICLAMP
        INT   AnisoBias                 : 4;    // int
        DWORD Dimension                 : 2;    // GPUDIMENSION
        DWORD PackedMips                : 1;    // BOOL
        DWORD MipAddress                : 20;   // DWORD
    };
    DWORD dword[6];
} GPUTEXTURE_FETCH_CONSTANT;

typedef union {
    struct {
        // DWORD 0:

        DWORD Type                      : 2;    // GPUCONSTANTTYPE
        DWORD BaseAddress               : 30;   // DWORD

        // DWORD 1:

        DWORD Endian                    : 2;    // GPUENDIAN
        DWORD Size                      : 24;   // DWORD
        DWORD AddressClamp              : 1;    // GPUADDRESSCLAMP
        DWORD                           : 1;
        DWORD RequestSize               : 2;    // GPUREQUESTSIZE
        DWORD ClampDisable              : 2;    // BOOL
    };
    DWORD dword[2];
} GPUVERTEX_FETCH_CONSTANT;

typedef union {
    GPUTEXTURE_FETCH_CONSTANT           Texture;
    GPUVERTEX_FETCH_CONSTANT            Vertex[3];
} GPUFETCH_CONSTANT;

typedef union {
    struct {
        // DWORD 0:

        DWORD Op                        : 5;    // GPUTEXTUREFETCHOP
        DWORD SrcGPR                    : 6;    // DWORD
        DWORD SrcLoopIndexRelative      : 1;    // BOOL
        DWORD DestGPR                   : 6;    // DWORD
        DWORD DestLoopIndexRelative     : 1;    // BOOL
        DWORD FetchValidOnly            : 1;    // BOOL
        DWORD ConstIndex                : 5;    // DWORD
        DWORD UnnormalizedTextureCoords : 1;    // BOOL
        DWORD SrcSelectX                : 2;    // DWORD
        DWORD SrcSelectY                : 2;    // DWORD
        DWORD SrcSelectZ                : 2;    // DWORD

        // DWORD 1:

        DWORD SwizzleX                  : 3;    // GPUSWIZZLE
        DWORD SwizzleY                  : 3;    // GPUSWIZZLE
        DWORD SwizzleZ                  : 3;    // GPUSWIZZLE
        DWORD SwizzleW                  : 3;    // GPUSWIZZLE
        DWORD MagFilter                 : 2;    // GPUMINMAGFILTER
        DWORD MinFilter                 : 2;    // GPUMINMAGFILTER
        DWORD MipFilter                 : 2;    // GPUMIPFILTER
        DWORD AnisoFilter               : 3;    // GPUANISOFILTER
        DWORD                           : 3;
        DWORD VolMagFilter              : 2;    // GPUMINMAGFILTER
        DWORD VolMinFilter              : 2;    // GPUMINMAGFILTER
        DWORD UseComputedLOD            : 1;    // BOOL
        DWORD UseRegisterLOD            : 1;    // BOOL
        DWORD                           : 1;
        DWORD Predicated                : 1;    // BOOL

        // DWORD 2:

        DWORD UseRegisterGradients      : 1;    // BOOL
        DWORD                           : 1;
        INT   LODBias                   : 7;    // int
        DWORD                           : 5;
        DWORD Dimension                 : 2;    // GPUDIMENSION (pseudo register)
        INT   OffsetX                   : 5;    // int
        INT   OffsetY                   : 5;    // int
        INT   OffsetZ                   : 5;    // int
        DWORD PredicationCondition      : 1;    // DWORD
    };
    DWORD dword[3];
} GPUTEXTURE_FETCH_INSTRUCTION;

typedef union {
    struct {
        // DWORD 0:

        DWORD Op                        : 5;    // GPUVERTEXFETCHOP
        DWORD SrcGPR                    : 6;    // DWORD
        DWORD SrcLoopIndexRelative      : 1;    // BOOL
        DWORD DestGPR                   : 6;    // DWORD
        DWORD DestLoopIndexRelative     : 1;    // BOOL
        DWORD FetchValidOnly            : 1;    // Must be 1
        DWORD ConstIndex                : 5;    // DWORD
        DWORD ConstIndexSelect          : 2;    // DWORD
        DWORD PrefetchCount             : 3;    // DWORD
        DWORD SrcSelect                 : 2;    // DWORD

        // DWORD 1:

        DWORD SwizzleX                  : 3;    // GPUSWIZZLE
        DWORD SwizzleY                  : 3;    // GPUSWIZZLE
        DWORD SwizzleZ                  : 3;    // GPUSWIZZLE
        DWORD SwizzleW                  : 3;    // GPUSWIZZLE
        DWORD Signed                    : 1;    // BOOL
        DWORD NumFormat                 : 1;    // GPUNUMFORMAT
        DWORD                           : 1;
        DWORD RoundIndex                : 1;    // BOOL
        DWORD DataFormat                : 6;    // GPUVERTEXFORMAT
        DWORD                           : 2;
        INT   ExpAdjust                 : 6;    // int
        DWORD MiniFetch                 : 1;    // BOOL
        DWORD Predicated                : 1;    // BOOL

        // DWORD 2:

        DWORD Stride                    : 8;    // DWORD
        INT   Offset                    : 23;   // int
        DWORD PredicationCondition      : 1;    // DWORD
    };
    DWORD dword[3];
} GPUVERTEX_FETCH_INSTRUCTION;

#ifdef __cplusplus

//------------------------------------------------------------------------------------------------
// Convert logical vertex stream number to hardware format

_forceinline
void SetVertexStream(GPUVERTEX_FETCH_INSTRUCTION* pVertexFetch, DWORD stream)
{
    // Avoid an integer division by using fixed point math
    static const DWORD kRemainderAndDiv = ((DWORD) ((4.0 / 3.0) * 256 + 1)) << 16
        | (0xffff & (DWORD)((1.0 / 3.0) * 256 + 1));
    DWORD temp = ((GPU_VERTEX_FETCH_CONSTANTS - 1) - stream) * kRemainderAndDiv;
    pVertexFetch->ConstIndex = (temp >> 8) & 0xff;
    pVertexFetch->ConstIndexSelect = (temp >> 24) & 0x3;
}

//------------------------------------------------------------------------------------------------
// Convert hardware format to logical vertex stream

_forceinline
DWORD GetVertexStream(const GPUVERTEX_FETCH_INSTRUCTION* pVertexFetch)
{
    return (GPU_VERTEX_FETCH_CONSTANTS - 1) - (pVertexFetch->ConstIndex * 3 + pVertexFetch->ConstIndexSelect);
}

#endif

typedef union {
    struct {
        DWORD Address                   : 12;
        DWORD Count                     : 3;
        DWORD Yield                     : 1;
        DWORD TypeAndSerialize          : 12;   // GPUEXECSERIALIZEMODE, GPUEXECINSTRUCTIONTYPE
        DWORD VertexCacheLow            : 4;    // GPUEXECFETCHCACHETYPE

        DWORD VertexCacheHigh           : 2;    // GPUEXECFETCHCACHETYPE
        DWORD                           : 7;
        DWORD PredicateClean            : 1;
        DWORD                           : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_EXEC;

typedef union {
    struct {
        DWORD Address                   : 12;
        DWORD Count                     : 3;
        DWORD Yield                     : 1;
        DWORD TypeAndSerialize          : 12;
        DWORD VertexCacheLow            : 4;

        DWORD VertexCacheHigh           : 2;
        DWORD BooleanAddress            : 8;
        DWORD Condition                 : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_COND_EXEC;

typedef union {
    struct {
        DWORD Address                   : 12;
        DWORD Count                     : 3;
        DWORD Yield                     : 1;
        DWORD TypeAndSerialize          : 12;
        DWORD VertexCacheLow            : 4;

        DWORD VertexCacheHigh           : 2;
        DWORD                           : 7;
        DWORD PredicateClean            : 1;
        DWORD Condition                 : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_COND_EXEC_PREDS;

typedef union {
    struct {
        DWORD JumpAddress               : 13;
        DWORD Repeat                    : 1;
        DWORD                           : 2;
        DWORD LoopID                    : 5;
        DWORD                           : 11;

        DWORD                           : 11;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_LOOP_START;

typedef union {
    struct {
        DWORD JumpAddress               : 13;
        DWORD                           : 3;
        DWORD LoopID                    : 5;
        DWORD PredicateBreak            : 1;
        DWORD                           : 10;

        DWORD                           : 10;
        DWORD Condition                 : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_LOOP_END;

typedef union {
    struct {
        DWORD JumpAddress               : 13;
        DWORD Force                     : 1;
        DWORD Predicated                : 1;
        DWORD                           : 17;

        DWORD                           : 2;
        DWORD BooleanAddress            : 8;
        DWORD Condition                 : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_COND_CALL;

typedef union {
    struct {
        DWORD                           : 32;

        DWORD                           : 11;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_RETURN;

typedef union {
    struct {
        DWORD JumpAddress               : 13;
        DWORD Force                     : 1;
        DWORD Predicated                : 1;
        DWORD                           : 17;

        DWORD                           : 1;
        DWORD ForwardOnly               : 1;
        DWORD BooleanAddress            : 8;
        DWORD Condition                 : 1;
        DWORD AbsoluteAddressing        : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_COND_JUMP;

typedef union {
    struct {
        DWORD Size                      : 3;
        DWORD                           : 29;

        DWORD                           : 8;
        DWORD DoNotSerialize            : 1;
        DWORD BufferSelect              : 2;    // GPUALLOCBUFFERSELECT
        DWORD                           : 1;
        DWORD Op                        : 4;
    };
} GPUFLOW_ALLOC;

typedef union {
    struct {
        DWORD                           : 32;

        DWORD                           : 12;
        DWORD Op                        : 4;    // GPUFLOWOP
    };
    GPUFLOW_EXEC                        Exec;
    GPUFLOW_COND_EXEC                   CondExec;
    GPUFLOW_COND_EXEC_PREDS             CondExecPreds;
    GPUFLOW_LOOP_START                  LoopStart;
    GPUFLOW_LOOP_END                    LoopEnd;
    GPUFLOW_COND_CALL                   CondCall;
    GPUFLOW_RETURN                      Return;
    GPUFLOW_COND_JUMP                   CondJump;
    GPUFLOW_ALLOC                       Alloc;
    DWORD                               dword[2];
} GPUFLOW_INSTRUCTION;

// Use GPU_GET_FLOW_INSTRUCTIONS() to decode GPUFLOW_INSTRUCTION_PAIR:

typedef struct {
    DWORD                               dword[3];
} GPUFLOW_INSTRUCTION_PAIR;

typedef union {
    struct {
        // DWORD 0:

        DWORD VectorDest                : 6;
        DWORD VectorDestRelative        : 1;
        DWORD AbsConstants              : 1;
        DWORD ScalarDest                : 6;
        DWORD ScalarDestRelative        : 1;    // Also used as export mask
        DWORD ResultExport              : 1;
        DWORD VectorMask                : 4;
        DWORD ScalarMask                : 4;
        DWORD VectorSaturate            : 1;
        DWORD ScalarSaturate            : 1;
        DWORD ScalarOp                  : 6;    // GPUALUSCALAROP

        // DWORD 1:

        DWORD SrcCSwizzle               : 8;
        DWORD SrcBSwizzle               : 8;
        DWORD SrcASwizzle               : 8;
        DWORD SrcCNegate                : 1;
        DWORD SrcBNegate                : 1;
        DWORD SrcANegate                : 1;
        DWORD PredicationCondition      : 1;
        DWORD Predicated                : 1;
        DWORD RelativeAddress           : 1;
        DWORD Constant1Relative         : 1;
        DWORD Constant0Relative         : 1;

        // DWORD 2:

        DWORD SrcCRegConst              : 8;
        DWORD SrcBRegConst              : 8;
        DWORD SrcARegConst              : 8;
        DWORD VectorOp                  : 5;    // GPUALUVECTOROP
        DWORD SrcCSelect                : 1;    // GPUALUSRCSELECT
        DWORD SrcBSelect                : 1;    // GPUALUSRCSELECT
        DWORD SrcASelect                : 1;    // GPUALUSRCSELECT
    };
    DWORD dword[3];
} GPUALU_INSTRUCTION;

//------------------------------------------------------------------------------
// used for GPUALUSCALAROP_MULC0..GPUALUSCALAROP_SUBC1

static __forceinline
DWORD GPU_GET_SCALAROP_SOURCE2_REG(
    GPUALU_INSTRUCTION* pALU)
{
    return (pALU->SrcCSwizzle & 0x3C)
        | (pALU->SrcCSelect << 1)
        | (pALU->ScalarOp & 1);
}

static __forceinline
VOID GPU_SET_SCALAROP_SOURCE2_REG(
    GPUALU_INSTRUCTION* pALU,
    DWORD reg)
{
    pALU->SrcCSwizzle =
        (pALU->SrcCSwizzle & 0xC3)
        | (reg & 0x3C);
    pALU->SrcCSelect = reg >> 1;
    pALU->ScalarOp =
        (pALU->ScalarOp & 0xFE)
        | (reg & 1);
}

//------------------------------------------------------------------------------

typedef union {
    GPUTEXTURE_FETCH_INSTRUCTION        TextureFetch;
    GPUVERTEX_FETCH_INSTRUCTION         VertexFetch;
    GPUFLOW_INSTRUCTION                 Flow0;
    GPUFLOW_INSTRUCTION_PAIR            FlowPair;
    GPUALU_INSTRUCTION                  Alu;
} GPUSHADER_INSTRUCTION;

//------------------------------------------------------------------------------

static __forceinline
void GPU_GET_FLOW_INSTRUCTIONS(
    const GPUFLOW_INSTRUCTION_PAIR* pPair,
    GPUFLOW_INSTRUCTION* pFlow0,
    GPUFLOW_INSTRUCTION* pFlow1)
{
    pFlow0->dword[0] = pPair->dword[0];
    pFlow0->dword[1] = 0xffff & pPair->dword[1]; // Load low 16 bits.
    pFlow1->dword[0] = (pPair->dword[1] >> 16) | (pPair->dword[2] << 16);
    pFlow1->dword[1] = (pPair->dword[2] >> 16);
}

static __forceinline
void GPU_PUT_FLOW_INSTRUCTIONS(
    GPUFLOW_INSTRUCTION_PAIR* pPair,
    const GPUFLOW_INSTRUCTION* pFlow0,
    const GPUFLOW_INSTRUCTION* pFlow1)
{
    pPair->dword[0] = pFlow0->dword[0];
    pPair->dword[1] = (pFlow0->dword[1] & 0xffff) | (pFlow1->dword[0] << 16);
    pPair->dword[2] = (pFlow1->dword[0] >> 16) | (pFlow1->dword[1] << 16);
}

//------------------------------------------------------------------------------

typedef union {
    struct {
        // float x:

        DWORD BaseAddress                   : 30; // Physical address in DWORDs
        DWORD _01                           : 2;  // Must be 01

        // float y:

        DWORD _4B000000                     : 32; // Must be 0x4B000000

        // float z:

        DWORD EndianSwap                    : 3; // GPUENDIAN128
        DWORD                               : 5;
        DWORD Format                        : 6; // GPUCOLORFORMAT
        DWORD                               : 2;
        DWORD NumericType                   : 3; // GPUSURFACENUMBER
        DWORD ComponentSwap                 : 1; // GPUSURFACESWAP
        DWORD _4B0                          : 12; // Must be 0x4B0

        // float w:

        DWORD StreamMaxIndex                : 23; // 0..StreamMaxIndex - 1 is valid
        DWORD _96                           : 9;  // Must be 0x96
    };
    DWORD dword[4];
    float c[4];
} GPU_MEMEXPORT_STREAM_CONSTANT;

//------------------------------------------------------------------------------

#ifdef _DEBUG

void GPU_SET_MEMEXPORT_STREAM_CONSTANT(
    GPU_MEMEXPORT_STREAM_CONSTANT* pConstant,
    VOID* pBaseAddress,
    DWORD StreamMaxIndex,
    GPUSURFACESWAP ComponentSwap,
    GPUSURFACENUMBER NumericType,
    GPUCOLORFORMAT Format,
    GPUENDIAN128 EndianSwap);

#else

__forceinline
void GPU_SET_MEMEXPORT_STREAM_CONSTANT(
    GPU_MEMEXPORT_STREAM_CONSTANT* pConstant,
    VOID* pBaseAddress,
    DWORD StreamMaxIndex,
    GPUSURFACESWAP ComponentSwap,
    GPUSURFACENUMBER NumericType,
    GPUCOLORFORMAT Format,
    GPUENDIAN128 EndianSwap)
{
    pConstant->_01 = 0x01;
    pConstant->BaseAddress = GPU_CONVERT_CPU_TO_GPU_ADDRESS(pBaseAddress) >> 2;
    pConstant->_4B000000 = 0x4b000000;
    pConstant->_4B0 = 0x4b0;
    pConstant->ComponentSwap = ComponentSwap;
    pConstant->NumericType = NumericType;
    pConstant->Format = Format;
    pConstant->EndianSwap = EndianSwap;
    pConstant->_96 = 0x96;
    pConstant->StreamMaxIndex = StreamMaxIndex;
}

#endif // _DEBUG

//------------------------------------------------------------------------------

typedef union {
    struct {
        // DWORD 0:

        DWORD VizQueryId                    : 6;
        DWORD UseVizQuery                   : 1;
        DWORD                               : 25;

        // DWORD 1:

        DWORD PrimType                      : 6;    // GPUPRIMTYPE
        DWORD SrcSelect                     : 2;    // Must be GPUINDEXSELECT_DMA
        DWORD MajorMode                     : 3;
        DWORD IndexType                     : 1;    // GPUINDEXTYPE
        DWORD NotEndOfPacket                : 1;
        DWORD                               : 3;
        DWORD NumIndices                    : 16;

        // DWORD 2:

        DWORD IndexBase                     : 32;

        // DWORD 3:

        DWORD IndexSize                     : 24;
        DWORD                               : 6;
        DWORD Endian                        : 2;    // GPUENDIAN
    };
    DWORD dword[4];
} GPUCOMMAND_DRAW_INDEX;

typedef union {
    struct {
        // DWORD 0:

        DWORD VizQueryId                    : 6;
        DWORD UseVizQuery                   : 1;
        DWORD                               : 25;

        // DWORD 1:

        DWORD PrimType                      : 6;    // GPUPRIMTYPE
        DWORD SrcSelect                     : 2;    // Must be GPUINDEXSELECT_AUTO
        DWORD MajorMode                     : 3;
        DWORD IndexSize                     : 1;
        DWORD NotEndOfPacket                : 1;
        DWORD                               : 3;
        DWORD NumIndices                    : 16;
    };
    DWORD dword[2];
} GPUCOMMAND_DRAW_AUTO;

typedef union {
    struct {
        // DWORD 0:

        DWORD PrimType                      : 6;    // GPUPRIMTYPE
        DWORD SrcSelect                     : 2;    // Must be GPUINDEXSELECT_IMMEDIATE
        DWORD MajorMode                     : 3;
        DWORD IndexType                     : 1;    // GPUINDEXTYPE
        DWORD NotEndOfPacket                : 1;
        DWORD                               : 3;
        DWORD NumIndices                    : 16;

        // DWORD 1:

        union {
            WORD Index16[];
            DWORD Index32[];
        };
    };
    DWORD dword[1];
} GPUCOMMAND_DRAW_IMMEDIATE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Register                      : 13;
        DWORD                               : 17;
        DWORD OrImmediate                   : 1;
        DWORD AndImmediate                  : 1;

        // DWORD 1:

        union {
            struct {
                DWORD AndMask               : 32;
            };
            struct {
                DWORD AndRegister           : 13;
                DWORD                       : 19;
            };
        };

        // DWORD 2:

        union {
            struct {
                DWORD OrMask                : 32;
            };
            struct {
                DWORD OrRegister            : 13;
                DWORD                       : 19;
            };
        };
    };
    DWORD dword[3];
} GPUCOMMAND_REG_RMW;

typedef union {
    struct {
        // DWORD 0:

        DWORD DestinationSize               : 4;
        DWORD DestinationDisable            : 1;
        DWORD DestinationAddress            : 27;

        // DWORD 1:

        DWORD WindowSize                    : 4;
        DWORD WindowDisable                 : 1;
        DWORD WindowAddress                 : 27;

        // DWORD 2:

        DWORD ValuesSize                    : 4;
        DWORD ValuesDisable                 : 1;
        DWORD ValuesAddress                 : 27;

        // DWORD 3:

        DWORD ProgramSize                   : 4;
        DWORD ProgramDisable                : 1;
        DWORD ProgramAddress                : 27;

        // DWORD 4:

        DWORD ControlSize                   : 4;
        DWORD ControlDisable                : 1;
        DWORD ControlAddress                : 27;

        // DWORD 5:

        DWORD TessellatorSize               : 4;
        DWORD TessellatorDisable            : 1;
        DWORD TessellatorAddress            : 27;

        // DWORD 6:

        DWORD MiscSize                      : 4;
        DWORD MiscDisable                   : 1;
        DWORD MiscAddress                   : 27;

        // DWORD 7:

        DWORD PointSize                     : 4;
        DWORD PointDisable                  : 1;
        DWORD PointAddress                  : 27;

        // DWORD 8:

        DWORD                               : 4;
        DWORD VertexShaderDisable           : 1;
        DWORD VertexShaderAddress           : 27;

        // DWORD 9:

        DWORD VertexShaderSize              : 14;
        DWORD                               : 18;

        // DWORD 10:

        DWORD                               : 4;
        DWORD PixelShaderDisable            : 1;
        DWORD PixelShaderAddress            : 27;

        // DWORD 11:

        DWORD PixelShaderSize               : 14;
        DWORD                               : 18;
    };
    DWORD dword[10];
} GPUCOMMAND_SET_STATE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Offset                        : 11;
        DWORD                               : 5;
        DWORD Id                            : 8;    // GPUCONSTANTID
        DWORD                               : 8;

        // DWORD 1:

        DWORD Data[];
    };
    DWORD dword[1];
} GPUCOMMAND_SET_CONSTANT;

typedef union {
    struct {
        // DWORD 0:

        DWORD                               : 2;
        DWORD Address                       : 30;

        // DWORD 1:

        DWORD Offset                        : 11;
        DWORD                               : 21;

        // DWORD 2:

        DWORD Size                          : 12;
        DWORD                               : 20;
    };
    DWORD dword[3];
} GPUCOMMAND_LOAD_ALU_CONSTANT;

typedef union {
    struct {
        // DWORD 0:

        DWORD Type                          : 2;    // GPULOADTYPE
        DWORD                               : 3;
        DWORD Address                       : 27;

        // DWORD 1:

        DWORD Size                          : 14;
        DWORD                               : 2;
        DWORD Start                         : 12;   // Unused unless GPULOADTYPE_SHARED
        DWORD                               : 4;
    };
    DWORD dword[2];
} GPUCOMMAND_LOAD_SHADER;

typedef union {
    struct {
        DWORD Destination                   : 1;
        DWORD Window                        : 1;
        DWORD Values                        : 1;
        DWORD Program                       : 1;
        DWORD Control                       : 1;
        DWORD Tessellator                   : 1;
        DWORD Misc                          : 1;
        DWORD Point                         : 1;
        DWORD VertexShader                  : 1;
        DWORD PixelShader                   : 1;
        DWORD AluConstantBase               : 1;
        DWORD FetchConstantBase             : 1;
        DWORD IncrementalRegisterBase       : 1;
        DWORD BooleanBase                   : 1;
        DWORD IntegerBase                   : 1;
        DWORD                               : 17;
    };
    DWORD dword[1];
} GPUCOMMAND_INVALIDATE_STATE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Function                      : 3;    // GPUSYNCFUNCTION
        DWORD                               : 1;
        DWORD MemSpace                      : 1;    // GPUSYNCSPACE
        DWORD                               : 27;

        // DWORD 1:

        union {
            struct {
                DWORD Endian                : 2;    // GPUENDIAN
                DWORD Address               : 30;
            };
            struct {
                DWORD Register              : 15;
                DWORD                       : 17;
            };
        };

        // DWORD 2:

        DWORD Reference                     : 32;

        // DWORD 3:

        DWORD Mask                          : 32;

        // DWORD 4:

        DWORD WaitInterval                  : 16;
        DWORD                               : 16;
    };
    DWORD dword[5];
} GPUCOMMAND_WAIT_REG_MEM;

typedef union {
    struct {
        // DWORD 0:

        DWORD Register                      : 15;
        DWORD                               : 17;

        // DWORD 1:

        DWORD Reference                     : 32;

        // DWORD 2:

        DWORD Mask                          : 32;

        // DWORD 3:

        DWORD WaitInterval                  : 16;
        DWORD                               : 16;
    };
    DWORD dword[4];
} GPUCOMMAND_WAIT_REG_EQ;

typedef union {
    struct {
        // DWORD 0:

        DWORD Register                      : 15;
        DWORD                               : 17;

        // DWORD 1:

        DWORD Reference                     : 32;

        // DWORD 2:

        DWORD Mask                          : 32;

        // DWORD 3:

        DWORD WaitInterval                  : 16;
        DWORD                               : 16;
    };
    DWORD dword[4];
} GPUCOMMAND_WAIT_REG_GTE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;

        // DWORD 1:

        DWORD Data                          : 32;
    };
    DWORD dword[2];
} GPUCOMMAND_MEM_WRITE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Function                      : 3;    // GPUSYNCFUNCTION
        DWORD                               : 1;
        DWORD PollSpace                     : 1;    // GPUSYNCSPACE
        DWORD                               : 3;
        DWORD WriteSpace                    : 1;    // GPUSYNCSPACE
        DWORD                               : 23;

        // DWORD 1:

        union {
            struct {
                DWORD PollEndian            : 2;    // GPUENDIAN
                DWORD PollAddress           : 30;
            };
            struct {
                DWORD PollRegister          : 15;
                DWORD                       : 17;
            };
        };

        // DWORD 2:

        DWORD Reference                     : 32;

        // DWORD 3:

        DWORD Mask                          : 32;

        // DWORD 4:

        union {
            struct {
                DWORD WriteEndian           : 2;    // GPUENDIAN
                DWORD WriteAddress          : 30;
            };
            struct {
                DWORD WriteRegister         : 15;
                DWORD                       : 17;
            };
        };

        // DWORD 5:

        DWORD Data                          : 32;
    };
    DWORD dword[6];
} GPUCOMMAND_COND_WRITE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;
    };
    DWORD dword[1];
} GPUCOMMAND_MEM_WRITE_COUNTER;

typedef union {
    struct {
        // DWORD 0:

        DWORD Initiator                     : 6;    // GPUINITIATOR
        DWORD                               : 24;
        DWORD SoftwareManaged               : 1;
        DWORD UseCounter                    : 1;

        // DWORD 1:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;

        // DWORD 2:

        DWORD Data                          : 32;
    };
    DWORD dword[3];
} GPUCOMMAND_EVENT_WRITE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Initiator                     : 6;    // GPUINITIATOR
        DWORD                               : 24;
        DWORD SoftwareManaged               : 1;
        DWORD UseCounter                    : 1;

        // DWORD 1:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;

        // DWORD 2:

        DWORD Data                          : 32;
    };
    DWORD dword[3];
} GPUCOMMAND_EVENT_WRITE_SHADER;

typedef union {
    struct {
        // DWORD 0:

        DWORD Initiator                     : 6;    // GPUINITIATOR
        DWORD                               : 25;
        DWORD UseCounter                    : 1;

        // DWORD 1:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;

        // DWORD 2:

        DWORD Data                          : 32;
    };
    DWORD dword[3];
} GPUCOMMAND_EVENT_WRITE_CACHE_FLUSH;

typedef union {
    struct {
        // DWORD 0:

        DWORD Initiator                     : 6;    // GPUINITIATOR
        DWORD                               : 26;

        // DWORD 1:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;
    };
    DWORD dword[2];
} GPUCOMMAND_EVENT_WRITE_SCREEN_EXTENT;

typedef union {
    struct {
        DWORD Initiator                     : 6;    // GPUINITIATOR
        DWORD                               : 26;
    };
    DWORD dword[1];
} GPUCOMMAND_EVENT_WRITE_ZPASS_DONE;

typedef union {
    struct {
        // DWORD 0:

        DWORD Register                      : 15;
        DWORD                               : 17;

        // DWORD 1:

        DWORD Endian                        : 2;    // GPUENDIAN
        DWORD Address                       : 30;
    };
    DWORD dword[2];
} GPUCOMMAND_REG_TO_MEM;

typedef union {
    struct {
        DWORD Unused                        : 32;
    };
    DWORD dword[1];
} GPUCOMMAND_WAIT_FOR_IDLE;

typedef union {
    struct {
        DWORD Cpu0                          : 1;
        DWORD Cpu1                          : 1;
        DWORD Cpu2                          : 1;
        DWORD Cpu3                          : 1;
        DWORD Cpu4                          : 1;
        DWORD Cpu5                          : 1;
        DWORD                               : 26;
    };
    DWORD dword[1];
} GPUCOMMAND_CPU_INTERRUPT;

typedef union {
    struct {
        DWORD Id                            : 6;
        DWORD                               : 1;
        DWORD End                           : 1;
        DWORD                               : 24;
    };
    DWORD dword[1];
} GPUCOMMAND_VIZ_QUERY;

typedef union {
    struct {
        // DWORD 0:

        DWORD PrimType                      : 6;    // Must be GPUPRIMTYPE_RECTLIST
        DWORD SrcSelect                     : 2;    // Must be GPUINDEXSELECT_IMMEDIATE
        DWORD MajorMode                     : 3;
        DWORD IndexType                     : 1;    // GPUINDEXTYPE
        DWORD NotEndOfPacket                : 1;
        DWORD                               : 3;
        DWORD NumIndices                    : 16;

        // DWORD 1:

        DWORD NumBaseIndices                : 14;
        DWORD                               : 18;

        // DWORD 2:

        DWORD Index32[];
    };
    DWORD dword[2];
} GPUCOMMAND_MPEG_INDEX;

typedef union {
    struct {
        DWORD Unused                        : 32;
    };
    DWORD dword[1];
} GPUCOMMAND_NOP;

typedef union {
    struct {
        // DWORD 0:

        DWORD Address                       : 32;

        // DWORD 1:

        DWORD Size                          : 20;
        DWORD                               : 11;
        DWORD MultiPass                     : 1;
    };
    DWORD dword[2];
} GPUCOMMAND_INDIRECT_BUFFER;

typedef union {
    struct {
        // DWORD 0:

        DWORD Register                      : 15;
        DWORD                               : 17;

        // DWORD 1:

        DWORD Value                         : 8;
        DWORD                               : 24;
    };
    DWORD dword[2];
} GPUCOMMAND_FIX_2_FLT_REG;

typedef union {
    struct {
        // DWORD 0:

        DWORD Low                           : 32;

        // DWORD 1:

        DWORD High                          : 32;
    };
    DWORD dword[2];
} GPUCOMMAND_SET_BIN_MASK;

typedef union {
    struct {
        // DWORD 0:

        DWORD Low                           : 32;

        // DWORD 1:

        DWORD High                          : 32;
    };
    DWORD dword[2];
} GPUCOMMAND_SET_BIN_SELECT;

typedef union {
    struct {
        DWORD Unused                        : 32;
    };
    DWORD dword[1];
} GPUCOMMAND_CONTEXT_UPDATE;


//------------------------------------------------------------------------------
// GPU performce counters

//------------------------------------------------------------------------------
// GPU performance counter event select enums

typedef enum
{
    GPUPE_CP_COUNT = 0, // Always Count
    GPUPE_CP_RBIU_STALL = 1, // RBIU Transaction FIFO FUll
    GPUPE_CP_RBIU_TAF = 2, // RBIU Transaction Almost FIFO
    GPUPE_CP_PFP_STALL = 3, // PFP Transaction is Waiting for RBBM in RCIU
    GPUPE_CP_RESERVED0 = 4, // Unused
    GPUPE_CP_RESERVED1 = 5, // Unused
    GPUPE_CP_NRT_RCIU_STALL = 6, // Transaction is Waiting for RBBM in RCIU
    GPUPE_CP_Reserved2 = 7, // Unused
    GPUPE_CP_NRT_MIU_STALL = 8, // CSF Fetcher Waiting on MIU
    GPUPE_CP_CSF_PFP_I1_FULL = 9, // CSF PFP I1 Request FIFO is FUll
    GPUPE_CP_CSF_PFP_I2_FULL = 10, // CSF PFP I2 Request FIFO is FUll
    GPUPE_CP_CSF_PFP_REQ_FULL = 11, // CSF PFP State Request FIFO is FUll
    GPUPE_CP_RESERVED3 = 12, // Unused
    GPUPE_CP_RING_ROQ_FULL = 13, // Ring Reorder Queue is Full
    GPUPE_CP_I1_ROQ_FULL = 14, // I1 Reorder Queue is Full
    GPUPE_CP_I2_ROQ_FULL = 15, // I2 Reorder Queue is Full
    GPUPE_CP_ST_ROQ_FULL = 16, // State Reorder Queue is Full
    GPUPE_CP_RTST_ROQ_FULL = 17, // Vertex Shader Early Fetch Done
    GPUPE_CP_MIU_TAG_MEM_FULL = 18, // MIU Tag Memory is Full
    GPUPE_CP_MIU_WRITECLEAN = 19, // MIU WriteClean is In-Progress
    GPUPE_CP_RESERVED4 = 20, // Unused
    GPUPE_CP_RESERVED5 = 21, // Unused
    GPUPE_CP_NRT_WRITE_STALL = 22, // Write Request Stalled by MIU Input FIFO
    GPUPE_CP_NRT_READ_STALL = 23, // Read Request Stalled by MIU Input FIFO
    GPUPE_CP_WC_FIFO_FULL = 24, // Write Confirm FIFO is FULL
    GPUPE_CP_VTX_DEALLOC_FIFO_FULL = 25, // Vertex Shader Dealloc FIFO is FULL
    GPUPE_CP_PIX_DEALLOC_FIFO_FULL = 26, // Pixel Shader Dealloc FIFO is FULL
    GPUPE_CP_VTX_EVENT_FIFO_FULL = 27, // Vertex Shader Event FIFO is FULL
    GPUPE_CP_PIX_EVENT_FIFO_FULL = 28, // Pixel Shader Event FIFO is FULL
    GPUPE_CP_CF_EVENT_FIFO_FULL = 29, // Cache Flush Event FIFO is FULL
    GPUPE_CP_ME_RB_STARVED = 30, // Micro Engine's RB Processing Starved by PFP
    GPUPE_CP_ME_I1_STARVED = 31, // Micro Engine's I1 Processing Starved by PFP
    GPUPE_CP_ME_I2_STARVED = 32, // Micro Engine's I2 Processing Starved by PFP
    GPUPE_CP_ME_ST_STARVED = 33, // Micro Engine's ST Processing Starved by PFP
    GPUPE_CP_RESERVED6 = 34, // Unused
    GPUPE_CP_RESERVED7 = 35, // Unused
    GPUPE_CP_RESERVED8 = 36, // Unused
    GPUPE_CP_RESERVED9 = 37, // Unused
    GPUPE_CP_RESERVED10 = 38, // Unused
    GPUPE_CP_RESERVED11 = 39, // Unused
    GPUPE_RCIU_RBBM_DWORD_SENT = 40, // RCIU is sending data to the RBBM
    GPUPE_ME_PARSER_BUSY_CLOCKS = 41, // Micro Engine’s Parser is Busy
    GPUPE_ME_WAIT_CONTEXT_AVAIL = 42, // Micro Engine is waiting for an available context
    GPUPE_PFP_TYPE0_PACKET = 43, // PFP processed a Type-0 packet
    GPUPE_PFP_TYPE3_PACKET = 44, // PFP processed a Type-3 packet
    GPUPE_CSF_RB_WPTR_NEQ_RPTR = 45, // The CSF has more data to fetch from the Ring Command buffer
    GPUPE_CSF_I1_SIZE_NEQ_ZERO = 46, // The CSF has more data to fetch from the Indirect1 Command buffer
    GPUPE_CSF_I2_SIZE_NEQ_ZERO = 47, // The CSF has more data to fetch from the Indirect2 Command buffer
    GPUPE_CSF_RB_I1_I2_FETCHING = 48, // The CSF has more data to fetch from any of the Command buffers (Ring/Indirect1/Indirect2)
    GPUPE_CP_RESERVED12 = 49, // Unused
    GPUPE_CP_RESERVED13 = 50, // Unused
    GPUPE_CP_RESERVED14 = 51, // Unused
    GPUPE_CP_RESERVED15 = 52, // Unused
    GPUPE_CP_RESERVED16 = 53, // Unused
    GPUPE_CP_RESERVED17 = 54, // Unused
    GPUPE_CP_RESERVED18 = 55, // Unused
    GPUPE_CP_RESERVED19 = 56, // Unused
    GPUPE_CP_RESERVED20 = 57, // Unused
    GPUPE_CP_RESERVED21 = 58, // Unused
    GPUPE_CP_RESERVED22 = 59, // Unused
    GPUPE_CP_RESERVED23 = 60, // Unused
    GPUPE_CP_RESERVED24 = 61, // Unused
    GPUPE_CP_RESERVED25 = 62, // Unused
    GPUPE_CP_RESERVED26 = 63, // Unused
} GPUPERFEVENT_CP;

typedef enum
{
    GPUPE_RBBM_COUNT = 0,
    GPUPE_RBBM_NRT_BUSY = 1,
    GPUPE_RBBM_BC_CNTX0_BUSY = 2,
    GPUPE_RBBM_BC_CNTX17_BUSY = 3,
    GPUPE_RBBM_SQ_CNTX0_BUSY = 4,
    GPUPE_RBBM_SQ_CNTX17_BUSY = 5,
    GPUPE_RBBM_VGT_BUSY = 6,
    GPUPE_RBBM_VGT_NODMA_BUSY = 7,
    GPUPE_RBBM_PA_BUSY = 8,
    GPUPE_RBBM_SC_CNTX0_BUSY = 9,
    GPUPE_RBBM_SC_CNTX17_BUSY = 10,
    GPUPE_RBBM_TPC_BUSY = 11,
    GPUPE_RBBM_TC_BUSY = 12,
    GPUPE_RBBM_SX_BUSY = 13,
    GPUPE_RESERVED1 = 14,
    GPUPE_RBBM_CP_COHER_BUSY = 15,
    GPUPE_RBBM_CP_NRT_BUSY = 16,
    GPUPE_RESERVED2 = 17,
    GPUPE_RBBM_CP_DMA_BUSY = 18,
    GPUPE_RESERVED3 = 19,
    GPUPE_RESERVED4 = 20,
    GPUPE_RESERVED5 = 21,
    GPUPE_RBBM_DMA_IDLE_STALL = 22,
    GPUPE_RESERVED6 = 23,
    GPUPE_RESERVED7 = 24,
    GPUPE_RBBM_GFX_IDLE_STALL = 25,
    GPUPE_RBBM_GFX_IDLEC_STALL = 26,
    GPUPE_RBBM_INTERRUPT = 27,
} GPUPERFEVENT_RBBM;

typedef enum
{
    GPUPE_SQ_PIXEL_VECTORS_SUB = 0, // Number of pixel vectors submitted
    GPUPE_SQ_VERTEX_VECTORS_SUB = 1, // Number of vertex vectors submitted
    GPUPE_SQ_ALU0_ACTIVE_VTX_SIMD0 = 2, // Number of cycles ALU 0 is active (vertex) for SIMD0
    GPUPE_SQ_ALU1_ACTIVE_VTX_SIMD0 = 3, // Number of cycles ALU 1 is active (vertex) for SIMD0
    GPUPE_SQ_ALU0_ACTIVE_PIX_SIMD0 = 4, // Number of cycles ALU 0 is active (pixel) for SIMD0
    GPUPE_SQ_ALU1_ACTIVE_PIX_SIMD0 = 5, // Number of cycles ALU 1 is active (pixel) for SIMD0
    GPUPE_SQ_ALU0_ACTIVE_VTX_SIMD1 = 6, // Number of cycles ALU 0 is active (vertex) for SIMD1
    GPUPE_SQ_ALU1_ACTIVE_VTX_SIMD1 = 7, // Number of cycles ALU 1 is active (vertex) for SIMD1
    GPUPE_SQ_ALU0_ACTIVE_PIX_SIMD1 = 8, // Number of cycles ALU 0 is active (pixel) for SIMD1
    GPUPE_SQ_ALU1_ACTIVE_PIX_SIMD1 = 9, // Number of cycles ALU 1 is active (pixel) for SIMD1
    GPUPE_SQ_EXPORT_CYCLES = 10, // Number of clocks the SQ is exporting data
    GPUPE_SQ_ALU_CST_WRITTEN = 11, // Number of ALU constants written from the CP for both pix/vtx
    GPUPE_SQ_TEX_CST_WRITTEN = 12, // Number of texture constants written from the CP for both pix/vtx
    GPUPE_SQ_ALU_CST_STALL = 13, // Number of clocks the constant memory is stalled because of ALU constant store full
    GPUPE_SQ_ALU_TEX_STALL = 14, // Number of clocks the constant memory is stalled because of texture constant store full
    GPUPE_SQ_INST_WRITTEN = 15, // Number of instructions written from the CP for both pix/vtx
    GPUPE_SQ_BOOLEAN_WRITTEN = 16, // Number of control flow booleans written from the CP for both pix/vtx
    GPUPE_SQ_LOOPS_WRITTEN = 17, // Number of control flow loops written from the CP for both pix/vtx
    GPUPE_SQ_PIXEL_SWAP_IN = 18, // Number of times a pixel vector is de-activated, this should be (number of clauses) * number of threads
    GPUPE_SQ_PIXEL_SWAP_OUT = 19, // Number of times a pixel vector is activated, this should be (number of clauses - 1) * number of threads
    GPUPE_SQ_VERTEX_SWAP_IN = 20, // Number of times a vertex vector is de-activated, this should be (number of clauses) * number of threads
    GPUPE_SQ_VERTEX_SWAP_OUT = 21, // Number of times a vertex vector is activated, this should be (number of clauses - 1) * number of threads
    GPUPE_SQ_ALU_VTX_INST_ISSUED = 22, // Number of ALU instruction issued (vertex) include all SIMDS and ALU 0/1
    GPUPE_SQ_TEX_VTX_INST_ISSUED = 23, // Number of Texture instruction issued (vertex)
    GPUPE_SQ_VC_VTX_INST_ISSUED = 24, // Number of VC instruction issued (vertex)
    GPUPE_SQ_CF_VTX_INST_ISSUED = 25, // Number of control flow instruction issued (vertex) include all resources (TP,VC,all SIMDS)
    GPUPE_SQ_ALU_PIX_INST_ISSUED = 26, // Number of ALU instruction issued (pixel) include all SIMDS and ALU 0/1
    GPUPE_SQ_TEX_PIX_INST_ISSUED = 27, // Number of Texture instruction issued (pixel)
    GPUPE_SQ_VC_PIX_INST_ISSUED = 28, // Number of VC instruction issued (pixel)
    GPUPE_SQ_CF_PIX_INST_ISSUED = 29, // Number of control flow instruction issued (pixel) include all resources (TP,VC,all SIMDS)
    GPUPE_SQ_ALU0_FIFO_EMPTY_SIMD0 = 30, // Number of clocks ALU 0 FIFO was empty (busy with control flow) for SIMD0, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU1_FIFO_EMPTY_SIMD0 = 31, // Number of clocks ALU 1 FIFO was empty (busy with control flow) for SIMD0, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU0_FIFO_EMPTY_SIMD1 = 32, // Number of clocks ALU 0 FIFO was empty (busy with control flow) for SIMD1, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU1_FIFO_EMPTY_SIMD1 = 33, // Number of clocks ALU 1 FIFO was empty (busy with control flow) for SIMD1, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU_NOPS = 34, // Number of ALU NOPs generated by the SQ. This counts the number of added ALU instructions because of the use of the address register immediately after the address is set
    GPUPE_SQ_PRED_SKIP = 35, // Number of times a vector was not executed because of the predicate optimization (counts for all threads)
    GPUPE_SQ_SYNC_ALU_STALL_SIMD0_VTX = 36, // Number of cycles all vertex threads are blocked because of synchronization (Alu SIMD0). This is all threads that want to go to Alu SIMD0 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD1_VTX = 37, // Number of cycles all vertex threads are blocked because of synchronization (Alu SIMD1) This is all threads that want to go to Alu SIMD1 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_TEX_STALL_VTX = 38, // Number of cycles all vertex threads are blocked because of synchronization (Texture) This is all threads that want to go to the TP but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_VC_STALL_VTX = 39, // Number of cycles all vertex threads are blocked because of synchronization (VC) This is all threads that want to go to the VC but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_CONSTANTS_USED_SIMD0 = 40, // Number of ALU constants used for SIMD0. Decodes the instruction to count how many constants there are (1,2,3)
    GPUPE_SQ_CONSTANTS_SENT_SP_SIMD0 = 41, // Number of ALU constants sent to the SP for SIMD0. This is the number of reads to the constant store. If greather than SQ_CONSTANTS_USED, an instruction was recirculated because of waterfaling.
    GPUPE_SQ_GPR_STALL_VTX = 42, // Number of stall cycles because of GPR resource (vertex). This prevents vertexes to be sent from the VGT.
    GPUPE_SQ_GPR_STALL_PIX = 43, // Number of stall cycles because of GPR resource (pixel). This prevents pixels to be sent from the SC.
    GPUPE_SQ_VTX_RS_STALL = 44, // Number of stall cycles because of Reservation Station (vertex). This prevents vertexes to be sent from the VGT.
    GPUPE_SQ_PIX_RS_STALL = 45, // Number of stall cycles because of Reservation Station (pixel). This prevents pixels to be sent from the SC.
    GPUPE_SQ_SX_PC_FULL  = 46, // Number of cycles with parameter cache preventing export of vertex vector
    GPUPE_SQ_SX_EXP_BUFF_FULL = 47, // Number of cycles with export buffers preventing export
    GPUPE_SQ_SX_POS_BUFF_FULL = 48, // Number of cycles with position buffers preventing export
    GPUPE_SQ_INTERP_QUADS = 49, // Number of interpolated quads. This is number of quads * number of parameters.
    GPUPE_SQ_INTERP_ACTIVE = 50, // Number of active interpolation cycles. Optimaly one cycle should interpolate 4 quads for 1 parameter.
    GPUPE_SQ_IN_PIXEL_STALL = 51, // Number of cycles a ready to go pixel vector stalled because of port arbitration to the GPRs
    GPUPE_SQ_IN_VTX_STALL = 52, // Number of cycles a ready to go vertex vector stalled because of port arbitration to the GPRs
    GPUPE_SQ_VTX_CNT = 53, // Number of individual vertexes sent to the sequencer
    GPUPE_SQ_VTX_16_VECTOR = 54, // Number of vertex vectors with less than 16 vertexes
    GPUPE_SQ_VTX_32_VECTOR = 55, // Number of vertex vectors with less than 32 vertexes
    GPUPE_SQ_VTX_48_VECTOR = 56, // Number of vertex vectors with less than 48 vertexes
    GPUPE_SQ_PIXEL_16_VECTOR = 57, // Number of pixel vectors with less than 16 pixels
    GPUPE_SQ_PIXEL_32_VECTOR = 58, // Number of pixel vectors with less than 32 pixels
    GPUPE_SQ_PIXEL_48_VECTOR = 59, // Number of pixel vectors with less than 48 pixels
    GPUPE_SQ_CONSTANTS_USED_SIMD1 = 60, // Number of ALU constants used for SIMD1. Decodes the instruction to count how many constants there are (1,2,3)
    GPUPE_SQ_CONSTANTS_SENT_SP_SIMD1 = 61, // Number of ALU constants sent to the SP for SIMD1. This is the number of reads to the constant store. If greather than SQ_CONSTANTS_USED, an instruction was recirculated because of waterfaling.
    GPUPE_SQ_SX_MEM_EXP_FULL = 62, // Number of cycles with memory export buffer preventing an export
    GPUPE_SQ_ALU0_ACTIVE_VTX_SIMD2 = 63, // Number of cycles ALU 0 is active (vertex) for SIMD2
    GPUPE_SQ_ALU1_ACTIVE_VTX_SIMD2 = 64, // Number of cycles ALU 1 is active (vertex) for SIMD2
    GPUPE_SQ_ALU0_ACTIVE_PIX_SIMD2 = 65, // Number of cycles ALU 0 is active (pixel) for SIMD2
    GPUPE_SQ_ALU1_ACTIVE_PIX_SIMD2 = 66, // Number of cycles ALU 1 is active (pixel) for SIMD2
    GPUPE_SQ_ALU0_ACTIVE_VTX_SIMD3 = 67, // Number of cycles ALU 0 is active (vertex) for SIMD3
    GPUPE_SQ_ALU1_ACTIVE_VTX_SIMD3 = 68, // Number of cycles ALU 1 is active (vertex) for SIMD3
    GPUPE_SQ_ALU0_ACTIVE_PIX_SIMD3 = 69, // Number of cycles ALU 0 is active (pixel) for SIMD3
    GPUPE_SQ_ALU1_ACTIVE_PIX_SIMD3 = 70, // Number of cycles ALU 1 is active (pixel) for SIMD3
    GPUPE_SQ_ALU0_FIFO_EMPTY_SIMD2 = 71, // Number of clocks ALU 0 FIFO was empty (busy with control flow) for SIMD2, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU1_FIFO_EMPTY_SIMD2 = 72, // Number of clocks ALU 1 FIFO was empty (busy with control flow) for SIMD2, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU0_FIFO_EMPTY_SIMD3 = 73, // Number of clocks ALU 0 FIFO was empty (busy with control flow) for SIMD3, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_ALU1_FIFO_EMPTY_SIMD3 = 74, // Number of clocks ALU 1 FIFO was empty (busy with control flow) for SIMD3, only counts when there is a thread in any of pixel or vertex RS
    GPUPE_SQ_SYNC_ALU_STALL_SIMD2_VTX = 75, // Number of cycles all vertex threads are blocked because of synchronization (Alu SIMD2) This is all threads that want to go to Alu SIMD2 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD3_VTX = 76, // Number of cycles all vertex threads are blocked because of synchronization (Alu SIMD3) This is all threads that want to go to Alu SIMD3 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD0_PIX = 77, // Number of cycles all pixel threads are blocked because of synchronization (Alu SIMD0) This is all threads that want to go to Alu SIMD0 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD1_PIX = 78, // Number of cycles all pixel threads are blocked because of synchronization (Alu SIMD1) This is all threads that want to go to Alu SIMD1 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD2_PIX = 79, // Number of cycles all pixel threads are blocked because of synchronization (Alu SIMD2) This is all threads that want to go to Alu SIMD2 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_ALU_STALL_SIMD3_PIX = 80, // Number of cycles all pixel threads are blocked because of synchronization (Alu SIMD3) This is all threads that want to go to Alu SIMD3 but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_TEX_STALL_PIX = 81, // Number of cycles all pixel threads are blocked because of synchronization (Texture) This is all threads that want to go to the TP but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_SYNC_VC_STALL_PIX = 82, // Number of cycles all pixel threads are blocked because of synchronization (VC) This is all threads that want to go to the VC but cannot because of the VC,TP or alloc resource.
    GPUPE_SQ_CONSTANTS_USED_SIMD2 = 83, // Number of ALU constants used for SIMD2. Decodes the instruction to count how many constants there are (1,2,3)
    GPUPE_SQ_CONSTANTS_SENT_SP_SIMD2 = 84, // Number of ALU constants sent to the SP for SIMD2. This is the number of reads to the constant store. If greather than SQ_CONSTANTS_USED, an instruction was recirculated because of waterfaling.
    GPUPE_SQ_CONSTANTS_USED_SIMD3 = 85, // Number of ALU constants used for SIMD3. Decodes the instruction to count how many constants there are (1,2,3)
    GPUPE_SQ_CONSTANTS_SENT_SP_SIMD3 = 86, // Number of ALU constants sent to the SP for SIMD3. This is the number of reads to the constant store. If greather than SQ_CONSTANTS_USED, an instruction was recirculated because of waterfaling.
    GPUPE_SQ_ALU0_FIFO_FULL_SIMD0 = 87, // Number of cycles ALU 0 FIFO was full for SIMD0
    GPUPE_SQ_ALU1_FIFO_FULL_SIMD0 = 88, // Number of cycles ALU 1 FIFO was full for SIMD0
    GPUPE_SQ_ALU0_FIFO_FULL_SIMD1 = 89, // Number of cycles ALU 0 FIFO was full for SIMD1
    GPUPE_SQ_ALU1_FIFO_FULL_SIMD1 = 90, // Number of cycles ALU 1 FIFO was full for SIMD1
    GPUPE_SQ_ALU0_FIFO_FULL_SIMD2 = 91, // Number of cycles ALU 0 FIFO was full for SIMD2
    GPUPE_SQ_ALU1_FIFO_FULL_SIMD2 = 92, // Number of cycles ALU 1 FIFO was full for SIMD2
    GPUPE_SQ_ALU0_FIFO_FULL_SIMD3 = 93, // Number of cycles ALU 0 FIFO was full for SIMD3
    GPUPE_SQ_ALU1_FIFO_FULL_SIMD3 = 94, // Number of cycles ALU 1 FIFO was full for SIMD3
    GPUPE_VC_PERF_STATIC = 95, // Number of cycles the VC is not ready to receive anything and the SQ is not ready to send anything
    GPUPE_VC_PERF_STALLED = 96, // Number of cycles the VC is not ready to receive anything but the SQ is ready to send something
    GPUPE_VC_PERF_STARVED = 97, // Number of cycles the VC is ready to receive something but the SQ is not ready to send anything
    GPUPE_VC_PERF_SEND = 98, // Number of cycles the VC is ready to receive something and the SQ is is ready to send something
    GPUPE_VC_PERF_ACTUAL_STARVED = 99, // Number of cycles the VC is starved
    GPUPE_PIXEL_THREAD_0_ACTIVE  = 100, // Number of cycles the thread 0 of pixel is active
    GPUPE_VERTEX_THREAD_0_ACTIVE     = 101, // Number of cycles the thread 0 of vertex is active
    GPUPE_PIXEL_THREAD_0_NUMBER  = 102, // Number of times the thread 0 of pixel is active
    GPUPE_VERTEX_THREAD_0_NUMBER     = 103, // Number of times the thread 0 of vertex is active
    GPUPE_VERTEX_EVENT_NUMBER    = 104, // Number of events sent in the vertex thread buffer
    GPUPE_PIXEL_EVENT_NUMBER     = 105, // Number of events sent in the pixel thread buffer
} GPUPERFEVENT_SQ;

typedef enum
{
    GPUPE_VGT_SQ_EVENT_WINDOW_ACTIVE = 0,
    GPUPE_VGT_SQ_SEND = 1,
    GPUPE_VGT_SQ_STALLED = 2,
    GPUPE_VGT_SQ_STARVED_BUSY = 3,
    GPUPE_VGT_SQ_STARVED_IDLE = 4,
    GPUPE_VGT_SQ_STATIC = 5,
    GPUPE_VGT_PA_EVENT_WINDOW_ACTIVE = 6,
    GPUPE_VGT_PA_CLIP_V_SEND = 7,
    GPUPE_VGT_PA_CLIP_V_STALLED = 8,
    GPUPE_VGT_PA_CLIP_V_STARVED_BUSY = 9,
    GPUPE_VGT_PA_CLIP_V_STARVED_IDLE = 10,
    GPUPE_VGT_PA_CLIP_V_STATIC = 11,
    GPUPE_VGT_PA_CLIP_P_SEND = 12,
    GPUPE_VGT_PA_CLIP_P_STALLED = 13,
    GPUPE_VGT_PA_CLIP_P_STARVED_BUSY = 14,
    GPUPE_VGT_PA_CLIP_P_STARVED_IDLE = 15,
    GPUPE_VGT_PA_CLIP_P_STATIC = 16,
    GPUPE_VGT_PA_CLIP_S_SEND = 17,
    GPUPE_VGT_PA_CLIP_S_STALLED = 18,
    GPUPE_VGT_PA_CLIP_S_STARVED_BUSY = 19,
    GPUPE_VGT_PA_CLIP_S_STARVED_IDLE = 20,
    GPUPE_VGT_PA_CLIP_S_STATIC = 21,
    GPUPE_RBIU_FIFOS_EVENT_WINDOW_ACTIVE = 22,
    GPUPE_RBIU_IMMED_DATA_FIFO_STARVED = 23,
    GPUPE_RBIU_IMMED_DATA_FIFO_STALLED = 24,
    GPUPE_RBIU_DMA_REQUEST_FIFO_STARVED = 25,
    GPUPE_RBIU_DMA_REQUEST_FIFO_STALLED = 26,
    GPUPE_RBIU_DRAW_INITIATOR_FIFO_STARVED = 27,
    GPUPE_RBIU_DRAW_INITIATOR_FIFO_STALLED = 28,
    GPUPE_SPARE29 = 29,
    GPUPE_SPARE30 = 30,
    GPUPE_SPARE31 = 31,
    GPUPE_SPARE32 = 32,
    GPUPE_SPARE33 = 33,
    GPUPE_SPARE34 = 34,
    GPUPE_SPARE35 = 35,
    GPUPE_SPARE36 = 36,
    GPUPE_SPARE37 = 37,
    GPUPE_SPARE38 = 38,
    GPUPE_SPARE39 = 39,
    GPUPE_TE_SU_IN_VALID = 40,
    GPUPE_TE_SU_IN_READ = 41,
    GPUPE_TE_SU_IN_PRIM = 42,
    GPUPE_TE_SU_IN_EOP = 43,
    GPUPE_TE_SU_IN_NULL_PRIM = 44,
    GPUPE_TE_WK_IN_VALID = 45,
    GPUPE_TE_WK_IN_READ = 46,
    GPUPE_TE_OUT_PRIM_VALID = 47,
    GPUPE_TE_OUT_PRIM_READ = 48,
} GPUPERFEVENT_VGT;

typedef enum
{
    GPUPE_RG_VERTICES = 0, // Number of vertices processed by RG
    GPUPE_RG_CLAMPED = 1, // Number of vertices clamped by RG
    GPUPE_RG_L2_REQUEST = 2, // Count of L2 Requests created by RG
    GPUPE_RG_L1_REQUEST = 3, // Count of L1 Requests created by RG
    GPUPE_RG_MEGAFETCH = 4, // Count of megafetches processed by RG
    GPUPE_RG_END_OF_GROUP = 5, // Count of end_of_group signals received from SQ
    GPUPE_RG_CONFLICT = 6, // Number of vertex pairs that conflict in the L2
    GPUPE_RG_DWORDS_REQUESTED = 7, // Number of dword requests created by RG
    GPUPE_RG_SPARE0 = 8, // Reserved for RG
    GPUPE_RG_SPARE1 = 9, // Reserved for RG
    GPUPE_CC_STALLS = 10, // Number of clocks CC stalled due to memory latency
    GPUPE_CC_HITS = 11, // L2 Requests that resulted in a cache hit
    GPUPE_CC_MISSES = 12, // L2 Requests that resulted in a cache miss
    GPUPE_CC_SECTOR_MISSES = 13, // L2 Requests that resulted in a sector miss
    GPUPE_CC_L2B_STALLS = 14, // CC stalled due to L2B FIFO not RTR
    GPUPE_CC_MI_STALLS = 15, // CC stalled due to MI not RTR
    GPUPE_CC_MULTICYCLE_STALLS = 16, // CC stalled due to multicycle
    GPUPE_CC_EVEN_ALLOC_STALLS = 17, // CC stalled due to even allocation stall
    GPUPE_CC_ODD_ALLOC_STALLS = 18, // CC stalled due to odd allocation stall
    GPUPE_CC_EVEN_BUSY_STALLS = 19, // CC stalled due to even counter busy
    GPUPE_CC_ODD_BUSY_STALLS = 20, // CC stalled due to odd counter busy
    GPUPE_CC_IN_FIFO_EMPTY = 21, // CC input FIFO is empty
    GPUPE_CC_IN_FIFO_FULL = 22, // CC input FIFO is full
    GPUPE_CC_FREEZE = 23, // CC frozen due to one of the stall conditions
    GPUPE_CC_SPARE0 = 24, // Reserved for CC
    GPUPE_CC_SPARE1 = 25, // Reserved for CC
    GPUPE_CC_SPARE2 = 26, // Reserved for CC
    GPUPE_CC_SPARE3 = 27, // Reserved for CC
    GPUPE_CC_SPARE4 = 28, // Reserved for CC
    GPUPE_CC_SPARE5 = 29, // Reserved for CC
    GPUPE_MI_REQUESTS_TO_MH = 30, // Number of memory requests issued to memory hub
    GPUPE_MI_AGP_REQUESTS = 31, // Number of AGP memory requests issued
    GPUPE_MI_LATENCY_BITS_4_0    = 32, // 4:0 of the latency count
    GPUPE_MI_LATENCY_BITS_9_5 = 33, // Bits 9:5 of the latency count
    GPUPE_MI_LATENCY_BITS_14_10 = 34, // Bits 14:10 of the latency count
    GPUPE_MI_LATENCY_BITS_17_15 = 35, // Bits 17:15 of the latency count
    GPUPE_MI_INPUT_FIFOS_FULL = 36, // Number of cycles any of the four Input FIFOs to the MI requestor are full
    GPUPE_MI_INPUT_FIFOS_0_FULL = 37, // Number of cycles any of the two Input FIFOs on request path 0 to the MI requestor are full
    GPUPE_MI_INPUT_FIFOS_1_FULL = 38, // Number of cycles any of the two Input FIFOs on request path 1 to the MI requestor are full
    GPUPE_MI_SPARE1 = 39, // Reserved for MI
    GPUPE_RP_SP_DATA_VALID = 40, // Number of clocks of valid data returned to SP
    GPUPE_RP_STALLED = 41, // Number of clocks RP stalled due to TC use of phase
    GPUPE_RP_SPARE0 = 42, // Reserved for RP
    GPUPE_RP_SPARE1 = 43, // Reserved for RP
    GPUPE_RP_SPARE2 = 44, // Reserved for RP
    GPUPE_RP_SPARE3 = 45, // Reserved for RP
    GPUPE_RP_SPARE4 = 46, // Reserved for RP
    GPUPE_RP_SPARE5 = 47, // Reserved for RP
    GPUPE_RP_SPARE6 = 48, // Reserved for RP
    GPUPE_RP_SPARE7 = 49, // Reserved for RP
    GPUPE_DC_NUM_VALIDS = 50, // Number of valid vectors returned to the SP
    GPUPE_DC_SPARE0 = 51, // Reserved for DC
    GPUPE_DC_SPARE1 = 52, // Reserved for DC
    GPUPE_DC_SPARE2 = 53, // Reserved for DC
    GPUPE_DC_SPARE3 = 54, // Reserved for DC
    GPUPE_DC_SPARE4 = 55, // Reserved for DC
    GPUPE_DC_SPARE5 = 56, // Reserved for DC
    GPUPE_DC_SPARE6 = 57, // Reserved for DC
    GPUPE_DC_SPARE7 = 58, // Reserved for DC
    GPUPE_DC_SPARE8 = 59, // Reserved for DC
    GPUPE_SQ_VC_SEND = 60, // Number of clocks the SQ is passing data to the VC
    GPUPE_VC_STARVED_IDLE = 61, // Number of clock cycles the VC is idle and waiting for more data from the SQ
    GPUPE_VC_BUSY = 62, // Number of clock cycles the VC is busy processing data
    GPUPE_VC_IDLE = 63, // Number of clock cycles the VC is idle
    GPUPE_VC_SPARE0 = 64, // Reserved for VC
    GPUPE_VC_SPARE1 = 65, // Reserved for VC
    GPUPE_VC_SPARE2 = 66, // Reserved for VC
    GPUPE_VC_SPARE3 = 67, // Reserved for VC
    GPUPE_VC_SPARE4 = 68, // Reserved for VC
    GPUPE_VC_SPARE5 = 69, // Reserved for VC
} GPUPERFEVENT_VC;

typedef enum
{
    GPUPE_PERF_PAPC_PASX_REQ = 0, // Number of PA->SX requests
    GPUPE_PERF_PAPC_PASX_DISABLE_PIPE = 1, // Number of transfers lost due to disabled pipe
    GPUPE_PERF_PAPC_PASX_FIRST_VECTOR = 2, // Number of First Vectors from SX to PA
    GPUPE_PERF_PAPC_PASX_SECOND_VECTOR = 3, // Number of Second Vectors from SX to PA
    GPUPE_PERF_PAPC_PASX_FIRST_DEAD = 4, // Number of Unused First Vectors (due to granularity of 4)
    GPUPE_PERF_PAPC_PASX_SECOND_DEAD = 5, // Number of Unused Second Vectors (due to granularity of 4)
    GPUPE_PERF_PAPC_PASX_VTX_KILL_DISCARD = 6, // Number of vertices which have VTX KILL Enabled and Set
    GPUPE_PERF_PAPC_PASX_VTX_NAN_DISCARD = 7, // Number ov vertices which have NaN and corresponding NaN discard
    GPUPE_PERF_PAPC_PA_INPUT_PRIM = 8, // Number of Primitives input to PA
    GPUPE_PERF_PAPC_PA_INPUT_NULL_PRIM = 9, // Number of Null Primitives input to PA
    GPUPE_PERF_PAPC_PA_INPUT_EVENT_FLAG = 10, // Number of Events input to PA
    GPUPE_PERF_PAPC_PA_INPUT_FIRST_PRIM_SLOT = 11, // Number of First-Prim-Of-Slots input to PA
    GPUPE_PERF_PAPC_PA_INPUT_END_OF_PACKET = 12, // Number of End-Of-Packets input to PA
    GPUPE_PERF_PAPC_CLPR_CULL_PRIM = 13, // Number of Prims Culled by Clipper for VV, UCP, VTX_KILL, VTX_NAN
    GPUPE_PERF_PAPC_CLPR_VVUCP_CULL_PRIM = 14, // Number of Prims Culled by Clipper for VV and UCP
    GPUPE_PERF_PAPC_CLPR_VV_CULL_PRIM = 15, // Number of Prims Culled by Clipper for VV
    GPUPE_PERF_PAPC_CLPR_UCP_CULL_PRIM = 16, // Number of Prims Culled by Clipper for UCP
    GPUPE_PERF_PAPC_CLPR_VTX_KILL_CULL_PRIM = 17, // Number of Prims Culled by Clipper for VTX_KILL
    GPUPE_PERF_PAPC_CLPR_VTX_NAN_CULL_PRIM = 18, // Number of Prims Culled by Clipper for VTX_NAN
    GPUPE_PERF_PAPC_CLPR_CULL_TO_NULL_PRIM = 19, // Number of Clipper Culled Prims Retained for Pipe Info
    GPUPE_PERF_PAPC_CLPR_VVUCP_CLIP_PRIM = 20, // Number of Prims Clipped by Clipper for VV and/or UCP
    GPUPE_PERF_PAPC_CLPR_VV_CLIP_PRIM = 21, // Number of Prims Clipped by Clipper for VV
    GPUPE_PERF_PAPC_CLPR_UCP_CLIP_PRIM = 22, // Number of Prims Clipped by Clipper for UCP
    GPUPE_PERF_PAPC_CLPR_POINT_CLIP_CANDIDATE = 23, // Number of Points which require detailed clip checked
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_1 = 24, // Number of Prims with 1 Clip Plane Intersection (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_2 = 25, // Number of Prims with 2 Clip Plane Intersections (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_3 = 26, // Number of Prims with 3 Clip Plane Intersections (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_4 = 27, // Number of Prims with 4 Clip Plane Intersections (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_5_8 = 28, // Number of Prims with 5-8 Clip Plane Intersections (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_CNT_9_12 = 29, // Number of Prims with 9-12 Clip Plane Intersections (includes VV and UCP)
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_NEAR = 30, // Number of Prims which intersect the NEAR   VV Plane
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_FAR = 31, // Number of Prims which intersect the FAR    VV Plane
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_LEFT = 32, // Number of Prims which intersect the LEFT   VV Plane
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_RIGHT = 33, // Number of Prims which intersect the RIGHT  VV Plane
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_TOP = 34, // Number of Prims which intersect the TOP    VV Plane
    GPUPE_PERF_PAPC_CLPR_CLIP_PLANE_BOTTOM = 35, // Number of Prims which intersect the BOTTOM VV Plane
    GPUPE_PERF_PAPC_CLSM_NULL_PRIM = 36, // Number of null primitives at Clip State Machine pipe stage
    GPUPE_PERF_PAPC_CLSM_TOTALLY_VISIBLE_PRIM = 37, // Number of totally visible (no-clipping) prims
    GPUPE_PERF_PAPC_CLSM_CLIP_PRIM = 38, // UNUSED
    GPUPE_PERF_PAPC_CLSM_CULL_TO_NULL_PRIM = 39, // Number of primitives which are culled during clip process
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_1 = 40, // Number of primitives which were clipped and result in 1 primitive
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_2 = 41, // Number of primitives which were clipped and result in 2 primitives
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_3 = 42, // Number of primitives which were clipped and result in 3 primitives
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_4 = 43, // Number of primitives which were clipped and result in 4 primitives
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_5_8 = 44, // Number of primitives which were clipped and result in 5-8 primitives
    GPUPE_PERF_PAPC_CLSM_OUT_PRIM_CNT_9_13 = 45, // Number of primitives which were clipped and result in 9-13 primitives
    GPUPE_PERF_PAPC_CLSM_NON_TRIVIAL_CULL = 46, // UNUSED
    GPUPE_PERF_PAPC_SU_INPUT_PRIM = 47, // Number of primitives input to the Setup block
    GPUPE_PERF_PAPC_SU_INPUT_CLIP_PRIM = 48, // Number of clipped primitives input to the Setup block
    GPUPE_PERF_PAPC_SU_INPUT_NULL_PRIM = 49, // Number of null primitives input to the Setup block
    GPUPE_PERF_PAPC_SU_ZERO_AREA_CULL_PRIM = 50, // Number of primitives culled due to zero area
    GPUPE_PERF_PAPC_SU_BACK_FACE_CULL_PRIM = 51, // Number of back-face primitives culled due to facedness
    GPUPE_PERF_PAPC_SU_FRONT_FACE_CULL_PRIM = 52, // Number of front-face primitives culled due to facedness
    GPUPE_PERF_PAPC_SU_POLYMODE_FACE_CULL = 53, // Number of polymode cull-determination primitives culled
    GPUPE_PERF_PAPC_SU_POLYMODE_BACK_CULL = 54, // Number of polymode primitives discarded due to Back-Face Cull
    GPUPE_PERF_PAPC_SU_POLYMODE_FRONT_CULL = 55, // Number of polymode primitives discarded due to Front-Face Cull
    GPUPE_PERF_PAPC_SU_POLYMODE_INVALID_FILL = 56, // Number of polymode lines and/or points which are culled because they are an internal edge or point
    GPUPE_PERF_PAPC_SU_OUTPUT_PRIM = 57, // Number of primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_CLIP_PRIM = 58, // Number of clipped primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_NULL_PRIM = 59, // Number of null primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_EVENT_FLAG = 60, // Number of events output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_FIRST_PRIM_SLOT = 61, // Number of First-Prim-Of-Slots output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_END_OF_PACKET = 62, // Number of End-Of-Packets output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_POLYMODE_FACE = 63, // Number of polymode facing primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_POLYMODE_BACK = 64, // Number of polymode back-face primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUTPUT_POLYMODE_FRONT = 65, // Number of polymode front-face primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUT_CLIP_POLYMODE_FACE = 66, // Number of clipped polymode facing primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUT_CLIP_POLYMODE_BACK = 67, // Number of clipped polymode back-face primitives output from the Setup block
    GPUPE_PERF_PAPC_SU_OUT_CLIP_POLYMODE_FRONT = 68, // Number of clipped polymode front-face primitives output from the Setup block
    GPUPE_PERF_PAPC_PASX_REQ_IDLE = 69, // Number of clocks PASX Requestor is Idle
    GPUPE_PERF_PAPC_PASX_REQ_BUSY = 70, // Number of clocks PASX Requestor is Busy
    GPUPE_PERF_PAPC_PASX_REQ_STALLED = 71, // Number of clocks PASX Requestor is Stalled
    GPUPE_PERF_PAPC_PASX_REC_IDLE = 72, // Number of clocks PASX Receiver is Idle
    GPUPE_PERF_PAPC_PASX_REC_BUSY = 73, // Number of clocks PASX Receiver is Busy
    GPUPE_PERF_PAPC_PASX_REC_STARVED_SX = 74, // Number of clocks PASX Receiver is Stalled by SX
    GPUPE_PERF_PAPC_PASX_REC_STALLED = 75, // Number of clocks PASX Reciever is Stalled by Position Memory or Clip Code Generator
    GPUPE_PERF_PAPC_PASX_REC_STALLED_POS_MEM = 76, // Number of clocks PASX Reciever is Stalled by Position Memory
    GPUPE_PERF_PAPC_PASX_REC_STALLED_CCGSM_IN = 77, // Number of clocks PASX Reciever is Stalled by Clip Code Generator
    GPUPE_PERF_PAPC_CCGSM_IDLE = 78, // Number of clocks Clip Code Gen is Idle
    GPUPE_PERF_PAPC_CCGSM_BUSY = 79, // Number of clocks Clip Code Gen is Busy
    GPUPE_PERF_PAPC_CCGSM_STALLED = 80, // Number of clocks Clip Code Gen is Stalled
    GPUPE_PERF_PAPC_CLPRIM_IDLE = 81, // Number of clocks Clip Primitive Machine is Idle
    GPUPE_PERF_PAPC_CLPRIM_BUSY = 82, // Number of clocks Clip Primitive Machine is Busy
    GPUPE_PERF_PAPC_CLPRIM_STALLED = 83, // Number of clocks Clip Primitive Machine is stalled by Clip State Machines
    GPUPE_PERF_PAPC_CLPRIM_STARVED_CCGSM = 84, // Number of clocks Clip Primitive Machine is starved by Clip Code Generator
    GPUPE_PERF_PAPC_CLIPSM_IDLE = 85, // Number of clocks Clip State Machines are Idle
    GPUPE_PERF_PAPC_CLIPSM_BUSY = 86, // Number of clocks Clip State Machines are Busy
    GPUPE_PERF_PAPC_CLIPSM_WAIT_CLIP_VERT_ENGH = 87, // Number of clocks Clip State Mahcines are waiting for Clip Vert storage resources
    GPUPE_PERF_PAPC_CLIPSM_WAIT_HIGH_PRI_SEQ = 88, // Number of clocks Clip State Machines are waiting for High Priority Sequencer
    GPUPE_PERF_PAPC_CLIPSM_WAIT_CLIPGA = 89, // Number of clocks Clip State Machines are waiting for ClipGA
    GPUPE_PERF_PAPC_CLIPSM_WAIT_AVAIL_VTE_CLIP = 90, // Number of clocks Clip State Machines are waiting for VTE cycles
    GPUPE_PERF_PAPC_CLIPSM_WAIT_CLIP_OUTSM = 91, // Number of clocks Clip State Machines are waiting for Clip Output State Machine
    GPUPE_PERF_PAPC_CLIPGA_IDLE = 92, // Number of clocks Clip Ga is Idle
    GPUPE_PERF_PAPC_CLIPGA_BUSY = 93, // Number of clocks Clip Ga is Busy
    GPUPE_PERF_PAPC_CLIPGA_STARVED_VTE_CLIP = 94, // Number of clocks Clip Ga is Starved by VTE or Clipper
    GPUPE_PERF_PAPC_CLIPGA_STALLED = 95, // Number of clocks Clip Ga is stalled
    GPUPE_PERF_PAPC_CLIP_IDLE = 96, // Number of clocks Clip is Idle
    GPUPE_PERF_PAPC_CLIP_BUSY = 97, // Number of clocks Clip is Busy
    GPUPE_PERF_PAPC_SU_IDLE = 98, // Number of clocks Setup is Idle
    GPUPE_PERF_PAPC_SU_BUSY = 99, // Number of clocks Setup is Busy
    GPUPE_PERF_PAPC_SU_STARVED_CLIP = 100, // Number of clocks Setup is starved by Clipper
    GPUPE_PERF_PAPC_SU_STALLED_SC = 101, // Number of clocks Setup is stalled by SC
} GPUPERFEVENT_PA_SU;

typedef enum
{
    GPUPE_SC_SR_WINDOW_VALID = 0, // Number of clocks event-window is valid at stage_reg
    GPUPE_SC_CW_WINDOW_VALID = 1, // Number of clocks event-window is valid at coarse_walker
    GPUPE_SC_QM_WINDOW_VALID = 2, // Number of clocks event-window is valid at quadmask
    GPUPE_SC_QPP_WINDOW_VALID = 3, // Number of clocks event-window is valid at quad pair proc
    GPUPE_SC_ITER_WINDOW_VALID = 4, // Number of clocks event-window is valid at iter
    GPUPE_SC_STARVED_BY_PA = 5, // sc_rtr and not pa_rts and sc_busy
    GPUPE_SC_STARVED_BY_RCC = 6, // sc_tile_fifo full, rcc z return fifo empty
    GPUPE_SC_STALLED_BY_PRIM_FF = 7, // sc primitive fifo full is causing a stall
    GPUPE_SC_STALLED_BY_RCC = 8, // sc_rts and not rcc_rtr
    GPUPE_SC_STALLED_BY_BC = 9, // sc_rts and not bc_rtr
    GPUPE_SC_STALLED_BY_SX = 10, // sc_rts and not (sx0_rtr and sx1_rtr)
    GPUPE_SC_STALLED_BY_SX0 = 11, // sc_rts and not sx0_rtr
    GPUPE_SC_STALLED_BY_SX1 = 12, // sc_rts and not sx1_rtr
    GPUPE_SC_STALLED_BY_SQ = 13, // sc_sq count >= max
    GPUPE_SC_STALLED_BY_SP = 14, // sc_sp count >= max
    GPUPE_SC_WAIT_FOR_R1 = 15, // qpp has a single quad and is waiting for another tile for grouping
    GPUPE_SC_SCISSOR_DISCARD = 16, // prim completely discarded by scissor
    GPUPE_SC_BB_DISCARD = 17, // prim discarded by bounding-box check, no pixels hit
    GPUPE_SC_SUPERT_PRIM_DISCARD = 18, // prim completely discarded by super_tile optimization
    GPUPE_SC_RT_PRIM = 19, // real-time prim count
    GPUPE_SC_TILE_VALID = 20, // tile count
    GPUPE_SC_TILE_PER_PRIM_H0 = 21, // prims with < 2   tiles
    GPUPE_SC_TILE_PER_PRIM_H1 = 22, // prims with < 4   tiles
    GPUPE_SC_TILE_PER_PRIM_H2 = 23, // prims with < 8   tiles
    GPUPE_SC_TILE_PER_PRIM_H3 = 24, // prims with < 16  tiles
    GPUPE_SC_TILE_PER_PRIM_H4 = 25, // prims with < 32  tiles
    GPUPE_SC_TILE_PER_PRIM_H5 = 26, // prims with < 64  tiles
    GPUPE_SC_TILE_PER_PRIM_H6 = 27, // prims with < 128 tiles
    GPUPE_SC_TILE_PER_PRIM_H7 = 28, // prims with < 256 tiles
    GPUPE_SC_TILE_PER_PRIM_H8 = 29, // prims with < 512 tiles
    GPUPE_SC_TILE_PER_PRIM_H9 = 30, // prims with < 1K  tiles
    GPUPE_SC_TILE_PER_PRIM_H10 = 31, // prims with < 2K  tiles
    GPUPE_SC_TILE_PER_PRIM_H11 = 32, // prims with < 4K  tiles
    GPUPE_SC_TILE_PER_PRIM_H12 = 33, // prims with < 8K  tiles
    GPUPE_SC_TILE_PER_PRIM_H13 = 34, // prims with < 16K tiles
    GPUPE_SC_TILE_PER_PRIM_H14 = 35, // prims with < 32K tiles
    GPUPE_SC_TILE_PER_PRIM_H15 = 36, // prims with < 64K tiles
    GPUPE_SC_TILE_PER_PRIM_H16 = 37, // prims with < 1M  tiles
    GPUPE_SC_SUPERT_TILE_DISCARD = 38, // tiles discarded by super_tile optimization
    GPUPE_SC_QM_NUM_QUADS = 39, // total quads hit by coarsewalk
    GPUPE_SC_QM_MASK_H0 = 40, // tiles walked with 0  quads hit
    GPUPE_SC_QM_MASK_H1 = 41, // tiles walked with 1  quads hit
    GPUPE_SC_QM_MASK_H2 = 42, // tiles walked with 2  quads hit
    GPUPE_SC_QM_MASK_H3 = 43, // tiles walked with 3  quads hit
    GPUPE_SC_QM_MASK_H4 = 44, // tiles walked with 4  quads hit
    GPUPE_SC_QM_MASK_H5 = 45, // tiles walked with 5  quads hit
    GPUPE_SC_QM_MASK_H6 = 46, // tiles walked with 6  quads hit
    GPUPE_SC_QM_MASK_H7 = 47, // tiles walked with 7  quads hit
    GPUPE_SC_QM_MASK_H8 = 48, // tiles walked with 8  quads hit
    GPUPE_SC_QM_MASK_H9 = 49, // tiles walked with 9  quads hit
    GPUPE_SC_QM_MASK_H10 = 50, // tiles walked with 10 quads hit
    GPUPE_SC_QM_MASK_H11 = 51, // tiles walked with 11 quads hit
    GPUPE_SC_QM_MASK_H12 = 52, // tiles walked with 12 quads hit
    GPUPE_SC_QM_MASK_H13 = 53, // tiles walked with 13 quads hit
    GPUPE_SC_QM_MASK_H14 = 54, // tiles walked with 14 quads hit
    GPUPE_SC_QM_MASK_H15 = 55, // tiles walked with 15 quads hit
    GPUPE_SC_QM_MASK_H16 = 56, // tiles walked with 16 quads hit
    GPUPE_SC_QM_COVERED_H0 = 57, // tiles walked with 0  quads covered
    GPUPE_SC_QM_COVERED_H1 = 58, // tiles walked with 1  quads covered
    GPUPE_SC_QM_COVERED_H2 = 59, // tiles walked with 2  quads covered
    GPUPE_SC_QM_COVERED_H3 = 60, // tiles walked with 3  quads covered
    GPUPE_SC_QM_COVERED_H4 = 61, // tiles walked with 4  quads covered
    GPUPE_SC_QM_COVERED_H5 = 62, // tiles walked with 5  quads covered
    GPUPE_SC_QM_COVERED_H6 = 63, // tiles walked with 6  quads covered
    GPUPE_SC_QM_COVERED_H7 = 64, // tiles walked with 7  quads covered
    GPUPE_SC_QM_COVERED_H8 = 65, // tiles walked with 8  quads covered
    GPUPE_SC_QM_COVERED_H9 = 66, // tiles walked with 9  quads covered
    GPUPE_SC_QM_COVERED_H10 = 67, // tiles walked with 10 quads covered
    GPUPE_SC_QM_COVERED_H11 = 68, // tiles walked with 11 quads covered
    GPUPE_SC_QM_COVERED_H12 = 69, // tiles walked with 12 quads covered
    GPUPE_SC_QM_COVERED_H13 = 70, // tiles walked with 13 quads covered
    GPUPE_SC_QM_COVERED_H14 = 71, // tiles walked with 14 quads covered
    GPUPE_SC_QM_COVERED_H15 = 72, // tiles walked with 15 quads covered
    GPUPE_SC_QM_COVERED_H16 = 73, // tiles walked with 16 quads covered
    GPUPE_SC_HIER_NUM_QUADS = 74, // total quads surviving z
    GPUPE_SC_HIER_MASK_H0 = 75, // tiles with 0  quads surviving z
    GPUPE_SC_HIER_MASK_H1 = 76, // tiles with 1  quads surviving z
    GPUPE_SC_HIER_MASK_H2 = 77, // tiles with 2  quads surviving z
    GPUPE_SC_HIER_MASK_H3 = 78, // tiles with 3  quads surviving z
    GPUPE_SC_HIER_MASK_H4 = 79, // tiles with 4  quads surviving z
    GPUPE_SC_HIER_MASK_H5 = 80, // tiles with 5  quads surviving z
    GPUPE_SC_HIER_MASK_H6 = 81, // tiles with 6  quads surviving z
    GPUPE_SC_HIER_MASK_H7 = 82, // tiles with 7  quads surviving z
    GPUPE_SC_HIER_MASK_H8 = 83, // tiles with 8  quads surviving z
    GPUPE_SC_HIER_MASK_H9 = 84, // tiles with 9  quads surviving z
    GPUPE_SC_HIER_MASK_H10 = 85, // tiles with 10 quads surviving z
    GPUPE_SC_HIER_MASK_H11 = 86, // tiles with 11 quads surviving z
    GPUPE_SC_HIER_MASK_H12 = 87, // tiles with 12 quads surviving z
    GPUPE_SC_HIER_MASK_H13 = 88, // tiles with 13 quads surviving z
    GPUPE_SC_HIER_MASK_H14 = 89, // tiles with 14 quads surviving z
    GPUPE_SC_HIER_MASK_H15 = 90, // tiles with 15 quads surviving z
    GPUPE_SC_HIER_MASK_H16 = 91, // tiles with 16 quads surviving z
    GPUPE_SC_DET_NUM_QUADS = 92, // total quads surviving detail sampler
    GPUPE_SC_PKR_QD_PER_ROW_H1 = 93, // packer row outputs with 1 valid quad
    GPUPE_SC_PKR_QD_PER_ROW_H2 = 94, // packer row outputs with 2 valid quad
    GPUPE_SC_PKR_QD_PER_ROW_H3 = 95, // packer row outputs with 3 valid quad
    GPUPE_SC_PKR_QD_PER_ROW_H4 = 96, // packer row outputs with 4 valid quad
    GPUPE_SC_PKR_END_OF_VECTOR = 97, // number of pixel vectors
    GPUPE_SC_PKR_ONE_CLK = 98, // number of one clock commands
    GPUPE_SC_QD_WITH_1_PIX = 99, // quads with 1 pixel surviving detail
    GPUPE_SC_QD_WITH_2_PIX = 100, // quads with 2 pixels surviving detail
    GPUPE_SC_QD_WITH_3_PIX = 101, // quads with 3 pixels surviving detail
    GPUPE_SC_QD_WITH_4_PIX = 102, // quads with 4 pixels surviving detail
    GPUPE_SC_SR_WINDOW_VALID_BUSY = 103, // Event-window is valid at stage_reg with sc busy
    GPUPE_SC_CW_WINDOW_VALID_BUSY = 104, // Event-window is valid at coarse_walker with sc busy
    GPUPE_SC_QM_WINDOW_VALID_BUSY = 105, // Event-window is valid at quadmask with sc busy
    GPUPE_SC_QPP_WINDOW_VALID_BUSY = 106, // Event-window is valid at quad pair proc with sc busy
    GPUPE_SC_ITER_WINDOW_VALID_BUSY = 107, // Event-window is valid at iter with sc busy
} GPUPERFEVENT_PA_SC;

typedef enum
{
    GPUPE_WRC_1VAL_QUADS = 0, // Number of writes to hz_mem with 1 valid quad
    GPUPE_WRC_2VAL_QUADS = 1, // Number of writes to hz_mem with 2 valid quads
    GPUPE_WRC_3VAL_QUADS = 2, // Number of writes to hz_mem with 3 valid quads
    GPUPE_WRC_4VAL_QUADS = 3, // Number of writes to hz_mem with 4 valid quads
    GPUPE_HZ_WR_BUSY = 4, // Modules on the write-side of HZ memory are busy
    GPUPE_HZ_SPARE0 = 5, // Reserved for HZ
    GPUPE_RDC_TILE_HIT = 6, // Number of tile hits in the Read Cache
    GPUPE_RDC_STAGE3_STALL = 7, // Number of conflicts/stalls in Read Stage 3
    GPUPE_HZ_SPARE1 = 8, // Reserved for HZ
    GPUPE_HZ_SPARE2 = 9, // Reserved for HZ
    GPUPE_QUADS_KEPT = 10, // Number of quads kept
    GPUPE_QUADS_ZCULL = 11, // Number of quads culled due to Z only
    GPUPE_QUADS_SCULL = 12, // Number of quads culled due to Stencil only
    GPUPE_QUADS_SZCULL = 13, // Number of quads culled due to both Z and Stencil
    GPUPE_HZ_RE_BUSY = 14, // Modules on the read-side of HZ memory are busy
    GPUPE_BC_SCLK_COUNT = 15, // SCLK count for BC
    GPUPE_BC_HZ_VALID0 = 16, // Valid input data from BC bus 0
    GPUPE_BC_HZ_VALID1 = 17, // Valid input data from BC bus 1
    GPUPE_BC_HZ_VALID2 = 18, // Valid input data from BC bus 2
    GPUPE_BC_HZ_VALID3 = 19, // Valid input data from BC bus 3
    GPUPE_SC_SCLK_COUNT = 20, // SCLK count for SC
    GPUPE_SC_HZ_COARSE_SND = 21, // SC sending coarse Z data to HZ
    GPUPE_HZ_SC_COARSE_HLD = 22, // HZ holding SC from sending coarse Z data
    GPUPE_HZ_SC_HIER_SND = 23, // HZ sending mask data to BC
    GPUPE_SC_HZ_HIER_HLD = 24, // SC holding HZ from sending mask data
    GPUPE_HZ_BC_TILE_SND = 25, // HZ sending tile data to BC
    GPUPE_BC_HZ_TILE_HLD = 26, // BC holding HZ from sending tile data
    GPUPE_SC_STALL_HZ = 27, // HZ stalled by SC
    GPUPE_BC_STALL_HZ = 28, // HZ stalled by BC
    GPUPE_EVENT_STALL = 29, // HZ stalled by an Outstanding Event
    GPUPE_SC_STARVE_HZ_BUSY = 30, // HZ starved by the SC when the HZ is busy
    GPUPE_SC_STARVE_HZ_IDLE = 31, // HZ starved by the SC when the HZ is idle
} GPUPERFEVENT_HZ;

typedef enum
{
    GPUPE_DGMMPD_IPMUX0_STALL = 0, // Number of clocks ipmux0 is stalled
    GPUPE_DGMMPD_IPMUX1_STALL = 1, // Number of clocks ipmux1 is stalled
    GPUPE_DGMMPD_IPMUX2_STALL = 2, // Number of clocks ipmux2 is stalled
    GPUPE_DGMMPD_IPMUX3_STALL = 3, // Number of clocks ipmux3 is stalled
    GPUPE_DGMMPD_IPMUX_ALL_STALL = 4, // Number of clocks all four ipmuxes are stalled
    GPUPE_OPMUX0_L2_WRITES = 5, // Number of opmux0 L2 writes
    GPUPE_OPMUX1_L2_WRITES = 6, // Number of opmux1 L2 writes
    GPUPE_OPMUX2_L2_WRITES = 7, // Number of opmux2 L2 writes
    GPUPE_OPMUX3_L2_WRITES = 8, // Number of opmux3 L2 writes
} GPUPERFEVENT_TCR; // Texture Cache Return

typedef enum
{
    GPUPE_QUAD0_RD_LAT_FIFO_EMPTY = 0, // Number of clocks Quad0 read latency fifo is empty
    GPUPE_QUAD0_RD_LAT_FIFO_16TH_FULL = 1, // Number of clocks Quad0 read latency fifo is at least 1/16th full, but less than 1/8th
    GPUPE_QUAD0_RD_LAT_FIFO_8TH_FULL = 2, // Number of clocks Quad0 read latency fifo is at least 1/8th full, but less than 1/4th
    GPUPE_QUAD0_RD_LAT_FIFO_4TH_FULL = 3, // Number of clocks Quad0 read latency fifo is at least 1/4th full, but less than half
    GPUPE_QUAD0_RD_LAT_FIFO_HALF_FULL = 4, // Number of clocks Quad0 read latency fifo is at least half full, but not full
    GPUPE_QUAD0_RD_LAT_FIFO_FULL = 5, // Number of clocks Quad0 read latency fifo is full
    GPUPE_QUAD0_RD_LAT_FIFO_LT_16TH_FULL = 6, // Number of clocks Quad0 read latency fifo is less than 1/16th full, but not empty
    GPUPE_QUAD1_RD_LAT_FIFO_EMPTY = 7, // Number of clocks Quad1 read latency fifo is empty
    GPUPE_QUAD1_RD_LAT_FIFO_16TH_FULL = 8, // Number of clocks Quad1 read latency fifo is at least 1/16th full, but less than 1/8th
    GPUPE_QUAD1_RD_LAT_FIFO_8TH_FULL = 9, // Number of clocks Quad1 read latency fifo is at least 1/8th full, but less than 1/4th
    GPUPE_QUAD1_RD_LAT_FIFO_4TH_FULL = 10, // Number of clocks Quad1 read latency fifo is at least 1/4th full, but less than half
    GPUPE_QUAD1_RD_LAT_FIFO_HALF_FULL = 11, // Number of clocks Quad1 read latency fifo is at least half full, but not full
    GPUPE_QUAD1_RD_LAT_FIFO_FULL = 12, // Number of clocks Quad1 read latency fifo is full
    GPUPE_QUAD1_RD_LAT_FIFO_LT_16TH_FULL = 13, // Number of clocks Quad1 read latency fifo is less than 1/16th full, but not empty
    GPUPE_QUAD2_RD_LAT_FIFO_EMPTY = 14, // Number of clocks Quad2 read latency fifo is empty
    GPUPE_QUAD2_RD_LAT_FIFO_16TH_FULL = 15, // Number of clocks Quad2 read latency fifo is at least 1/16th full, but less than 1/8th
    GPUPE_QUAD2_RD_LAT_FIFO_8TH_FULL = 16, // Number of clocks Quad2 read latency fifo is at least 1/8th full, but less than 1/4th
    GPUPE_QUAD2_RD_LAT_FIFO_4TH_FULL = 17, // Number of clocks Quad2 read latency fifo is at least 1/4th full, but less than half
    GPUPE_QUAD2_RD_LAT_FIFO_HALF_FULL = 18, // Number of clocks Quad2 read latency fifo is at least half full, but not full
    GPUPE_QUAD2_RD_LAT_FIFO_FULL = 19, // Number of clocks Quad2 read latency fifo is full
    GPUPE_QUAD2_RD_LAT_FIFO_LT_16TH_FULL = 20, // Number of clocks Quad2 read latency fifo is less than 1/16th full, but not empty
    GPUPE_QUAD3_RD_LAT_FIFO_EMPTY = 21, // Number of clocks Quad3 read latency fifo is empty
    GPUPE_QUAD3_RD_LAT_FIFO_16TH_FULL = 22, // Number of clocks Quad3 read latency fifo is at least 1/16th full, but less than 1/8th
    GPUPE_QUAD3_RD_LAT_FIFO_8TH_FULL = 23, // Number of clocks Quad3 read latency fifo is at least 1/8th full, but less than 1/4th
    GPUPE_QUAD3_RD_LAT_FIFO_4TH_FULL = 24, // Number of clocks Quad3 read latency fifo is at least 1/4th full, but less than half
    GPUPE_QUAD3_RD_LAT_FIFO_HALF_FULL = 25, // Number of clocks Quad3 read latency fifo is at least half full, but not full
    GPUPE_QUAD3_RD_LAT_FIFO_FULL = 26, // Number of clocks Quad3 read latency fifo is full
    GPUPE_QUAD3_RD_LAT_FIFO_LT_16TH_FULL = 27, // Number of clocks Quad3 read latency fifo is less than 1/16th full, but not empty
    GPUPE_READ_STARVED_QUAD0 = 28, // Number of clocks Quad0 is starved waiting for data from MC
    GPUPE_READ_STARVED_QUAD1 = 29, // Number of clocks Quad1 is starved waiting for data from MC
    GPUPE_READ_STARVED_QUAD2 = 30, // Number of clocks Quad2 is starved waiting for data from MC
    GPUPE_READ_STARVED_QUAD3 = 31, // Number of clocks Quad3 is starved waiting for data from MC
    GPUPE_READ_STARVED = 32, // Number of clocks any quad is starved waiting for data from MC
    GPUPE_READ_STALLED_QUAD0 = 33, // Number of clocks Quad0 a read is stalled off by waiting for other quads to sync up
    GPUPE_READ_STALLED_QUAD1 = 34, // Number of clocks Quad1 a read is stalled off by waiting for other quads to sync up
    GPUPE_READ_STALLED_QUAD2 = 35, // Number of clocks Quad2 a read is stalled off by waiting for other quads to sync up
    GPUPE_READ_STALLED_QUAD3 = 36, // Number of clocks Quad3 a read is stalled off by waiting for other quads to sync up
    GPUPE_READ_STALLED = 37, // Number of clocks a read is stalled off by waiting for other quads to sync up
    GPUPE_VALID_READ_QUAD0 = 38, // Valid cycles of cache reads on Quad0
    GPUPE_VALID_READ_QUAD1 = 39, // Valid cycles of cache reads on Quad1
    GPUPE_VALID_READ_QUAD2 = 40, // Valid cycles of cache reads on Quad2
    GPUPE_VALID_READ_QUAD3 = 41, // Valid cycles of cache reads on Quad3
    GPUPE_TC_TP_STARVED_QUAD0 = 42, // Quad0 is waiting for another quad to be valid before sending to TP
    GPUPE_TC_TP_STARVED_QUAD1 = 43, // Quad1 is waiting for another quad to be valid before sending to TP
    GPUPE_TC_TP_STARVED_QUAD2 = 44, // Quad2 is waiting for another quad to be valid before sending to TP
    GPUPE_TC_TP_STARVED_QUAD3 = 45, // Quad3 is waiting for another quad to be valid before sending to TP
    GPUPE_TC_TP_STARVED = 46, // Some data is ready for the TP, but stalled waiting for the rest
} GPUPERFEVENT_TCM;  // Texture Cache Memory

typedef enum
{
    GPUPE_VALID_CYCLES  = 0,
    GPUPE_SINGLE_PHASES  = 1,
    GPUPE_ANISO_PHASES  = 2,
    GPUPE_MIP_PHASES  = 3,
    GPUPE_VOL_PHASES  = 4,
    GPUPE_MIP_VOL_PHASES  = 5,
    GPUPE_MIP_ANISO_PHASES  = 6,
    GPUPE_VOL_ANISO_PHASES  = 7,
    GPUPE_ANISO_2_1_PHASES  = 8,
    GPUPE_ANISO_4_1_PHASES  = 9,
    GPUPE_ANISO_6_1_PHASES  = 0,
    GPUPE_ANISO_8_1_PHASES  = 11,
    GPUPE_ANISO_10_1_PHASES  = 12,
    GPUPE_ANISO_12_1_PHASES  = 13,
    GPUPE_ANISO_14_1_PHASES  = 14,
    GPUPE_ANISO_16_1_PHASES  = 15,
    GPUPE_MIP_VOL_ANISO_PHASES  = 16,
    GPUPE_ALIGN_2_PHASES  = 17,
    GPUPE_ALIGN_4_PHASES  = 18,
    GPUPE_TPC_BUSY  = 19,
    GPUPE_TPC_STALLED  = 20,
    GPUPE_TPC_STARVED  = 21,
    GPUPE_TPC_WORKING  = 22,
    GPUPE_TPC_WALKER_BUSY  = 23,
    GPUPE_TPC_WALKER_STALLED  = 24,
    GPUPE_TPC_WALKER_WORKING  = 25,
    GPUPE_TPC_ALIGNER_BUSY  = 26,
    GPUPE_TPC_ALIGNER_STALLED  = 27,
    GPUPE_TPC_ALIGNER_STALLED_BY_BLEND  = 28,
    GPUPE_TPC_ALIGNER_STALLED_BY_CACHE  = 29,
    GPUPE_TPC_ALIGNER_WORKING  = 30,
    GPUPE_TPC_BLEND_BUSY  = 31,
    GPUPE_TPC_BLEND_SYNC  = 32,
    GPUPE_TPC_BLEND_STARVED  = 33,
    GPUPE_TPC_BLEND_WORKING  = 34,
    GPUPE_OPCODE_0X00  = 35,
    GPUPE_OPCODE_0X01  = 36,
    GPUPE_OPCODE_0X04  = 37,
    GPUPE_OPCODE_0X10  = 38,
    GPUPE_OPCODE_0X11  = 39,
    GPUPE_OPCODE_0X12  = 40,
    GPUPE_OPCODE_0X13  = 41,
    GPUPE_OPCODE_0X18  = 42,
    GPUPE_OPCODE_0X19  = 43,
    GPUPE_OPCODE_0X1A  = 44,
    GPUPE_OPCODE_OTHER  = 45,
    GPUPE_RESERVED_46  = 46,
    GPUPE_RESERVED_47  = 47,
    GPUPE_RESERVED_48  = 48,
    GPUPE_RESERVED_49  = 49,
    GPUPE_RESERVED_50  = 50,
    GPUPE_RESERVED_51  = 51,
    GPUPE_RESERVED_52  = 52,
    GPUPE_RESERVED_53  = 53,
    GPUPE_RESERVED_54  = 54,
    GPUPE_RESERVED_55  = 55,
    GPUPE_IN_FIFO_0_EMPTY  = 56,
    GPUPE_IN_FIFO_0_LT_HALF_FULL  = 57,
    GPUPE_IN_FIFO_0_HALF_FULL  = 58,
    GPUPE_IN_FIFO_0_FULL  = 59,
    GPUPE_IN_FIFO_1_EMPTY  = 60,
    GPUPE_IN_FIFO_1_LT_HALF_FULL  = 61,
    GPUPE_IN_FIFO_1_HALF_FULL  = 62,
    GPUPE_IN_FIFO_1_FULL  = 63,
    GPUPE_IN_FIFO_2_EMPTY  = 64,
    GPUPE_IN_FIFO_2_LT_HALF_FULL  = 65,
    GPUPE_IN_FIFO_2_HALF_FULL  = 66,
    GPUPE_IN_FIFO_2_FULL  = 67,
    GPUPE_IN_FIFO_3_EMPTY  = 68,
    GPUPE_IN_FIFO_3_LT_HALF_FULL  = 69,
    GPUPE_IN_FIFO_3_HALF_FULL  = 70,
    GPUPE_IN_FIFO_3_FULL  = 71,
    GPUPE_IN_FIFO_TPC_EMPTY  = 72,
    GPUPE_IN_FIFO_TPC_LT_HALF_FULL  = 73,
    GPUPE_IN_FIFO_TPC_HALF_FULL  = 74,
    GPUPE_IN_FIFO_TPC_FULL  = 75,
    GPUPE_TPC_TC_XFC  = 76,
    GPUPE_TPC_TC_STATE  = 77,
    GPUPE_TC_STALL  = 78,
    GPUPE_QUAD0_TAPS  = 79,
    GPUPE_QUAD1_TAPS  = 80,
    GPUPE_QUAD2_TAPS  = 81,
    GPUPE_QUAD3_TAPS  = 82,
    GPUPE_QUADS  = 83,
    GPUPE_TCA_SYNC_STALL  = 84,
    GPUPE_TAG_STALL  = 85,
    GPUPE_SLICE_STALL  = 86,
    GPUPE_SKEW_STALL  = 87,
    GPUPE_TCB_SYNC_STALL  = 88,
    GPUPE_TCA_VALID  = 89,
    GPUPE_PROBES_VALID  = 90,
    GPUPE_MISS_STALL  = 91,
    GPUPE_FETCH_FIFO_STALL  = 92,
    GPUPE_TCO_STALL  = 93,
    GPUPE_ANY_STALL  = 94,
    GPUPE_TAG_MISSES  = 95,
    GPUPE_TAG_HITS  = 96,
    GPUPE_SUB_TAG_MISSES  = 97,
    GPUPE_SET0_INVALIDATES  = 98,
    GPUPE_SET1_INVALIDATES  = 99,
    GPUPE_SET2_INVALIDATES  = 100,
    GPUPE_SET3_INVALIDATES  = 101,
    GPUPE_SET0_TAG_MISSES  = 102,
    GPUPE_SET1_TAG_MISSES  = 103,
    GPUPE_SET2_TAG_MISSES  = 104,
    GPUPE_SET3_TAG_MISSES  = 105,
    GPUPE_SET0_TAG_HITS  = 106,
    GPUPE_SET1_TAG_HITS  = 107,
    GPUPE_SET2_TAG_HITS  = 108,
    GPUPE_SET3_TAG_HITS  = 109,
    GPUPE_SET0_SUB_TAG_MISSES  = 110,
    GPUPE_SET1_SUB_TAG_MISSES  = 111,
    GPUPE_SET2_SUB_TAG_MISSES  = 112,
    GPUPE_SET3_SUB_TAG_MISSES  = 113,
    GPUPE_SET0_EVICT1  = 114,
    GPUPE_SET0_EVICT2  = 115,
    GPUPE_SET0_EVICT3  = 116,
    GPUPE_SET0_EVICT4  = 117,
    GPUPE_SET0_EVICT5  = 118,
    GPUPE_SET0_EVICT6  = 119,
    GPUPE_SET0_EVICT7  = 120,
    GPUPE_SET0_EVICT8  = 121,
    GPUPE_SET0_EVICT9  = 122,
    GPUPE_SET0_EVICT10  = 123,
    GPUPE_SET0_EVICT11  = 124,
    GPUPE_SET0_EVICT12  = 125,
    GPUPE_SET0_EVICT13  = 126,
    GPUPE_SET0_EVICT14  = 127,
    GPUPE_SET0_EVICT15  = 128,
    GPUPE_SET0_EVICT16  = 129,
    GPUPE_SET1_EVICT1  = 130,
    GPUPE_SET1_EVICT2  = 131,
    GPUPE_SET1_EVICT3  = 132,
    GPUPE_SET1_EVICT4  = 133,
    GPUPE_SET1_EVICT5  = 134,
    GPUPE_SET1_EVICT6  = 135,
    GPUPE_SET1_EVICT7  = 136,
    GPUPE_SET1_EVICT8  = 137,
    GPUPE_SET1_EVICT9  = 138,
    GPUPE_SET1_EVICT10  = 139,
    GPUPE_SET1_EVICT11  = 140,
    GPUPE_SET1_EVICT12  = 141,
    GPUPE_SET1_EVICT13  = 142,
    GPUPE_SET1_EVICT14  = 143,
    GPUPE_SET1_EVICT15  = 144,
    GPUPE_SET1_EVICT16  = 145,
    GPUPE_SET2_EVICT1  = 146,
    GPUPE_SET2_EVICT2  = 147,
    GPUPE_SET2_EVICT3  = 148,
    GPUPE_SET2_EVICT4  = 149,
    GPUPE_SET2_EVICT5  = 150,
    GPUPE_SET2_EVICT6  = 151,
    GPUPE_SET2_EVICT7  = 152,
    GPUPE_SET2_EVICT8  = 153,
    GPUPE_SET2_EVICT9  = 154,
    GPUPE_SET2_EVICT10  = 155,
    GPUPE_SET2_EVICT11  = 156,
    GPUPE_SET2_EVICT12  = 157,
    GPUPE_SET2_EVICT13  = 158,
    GPUPE_SET2_EVICT14  = 159,
    GPUPE_SET2_EVICT15  = 160,
    GPUPE_SET2_EVICT16  = 161,
    GPUPE_SET3_EVICT1  = 162,
    GPUPE_SET3_EVICT2  = 163,
    GPUPE_SET3_EVICT3  = 164,
    GPUPE_SET3_EVICT4  = 165,
    GPUPE_SET3_EVICT5  = 166,
    GPUPE_SET3_EVICT6  = 167,
    GPUPE_SET3_EVICT7  = 168,
    GPUPE_SET3_EVICT8  = 169,
    GPUPE_SET3_EVICT9  = 170,
    GPUPE_SET3_EVICT10  = 171,
    GPUPE_SET3_EVICT11  = 172,
    GPUPE_SET3_EVICT12  = 173,
    GPUPE_SET3_EVICT13  = 174,
    GPUPE_SET3_EVICT14  = 175,
    GPUPE_SET3_EVICT15  = 176,
    GPUPE_SET3_EVICT16  = 177,
    GPUPE_FF_EMPTY  = 178,
    GPUPE_FF_LT_HALF_FULL  = 179,
    GPUPE_FF_HALF_FULL  = 180,
    GPUPE_FF_FULL  = 181,
    GPUPE_FF_XFC  = 182,
    GPUPE_FF_STALLED  = 183,
    GPUPE_FG_MASKS  = 184,
    GPUPE_FG_LEFT_MASKS  = 185,
    GPUPE_FG_LEFT_MASK_STALLED  = 186,
    GPUPE_FG_LEFT_NOT_DONE_STALL  = 187,
    GPUPE_FG_LEFT_FG_STALL  = 188,
    GPUPE_FG_LEFT_SECTORS  = 189,
    GPUPE_FG_RIGHT_MASKS  = 190,
    GPUPE_FG_RIGHT_MASK_STALLED  = 191,
    GPUPE_FG_RIGHT_NOT_DONE_STALL  = 192,
    GPUPE_FG_RIGHT_FG_STALL  = 193,
    GPUPE_FG_RIGHT_SECTORS  = 194,
    GPUPE_FG0_REQUESTS  = 195,
    GPUPE_FG0_STALLED  = 196,
    GPUPE_FG1_REQUESTS  = 197,
    GPUPE_FG1_STALLED  = 198,
    GPUPE_MEM_REQ512  = 199,
    GPUPE_MEM_REQ_SENT  = 200,
    GPUPE_MEM_AGP_READ_REQ  = 201,
    GPUPE_MEM_LOCAL_READ_REQ  = 202,
    GPUPE_TC0_MH_STALLED  = 203,
    GPUPE_TC1_MH_STALLED  = 204,
} GPUPERFEVENT_TCF;  // Texture Cache Fetch

typedef enum
{
    GPUPE_POINT_QUADS = 0, // Any Point (including 1:1 aniso) sampled quads
    GPUPE_BILIN_QUADS = 1, // Any Bilinearly (including 1:1 aniso) filtered quads
    GPUPE_ANISO_QUADS = 2, // Any Aniso (>1:1) filtered quads
    GPUPE_MIP_QUADS = 3, // Any mip filtered quads
    GPUPE_VOL_QUADS = 4, // Any volume filtered quads
    GPUPE_MIP_VOL_QUADS = 5, // Mip and volume filtered quads
    GPUPE_MIP_ANISO_QUADS = 6, // Mip and aniso (>1:1) filtered quads
    GPUPE_VOL_ANISO_QUADS = 7, // Volume and aniso (>1:1) filtered quads
    GPUPE_ANISO_2_1_QUADS = 8, // Any quads with 2:1 anisotropic filtering
    GPUPE_ANISO_4_1_QUADS = 9, // Any quads with 4:1 anisotropic filtering
    GPUPE_ANISO_6_1_QUADS = 10, // Any quads with 6:1 anisotropic filtering
    GPUPE_ANISO_8_1_QUADS = 11, // Any quads with 8:1 anisotropic filtering
    GPUPE_ANISO_10_1_QUADS = 12, // Any quads with 10:1 anisotropic filtering
    GPUPE_ANISO_12_1_QUADS = 13, // Any quads with 12:1 anisotropic filtering
    GPUPE_ANISO_14_1_QUADS = 14, // Any quads with 14:1 anisotropic filtering
    GPUPE_MIP_VOL_ANISO_QUADS = 15, // Mip, volume and aniso (>1:1) filtered quads
    GPUPE_ALIGN_2_QUADS = 16, // 2-cycle misaligned quads
    GPUPE_ALIGN_4_QUADS = 17, // 4-cycle misaligned quads
    GPUPE_PIX_0_QUAD = 18, // No valid pixels in quad
    GPUPE_PIX_1_QUAD = 19, // 1 valid pixel in quad
    GPUPE_PIX_2_QUAD = 20, // 2 valid pixels in quad
    GPUPE_PIX_3_QUAD = 21, // 3 valid pixels in quad
    GPUPE_PIX_4_QUAD = 22, // 4 valid pixels in quad
    GPUPE_TP_MIPMAP_LOD0 = 23, // MipMap LOD 0
    GPUPE_TP_MIPMAP_LOD1 = 24, // MipMap LOD 1
    GPUPE_TP_MIPMAP_LOD2 = 25, // MipMap LOD 2
    GPUPE_TP_MIPMAP_LOD3 = 26, // MipMap LOD 3
    GPUPE_TP_MIPMAP_LOD4 = 27, // MipMap LOD 4
    GPUPE_TP_MIPMAP_LOD5 = 28, // MipMap LOD 5
    GPUPE_TP_MIPMAP_LOD6 = 29, // MipMap LOD 6
    GPUPE_TP_MIPMAP_LOD7 = 30, // MipMap LOD 7
    GPUPE_TP_MIPMAP_LOD8 = 31, // MipMap LOD 8
    GPUPE_TP_MIPMAP_LOD9 = 32, // MipMap LOD 9
    GPUPE_TP_MIPMAP_LOD10 = 33, // MipMap LOD 10
    GPUPE_TP_MIPMAP_LOD11 = 34, // MipMap LOD 11
    GPUPE_TP_MIPMAP_LOD12 = 35, // MipMap LOD 12
    GPUPE_TP_MIPMAP_LOD13 = 36, // MipMap LOD 13
    GPUPE_TP_MIPMAP_LOD14 = 37, // MipMap LOD 14
} GPUPERFEVENT_TP;

typedef enum
{
    GPUPE_SX_SC_QUADS = 0, // Number of quads sent by the SC
    GPUPE_SX_SC_QUAD_FIFO_FULL = 1, // Number of cycles where the SC quad FIFO is full
    GPUPE_SX_EXPORT_VECTORS = 2, // Number of exported vectors
    GPUPE_SX_DUMMY_QUADS = 3, // Number of dummy quads
    GPUPE_SX_ALPHA_FAIL = 4, // Number of pixels that fail alpha test
    GPUPE_SX_RB_QUAD_BUSY = 5, // SX sending quads to the RBs
    GPUPE_SX_RB_COLOR_BUSY = 6, // SX sending colors to the RBs
    GPUPE_SX_RB_QUAD_STALL = 7, // SX idle on the quad interface
    GPUPE_SX_RB_COLOR_STALL = 8, // SX idle on the color interface
} GPUPERFEVENT_SX;

typedef enum
{
    GPUPE_BC_CNTX0_BUSY = 0, // Number of cycles BC is busy processing data(bc_context0_busy)
    GPUPE_BC_CNTX17_BUSY = 1, // Number of cycles BC is busy processing data(bc_context17_busy)
    GPUPE_BC_RQ_STALLED = 2, // Number of cycles BC is stalled due to no available banks for reorder queue to select
    GPUPE_BC_AZ_STALLED = 3, // Number of cycles BC is stalled by AZ
    GPUPE_BC_MH_CPY_STALLED = 4, // Number of cycles BC is stalled by MH for copy/resolve
    GPUPE_BC_MH_EXP_STALLED = 5, // Number of cycles BC is stalled by MH for memory exports
    GPUPE_BC_SC_STARVED = 6, // Number of cycles SC->BC quad fifo is empty and BC has a tile from HZ to work on
    GPUPE_BC_SX_STARVED = 7, // Number of cycles SX->BC quad fifo is empty and we're in color mode and BC has a tile and quads from HZ and SC
    GPUPE_BC_ACC_COUNT = 8, // Number of times multiple fragments are combined into a quad (acc opcodes)
    GPUPE_BC_DRAW_COUNT = 9, // Number of quads sent to AZ (draw opcodes)
    GPUPE_BC_ACC2_COUNT = 10, // Number of accumulate two opcodes for 64bpp and MRT's
    GPUPE_BC_DRAW2_COUNT = 11, // Number of draw two opcodes for 64bpp and MRT's
    GPUPE_BC_SETZ_COUNT = 12, // Number of depth exports
    GPUPE_BC_READ_COUNT = 13, // Number of read opcodes when resolving
    GPUPE_BC_READ_ACC_COUNT = 14, // Number of read_acc opcodes when resolving
    GPUPE_BC_STATE_COUNT = 15, // Number of state opcodes sent to AZ
    GPUPE_BC_STATE2_COUNT = 16, // Number of state2 opcodes sent to AZ
    GPUPE_BC_COPY_WRITE_COUNT = 17, // Number of 256-bit system memory writes for EDRAM copy/resolve
    GPUPE_BC_EXPORT_COUNT = 18, // Number of memory exports from SX
} GPUPERFEVENT_BC;

typedef enum
{
    GPUPE_RANK_BANK0_ACCESSES = 0, // Rank Bank 0 access event
    GPUPE_RANK_BANK1_ACCESSES = 1, // Rank Bank 1 access event
    GPUPE_RANK_BANK2_ACCESSES = 2, // Rank Bank 2 access event
    GPUPE_RANK_BANK3_ACCESSES = 3, // Rank Bank 3 access event
    GPUPE_RANK_BANK4_ACCESSES = 4, // Rank Bank 4 access event
    GPUPE_RANK_BANK5_ACCESSES = 5, // Rank Bank 5 access event
    GPUPE_RANK_BANK6_ACCESSES = 6, // Rank Bank 6 access event
    GPUPE_RANK_BANK7_ACCESSES = 7, // Rank Bank 7 access event
    GPUPE_RANK_BANK8_ACCESSES = 8, // Rank Bank 8 access event
    GPUPE_RANK_BANK9_ACCESSES = 9, // Rank Bank 9 access event
    GPUPE_RANK_BANK10_ACCESSES = 10, // Rank Bank 10 access event
    GPUPE_RANK_BANK11_ACCESSES = 11, // Rank Bank 11 access event
    GPUPE_RANK_BANK12_ACCESSES = 12, // Rank Bank 12 access event
    GPUPE_RANK_BANK13_ACCESSES = 13, // Rank Bank 13 access event
    GPUPE_RANK_BANK14_ACCESSES = 14, // Rank Bank 14 access event
    GPUPE_RANK_BANK15_ACCESSES = 15, // Rank Bank 15 access event
    GPUPE_READ_2_WRITE = 16, // Read to Write transition event
    GPUPE_WRITE_2_READ = 17, // Write to Read transition event
    GPUPE_NEW_PAGE_ACCESSES = 18, // Number of new page accesses out of the ordering engine
    GPUPE_TOTAL_ACCESSES = 19, // Total accesses out of the ordering engine
    GPUPE_READ_ACCESSES = 20, // Number of reads out of the ordering engine
    GPUPE_ACCESS_PRESENT_NO_ISSUE_CLKS = 21, // Number of clocks in which an access is present but ordering engine doesn't issue
    GPUPE_CMD_PRESENT_NO_XFER_CLKS = 22, // Number of non-transfer clocks on DRAM data bus when commands are present
    GPUPE_URGENT_DC_ACCESSES = 23, // Number of urgent accesses from DC queue
    GPUPE_URGENT_SB_ACCESSES = 24, // Number of urgent accesses from SB queue
    GPUPE_URGENT_BIU_ACCESSES = 25, // Number of urgent accesses from BIUS (slow) queue
    GPUPE_NEW_BIUF_ACCESSES = 26, // Number of new accesses from BIUF (fast) queue
    GPUPE_NEW_CP_ACCESSES = 27, // Number of new accesses from CP queue
    GPUPE_NEW_TC_ACCESSES = 28, // Number of new accesses from TC queue
    GPUPE_NEW_VC_ACCESSES = 29, // Number of new accesses from VC queue
    GPUPE_NEW_BC_CP_ACCESSES = 30, // Number of new accesses from BC_CP queue
    GPUPE_NEW_BC_EX_ACCESSES = 31, // Number of new accesses from BC_EX queue
    GPUPE_NEW_VGT_ACCESSES = 32, // Number of new accesses from VGT queue
    GPUPE_NEW_DC_ACCESSES = 33, // Number of new accesses from DC queue
    GPUPE_NEW_SB_ACCESSES = 34, // Number of new accesses from SB queue
    GPUPE_NEW_BIUS_ACCESSES = 35, // Number of new accesses from BIUS (slow) queue
    GPUPE_BIUS_READ_ACCESSES = 36, // Number of Read accesses from BIUS (slow) queue
    GPUPE_SB_READ_ACCESSES = 37, // Number of Read accesses from SB queue
    GPUPE_CP_READ_ACCESSES = 38, // Number of Read accesses from CP queue
    GPUPE_NTH_SMPG_ACCESS_IS_BIUF = 39, // Number of times the Nth access in a same page sequence is from BIUF (fast) queue
    GPUPE_NTH_SMPG_ACCESS_IS_CP = 40, // Number of times the Nth access in a same page sequence is from CP queue
    GPUPE_NTH_SMPG_ACCESS_IS_TC = 41, // Number of times the Nth access in a same page sequence is from TC queue
    GPUPE_NTH_SMPG_ACCESS_IS_VC = 42, // Number of times the Nth access in a same page sequence is from VC queue
    GPUPE_NTH_SMPG_ACCESS_IS_BC_CP = 43, // Number of times the Nth access in a same page sequence is from BC_CP queue
    GPUPE_NTH_SMPG_ACCESS_IS_BC_EX = 44, // Number of times the Nth access in a same page sequence is from BC_EX queue
    GPUPE_NTH_SMPG_ACCESS_IS_VGT = 45, // Number of times the Nth access in a same page sequence is from VGT queue
    GPUPE_NTH_SMPG_ACCESS_IS_DC = 46, // Number of times the Nth access in a same page sequence is from DC queue
    GPUPE_NTH_SMPG_ACCESS_IS_SB = 47, // Number of times the Nth access in a same page sequence is from SB queue
    GPUPE_NTH_SMPG_ACCESS_IS_BIUS = 48, // Number of times the Nth access in a same page sequence is from BIUS (slow) queue
    GPUPE_N_VALID_ENTRY_IN_TEXTURE_CAM_CLKS = 49, // Number of clocks where there are N valid entries in the texture cam
    GPUPE_N_VALID_ENTRY_IN_VC_CAM_CLKS = 50, // Number of clocks where there are N valid entries in the VC cam
    GPUPE_N_VALID_ENTRY_IN_BIUS_CAM_CLKS = 51, // Number of clocks where there are N valid entries in the BIUS cam
    GPUPE_N_SB_BUF_USED_CLKS = 52, // Number of clocks when q_rdbuf_sb_buffers_used = N
    GPUPE_N_TC_BUF_USED_CLKS = 53, // Number of clocks when q_rdbuf_tc_buffers_used = N
    GPUPE_N_VC_BUF_USED_CLKS = 54, // Number of clocks when q_rdbuf_vc_buffers_used = N
    GPUPE_N_DC_BUF_USED_CLKS = 55, // Number of clocks when q_rdbuf_dc_buffers_used = N
    GPUPE_N_VGT_BUF_USED_CLK = 56, // Number of clocks when q_rdbuf_vgt_buffers_used = N
    GPUPE_N_CP_BUF_USED_CLKS = 57, // Number of clocks when q_rdbuf_cp_buffers_used = N
} GPUPERFEVENT_MC;

typedef enum
{
    GPUPE_CP_READ_MEMORY = 0,
    GPUPE_CP_READ_PGLB  = 1,
    GPUPE_CP_WRITE_MEMORY  = 2,
    GPUPE_CP_WRITE_SNOOPED  = 3,
    GPUPE_CP_WRITE_WRITEBACK  = 4,
    GPUPE_MH_CP_RTR  = 5,
    GPUPE_VGT_READ_MEMORY  = 6,
    GPUPE_VGT_READ_PGLB  = 7,
    GPUPE_MH_VGT_RTR  = 8,
    GPUPE_IOC_READ  = 9,
    GPUPE_IOC_WRITE  = 10,
    GPUPE_IOC_READ_BYTE_COUNT  = 11,
    GPUPE_IOC_WRITE_BYTE_COUNT  = 12,
    GPUPE_IOC_URGENT  = 13,
    GPUPE_MH_IOC_RTR  = 14,
    GPUPE_TC0_READ  = 15,
    GPUPE_MH_TC0_RTR  = 16,
    GPUPE_TC1_READ  = 17,
    GPUPE_MH_TC1_RTR  = 18,
    GPUPE_VC0_READ_MEMORY  = 19,
    GPUPE_VC0_READ_PGLB  = 20,
    GPUPE_MH_VC0_RTR  = 21,
    GPUPE_VC1_READ_MEMORY  = 22,
    GPUPE_VC1_READ_PGLB  = 23,
    GPUPE_MH_VC1_RTR  = 24,
    GPUPE_BC0_CP_WRITE  = 25,
    GPUPE_BC0_EX_WRITE  = 26,
    GPUPE_MH_BC0_RTR  = 27,
    GPUPE_BC1_CP_WRITE  = 28,
    GPUPE_BC1_EX_WRITE  = 29,
    GPUPE_MH_BC1_RTR  = 30,
    GPUPE_DC_GRAPHICS_REQ  = 31,
    GPUPE_DC_OVERLAY_REQ  = 32,
    GPUPE_DC_URGENT  = 33,
    GPUPE_PGLB_BIU_REQ  = 34,
    GPUPE_BIU_PGL_READ_DATA  = 35,
    GPUPE_PGL_MHS_READ_DATA  = 36,
    GPUPE_MH_MC0_READ_REQS  = 37,
    GPUPE_MH_MC0_WRITE_REQS  = 38,
    GPUPE_MH_MC1_READ_REQS  = 39,
    GPUPE_MH_MC1_WRITE_REQS  = 40,
    GPUPE_MC0_MH_READ_DATA  = 41,
    GPUPE_MC1_MH_READ_DATA  = 42,
    GPUPE_MH_CP_SEND  = 43,
    GPUPE_MH_VGT_SEND  = 44,
    GPUPE_MH_IOC_SEND  = 45,
    GPUPE_MH_TC0_SEND  = 46,
    GPUPE_MH_TC1_SEND  = 47,
    GPUPE_MH_VC0_SEND  = 48,
    GPUPE_MH_VC1_SEND  = 49,
    GPUPE_MH_DC_SEND  = 50,
    GPUPE_DC0_INFLIGHT_FULL  = 51,
    GPUPE_DC1_INFLIGHT_FULL  = 52,
    GPUPE_VC0_INFLIGHT_FULL  = 53,
    GPUPE_VC1_INFLIGHT_FULL  = 54,
    GPUPE_TC0_INFLIGHT_FULL  = 55,
    GPUPE_TC1_INFLIGHT_FULL  = 56,
    GPUPE_CP0_INFLIGHT_FULL  = 57,
    GPUPE_CP1_INFLIGHT_FULL  = 58,
    GPUPE_VGT0_INFLIGHT_FULL  = 59,
    GPUPE_VGT1_INFLIGHT_FULL  = 60,
    GPUPE_SB0_INFLIGHT_FULL  = 61,
    GPUPE_SB1_INFLIGHT_FULL  = 62,
    GPUPE_VCPGL_INFLIGHT_FULL  = 63,
    GPUPE_CPPGL_INFLIGHT_FULL  = 64,
    GPUPE_VGTPGL_INFLIGHT_FULL  = 65,
    GPUPE_MC0_DC_Q_FULL  = 66,
    GPUPE_MC0_VC_Q_FULL  = 67,
    GPUPE_MC0_TC_Q_FULL  = 68,
    GPUPE_MC0_CP_Q_FULL  = 69,
    GPUPE_MC0_SB_Q_FULL  = 70,
    GPUPE_MC0_VGT_Q_FULL  = 71,
    GPUPE_MC0_BCCP_Q_FULL  = 72,
    GPUPE_MC0_BCEX_Q_FULL  = 73,
    GPUPE_MC1_DC_Q_FULL  = 74,
    GPUPE_MC1_VC_Q_FULL  = 75,
    GPUPE_MC1_TC_Q_FULL  = 76,
    GPUPE_MC1_CP_Q_FULL  = 77,
    GPUPE_MC1_SB_Q_FULL  = 78,
    GPUPE_MC1_VGT_Q_FULL  = 79,
    GPUPE_MC1_BCCP_Q_FULL  = 80,
    GPUPE_MC1_BCEX_Q_FULL  = 81,
    GPUPE_CP_PGL_CACHE_HIT  = 82,
    GPUPE_VGT_PGL_CACHE_HIT  = 83,
    GPUPE_VC_PGL_CACHE_HIT  = 84,
    GPUPE_CP_PGL_FULL  = 85,
    GPUPE_VGT_PGL_FULL  = 86,
    GPUPE_VC0_PGL_FULL  = 87,
    GPUPE_VC1_PGL_FULL  = 88,
    GPUPE_MC0_WDB_FULL  = 89,
    GPUPE_MC0_TAGBUF_FULL  = 90,
    GPUPE_MC1_WDB_FULL  = 91,
    GPUPE_MC1_TAGBUF_FULL  = 92,
    GPUPE_PGL_TAGBUF_FULL  = 93,
    GPUPE_CP_WRITENOTIFY  = 94,
    GPUPE_BC_WRITENOTIFY  = 95,
    GPUPE_IOC_SYNC  = 96,
    GPUPE_PGL_BIU_LATENCY  = 97,
    GPUPE_MH_MC0_LATENCY  = 98,
    GPUPE_MH_MC0_DC_LATENCY  = 99,
    GPUPE_MH_MC0_VC_LATENCY  = 100,
    GPUPE_MH_MC0_TC_LATENCY  = 101,
    GPUPE_MH_MC0_CP_LATENCY  = 102,
    GPUPE_MH_MC0_SB_LATENCY  = 103,
    GPUPE_MH_MC0_VGT_LATENCY  = 104,
    GPUPE_MH_MC1_LATENCY  = 105,
    GPUPE_MH_MC1_DC_LATENCY  = 106,
    GPUPE_MH_MC1_VC_LATENCY  = 107,
    GPUPE_MH_MC1_TC_LATENCY  = 108,
    GPUPE_MH_MC1_CP_LATENCY  = 109,
    GPUPE_MH_MC1_SB_LATENCY  = 110,
    GPUPE_MH_MC1_VGT_LATENCY  = 111,
    GPUPE_MH_PGL_LATENCY  = 112,
    GPUPE_MH_PGL_CP_LATENCY  = 113,
    GPUPE_MH_PGL_VC_LATENCY  = 114,
    GPUPE_MH_PGL_VGT_LATENCY  = 115,
    GPUPE_TC0_RDY_AND_NOROOM  = 116,
    GPUPE_TC1_RDY_AND_NOROOM  = 117,
    GPUPE_IOC_RDY_AND_NOROOM  = 118,
} GPUPERFEVENT_MH;

typedef enum
{
    GPUPE_0 = 0, // Always Count
    GPUPE_1 = 1, // RBBM_IF Fifo Full
    GPUPE_2 = 2, // MIOC Fifo Full
    GPUPE_3 = 3, // MIOC has a transaction, and is waiting for the RBBM to be ready
    GPUPE_4 = 4, // SIOC Fifo Full
    GPUPE_5 = 5, // SIOC has a transaction, and is waiting for the IOC to be ready
} GPUPERFEVENT_BIF;

typedef enum
{
    GPUPE_DMIF_PER_DCREQ_EVENT = 0,
    GPUPE_DMIF_PER_DCGRPH_REQ_EVENT = 1,
    GPUPE_DMIF_PER_DCOVL_REQ_EVENT = 2,
    GPUPE_DMIF_PER_DCREQ_SIZE_EVENT = 3,
    GPUPE_DMIF_PER_DCGRPH_REQ_SIZE_EVENT = 4,
    GPUPE_DMIF_PER_DCOVL_REQ_SIZE_EVENT = 5,
    GPUPE_DMIF_PER_DCSURFACE_UPDATE_EVENT = 6,
    GPUPE_DMIF_PER_DC_MH_REQ_EVENT = 7,
    GPUPE_DMIF_PER_DC_MH_D1GRPH_REQ_EVENT = 8,
    GPUPE_DMIF_PER_DC_MH_D1OVL_REQ_EVENT = 9,
    GPUPE_DMIF_PER_DC_MH_REQ_SIZE_EVENT = 10,
    GPUPE_DMIF_PER_DC_MH_D1GRPH_REQ_SIZE_EVENT = 11,
    GPUPE_DMIF_PER_DC_MH_D1OVL_REQ_SIZE_EVENT = 12,
    GPUPE_DMIF_PER_DC_MH_SURFACE_UPDATE_EVENT = 13,
    GPUPE_DMIF_PER_MH_DC_RTR_EVENT = 14,
    GPUPE_DMIF_PER_CMD_PROC_WAIT_RTR_STATE_EVENT = 15,
    GPUPE_DMIF_PER_CMD_PROC_IDLE_STATE_EVENT = 16,
    GPUPE_DMIF_PER_DC_MH_URGENT_EVENT = 17,
    GPUPE_DMIF_PER_MH_DC_SEND_EVENT = 18,
    GPUPE_DMIF_PER_MH_DC_SEND_D1GRPH_EVENT = 19,
    GPUPE_DMIF_PER_MH_DC_SEND_D1OVL_EVENT = 20,
    GPUPE_DMIF_PER_DC_MH_RTR_EVENT = 21,
    GPUPE_DMIF_PER_DMIF_BUSY_EVENT = 22,
    GPUPE_DMIF_PER_DMIF_BUSY_MH_DC_SEND_EVENT = 23,
    GPUPE_DMIF_PER_DMIF_BUSY_DC_MH_RTR_EVENT = 24,
    GPUPE_DMIF_PER_DMIF_DCSEND_EVENT = 25,
    GPUPE_DMIF_PER_DMIF_DCNOT_RTS_EVENT = 26,
    GPUPE_DMIF_PER_DCDMIF_NOT_RTR_EVENT = 27,
    GPUPE_DCP_PER_LUT_HOST_RW_EVENT = 28,
    GPUPE_DCP_PER_LUT_RW_BY_HOST_EVENT = 29,
    GPUPE_DCP_PER_RTR_LOW_BY_LUT_HOST_RW_EVENT = 30,
    GPUPE_DCCG_PER_SCLK_R_RBBMIF_CLOCK_ON_EVENT = 31,
    GPUPE_DCCG_PER_SCLK_R_DISCLOCK_ON_EVENT = 32,
    GPUPE_DCCG_PER_SCLK_G_SCL_CLOCK_ON_EVENT = 33,
    GPUPE_DCCG_PER_SCLK_G_DCCLOCK_ON_EVENT = 34,
    GPUPE_DCCG_PER_PCLK_CRTC_CLOCK_ON_EVENT = 35,
    GPUPE_DCCG_PER_DVOACLK_C_CLOCK_ON_EVENT = 36,
    GPUPE_DCCG_PER_DVOACLK_D_CLOCK_ON_EVENT = 37,
    GPUPE_CRTC1_PER_START_LINE_EVENT = 38,
    GPUPE_CRTC1_PER_HSYNC_A_EVENT = 39,
    GPUPE_CRTC1_PER_VSYNC_A_EVENT = 40,
    GPUPE_CRTC1_PER_H_DATA_ACTIVE_EVENT = 41,
    GPUPE_CRTC1_PER_V_DATA_ACTIVE_EVENT = 42,
    GPUPE_CRTC1_PER_DATA_ACTIVE_EVENT = 43,
    GPUPE_CRTC1_PER_H_BLANK_EVENT = 44,
    GPUPE_CRTC1_PER_V_BLANK_EVENT = 45,
    GPUPE_CRTC1_PER_BLANK_EVENT = 46,
    GPUPE_CRTC1_PER_INTERLACE_SELECT_EVENT = 47,
    GPUPE_CRTC1_PER_STEREO_SELECT_EVENT = 48,
    GPUPE_SCL1_PER_HOST_CONFLICT_EVENT = 49,
    GPUPE_SCL1_PER_ADVANCE_FILTER_POS_EVENT = 50,
    GPUPE_SCL1_PER_TAINC_EVENT = 51,
    GPUPE_SCL1_PER_REQUEST_EOL_EVENT = 52,
    GPUPE_SCL1_PER_V_COEF_PRELOAD_EVENT = 53,
    GPUPE_SCL1_PER_EOL_EVENT = 54,
    GPUPE_SCL1_PER_SOF_EVENT = 55,
    GPUPE_LB_PER_DISP1_RESET_REQ_EVENT = 56,
    GPUPE_LB_PER_DISP1_REQ_SEND_EVENT = 57,
    GPUPE_LB_PER_DISP1_REQ_UNDERFLOW_EVENT = 58,
    GPUPE_LB_PER_DISP1_DATA_UNDERFLOW_EVENT = 59,
    GPUPE_LB_PER_DISP1_URGENT_EVENT = 60,
    GPUPE_LB_PER_DISP1_VBLANK_STAT_EVENT = 61,
    GPUPE_LB_PER_DISP1_VLINE_STAT_EVENT = 62,
    GPUPE_DOUT_PER_SCL_DISP1_MODE_CHANGE_INTERRUPT_EVENT = 63,
    GPUPE_DOUT_PER_LB_D1_VLINE_INTERRUPT_EVENT = 64,
    GPUPE_DOUT_PER_LB_D1_VBLANK_INTERRUPT_EVENT = 65,
    GPUPE_DOUT_PER_DISTIMER_INTERRUPT_EVENT = 66,
    GPUPE_LOGIC1_EVENT = 256,
} DCPERFEVENT;

typedef enum
{
    BIUPE_RECV_REQUESTS = 0,
    BIUPE_RECV_RESPONSES = 1, // (pg data only),
    BIUPE_IOC_READS = 2,
    BIUPE_IOC_WRITES = 3,
    BIUPE_MEM_READS = 4,
    BIUPE_MEM_WRITES = 5,
    BIUPE_FSB_SYNCS = 6,
    BIUPE_EIEIOS = 7,
    BIUPE_EOIS = 8,
    BIUPE_FLUSH_ACKS = 9,
    BIUPE_REQUEST_READ_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 10,
    BIUPE_REQUEST_READ_DATA_BYTES_16_BYTE_AND_MORE_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 11,
    BIUPE_REQUEST_WRITE_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 12,
    BIUPE_REQUEST_WRITE_DATA_BYTES_16_BYTE_AND_MORE_TRANSFER_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 13,
    BIUPE_RESERVED0 = 14,
    BIUPE_XMIT_REQUESTS = 15,
    BIUPE_XMIT_RESPONSES = 16,
    BIUPE_READ128S = 17,
    BIUPE_FLUSH_REQ_READS = 18,
    BIUPE_FLUSH_REQ_WRITES = 19,
    BIUPE_CPUWB = 20,
    BIUPE_INTERRUPTS = 21,
    BIUPE_RESPONSES_MC0 = 22,
    BIUPE_RESPONSES_MC1 = 23,
    BIUPE_RESPONSES_IOC_MST = 24,
    BIUPE_MC0_RESPONSE_READ_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 25,
    BIUPE_MC0_RESPONSE_READ_DATA_BYTES_16_BYTE_AND_MORE_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 26,
    BIUPE_MC1_RESPONSE_READ_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 27,
    BIUPE_MC1_RESPONSE_READ_DATA_BYTES_16_BYTE_AND_MORE_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 28,
    BIUPE_IOC_MST_RESPONSE_READ_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 29,
    BIUPE_IOC_MST_RESPONSE_READ_DATA_BYTES_16_BYTE_AND_MORE_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 30,
    BIUPE_RESERVED1A = 31,
    BIUPE_RESERVED2A = 32,
    BIUPE_RESERVED3A = 33,
    BIUPE_RESERVED4A = 34,
    BIUPE_RESERVED5A = 35,
    BIUPE_RESERVED6A = 36,
    BIUPE_RESERVED7A = 37,
    BIUPE_RESERVED8A = 38,
    BIUPE_RESERVED9A = 39,
    BIUPE_REQUESTS = 40,
    BIUPE_IOC_MAST_READS = 41,
    BIUPE_IOC_MAST_WRITES = 42,
    BIUPE_IOC_MAST_EOIS = 43,
    BIUPE_IOC_MAST_REQUEST_READ_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 44,
    BIUPE_IOC_MAST_REQUEST_READ_DATA_BYTES_16_BYTE_AND_MORE_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 45,
    BIUPE_IOC_MAST_REQUEST_WRITE_DATA_BYTES_8_BYTE_AND_LESS_TRANSFERS_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 46,
    BIUPE_IOC_MAST_REQUEST_WRITE_DATA_BYTES_16_BYTE_AND_MORE_TRANSFER_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER = 47,
    BIUPE_RESERVED10 = 48,
    BIUPE_RESERVED11 = 49,
    BIUPE_IOC_SLV_REQUESTS = 50,
    BIUPE_IOC_SLV_SNOOP_READS = 51,
    BIUPE_IOC_SLV_SNOOP_WRITES = 52,
    BIUPE_IOC_SLV_INTERRUPTS = 53,
    BIUPE_IOC_SLV_SYNCS = 54,
    BIUPE_IOC_SLV_SNOOP_READS_NS = 55,
    BIUPE_IOC_SLV_SNOOP_WRITES_NS = 56,
    BIUPE_IOC_SLV_CPUWBS = 57,
    BIUPE_IOC_SLV_REQUEST_READ_DATA_BYTES_DIVIDED_BY_4_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 58,
    BIUPE_IOC_SLV_REQUEST_WRITE_DATA_BYTES_DIVIDED_BY_4_DEPENDENT_ON_SECONDARY_CONTROL_REGISTER_ = 59,
    BIUPE_IOC_SLV_RESPONSES_BEATS_ = 60,
    BIUPE_IOC_SLV_RESP_READ_DATA_BYTES_DIVIDED_BY_4_ = 61,
    BIUPE_IOC_SLV_SYNCCLEANS = 62,
    BIUPE_RESERVED12 = 63,
    BIUPE_RESERVED13 = 64,
    BIUPE_RESERVED14 = 65,
    BIUPE_RESERVED15 = 66,
    BIUPE_RESERVED16 = 67,
    BIUPE_RESERVED17 = 68,
    BIUPE_RESERVED18 = 69,
    BIUPE_MC_0_FAST_PATH_CPU_READS = 70,
    BIUPE_MC_0_ORDERED_CPU_PATH_READS = 71,
    BIUPE_MC_0_ORDERED_CPU_PATH_WRITES = 72,
    BIUPE_MC_0_ORDERED_IOC_PATH_READS = 73,
    BIUPE_MC_0_ORDERED_IOC_PATH_WRITES = 74,
    BIUPE_MC_0_SYNC_A = 75,
    BIUPE_MC_0_SYNC_B = 76,
    BIUPE_RESERVED19 = 77,
    BIUPE_RESERVED20 = 78,
    BIUPE_RESERVED21 = 79,
    BIUPE_MC_1_FAST_PATH_CPU_READS = 80,
    BIUPE_MC_1_ORDERED_CPU_PATH_READS = 81,
    BIUPE_MC_1_ORDERED_CPU_PATH_WRITES = 82,
    BIUPE_MC_1_ORDERED_IOC_PATH_READS = 83,
    BIUPE_MC_1_ORDERED_IOC_PATH_WRITES = 84,
    BIUPE_MC_1_SYNC_A = 85,
    BIUPE_MC_1_SYNC_B = 86,
    BIUPE_MCQF_NEARFULL  = 100,
    BIUPE_MCQDF_NEARFULL  = 101,
    BIUPE_IMQF_NEARFULL  = 102,
    BIUPE_PSF_NEARFULL  = 103,
    BIUPE_M0SF_NEARFULL  = 104,
    BIUPE_M1SF_NEARFULL  = 105,
    BIUPE_IMSF_NEARFULL  = 106,
    BIUPE_IMSDF_NEARFULL  = 107,
    BIUPE_ISQF_NEARFULL  = 108,
    BIUPE_ISSF_NEARFULL  = 109,
    BIUPE_ISYF_NEARFULL  = 110,
    BIUPE_CPYF_NEARFULL  = 111,
    BIUPE_PQF_NEARFULL  = 112,
    BIUPE_TOTAL_MEMORY_LATENCY_FAST_READS_MUST_BE_DIVIDED_BY_MEM_READ_REQUESTS_FOR_AVERAGE_LATENCY  = 140,
    BIUPE_TOTAL_MEMORY_LATENCY_ORDERED_CPU_READS_MUST_BE_DIVIDED_BY_MEM_READ_REQUESTS_FOR_AVERAGE_LATENCY  = 141,
    BIUPE_TOTAL_MEMORY_LATENCY_ORDERED_IOC_READS_MUST_BE_DIVIDED_BY_MEM_READ_REQUESTS_FOR_AVERAGE_LATENCY  = 142,
    BIUPE_TOTAL_FLUSH_LATENCY_FOR_SNOOP_READS_MUST_BE_DIVIED_BY_FLUSH_REQUESTS_FOR_AVERAGE_LATENCY  = 143,
    BIUPE_TOTAL_FLUSH_LATENCY_FOR_SNOOP_WRITES_MUST_BE_DIVIED_BY_FLUSH_REQUESTS_FOR_AVERAGE_LATENCY  = 144,
    BIUPE_SNOOP_COMPLETION_BUFFER_FULL  = 145,
    BIUPE_RESERVED62  = 146,
    BIUPE_RESERVED63  = 147,
    BIUPE_RESERVED64  = 148,
    BIUPE_RESERVED65  = 149,
    BIUPE_MC0_SLOW_CREDIT_COUNT_IS_ZERO  = 150,
    BIUPE_MC1_SLOW_CREDIT_COUNT_IS_ZERO  = 151,
    BIUPE_MC0_FAST_CREDIT_COUNT_IS_ZERO  = 152,
    BIUPE_MC1_FAST_CREDIT_COUNT_IS_ZERO  = 153,
    BIUPE_RESERVED66  = 154,
    BIUPE_RESERVED67  = 155,
    BIUPE_FSB_SLV_REQ_STALL_FOR_COMMAND_CREDITS  = 156,
    BIUPE_FSB_SLV_REQ_STALL_FOR_DATA_CREDITS  = 157,
    BIUPE_FSB_PG_REQ_STALL_FOR_CMD_CREDITS  = 158,
    BIUPE_SCLK_COUNTS_SCLKS_IN_THE_MCLK_DOMAIN_  = 200,
    BIUPE_IOC_MASTER_REQ_STALL_BIU_HAS_DATA_BUT_IOC_NOT_READY_SCLK_DOMAIN_  = 201,
    BIUPE_IOC_SLAVE_RSP_STALL_SCLK_DOMAIN_  = 202,
    BIUPE_INCREMENT_BY_0_DISABLE_COUNT  = 252,
    BIUPE_INCREMENT_BY_1_USED_TO_COUNT_CLOCKS  = 253,
    BIUPE_INCREMENT_BY_63_USED_FOR_SIMULATION_TO_RAPIDLY_INCREMENT_COUNTER  = 254,
    BIUPE_OTHERS = 255, // Reserved
} BIUPERFEVENT;

typedef enum
{
    IOCPE_PERF_CYCLES_WAITING_FOR_REGISTER_BUS = 0,
    IOCPE_PERF_CYCLES_WAITING_FOR_BIU_SLV_REQ = 1,
    IOCPE_PERF_CYCLES_WAITING_FOR_BIU_MST_CPL = 2,
    IOCPE_PERF_CYCLES_WAITING_FOR_BSB_MST_REQ = 3,
    IOCPE_PERF_CYCLES_WAITING_FOR_MH_SLV_REQ = 4,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_REQUESTS = 5,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_READS = 6,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_WRITES = 7, // (includes interrupts)
    IOCPE_PERF_BSB_SLV_REQ_SNOOPED_READS = 8,
    IOCPE_PERF_BSB_SLV_REQ_SNOOPED_WRITES = 9, //includes interrupts, if snooped attr is true)
    IOCPE_PERF_BSB_SLV_REQ_NON_SNOOPED_READS = 10,
    IOCPE_PERF_BSB_SLV_REQ_NON_SNOOPED_WRITES = 11, //( includes interrupts, if non-snooped attr is true)
    IOCPE_PERF_BSB_SLV_REQ_INTERRUPTS = 12,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_REQUESTS_DWORDS = 13,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_READS_DWORDS = 14,
    IOCPE_PERF_BSB_SLV_REQ_TOTAL_WRITES_DWORDS = 15,
    IOCPE_PERF_BSB_SLV_REQ_SNOOPED_READS_DWORDS = 16,
    IOCPE_PERF_BSB_SLV_REQ_SNOOPED_WRITES_DWORDS = 17,
    IOCPE_PERF_BSB_SLV_REQ_NON_SNOOPED_READS_DWORDS = 18,
    IOCPE_PERF_BSB_SLV_REQ_NON_SNOOPED_WRITES_DWORDS = 19,
    IOCPE_PERF_BSB_SLV_READ_LATENCY_CYCLES = 20, // (snooped or non-snooped dependent upon secondary control register) (used to calc average latency with # read requests)
    IOCPE_PERF_BSB_SLV_CPL_TOTAL_READ_COMPLETIONS = 21,
    IOCPE_PERF_BSB_SLV_CPL_SNOOPED_READ_COMPLETIONS = 22,
    IOCPE_PERF_BSB_SLV_CPL_NON_SNOOPED_READ_COMPLETIONS = 23,
    IOCPE_PERF_BSB_SLV_CPL_TOTAL_READ_COMPLETIONS_DWORDS = 24,
    IOCPE_PERF_BSB_SLV_CPL_SNOOPED_READ_COMPLETIONS_DWORDS = 25,
    IOCPE_PERF_BSB_SLV_CPL_NON_SNOOPED_READ_COMPLETIONS_DWORDS = 26,
    IOCPE_PERF_BSB_SLV_CPL_URS = 27, // (counts ur's and ca's) (ca's don't happen in production mode),
    IOCPE_PERF_BIF_SLV_REQ_TOTAL_REQUESTS = 28,
    IOCPE_PERF_BIF_SLV_REQ_TOTAL_CP_WRITEBACKS = 29,
    IOCPE_PERF_BIF_SLV_REQ_TOTAL_SNOOPED_WRITES = 30,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_REQUESTS = 31,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_READS = 32,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_WRITES = 33,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_REQUESTS_DWORDS = 34,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_READS_DWORDS = 35,
    IOCPE_PERF_BSB_MST_REQ_TOTAL_WRITES_DWORDS = 36,
    IOCPE_PERF_BSB_MST_CPL_TOTAL_COMPLETIONS = 37,
    IOCPE_PERF_BSB_MST_CPL_TOTAL_COMPLETIONS_DWORDS = 38,
    IOCPE_PERF_REG_BUS_REQ_TOTAL_REQUESTS = 39,
    IOCPE_PERF_REG_BUS_REQ_TOTAL_READS = 40,
    IOCPE_PERF_REG_BUS_REQ_TOTAL_WRITES = 41,
    IOCPE_PERF_REG_BUS_CPL_READ_COMPLETIONS = 42,
    IOCPE_PERF_TOTAL_SYNCS_ISSUED = 43,
    IOCPE_PERF_BIU_SYNCS_ISSUED = 44,
    IOCPE_PERF_MH_SYNCS_ISSUED = 45,
    IOCPE_PERF_TOTAL_SYNCS_RECEIVED = 46,
    IOCPE_PERF_BIU_SYNCS_RECEIVED = 47,
    IOCPE_PERF_MH_SYNCS_RECEIVED = 48,
    IOCPE_PERF_FIFO_BSB_MST_REQ_FULL = 49,
    IOCPE_PERF_FIFO_BSB_MST_REQ_EMPTY = 50,
    IOCPE_PERF_FIFO_SYNC_FULL = 51,
    IOCPE_PERF_FIFO_SYNC_EMPTY = 52,
    IOCPE_PERF_FIFO_INTERRUPT_FULL = 53,
    IOCPE_PERF_FIFO_INTERRUPT_EMPTY = 54,
    IOCPE_PERF_FIFO_REG_FULL = 55,
    IOCPE_PERF_FIFO_REG_EMPTY = 56,
    IOCPE_PERF_FIFO_BIU_SLV_CPL_NEARFULL = 57,
    IOCPE_PERF_FIFO_BIU_SLV_CPL_EMPTY = 58,
    IOCPE_PERF_FIFO_MH_SLV_CPL_FULL = 59,
    IOCPE_PERF_FIFO_MH_SLV_CPL_EMPTY = 60,
    IOCPE_PERF_FIFO_BSB_SLV_REQ_NEARFULL = 61,
    IOCPE_PERF_FIFO_BSB_SLV_REQ_EMPTY = 62,
    IOCPE_PERF_FIFO_BSB_MST_CPL_NEARFULL = 63,
    IOCPE_PERF_FIFO_BSB_MST_CPL_EMPTY = 64,
    IOCPE_PERF_FIFO_BIU_MST_REQ_NEARFULL = 65,
    IOCPE_PERF_FIFO_BIU_MST_REQ_EMPTY = 66,
    IOCPE_PERF_FIFO_BIF_SLV_REQ_NEARFULL = 67,
    IOCPE_PERF_FIFO_BIF_SLV_REQ_EMPTY = 68,
    IOCPE_PERF_FIFO_READ_LATENCY_FULL = 69,
    IOCPE_PERF_FIFO_READ_LATENCY_EMPTY = 70,
    IOCPE_PERF_NUMBER_OF_SYSTEM_CLOCKS = 71,
    IOCPE_PERF_ZERO_COUNT = 72, //(disable count)
    IOCPE_PERF_SIXTY_THREE_COUNT = 73, // (sim rapidly inc counter)
    IOCPE_PERF_BIU_SPARE = 74, // (so dff ins don't disappear)
    IOCPE_PERF_BSB_SLV_REQ_MESSAGES = 75,
    IOCPE_PERF_BSB_MST_REQ_CONFIG_READS = 76, // (all 1 dword)
    IOCPE_PERF_BSB_MST_REQ_CONFIG_WRITES = 77, // (all 1 dword)
    IOCPE_PERF_BSB_MST_REQ_POSTED_WRITES = 78,
    IOCPE_PERF_BSB_MST_REQ_POSTED_WRITES_DWORDS = 79,
    IOCPE_PERF_BSB_MST_CPL_NON_POSTED_WRITES = 80, // (all 1 dword)
    IOCPE_PERF_CYCLES_WAITING_FOR_INT_SYNC = 81, // (total time waiting for any type of interrupt sync)
    IOCPE_PERF_CYCLES_WAITING_FOR_INT_SYNC_SNOOP_PATH = 82, // (time waiting for interrupts that only caused syncs to biu)
    IOCPE_PERF_CYCLES_WAITING_FOR_INT_SYNC_NON_SNOOP_PATH = 83, // (time waiting for interrupts that only caused syncs to mh)
    IOCPE_PERF_CYCLES_WAITING_FOR_INT_SYNC_BOTH_PATHS = 84, // (time waiting for interrupts that caused syncs to mh and biu)
    IOCPE_PERF_CYCLES_WAITING_FOR_MST_RD_CPL_SYNC = 85, // (total time waiting for any type of mst cpl sync)
    IOCPE_PERF_CYCLES_WAITING_FOR_MST_RD_CPL_SYNC_SNOOP_PATH = 86, // (time waiting for mst cpls that only caused syncs to biu)
    IOCPE_PERF_CYCLES_WAITING_FOR_MST_RD_CPL_SYNC_NON_SNOOP_PATH = 87, // (time waiting for mst cpls that only caused syncs to mh)
    IOCPE_PERF_CYCLES_WAITING_FOR_MST_RD_CPL_SYNC_BOTH_PATHS = 88, // (time waiting for mst cpls that caused syncs to mh and biu)
    IOCPE_PERF_CYCLES_WAITING_FOR_NSR_PASS_SW = 89,
    IOCPE_PERF_CYCLES_WAITING_FOR_SR_PASS_NSW = 90,
    IOCPE_PERF_CYCLES_WAITING_FOR_NSW_PASS_SW = 91,
    IOCPE_PERF_CYCLES_WAITING_FOR_SW_PASS_NSW = 92,
    IOCPE_PERF_FIFO_MST_CPL_SYNC_TYPE_FIFO_FULL = 93,
    IOCPE_PERF_FIFO_MST_CPL_SYNC_TYPE_FIFO_EMPTY = 94,
    IOCPE_PERF_FIFO_BSB_SNOOP_SLV_REQ_FULL = 95,
    IOCPE_PERF_FIFO_BSB_SNOOP_SLV_REQ_EMPTY = 96,
    IOCPE_PERF_FIFO_BSB_SNOOP_SLV_REQ_DATA_FULL = 97,
    IOCPE_PERF_FIFO_BSB_SNOOP_SLV_REQ_DATA_EMPTY = 98,
    IOCPE_PERF_FIFO_READ_LATENCY_BAD_REQ_FULL = 99,
    IOCPE_PERF_FIFO_READ_LATENCY_BAD_REQ_EMPTY = 100,
    IOCPE_PERF_FIFO_READ_LATENCY_NON_SNOOP_CAM_ADDRESS_FULL = 101,
    IOCPE_PERF_FIFO_READ_LATENCY_NON_SNOOP_CAM_ADDRESS_EMPTY = 102,
} IOCPERFEVENT;

//------------------------------------------------------------------------------
// GPU performance counter register offsets

typedef enum
{
    GPUPERFREG_PA_SU_PERFCOUNTER0_SELECT = 0x0C88,
    GPUPERFREG_PA_SU_PERFCOUNTER1_SELECT = 0x0C89,
    GPUPERFREG_PA_SU_PERFCOUNTER2_SELECT = 0x0C8A,
    GPUPERFREG_PA_SU_PERFCOUNTER3_SELECT = 0x0C8B,
    GPUPERFREG_PA_SC_PERFCOUNTER0_SELECT = 0x0C98,
    GPUPERFREG_PA_SC_PERFCOUNTER1_SELECT = 0x0C99,
    GPUPERFREG_PA_SC_PERFCOUNTER2_SELECT = 0x0C9A,
    GPUPERFREG_PA_SC_PERFCOUNTER3_SELECT = 0x0C9B,
    GPUPERFREG_VGT_PERFCOUNTER0_SELECT = 0x0C48,
    GPUPERFREG_VGT_PERFCOUNTER1_SELECT = 0x0C49,
    GPUPERFREG_VGT_PERFCOUNTER2_SELECT = 0x0C4A,
    GPUPERFREG_VGT_PERFCOUNTER3_SELECT = 0x0C4B,
    GPUPERFREG_TCR_PERFCOUNTER0_SELECT = 0x0E05,
    GPUPERFREG_TCR_PERFCOUNTER1_SELECT = 0x0E08,
    GPUPERFREG_TP0_PERFCOUNTER0_SELECT = 0x0E1F,
    GPUPERFREG_TP0_PERFCOUNTER1_SELECT = 0x0E22,
    GPUPERFREG_TP1_PERFCOUNTER0_SELECT = 0x0E28,
    GPUPERFREG_TP1_PERFCOUNTER1_SELECT = 0x0E2B,
    GPUPERFREG_TP2_PERFCOUNTER0_SELECT = 0x0E31,
    GPUPERFREG_TP2_PERFCOUNTER1_SELECT = 0x0E34,
    GPUPERFREG_TP3_PERFCOUNTER0_SELECT = 0x0E3A,
    GPUPERFREG_TP3_PERFCOUNTER1_SELECT = 0x0E3D,
    GPUPERFREG_TCM_PERFCOUNTER0_SELECT = 0x0E54,
    GPUPERFREG_TCM_PERFCOUNTER1_SELECT = 0x0E57,
    GPUPERFREG_TCF_PERFCOUNTER0_SELECT = 0x0E5A,
    GPUPERFREG_TCF_PERFCOUNTER1_SELECT = 0x0E5D,
    GPUPERFREG_TCF_PERFCOUNTER2_SELECT = 0x0E60,
    GPUPERFREG_TCF_PERFCOUNTER3_SELECT = 0x0E63,
    GPUPERFREG_TCF_PERFCOUNTER4_SELECT = 0x0E66,
    GPUPERFREG_TCF_PERFCOUNTER5_SELECT = 0x0E69,
    GPUPERFREG_TCF_PERFCOUNTER6_SELECT = 0x0E6C,
    GPUPERFREG_TCF_PERFCOUNTER7_SELECT = 0x0E6F,
    GPUPERFREG_TCF_PERFCOUNTER8_SELECT = 0x0E72,
    GPUPERFREG_TCF_PERFCOUNTER9_SELECT = 0x0E75,
    GPUPERFREG_TCF_PERFCOUNTER10_SELECT = 0x0E78,
    GPUPERFREG_TCF_PERFCOUNTER11_SELECT = 0x0E7B,
    GPUPERFREG_VC_PERFCOUNTER0_SELECT = 0x0E48,
    GPUPERFREG_VC_PERFCOUNTER1_SELECT = 0x0E4B,
    GPUPERFREG_VC_PERFCOUNTER2_SELECT = 0x0E4E,
    GPUPERFREG_VC_PERFCOUNTER3_SELECT = 0x0E51,
    GPUPERFREG_SQ_PERFCOUNTER0_SELECT = 0x0DC8,
    GPUPERFREG_SQ_PERFCOUNTER1_SELECT = 0x0DC9,
    GPUPERFREG_SQ_PERFCOUNTER2_SELECT = 0x0DCA,
    GPUPERFREG_SQ_PERFCOUNTER3_SELECT = 0x0DCB,
    GPUPERFREG_SX_PERFCOUNTER0_SELECT = 0x0DD4,
    GPUPERFREG_MC0_PERFCOUNTER0_SELECT = 0x0815,
    GPUPERFREG_MC1_PERFCOUNTER0_SELECT = 0x0855,
    GPUPERFREG_MH_PERFCOUNTER0_SELECT = 0x0A18,
    GPUPERFREG_MH_PERFCOUNTER1_SELECT = 0x0A1B,
    GPUPERFREG_MH_PERFCOUNTER2_SELECT = 0x0A1E,
    GPUPERFREG_BIF_PERFCOUNTER0_SELECT = 0x0048,
    GPUPERFREG_HZ_PERFCOUNTER0_SELECT = 0x1004,
    GPUPERFREG_HZ_PERFCOUNTER1_SELECT = 0x1007,
    GPUPERFREG_BC_PERFCOUNTER0_SELECT = 0x0F04,
    GPUPERFREG_BC_PERFCOUNTER1_SELECT = 0x0F05,
    GPUPERFREG_BC_PERFCOUNTER2_SELECT = 0x0F06,
    GPUPERFREG_BC_PERFCOUNTER3_SELECT = 0x0F07,
    GPUPERFREG_RBBM_PERFCOUNTER0_SELECT = 0x0395,
    GPUPERFREG_RBBM_PERFCOUNTER1_SELECT = 0x0396,
    GPUPERFREG_CP_PERFCOUNTER0_SELECT = 0x01E6,
    GPUPERFREG_PA_SU_PERFCOUNTER0_LOW = 0x0C8C,
    GPUPERFREG_PA_SU_PERFCOUNTER0_HI = 0x0C8D,
    GPUPERFREG_PA_SU_PERFCOUNTER1_LOW = 0x0C8E,
    GPUPERFREG_PA_SU_PERFCOUNTER1_HI = 0x0C8F,
    GPUPERFREG_PA_SU_PERFCOUNTER2_LOW = 0x0C90,
    GPUPERFREG_PA_SU_PERFCOUNTER2_HI = 0x0C91,
    GPUPERFREG_PA_SU_PERFCOUNTER3_LOW = 0x0C92,
    GPUPERFREG_PA_SU_PERFCOUNTER3_HI = 0x0C93,
    GPUPERFREG_PA_SC_PERFCOUNTER0_LOW = 0x0C9C,
    GPUPERFREG_PA_SC_PERFCOUNTER0_HI = 0x0C9D,
    GPUPERFREG_PA_SC_PERFCOUNTER1_LOW = 0x0C9E,
    GPUPERFREG_PA_SC_PERFCOUNTER1_HI = 0x0C9F,
    GPUPERFREG_PA_SC_PERFCOUNTER2_LOW = 0x0CA0,
    GPUPERFREG_PA_SC_PERFCOUNTER2_HI = 0x0CA1,
    GPUPERFREG_PA_SC_PERFCOUNTER3_LOW = 0x0CA2,
    GPUPERFREG_PA_SC_PERFCOUNTER3_HI = 0x0CA3,
    GPUPERFREG_VGT_PERFCOUNTER0_LOW = 0x0C4C,
    GPUPERFREG_VGT_PERFCOUNTER0_HI = 0x0C4D,
    GPUPERFREG_VGT_PERFCOUNTER1_LOW = 0x0C4E,
    GPUPERFREG_VGT_PERFCOUNTER1_HI = 0x0C4F,
    GPUPERFREG_VGT_PERFCOUNTER2_LOW = 0x0C50,
    GPUPERFREG_VGT_PERFCOUNTER2_HI = 0x0C51,
    GPUPERFREG_VGT_PERFCOUNTER3_LOW = 0x0C52,
    GPUPERFREG_VGT_PERFCOUNTER3_HI = 0x0C53,
    GPUPERFREG_TCR_PERFCOUNTER0_LOW = 0x0E07,
    GPUPERFREG_TCR_PERFCOUNTER0_HI = 0x0E06,
    GPUPERFREG_TCR_PERFCOUNTER1_LOW = 0x0E0A,
    GPUPERFREG_TCR_PERFCOUNTER1_HI = 0x0E09,
    GPUPERFREG_TP0_PERFCOUNTER0_LOW = 0x0E21,
    GPUPERFREG_TP0_PERFCOUNTER0_HI = 0x0E20,
    GPUPERFREG_TP0_PERFCOUNTER1_LOW = 0x0E24,
    GPUPERFREG_TP0_PERFCOUNTER1_HI = 0x0E23,
    GPUPERFREG_TP1_PERFCOUNTER0_LOW = 0x0E2A,
    GPUPERFREG_TP1_PERFCOUNTER0_HI = 0x0E29,
    GPUPERFREG_TP1_PERFCOUNTER1_LOW = 0x0E2D,
    GPUPERFREG_TP1_PERFCOUNTER1_HI = 0x0E2C,
    GPUPERFREG_TP2_PERFCOUNTER0_LOW = 0x0E33,
    GPUPERFREG_TP2_PERFCOUNTER0_HI = 0x0E32,
    GPUPERFREG_TP2_PERFCOUNTER1_LOW = 0x0E36,
    GPUPERFREG_TP2_PERFCOUNTER1_HI = 0x0E35,
    GPUPERFREG_TP3_PERFCOUNTER0_LOW = 0x0E3C,
    GPUPERFREG_TP3_PERFCOUNTER0_HI = 0x0E3B,
    GPUPERFREG_TP3_PERFCOUNTER1_LOW = 0x0E3F,
    GPUPERFREG_TP3_PERFCOUNTER1_HI = 0x0E3E,
    GPUPERFREG_TCM_PERFCOUNTER0_LOW = 0x0E56,
    GPUPERFREG_TCM_PERFCOUNTER0_HI = 0x0E55,
    GPUPERFREG_TCM_PERFCOUNTER1_LOW = 0x0E59,
    GPUPERFREG_TCM_PERFCOUNTER1_HI = 0x0E58,
    GPUPERFREG_TCF_PERFCOUNTER0_LOW = 0x0E5C,
    GPUPERFREG_TCF_PERFCOUNTER0_HI = 0x0E5B,
    GPUPERFREG_TCF_PERFCOUNTER1_LOW = 0x0E5F,
    GPUPERFREG_TCF_PERFCOUNTER1_HI = 0x0E5E,
    GPUPERFREG_TCF_PERFCOUNTER2_LOW = 0x0E62,
    GPUPERFREG_TCF_PERFCOUNTER2_HI = 0x0E61,
    GPUPERFREG_TCF_PERFCOUNTER3_LOW = 0x0E65,
    GPUPERFREG_TCF_PERFCOUNTER3_HI = 0x0E64,
    GPUPERFREG_TCF_PERFCOUNTER4_LOW = 0x0E68,
    GPUPERFREG_TCF_PERFCOUNTER4_HI = 0x0E67,
    GPUPERFREG_TCF_PERFCOUNTER5_LOW = 0x0E6B,
    GPUPERFREG_TCF_PERFCOUNTER5_HI = 0x0E6A,
    GPUPERFREG_TCF_PERFCOUNTER6_LOW = 0x0E6E,
    GPUPERFREG_TCF_PERFCOUNTER6_HI = 0x0E6D,
    GPUPERFREG_TCF_PERFCOUNTER7_LOW = 0x0E71,
    GPUPERFREG_TCF_PERFCOUNTER7_HI = 0x0E70,
    GPUPERFREG_TCF_PERFCOUNTER8_LOW = 0x0E74,
    GPUPERFREG_TCF_PERFCOUNTER8_HI = 0x0E73,
    GPUPERFREG_TCF_PERFCOUNTER9_LOW = 0x0E77,
    GPUPERFREG_TCF_PERFCOUNTER9_HI = 0x0E76,
    GPUPERFREG_TCF_PERFCOUNTER10_LOW = 0x0E7A,
    GPUPERFREG_TCF_PERFCOUNTER10_HI = 0x0E79,
    GPUPERFREG_TCF_PERFCOUNTER11_LOW = 0x0E7D,
    GPUPERFREG_TCF_PERFCOUNTER11_HI = 0x0E7C,
    GPUPERFREG_VC_PERFCOUNTER0_LOW = 0x0E4A,
    GPUPERFREG_VC_PERFCOUNTER0_HI = 0x0E49,
    GPUPERFREG_VC_PERFCOUNTER1_LOW = 0x0E4D,
    GPUPERFREG_VC_PERFCOUNTER1_HI = 0x0E4C,
    GPUPERFREG_VC_PERFCOUNTER2_LOW = 0x0E50,
    GPUPERFREG_VC_PERFCOUNTER2_HI = 0x0E4F,
    GPUPERFREG_VC_PERFCOUNTER3_LOW = 0x0E53,
    GPUPERFREG_VC_PERFCOUNTER3_HI = 0x0E52,
    GPUPERFREG_SQ_PERFCOUNTER0_LOW = 0x0DCC,
    GPUPERFREG_SQ_PERFCOUNTER0_HI = 0x0DCD,
    GPUPERFREG_SQ_PERFCOUNTER1_LOW = 0x0DCE,
    GPUPERFREG_SQ_PERFCOUNTER1_HI = 0x0DCF,
    GPUPERFREG_SQ_PERFCOUNTER2_LOW = 0x0DD0,
    GPUPERFREG_SQ_PERFCOUNTER2_HI = 0x0DD1,
    GPUPERFREG_SQ_PERFCOUNTER3_LOW = 0x0DD2,
    GPUPERFREG_SQ_PERFCOUNTER3_HI = 0x0DD3,
    GPUPERFREG_SX_PERFCOUNTER0_LOW = 0x0DD8,
    GPUPERFREG_SX_PERFCOUNTER0_HI = 0x0DD9,
    GPUPERFREG_MC0_PERFCOUNTER0_LOW = 0x0817,
    GPUPERFREG_MC0_PERFCOUNTER0_HI = 0x0816,
    GPUPERFREG_MC1_PERFCOUNTER0_LOW = 0x0857,
    GPUPERFREG_MC1_PERFCOUNTER0_HI = 0x0856,
    GPUPERFREG_MH_PERFCOUNTER0_LOW = 0x0A1A,
    GPUPERFREG_MH_PERFCOUNTER0_HI = 0x0A19,
    GPUPERFREG_MH_PERFCOUNTER1_LOW = 0x0A1D,
    GPUPERFREG_MH_PERFCOUNTER1_HI = 0x0A1C,
    GPUPERFREG_MH_PERFCOUNTER2_LOW = 0x0A20,
    GPUPERFREG_MH_PERFCOUNTER2_HI = 0x0A1F,
    GPUPERFREG_BIF_PERFCOUNTER0_LOW = 0x004A,
    GPUPERFREG_BIF_PERFCOUNTER0_HI = 0x0049,
    GPUPERFREG_HZ_PERFCOUNTER0_LOW = 0x1006,
    GPUPERFREG_HZ_PERFCOUNTER0_HI = 0x1005,
    GPUPERFREG_HZ_PERFCOUNTER1_LOW = 0x1009,
    GPUPERFREG_HZ_PERFCOUNTER1_HI = 0x1008,
    GPUPERFREG_BC_PERFCOUNTER0_LOW = 0x0F08,
    GPUPERFREG_BC_PERFCOUNTER0_HI = 0x0F09,
    GPUPERFREG_BC_PERFCOUNTER1_LOW = 0x0F0A,
    GPUPERFREG_BC_PERFCOUNTER1_HI = 0x0F0B,
    GPUPERFREG_BC_PERFCOUNTER2_LOW = 0x0F0C,
    GPUPERFREG_BC_PERFCOUNTER2_HI = 0x0F0D,
    GPUPERFREG_BC_PERFCOUNTER3_LOW = 0x0F0E,
    GPUPERFREG_BC_PERFCOUNTER3_HI = 0x0F0F,
    GPUPERFREG_RBBM_PERFCOUNTER0_LOW = 0x0397,
    GPUPERFREG_RBBM_PERFCOUNTER0_HI = 0x0398,
    GPUPERFREG_RBBM_PERFCOUNTER1_LOW = 0x0399,
    GPUPERFREG_RBBM_PERFCOUNTER1_HI = 0x039A,
    GPUPERFREG_CP_PERFCOUNTER0_LOW = 0x01E7,
    GPUPERFREG_CP_PERFCOUNTER0_HI = 0x01E8,

    GPUPERFREG_CP_PERFMON_CNTL = 0x01F5,
    GPUPERFREG_VGT_EVENT_INITIATOR = 0x21F9,
} GPUPERFREGISTER;

typedef enum
{
    BIUPERFREG_BIU_PERFCOUNTER0_SELECT = 0x408041,
    BIUPERFREG_BIU_PERFCOUNTER1_SELECT = 0x408044,
    BIUPERFREG_BIU_PERFCOUNTER2_SELECT = 0x408047,
    BIUPERFREG_BIU_PERFCOUNTER3_SELECT = 0x40804A,
    BIUPERFREG_BIU_PERFCOUNTER0_LOW = 0x408043,
    BIUPERFREG_BIU_PERFCOUNTER0_HI = 0x408042,
    BIUPERFREG_BIU_PERFCOUNTER1_LOW = 0x408046,
    BIUPERFREG_BIU_PERFCOUNTER1_HI = 0x408045,
    BIUPERFREG_BIU_PERFCOUNTER2_LOW = 0x408049,
    BIUPERFREG_BIU_PERFCOUNTER2_HI = 0x408048,
    BIUPERFREG_BIU_PERFCOUNTER3_LOW = 0x40804C,
    BIUPERFREG_BIU_PERFCOUNTER3_HI = 0x40804B,

    BIUPERFREG_BIU_PERFMON_CNTL = 0x408040,
} BIUPERFREGISTER;

typedef enum
{
    DCPERFREG_DC_PERFCOUNTER0_SELECT = 0x1FC8,
    DCPERFREG_DC_PERFCOUNTER1_SELECT = 0x1FCB,

    DCPERFREG_DC_PERFCOUNTER0_LOW = 0x1FCA,
    DCPERFREG_DC_PERFCOUNTER0_HI = 0x1FC9,
    DCPERFREG_DC_PERFCOUNTER1_LOW = 0x1FCD,
    DCPERFREG_DC_PERFCOUNTER1_HI = 0x1FCC,
} DCPERFREGISTER;

typedef enum
{
    IOCPERFREG_IOC_PERFCOUNTER0_SELECT = 0x410081,
    IOCPERFREG_IOC_PERFCOUNTER1_SELECT = 0x410084,
    IOCPERFREG_IOC_PERFCOUNTER2_SELECT = 0x410087,
    IOCPERFREG_IOC_PERFCOUNTER3_SELECT = 0x41008A,
    IOCPERFREG_IOC_PERFCOUNTER0_LOW = 0x410083,
    IOCPERFREG_IOC_PERFCOUNTER0_HI = 0x410082,
    IOCPERFREG_IOC_PERFCOUNTER1_LOW = 0x410086,
    IOCPERFREG_IOC_PERFCOUNTER1_HI = 0x410085,
    IOCPERFREG_IOC_PERFCOUNTER2_LOW = 0x410089,
    IOCPERFREG_IOC_PERFCOUNTER2_HI = 0x410088,
    IOCPERFREG_IOC_PERFCOUNTER3_LOW = 0x41008C,
    IOCPERFREG_IOC_PERFCOUNTER3_HI = 0x41008B,

    IOCPERFREG_IOC_PERFMON_CNTL = 0x410080,
} IOCPERFREGISTER;


//------------------------------------------------------------------------------
// GPU performance counter register defines

typedef union
{
    struct {
        DWORD Select                    : 8;
        DWORD N                         : 8;
        DWORD                           : 16;
    };
    DWORD dword;
} GPUPERFCOUNTER_SELECT;

typedef union {
    struct {
        DWORD Low                       : 32;
        DWORD High                      : 16;
        DWORD                           : 16;

    };
    ULARGE_INTEGER qword;
} GPUPERFCOUNTER_VALUE;

typedef union {
    struct {
        DWORD Low                       : 32;
        DWORD High                      : 16;
        DWORD                           : 16;
    };
    ULARGE_INTEGER qword;
} DCPERFCOUNTER_VALUE;

typedef union {
    struct {
        DWORD Low                       : 32;
        DWORD High                      : 16;
        DWORD                           : 16;


    };
    ULARGE_INTEGER qword;
} BIUPERFCOUNTER_VALUE;

typedef union {
    struct {
        DWORD Low                       : 32;
        DWORD High                      : 16;
        DWORD                           : 16;
    };
    ULARGE_INTEGER qword;
} IOCPERFCOUNTER_VALUE;

typedef union {
    struct {
        DWORD State                     : 4;
        DWORD                           : 4;
        DWORD EnableMode                : 2;
        DWORD                           : 22;
    };
    DWORD dword;
} GPUPERFCOUNTER_CNTL;

typedef union {
    struct {
        DWORD State                     : 4;
        DWORD                           : 4;
        DWORD EnableMode                : 2;
        DWORD                           : 22;
    };
    DWORD dword;
} DCPERFCOUNTER_CNTL;

typedef union {
    struct {
        DWORD State                     : 3;
        DWORD                           : 29;
    };
    DWORD dword;
} BIUPERFCOUNTER_CNTL;

typedef union {
    struct {
        DWORD State                     : 4;
        DWORD                           : 28;
    };
    DWORD dword;
} IOCPERFCOUNTER_CNTL;


#if defined(_M_PPCBE)
#pragma bitfield_order(pop)
#endif

#pragma warning(pop)

#ifdef __cplusplus
};
#endif

#endif /* _D3D9GPU_H_ */
