#!/bin/bash

# Ubuntu build script for WLEditor

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🔧 Building WLEditor for Ubuntu${NC}"

# Check Qt6 installation
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo -e "${RED}❌ Error: Qt6 not found. Please install Qt6 development packages.${NC}"
    echo -e "${YELLOW}💡 Ubuntu: sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential${NC}"
    exit 1
fi

# Configuration
BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_DIR="build-ubuntu"
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}

echo -e "${GREEN}📋 Build Configuration:${NC}"
echo -e "  Build Type: ${BUILD_TYPE}"
echo -e "  Build Directory: ${BUILD_DIR}"
echo -e "  Install Prefix: ${INSTALL_PREFIX}"

# Create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "\n${GREEN}🏗️  Configuring CMake...${NC}"

# Configure CMake for Ubuntu
cmake \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
    -DANDROID=OFF \
    ..

echo -e "\n${GREEN}🔨 Building...${NC}"

# Build
make -j$(nproc)

echo -e "${GREEN}✅ Build completed successfully!${NC}"

# Test the build
if [ -f "./wledit" ]; then
    echo -e "${GREEN}✅ Executable created: ./wledit${NC}"
    
    # Check if it's properly linked
    if ldd ./wledit | grep -q "Qt6"; then
        echo -e "${GREEN}✅ Qt6 libraries properly linked${NC}"
    else
        echo -e "${YELLOW}⚠️  Warning: Qt6 libraries may not be properly linked${NC}"
    fi
else
    echo -e "${RED}❌ Error: Executable not found${NC}"
    exit 1
fi

echo -e "\n${GREEN}🎯 Build Summary:${NC}"
echo -e "  ✅ Executable: ${BUILD_DIR}/wledit"
echo -e "  🔧 Build type: ${BUILD_TYPE}"
echo -e "  📁 Build directory: ${BUILD_DIR}"

# Optional: Create AppImage
if command -v appimagetool &> /dev/null; then
    echo -e "\n${GREEN}📦 Creating AppImage...${NC}"
    
    APPDIR="${BUILD_DIR}/WLEditor.AppDir"
    mkdir -p "$APPDIR/usr/bin"
    mkdir -p "$APPDIR/usr/share/applications"
    mkdir -p "$APPDIR/usr/share/icons/hicolor/128x128/apps"
    
    # Copy executable
    cp wledit "$APPDIR/usr/bin/"
    
    # Copy desktop file
    cp ../desktop/wledit.desktop "$APPDIR/usr/share/applications/"
    
    # Copy icon
    cp ../icons/ubuntu/128x128.png "$APPDIR/usr/share/icons/hicolor/128x128/apps/wledit.png"
    
    # Create AppRun
    cat > "$APPDIR/AppRun" << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
exec "${HERE}/usr/bin/wledit" "$@"
EOF
    chmod +x "$APPDIR/AppRun"
    
    # Create AppImage
    appimagetool "$APPDIR" WLEditor-x86_64.AppImage
    
    echo -e "${GREEN}✅ AppImage created: ${BUILD_DIR}/WLEditor-x86_64.AppImage${NC}"
else
    echo -e "${YELLOW}💡 Install appimagetool to create AppImage packages${NC}"
fi

echo -e "\n${GREEN}🚀 Ready for testing!${NC}"
echo -e "${YELLOW}💡 Run: ./${BUILD_DIR}/wledit${NC}"

cd ..