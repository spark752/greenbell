# Greenbell
A simple experimental OpenGL rendering engine using SDL2

## Requirements
* C++17 compatible compiler (developed using Clang 11.0.1)
* *SDL2* and development libraries installed
* *SDL2_ttf* and development libraries installed
* System supporting OpenGL 4.5 (released August 2014) or newer

## Build
* Clone project
* Clone submodules
```
cd greenbell
git submodule update --init
```
* Configure build for clang
```
mkdir build
cd build
cmake -C ../clang_init.cmake ..
```
* Build
```
make -j8
```

## Licenses
Greenbell code is licensed under the MIT license and is built as a static library.

Some additional items will be included in the library and therefore become part of the application:
* **glad** - Uses generated code which is public domain [Source](https://glad.dav1d.de)
* **fmt** - MIT license [Source](https://github.com/fmtlib/fmt)
* **tinygltf** - MIT license [Source](https://github.com/syoyo/tinygltf)
* **json.hpp** - MIT license, used by tinygltf [Source](https://github.com/nlohmann/json)
* **base64** - Zlib license, used by tinygltf [Source](https://github.com/ReneNyffenegger/cpp-base64)
* **stb** - public domain, used by tinygltf [Source](https://github.com/nothings/stb)
* **gcem** - Apache 2 licensed [Source](https://github.com/kthohr/gcem)

Applications will need to link to additional items that are *not* included in this source code:
* **SDL2** - zlib license [Source](https://github.com/libsdl-org/SDL)
* **SDL2_ttf** zlib license [Source](https://github.com/libsdl-org/SDL)
* **freetype** - FTL (a BSD-style license), used by SDL2_ttf [Source](https://freetype.org/license.html)

Note that Greenbell does not currently have audio support so does not require applications to link against *SDL2_mixer*. While *SDL2_mixer* uses the zlib license, it may have been built using *libmad0* which is under the GPL (not LGPL) so caution is advised.

Additional items included for demos and tests which do *not* become part of the static library and are *not* linked to the application:
* None at this time
