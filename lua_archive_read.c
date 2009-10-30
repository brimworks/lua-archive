//////////////////////////////////////////////////////////////////////
// Implement the archive{read} object
//////////////////////////////////////////////////////////////////////

#include <archive.h>
#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#include "private_archive_read.h"

static int __ref_count = 0;

//////////////////////////////////////////////////////////////////////
// For debugging GC issues.
static int ar_ref_count(lua_State *L) {
    lua_pushnumber(L, __ref_count);
    lua_settop(L, -1);
    return 1;
}

//////////////////////////////////////////////////////////////////////
static int ar_read(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_read_destroy(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
int lua_archive_read(lua_State *L) {
    luaL_checktype(L, LUA_TTABLE, -1); // {class}

    static luaL_reg fns[] = {
        { "read",  ar_read },
        { "_read_ref_count", ar_ref_count },
        { NULL, NULL }
    };
    luaL_register(L, NULL, fns); // {class}

    luaL_newmetatable(L, AR_READ); // {class}, {meta}

    lua_pushvalue(L, -1); // {class}, {meta}, {meta}
    lua_setfield(L, -2, "__index"); // {class}, {meta}

    static luaL_reg m_fns[] = {
        { "__gc",    ar_read_destroy },
        { NULL, NULL }
    };
    luaL_register(L, NULL, m_fns); // {class}, {meta}

    lua_pop(L, 1); // {class}

    return 0;
}

