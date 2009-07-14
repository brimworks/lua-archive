#include <archive.h>
#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#define AR_WRITE "archive{write}"

static __LA_SSIZE_T ar_write_cb(struct archive * ar,
                                void *opaque,
                                const void *buff, size_t len);
static int ar_close_cb(struct archive * ar,
                       void *opaque);
static int ar_version(lua_State *L);
static int ar_write(lua_State *L);

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
static int ar_write(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    struct archive** ar = (struct archive**)lua_newuserdata(L, sizeof(struct archive*));
    luaL_getmetatable(L, AR_WRITE);
    lua_setmetatable(L, -2);
    *ar = archive_write_new();

#define err(...) do {         \
        archive_write_finish(*ar);         \
        luaL_error(L, __VA_ARGS__); \
    } while ( 0 )

    lua_getfield(L, 1, "bytes_per_block");
    if ( ! lua_isnil(L, -1) &&
         ARCHIVE_OK != archive_write_set_bytes_per_block(*ar, lua_tointeger(L, -1)) )
    {
        err("archive_write_set_bytes_per_block: %s", archive_error_string(*ar));
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "bytes_in_last_block");
    if ( ! lua_isnil(L, -1) &&
         ARCHIVE_OK != archive_write_set_bytes_in_last_block(*ar, lua_tointeger(L, -1)) )
    {
        err("archive_write_set_bytes_in_last_block: %s", archive_error_string(*ar));
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "skip_file");
    if ( ! lua_isnil(L, -1) ) {
        dev_t dev;
        ino_t ino;

        if ( LUA_TTABLE != lua_type(L, -1) ) {
            err("skip_file member must be a table object");
        }

        lua_getfield(L, -1, "dev");
        if ( ! lua_isnumber(L, -1) ) {
            err("skip_file.dev member must be a number");
        }
        dev = (dev_t)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "ino");
        if ( ! lua_isnumber(L, -1) ) {
            err("skip_file.ino member must be a number");
        }
        ino = (ino_t)lua_tonumber(L, -1);
        lua_pop(L, 1);

        if ( ARCHIVE_OK != archive_write_set_skip_file(*ar, dev, ino) ) {
            err("archive_write_set_skip_file: %s", archive_error_string(*ar));
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "format");
    if ( ! lua_isnil(L, -1) ) {
        static struct {
            const char *name;
            int (*setter)(struct archive *);
        } names[] = {
            /* Copied from archive_write_set_format_by_name.c */
            { "ar",         archive_write_set_format_ar_bsd },
            { "arbsd",      archive_write_set_format_ar_bsd },
            { "argnu",      archive_write_set_format_ar_svr4 },
            { "arsvr4",     archive_write_set_format_ar_svr4 },
            { "cpio",       archive_write_set_format_cpio },
            { "mtree",      archive_write_set_format_mtree },
            { "newc",       archive_write_set_format_cpio_newc },
            { "odc",        archive_write_set_format_cpio },
            { "pax",        archive_write_set_format_pax },
            { "posix",      archive_write_set_format_pax },
            { "shar",       archive_write_set_format_shar },
            { "shardump",   archive_write_set_format_shar_dump },
            { "ustar",      archive_write_set_format_ustar },
            /* New ones to more closely match the C API */
            { "ar_bsd",     archive_write_set_format_ar_bsd },
            { "ar_svr4",    archive_write_set_format_ar_svr4 },
            { "cpio_newc",  archive_write_set_format_cpio_newc },
            { "pax_restricted", archive_write_set_format_pax_restricted },
            { "shar_dump",  archive_write_set_format_shar_dump },
            { NULL,         NULL }
        };
        int idx = 0;
        const char* name = lua_tostring(L, -1);
        for ( ;; idx++ ) {
            if ( names[idx].name == NULL ) {
                err("archive_write_set_format_*: No such format '%s'", name);
            }
            if ( strcmp(name, names[idx].name) == 0 ) break;
        }
        if ( ARCHIVE_OK != (names[idx].setter)(*ar) ) {
            err("archive_write_set_format_*: %s", archive_error_string(*ar));
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "compression");
    if ( ! lua_isnil(L, -1) ) {
        static struct {
            const char *name;
            int (*setter)(struct archive *);
        } names[] = {
            { "bzip2",    archive_write_set_compression_bzip2 },
            { "compress", archive_write_set_compression_compress },
            { "gzip",     archive_write_set_compression_gzip },
            { "lzma",     archive_write_set_compression_lzma },
            { "xz",       archive_write_set_compression_xz },
            { NULL,       NULL }
        };
        int idx = 0;
        const char* name = lua_tostring(L, -1);
        for ( ;; idx++ ) {
            if ( names[idx].name == NULL ) {
                err("archive_write_set_compression_*: No such compression '%s'", name);
            }
            if ( strcmp(name, names[idx].name) == 0 ) break;
        }
        if ( ARCHIVE_OK != (names[idx].setter)(*ar) ) {
            err("archive_write_set_compression_*: %s", archive_error_string(*ar));
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, "options");
    if ( ! lua_isnil(L, -1) &&
         ARCHIVE_OK != archive_write_set_options(*ar, lua_tostring(L, -1)) )
    {
        err("archive_write_set_options: %s",  archive_error_string(*ar));
    }
    lua_pop(L, 1);

#undef err

    archive_write_open(*ar, L, NULL, &ar_write_cb, &ar_close_cb);

    return 0;
}

//////////////////////////////////////////////////////////////////////
static __LA_SSIZE_T ar_write_cb(struct archive * ar,
                          void *opaque,
                          const void *buff, size_t len)
{
    lua_State* L = (lua_State*)opaque;
    luaL_error(L, "Not implemented!");
    return 0;
}

//////////////////////////////////////////////////////////////////////
static int ar_close_cb(struct archive * ar,
                          void *opaque)
{
    lua_State* L = (lua_State*)opaque;
    luaL_error(L, "Not implemented!");
    return 0;
}

//////////////////////////////////////////////////////////////////////
LUALIB_API int luaopen_archive(lua_State *L) {
    lua_createtable(L, 0, 8);

    lua_pushcfunction(L, ar_version);
    lua_setfield(L, -2, "version");

    lua_pushcfunction(L, ar_write);
    lua_setfield(L, -2, "write");

    return 1;
}
