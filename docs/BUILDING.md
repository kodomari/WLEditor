# Building WLEditor

This guide covers building WLEditor on various platforms.

## Prerequisites

### All Platforms
- **Qt 6.2+** (Core, Widgets modules)
- **CMake 3.16+**
- **C++17** compatible compiler

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
Linux (Fedora/RHEL)
bashsudo dnf install qt6-qtbase-devel qt6-qttools-devel cmake gcc-c++
macOS
bashbrew install qt@6 cmake
Building
Linux/macOS Standard Build
bash# Clone repository
git clone https://github.com/yourusername/WLEditor.git
cd WLEditor

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build (use all CPU cores)
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS

# Test run
./wledit
Debug Build
bashmkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Run with debug symbols
gdb ./wledit
Static Build (Portable Binary)
bashmkdir build-static && cd build-static
cmake -DCMAKE_BUILD_TYPE=Release \
      -DQT_FEATURE_static=ON \
      -DCMAKE_EXE_LINKER_FLAGS=-static \
      ..
make -j$(nproc)

# Result: self-contained binary
ldd ./wledit  # Should show minimal dependencies
Cross-compilation for ARM64
bash# Install cross-compilation tools
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# ARM64 build
mkdir build-arm64 && cd build-arm64
cmake -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
      -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
      -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
      ..
make -j$(nproc)
Raspberry Pi (ARMv7)
bash# Install cross-compilation tools
sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# ARMv7 build
mkdir build-armv7 && cd build-armv7
cmake -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
      -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
      -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=arm \
      ..
make -j$(nproc)
Build Options
CMake Variables
VariableDefaultDescriptionCMAKE_BUILD_TYPEReleaseBuild type (Debug/Release)CMAKE_INSTALL_PREFIX/usr/localInstallation prefixWLEDIT_VERSIONautoOverride version string
Custom Build Example
bashcmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/wledit \
      -DWLEDIT_VERSION="1.0.0-custom" \
      ..
Troubleshooting
Common Issues
Qt6 not found:
bash# Set Qt6 path manually
export CMAKE_PREFIX_PATH=/path/to/qt6
cmake ..
Missing fonts on minimal systems:
bashsudo apt install fonts-noto-cjk
Build fails with older GCC:
bash# Ensure GCC 7+ for C++17 support
gcc --version
sudo apt install gcc-9 g++-9  # if needed
Performance Tips

Use make -j$(nproc) for parallel builds
Use ccache for faster rebuilds: sudo apt install ccache
Use ninja instead of make: sudo apt install ninja-build

bashcmake -GNinja ..
ninja -j$(nproc)
Next Steps
After successful build, see:

Installation Guide - System installation
Android Guide - Mobile development
