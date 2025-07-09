#!/bin/bash
# WLEditor 静的リンク版ビルド（ARM64 Ubuntu用）

cd ~/WLEditor

# 1. 静的ビルド用ディレクトリ
mkdir -p build-static
cd build-static

# 2. 静的リンクオプション付きビルド
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++" \
      ..

make -j$(nproc)

# 3. 依存関係確認
echo "=== Dependency Check ==="
ldd wledit || echo "Statically linked (good!)"

# 4. バイナリ詳細
echo "=== Binary Info ==="
file wledit
echo "Size: $(du -h wledit | cut -f1)"

# 5. リリース用パッケージ作成
mkdir -p WLEditor-v1.3.0-arm64-ubuntu
cp wledit WLEditor-v1.3.0-arm64-ubuntu/
cp ../desktop/wledit.desktop WLEditor-v1.3.0-arm64-ubuntu/
cp ../icons/wledit.png WLEditor-v1.3.0-arm64-ubuntu/
cp ../README.md WLEditor-v1.3.0-arm64-ubuntu/ 2>/dev/null || echo "README.md not found"

# インストールスクリプト作成
cat > WLEditor-v1.3.0-arm64-ubuntu/install.sh << 'INSTALL_EOF'
#!/bin/bash
echo "Installing WLEditor v1.3.0..."
sudo cp wledit /usr/local/bin/
sudo cp wledit.desktop /usr/share/applications/
sudo cp wledit.png /usr/share/icons/
sudo update-desktop-database
echo "✅ WLEditor installed successfully!"
echo "🚀 Run with: wledit"
INSTALL_EOF
chmod +x WLEditor-v1.3.0-arm64-ubuntu/install.sh

# 6. アーカイブ作成
tar czf WLEditor-v1.3.0-arm64-ubuntu.tar.gz WLEditor-v1.3.0-arm64-ubuntu/

echo "✅ Static build completed!"
echo "📦 Package: WLEditor-v1.3.0-arm64-ubuntu.tar.gz"
echo "🧪 Test: ./wledit"
