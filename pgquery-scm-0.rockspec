package = "pgquery"
version = "scm-0"
source = {
	url = "git+https://github.com/daurnimator/lua-pgquery.git";
}
description = {
	summary = "Bindings to libpg_query, a C library for accessing the PostgreSQL parser outside of the server.";
	-- detailed = [[]];
	homepage = "https://github.com/daurnimator/lua-pgquery";
	license = "MIT";
}
dependencies = {}
external_dependencies = {
	PG_QUERY = {
		header = "pg_query.h";
		library = "pg_query";
	};
}
build = {
	type = "builtin";
	modules = {
		pgquery = {
			sources = "src/main.c";
			libraries = {
				"pg_query";
			};
			defines = {
			};
			incdirs = {
				"$(PG_QUERY_INCDIR)";
			};
			libdirs = {
				"$(PG_QUERY_LIBDIR)";
			};
		};
	};
}
