// This is a private header subject to change.

/*
 * 64-bit integers
 */
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64			int64_t;
typedef unsigned __int64	uint64_t;
#else
#include <stdint.h>
#endif

#if LUA_VERSION_NUM >= 502
#define luaL_register(L,n,l)    luaL_setfuncs((L), (l), 0)
#define lua_setfenv		lua_setuservalue
#define lua_getfenv		lua_getuservalue
#endif

#define AR_ENTRY "archive{entry}"

#define ar_entry_check(L, narg) \
    ((struct archive_entry**)luaL_checkudata((L), (narg), AR_ENTRY))

int ar_entry_init(lua_State *L);
int ar_entry(lua_State *L);
