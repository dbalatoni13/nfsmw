/*
** $Id: lmem.h,v 1.26 2002/05/01 20:40:42 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/

#ifndef lmem_h
#define lmem_h

#include <stddef.h>

#include "llimits.h"
#include "lua.h"

typedef enum LuaAllocType {
    LUAALLOC_INVALID = 0,
    LUAALLOC_CHAR = 1,
    LUAALLOC_FIRST = 1,
    LUAALLOC_INT = 2,
    LUAALLOC_STATE = 3,
    LUAALLOC_GLOBALSTATE = 4,
    LUAALLOC_TABLE = 5,
    LUAALLOC_TOBJECT = 6,
    LUAALLOC_LOCVAR = 7,
    LUAALLOC_TSTRINGPTR = 8,
    LUAALLOC_GCOBJECTPTR = 9,
    LUAALLOC_INSTRUCTION = 10,
    LUAALLOC_CALLINFO = 11,
    LUAALLOC_CLOSURE = 12,
    LUAALLOC_UPVAL = 13,
    LUAALLOC_PROTO = 14,
    LUAALLOC_PROTOPTR = 15,
    LUAALLOC_UDATA = 16,
    LUAALLOC_NODE = 17,
    LUAALLOC_LAST = 17,
} LuaAllocType;

typedef void *(*LuaReallocFunc)(lua_State *, void *, unsigned int, unsigned int, LuaAllocType);
typedef void (*LuaFreeFunc)(lua_State *, void *, unsigned int, LuaAllocType);

#define MEMERRMSG "not enough memory"

void luaM_setallocator(LuaReallocFunc reallocFunc, LuaFreeFunc freeFunc);

void *luaM_realloc(lua_State *L, void *oldblock, lu_mem oldsize, lu_mem size, LuaAllocType type);

void *luaM_growaux(lua_State *L, void *block, int *size, int size_elem, int limit, const char *errormsg);

#define luaM_free(L, b, s, at) luaM_realloc(L, (b), (s), 0, at)
#define luaM_freelem(L, b, at) luaM_realloc(L, (b), sizeof(*(b)), 0, at)
#define luaM_freearray(L, b, n, t, at) luaM_realloc(L, (b), cast(lu_mem, n) * cast(lu_mem, sizeof(t)), 0, at)

#define luaM_malloc(L, t, at) luaM_realloc(L, NULL, 0, (t), at)
#define luaM_new(L, t, at) cast(t *, luaM_malloc(L, sizeof(t), at))
#define luaM_newvector(L, n, t, at) cast(t *, luaM_malloc(L, cast(lu_mem, n) * cast(lu_mem, sizeof(t)), at))

#define luaM_growvector(L, v, nelems, size, t, limit, e)                                                                                             \
    if (((nelems) + 1) > (size))                                                                                                                     \
    ((v) = cast(t *, luaM_growaux(L, v, &(size), sizeof(t), limit, e)))

#define luaM_reallocvector(L, v, oldn, n, t, at)                                                                                                     \
    ((v) = cast(t *, luaM_realloc(L, v, cast(lu_mem, oldn) * cast(lu_mem, sizeof(t)), cast(lu_mem, n) * cast(lu_mem, sizeof(t)), at)))

#endif
