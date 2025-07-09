#!/bin/bash
set -e

echo "Uninstalling WLEditor..."

# システムファイル削除
sudo rm -f /usr/local/bin/wleditor
sudo rm -f /usr/local/share/icons/wledit.png
sudo rm -f /usr/local/share/icons/wledit.svg
sudo rm -f /usr/share/applications/wledit.desktop

# デスクトップデータベース更新
sudo update-desktop-database

echo "✅ WLEditor uninstalled successfully!"
