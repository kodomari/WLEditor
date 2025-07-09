#!/bin/bash
echo "Installing WLEditor v1.3.0..."
sudo cp wledit /usr/local/bin/
sudo cp wledit.png /usr/share/icons/

# 正しいデスクトップファイルを作成
sudo tee /usr/share/applications/wledit.desktop > /dev/null << 'DESKTOP_EOF'
[Desktop Entry]
Version=1.0
Type=Application
Name=WLEditor
Comment=WordStar-like Text Editor
GenericName=Text Editor
Exec=/usr/local/bin/wledit %F
Icon=wledit
Terminal=false
StartupNotify=true
StartupWMClass=WLEditor
Categories=TextEditor;
Keywords=editor;text;wordstar;programming;
MimeType=text/plain;text/x-c;text/x-c++;text/x-c++src;text/x-csrc;text/x-chdr;text/x-python;text/x-java;text/x-makefile;text/x-script;application/x-shellscript;text/x-cmake;text/x-qml;
DESKTOP_EOF

sudo update-desktop-database
echo "✅ WLEditor installed successfully!"
echo "🚀 Run with: wledit"
