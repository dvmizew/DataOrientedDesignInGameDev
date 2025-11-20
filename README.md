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

**SDL3, SDL3_image, SDL3_ttf as git submodules (optional)**

The project vendors SDL through submodules located in:
- `external/SDL3`
- `external/SDL3_image`
- `external/SDL3_ttf`

By default the CMake options below are ON and will build the vendored sources:
- `USE_SUBMODULE_SDL3` (default ON)
- `USE_SUBMODULE_SDL3_IMAGE` (default ON)
- `USE_SUBMODULE_SDL3_TTF` (default ON)
- `SDL_BUILD_SHARED` (default ON) → build shared SDL libraries. Set to OFF to build static.

If you prefer using system-installed packages (e.g. via your package manager or vcpkg), disable these options when configuring:

```bash
cmake -DUSE_SUBMODULE_SDL3=OFF -DUSE_SUBMODULE_SDL3_IMAGE=OFF -DUSE_SUBMODULE_SDL3_TTF=OFF ..
```

When OFF, CMake will `find_package(SDL3 REQUIRED)` etc., so ensure your toolchain can locate the SDL3 config files.

Common reasons to use submodules:
- Consistent cross-platform version (same source on Windows/Linux/macOS).
- Avoid needing development packages installed system-wide.

Common reasons to use system packages:
- Faster incremental builds (pre-built libs).
- Integration with distro security updates.

If submodule builds fail on Linux due to missing optional dependencies (e.g. XTEST), they are disabled automatically (`SDL_X11_XTEST=OFF`). Install additional packages if you need those features.

---

**ImGui as a git submodule**

ImGui is included as a git submodule in this project (under `external/imgui`). Make sure you have initialized and updated submodules before building. If the submodule is missing, see the Quick Start section above.

---

What the project uses:
- SDL3 (core library)
- SDL3_image (loading PNG/JPG images)
- SDL3_ttf (font rendering; requires FreeType)
- ImGui (immediate mode GUI library; included as a git submodule in `external/imgui`)

> **Note:** All of these can be built from source via submodules (default) or you can disable the submodules and rely on system-installed packages.

Important files:
- `CMakeLists.txt` (project build configuration)
- `src/` (source code)
- `assets/` (image assets)
- `external/` (external dependencies, e.g. ImGui, SDL3 family)

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
- Install dependencies (if NOT using submodules for SDL):

```bash
sudo apt update
sudo apt install build-essential cmake libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
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
- Install dependencies (if not using submodules):

```bash
sudo dnf install @development-tools cmake SDL3-devel SDL3_image-devel SDL3_ttf-devel
```

Linux (Arch / Manjaro)
- Install dependencies (if not using submodules):

```bash
sudo pacman -Syu
sudo pacman -S base-devel cmake sdl3 sdl3_image sdl3_ttf
```

macOS (Homebrew)
- Install dependencies (if not using submodules):

```bash
brew update
brew install sdl3 sdl3_image sdl3_ttf cmake
```

Windows (MSYS2 / MinGW-w64)
- Install dependencies (if not using submodules):

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3_image mingw-w64-x86_64-sdl3_ttf
```

Windows (vcpkg + Visual Studio / CMake)
- If you use `vcpkg`, install packages:

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install sdl3 sdl3-image sdl3-ttf
```

---

3) Toggle between submodule and system SDL

Example: use system SDL but keep SDL_image/SDL_ttf submodules:

```bash
cmake -DUSE_SUBMODULE_SDL3=OFF -DUSE_SUBMODULE_SDL3_IMAGE=ON -DUSE_SUBMODULE_SDL3_TTF=ON ..
```

Example: use only system packages:

```bash
cmake -DUSE_SUBMODULE_SDL3=OFF -DUSE_SUBMODULE_SDL3_IMAGE=OFF -DUSE_SUBMODULE_SDL3_TTF=OFF ..
```

Example: build static vendored SDL:

```bash
cmake -DSDL_BUILD_SHARED=OFF ..
```

After flipping these options, if you see cache conflicts, do a clean configure:

```bash
rm -rf build
mkdir build && cd build
cmake ..
```

---

4) Common problems and troubleshooting
- "external/SDL3 not found" → submodules not initialized; run `git submodule update --init --recursive`.
- Missing X11 optional dependencies (e.g. XTEST) → features are disabled automatically; install `libxtst-dev` if you need them and reconfigure.
- Font rendering issues → ensure `SDL3_ttf` built or installed; check that the font file path is correct.
- Asset path issues → run the executable from the build directory or copy `assets/` into the build directory.
- Switching between system and submodule builds → clean your build directory to avoid stale cache variables.

---

5) Quick copy & paste examples

Linux (Ubuntu, submodules ON):

```bash
git clone --recursive https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
./DataOrientedDesignInGameDev
```

macOS (Homebrew, system SDL):

```bash
brew install cmake sdl3 sdl3_image sdl3_ttf
git clone https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build && cd build
cmake -DUSE_SUBMODULE_SDL3=OFF -DUSE_SUBMODULE_SDL3_IMAGE=OFF -DUSE_SUBMODULE_SDL3_TTF=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
./DataOrientedDesignInGameDev
```

Windows (MSYS2, submodules OFF):

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3_image mingw-w64-x86_64-sdl3_ttf
git clone https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build && cd build
cmake -G "MinGW Makefiles" -DUSE_SUBMODULE_SDL3=OFF -DUSE_SUBMODULE_SDL3_IMAGE=OFF -DUSE_SUBMODULE_SDL3_TTF=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./DataOrientedDesignInGameDev.exe
```
