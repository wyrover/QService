/**
 * Copyright (c) 2009, Sun Microsystems Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sun Microsystems Inc. nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <string.h>
#include <stdlib.h>

#include "sigar.h"
#include "lua-sigar.h"

int lua_sigar_version_get(lua_State *L) {
#ifdef WIN32
    sigar_t *s = *(sigar_t **)luaL_checkudata(L, 1, "sigar");
#else
    (void)luaL_checkudata(L, 1, "sigar");
#endif	
	sigar_version_t *version;

	version = sigar_version_get();

	lua_newtable(L);
#define DATA \
	(version)

	LUA_EXPORT_STR(DATA, build_date);
	LUA_EXPORT_STR(DATA, scm_revision);
	LUA_EXPORT_STR(DATA, version);
	LUA_EXPORT_STR(DATA, archname);
	LUA_EXPORT_STR(DATA, archlib);
	LUA_EXPORT_STR(DATA, binname);
	LUA_EXPORT_STR(DATA, description);
	LUA_EXPORT_INT(DATA, major);
	LUA_EXPORT_INT(DATA, minor);
	LUA_EXPORT_INT(DATA, maint);
	LUA_EXPORT_INT(DATA, build);

#undef DATA

	return 1;
}

