#include "lua_bindings.hpp"

int LuaBindings::l_Open(lua_State *L)
{
    Sonify *s = reinterpret_cast<Sonify*>(lua_touserdata(L, 1));
    QString filepath = QString::fromStdString(luaL_checkstring(L, 1));
    s->Open(filepath);
    return 1;
}

int LuaBindings::l_Msg(lua_State *L)
{
    Sonify *s = reinterpret_cast<Sonify*>(lua_touserdata(L, 1));
    QString msg = QString::fromStdString(luaL_checkstring(L, 1));

}
