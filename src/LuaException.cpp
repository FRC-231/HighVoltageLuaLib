#include "Keeko/LuaException.h"

namespace HighVoltage
{

LuaException::LuaException(lua_State* state)
{
    error = lua_tostring(state, -1);
}

}