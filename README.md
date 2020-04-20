# LD46
This repository contains the source code and assets for my Ludum Dare 46 entry.

Note: if you are looking here before the 18th of April 2020, this repo only contains some base code.

## Compiling

### Linux

A Linux build *is* provided but it is strongly advised that you compile it on your own.

You will need:
 - A C++17 compliant compiler (GCC 7 or newer, or Clang 5 or newer recommended)
 - SDL2 libraries (both the normal and devel variant, eg. libsdl2 and libsdl2-dev)
 - SDL2\_mixer libraries (both the normal and devel variant, eg. SDL2\_mixer and SDL2\_mixer-devel)
 - libepoxy (both the normal and devel variant, eg. libepoxy and libepoxy-dev)
 - Meson and ninja

To build, do the following:
```
$ meson build
$ ninja -C build
```

After that, you can run the game by doing running `build/ldeng`

Note: make sure the `res` directory is in the current working directory when running the game.

### Windows

A Windows build is provided and it shouldn't be required for you to compile it on your own, unless you want to compile it for 32-bit machines or you want to hack on the source code.

#### On Windows
lol good luck; I haven't done this.

#### On Linux

You will need:
 - A C++17 compliant cross-compiler that targets `x86_64-w64-mingw32` (or some other mingw32 toolchain if you modify the cross file)
 - SDL2 mingw developer package installed into the mingw directories (`/usr/x86_64-w64-mingw32/{bin,lib,include}`)
 - SDL2\_mixer mingw developer package installed into the mingw directories (`/usr/x86_64-w64-mingw32/{bin,lib,include}`)
 - libepoxy installed into mingw directories (you need to do this manually)
 - Meson and ninja

This definitely requires some manual work, you will probably need to modify the cross file to suit your particular mingw setup.

To build, do roughly the following:
```
$ meson build --cross-file void-mingw32.txt --pkg-config-path /path/to/mingw/sysroot/lib/pkgconfig --build.pkg-config-path /path/to/mingw/sysroot/lib/pkgconfig
$ ninja -C build
$ mkdir run && cd run
$ cp -r ../res .
$ cp /path/to/mingw/sysroot/{bin,lib}/*.dll .
$ cp ../build/ldeng.exe .
```

Note: `/path/to/mingw/sysroot` refers to the directory where mingw stores it's `lib`, `bin` and `include` directories, like `/usr/x86_64-w64-mingw32/`. This path is also referenced in the void-mingw32.txt cross file, that's why you need to modify it.

##### Compiling libepoxy for Windows with mingw

You need to do this before compiling the actual game. libepoxy depends on roughly the same things as the game, and you can use the same cross file (with the same modifications).

To build libepoxy do roughly:
```
$ git clone https://github.com/anholt/libepoxy && cd libepoxy
$ meson build --cross-file path/to/cross-file --prefix /
$ ninja -C build
$ sudo DESTDIR=/path/to/mingw/sysroot ninja -C build install
```
