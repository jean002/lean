/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include <sstream>
#include <lua.hpp>
#include "util/debug.h"
#include "util/name.h"
#include "util/sstream.h"
#include "util/sexpr/options.h"
#include "util/sexpr/option_declarations.h"
#include "bindings/lua/util.h"
#include "bindings/lua/name.h"
#include "bindings/lua/io_state.h"

namespace lean {
DECL_UDATA(options)

static int mk_options(lua_State * L) {
    options r;
    int nargs = lua_gettop(L);
    if (nargs % 2 != 0)
        throw exception("options expects an even number of arguments");
    for (int i = 1; i < nargs; i+=2) {
        name k = to_name_ext(L, i);
        auto it = get_option_declarations().find(k);
        if (it == get_option_declarations().end()) {
            throw exception(sstream() << "unknown option '" << k.to_string().c_str() << "'");
        } else {
            option_declaration const & d = it->second;
            switch (d.kind()) {
            case BoolOption:      r = r.update(k, static_cast<bool>(lua_toboolean(L, i+1))); break;
            case IntOption:       r = r.update(k, static_cast<int>(lua_tointeger(L, i+1))); break;
            case UnsignedOption:  r = r.update(k, static_cast<unsigned>(lua_tointeger(L, i+1))); break;
            case DoubleOption:    r = r.update(k, static_cast<double>(lua_tonumber(L, i+1))); break;
            case StringOption:    r = r.update(k, lua_tostring(L, i+1)); break;
            default:              throw exception(sstream() << "unsupported option kind for '" << k.to_string().c_str() << "'");
            }
        }
    }
    return push_options(L, r);
}

static int options_tostring(lua_State * L) {
    std::ostringstream out;
    out << to_options(L, 1);
    lua_pushstring(L, out.str().c_str());
    return 1;
}

static int options_size(lua_State * L) {
    lua_pushinteger(L, to_options(L, 1).size());
    return 1;
}

static int options_contains(lua_State * L) {
    lua_pushboolean(L, to_options(L, 1).contains(to_name_ext(L, 2)));
    return 1;
}

static int options_empty(lua_State * L) {
    lua_pushboolean(L, to_options(L, 1).empty());
    return 1;
}

static int options_get_bool(lua_State * L) {
    int nargs = lua_gettop(L);
    bool defval = nargs < 3 ? false : lua_toboolean(L, 3);
    lua_pushboolean(L, to_options(L, 1).get_bool(to_name_ext(L, 2), defval));
    return 1;
}

static int options_get_int(lua_State * L) {
    int nargs = lua_gettop(L);
    int defval = nargs < 3 ? 0 : lua_tointeger(L, 3);
    lua_pushinteger(L, to_options(L, 1).get_int(to_name_ext(L, 2), defval));
    return 1;
}

static int options_get_unsigned(lua_State * L) {
    int nargs = lua_gettop(L);
    unsigned defval = nargs < 3 ? 0 : lua_tointeger(L, 3);
    lua_pushnumber(L, to_options(L, 1).get_unsigned(to_name_ext(L, 2), defval));
    return 1;
}

static int options_get_double(lua_State * L) {
    int nargs = lua_gettop(L);
    double defval = nargs < 3 ? 0.0 : lua_tonumber(L, 3);
    lua_pushnumber(L, to_options(L, 1).get_double(to_name_ext(L, 2), defval));
    return 1;
}

static int options_get_string(lua_State * L) {
    int nargs = lua_gettop(L);
    char const * defval = nargs < 3 ? "" : lua_tostring(L, 3);
    lua_pushstring(L, to_options(L, 1).get_string(to_name_ext(L, 2), defval));
    return 1;
}

static int options_update_bool(lua_State * L) {
    return push_options(L, to_options(L, 1).update(to_name_ext(L, 2), static_cast<bool>(lua_toboolean(L, 3))));
}

static int options_update_int(lua_State * L) {
    return push_options(L, to_options(L, 1).update(to_name_ext(L, 2), static_cast<int>(lua_tointeger(L, 3))));
}

static int options_update_unsigned(lua_State * L) {
    return push_options(L, to_options(L, 1).update(to_name_ext(L, 2), static_cast<unsigned>(lua_tointeger(L, 3))));
}

static int options_update_double(lua_State * L) {
    return push_options(L, to_options(L, 1).update(to_name_ext(L, 2), lua_tonumber(L, 3)));
}

static int options_update_string(lua_State * L) {
    return push_options(L, to_options(L, 1).update(to_name_ext(L, 2), lua_tostring(L, 3)));
}

static int options_get(lua_State * L) {
    name k = to_name_ext(L, 2);
    auto it = get_option_declarations().find(k);
    if (it == get_option_declarations().end()) {
        throw exception(sstream() << "unknown option '" << k.to_string().c_str() << "'");
    } else {
        option_declaration const & d = it->second;
        switch (d.kind()) {
        case BoolOption:      return options_get_bool(L);
        case IntOption:       return options_get_int(L);
        case UnsignedOption:  return options_get_unsigned(L);
        case DoubleOption:    return options_get_double(L);
        case StringOption:    return options_get_string(L);
        default:              throw exception(sstream() << "unsupported option kind for '" << k.to_string().c_str() << "'");
        }
    }
}

static int options_update(lua_State * L) {
    name k = to_name_ext(L, 2);
    auto it = get_option_declarations().find(k);
    if (it == get_option_declarations().end()) {
        throw exception(sstream() << "unknown option '" << k.to_string().c_str() << "'");
    } else {
        option_declaration const & d = it->second;
        switch (d.kind()) {
        case BoolOption:      return options_update_bool(L);
        case IntOption:       return options_update_int(L);
        case UnsignedOption:  return options_update_unsigned(L);
        case DoubleOption:    return options_update_double(L);
        case StringOption:    return options_update_string(L);
        default:              throw exception(sstream() << "unsupported option kind for '" << k.to_string().c_str() << "'");
        }
    }
}

static char g_options_key;

options get_global_options(lua_State * L) {
    io_state * S = get_io_state(L);
    if (S != nullptr) {
        return S->get_options();
    } else {
        lua_pushlightuserdata(L, static_cast<void *>(&g_options_key));
        lua_gettable(L, LUA_REGISTRYINDEX);
        options r;
        if (is_options(L, -1))
            r = to_options(L, -1);
        lua_pop(L, 1);
        return r;
    }
}

void set_global_options(lua_State * L, options const & o) {
    io_state * S = get_io_state(L);
    if (S != nullptr) {
        S->set_options(o);
    } else {
        lua_pushlightuserdata(L, static_cast<void *>(&g_options_key));
        push_options(L, o);
        lua_settable(L, LUA_REGISTRYINDEX);
    }
}

static int _get_global_options(lua_State * L) {
    return push_options(L, get_global_options(L));
}

static int _set_global_options(lua_State * L) {
    options o = to_options(L, 1);
    set_global_options(L, o);
    return 0;
}

static int _set_global_option(lua_State * L) {
    options o = get_global_options(L);
    push_options(L, o);
    lua_insert(L, 1);
    options_update(L);
    o = to_options(L, -1);
    set_global_options(L, o);
    return 0;
}

static const struct luaL_Reg options_m[] = {
    {"__gc",            options_gc}, // never throws
    {"__tostring",      safe_function<options_tostring>},
    {"__len",           safe_function<options_size> },
    {"contains",        safe_function<options_contains>},
    {"size",            safe_function<options_size>},
    {"empty",           safe_function<options_empty>},
    {"get",             safe_function<options_get>},
    {"update",          safe_function<options_update>},
    // low-level API
    {"get_bool",        safe_function<options_get_bool>},
    {"get_int",         safe_function<options_get_int>},
    {"get_unsigned",    safe_function<options_get_unsigned>},
    {"get_double",      safe_function<options_get_double>},
    {"get_string",      safe_function<options_get_string>},
    {"update_bool",     safe_function<options_update_bool>},
    {"update_int",      safe_function<options_update_int>},
    {"update_unsigned", safe_function<options_update_unsigned>},
    {"update_double",   safe_function<options_update_double>},
    {"update_string",   safe_function<options_update_string>},
    {0, 0}
};

void open_options(lua_State * L) {
    luaL_newmetatable(L, options_mt);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    setfuncs(L, options_m, 0);

    SET_GLOBAL_FUN(mk_options,          "options");
    SET_GLOBAL_FUN(options_pred,        "is_options");
    SET_GLOBAL_FUN(_get_global_options, "get_options");
    SET_GLOBAL_FUN(_set_global_options, "set_options");
    SET_GLOBAL_FUN(_set_global_option,  "set_option");
}
}
