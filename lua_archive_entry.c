#include <archive_entry.h>
#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#include "private_archive_entry.h"

static int __ref_count = 0;

//////////////////////////////////////////////////////////////////////
// For debugging GC issues.
static int ar_ref_count(lua_State *L) {
    lua_pushnumber(L, __ref_count);
    lua_settop(L, -1);
    return 1;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry(lua_State *L) {
/*
    archive_entry_new();

    if ( lua_istable(L, 1) ) {

        // Iterate over the table and call the method with that name
        lua_pushnil(L);
        while (lua_next(L, t) != 0) {
            printf("%s - %s\n",
                   lua_typename(L, lua_type(L, -2)),
                   lua_typename(L, lua_type(L, -1)));
            lua_pop(L, 1);
        }

    } else if ( lua_isstring(L, 1) ) {
        // Create entry from file path.
    }
*/
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_destroy(lua_State *L) {
    struct archive_entry** ar = ar_entry_check(L, 1);
    if ( *ar != NULL ) {
        archive_entry_free(*ar);
        *ar = NULL;
    }
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_fflags(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_dev(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_ino(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_mode(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_nlink(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_uid(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_uname(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_gid(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_gname(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_rdev(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_atime(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_mtime(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_ctime(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_birthtime(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_size(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_link(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_sourcepath(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_symlink(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_filetype(lua_State *L) {
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_hardlink(lua_State *L) {
    struct archive_entry** self = ar_entry_check(L, 1);
    if ( NULL == *self ) return 0;

    int is_set = ( lua_gettop(L) == 2 );
    lua_pushstring(L, archive_entry_hardlink(*self));
    if ( is_set ) {
        archive_entry_set_hardlink(*self, lua_tostring(L, 2));
    }
    lua_settop(L, -1);
    return 1;
}

//////////////////////////////////////////////////////////////////////
static int ar_entry_pathname(lua_State *L) {
    struct archive_entry** self = ar_entry_check(L, 1);
    if ( NULL == *self ) return 0;

    int is_set = ( lua_gettop(L) == 2 );
    lua_pushstring(L, archive_entry_pathname(*self));
    if ( is_set ) {
        archive_entry_set_pathname(*self, lua_tostring(L, 2));
    }
    lua_settop(L, -1);
    return 1;
}

//////////////////////////////////////////////////////////////////////
int lua_archive_entry(lua_State *L) {
    luaL_checktype(L, LUA_TTABLE, -1); // {class}
    static luaL_reg fns[] = {
        { "entry",  ar_entry },
        { "_entry_ref_count", ar_ref_count },
        { NULL, NULL }
    };
    luaL_register(L, NULL, fns); // {class}

    luaL_newmetatable(L, AR_ENTRY); // {class}, {meta}

    lua_pushvalue(L, -1); // {class}, {meta}, {meta}
    lua_setfield(L, -2, "__index"); // {class}, {meta}

    // So far there are no methods on the entry objects.
    static luaL_reg m_fns[] = {
        { "fflags", ar_entry_fflags },
        { "dev", ar_entry_dev },
        { "ino", ar_entry_ino },
        { "mode", ar_entry_mode },
        { "nlink", ar_entry_nlink },
        { "uid", ar_entry_uid },
        { "uname", ar_entry_uname },
        { "gid", ar_entry_gid },
        { "gname", ar_entry_gname },
        { "rdev", ar_entry_rdev },
        { "atime", ar_entry_atime },
        { "mtime", ar_entry_mtime },
        { "ctime", ar_entry_ctime },
        { "birthtime", ar_entry_birthtime },
        { "size", ar_entry_size },
        { "link", ar_entry_link },
        { "sourcepath", ar_entry_sourcepath },
        { "symlink", ar_entry_symlink },
        { "filetype", ar_entry_filetype },
        { "hardlink", ar_entry_hardlink },
        { "pathname", ar_entry_pathname },
        { "__gc",    ar_entry_destroy },
        { NULL, NULL }
    };
    luaL_register(L, NULL, m_fns); // {1}

    lua_pop(L, 1);
    return 0;
}
