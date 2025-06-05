# Installing WLEditor

This guide covers installing WLEditor on your system.

## System Installation (Linux)

### Ubuntu/Debian Package Installation

```bash
# After building (from build directory)
sudo make install

# Manual verification
wledit --version
which wledit
Custom Installation Script
WLEditor includes an automated installation script:
bash# From project root
cd desktop
sudo ./install.sh
This script:

Copies binary to /usr/local/bin/wledit
Installs icons for all sizes (16x16 to 512x512)
Sets up desktop integration (.desktop file)
Updates system databases

Manual Installation
bash# Copy binary
sudo cp build/wledit /usr/local/bin/

# Install icons
sudo cp icons/wledit.svg /usr/share/icons/hicolor/scalable/apps/
sudo cp icons/ubuntu/48x48.png /usr/share/pixmaps/wledit.png

# Install desktop file
sudo cp desktop/wledit.desktop /usr/share/applications/

# Update system databases
sudo update-desktop-database
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
Desktop Integration
Application Menu
After installation, WLEditor appears in:

Applications → Office → WordstarLike Editor
Activities → search for "WLEditor" or "WordStar"

File Association
WLEditor registers for these file types:

Plain text (.txt)
Source code (.c, .cpp, .h, .py, .js)
Markdown (.md)

Right-click any text file → Open With → WordstarLike Editor
Command Line Usage
bash# Open files
wledit file1.txt file2.cpp

# New document
wledit

# Help
wledit --help
Portable Installation
Single Binary Deployment
For systems without package management:
bash# Copy just the binary (if statically linked)
cp build/wledit ~/bin/  # or any directory in PATH

# Make executable
chmod +x ~/bin/wledit

# Optional: create desktop shortcut manually
User-only Installation
Install without administrator privileges:
bash# User binary directory
mkdir -p ~/.local/bin
cp build/wledit ~/.local/bin/

# User applications directory
mkdir -p ~/.local/share/applications
cp desktop/wledit.desktop ~/.local/share/applications/

# User icons
mkdir -p ~/.local/share/icons/hicolor/scalable/apps
cp icons/wledit.svg ~/.local/share/icons/hicolor/scalable/apps/

# Update user databases
update-desktop-database ~/.local/share/applications
Configuration
Default Settings
WLEditor stores configuration in:

Linux: ~/.config/WLEditor/
Settings file: wledit.conf

Font Configuration
Default font: Noto Sans Mono CJK JP 12pt
To change defaults, edit source and rebuild, or use the font picker in the application.
Uninstallation
Using Uninstall Script
bashcd desktop
sudo ./uninstall.sh
Manual Removal
bash# Remove binary
sudo rm /usr/local/bin/wledit

# Remove desktop integration
sudo rm /usr/share/applications/wledit.desktop
sudo rm /usr/share/pixmaps/wledit.png
sudo rm /usr/share/icons/hicolor/scalable/apps/wledit.svg

# Remove size-specific icons
for size in 16 32 48 64 128 256 512; do
    sudo rm -f "/usr/share/icons/hicolor/${size}x${size}/apps/wledit.png"
done

# Update databases
sudo update-desktop-database
sudo gtk-update-icon-cache /usr/share/icons/hicolor/

# Remove user configuration (optional)
rm -rf ~/.config/WLEditor/
Troubleshooting
Binary Not Found
bash# Check PATH
echo $PATH
which wledit

# Add to PATH if needed (add to ~/.bashrc)
export PATH=$PATH:/usr/local/bin
Icon Not Showing
bash# Refresh icon cache
sudo gtk-update-icon-cache /usr/share/icons/hicolor/

# Check icon installation
ls /usr/share/icons/hicolor/scalable/apps/wledit.svg
Desktop File Not Working
bash# Validate desktop file
desktop-file-validate /usr/share/applications/wledit.desktop

# Refresh application database
sudo update-desktop-database
Distribution Packages
Future plans include packages for:

Ubuntu PPA
Debian repositories
Fedora COPR
AUR (Arch Linux)
Flatpak
AppImage

Next Steps

Key Map Reference - Learn WordStar key bindings
Building Guide - Build from source
