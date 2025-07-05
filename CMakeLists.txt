cmake_minimum_required(VERSION 3.16)

project(WLEditor VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6を検索 (Android以外)
if(NOT ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
endif()

# Qt MOCを有効化
set(CMAKE_AUTOMOC ON)

# ソースファイル
if(ANDROID)
    set(SOURCES
        src/android_main.cpp
    )
    set(HEADERS)
else()
    set(SOURCES
        src/main.cpp
        src/MainWindow.cpp
    )
    set(HEADERS
        src/MainWindow.h
    )
endif()

# プラットフォーム固有の設定
if(ANDROID)
    # Android固有の設定
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    
    # Android用の共有ライブラリを作成（Qt不使用）
    add_library(wledit SHARED ${SOURCES} ${HEADERS})
    
    # Android用ライブラリをリンク
    find_library(log-lib log)
    find_library(android-lib android)
    target_link_libraries(wledit ${log-lib} ${android-lib})
    target_compile_definitions(wledit PRIVATE ANDROID_BUILD)
    
    # Android用リソースは不要（Gradleが処理）
    
else()
    # Ubuntu/Linux用の実行可能ファイルを作成
    add_executable(wledit ${SOURCES} ${HEADERS})
    
    # Qtライブラリをリンク
    target_link_libraries(wledit Qt6::Core Qt6::Widgets)
    
    # インストール設定
    install(TARGETS wledit DESTINATION bin)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/desktop/wledit.desktop")
        install(FILES desktop/wledit.desktop DESTINATION share/applications)
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons/ubuntu/128x128.png")
        install(FILES icons/ubuntu/128x128.png DESTINATION share/icons/hicolor/128x128/apps RENAME wledit.png)
        install(FILES icons/ubuntu/64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME wledit.png)
        install(FILES icons/ubuntu/48x48.png DESTINATION share/icons/hicolor/48x48/apps RENAME wledit.png)
        install(FILES icons/ubuntu/32x32.png DESTINATION share/icons/hicolor/32x32/apps RENAME wledit.png)
        install(FILES icons/ubuntu/16x16.png DESTINATION share/icons/hicolor/16x16/apps RENAME wledit.png)
    endif()
endif()