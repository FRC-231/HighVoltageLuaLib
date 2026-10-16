# Third-Party Notices

HighVoltageLuaLib is licensed under the GNU General Public License v3.0
(see [LICENSE](LICENSE)). It bundles and redistributes the following
third-party components, each under its own GPLv3-compatible license:

## Lua 5.4.4

- License: MIT
- Upstream: https://www.lua.org/
- Vendored at `third_party/lua/`, an unmodified copy of the upstream source
  tree. `lua.c` and `luac.c` are excluded from the build (both define
  `main()`); no Lua source is otherwise modified.
- Lua's five public headers (`lua.h`, `luaconf.h`, `lauxlib.h`, `lualib.h`,
  `lua.hpp`) are redistributed in this library's published headers archive.
  Lua's object code is linked into the published static library.
- MIT is GPLv3-compatible, so the combined work may be distributed under
  GPLv3. Lua's copyright notice and permission notice are reproduced below
  and must be preserved in any redistribution.

```
Copyright (C) 1994-2022 Lua.org, PUC-Rio.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## LuaBridge3

- License: MIT
- Upstream: https://github.com/kunitoki/LuaBridge3
- Vendored at `third_party/LuaBridge3/LuaBridge.h`, header-only and unmodified.
- Redistributed in this library's published headers archive, because
  `Keeko/LuaState.h`, `Keeko/Scriptable.h` and `Keeko/ThreadedScript.h` all
  include it, making it part of this library's public interface.
- The full copyright and permission notice is in the header of
  `LuaBridge.h` and is preserved unmodified.

```
Copyright (C) 2020 Lucio Asnaghi
Copyright (C) 2019 Dmitry Tarakanov
Copyright (C) 2012 Vinnie Falco
Copyright (C) 2007 Nathan Reed
```

## Distributing binaries of this library

The published Maven artifacts contain compiled object code covered by GPLv3.
GPLv3 §6 requires that the corresponding source be made available to anyone
receiving those binaries. This is satisfied by the public source repository at
https://github.com/FRC-231/HighVoltageLuaLib, and by the `sources` archive
published alongside each release. Keep the repository public for as long as
any released binary is distributed.
