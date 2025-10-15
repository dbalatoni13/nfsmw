/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuimemory.h
 *  Content:    Xbox 360 UI memory management defines and functions
 *
 ****************************************************************************/

#ifndef __XUIMEMORY_H__
#define __XUIMEMORY_H__

#ifdef __cplusplus
#define DECLARE_XUI_ALLOC()\
    inline void* __cdecl operator new (size_t size)\
    {\
        return XuiAlloc(size);\
    }\
\
    inline void * __cdecl operator new(size_t size, void *pvObj)\
    {\
        return pvObj;\
    }\
\
    inline void *__cdecl operator new[] (size_t count)\
    {\
        return XuiAlloc(count);\
    }\
\
    inline void __cdecl operator delete (void* pvBuffer)\
    {\
        XuiFree(pvBuffer);\
    }\
\
    inline void __cdecl operator delete[] (void *pvBuffer)\
    {\
        XuiFree(pvBuffer);\
    }
#else
#define DECLARE_XUI_ALLOC()
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XUI_DBG_MEM

#define XuiAlloc(size) XuiAllocTagged(size, __FUNCTION__)
void* APIENTRY XuiAllocTagged(size_t size, LPCSTR szTag);

#define XuiRealloc(pv, size) XuiReallocTagged(pv, size, __FUNCTION__)
void* APIENTRY XuiReallocTagged(void *pv, size_t size, LPCSTR szTag);

#else

void* APIENTRY XuiAlloc(size_t size);
void* APIENTRY XuiRealloc(void *pv, size_t size);

#endif

size_t XuiGetMemorySize(void *pv);
VOID APIENTRY XuiFree(void *pv);
#ifdef __cplusplus
}
#endif

#endif // __XUIMEMORY_H__

