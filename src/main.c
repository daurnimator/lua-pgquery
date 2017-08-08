#include <pg_query.h>

#include <lua.h>
#include <lauxlib.h>

#define LUAPGQUERY_NAME "lua-pg_query"
#define LUAPGQUERY_DESCRIPTION "Bindings to libpg_query"

/* compatibility with lua 5.1 */
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
#define luaL_newmetatable(L, tn) (luaL_newmetatable(L, tn) ? (lua_pushstring(L, tn), lua_setfield(L, -2, "__name"), 1) : 0)
#define luaL_setmetatable luapgquery_setmetatable
static void luaL_setmetatable (lua_State *L, const char *tname) {
        luaL_checkstack(L, 1, "not enough stack slots");
        luaL_getmetatable(L, tname);
        lua_setmetatable(L, -2);
}
#endif

static void luapgquery_pushPgQueryError(lua_State *L, PgQueryError* error) {
	lua_createtable(L, 0, 6);
	lua_pushstring(L, error->message);
	lua_setfield(L, -2, "message");
	lua_pushstring(L, error->funcname);
	lua_setfield(L, -2, "funcname");
	lua_pushstring(L, error->filename);
	lua_setfield(L, -2, "filename");
	lua_pushinteger(L, error->lineno);
	lua_setfield(L, -2, "lineno");
	lua_pushinteger(L, error->cursorpos);
	lua_setfield(L, -2, "cursorpos");
	lua_pushstring(L, error->context);
	lua_setfield(L, -2, "context");
	luaL_setmetatable(L, "PgQueryError");
}

static int luapgquery_PgQueryNormalizeResult__gc(lua_State *L) {
	PgQueryNormalizeResult *res = lua_touserdata(L, 1);
	pg_query_free_normalize_result(*res);
	return 0;
}

static int luapgquery_normalize(lua_State *L) {
	int n;
	const char *input = luaL_checkstring(L, 1);
	PgQueryNormalizeResult *res = lua_newuserdata(L, sizeof(PgQueryNormalizeResult));
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	*res = pg_query_normalize(input);
	if (res->error) {
		lua_pushnil(L);
		luapgquery_pushPgQueryError(L, res->error);
		n = 2;
	} else {
		lua_pushstring(L, res->normalized_query);
		n = 1;
	}
	lua_pushnil(L);
	lua_setmetatable(L, -n-2);
	pg_query_free_normalize_result(*res);
	return n;
}

static int luapgquery_PgQueryParseResult__gc(lua_State *L) {
	PgQueryParseResult *res = lua_touserdata(L, 1);
	pg_query_free_parse_result(*res);
	return 0;
}

static int luapgquery_parse(lua_State *L) {
	int n;
	const char *input = luaL_checkstring(L, 1);
	PgQueryParseResult *res = lua_newuserdata(L, sizeof(PgQueryParseResult));
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	*res = pg_query_parse(input);
	if (res->error) {
		lua_pushnil(L);
		luapgquery_pushPgQueryError(L, res->error);
		/* ignore res->stderr_buffer */
		n = 2;
	} else {
		lua_pushstring(L, res->parse_tree);
		n = 1;
	}
	lua_pushnil(L);
	lua_setmetatable(L, -n-2);
	pg_query_free_parse_result(*res);
	return n;
}

static int luapgquery_PgQueryPlpgsqlParseResult__gc(lua_State *L) {
	PgQueryPlpgsqlParseResult *res = lua_touserdata(L, 1);
	pg_query_free_plpgsql_parse_result(*res);
	return 0;
}

static int luapgquery_parse_plpgsql(lua_State *L) {
	int n;
	const char *input = luaL_checkstring(L, 1);
	PgQueryPlpgsqlParseResult *res = lua_newuserdata(L, sizeof(PgQueryPlpgsqlParseResult));
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	*res = pg_query_parse_plpgsql(input);
	if (res->error) {
		lua_pushnil(L);
		luapgquery_pushPgQueryError(L, res->error);
		n = 2;
	} else {
		lua_pushstring(L, res->plpgsql_funcs);
		n = 1;
	}
	lua_pushnil(L);
	lua_setmetatable(L, -n-2);
	pg_query_free_plpgsql_parse_result(*res);
	return n;
}

static int luapgquery_PgQueryFingerprintResult__gc(lua_State *L) {
	PgQueryFingerprintResult *res = lua_touserdata(L, 1);
	pg_query_free_fingerprint_result(*res);
	return 0;
}

static int luapgquery_fingerprint(lua_State *L) {
	int n;
	const char *input = luaL_checkstring(L, 1);
	PgQueryFingerprintResult *res = lua_newuserdata(L, sizeof(PgQueryFingerprintResult));
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	*res = pg_query_fingerprint(input);
	if (res->error) {
		lua_pushnil(L);
		luapgquery_pushPgQueryError(L, res->error);
		/* ignore res->stderr_buffer */
		n = 2;
	} else {
		lua_pushstring(L, res->hexdigest);
		n = 1;
	}
	lua_pushnil(L);
	lua_setmetatable(L, -n-2);
	pg_query_free_fingerprint_result(*res);
	return n;
}

int luaopen_pgquery(lua_State *L) {
	luaL_newmetatable(L, "PgQueryError");
	lua_pop(L, 1);

	lua_createtable(L, 0, 7);

	lua_pushliteral(L, LUAPGQUERY_NAME);
	lua_setfield(L, -2, "_NAME");
	lua_pushliteral(L, LUAPGQUERY_DESCRIPTION);
	lua_setfield(L, -2, "_DESCRIPTION");
	lua_pushnil(L);
	lua_setfield(L, -2, "_VERSION");

	lua_createtable(L, 1, 0);
	lua_pushcfunction(L, luapgquery_PgQueryNormalizeResult__gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcclosure(L, luapgquery_normalize, 1);
	lua_setfield(L, -2, "normalize");

	lua_createtable(L, 1, 0);
	lua_pushcfunction(L, luapgquery_PgQueryParseResult__gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcclosure(L, luapgquery_parse, 1);
	lua_setfield(L, -2, "parse");

	lua_createtable(L, 1, 0);
	lua_pushcfunction(L, luapgquery_PgQueryPlpgsqlParseResult__gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcclosure(L, luapgquery_parse_plpgsql, 1);
	lua_setfield(L, -2, "parse_plpgsql");

	lua_createtable(L, 1, 0);
	lua_pushcfunction(L, luapgquery_PgQueryFingerprintResult__gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcclosure(L, luapgquery_fingerprint, 1);
	lua_setfield(L, -2, "fingerprint");

	return 1;
}
