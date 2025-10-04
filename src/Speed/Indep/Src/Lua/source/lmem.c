/*
** $Id: lmem.c,v 1.61 2002/12/04 17:38:31 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/

#include <stdlib.h>

#define lmem_c

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"

static LuaReallocFunc sRealloc = NULL;
static LuaFreeFunc sFree = NULL;

#define MINSIZEARRAY 4

void *luaM_growaux(lua_State *L, void *block, int *size, int size_elems, int limit, const char *errormsg) {
    void *newblock;
    int newsize = (*size) * 2;
    if (newsize < MINSIZEARRAY)
        newsize = MINSIZEARRAY;           /* minimum size */
    else if (*size >= limit / 2) {        /* cannot double it? */
        if (*size < limit - MINSIZEARRAY) /* try something smaller... */
            newsize = limit;              /* still have at least MINSIZEARRAY free places */
        else
            luaG_runerror(L, errormsg);
    }
    newblock =
        luaM_realloc(L, block, cast(lu_mem, *size) * cast(lu_mem, size_elems), cast(lu_mem, newsize) * cast(lu_mem, size_elems), LUAALLOC_INVALID);
    *size = newsize; /* update only when everything else is OK */
    return newblock;
}

void luaM_setallocator(LuaReallocFunc reallocFunc, LuaFreeFunc freeFunc) {
    sRealloc = reallocFunc;
    sFree = freeFunc;
}

/*
** generic allocation routine.
*/
void *luaM_realloc(lua_State *L, void *block, lu_mem oldsize, lu_mem size, LuaAllocType type) {
    lua_assert((oldsize == 0) == (block == NULL));
    if (size == 0) {
        if (block != NULL) {
            sFree(L, block, oldsize, type);
            block = NULL;
        } else
            return NULL; /* avoid `nblocks' computations when oldsize==size==0 */
    } else if (size >= MAX_SIZET)
        luaG_runerror(L, "memory allocation error: block too big");
    else {
        block = sRealloc(L, block, oldsize, size, type);
        if (block == NULL) {
            if (L)
                luaD_throw(L, LUA_ERRMEM);
            else
                return NULL; /* error before creating state! */
        }
    }
    if (L) {
        lua_assert(G(L) != NULL && G(L)->nblocks > 0);
        G(L)->nblocks -= oldsize;
        G(L)->nblocks += size;
    }
    return block;
}
