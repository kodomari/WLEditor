#!/bin/bash

# Android build script for WLEditor

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🔧 Building WLEditor for Android${NC}"

# Check required environment variables
if [ -z "$ANDROID_HOME" ]; then
    echo -e "${RED}❌ Error: ANDROID_HOME environment variable is not set${NC}"
    exit 1
fi

if [ -z "$ANDROID_NDK" ]; then
    echo -e "${RED}❌ Error: ANDROID_NDK environment variable is not set${NC}"
    exit 1
fi

if [ -z "$Qt6_DIR" ]; then
    echo -e "${RED}❌ Error: Qt6_DIR environment variable is not set${NC}"
    echo -e "${YELLOW}💡 Example: export Qt6_DIR=/path/to/Qt/6.5.0/android_arm64_v8a${NC}"
    exit 1
fi

# Configuration
ARCHITECTURES=("arm64-v8a" "armeabi-v7a")
BUILD_TYPE=${BUILD_TYPE:-Release}
API_LEVEL=${API_LEVEL:-23}

echo -e "${GREEN}📋 Build Configuration:${NC}"
echo -e "  Build Type: ${BUILD_TYPE}"
echo -e "  API Level: ${API_LEVEL}"
echo -e "  Architectures: ${ARCHITECTURES[*]}"
echo -e "  Android NDK: ${ANDROID_NDK}"
echo -e "  Qt6 Directory: ${Qt6_DIR}"

# Create build directory
BUILD_DIR="build-android"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build for each architecture
for arch in "${ARCHITECTURES[@]}"; do
    echo -e "\n${GREEN}🏗️  Building for ${arch}${NC}"
    
    ARCH_BUILD_DIR="${BUILD_DIR}/${arch}"
    mkdir -p "$ARCH_BUILD_DIR"
    cd "$ARCH_BUILD_DIR"
    
    # Configure CMake for Android
    cmake \
        -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" \
        -DANDROID_ABI="${arch}" \
        -DANDROID_PLATFORM="android-${API_LEVEL}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_FIND_ROOT_PATH="${Qt6_DIR}" \
        -DQt6_DIR="${Qt6_DIR}/lib/cmake/Qt6" \
        -DANDROID=ON \
        ../..
    
    # Build
    make -j$(nproc)
    
    echo -e "${GREEN}✅ Successfully built for ${arch}${NC}"
    cd ../..
done

echo -e "\n${GREEN}🎉 Android build completed successfully!${NC}"
echo -e "${YELLOW}📦 Built libraries are in: ${BUILD_DIR}/*/libwledit.so${NC}"

# Create APK (if androiddeployqt is available)
if command -v androiddeployqt6 &> /dev/null; then
    echo -e "\n${GREEN}📱 Creating APK...${NC}"
    
    APK_DIR="${BUILD_DIR}/apk"
    mkdir -p "$APK_DIR"
    
    # Use the first architecture for APK creation
    PRIMARY_ARCH="${ARCHITECTURES[0]}"
    
    androiddeployqt6 \
        --input "${BUILD_DIR}/${PRIMARY_ARCH}/android-wledit-deployment-settings.json" \
        --output "$APK_DIR" \
        --android-platform "android-${API_LEVEL}" \
        --release
    
    echo -e "${GREEN}✅ APK created: ${APK_DIR}/build/outputs/apk/release/wledit-release-unsigned.apk${NC}"
else
    echo -e "${YELLOW}⚠️  androiddeployqt6 not found. APK creation skipped.${NC}"
    echo -e "${YELLOW}💡 Install Qt Android tools to enable APK creation.${NC}"
fi

echo -e "\n${GREEN}🎯 Build Summary:${NC}"
echo -e "  ✅ Native libraries built for: ${ARCHITECTURES[*]}"
echo -e "  📁 Build directory: ${BUILD_DIR}"
if command -v androiddeployqt6 &> /dev/null; then
    echo -e "  📱 APK created in: ${APK_DIR}"
fi

echo -e "\n${GREEN}🚀 Ready for testing and deployment!${NC}"