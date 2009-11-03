// This is a private header subject to change.

#define AR_ENTRY "archive{entry}"

#define ar_entry_check(L, narg) \
    ((struct archive_entry**)luaL_checkudata((L), (narg), AR_ENTRY))

int lua_archive_entry(lua_State *L);
int ar_entry(lua_State *L);
