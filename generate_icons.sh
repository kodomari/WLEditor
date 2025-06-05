#!/bin/bash
# WLEDITアイコン一括生成スクリプト

echo "WLEDITアイコンを生成中..."

# SVGファイルの存在確認
if [ ! -f "wledit.svg" ]; then
    echo "エラー: wledit.svg が見つかりません"
    exit 1
fi

# Inkscapeの存在確認
if ! command -v inkscape &> /dev/null; then
    echo "エラー: Inkscapeがインストールされていません"
    echo "sudo apt install inkscape でインストールしてください"
    exit 1
fi

# ディレクトリ作成
echo "ディレクトリを作成中..."
mkdir -p ubuntu
mkdir -p android/mipmap-{mdpi,hdpi,xhdpi,xxhdpi,xxxhdpi}

# Ubuntu用アイコン生成
echo "Ubuntu用アイコンを生成中..."
sizes=(16 32 48 64 128 256 512)
for size in "${sizes[@]}"; do
    echo "  ${size}x${size}.png"
    inkscape --export-type=png \
        --export-filename="ubuntu/${size}x${size}.png" \
        --export-width=$size \
        --export-height=$size \
        wledit.svg 2>/dev/null
done

# Android用アイコン生成
echo "Android用アイコンを生成中..."
declare -A android_sizes=(
    ["mipmap-mdpi"]=48
    ["mipmap-hdpi"]=72
    ["mipmap-xhdpi"]=96
    ["mipmap-xxhdpi"]=144
    ["mipmap-xxxhdpi"]=192
)

for dir in "${!android_sizes[@]}"; do
    size=${android_sizes[$dir]}
    echo "  ${dir}/ic_launcher.png (${size}x${size})"
    inkscape --export-type=png \
        --export-filename="android/${dir}/ic_launcher.png" \
        --export-width=$size \
        --export-height=$size \
        wledit.svg 2>/dev/null
done

echo "✅ アイコン生成完了！"
echo "📁 生成されたファイル:"
echo "   Ubuntu用: icons/ubuntu/"
echo "   Android用: icons/android/"
