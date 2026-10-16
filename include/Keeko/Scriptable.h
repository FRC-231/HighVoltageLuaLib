#pragma once

//Lua Includes
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "LuaBridge.h"

//Keeko includes
#include "Keeko/LuaState.h"


namespace HighVoltage
{

class Scriptable
{
public:
    virtual void addClassToState(lua_State* state) = 0;
    inline void addClassToState(const HighVoltage::LuaState& state)
    {
        lua_State* ls = state;
        addClassToState(ls);
    }
};

} // namespace HighVoltage