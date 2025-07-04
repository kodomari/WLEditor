cmake_minimum_required(VERSION 3.16)

project(WLEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6を検索
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Qt MOCを有効化
set(CMAKE_AUTOMOC ON)

# ソースファイル
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
)

set(HEADERS
    src/MainWindow.h
)

# プラットフォーム固有の設定
if(ANDROID)
    # Android固有の設定
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    
    # Android用の実行可能ファイルを作成
    add_library(wledit SHARED ${SOURCES} ${HEADERS})
    
    # Android用ライブラリをリンク
    find_library(log-lib log)
    target_link_libraries(wledit Qt6::Core Qt6::Widgets ${log-lib})
    
    # Android用リソースを追加
    qt_add_resources(wledit "android_resources"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/android"
        FILES
            android/res/values/strings.xml
            android/AndroidManifest.xml
    )
    
    # Android用アイコンをコピー
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icons/android/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/android/res/)
    
else()
    # Ubuntu/Linux用の実行可能ファイルを作成
    add_executable(wledit ${SOURCES} ${HEADERS})
    
    # Qtライブラリをリンク
    target_link_libraries(wledit Qt6::Core Qt6::Widgets)
    
    # インストール設定
    install(TARGETS wledit DESTINATION bin)
    install(FILES desktop/wledit.desktop DESTINATION share/applications)
    install(FILES icons/ubuntu/128x128.png DESTINATION share/icons/hicolor/128x128/apps RENAME wledit.png)
    install(FILES icons/ubuntu/64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME wledit.png)
    install(FILES icons/ubuntu/48x48.png DESTINATION share/icons/hicolor/48x48/apps RENAME wledit.png)
    install(FILES icons/ubuntu/32x32.png DESTINATION share/icons/hicolor/32x32/apps RENAME wledit.png)
    install(FILES icons/ubuntu/16x16.png DESTINATION share/icons/hicolor/16x16/apps RENAME wledit.png)
endif()