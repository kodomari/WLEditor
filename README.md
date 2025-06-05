# WLEditor (WordstarLike Editor)

A modern cross-platform text editor with classic WordStar like key bindings.

![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Android%20%7C%20ARM-lightgrey.svg)
![Qt](https://img.shields.io/badge/Qt-6.2%2B-green.svg)

## 🚀 Features

- **Classic WordStar navigation**: Diamond pattern (Ctrl+E/S/D/X)
- **Two-level key bindings**: Ctrl+Q and Ctrl+K command sequences
- **Multi-level clipboard**: 10-item history with easy access
- **Character-based wrapping**: Configurable line width
- **Unicode support**: Full UTF-8 with CJK fonts (Noto Sans Mono CJK JP)
- **Cross-platform**: Linux, Android, ARM support

## 🎯 Design Philosophy

WLEditor brings the efficiency of 1980s WordStar key bindings to modern computing environments. The diamond navigation pattern keeps your hands on the home row, dramatically improving typing efficiency for touch typists.

## ⚡ Quick Start

```bash
# Clone and build
git clone https://github.com/yourusername/WLEditor.git
cd WLEditor
mkdir build && cd build
cmake .. && make -j$(nproc)

# Run
./wledit
📚 Documentation

Building Guide - Detailed build instructions for all platforms
Installation Guide - System installation and desktop integration
Key Map Reference - Complete WordStar key bindings reference
Android Guide - Android development and deployment

🤝 Contributing
Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.
📄 License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.
🙏 Acknowledgments

WordStar - The original diamond navigation concept
Qt Project - Excellent cross-platform framework
Noto Fonts - Beautiful CJK font support
