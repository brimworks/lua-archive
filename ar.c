#include <archive.h>
#include <archive_entry.h>
#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#include "ar_registry.h"
#include "ar_read.h"
#include "ar_write.h"
#include "ar_entry.h"

//////////////////////////////////////////////////////////////////////
static int ar_version(lua_State *L) {
    const char* version = archive_version_string();
    int         count   = strlen(version) + 1;
    char*       cur     = (char*)memcpy(lua_newuserdata(L, count),
                                        version, count);

    count = 0;
    while ( *cur ) {
        char* begin = cur;
        // Find all digits:
        while ( isdigit(*cur) ) cur++;
        if ( begin != cur ) {
            int is_end = *cur == '\0';
            *cur = '\0';
            lua_pushnumber(L, atoi(begin));
            count++;
            if ( is_end ) break;
            cur++;
        }
        while ( *cur && ! isdigit(*cur) ) cur++;
    }

    return count;
}

//////////////////////////////////////////////////////////////////////
LUALIB_API int luaopen_archive(lua_State *L) {
    lua_newtable(L);

    static luaL_reg fns[] = {
        { "version",     ar_version },
        { NULL, NULL }
    };
    luaL_register(L, NULL, fns);

    lua_archive_registry(L);
    lua_archive_read(L);
    lua_archive_write(L);
    lua_archive_entry(L);

    return 1;
}
