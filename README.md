# Robikz's Input Test

This desktop application runs on SDL3 and lets you move 2D shapes across
the screen using a keyboard or gamepad. Press specific keys or buttons
to make the shapes flash. Each connected input device controls its own
shape, and devices can be plugged in or removed at any time.

Use this app to measure your input device's reaction time against your
display and identify any input lag.

// **TODO:** short demo vid goes here

## Compilation

The app can be installed by building it from source.

### Requirements

- CMake 3.22 or newer
- C++17 compatible compiler
- Internet connection for downloading dependencies:
    - SDL3
    - Dear ImGui

### Building - Linux

On a Linux platform it should be sufficient to clone the repository and
`cd` into it, then:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Building - Windows

On MS Windows, <https://winlibs.com/> with UCRT runtime and Win64 POSIX threads
is the suggested compiler. The compiler executables, GNU `make` and CMake should
be on `%PATH%`. Then the compilation steps should be identical to Linux.

## Usage

After building, the executable will appear in `./src` subdirectory:

```bash
./src/robikzinputtest
```

The application displays basic usage info upon launch on the help overlay.

**Measuring Input Lag:**

The app, by itself, is merely reacting to your input by moving or flashing
shapes on screen. To measure actual response time, record your input
device and screen simultaneously with a camera, then count frames
between input activation and visual response.

For quick testing: if lag is noticeable during normal use, it's probably significant.

## Platforms

The app has been developed and tested on the following platforms:

- Microsoft Windows 10
- Linux Ubuntu 24.04
- Steam Deck

## License

MIT License. See [LICENSE](LICENSE.txt).
