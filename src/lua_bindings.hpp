#ifndef LUA_BINDINGS_HPP
#define LUA_BINDINGS_HPP

#include <lua.hpp>
#include "sonify.hpp"

class LuaBindings
{

public:
    LuaBindings() {}


    int l_Open(lua_State *L);
    int l_Msg(lua_State *L);


};

#endif
