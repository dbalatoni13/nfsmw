/*
** $Id: lgc.c,v 1.171a 2003/04/03 13:35:34 roberto Exp $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#define lgc_c

#include "lua.h"

#include "lgc.h"

size_t luaC_separateudata(lua_State *L) {
    return 0;
}

void luaC_callGCTM(lua_State *L) {}

void luaC_sweep(lua_State *L, int all) {}

void luaC_link(lua_State *L, GCObject *o, lu_byte tt) {}
