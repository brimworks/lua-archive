#define AR_REGISTRY  "archive{registry}"

void lua_archive_registry(lua_State *L);
void lua_archive_register(lua_State *L, void *ptr);
int lua_archive_get(lua_State *L, void *ptr);
