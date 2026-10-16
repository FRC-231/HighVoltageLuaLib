# HighVoltageLuaLib

Lua scripting for FRC robot code: a C++ wrapper around Lua 5.4 and LuaBridge3 that
lets autonomous routines live in `.lua` files deployed alongside the robot program,
so they can be edited and re-deployed without rebuilding C++.

Maintained by FRC Team 231 (High Voltage).

**This library does not depend on WPILib.** It needs nothing but a C++20 compiler,
Lua and LuaBridge3, both of which are vendored in `third_party/`.

## Using it in a robot project

Download `HighVoltageLuaLib.json` and drop it in your project's `vendordeps/`
directory, then rebuild:

```bash
curl -O https://frc-231.github.io/HighVoltageLuaLib/HighVoltageLuaLib.json
mv HighVoltageLuaLib.json vendordeps/
./gradlew build
```

That is all — no submodule, no `srcDir` lines, no separate Lua checkout.

### Example

```cpp
#include "Keeko/ThreadedScript.h"

HighVoltage::ThreadedScript script;

script.setOnFailCallback([]() { /* required - see Known issues */ });
script.addInstance("drive", &drive_system);
script.runFile("/home/lvuser/deploy/scripts/autons/center.lua");
```

```lua
-- center.lua
drive:driveForward(2.0)
control:sleep(1.5)
drive:stop()
```

`control` is bound automatically and exposes `sleep(seconds)` and
`waitUntilTrue(callback)`. Both abort early when the script is stopped.

## Supported platforms

| Platform | Built on |
|---|---|
| `linuxathena` (roboRIO) | Ubuntu CI, cross-compiled |
| `linuxx86-64` | Ubuntu CI |
| `windowsx86-64` | Windows CI |

No macOS build. Nobody on the team runs it; add `macos-latest` to
`.github/workflows/publish.yml` and `osxuniversal` to the vendordep's
`binaryPlatforms` if that changes.

## Building this repository

```bash
./gradlew build           # compile for every platform this machine can target
./gradlew test            # run the GoogleTest suite on the desktop
./gradlew publishToMavenLocal
```

`./gradlew installRoboRioToolchain` once, first, if you want the athena build.

### Testing a change against a robot project before publishing

```bash
./gradlew publishToMavenLocal
```

then in the robot project, point a scratch copy of the vendordep json at your
local maven cache:

```json
"mavenUrls": [ "file:///home/<you>/.m2/repository/" ]
```

This exercises the real published artifact — headers zip included — without
touching GitHub Pages. It is the fastest way to catch a packaging mistake.

## Layout

```
src/main/native/include/Keeko/   public headers (published)
src/main/native/cpp/             implementation
src/test/native/cpp/             GoogleTest suite
third_party/lua/                 vendored Lua 5.4.4
third_party/LuaBridge3/          vendored LuaBridge3 (single header)
```

The `Keeko/` directory name is historical — the namespace was renamed to
`HighVoltage`, but the include prefix was kept so existing `#include` lines
still work. It also keeps generic names like `LuaState.h` from colliding at the
include root.

## Known issues

These are real defects, each with a corresponding `DISABLED_` test in
`src/test/native/cpp/ThreadedScriptTest.cpp`. Re-enable the test when you fix one.

1. **A failing script with no fail-callback kills the robot program.**
   `runFile()` calls `onFailCallback()` unconditionally when the script throws,
   but the member is an empty `std::function` unless `setOnFailCallback()` was
   called. Invoking it throws `std::bad_function_call` on the worker thread,
   where nothing catches it, so the process calls `std::terminate()`.
   **Until this is fixed, always call `setOnFailCallback()`.**
2. **`running` is a non-atomic `bool`** written by the worker thread and read
   from others — a data race. It also means `isRunning()` can return `false`
   immediately after `runFile()` returns, because the flag is set inside the
   new thread.
3. **`~ThreadedScript()` does not notify `kill_cv`**, so destroying a script
   parked in `control:sleep(30)` blocks for the remaining sleep. `stop()` does
   notify; the destructor should just call it.
4. **`LuaState::loadFile()` does not check that the file opened**, so a
   mistyped path silently runs an empty chunk instead of reporting an error.
5. **`LuaState::addGlobalFunction()` is declared but never defined** — calling
   it is a link error.

## Versioning

One branch per season (`2026`, `2027`, ...), `master` tracking the current one.
Releases are tagged `v2026.1.0`; pushing a `v*` tag publishes to GitHub Pages.
A robot project pins an exact version string in its vendordep json, so an
in-season change here can never silently alter a competition-ready robot.
