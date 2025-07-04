# WLEditor (WordstarLike Editor)

A modern cross-platform text editor with classic WordStar like key bindings.

![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Android%20%7C%20ARM-lightgrey.svg)
![Qt](https://img.shields.io/badge/Qt-6.5%2B-green.svg)
![Build Status](https://github.com/kodomari/WLEditor/actions/workflows/build.yml/badge.svg)

## 🚀 Features

- **Classic WordStar navigation**: Diamond pattern (Ctrl+E/S/D/X)
- **Two-level key bindings**: Ctrl+Q and Ctrl+K command sequences
- **Multi-level clipboard**: 10-item history with easy access
- **Character-based wrapping**: Configurable line width
- **Unicode support**: Full UTF-8 with CJK fonts (Noto Sans Mono CJK JP)
- **Cross-platform**: Linux, Android, ARM support
- **Touch-optimized**: Android version with touch-friendly interface

## 🎯 Design Philosophy

WLEditor brings the efficiency of 1980s WordStar key bindings to modern computing environments. The diamond navigation pattern keeps your hands on the home row, dramatically improving typing efficiency for touch typists.

## ⚡ Quick Start

### Ubuntu/Linux
```bash
# Clone and build
git clone https://github.com/kodomari/WLEditor.git
cd WLEditor
./scripts/build-ubuntu.sh

# Run
./build-ubuntu/wledit
```

### Android
```bash
# Prerequisites: Android SDK, NDK, Qt6 for Android
git clone https://github.com/kodomari/WLEditor.git
cd WLEditor
./scripts/build-android.sh

# Install APK
adb install build-android/apk/build/outputs/apk/release/wledit-release-unsigned.apk
```

## 📱 Platform Support

| Platform | Status | Download |
|----------|--------|----------|
| Ubuntu 20.04+ | ✅ | [AppImage](https://github.com/kodomari/WLEditor/releases) |
| Android 6.0+ | ✅ | [APK](https://github.com/kodomari/WLEditor/releases) |
| ARM Linux | ✅ | Build from source |

## 🛠️ Building

### Prerequisites
- Qt 6.5 or later
- CMake 3.16+
- C++17 compiler

### Ubuntu Dependencies
```bash
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

### Android Dependencies
- Android SDK (API 23+)
- Android NDK (r21+)
- Java JDK (8 or 11)
- Qt for Android

See [docs/BUILDING.md](docs/BUILDING.md) for detailed build instructions.

## 🔧 Development

### Project Structure
```
WLEditor/
├── src/              # Source code
├── android/          # Android-specific files
├── icons/            # Application icons
├── desktop/          # Linux desktop integration
├── docs/             # Documentation
├── scripts/          # Build scripts
└── .github/          # CI/CD workflows
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## 📚 Documentation

- [Building Guide](docs/BUILDING.md) - Detailed build instructions for all platforms
- [Installation Guide](docs/INSTALL.md) - System installation and desktop integration
- [Key Map Reference](docs/KEYMAP.md) - Complete WordStar key bindings reference
- [Android Guide](docs/ANDROID.md) - Android development and deployment

## 🎹 Key Bindings

### Diamond Navigation (Keep hands on home row)
- **Ctrl+E**: Up
- **Ctrl+S**: Left
- **Ctrl+D**: Right
- **Ctrl+X**: Down

### Two-Level Commands
- **Ctrl+Q**: File operations prefix
- **Ctrl+K**: Block operations prefix

### Multi-Level Clipboard
- **Ctrl+C**: Copy to clipboard history
- **Ctrl+V**: Paste from clipboard history
- **Ctrl+Shift+V**: Show clipboard history

## 🔄 Continuous Integration

The project uses GitHub Actions for automated building and testing:
- Ubuntu builds with AppImage generation
- Android builds for multiple API levels
- Automated testing and security scanning
- Documentation generation

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **WordStar** - The original diamond navigation concept
- **Qt Project** - Excellent cross-platform framework
- **Noto Fonts** - Beautiful CJK font support
- **Contributors** - Thanks to all who help improve WLEditor

---

*Made with ❤️ for productivity and efficiency*
