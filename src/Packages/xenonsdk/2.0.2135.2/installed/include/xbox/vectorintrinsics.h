/*

Copyright (C) Microsoft Corporation 2005.  All rights reserved.

Module Name:

    VectorIntrinsics.h

Abstract:
    __vector4 intrinsic type declaration, and intrinsic prototypes.
    

*/

#ifndef __VECTORINTRINSICS_H__
#define __VECTORINTRINSICS_H__

#pragma warning(push)
#pragma warning(disable:4201)
 
#ifndef _NOALIGNMENT_
    #define _DECLSPEC_ALIGN_16_   __declspec(align(16))
#else // defined _NOALIGNMENT_
    #define _DECLSPEC_ALIGN_16_
#endif //_NOALIGNMENT_


typedef struct __declspec(intrin_type) _DECLSPEC_ALIGN_16_  __vector4 {
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float        v[4];
        unsigned int u[4];
    };
} __vector4;

typedef enum __VECTOR_PACK_TYPES
{
    VPACK_D3DCOLOR          = 0x00,
    VPACK_NORMSHORT2        = 0x01,
    VPACK_NORMPACKED32      = 0x02,
    VPACK_FLOAT16_2         = 0x03,
    VPACK_NORMSHORT4        = 0x04,
    VPACK_FLOAT16_4         = 0x05,
    VPACK_NORMPACKED64      = 0x06,
} __VECTOR_PACK_TYPES;


// MASK SELECT    SHIFT           RESULTING MASK
// 
// VPACK_32 shift 0   0x00000000 00000000 00000000 FFFFFFFF
// VPACK_32 shift 1   0x00000000 00000000 FFFFFFFF 00000000
// VPACK_32 shift 2   0x00000000 FFFFFFFF 00000000 00000000
// VPACK_32 shift 3   0xFFFFFFFF 00000000 00000000 00000000

// VPACK_64LO shift 0 0x00000000 00000000 FFFFFFFF FFFFFFFF
// VPACK_64LO shift 1 0x00000000 FFFFFFFF FFFFFFFF 00000000
// VPACK_64LO shift 2 0xFFFFFFFF FFFFFFFF 00000000 00000000
// VPACK_64LO shift 3 0xFFFFFFFF 00000000 00000000 00000000

// VPACK_64HI shift 0 0x00000000 00000000 FFFFFFFF FFFFFFFF
// VPACK_64HI shift 1 0x00000000 FFFFFFFF FFFFFFFF 00000000
// VPACK_64HI shift 2 0xFFFFFFFF FFFFFFFF 00000000 00000000
// VPACK_64HI shift 3 0x00000000 00000000 00000000 FFFFFFFF

typedef enum __VECTOR_PACK_MASK
{
    VPACK_32      = 1,    // inserts one 32bit value based on shift
    VPACK_64LO    = 2,    // inserts one 64bit value based on shift
                          // only low word for shift by 3.
    VPACK_64HI    = 3,    // same as VPACK_64LO except only high word for shift by 3
} __VECTOR_PACK_MASK;

#pragma warning(pop)
#ifdef __cplusplus
extern "C"{
#endif

//////////////////////////////////////////////////////////////////////////////
// Base VMX32 intrinsics

__vector4 __lvsl  (const void * base, int offset);
__vector4 __lvsr  (const void * base, int offset);

#ifdef  __cplusplus
#define __lvx(base, offset)         (*(reinterpret_cast<const __vector4*>(reinterpret_cast<const char*>(base) + (offset))))
// reinterpret_cast ensures that const correctness is maintained - it prevents using __stvx to write
// to a const pointer.
#define __stvx(vSrc, base, offset) *(reinterpret_cast<__vector4*>(reinterpret_cast<char*>(base) + (offset))) = (vSrc);
#else
// C version that doesn't use reinterpret_cast.
#define __lvx(base, offset)         (*((__vector4*)((char*)(base) + (offset))))
#define __stvx(vSrc, base, offset) *((__vector4*)((char*)(base) + (offset))) = (vSrc);
#endif

// load/store vector element.  Note that these can act as read/write barriers
// if used incorrectly.
void __stvebx(__vector4 vSrc, void* base, int offset);
void __stvehx(__vector4 vSrc, void* base, int offset);
void __stvewx(__vector4 vSrc, void* base, int offset);
__vector4 __lvebx(const void* base, int offset);
__vector4 __lvehx(const void* base, int offset);
__vector4 __lvewx(const void* base, int offset);



__vector4 __vaddfp  (__vector4 VRA, __vector4 VRB);
__vector4 __vaddcuw (__vector4 VRA, __vector4 VRB);
__vector4 __vaddsbs (__vector4 VRA, __vector4 VRB);
__vector4 __vaddshs (__vector4 VRA, __vector4 VRB);
__vector4 __vaddsws (__vector4 VRA, __vector4 VRB);
__vector4 __vaddubm (__vector4 VRA, __vector4 VRB);
__vector4 __vaddubs (__vector4 VRA, __vector4 VRB);
__vector4 __vadduhm (__vector4 VRA, __vector4 VRB);
__vector4 __vadduhs (__vector4 VRA, __vector4 VRB); 
__vector4 __vadduwm (__vector4 VRA, __vector4 VRB);
__vector4 __vadduws (__vector4 VRA, __vector4 VRB);
__vector4 __vand    (__vector4 VRA, __vector4 VRB);
__vector4 __vandc   (__vector4 VRA, __vector4 VRB);
__vector4 __vavgsb  (__vector4 VRA, __vector4 VRB);
__vector4 __vavgsh  (__vector4 VRA, __vector4 VRB);
__vector4 __vavgsw  (__vector4 VRA, __vector4 VRB);
__vector4 __vavgub  (__vector4 VRA, __vector4 VRB);
__vector4 __vavguh  (__vector4 VRA, __vector4 VRB);
__vector4 __vavguw  (__vector4 VRA, __vector4 VRB);
__vector4 __vcfsx   (__vector4 VRB, unsigned short scale);
__vector4 __vcfux   (__vector4 VRB, unsigned short scale);
__vector4 __vcmpbfp (__vector4 VRA, __vector4 VRB);
__vector4 __vcmpeqfp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpequb(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpequh(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpequw(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgefp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtfp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtsb(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtsh(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtsw(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtub(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtuh(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtuw(__vector4 VRA, __vector4 VRB);


// Recording forms of vector compares.  Will affect CR6.
// CR is written into 3rd argument

__vector4 __vcmpbfpR (__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpeqfpR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpequbR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpequhR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpequwR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgefpR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtfpR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtsbR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtshR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtswR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtubR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtuhR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);
__vector4 __vcmpgtuwR(__vector4 VRA, __vector4 VRB, /* OUT */ unsigned int *CR);

__vector4 __vctsxs(__vector4 VRB, unsigned int scale);
__vector4 __vctuxs(__vector4 VRB, unsigned int scale);
__vector4 __vexptefp(__vector4 VRB);
__vector4 __vlogefp(__vector4 VRB);
__vector4 __vmaddfp(__vector4 VRA, __vector4 VRC, __vector4 VRB);
__vector4 __vmaxfp(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxsb(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxsh(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxsw(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxub(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxuh(__vector4 VRA, __vector4 VRB);
__vector4 __vmaxuw(__vector4 VRA, __vector4 VRB);
__vector4 __vminfp(__vector4 VRA, __vector4 VRB);
__vector4 __vminsb(__vector4 VRA, __vector4 VRB);
__vector4 __vminsh(__vector4 VRA, __vector4 VRB);
__vector4 __vminsw(__vector4 VRA, __vector4 VRB);
__vector4 __vminub(__vector4 VRA, __vector4 VRB);
__vector4 __vminuh(__vector4 VRA, __vector4 VRB);
__vector4 __vminuw(__vector4 VRA, __vector4 VRB);
__vector4 __vmrghb(__vector4 VRA, __vector4 VRB);
__vector4 __vmrghh(__vector4 VRA, __vector4 VRB);
__vector4 __vmrghw(__vector4 VRA, __vector4 VRB);
__vector4 __vmrglb(__vector4 VRA, __vector4 VRB);
__vector4 __vmrglh(__vector4 VRA, __vector4 VRB);
__vector4 __vmrglw(__vector4 VRA, __vector4 VRB);
__vector4 __vnmsubfp(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vnor(__vector4 VRA, __vector4 VRB);
__vector4 __vor(__vector4 VRA, __vector4 VRB);
__vector4 __vperm(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vpkpx(__vector4 VRA, __vector4 VRB);
__vector4 __vpkshss(__vector4 VRA, __vector4 VRB);
__vector4 __vpkshus(__vector4 VRA, __vector4 VRB);
__vector4 __vpkswss(__vector4 VRA, __vector4 VRB);
__vector4 __vpkswus(__vector4 VRA, __vector4 VRB);
__vector4 __vpkuhum(__vector4 VRA, __vector4 VRB);
__vector4 __vpkuhus(__vector4 VRA, __vector4 VRB);
__vector4 __vpkuwum(__vector4 VRA, __vector4 VRB);
__vector4 __vpkuwus(__vector4 VRA, __vector4 VRB);
__vector4 __vrefp(__vector4 VRB);
__vector4 __vrfim(__vector4 VRB);
__vector4 __vrfin(__vector4 VRB);
__vector4 __vrfip(__vector4 VRB);
__vector4 __vrfiz(__vector4 VRB);
__vector4 __vrlb(__vector4 VRA, __vector4 VRB);
__vector4 __vrlh(__vector4 VRA, __vector4 VRB);
__vector4 __vrlw(__vector4 VRA, __vector4 VRB);
__vector4 __vrsqrtefp(__vector4 VRB);
__vector4 __vsel(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vsl(__vector4 VRA, __vector4 VRB);
__vector4 __vslb(__vector4 VRA, __vector4 VRB);
__vector4 __vsldoi(__vector4 VRA, __vector4 VRB, unsigned int SHB);
__vector4 __vslh(__vector4 VRA, __vector4);
__vector4 __vslo(__vector4 VRA, __vector4);
__vector4 __vslw(__vector4 VRA, __vector4);
__vector4 __vspltb(__vector4 VRB, unsigned int UIM);
__vector4 __vsplth(__vector4 VRB, unsigned int UIM);
__vector4 __vspltisb(int SIM);
__vector4 __vspltish(int SIM);
__vector4 __vspltisw(int SIM);
__vector4 __vspltw(__vector4 VRB, unsigned int UIM);
__vector4 __vsr(__vector4 VRA, __vector4 VRB);
__vector4 __vsrab(__vector4 VRA, __vector4 VRB);
__vector4 __vsrah(__vector4 VRA, __vector4 VRB);
__vector4 __vsraw(__vector4 VRA, __vector4 VRB);
__vector4 __vsrb(__vector4 VRA, __vector4 VRB);
__vector4 __vsrh(__vector4 VRA, __vector4 VRB);
__vector4 __vsro(__vector4 VRA, __vector4 VRB);
__vector4 __vsrw(__vector4 VRA, __vector4 VRB);
__vector4 __vsubcuw(__vector4 VRA, __vector4 VRB);
__vector4 __vsubfp(__vector4 VRA, __vector4 VRB);
__vector4 __vsubsbs(__vector4 VRA, __vector4 VRB);
__vector4 __vsubshs(__vector4 VRA, __vector4 VRB);
__vector4 __vsubsws(__vector4 VRA, __vector4 VRB);
__vector4 __vsububm(__vector4 VRA, __vector4 VRB);
__vector4 __vsububs(__vector4 VRA, __vector4 VRB);
__vector4 __vsubuhm(__vector4 VRA, __vector4 VRB);
__vector4 __vsubuhs(__vector4 VRA, __vector4 VRB);
__vector4 __vsubuwm(__vector4 VRA, __vector4 VRB);
__vector4 __vsubuws(__vector4 VRA, __vector4 VRB);
__vector4 __vupkhpx(__vector4 VRB);
__vector4 __vupkhsb(__vector4 VRB);
__vector4 __vupkhsh(__vector4 VRB);
__vector4 __vupklpx(__vector4 VRB);
__vector4 __vupklsb(__vector4 VRB);
__vector4 __vupklsh(__vector4 VRB);
__vector4 __vxor(__vector4 VRA, __vector4 VRB);

// Support the regular (new) mnemonics as well as the extended (old) mnemonics for these instructions
#define __vcsxwfp __vcfsx
#define __vcuxwfp __vcfux
#define __vcfpsxws __vctsxs
#define __vcfpuxws __vctuxs

// Pseudo instructions

// 
// __vzero creates a zero vector without creating any register/variable dependencies
// Using __vspltisw instead of __vspltisb allows efficient access to all 128 registers.
//
#define __vzero()  __vspltisw(0)



//////////////////////////////////////////////////////////////////////////////
// Emulated intrinsics, likely to be slow.

__vector4 __vmsum3fp(__vector4, __vector4);
__vector4 __vmsum4fp(__vector4, __vector4);
__vector4 __vmulfp(__vector4, __vector4);
__vector4 __vpermwi(__vector4, unsigned int);

// Although vrlimi128 and vpkd3d128 both hammer the incoming VRT register, the value is copied
// if the original value is needed.  So codegen is most efficient when you assume that
// VRT = __vrlimi(VRT, ...)

// WMASK and SHW MUST be integer constants for final hardware.
__vector4 __vrlimi(__vector4 VRT, __vector4 VRB, unsigned int WMASK, unsigned int SHW);

// Note that __vpkd3d will not perform the normalization for VPACK_D3DCOLOR, or any VPACK_NORM* 
// format.  Wrapper code must scale incoming numbers by 255.0f/(float)(1<<22) and add 3.0f.

// DT, MS, and SHW MUST be integer constants for final hardware.
__vector4 __vpkd3d(__vector4 VRT, 
                         __vector4 VRB, 
                         __VECTOR_PACK_TYPES DT,    // datatype
                         __VECTOR_PACK_MASK MS,     // mask select
                         unsigned int SHW);         // rotate-left count, must be an integer constant
__vector4 __vupkd3d(__vector4 VRB, __VECTOR_PACK_TYPES DT);

// Note that lvrx expects an address 16 greater than the matching lvlx
__vector4 __lvlx(const void * base, int offset);
__vector4 __lvrx(const void * base, int offset);

void __stvlx(__vector4 vSrc, void* base, int offset);
void __stvrx(__vector4 vSrc, void* base, int offset);

//////////////////////////////////////////////////////////////////////////////
// Helpful macros 

// Create a permute vector for __vpermwi. Specify which element number
// (from 0-3) to copy to x, y, z, and w.
#define VPERMWI_CONST(x,y,z,w)	((((x)&3) << 6) | (((y)&3) << 4) | (((z)&3) << 2) | ((w)&3))

#define __loadunalignedvector(ptr) __vor(__lvlx(ptr, 0), __lvrx(ptr,16))
#define __storeunalignedvector(v, ptr) (__stvlx(v, ptr, 0), __stvrx(v, ptr,16))

// Load 32b from ptr into v.v[dstelem] without incurring load-hit-store
#define __loadunalignedvectorelement(v, ptr, dstelem) {\
    __vector4 tmp = __lvlx(ptr, 0);\
    v = __vrlimi(v, tmp, 0x8>>(dstelem),(4-(dstelem))%4);\
}


#define __vdot3fp __vmsum3fp
#define __vdot4fp __vmsum4fp

//////////////////////////////////////////////////////////////////////////////
// These intrinsics will assign any VMX register to a variable, and vice-versa.
// Argument must be an integer constant, or lowered into one.

// They are intended for fast vector/matrix processing by keeping important
// variables in the same registers across function calls.

// Programming model currently TBD - Intention is to reserve the top N (N~=16)
// vector registers for cross-function matrix/vector storage.
// Since the compiler does NOT reserve these yet ( we don't have enough registers)
// it is very possible to break the compiler with these intrinsics.

// For now, just use the top 4 VMX registers (vr28-vr31)
// Wanton use of these intrinsics can result in undefined behavior.

// VMXGetReg should generally be used at variable initialization time
    
__vector4   __VMXGetReg(const int );
void        __VMXSetReg(const int , __vector4);

#ifdef __cplusplus
}
#endif

#endif
