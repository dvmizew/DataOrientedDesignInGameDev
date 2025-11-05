## DataOrientedDesignInGameDev

---

### Quick Start: Clone, Build & Run

Clone the repository (with submodules):

```bash
git clone --recursive https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
```

If you already cloned without --recursive, initialize submodules:

```bash
git submodule update --init --recursive
```

Build and run (example for Linux):

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
./DataOrientedDesignInGameDev
```

For other platforms (macOS, Windows), see the detailed instructions below.

---

**ImGui as a git submodule**

ImGui is included as a git submodule in this project (under `external/imgui`). Make sure you have initialized and updated submodules before building. If the submodule is missing, see the Quick Start section above.

---

What the project uses:
- SDL3 (core library)
- SDL3_image (loading PNG/JPG images)
- ImGui (immediate mode GUI library; included as a git submodule in `external/imgui`)

> **Note:** ImGui is included as a git submodule under `external/imgui`. You do not need to install ImGui separately or use a package manager for it, but you must initialize the submodule as shown above.

Important files:
- `CMakeLists.txt` (project build configuration)
- `src/` (source code)
- `assets/` (image assets)
- `external/` (external dependencies, e.g. ImGui)

---

1) General build steps (valid for all platforms, package names vary per OS):

- Create a build directory and run CMake:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
```

- The produced executable will be `DataOrientedDesignInGameDev` (or `DataOrientedDesignInGameDev.exe` on Windows). Make sure to run the executable from the build directory so `../assets/dragan.png` can be found, or copy `assets/` to the build directory.

---

2) Platform-specific instructions

Linux (Debian / Ubuntu)
- Install dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake libsdl3-dev libsdl3-image-dev
```

- Build:

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
```

- Run (from `build`):

```bash
./DataOrientedDesignInGameDev
```

Linux (Fedora)
- Install dependencies:

```bash
sudo dnf install @development-tools cmake SDL3-devel SDL3_image-devel
```

- Build & run: use the same CMake steps shown above.

Linux (Arch / Manjaro)
- Install dependencies:

```bash
sudo pacman -Syu
sudo pacman -S base-devel cmake sdl3 sdl3_image
```

- Build & run: same CMake steps.

macOS (Homebrew)
- Make sure Xcode Command Line Tools are installed (`xcode-select --install`).
- Install SDL3 and SDL3_image with Homebrew:

```bash
brew update
brew install sdl3 sdl3_image cmake
```

- Build:

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

- Run (from `build`):

```bash
./DataOrientedDesignInGameDev
```

Windows (MSYS2 / MinGW-w64)
- Download and install MSYS2: https://www.msys2.org/
- Open the "MSYS2 MinGW 64-bit" shell (for 64-bit builds). Then:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3_image
```

- Build (inside the MinGW64 shell):

```bash
mkdir -p build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

- Run (from `build`):

```bash
./DataOrientedDesignInGameDev.exe
```

Windows (vcpkg + Visual Studio / CMake)
- If you use `vcpkg`, install packages:

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install sdl3 sdl3-image
```

- Integrate vcpkg with CMake (option 1: pass the toolchain file):

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build .
```

- Or use Visual Studio integration: `./vcpkg integrate install` and build from Visual Studio or use the Visual Studio CMake generator.

Notes for Windows:
- Use the appropriate shell (MinGW vs Visual Studio) depending on which packages/toolchain you installed.
- If using MSYS2/MinGW, use the `MinGW Makefiles` or Ninja generator.

---

3) Common problems and troubleshooting
- "SDL.h: No such file or directory" → the development packages are not installed or CMake cannot find SDL3. Verify `libsdl3-dev` / `SDL3-devel` / `sdl3` are installed.
- Asset path issues → run the executable from the build directory or copy `assets/` into the build directory.
- If CMake cannot find SDL3, make sure your CMake version is recent enough (project requires at least 3.10). You can also set `CMAKE_PREFIX_PATH` to the SDL3 installation prefix.

---

4) Quick copy & paste examples

Linux (Ubuntu):

```bash
sudo apt update
sudo apt install build-essential cmake libsdl3-dev libsdl3-image-dev
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
./DataOrientedDesignInGameDev
```

macOS (Homebrew):

```bash
brew install cmake sdl3 sdl3_image
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
./DataOrientedDesignInGameDev
```

Windows (MSYS2 MinGW64):

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3_image
mkdir -p build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./DataOrientedDesignInGameDev.exe
```
