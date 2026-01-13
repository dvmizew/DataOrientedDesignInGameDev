# Data Oriented Design in Game Dev

## Dependencies
- SDL3, SDL3_image, SDL3_ttf. By default CMake will try system/vcpkg first, then auto-fetch from upstream (no submodules, ImGui removed).
- To disable fetching and force system packages only: add `-DDOD_FETCH_SDL=OFF`.

### Linux
System packages (Debian/Ubuntu):
```bash
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
```
Fedora:
```bash
sudo dnf install SDL3-devel SDL3_image-devel SDL3_ttf-devel
```
Then build:
```bash
cmake -B build -S .
cmake --build build
```

### Windows
Recommended with vcpkg:
```bash
vcpkg install sdl3 sdl3-image sdl3-ttf --triplet x64-windows
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```
If you prefer auto-fetch (no vcpkg/system):
```bash
cmake -B build -S . -DDOD_FETCH_SDL=ON
cmake --build build
```
Or point to prebuilt configs via `-DSDL3_DIR`, `-DSDL3_image_DIR`, `-DSDL3_ttf_DIR`.