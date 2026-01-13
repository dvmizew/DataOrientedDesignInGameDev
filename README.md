## DataOrientedDesignInGameDev

---

### Build & Run

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

git clone https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./DataOrientedDesignInGameDev
```

**Windows (vcpkg):**
```batch
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg install sdl3:x64-windows sdl3-image:x64-windows sdl3-ttf:x64-windows

git clone https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
Release\DataOrientedDesignInGameDev.exe
```

**Alternative: submodules:**
```bash
git clone --recursive https://github.com/dvmizew/DataOrientedDesignInGameDev.git
cd DataOrientedDesignInGameDev
mkdir build && cd build
cmake ..
cmake --build .
```

---

### Other platforms

**Fedora:**
```bash
sudo dnf install SDL3-devel SDL3_image-devel SDL3_ttf-devel
```

**Arch-based:**
```bash
sudo pacman -S sdl3 sdl3_image sdl3_ttf
```

**macOS (Homebrew):**
```bash
brew install sdl3 sdl3_image sdl3_ttf
```