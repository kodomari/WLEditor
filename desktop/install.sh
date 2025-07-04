#!/bin/bash
# WLEditor システムインストールスクリプト

set -e  # エラーで停止

INSTALL_DIR="/usr/local/bin"
ICON_DIR="/usr/share/icons/hicolor"
PIXMAPS_DIR="/usr/share/pixmaps"
APPS_DIR="/usr/share/applications"

echo "🚀 WLEditor をシステムにインストール中..."

# 実行ファイルの存在確認
if [ ! -f "../build/wordstar-editor" ] && [ ! -f "./wledit" ]; then
    echo "❌ エラー: 実行ファイルが見つかりません"
    echo "   ../build/wordstar-editor または ./wledit が必要です"
    exit 1
fi

# バイナリをインストール
echo "📦 実行ファイルをインストール中..."
if [ -f "../build/wordstar-editor" ]; then
    sudo cp ../build/wordstar-editor "$INSTALL_DIR/wledit"
elif [ -f "./wledit" ]; then
    sudo cp ./wledit "$INSTALL_DIR/"
fi
sudo chmod +x "$INSTALL_DIR/wledit"

# アイコンをインストール
echo "🎨 アイコンをインストール中..."
if [ -f "../wledit.svg" ]; then
    sudo cp ../wledit.svg "$ICON_DIR/scalable/apps/"
fi

if [ -d "../icons/ubuntu" ]; then
    # 各サイズのアイコンをインストール
    for size in 16 32 48 64 128 256 512; do
        if [ -f "../icons/ubuntu/${size}x${size}.png" ]; then
            sudo mkdir -p "$ICON_DIR/${size}x${size}/apps"
            sudo cp "../icons/ubuntu/${size}x${size}.png" "$ICON_DIR/${size}x${size}/apps/wledit.png"
        fi
    done
    
    # pixmapsにも48x48をコピー
    if [ -f "../icons/ubuntu/48x48.png" ]; then
        sudo cp ../icons/ubuntu/48x48.png "$PIXMAPS_DIR/wledit.png"
    fi
fi

# .desktopファイルをインストール
echo "🖥️  デスクトップ統合をセットアップ中..."
sudo cp wledit.desktop "$APPS_DIR/"

# システムデータベースを更新
echo "🔄 システムデータベースを更新中..."
sudo update-desktop-database "$APPS_DIR" 2>/dev/null || true
sudo gtk-update-icon-cache "$ICON_DIR" 2>/dev/null || true

echo "✅ WLEditor のインストール完了！"
echo ""
echo "📍 起動方法:"
echo "   - コマンドライン: wledit"
echo "   - アプリケーションメニュー: WLEditor"
echo "   - ファイルマネージャー: テキストファイルを右クリック → 'プログラムから開く'"
echo ""
echo "🗑️  アンインストール: sudo ./uninstall.sh"
