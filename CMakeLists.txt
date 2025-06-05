cmake_minimum_required(VERSION 3.16)

project(WordStarEditor VERSION 1.0.0 LANGUAGES CXX)

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

# 実行可能ファイルを作成
add_executable(wordstar-editor ${SOURCES} ${HEADERS})

# Qtライブラリをリンク
target_link_libraries(wordstar-editor Qt6::Core Qt6::Widgets)