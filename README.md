# Robikz's Input Test

This desktop application runs on SDL3 and lets you move 2D shapes across
the screen using a keyboard or gamepad. Press specific keys or buttons
to make the shapes flash. Each connected input device controls its own
shape, and devices can be plugged in or removed at any time.

Use this app to measure your input device's reaction time against your
display and identify any input lag.

[](https://github.com/user-attachments/assets/6d6ca035-67c1-4905-bcba-1dec78b2d14c)

## Compilation

The app can be installed by building it from source.

### Requirements

- CMake 3.22 or newer
- C++17 compatible compiler
- Internet connection for downloading dependencies:
    - SDL3
    - Dear ImGui

### Building - Linux

On a Linux platform it should be sufficient to clone the repository,
`cd` into it, and then:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j `nproc`
```

### Building - Windows

On MS Windows, <https://winlibs.com/> with UCRT runtime and Win64 POSIX threads
is the suggested compiler, e.g.:

```
winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-13.0.0-r3
```

The compiler executables, GNU `make` and CMake should be on `%PATH%`. Then the
compilation steps should be virtually identical to Linux.

After compilation, compiler's runtime and SDL DLLs may need to be copied to
the `bin/` dir to run the app:

```
bin/
  robikzinputtest.exe
  SDL3.dll
  libgcc_s_seh-1.dll
  libstdc++-6.dll
  libwinpthread-1.dll
```

## Usage

After building, the executable will appear in `./bin` subdirectory:

```bash
./bin/robikzinputtest
```

The application displays basic usage info upon launch on the help overlay.

**Measuring Input Lag:**

The app, by itself, is merely reacting to your input by moving or flashing
shapes on screen. To measure actual response time, record your input
device and screen simultaneously with a camera, then count frames
between input activation and visual response.

For quick testing: if lag is noticeable during normal use, it's probably significant.

## Packaging

Packaging is for a public release.

### Packaging for Linux -- AppImage

**NOTE:** Packaging operation should be run on a sufficiently old Linux system
so that compatibility with older libc versions is maintained.

Building a clean release package:

```bash
mkdir -p ./build/appimage-release && cd ./build/appimage-release
cmake -D CMAKE_BUILD_TYPE=Release ../..
cmake --build . --target clean
cmake --build . -j `nproc`
rm -rf ./AppDir && cmake --build . --target appimage
```

An `.appimage` file should appear in the build directory.

For extra cleaniness, the entire build directory can be recreated for
each release, but it should be sufficient to redo the steps starting
with `--target clean`.

### Packaging for Windows -- ZIP

From command prompt (PowerShell should also be okay), starting
at repository's root:

```cmd
mkdir build
cd build
mkdir zip-release
cd zip-release
cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release ../..
cmake --build . --target clean
cmake --build . -j32
cpack .
```

For subsequent packages, it should be sufficient to redo the steps starting
with `cmake -G ...`.

## Platforms

The app has been developed and tested on the following platforms:

- Microsoft Windows 10
- Linux Ubuntu 24.04
- Steam Deck

## License

MIT License. See [LICENSE](LICENSE.txt).

Some files may be under a different license, as denoted
by [ATTRIBUTION](ATTRIBUTION.md) or file headers.
