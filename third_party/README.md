# Bundled third-party code

| Component | Version | Upstream | License |
|---|---|---|---|
| Lua | 5.4.4 | https://www.lua.org/ | MIT |
| LuaBridge3 | (see `LuaBridge3/LuaBridge.h`) | https://github.com/kunitoki/LuaBridge3 | MIT |

Both are vendored rather than fetched, so this repository builds with no network
access and the exact Lua version is pinned to whatever the robot was tested against.

## Lua

`third_party/lua/` is an unmodified copy of the upstream Lua 5.4.4 source tree.

`lua.c` and `luac.c` are excluded from the build in `build.gradle` — they are the
standalone interpreter and compiler, and both define `main()`.

Lua is compiled with no `LUA_USE_*` platform defines, matching how `FRC231_2026`
built it before the split. That means ANSI C mode: no `dlopen`-based `require` of
binary modules, no `readline`. Adding `LUA_USE_LINUX` would change behaviour on
the roboRIO, so it is deliberately left off.

Only Lua's five public headers (`lua.h`, `luaconf.h`, `lauxlib.h`, `lualib.h`,
`lua.hpp`) are published in the vendordep headers zip. The other 22 headers in
`lua/src` are internal to the implementation and must not land on a consumer's
include path — see the `cppHeadersZip` task in `publish.gradle`.

## LuaBridge3

Header-only, single file. `LuaBridge.h` is published in the headers zip because
`Keeko/LuaState.h`, `Keeko/Scriptable.h` and `Keeko/ThreadedScript.h` all include
it, making it part of this library's public interface.
